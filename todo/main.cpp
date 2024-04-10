
#include "main.h"

#include <Brawl/FT/ftOwner.h>

bool initialized = false;

// responsible for pausing exeuction of the game itself while the code menu is up
INJECTION("TOGGLE_PAUSE", 0x8002E5B0, R"(
    mr r3, r25
    bl checkMenuPaused 
    lwz r3, 0 (r25)
)");

extern "C" void checkMenuPaused(char* gfTaskSchedulerInst) {
    // OSReport("Visible: %s, paused: %s\n", visible ? "T" : "F", paused ? "T" : "F");
    if (punchMenu.paused && punchMenu.visible) { gfTaskSchedulerInst[0xB] |= 0x8; }
    else { gfTaskSchedulerInst[0xB] &= ~0x8; }
}

void printRABools(const soWorkManageModuleImpl& workModule) {
    auto RABoolArr = (*(u32 (*)[workModule.RAVariables->bitsSize])workModule.RAVariables->bitVariables);
    u32 boolNum;
    u32 chunk;
    char boolVal;

    OSReport("RA bools (%x): ", RABoolArr);
    for (int i = 0; i < workModule.RAVariables->bitsSize; i++) {
        chunk = RABoolArr[i];
        for (int j = 0; j < (sizeof(boolNum)*8 - 1); j++) {
            boolNum = (i*sizeof(boolNum)) + (sizeof(boolNum)-j);
            boolVal = (chunk & (0x1 << j)) >> j;
            OSReport("%d", boolVal);

            if ((j % 8) == 7) {
                OSReport(" ");
            }
        }
        OSReport("  |  ");
    }
    OSReport("\n");
}

bool getRABit(const soWorkManageModuleImpl& workModule, u32 idx) {
    auto RABitsCnt = workModule.RAVariables->bitsSize;
    auto RABitsAry = (*(u32 (*)[RABitsCnt])workModule.RAVariables->bitVariables);

    // printRABools(workModule);

    if (!(idx < RABitsCnt*8*4)) {
        OSReport("Warning: asked for invalid RA bit %d from workModule %x.\n", idx, (void*)&workModule);
        return false;
    }

    u32 bitsChunk = RABitsAry[idx / 32];
    char remainder = idx % 32;
    if ((bitsChunk & (1 << remainder) >> remainder) == 0) {
        return false;
    } else {
        return true;
    }
}

void breakpoint() {
    OSReport("hello.\n");
}

void printMessage(char const* msg, float xPos, float yPos, GXColor color = COLOR_WHITE){
    OSReport("%s\n", msg);
    printer.setup();
    printer.setTextColor(color);
    printer.renderPre = true;
    Message* printerMsgObj = &(printer.message);
    printer.lineHeight = punchMenu.lineHeight();
    printerMsgObj->fontScaleY = punchMenu.baseFontScale.y;
    printerMsgObj->fontScaleX = punchMenu.baseFontScale.x;
    printerMsgObj->xPos = xPos;
    printerMsgObj->yPos = yPos;
    printerMsgObj->zPos = 0;
    printer.start2D();

    printer.startBoundingBox();
    printer.print(msg);
    printer.saveBoundingBox(printer.bboxIdx, COLOR_TRANSPARENT_GREY, 10);
}

void printFighterState(PlayerData& playerData) {
    playerData.debugStr(strManipBuffer);
    printer.setup();
    printer.renderPre = true;
    printer.setTextColor(0xFFFFFFFF);
    auto& msg = printer.message;
    printer.lineHeight = punchMenu.lineHeight();
    msg.fontScaleY = punchMenu.baseFontScale.y * punchMenu.fontScaleMultiplier;
    msg.fontScaleX = punchMenu.baseFontScale.x * punchMenu.fontScaleMultiplier;
    msg.edgeWidth = 1.0f;
    msg.edgeColor = 0x000000FF;
    msg.xPos = 50;
    msg.yPos = 50;
    printer.start2D();

    printer.startBoundingBox();
    printer.print(strManipBuffer);
    printer.saveBoundingBox(printer.bboxIdx, 0, 10);
}

inline bool needsInitializing() {
    if (initialized) return false;
    auto fighters = FIGHTER_MANAGER->getEntryCount();
    for (char i =  0; i < fighters; i++) {
        auto entryId = FIGHTER_MANAGER->getEntryIdFromIndex(i);
        auto opStatus = FIGHTER_MANAGER->getFighterOperationStatus(entryId);
        #ifdef PP_INIT_DEBUG
        OSReport("Fighter %d status %d\n", entryId, opStatus);
        #endif
        if (opStatus == 0) { 
            return false;
        }
    }

    return true;
}

// calls our function
INJECTION("update_pre_frame", /*0x8001792c*/ 0x800177B0, R"(
    SAVE_REGS
    bl updatePreFrame
    RESTORE_REGS
    or r0, r0, r3
)");
extern "C" void updatePreFrame() {
    SCENE_TYPE sceneType = (SCENE_TYPE)getScene();

    if (sceneType == SCENE_TYPE::VS || sceneType == SCENE_TYPE::TRAINING_MODE_MMS) {
        frameCounter += 1;
        renderables.renderPre();
        renderables.clearAll();
        if (!initialized && !needsInitializing()) {
            #ifdef PP_INIT_DEBUG
            OSReport("Bailing out to start early\n");
            #endif
            startNormalDraw();
            return;
        }

        int fighterCount;
        fighterCount = FIGHTER_MANAGER->getEntryCount();
        u8 idx = 0;
        for (idx = 0; idx < fighterCount; idx++) {
            gatherData(idx);
        }

        if (!punchMenu.visible) {
            for (idx = 0; idx < fighterCount; idx++) {
                if (allPlayerData[idx].showFighterState) {
                    printFighterState(allPlayerData[idx]);
                    break;
                }
            }
        }

        for(idx = 0; idx < fighterCount; idx++) {
            auto& playerData = allPlayerData[idx];

            if (playerData.didActionChange() && isAttackingAction(playerData.action())) {
                playerData.didStartAttack = true;
            }

            if (playerData.didStartAttack) {
                playerData.resolvePlayerActionable();
            }
        }

        for (idx = 0; idx < fighterCount; idx++) {
            resolveAttackTarget(idx);
        }

        for (idx = 0; idx < fighterCount; idx++) {
            checkAttackTargetActionable(idx);
        }

        if (FIGHTER_MANAGER->getEntryCount() > 0) {
            if (initialized) {
                punchMenu.handleInput();
                punchMenu.render(printer, strManipBuffer, PP_STR_MANIP_SIZE);
            } else if (needsInitializing()) {
                initialized = true;
                punchMenu.init();

                PADButtons pad;
                pad.bits = (
                    PREVIOUS_PADS[0].button.bits 
                    | PREVIOUS_PADS[1].button.bits 
                    | PREVIOUS_PADS[2].button.bits 
                    | PREVIOUS_PADS[3].button.bits
                );
                if (!(pad.L == true || pad.R == true)) {
                    punchMenu.toggle();
                }
            }
        }

        drawAllPopups();
    } else {
        if (initialized) {
            initialized = false;
            punchMenu.cleanup();

            delete[] allPlayerData;
            allPlayerData = new PlayerData[PP_MAX_PLAYERS];
        }
    }


    startNormalDraw();
}

void debugWorkModule(const soWorkManageModuleImpl& workModule) {
    u32 i;
    auto RABasicsArr = (*(int(*)[workModule.RAVariables->basicsSize])workModule.RAVariables->basics);
    auto RAFloatArr = (*(int(*)[workModule.RAVariables->floatsSize])workModule.RAVariables->floats);
    auto LABasicsArr = (*(int(*)[workModule.LAVariables->basicsSize])workModule.LAVariables->basics);
    auto LAFloatArr = (*(float(*)[workModule.LAVariables->floatsSize])workModule.LAVariables->floats);

    for(i = 0; i < workModule.RAVariables->basicsSize; i++) {
        if (RABasicsArr[i] != 0 && RABasicsArr[i] != -1){
            OSReport("\tRABasic #%d: %d \n", i, RABasicsArr[i]);
        }
    }

    for (i = 0; i < workModule.RAVariables->floatsSize; i++) {
        if (RAFloatArr[i] != 0 && RAFloatArr[i] != -1) {
            OSReport("\tRAFloat #%d: %0.2f\n", i, RAFloatArr[i]);
        }
    }

    for(i = 0; i < workModule.LAVariables->basicsSize; i++) {
        if (LABasicsArr[i] != 0 && LABasicsArr[i] != -1) {
            OSReport("\tLABasic #%d: %d \n", i, LABasicsArr[i]);
        }
    }

    for (i = 0; i < workModule.LAVariables->floatsSize; i++) {
        if (LAFloatArr[i] != 0 && LAFloatArr[i] != -1) {
            OSReport("\tLAFloat #%d: %0.2f\n", i, LAFloatArr[i]);
        }
    }

    return;
}


void gatherData(u8 player) {
    if (player > 3) {
        OSReport("Asked to gather data for invalid player %d\n", player);
        return;
    }

    EntryID entryId = FIGHTER_MANAGER->getEntryIdFromIndex(player);
    Fighter* fighter = FIGHTER_MANAGER->getFighter(entryId, 0);
    u8 playerNumber = FIGHTER_MANAGER->getPlayerNo(entryId);

    if (needsInitializing()) {
        auto* ftInput = FIGHTER_MANAGER->getInput(entryId);
        int opType = FIGHTER_MANAGER->getFighterOperationType(entryId);
        OSReport("Player %d op type: %d input: 0x%x\n", playerNumber, opType, ftInput);
        if (opType != 0) {
            allPlayerData[playerNumber].showOnShieldAdvantage = false;
            allPlayerData[playerNumber].showOnHitAdvantage = false;
        } else {
            allPlayerData[playerNumber].showOnShieldAdvantage = true;
            allPlayerData[playerNumber].showOnHitAdvantage = false;
        };

    }

    PlayerData& playerData = allPlayerData[playerNumber];
    playerData.prepareNextFrame();
    PlayerDataOnFrame& currentData = *playerData.current;
    PlayerDataOnFrame& prevData = *playerData.current;
    playerData.charId = (CHAR_ID)(fighter->getFtKind());

    allPlayerData[player].playerNumber = playerNumber;

    auto workModule = fighter->modules->workModule;
    auto statusModule = fighter->modules->statusModule;
    auto motionModule = fighter->modules->motionModule;
    auto stopModule = fighter->modules->ftStopModule;
    auto cancelModule = fighter->getCancelModule();

    /*
    OSReport(
        "Player: %d\n"
        "  char type: 0x%X\n" 
        "  entryId: 0x%X\n"
        "  fighter addr: 0x%X\n"
        ,
        player, character, entryId, fighter
    );
    */

    // OSReport("Module Locations:\n\tworkModule: %x\n\tstatusModule: %x\n\tmotionModule: %x\n", workModule, statusModule, motionModule);

    if (statusModule != nullptr) {
        /* OSReport("Action number: %x\n", statusModule->action); */
        // never seems to work. strcpy(playerData.current->actionname, statusModule->getStatusName(), PP_ACTION_NAME_LEN);
        playerData.current->action = statusModule->action;
        if (statusModule->attackHasConnected) {
            playerData.didConnectAttack = true;
        }
    }

    /* hitstun/shieldstun stuff comes from the work module. */
    if (workModule != nullptr) {
        // OSReport("Work module vals for player %d:\n", player);
        // debugWorkModule(*workModule);
        auto RABasicsArr = (*(int(*)[workModule->RAVariables->basicsSize])workModule->RAVariables->basics);
        auto RAFloatArr = (*(int(*)[workModule->RAVariables->floatsSize])workModule->RAVariables->floats);
        auto RABoolArr = (*(u32 (*)[workModule->RAVariables->bitsSize])workModule->RAVariables->bitVariables);
        auto LABasicsArr = (*(int(*)[workModule->LAVariables->basicsSize])workModule->LAVariables->basics);
        auto LAFloatArr = (*(float(*)[workModule->LAVariables->floatsSize])workModule->LAVariables->floats);
        auto LABoolArr = (*(u32 (*)[workModule->LAVariables->bitsSize])workModule->LAVariables->bitVariables);

        // float shieldValue = LAFloatArr[0x3];

        if (currentData.action == ACTION_GUARDDAMAGE) {
            // This also tracks the lag on shield release frames, which
            // we don't care about for actionability. So we don't want to get this
            // during GuardOff, for example.
            currentData.shieldstun = RABasicsArr[0x5];
        }
        if (currentData.shieldstun != 0) {
            if (playerData.didReceiveShieldstun()) {
                playerData.maxShieldstun = currentData.shieldstun;
                playerData.becameActionableOnFrame = -1;
                playerData.advantageBonusCounter = 0;
            }

            /*
            OSReport("Player %d shieldstun: %d/%d\n",
                playerData.playerNumber, currentData.shieldstun, playerData.maxShieldstun
            );
            */
        } else {
            playerData.maxShieldstun = 0;
        }

        int remainingHitstun = LABasicsArr[0x38];
        playerData.current->hitstun = remainingHitstun;

        if (remainingHitstun != 0) {
            if (playerData.didReceiveHitstun()) {
                playerData.maxHitstun = remainingHitstun;
                playerData.becameActionableOnFrame = -1;
                playerData.advantageBonusCounter = 0;
            }
        } else {
            playerData.maxHitstun = 0;
        }

        if (playerData.playerNumber == 0) {
            //OSReport("[Action 0x%x %s] ", currentData.action, actionName(currentData.action));
            // printRABools(*workModule);
        }
        currentData.lowRABits = RABoolArr[0];
    }


    /* subaction stuff */
    if (motionModule != nullptr) {
        soAnimChr animationData = motionModule->mainAnimationData;
        playerData.current->subaction = motionModule->subAction;
        if (animationData.resPtr != nullptr) {
            auto animationResource = animationData.resPtr->CHR0Ptr;
            // OSReport("Animation Resource: 0x%X\n", animationResource);
            if (animationResource == nullptr) {
                strncpy(playerData.current->subactionName, "UNKNOWN", PP_ACTION_NAME_LEN);
                playerData.current->actionTotalFrames = -1;
            } else {
                playerData.current->subactionFrame = motionModule->getFrame();

                // do these ever differ, except by 1?
                playerData.current->subactionTotalFrames = motionModule->getEndFrame();
                playerData.current->actionTotalFrames = animationResource->animLength;

                strncpy(playerData.current->subactionName, animationResource->getString(), PP_ACTION_NAME_LEN);
            }

            playerData.current->actionFrame = (u32)animationData.animFrame;
        }

    }
    if (cancelModule != nullptr) {
        u32 isEnableCancel = cancelModule->isEnableCancel();
        // TODO: investigate cancel groups here.
        currentData.canCancel = (bool)isEnableCancel;
    }
}

void resolveAttackTarget(u8 playerIdx) {
    PlayerData& player = allPlayerData[playerIdx];
    // False most of the time, so this isn't as slow as it looks.
    if (player.didConnectAttack != false) {
        for (char otherIdx = 0; otherIdx < PP_MAX_PLAYERS; otherIdx++) {
            if (playerIdx == otherIdx) {
                continue;
            }

            auto& otherPlayer = allPlayerData[otherIdx];
            if (player.showOnHitAdvantage && otherPlayer.didReceiveHitstun()) {
                player.resetTargeting();
                otherPlayer.resetTargeting();

                OSReport("Setting on-hit Attacker %d -> Defender %d\n", player.playerNumber, otherPlayer.playerNumber);
                player.attackTarget = &(otherPlayer);
                player.attackingAction = player.current->action;
                player.isAttackingFighter = true;
                break;
            } else if (player.showOnShieldAdvantage && otherPlayer.didReceiveShieldstun()) {
                player.resetTargeting();
                otherPlayer.resetTargeting();

                OSReport("Setting on-shield Attacker %d -> Defender %d\n", player.playerNumber, otherPlayer.playerNumber);
                player.attackTarget = &(otherPlayer);
                player.attackingAction = player.current->action;
                player.isAttackingShield = true;
                break;

            }
        }
    }
}

void checkAttackTargetActionable(u8 playerNum) {
    PlayerData& player = allPlayerData[playerNum];

    // Player is attacking someone.
    if (player.attackTarget != nullptr){
        PlayerData& target = *(player.attackTarget);

        bool targetIsActionable = target.resolveTargetActionable();
        bool playerIsActionable = player.resolvePlayerActionable();


        if (playerIsActionable && targetIsActionable) {
            /* POP UP TIME! */
            short int advantage = target.becameActionableOnFrame - player.becameActionableOnFrame;

            /* Lots of weird edge cases where the ending doesn't register, such as dying or teching. */
            /* > 30 frames is generally judge-able with a human eye anyway. */
            if (advantage > -30 && advantage < 30) {
                if (player.showOnShieldAdvantage) {
                    OSReport("Displaying popup for attacker: %d\n", player.playerNumber);
                    snprintf(strManipBuffer, PP_STR_MANIP_SIZE, "Advantage: %d\n", advantage);
                    OSReport(strManipBuffer);
                    Popup& popup = *(new Popup(strManipBuffer));
                    popup.coords = getHpPopupBoxCoords(player.playerNumber);
                    popup.durationSecs = 3;
                    playerPopups[player.playerNumber].append(popup);
                }
            }

            player.resetTargeting();
            target.resetTargeting();
        }
    }
}


//hacky way to check if in game
SCENE_TYPE getScene() {
    u32* ptr = (u32*) (0x805b4fd8 + 0xd4);
    ptr = (u32*) *ptr;
    if(ptr < (u32*)0xA0000000) {
        ptr = (u32*) *(ptr + (0x10 / 4));
        if(ptr != nullptr) {
            u32 scene = *(ptr + (8 / 4));
            return (SCENE_TYPE)scene;
        }
    }
    return SCENE_TYPE::UNKNOWN;
}

#define P1_2P_COORDS Coord2D{.x = 200, .y = 350}
#define P2_2P_COORDS Coord2D{.x = 355, .y = 350}
#define P1_4P_COORDS Coord2D{.x = 50, .y = 350}
#define P2_4P_COORDS Coord2D{.x = 200, .y = 350}
#define P3_4P_COORDS Coord2D{.x = 350, .y = 350}
#define P4_4P_COORDS Coord2D{.x = 500, .y = 350}
Coord2D getHpPopupBoxCoords(int playerNum) {
    SCENE_TYPE scene = getScene();
    char totalPlayers;

    // I don't know why training mode has the player numbers
    // backwards. :(
    if (scene == TRAINING_MODE_MMS) {
        totalPlayers = 4;
        switch(playerNum){
        case 0:
            return P2_4P_COORDS;
        case 1:
            return P1_4P_COORDS;
        default:
            return Coord2D{};
        }
    }

    totalPlayers = FIGHTER_MANAGER->getEntryCount();

    if (totalPlayers == 2) {
        switch(playerNum) {
            case 0:
                return P1_2P_COORDS;
            case 1:
                return P2_2P_COORDS;
            default:
                return Coord2D{};
        }
    }
    if (totalPlayers == 4) {
        switch(playerNum) {
            case 0:
                return P1_4P_COORDS;
            case 1:
                return P2_4P_COORDS;
            case 2:
                return P3_4P_COORDS;
            case 3:
                return P4_4P_COORDS;
            default:
                return Coord2D {};
        }
    }

    // TODO: Other numbers of players.

    return Coord2D{};
}

void drawAllPopups() {
    for(int i = 0; i < PP_MAX_PLAYERS; i++) {
        auto itr = LinkedlistIterator(playerPopups[i]);
        Popup* popup;
        Coord2D coords = getHpPopupBoxCoords(i);


        while ((popup = itr.next()) != nullptr) {
            if (popup->expired()) {
                itr.deleteHere();
                delete popup;
            } else {
                popup->coords = coords;
                // OSReport("Set popup coords to %d,%d\n", coords.x, coords.y);
                popup->draw(printer);

                coords.y -= PP_POPUP_VERTICAL_OFFSET;
            }
        }
    }
}
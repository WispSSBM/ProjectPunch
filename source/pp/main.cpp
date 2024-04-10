#include <cstring>
#include <so/work/so_general_work_abstract.h>
#include <so/work/so_work_manage_module_impl.h>
#include <ft/fighter.h>
#include <ft/ft_manager.h>
#include <gf/gf_scene.h>
#include <gf/gf_heap_manager.h>

#include "pp/main.h"
#include "pp/ui.h"
#include "pp/collections/linkedlist.h"
#include "pp/graphics/draw.h"
#include "pp/input/pad.h"
#include "pp/ft_cancel_module.h"

using namespace ProjectPunch::Graphics;
using namespace ProjectPunch::Input;
using namespace ProjectPunch::Collections;

namespace ProjectPunch {
bool initialized = false;


void checkMenuPaused(char* gfTaskSchedulerInst) {
    // OSReport("Visible: %s, paused: %s\n", visible ? "T" : "F", paused ? "T" : "F");
    //OSReport("Trying to read gfTaskSchedulerInst @ 0x%0X\n", gfTaskSchedulerInst);
    if (punchMenu.paused && punchMenu.visible) { gfTaskSchedulerInst[0xB] |= 0x8; }
    else { gfTaskSchedulerInst[0xB] &= ~0x8; }
}

soGeneralWorkSimple* getWorkVars(const Fighter& fighter, WorkModuleVarType varType) {
    // assume the work manage module is of the normal impl type. Not technically typesafe, could use the proper brawl
    // api surface later.
    const soWorkManageModuleImpl& workModule = *reinterpret_cast<const soWorkManageModuleImpl*>(fighter.m_moduleAccesser->getWorkManageModule());
    return reinterpret_cast<soGeneralWorkSimple*>(workModule.m_generalWorks[varType]);
}
/*
void printRABools(const Fighter& fighter) {
    const soGeneralWorkSimple& raVars = *getWorkVars(fighter, RA_VARS);
    u32 *raBools = raVars.m_flagWorks;
    u32 boolNum;
    u32 chunk;
    char boolVal;

    OSReport("RA bools (%x): ", raBools);
    for (u32 i = 0; i < raVars.m_flagWorkSize; i++) {
        chunk = raBools[i];
        for (int j = 0; j < (sizeof(chunk)*8 - 1); j++) {
            boolNum = (i*sizeof(chunk)) + (sizeof(chunk)-j);
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
*/

bool getRABit(const Fighter& fighter, u32 idx) {
    soGeneralWorkSimple& raVars = *getWorkVars(fighter, RA_VARS);
    // array
    u32* raBools =  raVars.m_flagWorks;
    int bitFieldsSize = raVars.m_flagWorkSize;

    // printRABools(workModule);

    if (!(idx < bitFieldsSize*8*4)) {
        OSReport("Warning: asked for invalid RA bit %d from workModule %x.\n", idx, (void*)fighter.m_moduleAccesser->getWorkManageModule());
        return false;
    }

    u32 bitsChunk = raBools[idx / 32];
    char remainder = idx % 32;
    if ((bitsChunk & (1 << remainder) >> remainder) == 0) {
        return false;
    } else {
        return true;
    }
}

void printMessage(char const* msg, float xPos, float yPos, Color color = COLOR_WHITE){
    OSReport("%s\n", msg);
    printer.setup();
    printer.setTextColor(color);
    printer.renderPre = true;
    ms::CharWriter& charWriter = *(printer.charWriter);
    printer.lineHeight = punchMenu.lineHeight();
    charWriter.SetScale(punchMenu.baseFontScale.x, punchMenu.baseFontScale.y);
    charWriter.SetCursor(xPos, yPos, 0);

    printer.startBoundingBox();
    printer.print(msg);
    printer.saveBoundingBox(COLOR_TRANSPARENT_GREY, 10);
}

void printFighterState(PlayerData& playerData) {
    playerData.debugStr(strManipBuffer);
    printer.setup();
    printer.renderPre = true;
    printer.setTextColor(0xFFFFFFFF);
    ms::CharWriter& charWriter = *(printer.charWriter);
    printer.lineHeight = punchMenu.lineHeight();
    charWriter.SetScale(
        punchMenu.baseFontScale.x * punchMenu.fontScaleMultiplier,
        punchMenu.baseFontScale.y * punchMenu.fontScaleMultiplier
    );
    
    charWriter.SetEdge(1.0f, Color(0x000000FF).utColor);
    charWriter.SetCursor(
        50,
        50
    );

    printer.startBoundingBox();
    printer.print(strManipBuffer);
    printer.saveBoundingBox(0, 10);
}

inline bool needsInitializing() {
    if (initialized) return false;
    int fighters = g_ftManager->getEntryCount();
    for (char i =  0; i < fighters; i++) {
        int entryId = g_ftManager->getEntryIdFromIndex(i);
        int opStatus = g_ftManager->getFighterOperationStatus(entryId);
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
void updatePreFrame() {
    renderables.renderPre();

    // todo use gfSceneManager to do this, but both training and normal fights are scMelee, so something else
    // is needed to distinguish them.
    SCENE_TYPE sceneType = (SCENE_TYPE)getScene();
    frameCounter += 1;

    if (frameCounter % 300 == 0) {
        gfHeapManager::dumpList();
    }

    if (sceneType == VS || sceneType == TRAINING_MODE_MMS) {
        if (!initialized && !needsInitializing()) {
            #ifdef PP_INIT_DEBUG
            OSReport("Bailing out to start early\n");
            #endif
            startNormalDraw();
            return;
        }

        int fighterCount;
        fighterCount = g_ftManager->getEntryCount();
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
            PlayerData& playerData = allPlayerData[idx];

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

        if (g_ftManager->getEntryCount() > 0) {
            if (initialized) {
                punchMenu.handleInput();
                punchMenu.render(printer, strManipBuffer, PP_STR_MANIP_SIZE);
            } else if (needsInitializing()) {
                initialized = true;
                punchMenu.init();

                PadStatus pad;
                pad.btns.bits = (
                    g_padStatus[0].btns.bits 
                    | g_padStatus[1].btns.bits 
                    | g_padStatus[2].btns.bits 
                    | g_padStatus[3].btns.bits
                );
                if (!(pad.btns.L == true || pad.btns.R == true)) {
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


    renderables.renderAll();
    startNormalDraw();
}

void debugWorkModule(const Fighter& fighter) {
    u32 i;

    soGeneralWorkSimple& raVars = *getWorkVars(fighter, RA_VARS);
    soGeneralWorkSimple& laVars = *getWorkVars(fighter, LA_VARS);
    int* raBasics = raVars.m_intWorks;
    float* raFloats = raVars.m_floatWorks;
    int* laBasics = laVars.m_intWorks;
    float* laFloats = laVars.m_floatWorks;

    for(i = 0; i < raVars.m_intWorkSize; i++) {
        if (raBasics[i] != 0 && raBasics[i] != -1){
            OSReport("\tRABasic #%d: %d \n", i, raBasics[i]);
        }
    }

    for (i = 0; i < raVars.m_floatWorkSize; i++) {
        if (raFloats[i] != 0 && raFloats[i] != -1) {
            OSReport("\tRAFloat #%d: %0.2f\n", i, raFloats[i]);
        }
    }

    for(i = 0; i < laVars.m_intWorkSize; i++) {
        if (laBasics[i] != 0 && laBasics[i] != -1) {
            OSReport("\tLABasic #%d: %d \n", i, laBasics[i]);
        }
    }

    for (i = 0; i < laVars.m_floatWorkSize; i++) {
        if (laFloats[i] != 0 && laFloats[i] != -1) {
            OSReport("\tLAFloat #%d: %0.2f\n", i, laFloats[i]);
        }
    }

    return;
}


void gatherData(u8 player) {
    if (player > 3) {
        OSReport("Asked to gather data for invalid player %d\n", player);
        return;
    }

    int entryId = g_ftManager->getEntryIdFromIndex(player);
    Fighter* fighter = g_ftManager->getFighter(entryId, 0);
    u8 playerNumber = g_ftManager->getPlayerNo(entryId);

    if (needsInitializing()) {
        int opType = g_ftManager->getFighterOperationType(entryId);
        OSReport("Player %d op type: %d input: 0x%x\n", playerNumber, opType);
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
    playerData.charId = (ftKind)(fighter->getFtKind());

    allPlayerData[player].playerNumber = playerNumber;
    soModuleAccesser& modules = *fighter->m_moduleAccesser;

    soWorkManageModuleImpl* workModule = dynamic_cast<soWorkManageModuleImpl*>(modules.getWorkManageModule());
    soStatusModule* statusModule = modules.getStatusModule();
    soMotionModuleImpl* motionModule = dynamic_cast<soMotionModuleImpl*>(modules.getMotionModule());
    ftCancelModule* cancelModule = (ftCancelModule*)fighter->getCancelModule();

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

    // OSReport("Module Locations:\n\tworkModule: %x\n/\tstatusModule: %x\n\tmotionModule: %x\n", workModule, statusModule, motionModule);

    if (statusModule != NULL) {
        /* OSReport("Action number: %x\n", statusModule->action); */
        // never seems to work. strcpy(playerData.current->actionname, statusModule->getStatusName(), PP_ACTION_NAME_LEN);
        playerData.current->action = statusModule->getStatusKind();
        if (statusModule->isCollisionAttackOccer()) {
            playerData.didConnectAttack = true;
        }
    }

    /* hitstun/shieldstun stuff comes from the work module. */
    if (workModule != NULL) {
        // OSReport("Work module vals for player %d:\n", player);
        // debugWorkModule(*workModule);
        soGeneralWorkSimple& raVars = *getWorkVars(*fighter, RA_VARS);
        soGeneralWorkSimple& laVars = *getWorkVars(*fighter, LA_VARS);
        const int *RABasicsArr = raVars.m_intWorks;
        const float* RAFloatArr = raVars.m_floatWorks;
        const u32* RABoolArr = raVars.m_flagWorks;

        const int* LABasicsArr = laVars.m_intWorks;
        const float* LAFloatArr = laVars.m_floatWorks;
        const u32* LABoolArr = laVars.m_flagWorks;

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
    if (motionModule != NULL) {
        // TODO: Find a fn that returns this.
        soAnimChr animationData = motionModule->m_mainAnim;
        playerData.current->subaction = motionModule->getKind();
        if (animationData.m_anmChrRes != NULL) {
            nw4r::g3d::CHR0* animationResource = animationData.m_anmChrRes->m_anmChrFile;
            // OSReport("Animation Resource: 0x%X\n", animationResource);
            if (animationResource == NULL) {
                strncpy(playerData.current->subactionName, "UNKNOWN", PP_ACTION_NAME_LEN);
                playerData.current->actionTotalFrames = -1;
            } else {
                playerData.current->subactionFrame = motionModule->getFrame();

                // do these ever differ, except by 1?
                playerData.current->subactionTotalFrames = motionModule->getEndFrame();
                playerData.current->actionTotalFrames = animationResource->m_animLength;

                strncpy(playerData.current->subactionName, motionModule->getName(), PP_ACTION_NAME_LEN);
            }

            playerData.current->actionFrame = (u32)animationData.m_animFrame;
        }

    }
    if (cancelModule != NULL) {
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

            PlayerData& otherPlayer = allPlayerData[otherIdx];
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
    if (player.attackTarget != NULL){
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
                    addPopup(player.playerNumber, popup);
                }
            }

            player.resetTargeting();
            target.resetTargeting();
        }
    }
}

} // namespace
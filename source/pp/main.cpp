#include <cstring>
#include <so/work/so_general_work_abstract.h>
#include <so/work/so_work_manage_module_impl.h>
#include <ft/fighter.h>
#include <ft/ft_manager.h>
#include <gf/gf_scene.h>
#include <gf/gf_heap_manager.h>
#include <mu/menu.h>
#include <memory.h>

#include "pp/main.h"
#include "pp/ui.h"
#include "pp/collections/linkedlist.h"
#include "pp/graphics/draw.h"
#include "pp/input/pad.h"
#include "pp/ft_cancel_module.h"
#include "pp/color_overlay.h"
#include "pp/anim_cmd_utils.h"
#include "pp/anim_cmd_watcher.h"
#include "pp/status_change_watcher.h"
#include "pp/ledge_tech.h"
#include "pp/graphics/watermark.h"

using namespace PP::Graphics;
using namespace PP::Input;
using namespace PP::Collections;

namespace PP {
bool initialized = false;


/*
 * This is quite brittle. This gets called without saving regs, and if too much is done
 * in this function crashes will occur.
*/
void checkMenuPaused(char* gfTaskSchedulerInst) {
    //OSReport("Visible: %s, paused: %s\n", visible ? "T" : "F", paused ? "T" : "F");
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

void printMessage(char const* msg, float xPos, float yPos, Color color = PP_COLOR_WHITE){
    OSReport("%s\n", msg);
    printer.setTextColor(color);
    printer.renderPre = true;
    printer.boxBgColor = PP_COLOR_TRANSPARENT_GREY;
    printer.boxPadding = 10;

    printer.lineHeight = punchMenu.lineHeight();
    printer.setScale(punchMenu.baseFontScale, punchMenu.fontScaleMultiplier, punchMenu.lineHeightMultiplier);
    printer.setPosition(xPos, yPos);

    printer.begin();
    printer.print(msg);
    printer.renderBoundingBox();
}

void printFighterState(PlayerData& playerData) {
    playerData.debugStr(strManipBuffer);
    printer.renderPre = true;
    printer.boxPadding = 10;
    printer.boxBgColor = 0x00000000;
    printer.boxHighlightColor = 0x00000000;
    printer.boxBorderWidth = 0;
    printer.opacity = 0xBB;
    printer.setTextColor(0xFFFFFFFF);
    printer.setScale(punchMenu.baseFontScale, punchMenu.fontScaleMultiplier, punchMenu.lineHeightMultiplier);
    printer.setTextBorder(0x000000FF, 1.0f);
    printer.setPosition(35,35);

    printer.begin();
    printer.print(strManipBuffer);
    printer.renderBoundingBox();
}

/*
 * This basically waits until all fighters have fully spawned in and triggers when GO! is displayed.
 */
bool needsInitializing() {
    if (initialized) return false;
    int fighters = g_ftManager->getEntryCount();
    if (fighters == 0) { return false; }
    for (char i =  0; i < fighters; i++) {
        int entryId = g_ftManager->getEntryIdFromIndex(i);
        int opStatus = g_ftManager->getFighterOperationStatus(entryId);
        DEBUG_INIT("Fighter %d status %d\n", entryId, opStatus);
        if (opStatus == 0) { 
            return false;
        }
    }

    return true;
}

// main entry point.
void updatePreFrame() {
    renderables.renderPre();
    frameCounter += 1;
    #ifdef PP_DEBUG_MEM
    if (frameCounter % 300 == 0) {
        gfHeapManager::dumpList();
    }
    #endif


    // TODO: tried to use gfSceneManager to do this, but both training and normal fights are scMelee, so something else
    // is needed to distinguish them.
    SCENE_TYPE sceneType = (SCENE_TYPE)getScene();

    // Do nothing if not in a game.
    if (sceneType == VS || sceneType == TRAINING_MODE_MMS) {
        if (!initialized) {
            if (!needsInitializing()) {
                DEBUG_INIT("Bailing out to start early\n");
                startNormalDraw();
                return;
            } else {
                // Don't let the frame counter count up forever.
                // In one game it is highly unlikely to overflow a u32.
                // There are ~200k frames in an hour, and a u32's range is
                // over 4 billion. We do this here instead of at the end with the
                // punch menu because the punch menu wants to be setup with gathered data,
                // whereas the framecounter wants to be set before anything happens.
                frameCounter = 0;
                if (allPlayerData == NULL) {
                    allPlayerData = new PlayerData[PP_MAX_PLAYERS];
                }
            }
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

            if (playerData.didActionChange && playerData.occupiedActionableStateThisFrame) {
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

                /* If it's a VS Mode, only show if L/R are held. In training mode, 
                 * auto-open the menu UNLESS L/R are held. 
                 */
                if ((sceneType == VS) == (pad.btns.L == true || pad.btns.R == true)) {
                    punchMenu.toggle();
                }
            }
        }

        drawAllPopups();

        for (idx = 0; idx < fighterCount; idx++) {
            processOverlays(allPlayerData[idx]);
        }

        g_watermark.process();
    } else { // end if we're a relevant scene.
        if (initialized) {
            initialized = false;
            punchMenu.cleanup();

            delete[] allPlayerData;
            allPlayerData = NULL;
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


enum GatherDataErrors {
    NO_FT_ENTRY,
    NO_FIGHTER,
    HIGH_SLOT,
    INVALID_SLOT
};
void gatherData(u8 playerEntryIdx) {
    if (playerEntryIdx > 3) {
        OSReport("Asked to gather data for invalid player entry index %d\n", playerEntryIdx);
        return;
    }

    int entryId = g_ftManager->getEntryIdFromIndex(playerEntryIdx);
    ftEntry* entry = g_ftEntryManager->getEntity(entryId);
    Fighter* fighter = g_ftManager->getFighter(entryId, 0);
    if (entry == NULL) {
        OSReport(g_strTypedError, "gatherData", NO_FT_ENTRY);
        return;
    }
    if (fighter == NULL) {
        OSReport(g_strTypedError, "gatherData", NO_FIGHTER);
        return;
    }

    /* 
     * This roundabout with the slot numbers is because the order the players are loaded in isn't always
     * the same as which "slot" holds their % and stock icons and it could be wrong otherwise.
     */
    if (entry->m_slotIndex >= g_ftManager->m_slotManager->m_slotCount) {
        OSReport(g_strTypedError, "gatherData", INVALID_SLOT);
    }
    int playerNumber = g_ftManager->m_slotManager->m_slots[entry->m_slotIndex].m_slotNo;
    if (playerNumber > 3) {
        OSReport(g_strTypedError, "gatherData", HIGH_SLOT);
        return;
    }
    PlayerData& playerData = allPlayerData[playerNumber];

    if (needsInitializing()) {
        int opType = g_ftManager->getFighterOperationType(entryId);
        playerData.charId = (ftKind)(fighter->getFtKind());
        playerData.taskId = fighter->m_taskId;
        int muCharKind = muMenu::exchangeGmCharacterKind2MuStockchkind(entry->m_characterKind);
        playerData.fighterName = muMenu::exchangeMuStockchkind2MuCharName(muCharKind);
        playerData.playerNumber = playerNumber;

        OSReport("Initializing P%d: %s\n", playerNumber, playerData.fighterName);

        DEBUG_INIT("Player %d op type: %d\n", playerNumber, opType);
        if (opType != 0) {
            allPlayerData[playerNumber].showOnShieldAdvantage = false;
            allPlayerData[playerNumber].showOnHitAdvantage = false;
        } else {
            allPlayerData[playerNumber].showOnShieldAdvantage = true;
            allPlayerData[playerNumber].showOnHitAdvantage = false;
        };

        AnimCmdWatcher* animCmdEventHandler = new AnimCmdWatcher(&allPlayerData[playerNumber], fighter);
        animCmdEventHandler->registerWith(fighter);
        allPlayerData[playerNumber].animCmdWatcher = animCmdEventHandler;
        StatusChangeWatcher* statusChangeWatcher = new StatusChangeWatcher(&allPlayerData[playerNumber]);
        statusChangeWatcher->registerWith(fighter);
        allPlayerData[playerNumber].statusChangeWatcher = statusChangeWatcher;

        playerData.initLedgeTechWatcher(*fighter);
    }

    playerData.prepareNextFrame();
    PlayerDataOnFrame& currentData = *playerData.current;
    PlayerDataOnFrame& prevData = *playerData.current;
    playerData.entryId = entryId;

    soModuleAccesser& modules = *fighter->m_moduleAccesser;

    soWorkManageModuleImpl* workModule = dynamic_cast<soWorkManageModuleImpl*>(modules.getWorkManageModule());
    soStatusModule* statusModule = modules.getStatusModule();
    soMotionModuleImpl* motionModule = dynamic_cast<soMotionModuleImpl*>(modules.getMotionModule());
    ftCancelModule* cancelModule = reinterpret_cast<ftCancelModule*>(fighter->getCancelModule());

    DEBUG_FIGHTERS(
        "Player: %d\n"
        "\tchar type: 0x%X\n" 
        "\tentryId: 0x%X\n"
        "\tfighter addr: 0x%X\n"
        ,
        playerNumber, playerData.charId, entryId, fighter
    );

    DEBUG_FIGHTERS(
        "\tModule Locations:\n"
        "\t\tworkModule: 0x%x\n"
        "\t\tstatusModule: 0x%x\n"
        "\t\tmotionModule: 0x%x\n"
        ,
        workModule, statusModule, motionModule);

    if (statusModule != NULL) {
        /* Status module stuff is mostly collected via the StatusChangeWatcher using the event system. */
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

        #pragma region shieldstun
        // float shieldValue = LAFloatArr[0x3];
        if (currentData.action == ACTION_GUARDDAMAGE) {
            // This also tracks the lag on shield release frames, which
            // we don't care about for actionability. So we don't want to get this
            // during GuardOff, for example.
            currentData.shieldstun = RABasicsArr[0x5];
        }
        if (currentData.shieldstun != 0) {
            /* TODO: Rework with event system */
            if (playerData.didReceiveShieldstun()) {
                playerData.maxShieldstun = currentData.shieldstun;
                playerData.becameActionableOnFrame = -1;
                playerData.advantageBonusCounter = 0;
            }

            DEBUG_FIGHTERS("Player %d shieldstun: %d/%d\n",
                playerData.playerNumber, currentData.shieldstun, playerData.maxShieldstun
            );
        } else {
            playerData.maxShieldstun = 0;
        }
        #pragma endregion

        #pragma region hitstun
        int remainingHitstun = LABasicsArr[0x38];
        playerData.current->hitstun = remainingHitstun;

        if (remainingHitstun != 0) {
            /* TODO: Rework with event system */
            if (playerData.didReceiveHitstun()) {
                playerData.maxHitstun = remainingHitstun;
                playerData.becameActionableOnFrame = -1;
                playerData.advantageBonusCounter = 0;
            }
        } else {
            playerData.maxHitstun = 0;
        }
        #pragma endregion

        currentData.lowRABits = RABoolArr[0];
    }


    #pragma region subaction
    if (motionModule != NULL) {
        // TODO: Find a fn that returns this.
        soAnimChr animationData = motionModule->m_mainAnim;
        playerData.current->subaction = motionModule->getKind();
        if (animationData.m_anmChrRes != NULL) {
            nw4r::g3d::CHR0* animationResource = animationData.m_anmChrRes->m_anmChrFile;
            // OSReport("Animation Resource: 0x%X\n", animationResource);
            if (animationResource == NULL) {
                playerData.current->subactionName = NULL;
            } else {
                playerData.current->subactionFrame = motionModule->getFrame();

                // do these ever differ, except by 1?
                playerData.current->subactionTotalFrames = motionModule->getEndFrame();
                playerData.current->subactionName = motionModule->getName();
            }
        }
    }
    #pragma endregion

    if (playerData.enableLedgeTechWatcher) {
        playerData.ledgeTechWatcher->process(*fighter);
    }
}

void resolveAttackTarget(u8 playerIdx) {
    PlayerData& player = allPlayerData[playerIdx];
    // False most of the time, so this isn't as slow as it looks. If either of the
    // "isAttackingBlah" flags are true, this already passed so we skip both for speed
    // and so that we don't get into a situation where a target thinks he's being attacked
    // but the player hit something else and moved on.
    if (!(player.isAttackingFighter || player.isAttackingShield)) {
        for (char otherIdx = 0; otherIdx < PP_MAX_PLAYERS; otherIdx++) {
            if (playerIdx == otherIdx) {
                continue;
            }

            PlayerData& otherPlayer = allPlayerData[otherIdx];
            if (player.showOnHitAdvantage && otherPlayer.didReceiveHitstun()) {
                player.resetTargeting();
                otherPlayer.resetTargeting();

                OSReport("Set on-hit ATK %d -> DEF %d\n", player.playerNumber, otherPlayer.playerNumber);
                player.attackTarget = &(otherPlayer);
                player.isAttackingFighter = true;
                break;
            } else if (player.showOnShieldAdvantage && otherPlayer.didReceiveShieldstun()) {
                player.resetTargeting();
                otherPlayer.resetTargeting();

                OSReport("Set on-shld ATK %d -> DEF %d\n", player.playerNumber, otherPlayer.playerNumber);
                player.attackTarget = &(otherPlayer);
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
            if (advantage > -50 && advantage < 50) {
                if (player.showOnShieldAdvantage) {
                    Popup& popup = *player.createPopup();
                    popup.printf("Advantage: %d\n", advantage);
                    popup.coords = getHpPopupBoxCoords(player.playerNumber);
                    popup.durationSecs = 3;
                }
            }

            player.resetTargeting();
            target.resetTargeting();
        }
    }
}

} // namespace
#include <cstring>
#include <limits.h>
#include <so/work/so_general_work_abstract.h>
#include <so/work/so_work_manage_module_impl.h>
#include <ft/fighter.h>
#include <ft/ft_manager.h>
#include <gf/gf_scene.h>
#include <gf/gf_heap_manager.h>
#include <mu/menu.h>
#include <memory.h>

#include "pp/main.h"
#include "pp/utils.h"
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
bool battleSceneInitialized = false;

/*
 * This is quite brittle. This gets called without saving regs, and if too much is done
 * in this function crashes will occur.
*/
void checkMenuPaused(char* gfTaskSchedulerInst) {
    //OSReport("Visible: %s, paused: %s\n", visible ? "T" : "F", paused ? "T" : "F");
    //OSReport("Trying to read gfTaskSchedulerInst @ 0x%0X\n", gfTaskSchedulerInst);
    if (PP_IS_PAUSED) { gfTaskSchedulerInst[0xB] |= 0x8; }
    else { gfTaskSchedulerInst[0xB] &= ~0x8; }
}



/*
 * This basically waits until all fighters have fully spawned in and triggers when GO! is displayed.
 */
bool battleSceneNeedsInitializing() {
    if (battleSceneInitialized) return false;
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
    /* Putting this first means that stuff like popups and the menu are drawn on top of it. */
    LedgeTechDisplayDrawable::drawInstance();
    renderables.renderPre();

    #ifdef PP_DEBUG_MEM
    if (frameCounter % 300 == 0) {
        gfHeapManager::dumpList();
    }
    #endif

    SCENE_TYPE sceneType = (SCENE_TYPE)getScene();

    // Do nothing if not in a game.
    if (sceneType == VS || sceneType == TRAINING_MODE_MMS) {
        if (!battleSceneInitialized) {
            initializeBattleScene();
        }

        if (battleSceneInitialized) {
            updateBattleScene();
        }
    } else { // end if we're a relevant scene.
        if (battleSceneInitialized) {
            battleSceneInitialized = false;
            punchMenu.cleanup();

            delete[] allPlayerData;
            allPlayerData = NULL;
        }
    }

    renderables.renderAll();
    startNormalDraw();
}

bool initializePlayer(u8 playerEntryIdx) {
    int entryId = g_ftManager->getEntryIdFromIndex(playerEntryIdx);
    ftEntry* entry = g_ftEntryManager->getEntity(entryId);
    Fighter* fighter = g_ftManager->getFighter(entryId, 0);
    if (entry == NULL) {
        OSReport(g_strTypedError, "gatherData", NO_FT_ENTRY);
        return false;
    }
    if (fighter == NULL) {
        OSReport(g_strTypedError, "gatherData", NO_FIGHTER);
        return false;
    }

    /*
    * This roundabout with the slot numbers is because the order the players are loaded in isn't always
    * the same as which "slot" holds their % and stock icons and it could be wrong otherwise.
    */
    if (entry->m_slotIndex >= g_ftManager->m_slotManager->m_slotCount) {
        OSReport(g_strTypedError, "gatherData", INVALID_SLOT);
    }
    int playerNumber = g_ftManager->m_slotManager->m_slots[entry->m_slotIndex].m_slotNo;
    PlayerData& playerData = allPlayerData[playerNumber];

    if (playerNumber > 3) {
        OSReport(g_strTypedError, "gatherData", HIGH_SLOT);
        return false;
    }

    int opType = g_ftManager->getFighterOperationType(entryId);
    playerData.charId = (ftKind)(fighter->getFtKind());
    playerData.taskId = fighter->m_taskId;
    playerData.entryId = entryId;
    int muCharKind = muMenu::exchangeGmCharacterKind2MuStockchkind(entry->m_characterKind);
    playerData.fighterName = muMenu::exchangeMuStockchkind2MuCharName(muCharKind);
    playerData.playerNumber = playerNumber;

    OSReport("Initializing P%d: %s\n", playerNumber, playerData.fighterName);

    AnimCmdWatcher* animCmdEventHandler = new AnimCmdWatcher(&allPlayerData[playerNumber], fighter);
    animCmdEventHandler->registerWith(fighter);
    allPlayerData[playerNumber].animCmdWatcher = animCmdEventHandler;
    StatusChangeWatcher* statusChangeWatcher = new StatusChangeWatcher(&allPlayerData[playerNumber]);
    statusChangeWatcher->registerWith(fighter);
    allPlayerData[playerNumber].statusChangeWatcher = statusChangeWatcher;

    DEBUG_INIT("Player %d op type: %d\n", playerNumber, opType);
    if (opType == 0) { // Player is a human
        allPlayerData[playerNumber].showOnShieldAdvantage = true;
        allPlayerData[playerNumber].showOnHitAdvantage = false;

        if (getScene() == TRAINING_MODE_MMS) {
            allPlayerData[playerNumber].enableLedgeTechFramesOnLedgePopup = false;
            allPlayerData[playerNumber].enableLedgeTechGalintPopup = true;
            allPlayerData[playerNumber].enableLedgeTechFrameDisplay = true;
        }
    };
    playerData.prepareNextFrame();

    return true;
}

void initializeBattleScene() {
    u8 idx = 0;
    if (!battleSceneNeedsInitializing()) {
        DEBUG_INIT("Bailing out to start early\n");

        // The punch menu isn't initialized at this point so we don't have to worry about it.
        g_frameAdvance.paused = false;
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
        menuFrameCounter = 0;

        if (allPlayerData == NULL) {
            allPlayerData = new PlayerData[PP_MAX_PLAYERS];
        }

        for (idx = 0; idx < g_ftManager->getEntryCount(); idx++) {
            bool didInitializePlayer = initializePlayer(idx);
            if (!didInitializePlayer) {
                OSReport("Failed to initialize player %d. Nothing will work.\n");
                return;
            }
        }

        punchMenu.init();
        battleSceneInitialized = true;
    }
}

void updateBattleScene() {
    u8 idx = 0;
    if (!battleSceneInitialized) { return; }

    punchMenu.handleInput();
    g_frameAdvance.process();

    int fighterCount = g_ftManager->getEntryCount();
    /* Data gathering */
    for (idx = 0; idx < fighterCount; idx++) {
        gatherData(allPlayerData[idx]);
    }

    for (idx = 0; idx < fighterCount; idx++) {
        resolveAttackTarget(allPlayerData[idx]);
    }

    for (idx = 0; idx < fighterCount; idx++) {
        checkAttackTargetActionable(allPlayerData[idx]);
    }

    /* Drawing Section */
    if (punchMenu.visible) {
        punchMenu.render(printer, strManipBuffer, PP_STR_MANIP_SIZE);
    } else {
        drawAllPopups();

        for (idx = 0; idx < fighterCount; idx++) {
            processOverlays(allPlayerData[idx]);
        }

        for (idx = 0; idx < fighterCount; idx++) {
            if (allPlayerData[idx].showFighterState) {
                allPlayerData[idx].printFighterState();
                break;
            }
        }
    }

    menuFrameCounter += 1;
    g_watermark.process();

    if (PP_IS_PAUSED) { return; }

    /* Clean up and prepare for the next frame. */
    for (idx = 0; idx < fighterCount; idx++) {
        /*
         * The location of this determines whether event handlers are considered on the
         * same frame or a previous frame. If this is at the beginning, it means that the
         * event handlers are considered as part of the previous frame, but they have full
         * access to that frame's gathered data.
         *
         * Doing this at the END means that the event handlers can modify the *current* frame but
         * they have to access the previous frame's data for more accurate calculations. This is more
         * accurate anyway, given that our update hook currently is placed at the very end of
         * the game's update loop.
         *
         */
        allPlayerData[idx].prepareNextFrame();
    }

    frameCounter += 1;
}

void gatherData(PlayerData& playerData) {
    playerData.preFrame();

    Fighter* fighter = g_ftManager->getFighter(playerData.entryId, 0);

    PlayerDataOnFrame& currentData = *playerData.current;
    PlayerDataOnFrame& prevData = *playerData.current;

    soModuleAccesser& modules = *fighter->m_moduleAccesser;

    soWorkManageModuleImpl* workModule = dynamic_cast<soWorkManageModuleImpl*>(modules.getWorkManageModule());
    soStatusModule* statusModule = modules.getStatusModule();
    soMotionModuleImpl* motionModule = dynamic_cast<soMotionModuleImpl*>(modules.getMotionModule());
    ftCancelModule* cancelModule = reinterpret_cast<ftCancelModule*>(fighter->getCancelModule());
    soCollisionHitModuleImpl* collisionHitModule = dynamic_cast<soCollisionHitModuleImpl*>(modules.getCollisionHitModule());
    soControllerImpl* controller = dynamic_cast<soControllerImpl*>(modules.getControllerModule()->getController());


    if (!(controller == NULL || controller == (soControllerImpl*)0xCCCCCCCC)) {
        playerData.current->recordControllerStatus(*controller);
    }

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
                playerData.attackStartFrame = frameCounter;
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
                playerData.attackStartFrame = frameCounter;
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
            nw4r::g3d::ResAnmChr animationResource = animationData.m_anmChrRes->m_anmChrFile;
            // OSReport("Animation Resource: 0x%X\n", animationResource);
            if (animationResource.ptr() == NULL) {
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

    if (collisionHitModule != NULL) {
        if (collisionHitModule->m_collisionHitGroupArray != NULL && collisionHitModule->m_collisionHitGroupArray->size() > 0) {
            soCollisionHitGroup* hitGroup = collisionHitModule->m_collisionHitGroupArray->at(0);

            playerData.current->bodyHurtboxType = (HurtboxStatus)hitGroup->m_whole;
            playerData.current->ledgeIntan = hitGroup->m_xluFrameGlobal;
            if (playerData.current->ledgeIntan == 0 && playerData.prev->ledgeIntan > INT_MIN) {
                playerData.current->ledgeIntan = playerData.prev->ledgeIntan - 1;
            }
        }
    }

    if (cancelModule != NULL) {
        playerData.current->canCancel = playerData.current->canCancel || cancelModule->isEnableCancel();
    }

    if (LedgeTechWatcher::isEnabled(playerData)) {
        if (playerData.ledgeTechWatcher == NULL) {
            playerData.initLedgeTechWatcher(*fighter);
        }
        playerData.ledgeTechWatcher->process(*fighter);
    }
}

void resolveAttackTarget(PlayerData& player) {
    // False most of the time, so this isn't as slow as it looks. If either of the
    // "isAttackingBlah" flags are true, this already passed so we skip both for speed
    // and so that we don't get into a situation where a target thinks he's being attacked
    // but the player hit something else and moved on.
    if (!(player.isAttackingFighter || player.isAttackingShield)) {
        for (char otherIdx = 0; otherIdx < PP_MAX_PLAYERS; otherIdx++) {
            if (player.playerNumber == otherIdx) {
                continue;
            }

            PlayerData& otherPlayer = allPlayerData[otherIdx];
            if (otherPlayer.didReceiveHitstun()) {
                player.resetTargeting();
                otherPlayer.resetTargeting();

                OSReport("Set on-hit ATK %d -> DEF %d\n", player.playerNumber, otherPlayer.playerNumber);
                player.attackTarget = &(otherPlayer);
                player.isAttackingFighter = true;
                break;
            } else if (otherPlayer.didReceiveShieldstun()) {
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

void checkAttackTargetActionable(PlayerData& player) {

    // Player is attacking someone. The second check guards against frame 1 moves.
    if (player.attackTarget != NULL && player.hasStartedAttack()){
        PlayerData& target = *(player.attackTarget);

        bool targetIsActionable = target.resolveTargetActionable();
        bool playerIsActionable = player.resolvePlayerActionable();


        if (playerIsActionable && targetIsActionable) {
            /* POP UP TIME! */
            short int advantage = target.becameActionableOnFrame - player.becameActionableOnFrame;

            /* Lots of weird edge cases where the ending doesn't register, such as dying or teching. */
            /* > 30 frames is generally judge-able with a human eye anyway. */
            if (advantage > -50 && advantage < 50) {
                if (player.isAttackingFighter && player.showOnHitAdvantage) {
                    player.createPopup("OnHit: %+d\n", advantage);
                } else if (player.isAttackingShield && player.showOnShieldAdvantage) {
                    player.createPopup("OnShield: %+d\n", advantage);
                }
            }

            player.resetTargeting();
            target.resetTargeting();
        }
    }
}

} // namespace
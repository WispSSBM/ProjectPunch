#pragma once

#include "pp/common.h"
#include "pp/frame_advance.h"
#include "pp/ui.h"

namespace PP {

#define CHECK_MENU_PAUSED_HOOK_LOC 0x8002e5b0
#define CHECK_MENU_PAUSED_HOOK_LOC_RETURN 0x8002e5b4
#define PP_IS_PAUSED (punchMenu.paused || g_frameAdvance.paused)

extern bool battleSceneInitialized;

enum GatherDataErrors {
    NO_FT_ENTRY,
    NO_FIGHTER,
    HIGH_SLOT,
    INVALID_SLOT
};

class PlayerData;

Coord2D getHpPopupBoxCoords(int playerNum);
void checkMenuPaused(char* gfTaskSchedulerInst);
void gatherData(PlayerData& playerData);
void drawAllPopups();

void initializeBattleScene();
void updateBattleScene();
bool battleSceneNeedsInitializing();
bool initializePlayer(u8 playerEntryIdx);

void resolveAttackTarget(PlayerData& playerData);
void checkAttackTargetActionable(PlayerData& playerData);
void updatePreFrame();

} //namespace
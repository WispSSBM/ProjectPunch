#pragma once

#include "pp/common.h"

namespace PP {

#define CHECK_MENU_PAUSED_HOOK_LOC 0x8002e5b0
#define CHECK_MENU_PAUSED_HOOK_LOC_RETURN 0x8002e5b4

extern bool initialized;

Coord2D getHpPopupBoxCoords(int playerNum);
void checkMenuPaused(char* gfTaskSchedulerInst);
void gatherData(u8 fighter);
void drawAllPopups();
void resolveAttackTarget(u8 playerNum);
void checkAttackTargetActionable(u8 playerNum);
void updatePreFrame();

} //namespace
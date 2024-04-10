#ifndef PP_DRAW_H
#define PP_DRAW_H

#include <stddef.h>
#include <Assembly.h>
#include <Memory.h>

#include <Brawl/sndSystem.h>
#include <Brawl/SO/SoMotionModule.h>

#include <Brawl/FT/ftManager.h>
#include <Brawl/FT/ftSlotManager.h>
#include <Brawl/FT/ftCancelModule.h>
#include <CLibs/cstring.h>

#include <Graphics/Drawable.h>
#include <Graphics/TextPrinter.h>

#include <Wii/PAD/PADStatus.h>

#include "./common.h"

#include "./actions.h"
#include "./playerdata.h"
#include "./popup.h"
#include "./linkedlist.h"
#include "./ui.h"

SCENE_TYPE getScene();

Coord2D getHpPopupBoxCoords(int playerNum);

float fmax(float x1, float x2);
int max(int x1, int x2);

void gatherData(u8 fighter);
void drawAllPopups();
void resolveAttackTarget(u8 playerNum);
void checkAttackTargetActionable(u8 playerNum);

#endif
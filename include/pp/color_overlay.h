#pragma once

#include "pp/playerdata.h"
#include "pp/common.h"

namespace PP {

#define COLOR_OVERLAY_WAIT ((Color)0x00FF00BB)
#define COLOR_OVERLAY_IASA ((Color)0xF5B642BB)
#define COLOR_OVERLAY_DASH ((Color)0x42D1F5BB)
#define COLOR_OVERLAY_RUN ((Color)0x427BF5BB)
#define COLOR_OVERLAY_NONE ((Color)0x00000000)

void processOverlays(const PlayerData& player);
bool processActionableOverlay(const PlayerData& player, soColorBlendModule& cbm);
bool processDashOverlay(const PlayerData& player, soColorBlendModule& cbm);
bool processIasaOverlay(const PlayerData& player, soColorBlendModule& cbm);

} // namespace
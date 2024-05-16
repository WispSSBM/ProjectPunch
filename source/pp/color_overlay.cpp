#include "pp/color_overlay.h"

#include <ft/ft_manager.h>
#include <OS/OSError.h>

namespace PP {

void processOverlays(const PlayerData& player) {
    Fighter& fighter = *g_ftManager->getFighter(player.entryId, 0);
    soColorBlendModule* cbm = fighter.m_moduleAccesser->getColorBlendModule();
    bool overlaySetThisFrame = false;
    if (cbm == NULL) {
        DEBUG_OVERLAY("Player %d with fighter @ 0x%0x has null color blend mod.\n", player.playerNumber, &fighter);
        return;
    }

    if (player.enableWaitOverlay) {
        overlaySetThisFrame |= processWaitOverlay(player, *cbm);
    }

    if (player.enableDashOverlay) {
        overlaySetThisFrame |= processDashOverlay(player, *cbm);
    }

    if (player.enableIasaOverlay) {
        overlaySetThisFrame |= processIasaOverlay(player, *cbm);
    }

    if (!overlaySetThisFrame) {
        cbm->setSubColor(COLOR_OVERLAY_NONE.gxColor, 1);
    }
}

bool processWaitOverlay(const PlayerData& player, soColorBlendModule& cbm) {
    if(player.occupiedWaitingStateThisFrame
      || (
        player.inGroundedIasa() && isLandingAction(player.current->action)
      )
    ) {
        cbm.setSubColor(COLOR_OVERLAY_WAIT.gxColor, 1);
        return true;
    }
    return false;
}

bool processDashOverlay(const PlayerData& player, soColorBlendModule& cbm) {
    u16 action = player.action();
    if (action == ACTION_DASH) {
        cbm.setSubColor(COLOR_OVERLAY_DASH.gxColor, 1);
        return true;
    } else if (action == ACTION_RUN || action == ACTION_RUNBRAKE) {
        cbm.setSubColor(COLOR_OVERLAY_RUN.gxColor, 1);
        return true;
    }
    return false;
}

bool processIasaOverlay(const PlayerData& player, soColorBlendModule& cbm) {
    if (player.inIasa() && player.inAttackState()) {
        cbm.setSubColor(COLOR_OVERLAY_IASA.gxColor, 1);
        return true;
    }
    return false;
}

} // namespace
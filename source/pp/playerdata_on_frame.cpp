#include "pp/playerdata_on_frame.h"
#include "pp/actions.h"

#include <so/controller/so_controller_impl.h>

namespace PP {

PlayerDataOnFrame::PlayerDataOnFrame() {
        DEBUG_CTOR("PlayerDataOnFrame ctor: this=0x%x\n", this);

        action = 0;
        actionName = NULL;
        subaction = 0;
        subactionFrame = 0;
        subactionName = NULL;
        subactionTotalFrames = 0;

        lowRABits = 0;
        hitstun = 0;
        shieldstun = 0;
}


bool PlayerDataOnFrame::getLowRABit(u32 idx) const {
    if (idx >= 32) {
        OSReport("WARNING: Asked for ra-bit that's too high.\n");
        return false;
    }

    return 1 == ((this->lowRABits & (1 << idx)) >> idx);
}

void PlayerDataOnFrame::recordControllerStatus(const soControllerImpl& controller) {
    this->controllerBtnHeld.m_bits = controller.m_button;
    this->controllerBtnPressed.m_bits = controller.m_trigger;
    this->controllerBtnReleased.m_bits = controller.m_release;
    this->stick = Coord2DF(controller.m_mainStickX, controller.m_mainStickY);
    this->substick = Coord2DF(controller.m_subStickX, controller.m_subStickY);
}

const char* PlayerDataOnFrame::subactionStr() const {
    if (subactionName == NULL || subactionName == (char*)0xCCCCCCCC) {
        return "UNKNOWN";
    } else {
        return subactionName;
    };
}

bool PlayerDataOnFrame::isShielding() const {
    return (action == ACTION_GUARD || action == ACTION_GUARDDAMAGE || action == ACTION_GUARDON);
}

bool PlayerDataOnFrame::inIasa() const {
    const InterruptGroupStates& ig = interruptGroups;
    if (isAirborne == true) {
        // IASA in the air is more for people being able to cancel tumble rather than something that's used a lot.
        // It comes up occasionally with peach float dair.
        return canAutocancel();
    } else {
        return inGroundedIasa();
    }
}

bool PlayerDataOnFrame::inGroundedIasa() const {
    const InterruptGroupStates& ig = interruptGroups;
    return !isAirborne && (canCancel || (ig.groundAttack | ig.groundDodge | ig.groundGrab | ig.groundGuard | ig.groundJump | ig.groundSpecial));
}

bool PlayerDataOnFrame::canAutocancel() const {
    return getLowRABit(RA_BIT_ENABLE_LANDING_LAG) != true;
}



} // namespace PP
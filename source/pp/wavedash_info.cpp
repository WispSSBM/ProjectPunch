#include <gf/gf_pad_system.h>
#include <gf/gf_pad_status.h>

#include "pp/graphics/drawable.h"
#include "pp/graphics/draw.h"
#include "pp/status_change_watcher.h"
#include "pp/collections/vector.h"
#include "pp/graphics/text_printer.h"
#include "pp/utils.h"

#include "pp/wavedash_info.h"


void PP::WavedashInfoModule::notifyEventChangeStatus(
    int statusKind,
    int prevStatusKind,
    soStatusData* statusData,
    soModuleAccesser *moduleAccesser
) {
    if (this->playerData == NULL || !this->isEnabled(*this->playerData)) { return; }

    switch (statusKind) {
        case ACTION_JUMPSQUAT:
            this->didEnterJumpsquat(moduleAccesser);
            break;
        case ACTION_JUMP:
            this->didGoAirborne(moduleAccesser);
            break;
        case ACTION_ESCAPEAIR:
            this->didAirdodge(moduleAccesser);
            break;
        case ACTION_LANDINGFALLSPECIAL:
            this->didLand(moduleAccesser);
            break;
    }
}

bool PP::WavedashInfoModule::isEnabled(const PlayerData& playerData) {
    PlayerSettings& settings = globalSettings.playerSettings[playerData.playerNumber];
    return settings.enableWavedashInfo;
}

bool PP::WavedashInfoModule::timedOut() {
    return 
     (
        (frameEnteredJumpsquat > 0)
        && ((frameCounter - frameEnteredJumpsquat) > 50)
     )
        || 
    (
        (frameEnteredAirdodge > 0)
        && ((frameCounter - frameEnteredAirdodge) > 50)
    );

}

bool PP::WavedashInfoModule::isJumpInputReleased(){
    gfPadStatus pad;
    g_gfPadSystem.getSysPadStatus(this->playerData->playerNumber, &pad);

    return (
        pad.m_buttonsReleasedThisFrame.bits & (GF_PAD_BUTTON_BIT_X | GF_PAD_BUTTON_BIT_Y)
    );
};


void PP::WavedashInfoModule::process(Fighter& fighter) {
    if (!this->isEnabled) { return; }
    if (this->frameEnteredJumpsquat == 0 && this->frameEnteredAirdodge == 0) {
        return;
    }


    if (
        playerData->current->action == ACTION_JUMPSQUAT
        && !didShorthop
        && isJumpInputReleased()
    ) {
        didShorthop = true;
    }

    if (
        this->playerData->current->controllerBtnPressed.m_shield
        && this->playerData->current->action == ACTION_JUMPSQUAT
    ) {
        // process() happens the frame after the statuswatcher methods.
        framePressedShield = frameCounter-1;

        u32 timeInJumpsquat = framePressedShield - frameEnteredJumpsquat;
        u32 tooEarly = (u32)(playerData->current->subactionTotalFrames) - timeInJumpsquat;

        this->playerData->createPopup(
            "WD: %df early\n",
            tooEarly
        );

        this->reset();
        return;
    }

    if (this->timedOut()) {
        this->reset();
    }
}

void PP::WavedashInfoModule::reset() {
    PP_DEBUG_WAVEDASH("Reset on f%d\n", frameCounter);
    frameEnteredJumpsquat = 0;
    frameEnteredAir = 0;
    frameEnteredAirdodge = 0;
    frameEnteredLanding = 0;
    frameAirdodgeInput = 0;
    framePressedShield = 0;
    didShorthop = false;
    airdodgeAngle = 0;
}

void PP::WavedashInfoModule::didEnterJumpsquat(soModuleAccesser* moduleAccesser){
    PP_DEBUG_WAVEDASH("didEnterJumpsquat f%d\n", frameCounter);
    this->frameEnteredJumpsquat = frameCounter;
}

void PP::WavedashInfoModule::didGoAirborne(soModuleAccesser* moduleAccesser) {
    PP_DEBUG_WAVEDASH("didGoAirborne f%d\n", frameCounter);

    // If this isn't checked here, we may never reach the next process() tick,
    // as you can airdodge and land on the same frame.
    if (isJumpInputReleased()) {
        didShorthop = true;
    }

    frameEnteredAir = frameCounter;
}

void PP::WavedashInfoModule::didAirdodge(soModuleAccesser* soModuleAccesser) {
    PP_DEBUG_WAVEDASH("didAirdodge f%d\n", frameCounter);
    soControllerImpl* controller = dynamic_cast<soControllerImpl*>(soModuleAccesser->getControllerModule()->getController());
    Coord2DF stick = Coord2DF(
        controller->m_mainStickX,
        controller->m_mainStickY
    );
    this->airdodgeAngle = stick.quadrantDegrees();
    this->frameEnteredAirdodge = frameCounter;
}

void PP::WavedashInfoModule::didLand(soModuleAccesser* soModuleAccesser) {
    PP_DEBUG_WAVEDASH("didLand f%d\n", frameCounter);
    this->frameEnteredLanding = frameCounter;

    if (frameEnteredAirdodge == 0) {
        return;
    }

    u32 airdodgeLandingDelay = frameEnteredLanding - frameEnteredAirdodge;
    u32 jumpsquatAirdodgeDelay = frameEnteredAirdodge - frameEnteredAir;

    this->playerData->createPopup(
        "f%d, %0.0fdeg, land: f%d, sHop: %s\n",
        jumpsquatAirdodgeDelay,
        this->airdodgeAngle,
        airdodgeLandingDelay,
        sbool(this->didShorthop)
    );

    this->reset();
}
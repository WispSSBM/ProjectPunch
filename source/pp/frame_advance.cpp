#include "pp/frame_advance.h"

#include "pp/input/pad.h"
#include "pp/global_settings.h"
#include "pp/common.h"
#include "pp/ui.h"

namespace PP {

FrameAdvanceModule g_frameAdvance = FrameAdvanceModule();

const char* frameAdvanceButtonOptions[] = { "Z", "L", "R" };

void gfPadUpdateHook() {
    register u32 padStatusR29;
    register u32 padStatusR28;
    asm {
        mr padStatusR28, r28;
        mr padStatusR29, r29;
    }

    gfPadStatus* padStatus = reinterpret_cast<gfPadStatus*>(padStatusR28);
    gfPadStatus* prevPadStatus = reinterpret_cast<gfPadStatus*>(padStatusR29);

    if (padStatus->m_error == gfPadError::NONE) {
        DEBUG_INPUT("padStatus: 0x%0x, error: %d\n", (u32)padStatus, padStatus->m_isNotConnected);
        DEBUG_INPUT("  A: %d B: %d X: %d Y: %d Z: %d L: %d R: %d S: %d\n",
            padStatus->m_buttonsCurrentFrame.m_a, 
            padStatus->m_buttonsCurrentFrame.m_b, 
            padStatus->m_buttonsCurrentFrame.m_x, 
            padStatus->m_buttonsCurrentFrame.m_y, 
            padStatus->m_buttonsCurrentFrame.m_z, 
            padStatus->m_buttonsCurrentFrame.m_l, 
            padStatus->m_buttonsCurrentFrame.m_r, 
            padStatus->m_buttonsCurrentFrame.m_start
        );

        DEBUG_INPUT("  wiimote stuff 0x18: %0.2f 0x1c: %0.2f 0x20: %0.2f 0x24: %0.2f 0x28: %0.2f 0x2C: %0.2f\n",
            padStatus->_0x18, padStatus->_0x1c,
            padStatus->_0x20, padStatus->_0x24,
            padStatus->_0x28, padStatus->_0x2c
        );

        DEBUG_INPUT("  stick: (%d, %d) substick: (%d, %d) analog L/R: (%d, %d)\n",
            padStatus->m_stickX,
            padStatus->m_stickY,
            padStatus->m_subStickX,
            padStatus->m_subStickY,
            padStatus->m_lTriggerAnalog,
            padStatus->m_rTriggerAnalog,
        );

        if (padStatus->m_error == gfPadError::NONE) {
            g_frameAdvance.handleInput(*padStatus);
        }
    }
}

bool FrameAdvanceModule::isRunning() {
    SCENE_TYPE scene = getScene();
    return (
        (GlobalSettings::enableFrameAdvance)
        && (scene == VS || scene == TRAINING_MODE_MMS)
        && !punchMenu.paused
    );
}

void FrameAdvanceModule::handleInput(gfPadStatus& padStatus) {
    if (!this->isRunning()) {
        this->paused = false;
        return;
    }

    switch(GlobalSettings::frameAdvanceButton) {
        case FAB_Z:
            _btnPressedThisFrame = _btnPressedThisFrame || padStatus.m_buttonsCurrentFrame.m_z;
            padStatus.m_buttonsCurrentFrame.m_z = 0;
            padStatus.m_buttonsCurrentFrame2.m_z = 0;
            padStatus.m_buttonsPressedThisFrame.m_z = 0;
            padStatus.m_buttonsPressedThisFrame2.m_z = 0;
            break;
        case FAB_L:
            _btnPressedThisFrame = _btnPressedThisFrame || padStatus.m_buttonsCurrentFrame.m_l;
            padStatus.m_buttonsCurrentFrame.m_l = 0;
            padStatus.m_buttonsCurrentFrame2.m_l = 0;
            padStatus.m_buttonsPressedThisFrame.m_l = 0;
            padStatus.m_buttonsPressedThisFrame2.m_l = 0;
            padStatus.m_lTriggerAnalog = 0;
            break;
        case FAB_R:
            _btnPressedThisFrame = _btnPressedThisFrame || padStatus.m_buttonsCurrentFrame.m_r;
            padStatus.m_buttonsCurrentFrame.m_r = 0;
            padStatus.m_buttonsCurrentFrame2.m_r = 0;
            padStatus.m_buttonsPressedThisFrame.m_r = 0;
            padStatus.m_buttonsPressedThisFrame2.m_r = 0;
            padStatus.m_rTriggerAnalog = 0;
            break;
    }
}

void FrameAdvanceModule::process() {
    if (!this->isRunning()) {
        this->paused = false;
        return;
    }

    if (_btnPressedThisFrame) {
        this->btnHeldDuration += 1;

        /* Three outcomes:
        - it's the first frame of the press              = Run
        - it's been held through the frame repeat delay  = Run
        - it's been pressed but not held long enough     = Pause
        */

        if (this->btnHeldDuration == 1) {
            this->paused = false;
        } else if (this->btnHeldDuration > GlobalSettings::frameAdvanceRepeatDelayFrames) {
            this->paused = false;
        } else {
            this->paused = true;
        }

    } else {
        this->btnHeldDuration = 0;
        this->paused = true;
    }

    this->_btnPressedThisFrame = false;
};

}
#include "pp/frame_advance.h"

#include "pp/input/pad.h"
#include "pp/global_settings.h"
#include "pp/common.h"
#include "pp/ui.h"

namespace PP {

FrameAdvanceModule g_frameAdvance = FrameAdvanceModule();

const char* frameAdvanceButtonOptions[] = { "Z", "L", "R" };

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
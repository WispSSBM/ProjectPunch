#pragma once

#include <gf/gf_pad_status.h>

#define PP_FAB_OPT_COUNT 3

namespace PP {
enum FrameAdvanceButton {
    FAB_Z = 0,
    FAB_L = 1,
    FAB_R = 2
};

extern const char* frameAdvanceButtonOptions[];

class FrameAdvanceModule {
public:
    FrameAdvanceModule() {
        btnHeldDuration = 0;
        paused = false;
        overpauseDelay = 5;
    }

    bool isRunning();
    void handleInput(gfPadStatus& padStatus);
    // Called every frame
    void process();
    void unpause();

    int btnHeldDuration;
    int overpauseDelay;
    bool paused;
private:
    // Maybe set to true on handleInput, set to false during process()
    bool _btnPressedThisFrame;
};

extern FrameAdvanceModule g_frameAdvance;

} // namespace PP
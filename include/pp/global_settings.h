#pragma once

#include "pp/frame_advance.h"

#define PP_SETTINGS_FILE_VERSION 1
#define PP_SETTINGS_FILE_PATH "sd:/Project+/projectpunch_settings.bin"

namespace PP {

class PlayerSettings {
public:
    PlayerSettings();

    bool showOnHitAdvantage;
    bool showOnShieldAdvantage;
    bool showActOutOfLag;
    bool showFighterState;
    bool enableWaitOverlay;
    bool enableDashOverlay;
    bool enableIasaOverlay;
    bool enableLedgeTechGalintPopup;
    bool enableLedgeTechFrameDisplay;
    bool enableLedgeTechFramesOnLedgePopup;
    bool enableLedgeTechAirdodgeAngle;

    void print();
};

class GlobalSettings {
public:
    GlobalSettings();

    int maxOsdLimit;
    int maxLedgedashVizFrames;
    bool enableFrameAdvance;
    FrameAdvanceButton frameAdvanceButton;
    int frameAdvanceRepeatDelayFrames;
    bool enableInvisibleTechs;
    int shieldActionabilityTolerance;

    PlayerSettings playerSettings[4];

    bool readGlobalSettings(const char* filepath);
    bool writeGlobalSettings(const char* filepath);
    void print();
};

extern GlobalSettings globalSettings;

} // namespace PP
#include "pp/global_settings.h"

#include "gf/gf_file_io_handle.h"
#include "OS/OSThread.h"

#include "pp/common.h"

namespace PP {

GlobalSettings::GlobalSettings() {
    this->maxOsdLimit = 3;
    this->maxLedgedashVizFrames = 50;
    this->enableFrameAdvance = false;
    this->frameAdvanceButton = FAB_Z;
    this->frameAdvanceRepeatDelayFrames = 15;
    this->enableInvisibleTechs = false;
    this->shieldActionabilityTolerance = 1;
}

PlayerSettings::PlayerSettings() {
    showOnHitAdvantage = false;
    showOnShieldAdvantage = false;
    showFighterState = false;
    showActOutOfLag = false;

    enableWaitOverlay = false;
    enableDashOverlay = false;
    enableIasaOverlay = false;
    enableLedgeTechFrameDisplay = false;
    enableLedgeTechFramesOnLedgePopup = false;
    enableLedgeTechGalintPopup = false;
    enableLedgeTechAirdodgeAngle = false;
}

bool GlobalSettings::readGlobalSettings(const char* filepath) {
    gfFileIOHandle handle;

    handle.checkFileRequest(filepath);
    while(!handle.isReady()) {
        OSSleepTicks(0, defaultSleepTicks());
    }

    if (handle.getReturnStatus() != 0) {
        OSReport("Project Punch settings file does not exist.\n");
        return false;
    }

    handle.read(filepath, Heaps::Tmp, 0);
    while(!handle.isReady()) {
        OSSleepTicks(0, defaultSleepTicks());
    }
    if (handle.getReturnStatus() != 0) {
        OSReport("Error reading Project Punch settings file: %d\n", handle.getReturnStatus());
        return false;
    }

    char* data = (char*)handle.getBuffer();
    size_t bufLen = handle.getSize();

    int* dataInts = (int*)data;
    int fileVersion = *dataInts++;
    if (fileVersion != PP_SETTINGS_FILE_VERSION) {
        OSReport("Ignoring PPunch Settings file with old version[%d] != %d", fileVersion, PP_SETTINGS_FILE_VERSION);
        return false;
    }
    if (bufLen < (0x20 * 5)) {
        OSReport("Ignoring PPunch Settings file that is too short.\n");
        return false;
    }

    this->maxOsdLimit = *dataInts++;
    this->maxLedgedashVizFrames = *dataInts++;
    this->frameAdvanceButton = (FrameAdvanceButton)*dataInts++;
    this->frameAdvanceRepeatDelayFrames = *dataInts++;
    this->shieldActionabilityTolerance = *dataInts++;

    bool* dataBools = (bool*)dataInts;
    this->enableFrameAdvance = *dataBools++;
    this->enableInvisibleTechs = *dataBools++;

    for(int i = 0; i < 4; i++) {
        dataBools = (bool*)data + (0x20 * (i+1));
        this->playerSettings[i].showOnHitAdvantage = *dataBools++;
        this->playerSettings[i].showOnShieldAdvantage = *dataBools++;
        this->playerSettings[i].showActOutOfLag = *dataBools++;
        this->playerSettings[i].showFighterState = *dataBools++;
        this->playerSettings[i].enableWaitOverlay = *dataBools++;
        this->playerSettings[i].enableDashOverlay = *dataBools++;
        this->playerSettings[i].enableIasaOverlay = *dataBools++;
        this->playerSettings[i].enableLedgeTechGalintPopup = *dataBools++;
        this->playerSettings[i].enableLedgeTechFrameDisplay = *dataBools++;
        this->playerSettings[i].enableLedgeTechFramesOnLedgePopup = *dataBools++;
        this->playerSettings[i].enableLedgeTechAirdodgeAngle = *dataBools++;
    }

    handle.release();

    OSReport("Successfully read settings file.\n");
    return true;
}

bool GlobalSettings::writeGlobalSettings(const char* filepath) {
    gfFileIOHandle handle;
    size_t bufferSize = 0x20 * 5;
    char buffer[0x20*5];
    int* intBufferPos = (int*)buffer;
    memset(buffer, 0xCC, bufferSize); // as sakurai intended.

    *intBufferPos++ = PP_SETTINGS_FILE_VERSION;
    *intBufferPos++ = this->maxOsdLimit;
    *intBufferPos++ = this->maxLedgedashVizFrames;
    *intBufferPos++ = (int)this->frameAdvanceButton;
    *intBufferPos++ = this->frameAdvanceRepeatDelayFrames;
    *intBufferPos++ = this->shieldActionabilityTolerance;

    bool* boolBufferPos = (bool*)intBufferPos;
    *boolBufferPos++ = this->enableFrameAdvance;
    *boolBufferPos++ = this->enableInvisibleTechs;

    u32 bytesInBuffer;
    if ((bytesInBuffer = (u32)boolBufferPos - (u32)buffer) > 0x20) {
        OSReport("Base PPunch Settings save too long: %d bytes\n", bytesInBuffer);
        return false;
    }

    for(int i = 0; i < 4; i++) {
        bool* boolBufferPosStart = (bool*)buffer + (0x20 * (i+1));
        boolBufferPos = boolBufferPosStart;
        *boolBufferPos++ = this->playerSettings[i].showOnHitAdvantage;
        *boolBufferPos++ = this->playerSettings[i].showOnShieldAdvantage;
        *boolBufferPos++ = this->playerSettings[i].showActOutOfLag;
        *boolBufferPos++ = this->playerSettings[i].showFighterState;
        *boolBufferPos++ = this->playerSettings[i].enableWaitOverlay;
        *boolBufferPos++ = this->playerSettings[i].enableDashOverlay;
        *boolBufferPos++ = this->playerSettings[i].enableIasaOverlay;
        *boolBufferPos++ = this->playerSettings[i].enableLedgeTechGalintPopup;
        *boolBufferPos++ = this->playerSettings[i].enableLedgeTechFrameDisplay;
        *boolBufferPos++ = this->playerSettings[i].enableLedgeTechFramesOnLedgePopup;
        *boolBufferPos++ = this->playerSettings[i].enableLedgeTechAirdodgeAngle;

        if ((bytesInBuffer = (u32)boolBufferPos - (u32)boolBufferPosStart) > 0x20) {
            OSReport("PPunch Settings save for player %d too long: %d bytes\n", i+1, bytesInBuffer);
            return false;
        }
    }

    handle.writeRequest(filepath, buffer, bufferSize, 0);
    while (!handle.isReady()) {
        OSSleepTicks(0, defaultSleepTicks());
    }
    return (handle.getReturnStatus() == 0);
}

void GlobalSettings::print() {
    OSReport("GlobalSettings:\n  maxOSD: %d maxLedgedash: %d frAdv: %d frAdvBtn: %d, frAdvDelay: %d, invisTechs: %d, onShieldDelay: %d\n",
        maxOsdLimit, maxLedgedashVizFrames, enableFrameAdvance, frameAdvanceButton, frameAdvanceRepeatDelayFrames,
        enableInvisibleTechs, shieldActionabilityTolerance
    );

    for (int i = 0; i < PP_MAX_PLAYERS; i++) {
        playerSettings[i].print();
    }
}

void PlayerSettings::print() {
    OSReport(
        "  PlayerOptions: \n\tonHit: %d onShield: %d ooLag: %d showState: %d waitOvr: %d dashOvr: %d iasaOvr: %d\n\t"
        "ltgalint: %d ltviz : %d ltlframes : %d ltairdodge : %d\n",
        showOnHitAdvantage, showOnShieldAdvantage, showActOutOfLag, showFighterState, enableWaitOverlay, enableDashOverlay, enableIasaOverlay,
        enableLedgeTechGalintPopup, enableLedgeTechFrameDisplay, enableLedgeTechFramesOnLedgePopup, enableLedgeTechAirdodgeAngle
    );
}

GlobalSettings globalSettings = GlobalSettings();

} // namespace PP::GlobalSettings
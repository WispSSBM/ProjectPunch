#pragma once

#include <so/anim_cmd/so_anim_cmd.h>

namespace ProjectPunch {

int snprint_anim_cmd(char* buffer, size_t len, const soAnimCmd& animCmd);
bool isLastAnimCmd(const soAnimCmd& cmd); 

namespace AnimCmdModuleNum{
enum AnimCmdModuleNum {
    CONTROL_FLOW = 0,
    soStatusModule = 0x2,
    soModelModule = 0x3,
    soMotionModule = 0x4,
    soPostureModule = 0x5,
    soCollisionModule = 0x6,
    ftControllerModule = 0x7,
    soGroundModule = 0x8,
    soSituationmodule = 0x9,
    soSoundModule = 0xA,
    soVisibilityModule = 0xB,
    ftFighter = 0xC, // This one is like... stuff innate to the character it seems.
    soAnimCmdModule = 0xD,
    soKineticModule = 0xE,
    soLinkModule = 0xF,
    soGenerateArticleManageModule = 0x10,
    soEffectModule = 0x11,
    soWorkManageModule = 0x12,
    soComboModule = 0x13,
    ftAreaModule = 0x14,
    soTerritoryModule = 0x15,
    soTargetSearchModule = 0x16,
    soPhysicsModule = 0x17,
    soSlopeModule = 0x18,
    soShadowModule = 0x19,
    soCameraModule = 0x1A,
    BRAWL = 0x1B, // This is module 27, which has most of brawl's innate stuff in it.
    ftStopMOdule = 0x1C,
    soShakeModule = 0x1D,
    soDamageModule = 0x1E,
    soItemManageModule = 0x1F,
    soTurnModule = 0x20,
    soColorBlendModule = 0x21,
    soTeamModule = 0x22,
    soSlowModule = 0x23,
    ftCancelModule = 0x64 // 100
};
} // modulenum namespace

namespace AnimCmdInterruptGroup {
enum AnimCmdInterruptGroup {
    GroundSpecial = 1,
    GroundItem = 2,
    GroundGrab = 3,
    GroundAttack = 4,
    GroundDodge = 5,
    GroundGuard = 6,
    GroundJump = 7,
    GroundMisc = 8,
    AirLand = 9,
    AirGrabLedge = 0xA,
    AirSpecial = 0xB,
    AirThrowItem = 0xC,
    AirTether = 0xD,
    AirDodge = 0xE,
    AirAttack = 0xF,
    AirFloat = 0x10,
    AirWalljump = 0x11,
    AirDoublejump = 0x12,
    FallThroughPlat = 0x13
};
}
} // namespace
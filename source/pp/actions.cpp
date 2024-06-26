#include "pp/actions.h"

namespace PP {
bool isAttackingAction(u16 action)
{
    if (action >= ACTION_JAB && action <= ACTION_AERIALATTACK)
    {
        /*
           Covers the following:
            JAB 0X24
            RAPIDJAB 0X25
            ATTACKDASH 0X26
            ATTACKS3 0X27
            ATTACKHI3 0X28
            ATTACKLW3 0X29
            ATTACKS4START 0X2A
            ATTACKS4HOLD 0X2B
            ATTACKS4S 0X2C
            ATTACKLW4START 0X2D
            ATTACKLW4HOLD 0X2E
            ATTACKLW4 0X2F
            ATTACKHI4START 0X30
            ATTACKHI4HOLD 0X31
            ATTACKHI4 0X32
            AERIALATTACK 0X33
        */
        return true;
    }
    if (action >= ACTION_SPECIALN && action <= ACTION_SPECIALLW)
    {
        /*
          Covers:
           SPECIALN 0X112
           SPECIALS 0X113
           SPECIALHI 0X114
           SPECIALLW 0X115
        */
        return true;
    }

    if (action >= ACTION_NEUTRALSWINGITEM and action <= ACTION_DASHSWINGITEM)
    {
        /*
          Covers:
           NEUTRALSWINGITEM 0X9E
           TILTSWINGITEM 0X9F
           SMASHSWINGITEMWINDUP 0XA0
           SMASHSWINGITEM 0XA1
           SMASHSWINGITEMCHARGE 0XA2
           DASHSWINGITEM 0XA3
        */
        return true;
    }

    switch (action)
    {
    case ACTION_FIRETETHER:
        return true;
    case ACTION_CATCHATTACK:
        return true;
    case ACTION_THROW:
        return true;
    default:
        return false;
    }
}

bool isDefinitelyActionable(u16 action) {
    switch (action) {
    case ACTION_WAIT:;
    case ACTION_WALK:;
    case ACTION_WALKBRAKE:;
    case ACTION_DASH:;
    case ACTION_GUARD:;
    case ACTION_GUARDOFF:;
    case ACTION_GUARDON:;
    case ACTION_SQUAT:;
    case ACTION_SQUATWAIT:;
    case ACTION_FALL:;
    case 0x122:; //Knuckles/Sanic spindash jump 
    case ACTION_JUMPSQUAT:
        return true;
    }

    return false;
}

const char* actionName(u16 action) {
    switch (action) {
    case ACTION_WAIT: return "Wait";
    case ACTION_WALK: return "Walk";
    case ACTION_WALKBRAKE: return "WalkBrake";
    case ACTION_DASH: return "Dash";
    case ACTION_RUN: return "Run";
    case ACTION_RUNBRAKE: return "RunBrake";
    case ACTION_TURN1: return "Turn1";
    case ACTION_TURN2: return "Turn2";
    case ACTION_TURNRUN: return "TurnRun";
    case ACTION_TURNRUNBRAKE: return "TurnRunBrake";
    case ACTION_JUMPSQUAT: return "JumpSquat";
    case ACTION_JUMP: return "Jump";
    case ACTION_JUMPAERIAL: return "JumpAerial";
    case ACTION_MULTIJUMP: return "Multijump";
    case ACTION_FALL: return "Fall";
    case ACTION_FALLAERIAL: return "FallAerial";
    case ACTION_FALLSPECIAL: return "FallSpecial";
    case ACTION_SQUAT: return "Squat";
    case ACTION_SQUATWAIT: return "SquatWait";
    case ACTION_SQUATF: return "SquatF";
    case ACTION_SQUATB: return "SquatB";
    case ACTION_SQUATRV: return "SquatRv";
    case ACTION_LANDINGHEAVY: return "LandingHeavy";
    case ACTION_LANDINGLIGHT: return "LandingLight";
    case ACTION_LANDINGLAGAERIALATTACK: return "LandingLagAerialAttack";
    case ACTION_LANDINGFALLSPECIAL: return "LandingFallSpecial";
    case ACTION_GUARDON: return "GuardOn";
    case ACTION_GUARD: return "Guard";
    case ACTION_GUARDOFF: return "GuardOff";
    case ACTION_GUARDDAMAGE: return "GuardDamage";
    case ACTION_ESCAPEN: return "EscapeN";
    case ACTION_ESCAPEF: return "EscapeF";
    case ACTION_ESCAPEB: return "EscapeB";
    case ACTION_ESCAPEAIR: return "EscapeAir";
    case ACTION_REBOUND: return "Rebound";
    case ACTION_REBOUND2: return "Rebound2";
    case ACTION_JAB: return "Jab";
    case ACTION_RAPIDJAB: return "RapidJab";
    case ACTION_ATTACKDASH: return "AttackDash";
    case ACTION_ATTACKS3: return "AttackS3";
    case ACTION_ATTACKHI3: return "AttackHi3";
    case ACTION_ATTACKLW3: return "AttackLw3";
    case ACTION_ATTACKS4START: return "AttackS4Start";
    case ACTION_ATTACKS4HOLD: return "AttackS4Hold";
    case ACTION_ATTACKS4S: return "AttackS4S";
    case ACTION_ATTACKLW4START: return "AttackLw4Start";
    case ACTION_ATTACKLW4HOLD: return "AttackLw4Hold";
    case ACTION_ATTACKLW4: return "AttackLw4";
    case ACTION_ATTACKHI4START: return "AttackHi4Start";
    case ACTION_ATTACKHI4HOLD: return "AttackHi4Hold";
    case ACTION_ATTACKHI4: return "AttackHi4";
    case ACTION_AERIALATTACK: return "AerialAttack";
    case ACTION_CATCH: return "Catch";
    case ACTION_CATCHPULL: return "CatchPull";
    case ACTION_CATCHDASH: return "CatchDash";
    case ACTION_CATCHWAIT1F: return "CatchWait1f";
    case ACTION_CATCHTURN: return "CatchTurn";
    case ACTION_CATCHWAIT: return "CatchWait";
    case ACTION_CATCHATTACK: return "CatchAttack";
    case ACTION_CATCHCUT: return "CatchCut";
    case ACTION_THROW: return "Throw";
    case ACTION_CAPTUREWAIT: return "CaptureWait";
    case ACTION_CAPTUREWAITHI: return "CaptureWaitHi";
    case ACTION_CAPTUREDAMAGE: return "CaptureDamage";
    case ACTION_CAPTURECUT: return "CaptureCut";
    case ACTION_CAPTUREJUMP: return "CaptureJump";
    case ACTION_THROWN: return "Thrown";
    case ACTION_GROUNDHIT: return "GroundHit";
    case ACTION_AIRHITLAND: return "AirHitLand";
    case ACTION_HITSTUN: return "Hitstun";
    case ACTION_HITGROUND: return "HitGround";
    case ACTION_HITWALL: return "HitWall";
    case ACTION_HITCEILING: return "HitCeiling";
    case ACTION_DAMAGEFALL: return "DamageFall";
    case ACTION_HITGROUNDTUMBLE: return "HitGroundTumble";
    case ACTION_HITWALLTUMBLE: return "HitWallTumble";
    case ACTION_HITCEILINGTUMBLE: return "HitCeilingTumble";
    case ACTION_DOWNWAIT: return "DownWait";
    case ACTION_DOWNEAT: return "DownEat";
    case ACTION_DOWNFOWARD: return "DownFoward";
    case ACTION_DOWNSTAND: return "DownStand";
    case ACTION_DOWNFORWARD: return "DownForward";
    case ACTION_DOWNATTACK: return "DownAttack";
    case ACTION_DOWNBOUND: return "DownBound";
    case ACTION_DOWNDAMAGE: return "DownDamage";
    case ACTION_SHIELDBREAK: return "ShieldBreak";
    case ACTION_SHIELDBREAKBOUND: case ACTION_SHIELDBREAKBOUND_2: return "ShieldBreakBound";
    case ACTION_FURAFURASTART: return "FuraFuraStart";
    case ACTION_FURAFURA: return "FuraFura";
    case ACTION_FURAFURAEND: return "FuraFuraEnd";
    case ACTION_FURASLEEPSTART: return "FuraSleepStart";
    case ACTION_FURASLEEPLOOP: return "FuraSleepLoop";
    case ACTION_FURASLEEPEND: return "FuraSleepEnd";
    case ACTION_DEKUNUTSTUN: return "DekuNutStun";
    case ACTION_PASSIVE: return "Passive";
    case ACTION_PASSIVESTAND: return "PassiveStand";
    case ACTION_PASSIVEWALL: return "PassiveWall";
    case ACTION_PASSIVEWALLJUMP: return "PassiveWallJump";
    case ACTION_PASSIVECEIL: return "PassiveCeil";
    case ACTION_RUNINTOWALL: return "RunIntoWall";
    case ACTION_HITHEADONWALL: return "HitHeadOnWall";
    case ACTION_WALLJUMP: return "Walljump";
    case ACTION_WALLCLINGSTART: return "WallclingStart";
    case ACTION_WALLCLINGWAIT: return "WallclingWait";
    case ACTION_WALLCLINGEND: return "WallclingEnd";
    case ACTION_WALLCLINGJUMP: return "WallclingJump";
    case ACTION_FOOTSTOOL: return "Footstool";
    case ACTION_GETFOOTSTOOLED: return "GetFootstooled";
    case ACTION_FOOTSTOOLSTAND: return "FootstoolStand";
    case ACTION_GETFOOTSTOOLEDAIRSTART: return "GetFootstooledAirStart";
    case ACTION_GETFOOTSTOOLED2: return "GetFootstooled2";
    case ACTION_PASS: return "Pass";
    case ACTION_CLIFFCATCHSTART: return "CliffCatchStart";
    case ACTION_CLIFFCATCH: return "CliffCatch";
    case ACTION_CLIFFWAIT: return "CliffWait";
    case ACTION_CLIFFATTACK: return "CliffAttack";
    case ACTION_CLIFFCLIMB: return "CliffClimb";
    case ACTION_CLIFFESCAPE: return "CliffEscape";
    case ACTION_CLIFFJUMPSTART: return "CliffJumpStart";
    case ACTION_CLIFFJUMPEND: return "CliffJumpEnd";
    case ACTION_OTTOTTO: return "Ottotto";
    case ACTION_OTTOTTOWAIT: return "OttottoWait";
    case ACTION_EDGESLIP: return "EdgeSlip";
    case ACTION_FIRETETHER: return "FireTether";
    case ACTION_TETHERAIMING: return "TetherAiming";
    case ACTION_TETHERGRABBINGSTAGE: return "TetherGrabbingStage";
    case ACTION_TETHERRISE: return "TetherRise";
    case ACTION_GLIDESTART: return "GlideStart";
    case ACTION_GLIDEDIRECTION: return "GlideDirection";
    case ACTION_GLIDELANDING: return "GlideLanding";
    case ACTION_GLIDEATTACK: return "GlideAttack";
    case ACTION_GLIDEEND: return "GlideEnd";
    case ACTION_SLIP: return "Slip";
    case ACTION_SLIPDOWN: return "SlipDown";
    case ACTION_SLIPTURN: return "SlipTurn";
    case ACTION_SLIPDASH: return "SlipDash";
    case ACTION_SLIPWAIT: return "SlipWait";
    case ACTION_SLIPSTAND: return "SlipStand";
    case ACTION_SLIPATTACK: return "SlipAttack";
    case ACTION_SLIPESCAPEF: return "SlipEscapeF";
    case ACTION_SLIPESCAPEB: return "SlipEscapeB";
    case ACTION_LADDERCLIMB: return "LadderClimb";
    case ACTION_LADDERWAIT: return "LadderWait";
    case ACTION_GRABITEM: return "GrabItem";
    case ACTION_GRABHEAVYITEM: return "GrabHeavyItem";
    case ACTION_THROWITEM: return "ThrowItem";
    case ACTION_THROWITEMRUN: return "ThrowItemRun";
    case ACTION_THROWHEAVYITEM: return "ThrowHeavyItem";
    case ACTION_NEUTRALSWINGITEM: return "NeutralSwingItem";
    case ACTION_TILTSWINGITEM: return "TiltSwingItem";
    case ACTION_SMASHSWINGITEMWINDUP: return "SmashSwingItemWindup";
    case ACTION_SMASHSWINGITEM: return "SmashSwingItem";
    case ACTION_SMASHSWINGITEMCHARGE: return "SmashSwingItemCharge";
    case ACTION_DASHSWINGITEM: return "DashSwingItem";
    case ACTION_ITEMSCREW: case ACTION_ITEMSCREW_2: return "ItemScrew";
    case ACTION_ITEMSCREWFALL: return "ItemScrewFall";
    case ACTION_ITEMASSIST: return "ItemAssist";
    case ACTION_SWIMFALL: return "SwimFall";
    case ACTION_SWIMRISE: return "SwimRise";
    case ACTION_SWIMDAMAGE: return "SwimDamage";
    case ACTION_SWIM: return "Swim";
    case ACTION_SWIMF: return "SwimF";
    case ACTION_SWIMEND: return "SwimEnd";
    case ACTION_SWIMTURN: return "SwimTurn";
    case ACTION_SWIMJUMPSQUAT: return "SwimJumpSquat";
    case ACTION_SWIMDROWN: return "SwimDrown";
    case ACTION_SWIMDROWNOUT: return "SwimDrownOut";
    case ACTION_DEATH: return "Death";
    case ACTION_RESPAWN: return "Respawn";
    case ACTION_SPRINGJUMP: return "SpringJump";
    case ACTION_EATENBYFISHONSUMMIT: return "EatenByFishOnSummit";
    case ACTION_CAPTURECHOMP: return "CaptureChomp";
    case ACTION_CAPTUREFALCONDIVE: return "CaptureFalconDive";
    case ACTION_BEINGINHALED: return "BeingInhaled";
    case ACTION_CAPTUREINHALE: return "CaptureInhale";
    case ACTION_CAPTUREMONKEYFLIP: return "CaptureMonkeyFlip";
    case ACTION_DARKDIVEGRABBED: return "DarkDiveGrabbed";
    case ACTION_FLAMECHOKEGROUNDGRABBED: return "FlameChokeGroundGrabbed";
    case ACTION_FLAMECHOKEAIRGRABBED: return "FlameChokeAirGrabbed";
    case ACTION_FLAMECHOKEAIRPLUMMETINGGRABBED: return "FlameChokeAirPlummetingGrabbed";
    case ACTION_FLAMECHOKESCRAMBLE: return "FlameChokeScramble";
    case ACTION_EGGIMMINENT: return "EggImminent";
    case ACTION_CAPTUREEGG: return "CaptureEgg";
    case ACTION_CAPTUREFLYINGSLAM: return "CaptureFlyingSlam";
    case ACTION_CAPTUREMASTERHAND: return "CaptureMasterHand";
    case ACTION_UNLOADED: return "Unloaded";
    case ACTION_APPEAL: return "Appeal";
    case ACTION_APPEALSMASH: return "AppealSmash";
    case ACTION_ENTRANCE: return "Entrance";
    case ACTION_VICTORY: return "Victory";
    case ACTION_LOSS: return "Loss";
    case ACTION_SPECIALN: return "SpecialN";
    case ACTION_SPECIALS: return "SpecialS";
    case ACTION_SPECIALHI: return "SpecialHi";
    case ACTION_SPECIALLW: return "SpecialLw";
    case ACTION_FINAL: return "Final";
    case ACTION_FUWAFUWA: return "FuwaFuwa";
    default: return "UNLABELED";
    }
}

bool isLandingAction(u16 action) {
    return (action >= 0x16 && action <= 0x19);
}

bool isEATBitExclusion(ftKind charKind, int actionId) {
    if (actionId == ACTION_JAB) {
        switch (charKind) {
            case Fighter_Ganon:;
            case Fighter_Marth:;
            case Fighter_Roy:;
                return true;
            default:
                return false;
        }
    }
    if (charKind == Fighter_ZeroSuitSamus) {
        switch (actionId) {
            // 117 and 118 are her uncharged/charged shot release actions.
            // They have action transition enabled the whole time, but she's not actionable.
        case ACTION_SPECIALN:;
        case 0x117:;
        case 0x118:;
            return true;
        default:
            return false;
        };
    } else if (charKind == Fighter_Squirtle) {
        switch (actionId) {
            // shell slide not immediately cancelable
            case ACTION_SPECIALS:;
                return true;
            default:
                return false;
        }
    } else if (charKind == Fighter_Sonic || Fighter_DixieKong /* Knuckles */) {
        // spindash not immediately cancelable
        switch (actionId) {
            case ACTION_SPECIALLW:;
                return true;
            default:
                return false;
        }
    } else {
        return false;
    }
}
}
#!/usr/bin/env python3
"""Generate the external Wii action-name table from the ACTIONS dictionary below.

Usage: python tools/pack_actions.py [output.bin]

Format (no header): big-endian u16 offsets[count], then ASCII names terminated
with NUL. Offsets are relative to the string blob; 0xFFFF means no name.
The loader must use the same count (currently 0x11F), and keep the file buffer
alive. Missing names should fall back to "UNLABELED", as actionName() does.
Only the standard library is required. This does not change the runtime code.
"""

import argparse
from pathlib import Path
import struct


ROOT = Path(__file__).resolve().parents[1]
MISSING = 0xFFFF


ACTIONS = {
    0x000: 'Wait',
    0x001: 'Walk',
    0x002: 'WalkBrake',
    0x003: 'Dash',
    0x004: 'Run',
    0x005: 'RunBrake',
    0x006: 'Turn1',
    0x007: 'Turn2',
    0x008: 'TurnRun',
    0x009: 'TurnRunBrake',
    0x00A: 'JumpSquat',
    0x00B: 'Jump',
    0x00C: 'JumpAerial',
    0x00D: 'Multijump',
    0x00E: 'Fall',
    0x00F: 'FallAerial',
    0x010: 'FallSpecial',
    0x011: 'Squat',
    0x012: 'SquatWait',
    0x013: 'SquatF',
    0x014: 'SquatB',
    0x015: 'SquatRv',
    0x016: 'LandingHeavy',
    0x017: 'LandingLight',
    0x018: 'LandingLagAerialAttack',
    0x019: 'LandingFallSpecial',
    0x01A: 'GuardOn',
    0x01B: 'Guard',
    0x01C: 'GuardOff',
    0x01D: 'GuardDamage',
    0x01E: 'EscapeN',
    0x01F: 'EscapeF',
    0x020: 'EscapeB',
    0x021: 'EscapeAir',
    0x022: 'Rebound',
    0x023: 'Rebound2',
    0x024: 'Jab',
    0x025: 'RapidJab',
    0x026: 'AttackDash',
    0x027: 'AttackS3',
    0x028: 'AttackHi3',
    0x029: 'AttackLw3',
    0x02A: 'AttackS4Start',
    0x02B: 'AttackS4Hold',
    0x02C: 'AttackS4S',
    0x02D: 'AttackLw4Start',
    0x02E: 'AttackLw4Hold',
    0x02F: 'AttackLw4',
    0x030: 'AttackHi4Start',
    0x031: 'AttackHi4Hold',
    0x032: 'AttackHi4',
    0x033: 'AerialAttack',
    0x034: 'Catch',
    0x035: 'CatchPull',
    0x036: 'CatchDash',
    0x037: 'CatchWait1f',
    0x038: 'CatchTurn',
    0x039: 'CatchWait',
    0x03A: 'CatchAttack',
    0x03B: 'CatchCut',
    0x03C: 'Throw',
    0x03D: 'CaptureWait',
    0x03E: 'CaptureWaitHi',
    0x03F: 'CaptureDamage',
    0x040: 'CaptureCut',
    0x041: 'CaptureJump',
    0x042: 'Thrown',
    0x043: 'GroundHit',
    0x044: 'AirHitLand',
    0x045: 'Hitstun',
    0x046: 'HitGround',
    0x047: 'HitWall',
    0x048: 'HitCeiling',
    0x049: 'DamageFall',
    0x04A: 'HitGroundTumble',
    0x04B: 'HitWallTumble',
    0x04C: 'HitCeilingTumble',
    0x04D: 'DownWait',
    0x04F: 'DownEat',
    0x050: 'DownFoward',
    0x051: 'DownStand',
    0x052: 'DownForward',
    0x053: 'DownAttack',
    0x054: 'DownBound',
    0x055: 'DownDamage',
    0x056: 'ShieldBreak',
    0x057: 'ShieldBreakBound',
    0x058: 'ShieldBreakBound',
    0x059: 'FuraFuraStart',
    0x05A: 'FuraFura',
    0x05B: 'FuraFuraEnd',
    0x05C: 'FuraSleepStart',
    0x05D: 'FuraSleepLoop',
    0x05E: 'FuraSleepEnd',
    0x05F: 'DekuNutStun',
    0x060: 'Passive',
    0x061: 'PassiveStand',
    0x062: 'PassiveWall',
    0x063: 'PassiveWallJump',
    0x064: 'PassiveCeil',
    0x065: 'RunIntoWall',
    0x066: 'HitHeadOnWall',
    0x067: 'Walljump',
    0x068: 'WallclingStart',
    0x069: 'WallclingWait',
    0x06A: 'WallclingEnd',
    0x06C: 'WallclingJump',
    0x06D: 'Footstool',
    0x06E: 'GetFootstooled',
    0x06F: 'FootstoolStand',
    0x070: 'GetFootstooledAirStart',
    0x071: 'GetFootstooled2',
    0x072: 'Pass',
    0x073: 'CliffCatchStart',
    0x074: 'CliffCatch',
    0x075: 'CliffWait',
    0x076: 'CliffAttack',
    0x077: 'CliffClimb',
    0x078: 'CliffEscape',
    0x079: 'CliffJumpStart',
    0x07A: 'CliffJumpEnd',
    0x07C: 'Ottotto',
    0x07D: 'OttottoWait',
    0x07E: 'EdgeSlip',
    0x07F: 'FireTether',
    0x080: 'TetherAiming',
    0x081: 'TetherGrabbingStage',
    0x082: 'TetherRise',
    0x084: 'GlideStart',
    0x085: 'GlideDirection',
    0x086: 'GlideLanding',
    0x087: 'GlideAttack',
    0x088: 'GlideEnd',
    0x089: 'Slip',
    0x08A: 'SlipDown',
    0x08B: 'SlipTurn',
    0x08C: 'SlipDash',
    0x08D: 'SlipWait',
    0x08E: 'SlipStand',
    0x08F: 'SlipAttack',
    0x090: 'SlipEscapeF',
    0x091: 'SlipEscapeB',
    0x092: 'LadderClimb',
    0x094: 'LadderWait',
    0x096: 'GrabItem',
    0x097: 'GrabHeavyItem',
    0x09B: 'ThrowItem',
    0x09C: 'ThrowItemRun',
    0x09D: 'ThrowHeavyItem',
    0x09E: 'NeutralSwingItem',
    0x09F: 'TiltSwingItem',
    0x0A0: 'SmashSwingItemWindup',
    0x0A1: 'SmashSwingItem',
    0x0A2: 'SmashSwingItemCharge',
    0x0A3: 'DashSwingItem',
    0x0AF: 'ItemScrew',
    0x0B0: 'ItemScrew',
    0x0B1: 'ItemScrewFall',
    0x0B2: 'ItemAssist',
    0x0B3: 'SwimFall',
    0x0B4: 'SwimRise',
    0x0B5: 'SwimDamage',
    0x0B6: 'Swim',
    0x0B7: 'SwimF',
    0x0B8: 'SwimEnd',
    0x0B9: 'SwimTurn',
    0x0BA: 'SwimJumpSquat',
    0x0BB: 'SwimDrown',
    0x0BC: 'SwimDrownOut',
    0x0BD: 'Death',
    0x0BE: 'Respawn',
    0x0C4: 'SpringJump',
    0x0C6: 'EatenByFishOnSummit',
    0x0CC: 'CaptureChomp',
    0x0CF: 'CaptureFalconDive',
    0x0D0: 'BeingInhaled',
    0x0D1: 'CaptureInhale',
    0x0D6: 'CaptureMonkeyFlip',
    0x0E6: 'DarkDiveGrabbed',
    0x0E7: 'FlameChokeGroundGrabbed',
    0x0E8: 'FlameChokeAirGrabbed',
    0x0E9: 'FlameChokeAirPlummetingGrabbed',
    0x0EA: 'FlameChokeScramble',
    0x0EB: 'EggImminent',
    0x0EC: 'CaptureEgg',
    0x0EE: 'CaptureFlyingSlam',
    0x0FA: 'CaptureMasterHand',
    0x10B: 'Unloaded',
    0x10C: 'Appeal',
    0x10D: 'AppealSmash',
    0x10E: 'Entrance',
    0x10F: 'Victory',
    0x110: 'Loss',
    0x112: 'SpecialN',
    0x113: 'SpecialS',
    0x114: 'SpecialHi',
    0x115: 'SpecialLw',
    0x116: 'Final',
    0x11E: 'FuwaFuwa',
}


def pack_names(names, count):
    """Build a deterministic dense table, reusing offsets for identical names."""
    if not 1 <= count <= 0x10000:
        raise ValueError("Table count must be between 1 and 65536")
    offsets = [MISSING] * count
    strings = bytearray()
    shared = {}
    for action, name in sorted(names.items()):
        if not 0 <= action < count:
            raise ValueError(f"Action ID {action:#x} does not fit table count {count:#x}")
        encoded = name.encode('ascii') + b'\0'
        if b'\0' in encoded[:-1]:
            raise ValueError(f"Embedded NUL in name for action {action:#x}")
        if name not in shared:
            if len(strings) + len(encoded) > MISSING:
                raise ValueError("String blob exceeds 65535 bytes")
            shared[name] = len(strings)
            strings.extend(encoded)
        offsets[action] = shared[name]
    return struct.pack(f'>{count}H', *offsets) + strings


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('output', nargs='?', type=Path, default=ROOT / 'tools/actions.bin')
    args = parser.parse_args()
    try:
        count = max(ACTIONS) + 1
        data = pack_names(ACTIONS, count)
        args.output.parent.mkdir(parents=True, exist_ok=True)
        args.output.write_bytes(data)
    except (OSError, ValueError) as error:
        parser.exit(1, f'Error: {error}\n')
    print(f'Wrote {args.output}: {len(ACTIONS)} named IDs, {count:#x} entries, '
          f'{count * 2} index bytes + {len(data) - count * 2} string bytes = {len(data)} bytes')


if __name__ == '__main__':
    main()

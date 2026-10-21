#include "pp/actions.h"
#include "pp/common.h"
#include "pp/utils.h"

#include <gf/gf_file_io_handle.h>
#include <OS/OSThread.h>

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

namespace {
// Must match tools/pack_actions.py's default table length, including ID 0x11E.
const u16 ACTION_NAME_COUNT = 0x11F;
const u16 NO_ACTION_NAME = 0xFFFF;
const char ACTION_NAMES_PATH[] = "sd://pf/ppunch/actions.bin";

u8 actionsLoaded = 0;
const u16* actionNameOffsets = NULL;
const char* actionNameStrings = NULL;
gfFileIOHandle* actionNameFile = NULL;
}

void loadActionNames(const char* filename) {
    if (actionsLoaded) { return; }
    actionNameFile = readFileBlocking(filename, Heaps::Tmp, 0);
    if (actionNameFile != NULL) {
        actionsLoaded = 1;
    }


    const char* data = static_cast<const char*>(actionNameFile->getBuffer());
    const int size = actionNameFile->getSize();
    const int indexSize = ACTION_NAME_COUNT * sizeof(u16);
    if (data == NULL || size <= indexSize || size > indexSize + 0xFFFF) {
        actionNameFile->release();
        actionsLoaded = 0;
        return;
    }

    // Wii/PPC reads the generator's big-endian u16 offsets directly.
    const u16* offsets = reinterpret_cast<const u16*>(data);
    const char* strings = data + indexSize;
    const int stringSize = size - indexSize;
    // A final NUL guarantees every in-bounds offset reaches a terminator.
    if (strings[stringSize - 1] != '\0') {
        actionNameFile->release();
        actionsLoaded = 0;
        return;
    }
    for (u16 i = 0; i < ACTION_NAME_COUNT; ++i) {
        if (offsets[i] != NO_ACTION_NAME && offsets[i] >= stringSize) {
            actionNameFile->release();
            actionsLoaded = 0;
            return;
        }
    }

    actionNameStrings = strings;
    actionNameOffsets = offsets;
    return;
}

const char* actionName(u16 action) {
    if (actionNameOffsets == NULL || action >= ACTION_NAME_COUNT ||
        actionNameOffsets[action] == NO_ACTION_NAME) {
        return "UNK";
    }
    return actionNameStrings + actionNameOffsets[action];
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

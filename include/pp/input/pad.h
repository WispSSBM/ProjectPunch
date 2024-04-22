#pragma once

#include <types.h>

namespace PP {
namespace Input {

union PadButtons {
    PadButtons() { bits = 0; }
    PadButtons(u16 bits) {
        this->bits = bits;
    }

    u16 bits;
    struct {
        u16 _none: 3;
        u16 Start: 1;
        u16 Y: 1;
        u16 X: 1;
        u16 B: 1;
        u16 A: 1;
        u16 _none2: 1;
        u16 L: 1;
        u16 R: 1;
        u16 Z: 1;
        u16 UpDPad: 1;
        u16 DownDPad: 1;
        u16 RightDPad: 1;
        u16 LeftDPad: 1;
    };
};

class PadStatus {
public:
    PadButtons btns;

    s8  stickX;
    s8  stickY;
    s8  substickX;
    s8  substickY;
    u8  triggerLeft;
    u8  triggerRight;
    u8  analogA;
    u8  analogB;
    s8  err;
};

// This is the direct memory that the controller polling thread writes to.
// It may only work for GCC.
extern volatile const PadStatus g_padStatus[4];

}} // namespace
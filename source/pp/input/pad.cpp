#include "pp/input/pad.h"

namespace PP {
namespace Input {

PadButtons btnsFromAllPads() {
    PadButtons buttons;        

    for (int i = 0; i < 4; i++) {
        buttons.bits |= g_padStatus[i].btns.bits;
    }

    return buttons;
}

}} // namespace PP::Input
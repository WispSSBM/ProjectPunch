#include <types.h>
#include "pp/common.h"

/********/
// Universal Externs. This is imported by common.h so it is usable throughout.

namespace ProjectPunch {
u32 frameCounter = 0;
char strManipBuffer[PP_STR_MANIP_SIZE] = {};
const Color& COLOR_BLACK = 0x000000FFu;
const Color& COLOR_WHITE = 0xFFFFFFFFu;
const Color& COLOR_TRANSPARENT_GREY = 0x000000DD;

/******/

float fmax(float x1, float x2) {
    if (x1 > x2) return x1;
    return x2;
}

int max(int x1, int x2) {
    if (x1 > x2) return x1;
    return x2;
}

Color applyAlpha(Color input, u8 alpha) {
    float alphaRatio = ((float)alpha / 255);
    input.a *= (float)alpha / (float)255;
    return input;
}
}
#include <types.h>
#include "pp/common.h"

/********/
// Universal Externs. This is imported by common.h so it is usable throughout.

namespace ProjectPunch {
u32 frameCounter = 0;
char strManipBuffer[PP_STR_MANIP_SIZE] = {};

/******/

float fmax(float x1, float x2) {
    if (x1 > x2) return x1;
    return x2;
}

int max(int x1, int x2) {
    if (x1 > x2) return x1;
    return x2;
}

GXColor applyAlpha(GXColor input, u8 alpha) {
    float alphaRatio = ((float)alpha / 255);
    input.a *= (float)alpha / (float)255;
    return input;
}
}
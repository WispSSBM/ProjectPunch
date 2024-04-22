#include <OS/OSError.h>
#include <types.h>
#include "pp/common.h"

/********/
// Universal Externs. This is imported by common.h so it is usable throughout.

namespace ProjectPunch {
/* TODO: Look into using the game's native frame counter. */
u32 frameCounter = 0;
char strManipBuffer[PP_STR_MANIP_SIZE]; // char(*)[PP_STR_MANIP_SIZE];
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

Color Color::withAlpha(u8 alpha) const {
    Color outColor = *this;
    outColor.a *= ((float)alpha / (float)255);
    return outColor;
}

void Color::applyAlpha(u8 alpha) {
    this->a *= (float)alpha / (float)255;
}

void Color::debugPrint(const char* title = "") {
    OSReport("%s - r: 0x%0x g: 0x%0x b: 0x%0x a: 0x%0x\n", title, this->r, this->b, this->g, this->a);
}

//hacky way to check if in game
SCENE_TYPE getScene() {
    u32* ptr = (u32*) (0x805b4fd8 + 0xd4);
    ptr = (u32*) *ptr;
    if(ptr < (u32*)0xA0000000) {
        ptr = (u32*) *(ptr + (0x10 / 4));
        if(ptr != NULL) {
            u32 scene = *(ptr + (8 / 4));
            return (SCENE_TYPE)scene;
        }
    }
    return UNKNOWN;
}

}
#include <OS/OSError.h>
#include <types.h>
#include "pp/common.h"
#include <gf/gf_scene.h>
#include <cstring>
#include <math.h>

/********/
// Universal Externs. This is imported by common.h so it is usable throughout.

namespace PP {
/* TODO: Look into using the game's native frame counter. */
u32 frameCounter = 0;
u32 menuFrameCounter = 0;
char strManipBuffer[PP_STR_MANIP_SIZE]; // char(*)[PP_STR_MANIP_SIZE];

/******/

float fmax(float x1, float x2) {
    if (x1 > x2) return x1;
    return x2;
}

int max(int x1, int x2) {
    if (x1 > x2) return x1;
    return x2;
}

float fabs(float f) {
    return f < 0 ? (-1*f) : f;
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

Coord2D::Coord2D(const Coord2DF& other) {
    this->x = other.x;
    this->y = other.y;
}

float Coord2DF::radians() const {
    float tanVal; 
    if (x == 0) {
        if (y == 0) {
            tanVal = 0;
        } else if (y > 0) {
            tanVal = PP_M_PI_2;
        } else {
            tanVal = -PP_M_PI_2;
        }
    } else {
        tanVal = atan(y / x);
    }
    if (x >= 0) {
        if (y >= 0) {
            return tanVal;
        } else {
            return (M_PI*2) + tanVal;
        }
    } else {
        return M_PI + tanVal;
    }
}

size_t printBinaryString(char* buffer, int byte) {
    /* sizeof(buffer) should probably be at least 50. */
    char boolVal;
    char* start = buffer;

    for (int j = 0; j < 32; j++) { // one int's worth of data
        *(buffer++) = ((byte & (0x1 << j)) >> j) == 1 ? '1' : '0';

        if ((j % 8) == 7 && j != 31) {
            *(buffer++) = ' ';
        }
    }

    *(buffer--) = '\0';

    return buffer - start;
}

float Coord2DF::degrees() const {
    return radians() * (180/M_PI);
}

float Coord2DF::quadrantDegrees() const {
    return Coord2DF(fabs(x), fabs(y)).degrees();
}

bool startsWith(const char* testStr, const char* prefix) {
    while (*prefix != '\0') {
        if (*testStr == '\0') {
            return false;
        }

        if (*prefix != *testStr) {
            return false;
        }

        prefix++;
        testStr++;
    }
    return true;
}

//hacky way to check if in game
SCENE_TYPE getScene() {
    gfSceneManager* instance = gfSceneManager::getInstance();
    if (instance == NULL) { return UNKNOWN; }

    gfSequence* seq = instance->m_currentSequence;
    if (seq == NULL) { return UNKNOWN; }

    int mysteriousInt = *(((int*)seq) + 2);
    return (SCENE_TYPE)mysteriousInt;
}

}
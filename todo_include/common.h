#ifndef PP_COMMON_H
#define PP_COMMON_H

#include <stddef.h>
#include <Wii/GX/GX.h>

/* Static value defines */

#define PP_STR_MANIP_SIZE 512
#define COLOR_TRANSPARENT_GREY ((GXColor)0x000000DD)
#define COLOR_WHITE ((GXColor)0xFFFFFFFF)
#define PP_ACTION_NAME_LEN 64 
#define PP_FONT_SCALE_INC 0.1
#define PP_DEFAULT_MSG_X_POS 100
#define PP_DEFAULT_MSG_Y_POS 100
#define PP_DEFAULT_FONT_SCALE_X 0.5
#define PP_DEFAULT_FONT_SCALE_Y 0.7
#define PP_POPUP_VERTICAL_OFFSET 50
#define PP_MAX_PLAYERS 4

#define OSReport ((void (*)(const char* text, ...)) 0x801d8600)
#define sprintf ((int (*)(char* buffer, const char* format, ...)) 0x803f89fc)
#define snprintf ((int (*)(char* buffer, size_t maxSize, const char* format, ...)) 0x803f8924)

#define RA_BIT_ENABLE_ACTION_TRANSITION 0x10


typedef struct Coord2D {
    int x = 0;
    int y = 0;
} Coord2D;

typedef struct Coord2DF {
    float x = 0;
    float y = 0;
} Coord2DF;

enum SCENE_TYPE {
    MAIN_MENU = 0x1,
    HRC_CSS = 0x2,
    DIFFICULTY_TRAINING_CSS = 0x4,
    CSS_HRC = 0x5,
    DIFFICULTY_TRAINING_SSS_EVT_CSS = 0x6,
    SSS_BTT = 0x7,
    TRAINING_MODE_MMS = 0x8,
    VS = 0xA,
    UNKNOWN=0xFF
};

extern u32 frameCounter;
extern char strManipBuffer[PP_STR_MANIP_SIZE];

int max(int a, int b);
float fmax(float a, float b);
GXColor applyAlpha(GXColor input, u8 alpha);
#endif
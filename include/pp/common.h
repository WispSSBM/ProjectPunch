#pragma once

#include <types.h>
#include <GX/GXTypes.h>
#include <nw4r/ut/color.h>

/* Static value defines */

#define PP_STR_MANIP_SIZE 512
#define PP_ACTION_NAME_LEN 64 
#define PP_FONT_SCALE_INC 0.1
#define PP_DEFAULT_MSG_X_POS 100
#define PP_DEFAULT_MSG_Y_POS 100
#define PP_DEFAULT_FONT_SCALE_X 0.5
#define PP_DEFAULT_FONT_SCALE_Y 0.7
#define PP_POPUP_VERTICAL_OFFSET 50
#define PP_MAX_PLAYERS 4

#define WORK_MODULE_RA_VARS 1
#define WORK_MODULE_LA_VARS 0

#define RA_BIT_ENABLE_ACTION_TRANSITION 0x10
#define RA_BIT_ENABLE_LANDING_LAG 0x1E

// Re-using brawl-defined strings to save memory
extern "C" const char* g_strError;          // "ERROR STRING: %s\n"
extern "C" const char* g_strTypedError;     // "%s Error: %d\n"
namespace ProjectPunch {

    typedef struct Coord2D {
        Coord2D() {
            x = 0;
            y = 0;
        }

        Coord2D(int x, int y) {this->x = x; this -> y = y;}

        int x;
        int y;
    } Coord2D;

    typedef struct Coord2DF {
        Coord2DF() {
            x = 0;
            y = 0;
        }

        Coord2DF(float x, float y) {
            this->x = x;
            this -> y = y;
        }

        float x;
        float y;
    } Coord2DF;

    typedef union Color {
        Color() { this->value = 0; }
        Color(u32 val): value(val) {};
        Color(nw4r::ut::Color utC): utColor(utC) {};
        Color(GXColor gxC): gxColor(gxC) {};
        Color withAlpha(u8 alpha) const;
        void applyAlpha(u8 alpha);
        void debugPrint(const char* title);

        u32 value;
        nw4r::ut::Color utColor;
        GXColor gxColor;
        struct {
            u8 r;
            u8 g;
            u8 b;
            u8 a;
        };
    } Color;

    extern const Color& COLOR_BLACK;
    extern const Color& COLOR_WHITE;
    extern const Color& COLOR_TRANSPARENT_GREY;
    extern Color globalOverlayColor;


    #define SC_SEL_CHAR "scSelChar"
    #define SC_MELEE "scMelee"

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

    enum WorkModuleVarType {
        LA_VARS = 0,
        RA_VARS = 1
    };

    extern u32 frameCounter;
    extern char strManipBuffer[PP_STR_MANIP_SIZE];

    int max(int a, int b);
    float fmax(float a, float b);
    Color applyAlpha(Color input, u8 alpha);
    SCENE_TYPE getScene();
}
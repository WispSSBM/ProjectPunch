#pragma once

#include "pp/entry.h"
#include "pp/global_settings.h"

#include <StaticAssert.h>
#include <types.h>
#include <GX/GXTypes.h>
#include <nw4r/ut/color.h>
#include <gm/gm_global.h>
#include <OS/OSError.h>

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

#define PP_COLOR_NONE 0x00000000
#define PP_COLOR_BLACK 0x000000FFu
#define PP_COLOR_WHITE 0xFFFFFFFFu
#define PP_COLOR_TRANSPARENT_GREY 0x000000DD
#define PP_COLOR_GREY 0x444444FF
#define PP_COLOR_DARK_GREY 0x333333FF
#define PP_COLOR_GREEN 0x008A00FF
#define PP_COLOR_CYAN 0x40CCCCFF
#define PP_COLOR_YELLOW 0xFFFF00FF
#define PP_COLOR_RED 0xFF3333FF
#define PP_COLOR_BLUE 0x4242FFFF
#define PP_COLOR_ORANGE 0xCC8800FF
#define PP_COLOR_LIGHT_GREY 0xCCCCCCFF
#define PP_COLOR_MAGENTA 0xFF00FFBB

#define PP_CENTER_SCREEN_X 320

#define WORK_MODULE_RA_VARS 1
#define WORK_MODULE_LA_VARS 0

#define RA_BIT_ENABLE_ACTION_TRANSITION 0x10
#define RA_BIT_ENABLE_LANDING_LAG 0x1E
#define PP_M_PI_2		1.57079632679489661923
#define PP_M_PI_4		0.78539816339744830962

// Re-using brawl-defined strings to save memory
extern "C" const char* g_strError;          // "ERROR STRING: %s\n"
extern "C" const char* g_strTypedError;     // "%s Error: %d\n"
extern GameGlobal* g_GameGlobal;
namespace PP {

    struct Coord2DF;
    struct Coord2D {
        Coord2D() {
            x = 0;
            y = 0;
        }

        Coord2D(int x, int y) {this->x = x; this -> y = y;}
        Coord2D(const Coord2DF& other);

        int x;
        int y;
    };

    struct Coord2DF {
        Coord2DF() {
            x = 0;
            y = 0;
        }

        Coord2DF(float x, float y) {
            this->x = x;
            this -> y = y;
        }

        Coord2DF(const Coord2D& other) {
            this->x = other.x;
            this->y = other.y;
        }

        float degrees() const;
        float quadrantDegrees() const;
        float radians() const;

        float x;
        float y;
    };

    union Color {
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
    };

    typedef void (*void_ptr)();

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
    extern u32 menuFrameCounter;
    extern char strManipBuffer[PP_STR_MANIP_SIZE];

    int max(int a, int b);
    float fmax(float a, float b);
    float fabs(float f); 

    inline bool betweenIncl(int a, int b, int c) { return (a <= b && b <= c); }
    inline bool betweenExcl(int a, int b, int c) { return (a < b && b < c); }

    bool startsWith(const char* testStr, const char* prefix);
    size_t printBinaryString(char* buffer, int byte);
    SCENE_TYPE getScene();

    #ifdef PP_DEBUG_ALL
    #define PP_DEBUG_ANIMCMD
    #define PP_DEBUG_CTORS
    #define PP_DEBUG_INIT
    #define PP_DEBUG_FIGHTERS
    #define PP_DEBUG_OVERLAY
    #define PP_DEBUG_POPUPS
    #define PP_DEBUG_MENU
    #define PP_DEBUG_MEM
    #define PP_DEBUG_TEXT_PRINTER
    #define PP_DEBUG_LEDGETECH
    #define PP_DEBUG_INPUT
    #endif

    // #define PP_MENU_DISPLAY_DEBUG

    /* Conditional print functions */
    #ifdef PP_DEBUG_CTOR
    #define DEBUG_CTOR(...) (OSReport(__VA_ARGS__))
    #else
    #define DEBUG_CTOR(...)
    #endif

    #ifdef PP_DEBUG_OVERLAY
    #define DEBUG_OVERLAY(...) (OSReport(__VA_ARGS__))
    #else
    #define DEBUG_OVERLAY(...)
    #endif

    #ifdef PP_DEBUG_MENU
    #define DEBUG_MENU(...) (OSReport(__VA_ARGS__))
    #else
    #define DEBUG_MENU(...)
    #endif

    #ifdef PP_DEBUG_TEXT_PRINTER
    #define DEBUG_TEXT_PRINTER(...) (OSReport(__VA_ARGS__))
    #else
    #define DEBUG_TEXT_PRINTER(...)
    #endif

    #ifdef PP_DEBUG_ANIMCMD
    #define DEBUG_ANIMCMD(...) (OSReport(__VA_ARGS__))
    #else
    #define DEBUG_ANIMCMD(...)
    #endif

    #ifdef PP_DEBUG_INIT
    #define DEBUG_INIT(...) (OSReport(__VA_ARGS__))
    #else
    #define DEBUG_INIT(...)
    #endif

    #ifdef PP_DEBUG_FIGHTERS
    #define DEBUG_FIGHTERS(...) (OSReport(__VA_ARGS__))
    #else
    #define DEBUG_FIGHTERS(...)
    #endif

    #ifdef PP_DEBUG_POPUPS
    #define DEBUG_POPUPS(...) (OSReport(__VA_ARGS__))
    #else
    #define DEBUG_POPUPS(...)
    #endif

    #ifdef PP_DEBUG_LEDGETECH
    #define DEBUG_LEDGETECH(...) (OSReport(__VA_ARGS__))
    #else
    #define DEBUG_LEDGETECH(...)
    #endif

    #ifdef PP_DEBUG_INPUT
    #define DEBUT_INPUT(...) (OSReport(__VA_ARGS__))
    #else
    #define DEBUG_INPUT(...)
    #endif

}
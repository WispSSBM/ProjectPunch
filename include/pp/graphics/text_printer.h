#pragma once
//
// Created by dareb on 7/20/2020.
// ported to syriinge by wisp on 3/2024
//

#include <GX/GXTypes.h>
#include <OS/OSError.h>

#include <ms/ms_char_writer.h>
#include <vector.h>

#include "pp/common.h"

namespace ProjectPunch {
namespace Graphics {

struct TextPrinter {
    TextPrinter() {
        lineHeight = 20;
        is2D = false;
        renderPre = false;
        bboxIdx = 0;
        charWriter = new ms::CharWriter();
    };

    void print(const char* characters);
    void printLine(const char* characters);
    void padToWidth(float width);
    void newLine(bool fromPrintFn = false);
    void startBoundingBox();
    void saveBoundingBox(Color color, float boxPadding = 0);
    void saveBoundingBox(Color color, Color outlineColor, Color highlightColor, int outlineWidth, float boxPadding = 0);
    void setTextColor(Color color);
    void setup(bool is2D);

    ms::CharWriter* charWriter;
    float lineHeight;
    float maxWidth;
    float lineStart;
    float startY;
    float lastPadLocation;
    bool is2D; 
    bool renderPre;
    u32 bboxIdx;
};

//TODO: Maybe make these linker symbols instead of static defs

//These seem to be pointers to some currently loaded fonts
//These must be cleared and restored durign setup, or can crash
//crashes occur in between certain loads, like when a game ends
/*
#define _FONT_THING1 ((void**) 0x805a0174)
#define _FONT_THING2 ((void**) 0x805a0178)

#define _FONT_SCALE_THING ((double*) 0x805a1a80)
#define _FONT_SCALE_THING2 ((double*) 0x805a2bc0)

//pretty sure this is melee font, which is always loaded
*/
#define MELEE_FONT ((void*) 0x80497e44)


extern TextPrinter printer;

}
}
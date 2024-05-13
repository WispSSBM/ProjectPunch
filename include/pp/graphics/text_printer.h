#pragma once
//
// Created by dareb on 7/20/2020.
// ported to syriinge by wisp on 3/2024
//

#include <GX/GXTypes.h>
#include <OS/OSError.h>

#include <ms/ms_char_writer.h>

#include "pp/common.h"

namespace PP {
namespace Graphics {

struct TextPrinter {
    TextPrinter();
    virtual ~TextPrinter() { delete this->charWriter; }
    void print(const char* characters);
    void printf(const char* fmt, ...);
    void printLine(const char* characters);
    void newLine(bool fromPrintFn = false);

    // lifecycle stuff.
    void begin();
    void begin(bool is2D);
    void renderBoundingBox();
    void reset();

    void setTextColor(Color color);
    void setTextBorder(Color color, float width = 1.f);
    void setScale(Coord2DF scaleDims, float scale, float lineHeight);

    void setPosition(float x, float y);
    void setPosition(const Coord2DF& pos);
    void setMinWidth(float minWidth);
    float getPositionX() const;
    float getPositionY() const;
    Coord2DF getPosition() const;

    ms::CharWriter* charWriter;
    Coord2DF scale;
    float startY;
    float lineHeight;
    float maxWidth;
    float lineStart;
    float boxPadding;
    u8 opacity;
    Color textColor;
    Color textBorderColor;
    Color boxBgColor;
    Color boxBorderColor;
    Color boxHighlightColor;
    int boxBorderWidth;
    float textBorderWidth;
    bool is2D; 
    bool renderPre;
    u32 bboxIdx;

    private:
        void padToWidth(float width);
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
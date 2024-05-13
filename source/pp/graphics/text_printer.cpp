//
// Created by dareb on 7/20/2020.
// ported to syriinge by wisp on 3/2024.
//

#include "pp/graphics/text_printer.h"
#include "pp/graphics/draw.h"
#include "pp/graphics/drawable.h"
#include <cstdio>
#include <stdarg.h>
#include <OS/OSError.h>
#include <gf/gf_draw.h>

namespace PP {
namespace Graphics {

TextPrinter printer;

TextPrinter::TextPrinter(): 
    scale(1.f, 1.f), 
    textColor(0xFFFFFFFF), 
    textBorderColor(0x00000000),
    boxBgColor(0xCCCCCCFF),
    boxHighlightColor(0xFFFFFFCC)
{
    lineHeight = 20;
    is2D = false;
    renderPre = false;
    bboxIdx = 0;
    charWriter = new ms::CharWriter();
    // This is always loaded, but should work out how to do font loading without 
    // randomly assigning memory addresses.
    charWriter->m_font = MELEE_FONT;

    reset();
};

/* Info about fonts... */
// MELEE_FONT is always loaded, but we should work out how to do 
// font loading without just picking an address in memory and praying.

// other fonts include: 
// - font_latin12.arc
// - font_latin1.arc
// - font_watch.arc

// // BLANK (0) (enc: 3; fmt: )
// message->font = (FontData*) 0x80497d2c;
// OSReport("0 enc: %08x\n", message->font->GetGlyph());
// // MELEE STANDARD (1) / DEFAULT (enc: 0; fmt: )
// message->font = (FontData*) 0x80497d54;
// OSReport("1 enc: %08x\n", message->font->GetGlyph());
// // Hiranga Font (2) (starts @ 0x32) (USA Tags) (enc: 0; fmt: )
// message->font = (FontData*) 0x80497da4;
// OSReport("2 enc: %08x\n", message->font->GetGlyph());
// // Ending Font (3) (enc: 0; fmt: )
// message->font = (FontData*) 0x80497dcc;
// OSReport("3 enc: %08x\n", message->font->GetGlyph());
// // Melee Font (4) (MELEE MONO) (enc: 0; fmt: )
// message->font = (FontData*) 0x80497e44;
// OSReport("4 enc: %08x\n", message->font->GetGlyph());
// // Resant Font (5) (enc: 0; fmt: )
// message->font = (FontData*) 0x80497df4;
// OSReport("5 enc: %08x\n", message->font->GetGlyph());
// // Fox Font (6) (enc: 0; fmt: )
// message->font = (FontData*) 0x80497e1c;
// OSReport("6 enc: %08x\n", message->font->GetGlyph());
// // Alert Font(?) (7) (enc: 0; fmt: )
// message->font = (FontData*) 0x80497e6c;
// OSReport("7 enc: %08x\n", message->font->GetGlyph());
// // USA MAIN MENU (8) (enc: 0; fmt: )
// message->font = (FontData*) 0x80497d7c;
// OSReport("8 enc: %08x\n", message->font->GetGlyph());

//Must be called before doing anything after graphics settings were
//modified. Otherwise the game will likely hang and crash.
void TextPrinter::begin() {
    this->begin(true);
}

void TextPrinter::begin(bool is2D) {
    DEBUG_TEXT_PRINTER("TextPrinter::begin()\n");
    //clear 2D flag
    this->is2D = is2D;
    if (is2D) {
        // If we render at the end of the same frame we draw
        // the text, we wind up putting the bounding box on top
        // of it and you can't see the text.
        //
        // This assumes that renderPre is called at the beginning
        // of the frame and renderAll is called at the end of it,
        // but this setup is dependent on how the hook is structured.
        renderPre = true;
        gfDrawSetupCoord2D();
    } else {
        renderPre = false;
        startNormalDraw();
    }

    startY    = charWriter->GetCursorY();
    lineStart = charWriter->GetCursorX();
}

void TextPrinter::reset() {

    // This is always loaded, but should work out how to do font loading without 
    // randomly assigning memory addresses.
    charWriter->m_font = MELEE_FONT;

    opacity = 0xFF;
    textColor = 0xFFFFFFFF;
    textBorderColor = 0x00000000;
    boxBgColor = 0x000000FF;
    boxBorderColor = 0xCCCCCCFF;
    boxHighlightColor = 0xFFFFFFCC;
    boxBorderWidth = 2;
    textBorderWidth = 0;
    maxWidth = 0;
    lineStart = 0;
    boxPadding = 10;
}



void TextPrinter::printLine(const char *chars) {
    print(chars);
    newLine();
}

void TextPrinter::setTextColor(Color color) {
    #ifdef PP_DEBUG_TEXT_PRINTER
    color.debugPrint("TextPrinter::setTextColor");
    #endif
    this->textColor = color;
}

void TextPrinter::setTextBorder(Color color, float width) {
    #ifdef PP_DEBUG_TEXT_PRINTER
    DEBUG_TEXT_PRINTER("TextPrinter::setTextBorder width: %f\n", width);
    color.debugPrint("setTextBorder");
    #endif
    
    this->textBorderColor = color;
    this->textBorderWidth = width;
}

void TextPrinter::setScale(Coord2DF scaleDims, float multiplier, float lineHeightMult = 25) {
    DEBUG_TEXT_PRINTER("TextPrinter::setScale(scale=(%f, %f), mult=%f, lineMult=%f);\n", scaleDims.x, scaleDims.y, multiplier, lineHeightMult);
    this->scale.x = scaleDims.x * multiplier;
    this->scale.y = scaleDims.y * multiplier;
    this->lineHeight = this->scale.y * lineHeightMult;
}

void TextPrinter::setPosition(const Coord2DF& pos) {
    DEBUG_TEXT_PRINTER("TextPrinter::setPosition(%f, %f);\n", pos.x, pos.y);
    charWriter->SetCursor(pos.x, pos.y, 0);
}

void TextPrinter::setPosition(float x, float y) {
    DEBUG_TEXT_PRINTER("TextPrinter::setPosition(%f, %f);\n", x, y);
    charWriter->SetCursor(x, y, 0);
}

void TextPrinter::setMinWidth(float minWidth) {
    this->maxWidth = minWidth;
}

Coord2DF TextPrinter::getPosition() const {
    return Coord2DF(charWriter->m_xPos, charWriter->m_yPos);
}

float TextPrinter::getPositionX() const {
    return this->charWriter->GetCursorX();
}
float TextPrinter::getPositionY() const {
    return this->charWriter->GetCursorY();
}

void TextPrinter::printf(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vsnprintf(strManipBuffer, PP_STR_MANIP_SIZE, fmt, args);
    print(strManipBuffer);
    va_end(args);
}

void TextPrinter::print(const char *chars) {
    DEBUG_TEXT_PRINTER("TextPrinter::print: %s\n", chars);
    nw4r::ut::Color realTextColor = textColor.withAlpha(opacity).utColor;
    charWriter->SetTextColor(realTextColor);
    charWriter->SetScale(scale.x, scale.y);
    charWriter->SetEdge(textBorderWidth, textBorderColor.withAlpha(opacity).utColor);
    // charWriter->m_colorRect.m_topLeft = realTextColor;
    // charWriter->m_colorRect.m_topRight = realTextColor;
    // charWriter->m_colorRect.m_bottomLeft = realTextColor;
    // charWriter->m_colorRect.m_bottomRight = realTextColor;

    charWriter->SetupGX();


    for (; *chars; chars++) {
        if (*chars == '\n') {
            newLine(true);
        }
        else {
            charWriter->Print(*chars); // implictly converts char byte to UTF16 unsigned short.
        }
    }
}

void TextPrinter::newLine(bool fromPrintFn) {
    if (lineStart + maxWidth < charWriter->m_xPos) {
        maxWidth = charWriter->m_xPos - lineStart;
    }

    float y = charWriter->GetCursorY();
    charWriter->SetCursor(lineStart, y + lineHeight);
}

void TextPrinter::renderBoundingBox() {
    if (lineStart + maxWidth < charWriter->m_xPos) {
        maxWidth = charWriter->m_xPos - lineStart;
    }

    Rect * r = new Rect(
            0,
            1,
            boxBgColor.gxColor,
            startY - boxPadding,
            charWriter->m_yPos + lineHeight + boxPadding,
            lineStart - boxPadding,
            lineStart + maxWidth + boxPadding,
            is2D
    );

    // OSReport("Rect in (t, b, l, r): %.3f, %.3f, %.3f, %.3f\n", r.top, r.bottom, r.left, r.right);
    if (renderPre) renderables.items.preFrame.push(static_cast<Drawable*>(r));
    else renderables.items.frame.push(static_cast<Drawable*>(r));

    if (boxBorderWidth != 0) {
        RectOutline* ro = new RectOutline(
            0,
            1,
            boxBorderColor.gxColor,
            startY - boxPadding,
            charWriter->m_yPos + lineHeight + boxPadding,
            lineStart - boxPadding,
            lineStart + maxWidth + boxPadding,
            boxBorderWidth * 6,
            is2D
        );

        RectOutline* roHighlight = new RectOutline(
            0,
            1,
            boxHighlightColor.gxColor,
            startY - boxPadding,
            charWriter->m_yPos + lineHeight + boxPadding,
            lineStart - boxPadding,
            lineStart + maxWidth + boxPadding,
            (int)(((float)boxBorderWidth / 2.0f) * 6.0f),
            is2D
        );

        if (renderPre) renderables.items.preFrame.push(static_cast<Drawable*>(ro));
        else renderables.items.frame.push(static_cast<Drawable*>(ro));
        if (renderPre) renderables.items.preFrame.push(static_cast<Drawable*>(roHighlight));
        else renderables.items.frame.push(static_cast<Drawable*>(roHighlight));

        reset();
    }
}
}} // namespace
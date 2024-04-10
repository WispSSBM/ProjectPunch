//
// Created by dareb on 7/20/2020.
// ported to syriinge by wisp on 3/2024.
//

#include "pp/graphics/text_printer.h"
#include "pp/graphics/draw.h"
#include "pp/graphics/drawable.h"
#include <OS/OSError.h>
#include <GX/GXGeometry.h>
#include <gf/gf_draw.h>

namespace ProjectPunch {
namespace Graphics {

TextPrinter printer;

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
void TextPrinter::setup() {
    this->setup(true);
}

void TextPrinter::setup(bool is2D) {
    // This is always loaded, but should work out how to do font loading without 
    // randomly assigning memory addresses.
    charWriter->m_font = MELEE_FONT;

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

    startBoundingBox();
    charWriter->SetupGX();
    GXSetCullMode(GX_CULL_NONE);
}

void TextPrinter::printLine(const char *chars) {
    #ifdef PPUNCH_TEXTPRINTER_DEBUG
    const ms::CharWriter& cw = *charWriter;
    OSReport("DEBUG: textPrinter print (cw.x: %f, cw.y: %f, cw.scalex: %f, cw.scaley: %f: %s\n", 
        cw.m_xPos, cw.m_yPos,
        cw.m_fontScaleX, cw.m_fontScaleY,
        chars
    );
    #endif
    print(chars);
    newLine();
}

void TextPrinter::setTextColor(Color color) {
    charWriter->SetTextColor(color.utColor);
}

void TextPrinter::print(const char *chars) {
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
    charWriter->m_xPos = lineStart;
    lastPadLocation = lineStart;
    charWriter->m_yPos += (fromPrintFn && is2D) ? lineHeight : lineHeight;
}

void TextPrinter::startBoundingBox() {
    startY = charWriter->m_yPos;
    lineStart = charWriter->m_xPos;
    lastPadLocation = charWriter->m_xPos;
    maxWidth = 0;
}

void TextPrinter::saveBoundingBox(Color color, float boxPadding) {
    saveBoundingBox(color, COLOR_BLACK, COLOR_BLACK, 0, boxPadding);
}

void TextPrinter::saveBoundingBox(Color bgColor, Color outlineColor, Color highlightColor, int outlineWidth, float boxPadding) {
    if (lineStart + maxWidth < charWriter->m_xPos) {
        maxWidth = charWriter->m_xPos - lineStart;
    }

    int multiplier = 1;// (is2D) ? 1 : -1;
    Rect * r = new Rect(
            0,
            1,
            bgColor.gxColor,
            (startY - boxPadding) * multiplier,
            (charWriter->m_yPos + lineHeight + boxPadding) * multiplier,
            lineStart - boxPadding,
            lineStart + maxWidth + boxPadding,
            is2D
    );

    // OSReport("Rect in (t, b, l, r): %.3f, %.3f, %.3f, %.3f\n", r.top, r.bottom, r.left, r.right);
    if (renderPre) renderables.items.preFrame.push(static_cast<Drawable*>(r));
    else renderables.items.frame.push(static_cast<Drawable*>(r));

    if (outlineWidth != 0) {
        RectOutline* ro = new RectOutline(
            0,
            1,
            outlineColor.gxColor,
            (startY - boxPadding) * multiplier,
            (charWriter->m_yPos + lineHeight + boxPadding) * multiplier,
            lineStart - boxPadding,
            lineStart + maxWidth + boxPadding,
            outlineWidth * 6,
            is2D
        );

        RectOutline* roHighlight = new RectOutline(
            0,
            1,
            highlightColor.gxColor,
            (startY - boxPadding) * multiplier,
            (charWriter->m_yPos + lineHeight + boxPadding) * multiplier,
            lineStart - boxPadding,
            lineStart + maxWidth + boxPadding,
            (int)(((float)outlineWidth / 2.0f) * 6.0f),
            is2D
        );

        if (renderPre) renderables.items.preFrame.push(static_cast<Drawable*>(ro));
        else renderables.items.frame.push(static_cast<Drawable*>(ro));
        if (renderPre) renderables.items.preFrame.push(static_cast<Drawable*>(roHighlight));
        else renderables.items.frame.push(static_cast<Drawable*>(roHighlight));
    }
}

void TextPrinter::padToWidth(float width) {
    float newLocation = width + lastPadLocation;

    if (charWriter->m_xPos < newLocation) {
        charWriter->m_xPos = newLocation;
        lastPadLocation = newLocation;
    }
    else {
        lastPadLocation = charWriter->m_xPos;
    }
}

}} // namespace
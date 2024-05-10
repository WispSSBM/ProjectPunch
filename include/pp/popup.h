#pragma once

#include <cstring>
#include <cstdio>
#include <stdarg.h>

#include "pp/graphics/drawable.h"
#include "pp/graphics/text_printer.h" 
#include "pp/common.h"

namespace PP {

class Popup {
    public:
        Popup(const char* text, ...) {
            id = 0;
            fps = 60;
            durationSecs = 5;
            minWidth = 100;
            int textLen = 0;
            this->text = new char[256];

            va_list args;
            va_start(args, text);
            textLen = vsnprintf(this->text, 256, text, args);
            DEBUG_POPUPS("Showing popup [\n  %s] on frame %d\n", this->text, frameCounter);

            if (textLen >= 256 || textLen < 0) {
                this->text[255] = '\0';
            }
            va_end(args);

            startFrame = frameCounter;
        };

        ~Popup() { delete[] this->text; };
        void draw(Graphics::TextPrinter& printer);
        float percentElapsed();
        bool expired();

        u32 startFrame;
        u8 id;
        u8 fps;
        u16 durationSecs;
        u16 minWidth;
        Coord2D coords;
    private:
        char* text;
};

class PopupConfig {
    public:
        PopupConfig(): 
            textColor(0xFFFFFFFF), 
            bgColor(COLOR_TRANSPARENT_GREY),
            outlineColor(0x333333FF),
            highlightColor(0xBBBBBBFF)
        {
            if (g_GameGlobal->m_record->m_menuData.m_isWidescreen) {
                fontScale = Coord2DF(0.5112f, 0.7f);
            } else {
                fontScale = Coord2DF(0.65f, 0.65f);
            }
            popupPadding = 8;
            fontScaleMult = 1.f;
            lineHeightMult = 20;
        };

        Coord2DF fontScale;
        float fontScaleMult;
        int lineHeightMult;
        int popupPadding;
        Color textColor;
        Color bgColor;
        Color outlineColor;
        Color highlightColor;
};

extern PopupConfig gPopupConfig;
} // namespace
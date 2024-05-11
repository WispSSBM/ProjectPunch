#pragma once

#include <cstring>
#include <cstdio>
#include <stdarg.h>

#include "pp/collections/linkedlist.h"
#include "pp/graphics/drawable.h"
#include "pp/graphics/text_printer.h" 
#include "pp/common.h"

namespace PP {

class Popup {
    public:
        Popup() {
            id = 0;
            fps = 60;
            durationSecs = 5;
            minWidth = 100;
            int textLen = 0;
            this->text = new char[256];

            startFrame = frameCounter;
        };

        ~Popup() { delete[] this->text; };
        void printf(const char* fmt, ...);
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

extern Collections::linkedlist<Popup> playerPopups[PP_MAX_PLAYERS];
extern PopupConfig gPopupConfig;
} // namespace
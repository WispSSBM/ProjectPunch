#pragma once

#include <cstring>
#include "pp/graphics/drawable.h"
#include "pp/graphics/text_printer.h" 
#include "pp/common.h"

namespace ProjectPunch {

class Popup {
    public:
        Popup(const char* text) {
            id = 0;
            fps = 60;
            durationSecs = 5;
            minWidth = 100;
            this->text = new char[strlen(text)];
            startFrame = frameCounter;
            strcpy(this->text, text);
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
            highlightColor(0xBBBBBBFF),
            fontScale(0.5f, 0.7f) 
        {
            popupPadding = 10;
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
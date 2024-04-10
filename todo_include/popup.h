#ifndef PROJECTMCODES_POPUP_H
#define PROJECTMCODES_POPUP_H

#include <CLibs/cstring.h>
#include <Containers/vector.h>
#include <Graphics/Drawable.h>
#include <Graphics/TextPrinter.h>
#include <Brawl/Message.h>

#include "common.h"

struct PopupConfig;
extern PopupConfig gPopupConfig;

class Popup {
    public:
        Popup(const char* text);
        ~Popup();
        void draw(TextPrinter& printer);
        float percentElapsed();
        bool expired();

        u32 startFrame;
        u8 id = 0;
        u8 fps = 60;
        u16 durationSecs = 5;
        u16 minWidth = 100;
        Coord2D coords;
    private:
        Rect* progressRect;
        char* text;
};

struct PopupConfig {
     int popupPadding;
     GXColor bgColor;
     GXColor outlineColor;
     GXColor highlightColor;
};

void drawAllPopups(vector<Popup*>& popups, TextPrinter& printer, u32 currentFrame);

#endif
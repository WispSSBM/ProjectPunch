#pragma once

#include "pp/common.h"
#include "pp/menu/menu.h"

namespace PP {

#define PP_MENU_INPUT_SPEED 10
#define PP_MENU_BASE_INPUT_DELAY 15
#define PP_MENU_INPUT_SCALING 0.3
#define PP_MENU_INPUT_DELAY_MIN 2
class PpunchMenu : public Menu {
    public:
        PpunchMenu() {
            pos = Coord2D(100, 50);
            size = Coord2D(440, 350);

            padding = 25;
            outlineWidth = 4;
            lineHeightMultiplier = 25;

            if (g_GameGlobal->m_record->m_menuData.m_isWidescreen) {
                baseFontScale = Coord2DF(0.5, 0.7);
                titleBaseFontScale = Coord2DF(0.5, 1.0);
            } else {
                baseFontScale = Coord2DF(0.7f, 0.7f);
                titleBaseFontScale = Coord2DF(0.684f, 1.0f);
            }

            fontScaleMultiplier = 1.15;
            titleFontScaleMultiplier = 1.0;

            bgColor = 0x303030DD;  // dark grey
            highlightedColor = 0xEEE8AAFF; //ylw
            selectedColor = 0x75A5E2FF; // blu
            defaultColor = 0xFFFFFFFF; 
            outlineColor = 0x505050FF; // light grey
            highlightBoxColor = 0x000000FF;

            fixedHeight = true;
            initialized = false;

            upDebounce = 0;
            downDebounce = 0;
            leftDebounce = 0;
            rightDebounce = 0;
            upDebounceDelay = 0;
            downDebounceDelay = 0;
            leftDebounceDelay = 0;
            rightDebounceDelay = 0;
        };

        Coord2D pos;
        Coord2D size;
        Coord2DF baseFontScale;
        Coord2DF titleBaseFontScale;

        float fontScaleMultiplier;
        float titleFontScaleMultiplier;
        int lineHeightMultiplier;
        u8 outlineWidth;

        bool fixedHeight;
        u32 initialized;

        void init();
        void cleanup();
        void handleInput();
        virtual void render(TextPrinter& printer, char* buffer, u32 maxLen);
        void drawBg(TextPrinter& printer);
        void drawOutline(TextPrinter& printer);
        void drawHighlightBox();
        float lineHeight();

        bool isActive() { return this->visible && this->paused; };
    private:
        u32 lastInputFrame;
        u8 upDebounce;
        u8 downDebounce;
        u8 leftDebounce;
        u8 rightDebounce;
        u8 upDebounceDelay;
        u8 downDebounceDelay;
        u8 leftDebounceDelay;
        u8 rightDebounceDelay;
};

Coord2D getHpPopupBoxCoords(int playerNum);
void drawAllPopups();

extern PpunchMenu& punchMenu;

} // namespace

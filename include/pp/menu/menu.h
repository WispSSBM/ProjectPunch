#pragma once

#include "pp/menu/page.h"
#include "pp/common.h"
#include "pp/graphics/text_printer.h"

using namespace PP::Graphics;
using namespace PP::Collections;

namespace PP {

class Menu {
public:
    Menu() :
        highlightedColor(0xFFFFFFFF),
        selectedColor(0x3333FFFF),
        readOnlyColor(0xAAAAAAFF),
        defaultColor(0xCCCCCCFF),
        bgColor(0x555555CC),
        outlineColor(0x000000FF),
        highlightBoxColor(0x55FF55FF)
    {
        opacity = 0xFF;
        padding = 5.f;
        visible = false;
        paused = false;
        selected = false;
        currentPageIdx = -1;
    };
    virtual ~Menu() {
        clearPages();
    }

    void nextPage();
    void prevPage();
    void addPage(Page* p);
    Page* getCurrentPage();
    virtual void select();
    virtual void deselect();
    virtual void up();
    virtual void down();
    virtual void modify(float amount);
    virtual void render(TextPrinter* printer, char* buffer);
    virtual void unpause();
    virtual void toggle();
    virtual void clearPages();

    bool visible;
    bool paused;
    bool selected;
    vector pages;

    u8 opacity;
    Color highlightedColor;
    Color selectedColor;
    Color readOnlyColor;
    Color defaultColor;
    Color bgColor;
    Color outlineColor;
    Color highlightBoxColor;

    float padding;
protected:
    int currentPageIdx;
    friend class Page;
};

}
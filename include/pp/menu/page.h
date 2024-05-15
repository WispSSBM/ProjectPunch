#pragma once
#include <cstring>
#include <types.h>
#include "pp/menu/options/standard_option.h"
#include "pp/collections/vector.h"

namespace PP {

struct Menu;
struct Graphics::TextPrinter;

struct Page {
    Page(Menu* myMenu = NULL);

    virtual ~Page() {
        delete this->title;
        for (int i = 0; i < options.size(); i++) {
            delete reinterpret_cast<OptionType*>(options[i]);
        }
    }
    void addOption(OptionType* option);
    void hide();
    void up();
    void down();
    void modify(float amount);
    virtual void render(Graphics::TextPrinter* printer, char* buffer);
    void saveHighlightRegion(Graphics::TextPrinter* printer);
    OptionType* getCurrentOption();
    virtual void show();
    virtual void select();
    virtual void deselect();
    virtual bool isFullySelected() { return reinterpret_cast<OptionType*>(options[currentOption])->isFullySelected(); };
    virtual const char* getTitle();

    Collections::vector options;
    char currentOption;
    bool isSelected;
    float highlightedOptionTop;
    float highlightedOptionBottom;
    Menu* menu;

    char* title;
};

}
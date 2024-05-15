#pragma once

#include <types.h>
#include <OS/OSError.h>

#include "pp/graphics/text_printer.h"

#define RENDER_X_SPACING 80

// this is a completely arbitrary number I HIGHLY doubt anyone will need
// ever so it'll act as a default value
#define NUMERIC_DEFAULT -983746279

namespace PP {

struct Page;
struct SubpageOption;

struct OptionType {
    virtual void select() { isSelected = !isSelected; };
    virtual void deselect() { isSelected = false; };
    virtual void modify(float amount) { };
    virtual void render(Graphics::TextPrinter* printer, char* buffer) = 0;
    virtual void down() = 0;
    virtual void up() = 0;
    virtual void setParentPage(Page* p) = 0;
    virtual bool isScalarOption() { return true; };
    virtual bool isFullySelected() { return isSelected; };

    OptionType() {
        name = "";
        parent = NULL;
        subParent = NULL;
        isSelected = false;
        canModify = true;
        terminal = true;
    }

    virtual ~OptionType() {};
    const char* name;
    Page* parent;
    SubpageOption* subParent;
    bool isSelected;
    bool canModify;
    bool terminal;
};

struct StandardOption : public OptionType {
    virtual void up() { };
    virtual void down() { };
    virtual void setParentPage(Page* p) { 
        this->parent = p; 
    }
    virtual ~StandardOption() {}
};

}
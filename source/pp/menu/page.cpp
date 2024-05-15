#include "pp/menu/page.h"
#include "pp/menu/menu.h"
#include "pp/graphics/text_printer.h"

using namespace PP::Graphics;
namespace PP {

Page::Page(Menu* myMenu) {
    this->menu = myMenu;
    this->title = new char[256];
    currentOption = 0;
    isSelected = false;
    highlightedOptionBottom = 0.f;
    highlightedOptionTop = 0.f;
    strcpy("generic page", title);
};

void Page::addOption(OptionType* option) {
    option->setParentPage(this);
    options.push(dynamic_cast<OptionType*>(option));
}

void Page::deselect() {
    if (isSelected) {
        getCurrentOption()->deselect();
        isSelected = getCurrentOption()->isSelected;
    }
}

OptionType* Page::getCurrentOption() {
    return reinterpret_cast<OptionType*>(options[currentOption]);
}

void Page::select() {
    if (options.size() == 0) { return; }

    getCurrentOption()->select();
    isSelected = getCurrentOption()->isSelected;
}

void Page::up() {
    if (options.size() == 0) { return; }
    if (isSelected) getCurrentOption()->up();
    else if (currentOption > 0) {
        char start = currentOption;
        for (char i = --currentOption; i > 0 && !reinterpret_cast<OptionType*>(options[i])->canModify; i--) {
            currentOption--;
        }
        if (!getCurrentOption()->canModify) currentOption = start;
    }
}
void Page::down() {
    if (options.size() == 0) { return; }
    if (isSelected) getCurrentOption()->down();
    else if (currentOption < options.size() - 1) {
        char start = currentOption;
        char size = options.size();
        for (char i = ++currentOption; i < (size - 1) && !reinterpret_cast<OptionType*>(options[i])->canModify; i++) {
            currentOption++;
        }
        if (!getCurrentOption()->canModify) currentOption = start;
    }
}

void Page::modify(float amount) {
    if (options.size() == 0) { return; }
    if (!isSelected) {
        if (amount >= 0) {
            up();
        } else {
            down();
        }
        return;
    }
    
    getCurrentOption()->modify(amount);
}

void Page::render(Graphics::TextPrinter* printer, char* buffer) {
    Menu& m = *menu;
    char len = options.size();

    for (char i = 0; i < len; i++) {
        OptionType* option = reinterpret_cast<OptionType*>(options[i]);

        // For complex options, defer.
        if (!option->terminal) {
            option->render(printer, buffer);
            return;
        }

        if (!option->canModify) printer->setTextColor(m.readOnlyColor);
        else if (i == currentOption && isSelected && menu->paused) {
            saveHighlightRegion(printer);
            printer->setTextColor(m.selectedColor);
        }
        else if (i == currentOption && menu->paused) {
            saveHighlightRegion(printer);
            printer->setTextColor(m.highlightedColor);
        }
        else {
            printer->setTextColor(m.defaultColor);
        }
        option->render(printer, buffer);
    }
}


void Page::saveHighlightRegion(Graphics::TextPrinter* printer) {
        highlightedOptionTop = printer->charWriter->GetCursorY();
        highlightedOptionBottom = printer->charWriter->GetCursorY() + printer->lineHeight;
}

void Page::hide() {}
void Page::show() {}

const char* Page::getTitle() { return this->title; }

} // namespace
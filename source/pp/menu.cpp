// Wanton hoist of fudgepop's menu for my own purposes/modification

#include <stdio.h>
#include "pp/menu.h"
#include "pp/common.h"
#include "pp/graphics/drawable.h"
#include <OS/OSError.h>


namespace ProjectPunch {

#pragma region Page
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

void Page::render(TextPrinter* printer, char* buffer) {
    Menu& m = *menu;
    char len = options.size();

    OSReport("Page @ 0x%0x render(), %d options.\n", this, len);
    for (char i = 0; i < len; i++) {
        OptionType* option = reinterpret_cast<OptionType*>(options[i]);

        // For complex options, defer.
        if (!option->terminal) {
            option->render(printer, buffer);
            return;
        }

        if (!option->canModify) printer->setTextColor(applyAlpha(m.readOnlyColor, m.opacity));
        else if (i == currentOption && isSelected && menu->paused) {
            saveHighlightRegion(printer);
            printer->setTextColor(applyAlpha(m.selectedColor, m.opacity));
        }
        else if (i == currentOption && menu->paused) {
            saveHighlightRegion(printer);
            printer->setTextColor(applyAlpha(m.highlightedColor, m.opacity));
        }
        else {
            printer->setTextColor(applyAlpha(m.defaultColor, m.opacity));
        }
        printer->padToWidth(RENDER_X_SPACING / 5);

        OSReport("Rendering option %d @ 0x%0x\n", i, option);
        option->render(printer, buffer);
    }
}

void Page::hide() {}
void Page::show() {}

const char* Page::getTitle() { return this->title; }
#pragma endregion

#pragma region Menu
void Menu::addPage(Page* p) {
    p->menu = this;
    pages.push(p);
    if (currentPageIdx == -1) { currentPageIdx = 0; }
}

void Menu::nextPage() {
    currentPageIdx += 1;
    if (currentPageIdx == pages.size()) { currentPageIdx = 0; }
}

void Menu::prevPage() {
    currentPageIdx -= 1;
    if (currentPageIdx == -1) { currentPageIdx = pages.size() - 1; }
}

Page* Menu::getCurrentPage() {
    if (pages.size() == 0) { return NULL; }
    return reinterpret_cast<Page*>(pages[currentPageIdx]);
}

void Menu::select() { 
    getCurrentPage()->select(); 
    selected = getCurrentPage()->isFullySelected(); 
}

void Menu::deselect() {
    if (getCurrentPage()->isSelected) {
        getCurrentPage()->deselect();
    } else {
        visible = false;
        paused = false;
    }
}

void Menu::up() { 
    getCurrentPage()->up(); 
    selected = getCurrentPage()->isFullySelected();
}

void Menu::down() {
    getCurrentPage()->down();
    selected = getCurrentPage()->isFullySelected();
}

void Menu::modify(float amount) { getCurrentPage()->modify(amount); }

void Menu::render(TextPrinter* printer, char* buffer) {
    printer->startBoundingBox();
    char internalBuf[128];
    snprintf(internalBuf, 128, "Page 1 / %d", pages.size());
    printer->printLine(buffer);
    printer->printLine("");
    getCurrentPage()->render(printer, buffer);
    printer->saveBoundingBox(0, 0x222222FF | (opacity & 0xFF), 0x000000FF | (opacity & 0xFF), 2, 10);
}

void Menu::unpause() { paused = false; }
void Menu::toggle() {
    if (paused) {
        paused = false;
        visible = false;
    }
    else {
        paused = true;
        visible = true;
    }
}
#pragma endregion

#pragma region ModifiableScalars

void FloatOption::modify(float amount) {
    value += amount * changeMultiplier;
    if (max != NUMERIC_DEFAULT && value > max) value = min;
    else if (min != NUMERIC_DEFAULT && value < min) value = max;
}
void FloatOption::render(TextPrinter* printer, char* buffer) {
    sprintf(buffer, "%s: %.3f", name, value);
    printer->printLine(buffer);
}

void BoolOption::modify(float amount) {
    value = !value;
}
void BoolOption::render(TextPrinter* printer, char* buffer) {
    sprintf(buffer, "%s: %s", name, value ? "on" : "off");
    printer->printLine(buffer);
}
#pragma endregion

#pragma region ControlOption
void ControlOption::modify(float amount) {}
void ControlOption::select() {
    value = true;
    this->parent->menu->paused = false;
}
void ControlOption::deselect() {
    value = false;
    this->parent->menu->paused = true;
}
void ControlOption::render(TextPrinter* printer, char* buffer) {
    sprintf(buffer, "%s", name);
    printer->printLine(buffer);
}
#pragma endregion

#pragma region StringOption
void StringOption::modify(float amount) {}
void StringOption::render(TextPrinter* printer, char* buffer) {
    sprintf(buffer, "%s: %s", name, value);
    printer->printLine(buffer);
}
#pragma endregion

void LabelOption::render(TextPrinter* printer, char* buffer) {
    sprintf(buffer, "%s: %s", name, value);
    // Never disabled, it's a static label.
    printer->setTextColor(applyAlpha(parent->menu->defaultColor, parent->menu->opacity));
    printer->printLine(buffer);
}


#pragma region NamedIndexOption
void NamedIndexOption::modify(float amount) {}
void NamedIndexOption::select() {}
void NamedIndexOption::deselect() {}
void NamedIndexOption::render(TextPrinter* printer, char* buffer) {
    sprintf(buffer, "%s: %s", name, (0 <= index && index < arrayLength) ? nameArray[index] : " ");
    printer->printLine(buffer);
}
#pragma endregion

#pragma region HexObserver
void HexObserver::modify(float amount) {}
void HexObserver::render(TextPrinter* printer, char* buffer) {
    switch (size) {
    case CHAR:
        sprintf(buffer, "%s: 0x%02x", name, *value);
        break;
    case SHORT:
        sprintf(buffer, "%s: 0x%04x", name, *value);
        break;
    case INT:
        sprintf(buffer, "%s: 0x%08x", name, *value);
        break;
    }
    printer->printLine(buffer);
}
#pragma endregion

#pragma region SubpageOption
void SubpageOption::deselect() {
    /*
     * Either:
     *   - The subpage isn't selected, so nothing happens.
     *   - The subpage is selected + has a selected option, so deselect that option.
     *   - The subpage is selected and has no selected option, so deselect the subpage.
    */

    if (hasSelection) {
        OptionType& opt = currentOptionRef();
        opt.deselect();
        hasSelection = opt.isSelected;
    } else {
        isSelected = false;
        collapsed = true;
        this->parent->isSelected = false;
    }
}

void SubpageOption::select() {
    /*
     * Cases:
     *   - The subpage is not focused.
     *       Gain focus.
     *   - The subpage is focused, but is not pointing at a viable child opt.
     *       Close the subpage.
     *         (note, this should be the header)
     *   - The subpage is focused, and is pointing at a viable child opt.
     *       Toggle the focus of the child opt.
     */
    if (isSelected) {
        if (!hasCurrentOption()) {
            deselect();
        } else {
            OptionType& option = currentOptionRef(); 
             if (option.canModify) {
                currentOptionRef().select();
                hasSelection = option.isSelected;
             }
        } 
    } else {
        isSelected = true;
        collapsed = false;
    }
}

void SubpageOption::modify(float amount) {
    if (!hasSelection) {
        amount > 0 ? up() : down();
        return;
    }

    /* safeguard */
    if (currentOptionRef().canModify) {
        currentOptionRef().modify(amount);
    }
}

void SubpageOption::up() {
    /*
     * Cases:
     *   - We have a focused element, so pass the up() through.
     *   - We are at the header (currentOpt == -1)
     *        close the subpage and tell the parent page to go up. 
     *   - We are at a child option (opt >= 0) and have a modifiable above us:
     *        Go to that modifiable
     *   - We are at a child option (opt >= 0) and have no modifiable above us:
     *        Go to the header (-1)
     *
     */
    if (hasSelection){
        currentOptionRef().up();
        return;
    }

    if (currentOption < 0 || options.size() == 0) {
        deselect();
        parent->up();
        return;
    }

    bool hasFoundOpt = false;
    int start = currentOption;
    for (int i = --start; i >= 0; i--) {
        // Go to next 
        if (reinterpret_cast<OptionType*>(options[i])->canModify) {
            currentOption = i;
            hasFoundOpt = true;
            break;
        }
    }

    if (!hasFoundOpt) {
        // No more options north of us.
        currentOption = -1;
        return;
    }
}

void SubpageOption::down() {
    /*
     * Cases:
     *   - We have a selected child, so passthrough
     *   - We have a modifiable element below us, so go to that.
     *   - We have no more modifiable children, so close this subpage and tell our parent to go down.
     */
    if (hasSelection) {
        currentOptionRef().down();
        return;
    }

    if (currentOption >= (int)options.size()) {
        // We're at the bottom, so get out of here.
        deselect();
        parent->down();
        return;
    }

    int start = currentOption;
    bool foundOption = false;
    for (int i = ++start; i < (int)options.size(); i++) {
        if (reinterpret_cast<OptionType*>(options[i])->canModify) {
            currentOption = i;
            foundOption = true;
            break;
        }
    }

    if (!foundOption) {
        // unlike in up(), we don't have a footer or anything, so close the subpage.
        deselect();
        parent->down();
        return;
    }
}

void SubpageOption::render(TextPrinter* printer, char* buffer) {
    float oldXPos;
    int len = options.size();
    Menu& m = *(parent->menu);

    if (isSelected && currentOption == -1) {
        this->parent->saveHighlightRegion(printer);
        printer->setTextColor(applyAlpha(m.highlightedColor, m.opacity));
    }
    sprintf(buffer, (collapsible) ? ((isSelected) ? "- %s" : "+ %s") : "%s:", name);

    printer->printLine(buffer);

    if ((collapsible && !collapsed) || !(collapsible)) {
        for (int i = 0; i < options.size(); i++) {
            oldXPos = printer->charWriter->GetCursorX();
            printer->charWriter->m_xPos += (indent + ((indent * (depth + 1))));
            if (!reinterpret_cast<OptionType*>(options[i])->terminal) {
                reinterpret_cast<OptionType*>(options[i])->render(printer, buffer);
                continue;
            }


            if (!reinterpret_cast<OptionType*>(options[i])->canModify) {
                printer->setTextColor(applyAlpha(m.readOnlyColor, m.opacity));
            } else if (i == currentOption && hasSelection && m.paused) {
                parent->saveHighlightRegion(printer);
                printer->setTextColor(applyAlpha(m.selectedColor, m.opacity));
            } else if (i == currentOption && m.paused) {
                parent->saveHighlightRegion(printer);
                printer->setTextColor(applyAlpha(m.highlightedColor, m.opacity));
            } else {
                printer->setTextColor(applyAlpha(m.defaultColor, m.opacity));
            }

            reinterpret_cast<OptionType*>(options[i])->render(printer, buffer);
            printer->charWriter->SetCursorX(oldXPos);
        }
    }
}

void SubpageOption::addOption(OptionType* option) {
    option->parent = this->parent;
    option->subParent = this;
    options.push(reinterpret_cast<void*>(option));

    if (option->canModify) {
        modifiableChildren++;
    }
}

bool SubpageOption::isFullySelected() {
    if (hasSelection) {
        OptionType& opt = currentOptionRef();
        (opt.isScalarOption() ? opt.isSelected : opt.isFullySelected());
    }

    return false;
}

void SubpageOption::setParentPage(Page* p) {
    this->parent = p;
    int len = options.size();
    for (int i = 0; i < len; i++) { 
        OptionType* option = reinterpret_cast<OptionType*>(options[i]);
        OSReport ("Subpage option setting option %d @ (0x%0X) to have parent page 0x%0X\n", i, option, p);
        option->setParentPage(p); 
    };
}

void SubpageOption::clearOptions() {
    options.clear();
}

void SubpageOption::removeOptions() {
    options.reallocate(0);
    options.reallocate(1);
}

int SubpageOption::getOptionCount() {
    return options.size();
}

#pragma endregion

} // namespace
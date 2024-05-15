#include "pp/menu/options/subpage_option.h"

#include "pp/menu/menu.h"
#include "pp/menu/page.h"

using namespace PP::Graphics;
namespace PP {
    
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
        printer->setTextColor(m.highlightedColor);
    }
    printer->printf((collapsible) ? ((isSelected) ? "- %s\n" : "+ %s\n") : "%s:\n", name);

    if ((collapsible && !collapsed) || !(collapsible)) {
        for (int i = 0; i < options.size(); i++) {
            oldXPos = printer->charWriter->GetCursorX();
            printer->charWriter->m_xPos += (indent + ((indent * (depth + 1))));
            if (!reinterpret_cast<OptionType*>(options[i])->terminal) {
                reinterpret_cast<OptionType*>(options[i])->render(printer, buffer);
                continue;
            }


            if (!reinterpret_cast<OptionType*>(options[i])->canModify) {
                printer->setTextColor(m.readOnlyColor);
            } else if (i == currentOption && hasSelection && m.paused) {
                parent->saveHighlightRegion(printer);
                printer->setTextColor(m.selectedColor);
            } else if (i == currentOption && m.paused) {
                parent->saveHighlightRegion(printer);
                printer->setTextColor(m.highlightedColor);
            } else {
                printer->setTextColor(m.defaultColor);
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

}
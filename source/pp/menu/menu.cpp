#include "pp/menu/menu.h"

namespace PP {

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
        this->toggle();
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

void Menu::modify(float amount) { 
    Page& currentPage = *getCurrentPage();
    if (!this->selected && currentPage.getCurrentOption()->canModify) {
        currentPage.select();
        currentPage.modify(amount); 
        currentPage.deselect();
    } else {
        currentPage.modify(amount);
    }
}

void Menu::render(TextPrinter* printer, char* buffer) {
    printer->boxBgColor = bgColor;
    printer->boxBorderColor = outlineColor;
    printer->boxHighlightColor = highlightBoxColor;
    printer->opacity = opacity;

    printer->begin();
    printer->printf("Page 1 / %d\n\n", pages.size());
    getCurrentPage()->render(printer, buffer);

    printer->renderBoundingBox();
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

void Menu::clearPages() {
    for (int i = 0; i < pages.size(); i++){
        delete reinterpret_cast<Page*>(pages[i]);
    }
    pages.clear();
}

#pragma endregion

}
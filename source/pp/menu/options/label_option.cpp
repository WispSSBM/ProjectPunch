#include "pp/menu/options/label_option.h"

#include "pp/menu/menu.h"
#include "pp/menu/page.h"

using namespace PP::Graphics;
namespace PP {
 
void LabelOption::render(TextPrinter* printer, char* buffer) {
    // Never disabled, it's a static label.
    printer->setTextColor(parent->menu->defaultColor);
    printer->printf("%s: %s\n", name, value);
}

}

#include "pp/menu/options/bool_option.h"

using namespace PP::Graphics;
namespace PP {
    
void BoolOption::modify(float amount) {
    value = !value;
}

void BoolOption::render(TextPrinter* printer, char* buffer) {
    printer->printf("%s: %s\n", name, value ? "on" : "off");
}

}
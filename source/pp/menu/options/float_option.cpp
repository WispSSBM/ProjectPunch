#include "pp/menu/options/float_option.h"

using namespace PP::Graphics;
namespace PP {
    
void FloatOption::modify(float amount) {
    value += amount * changeMultiplier;
    if (max != NUMERIC_DEFAULT && value > max) value = min;
    else if (min != NUMERIC_DEFAULT && value < min) value = max;
}
void FloatOption::render(TextPrinter* printer, char* buffer) {
    printer->printf("%s: %.3f\n", name, value);
}

}
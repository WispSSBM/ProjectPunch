#include "pp/menu/options/choice_option.h"

namespace PP {

void ChoiceOption::modify(float amount) {
    if (amount > 0) {
        index = (index + 1) % arrayLength;
    } else if (amount < 0) {
        if (index) {
            index--;
        } else {
            index = arrayLength - 1;
        }
    }
}

void ChoiceOption::render(Graphics::TextPrinter* printer, char* buffer) {
    const char* optionValue = 
        (0 <= index && index < arrayLength) 
            ? choices[index] 
            : "???";

    printer->printf("%s: %s\n", name, optionValue);
}

} // namespace PP
#pragma once
#include "pp/menu/options/standard_option.h"

using namespace PP::Graphics;
namespace PP {
    
template <typename T>
class IntOption : public StandardOption {
public:
    IntOption(const char* name, T& value, T min = NUMERIC_DEFAULT, T max = NUMERIC_DEFAULT, bool canModify = true, bool asHex = true) : value(value) {
        this->name = name;
        this->value = value;
        this->min = min;
        this->max = max;
        this->canModify = canModify;
        hasBounds = true;
        this->asHex = asHex;
    }

    void modify(float amount);
    void render(TextPrinter* printer, char* buffer);

    T& value;
    T max;
    T min;
    bool hasBounds;
    bool asHex;
};

template <typename T>
void IntOption<T>::modify(float amount) {
    value += (T)amount;
    if (hasBounds && value > max) value = min;
    else if (hasBounds && value < min) value = max;
}

template <typename T>
void IntOption<T>::render(TextPrinter* printer, char* buffer) {
    if (asHex) {
      sprintf(buffer, "%s: 0x%02X", name, value);
    } else {
      sprintf(buffer, "%s: %02d", name, value);
    }
    printer->printLine(buffer);
}

}
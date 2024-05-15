#pragma once
#include "pp/menu/options/standard_option.h"

using namespace PP::Graphics;
namespace PP {

class FloatOption : public StandardOption {
public:
    FloatOption(const char* name, float& value, float min = NUMERIC_DEFAULT, float max = NUMERIC_DEFAULT, bool canModify = true) : value(value) {
        this->name = name;
        this->value = value;
        this->min = min;
        this->max = max;
        this->canModify = true;
        changeMultiplier = 1;
    }

    void modify(float amount);
    void render(TextPrinter* printer, char* buffer);

    ~FloatOption() {}

    float& value;
    float max;
    float min;
    float changeMultiplier;
};

}
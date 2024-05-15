#pragma once
#include "pp/menu/options/standard_option.h"

using namespace PP::Graphics;
namespace PP {

class BoolOption : public StandardOption {
public:
    BoolOption(const char* name, bool& value, bool canModify = true) : value(value) {
        this->name = name;
        this->canModify = canModify;
    }

    void modify(float amount);
    void render(TextPrinter* printer, char* buffer);

    bool& value;
};

}

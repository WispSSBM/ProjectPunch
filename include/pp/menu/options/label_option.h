#pragma once
#include "pp/menu/options/standard_option.h"

using namespace PP::Graphics;
namespace PP {
    
class LabelOption: public StandardOption {
public:
    LabelOption(const char* name, const char* value): value(value) {
        this->name = name;
        this->canModify = false;
    }

    void modify(float amount) {};
    void render(TextPrinter* printer, char* buffer);
private:
    const char* value;
};

}
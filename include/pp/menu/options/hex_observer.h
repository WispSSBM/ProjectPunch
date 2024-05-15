#pragma once
#include "pp/menu/options/standard_option.h"

using namespace PP::Graphics;
namespace PP {

enum HexSize {
    CHAR = 1,
    SHORT = 2,
    INT = 3
};

class HexObserver : public StandardOption {
public:
    HexObserver(const char* name, u32* val, HexSize size) {
        this->name = name;
        this->canModify = false;
        this->size = size;
        this->value = value;
    }
    void modify(float amount);
    void render(TextPrinter* printer, char* buffer);

private:
    const u32* value;
    HexSize size;
};

}
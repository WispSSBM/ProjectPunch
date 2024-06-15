#pragma once

#include "pp/menu/options/standard_option.h"
#include "pp/graphics/text_printer.h"

namespace PP {

class ChoiceOption : public StandardOption {
public:
    ChoiceOption(const char* name, const char** choices, int& index, int arrayLength, bool canModify = true) : index(index), choices(choices) {
        this->name = name;

        this->canModify = canModify;
        this->arrayLength = arrayLength;
    }

    void modify(float amount);
    void render(Graphics::TextPrinter* printer, char* buffer);

private:
    const char** choices;
    int& index;
    int arrayLength;
};

} // namespace PP
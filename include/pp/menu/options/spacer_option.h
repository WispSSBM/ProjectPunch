
#include "pp/menu/options/standard_option.h"

using namespace PP::Graphics;
namespace PP {

class SpacerOption : public StandardOption {
public:
    SpacerOption() { canModify = false; }
    void render(TextPrinter* printer, char* buffer);
};


}
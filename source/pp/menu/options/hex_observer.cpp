#include "pp/menu/options/hex_observer.h"

using namespace PP::Graphics;
namespace PP {

void HexObserver::modify(float amount) {}

void HexObserver::render(TextPrinter* printer, char* buffer) {
    switch (size) {
    case CHAR:
        printer->printf("%s: 0x%02x\n", name, *value);
        break;
    case SHORT:
        printer->printf("%s: 0x%04x\n", name, *value);
        break;
    case INT:
        printer->printf("%s: 0x%08x\n", name, *value);
        break;
    }
}   

}
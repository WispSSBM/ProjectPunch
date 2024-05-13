#include "pp/common.h"

#define WATERMARK_DEFAULT_OPACITY 0xFF
#define WATERMARK_DEFAULT_POSITION Coord2DF(530, 455)

namespace PP {
namespace Graphics {

class Watermark {
public:

    Watermark(): position(WATERMARK_DEFAULT_POSITION) {
        isEnabled = true;
        opacity = WATERMARK_DEFAULT_OPACITY;
        _lastFinalX = -1;
    };

    void process();

    Coord2DF position;
    bool isEnabled;
    u8 opacity;

private:
    void adjustStartX();
    float _lastFinalX;
};

extern Watermark g_watermark;
}} // namespace PP::Graphics
#include "pp/graphics/watermark.h"
#include "pp/common.h"
#include "pp/graphics/text_printer.h"
#include "pp/popup.h"

void PP::Graphics::Watermark::process() {
    if (isEnabled) {
        printer.setPosition(this->position);
        float fontScaleMult = gPopupConfig.isWidescreen ? 1.f : 0.8f;
        printer.setScale(gPopupConfig.fontScale, fontScaleMult, gPopupConfig.lineHeightMult);
        printer.boxBgColor = PP_COLOR_NONE;
        printer.boxBorderColor = PP_COLOR_NONE;
        printer.boxHighlightColor = PP_COLOR_NONE;
        printer.opacity = this->opacity;
        printer.setTextColor(PP_COLOR_WHITE);
        printer.setTextBorder(PP_COLOR_BLACK, 1);

        printer.begin();
        printer.printf("ProjectPunch v%s", PP_VERSION);

        if (_lastFinalX < 0) {
            _lastFinalX = printer.getPositionX();
            adjustStartX();
        }

        printer.renderBoundingBox();
    }
}

void PP::Graphics::Watermark::adjustStartX() {
    float width = _lastFinalX - this->position.x;
    // assume screen width of 640 minus some padding for aesthetic
    this->position.x = 630 - width;
    // now the watermark is always perfectly in the corner after frame 1.
}

PP::Graphics::Watermark PP::Graphics::g_watermark;
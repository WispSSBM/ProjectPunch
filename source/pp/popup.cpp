#include "pp/popup.h"

extern u32 frameCounter;

namespace PP {

using namespace PP::Graphics;
using namespace PP::Collections;

PopupConfig gPopupConfig = PopupConfig();


void Popup::draw(TextPrinter& printer) {
    // This will mess with the printer settings, you will likely have to reset draw mode and
    // so on after this is called.
    if (!this->expired()) {
        printer.renderPre = true;
        printer.setPosition(coords.x, coords.y);
        printer.setMinWidth(this->minWidth);
        printer.setScale(gPopupConfig.fontScale, gPopupConfig.fontScaleMult, gPopupConfig.lineHeightMult);
        printer.setTextColor(gPopupConfig.textColor);
        printer.boxPadding = gPopupConfig.popupPadding;
        printer.boxBgColor = gPopupConfig.bgColor;
        printer.boxBorderColor = gPopupConfig.outlineColor;
        printer.boxHighlightColor = gPopupConfig.highlightColor;
        printer.boxBorderWidth = 2;
        const ms::CharWriter& charWriter = *printer.charWriter;

        printer.begin();
        printer.print(this->text);

        // Gradient from yellow to red based on progress.
        Color progressColor = 0;
        progressColor.r = 255;
        float elapsed = this->percentElapsed();
        progressColor.g = ((1 - elapsed) * 255);
        progressColor.b = 0;
        progressColor.a = 255;
        float actualWidth;


        if (printer.lineStart + printer.maxWidth < charWriter.GetCursorX()) {
            actualWidth = charWriter.GetCursorX() - printer.lineStart;
        } else {
            actualWidth = printer.maxWidth;
        }


        float left = printer.lineStart - gPopupConfig.popupPadding;
        float right = printer.lineStart + actualWidth + gPopupConfig.popupPadding;
        float lrdelta = right - left;

        int multiplier = 1;

        // We can't just re-use this rect because the renderables vector implementation
        // will delete it after it has been drawn :(
        Rect* progressRect = new Rect(
            0,
            1,
            progressColor.gxColor,
            ((charWriter.GetCursorY() + printer.lineHeight + gPopupConfig.popupPadding) * multiplier) - (7*multiplier),
            (charWriter.GetCursorY() + printer.lineHeight + gPopupConfig.popupPadding) * multiplier,
            left,
            right - (this->percentElapsed()*lrdelta),
            printer.is2D
        );

        #ifdef PP_POPUP_DEBUG
        OSReport("Rendering popup: %s", this->message);
        #endif

        printer.renderBoundingBox();
        vector& shapes = renderables.items.preFrame;
        shapes.push(static_cast<Drawable*>(progressRect));
        // put the progress rect underneath the outline/highlight.
        int i = shapes.size()-1;
        void* n = shapes[i];
        void* nm1 = shapes[i-1];
        void* nm2 = shapes[i-2];
        shapes.set(i, nm1);
        shapes.set(i-1, nm2);
        shapes.set(i-2, n);
    }
}

float Popup::percentElapsed() {
    u32 framesElapsed = frameCounter - this->startFrame;
    return framesElapsed / (float)(this->durationSecs * this->fps);
}

bool Popup::expired() {
    return (this->percentElapsed() >= 1);
}

}
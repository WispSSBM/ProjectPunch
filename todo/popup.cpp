#include "popup.h"

extern u32 frameCounter;

PopupConfig gPopupConfig = PopupConfig {
    .popupPadding = 10,
    .bgColor = COLOR_TRANSPARENT_GREY,
    .outlineColor = 0x333333FF,
    .highlightColor = 0xBBBBBBFF
};

Popup::Popup(const char* text) {
    this->startFrame = startFrame;
    this->text = new char[strlen(text)];
    this->startFrame = frameCounter;
    strcpy(this->text, text);
}

Popup::~Popup() {
    delete[] this->text;
}

        void sneaky(vector<Drawable*>& shapes) {
        }

void Popup::draw(TextPrinter& printer) {
    // This will mess with the printer settings, you will likely have to reset draw mode and
    // so on after this is called.
    if (!this->expired()) {
        printer.setup();
        printer.setTextColor(COLOR_WHITE);
        printer.renderPre = true;
        Message* printerMsgObj = &(printer.message);
        printerMsgObj->fontScaleX = PP_DEFAULT_FONT_SCALE_X;
        printerMsgObj->fontScaleY = PP_DEFAULT_FONT_SCALE_Y;
        printer.lineHeight = printerMsgObj->fontScaleY * 20;
        printerMsgObj->xPos = coords.x;
        printerMsgObj->yPos = coords.y;
        printerMsgObj->zPos = 0;
        printer.start2D();

        printer.startBoundingBox();
        printer.print(this->text);
        printer.padToWidth(this->minWidth);


        // Gradient from yellow to red based on progress.
        GXColor progressColor = 0;
        progressColor.red = 255;
        float elapsed = this->percentElapsed();
        progressColor.green = ((1 - elapsed) * 255);
        progressColor.blue = 0;
        progressColor.alpha = 255;
        float actualWidth;


        if (printer.lineStart + printer.maxWidth < printerMsgObj->xPos) {
            actualWidth = printerMsgObj->xPos - printer.lineStart;
        } else {
            actualWidth = printer.maxWidth;
        }


        int multiplier = (printer.is2D) ? 1 : -1;
        float left = printer.lineStart - gPopupConfig.popupPadding;
        float right = printer.lineStart + actualWidth + gPopupConfig.popupPadding;
        float lrdelta = right - left;

        // We can't just re-use this rect because the renderables vector implementation
        // will delete it after it has been drawn :(
        Rect* progressRect = new Rect {
            0,
            1,
            progressColor,
            ((printerMsgObj->yPos + printer.lineHeight + gPopupConfig.popupPadding) * multiplier) - (7*multiplier),
            (printerMsgObj->yPos + printer.lineHeight + gPopupConfig.popupPadding) * multiplier,
            left,
            right - (this->percentElapsed()*lrdelta),
            printer.is2D
        };

        #ifdef PP_POPUP_DEBUG
        OSReport("Rendering popup: %s", this->message);
        #endif
        printer.saveBoundingBox(printer.bboxIdx, gPopupConfig.bgColor, gPopupConfig.outlineColor, gPopupConfig.highlightColor, 2, gPopupConfig.popupPadding);
        auto& shapes = renderables.items.preFrame;
        shapes.push(progressRect);
        // put the progress rect underneath the outline/highlight.
        auto i = shapes.size()-1;
        auto n = shapes[i];
        auto nm1 = shapes[i-1];
        auto nm2 = shapes[i-2];
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

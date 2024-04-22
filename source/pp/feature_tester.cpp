#ifdef PP_FEATURE_TEST
#include <OS/OSError.h>
#include "gf/gf_scene.h"

#include "pp/main.h"
#include "pp/common.h"
#include "pp/feature_tester.h"
#include "pp/collections/linkedlist.h"
#include "pp/graphics/text_printer.h"
#include "pp/graphics/draw.h"
#include "pp/graphics/drawable.h"
#include "pp/popup.h"
#include "pp/input/pad.h"

namespace PP {
Graphics::TextPrinter printer = Graphics::TextPrinter();
PP::Popup* popup = 0;


void featureTester() {
    frameCounter += 1;
    Graphics::renderables.renderPre();
    gfScene& scene = *gfSceneManager::getInstance()->m_currentScene;
    if (strcmp("scCharSel", scene.m_sceneName)) {

        if (!initialized) {
            OSReport("Hello world.\n");
            Collections::testLinkedList();
            initialized = true;


        }

        if (PP::Input::g_padStatus[0].btns.A && popup == NULL) {
            popup = new PP::Popup("A press.\n");
            popup->coords.x = 100;
            popup->coords.y = 100;
        }

        /* Test out printing stuff. */
        printer.bgColor = COLOR_BLACK;
        printer.boxBorderColor = 0x888888FF;
        printer.boxHighlightColor = COLOR_WHITE;
        printer.boxPadding = 5;
        pritner.boxBorderWidth = 6;

        printer.setPosition(200, 200);
        printer.setScale(Coord2DF(1.f, 1.f));
        printer.setTextColor(0x00FFFFFF);

        printer.begin();
        printer.print("Hello world.\n");
        printer.renderBoundingBox();

        /* Test out drawing a rect. */
        Graphics::renderables.items.frame.push(
            new Graphics::Rect(
                300.f,
                100.f,
                100.f,
                50.f,
                true,
                Graphics::makeGXColor(0, 255, 128, 255)
            )
        );

        if (popup != 0) {
            if (popup->expired()) {
                delete popup;
                popup = 0;
            } else {
                popup->draw(printer);
            }
        }
    }

    Graphics::renderables.renderAll();
    Graphics::startNormalDraw();
}

} // namespace
#endif
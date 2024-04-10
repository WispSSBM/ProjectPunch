#include <OS/OSError.h>
#include "gf/gf_scene.h"

#include "pp/common.h"
#include "pp/feature_tester.h"
#include "pp/collections/linkedlist.h"
#include "pp/graphics/text_printer.h"
#include "pp/graphics/draw.h"
#include "pp/graphics/drawable.h"
#include "pp/popup.h"
#include "pp/input/pad.h"

namespace ProjectPunch {

bool initialized = false;
Graphics::TextPrinter printer = Graphics::TextPrinter();
ProjectPunch::Popup* popup = 0;


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

        if (ProjectPunch::Input::g_padStatus[0].btns.A && popup == NULL) {
            popup = new ProjectPunch::Popup("A press.\n");
            popup->coords.x = 100;
            popup->coords.y = 100;
        }

        /* Test out printing stuff. */
        printer.setup(true);
        ms::CharWriter& message = *(printer.charWriter);
        message.SetCursor(200, 200, 0);
        message.SetScale(1.f);
        Color c = 0x00FFFFFF;
        message.SetTextColor(c.utColor);

        printer.startBoundingBox();
        printer.print("Hello world.\n");
        printer.saveBoundingBox(COLOR_BLACK, 0x888888FF, COLOR_WHITE, 6, 5);

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
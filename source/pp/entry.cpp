#include <ft/fighter.h>
#include <cstring>
#include <gf/gf_scene.h>
#include <GX/GXTypes.h>
#include <OS/OSError.h>

#include <sy_core.h>

#include "pp/collections/linkedlist.h"
#include "pp/entry.h"
#include "pp/graphics/text_printer.h"
#include "pp/graphics/draw.h"
#include "pp/graphics/drawable.h"

namespace ProjectPunch {
namespace Entry {

bool initialized = false;
Graphics::TextPrinter printer = Graphics::TextPrinter();

void Init() {
    // 0x800177B0
    SyringeCore::syInlineHook(0x800177B0, (void*)helloWorld);
}

void Destroy() {

}

void helloWorld() {
    Graphics::renderables.renderPre();

    if (!initialized) {
        OSReport("Hello world.\n");
        Collections::testLinkedList();
        initialized = true;
    }
    gfScene& scene = *gfSceneManager::getInstance()->m_currentScene;
    if (strcmp("scCharSel", scene.m_sceneName)) {

        /* Test out printing stuff. */
        printer.setup(true);
        ms::CharWriter& message = *(printer.message);
        message.SetCursor(200, 200, 0);
        message.SetScale(1.f);
        Color c = 0x00FFFFFF;
        message.SetTextColor(c.utColor);

        printer.startBoundingBox();
        printer.print("Hello world.\n");
        printer.saveBoundingBox(PP_COLOR_BLACK, 0x888888FF, PP_COLOR_WHITE, 6, 5);

        /* Test out drawing a rect. */
        Graphics::renderables.items.frame.push(
            new Graphics::Rect(
                100.f,
                100.f,
                100.f,
                50.f,
                true,
                Graphics::makeGXColor(0, 255, 128, 255)
            )
        );

    }

    Graphics::renderables.renderAll();
    Graphics::startNormalDraw();
}
}
}
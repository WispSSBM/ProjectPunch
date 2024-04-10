#include <ft/fighter.h>
#include <OS/OSError.h>
#include <sy_core.h>

#include "projectPunch.h"
#include "linkedlist.h"

namespace ProjectPunch {
    static bool initialized = false;

    void Init() {
        // 0x800177B0
        SyringeCore::syInlineHook(0x800177B0, (void*)helloWorld);
    }

    void Destroy() {

    }

    void helloWorld() {
        if(!initialized) {
            OSReport("Hello world.\n");
            testLinkedList();
            initialized = true;
        }
    }
}
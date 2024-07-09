#include <sy_core.h>

#include "pp/entry.h"
#include "pp/feature_tester.h"
#include "pp/main.h"
#include "pp/playerdata.h"
#include "pp/global_settings.h"
#include "pp/invis_techchase.h"
#include "pp/frame_advance.h"
#include "gf/gf_pad_status.h"
#include "memory.h"

extern void pauseHookReturn();

using namespace PP;
namespace PP {
namespace Entry {

asm void pauseHook() {
    nofralloc
    mr r3, r25; 
    bl checkMenuPaused; // if this function messes with the wrong registers things will explode.
    lwz r3, 0(r25);     // original instruction
    b pauseHookReturn;
}

void Init() {
    //SyringeCore::syInlineHook(0x800177B0, (void*)ProjectPunch::featureTester);
    SyringeCore::syInlineHook(0x80017794, (void*)PP::updatePreFrame);
    SyringeCore::syInlineHook(0x8002a258, PP::gfPadUpdateHook);
    SyringeCore::sySimpleHook(0x8002e5b0, pauseHook);
    // SyringeCore::sySimpleHookRel(0x5afb4, PP::invisTechchaseMainHook, 27);
}

void Destroy() {}

}
}
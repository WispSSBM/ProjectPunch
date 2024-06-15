#include <sy_core.h>

#include "pp/entry.h"
#include "pp/feature_tester.h"
#include "pp/main.h"
#include "pp/playerdata.h"
#include "pp/global_settings.h"
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

void gfPadUpdateHook() {
    register u32 padStatusR29;
    register u32 padStatusR28;
    asm {
        mr padStatusR28, r28;
        mr padStatusR29, r29;
    }

    gfPadStatus* padStatus = reinterpret_cast<gfPadStatus*>(padStatusR28);
    gfPadStatus* prevPadStatus = reinterpret_cast<gfPadStatus*>(padStatusR29);

    if (padStatus->m_error == GF_PAD_ERROR_NONE) {
        DEBUG_INPUT("padStatus: 0x%0x, error: %d\n", (u32)padStatus, padStatus->m_isNotConnected);
        DEBUG_INPUT("  A: %d B: %d X: %d Y: %d Z: %d L: %d R: %d S: %d\n",
            padStatus->m_buttonsCurrentFrame.m_a, 
            padStatus->m_buttonsCurrentFrame.m_b, 
            padStatus->m_buttonsCurrentFrame.m_x, 
            padStatus->m_buttonsCurrentFrame.m_y, 
            padStatus->m_buttonsCurrentFrame.m_z, 
            padStatus->m_buttonsCurrentFrame.m_l, 
            padStatus->m_buttonsCurrentFrame.m_r, 
            padStatus->m_buttonsCurrentFrame.m_start
        );

        DEBUG_INPUT("  wiimote stuff 0x18: %0.2f 0x1c: %0.2f 0x20: %0.2f 0x24: %0.2f 0x28: %0.2f 0x2C: %0.2f\n",
            padStatus->_0x18, padStatus->_0x1c,
            padStatus->_0x20, padStatus->_0x24,
            padStatus->_0x28, padStatus->_0x2c
        );

        DEBUG_INPUT("  stick: (%d, %d) substick: (%d, %d) analog L/R: (%d, %d)\n",
            padStatus->m_stickX,
            padStatus->m_stickY,
            padStatus->m_subStickX,
            padStatus->m_subStickY,
            padStatus->m_lTriggerAnalog,
            padStatus->m_rTriggerAnalog,
        );

        if (padStatus->m_error == GF_PAD_ERROR_NONE) {
            g_frameAdvance.handleInput(*padStatus);
        }
    }
}

void Init() {
    //SyringeCore::syInlineHook(0x800177B0, (void*)ProjectPunch::featureTester);
    SyringeCore::syInlineHook(0x80017794, (void*)PP::updatePreFrame);
    SyringeCore::syInlineHook(0x8002a258, gfPadUpdateHook);
    SyringeCore::sySimpleHook(0x8002e5b0, pauseHook);
}

void Destroy() {}

}
}
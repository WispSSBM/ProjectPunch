#include <ft/fighter.h>
#include <so/shadow/so_shadow_module_impl.h>

#include "pp/common.h"
#include "pp/global_settings.h"
#include "pp/invis_techs.h"
#include <OS/OSError.h>

typedef void (*setActiveStatusPtr)(int*, bool);

#define SUBACT_NEUTRAL_GETUP_1 0xAF
#define SUBACT_GETUP_ATK_1 0xB0
#define SUBACT_ROLL_FWD_1 0xB1
#define SUBACT_ROLL_BACK_1 0xB2

#define SUBACT_NEUTRAL_GETUP_2 0xB8
#define SUBACT_GETUP_ATK_2 0xB9
#define SUBACT_ROLL_FWD_2 0xBA
#define SUBACT_ROLL_BACK_2 0xBB

#define SUBACT_TECH_INPLACE 0xBE
#define SUBACT_TECH_FWD 0xBF
#define SUBACT_TECH_BACK 0xC0


namespace PP {

static bool& enableInvisTechs = GlobalSettings::enableInvisibleTechs;

// defined in EXTRAS.lst

bool isTechAnim(int currentSubaction) {
    if (currentSubaction < SUBACT_NEUTRAL_GETUP_1) {
        return false;
    } else {
        if (betweenIncl(SUBACT_GETUP_ATK_1, currentSubaction, SUBACT_ROLL_BACK_1)) {
            return true;
        }

        if (betweenIncl(SUBACT_NEUTRAL_GETUP_2, currentSubaction, SUBACT_ROLL_BACK_2)) {
            return true;
        }

        if (betweenIncl(SUBACT_TECH_INPLACE, currentSubaction, SUBACT_TECH_BACK)) {
            return true;
        }
    }

    return false;
}

bool isInInvisTechWindow(soMotionModule& motionModule) {
    int currentSubaction = motionModule.getKind();
    int subactionFrame = (int)motionModule.getFrame();

    if (isTechAnim(currentSubaction) && betweenExcl(5, subactionFrame, 20)) {
        return true;
    } else {
        return false;
    }
}

extern void invisTechsMainHookReturn();
asm void invisTechsMainHook() {
    nofralloc  // We're handling this ourself, because we want to preserve the calling registers
               // which goes against ppc behavior.

    enableCheck:
        lis r8, enableInvisTechs@h;
        lwz r8, enableInvisTechs@l(r8);
        lbz r8, 0(r8); // dereference the pointer
        cmplwi r8, 1;
        bne done;
    
    prologue:
        stwu sp, -0x34(sp);
        mflr r0;
        stw r0, 0x38(sp);

        stw r4, 0x8(sp); // We're not preserving r3
        stw r5, 0xC(sp);
        stw r6, 0x10(sp);
        stw r7, 0x14(sp);
        stw r8, 0x18(sp);
        stw r9, 0x1C(sp);
        stw r10, 0x20(sp);
        stw r11, 0x24(sp);
        stw r12, 0x28(sp);
        stw r13, 0x2C(sp);

    main:
        mr r3, r30; // r30 is a soVisibilityModuleSimple
        lwz r3, 0x10(r3); // This is the soModuleAccesser
        bl invisTechsMain;


    epilogue:
        lwz r0, 0x34(sp);
        lwz r4, 0x8(sp);
        lwz r5, 0xC(sp);
        lwz r6, 0x10(sp);
        lwz r7, 0x14(sp);
        lwz r8, 0x18(sp);
        lwz r9, 0x1C(sp);
        lwz r10, 0x20(sp);
        lwz r11, 0x24(sp);
        lwz r12, 0x28(sp);
        lwz r13, 0x2C(sp);
        mtlr r0;
        addi sp, sp, 0x34;

    done:
        // Jump Back //
        cmpwi r3, 0; // original instruction
        b invisTechsMainHookReturn;
}

bool invisTechsMain(const soModuleAccesser* moduleAccesser) {
    /*
     * The return value of this sets the fighter's visibility by
     * the intercepted function.
     */
    if (moduleAccesser == 0 || moduleAccesser->m_enumerationStart == 0) {
        return 1;
    }

    soMotionModule* motionModule = moduleAccesser->getMotionModule();
    soShadowModule* shadowModule = moduleAccesser->getShadowModule();

    // OSReport("Motion Module: 0x%08x Shadow Module: 0x%08x\n", motionModule, shadowModule);

    bool isInTechAnim = false;
    if (isInInvisTechWindow(*motionModule)){
        shadowModule->setActiveStatus(0);
        return 0;
    } else {
        shadowModule->setActiveStatus(1);
        return 1;
    }
}

extern void invisTechsCursorHookReturnSkip();
extern void invisTechsCursorHookReturnNoSkip();
asm void invisTechsCursorHook() {
    /* Hooks update/[soVisibilityModuleSimple] + 0x38 */
    nofralloc

    enableCheck:
        lis r8, enableInvisTechs@h;
        lwz r8, enableInvisTechs@l(r8);
        lbz r8, 0(r8);
        cmplwi r8, 1;
        bne normalCursor;

    main:
        mr r3, r30;
        bl invisTechsCursorMain;
        cmplwi r8, 0;
        beq normalCursor;

    skipCursor:
        // original instruction isn't needed anyway
        b invisTechsCursorHookReturnSkip;

    normalCursor:
        lwz r3, 0xd8(r31); // original instruction
        b invisTechsCursorHookReturnNoSkip;
}

bool invisTechsCursorMain(Fighter& fighter) {
    /*
     *  If this returns 1, it means that the feature is enabled
     *  and active this frame and to skip the normal behavior.
     */
    soModuleAccesser& modules = *fighter.m_moduleAccesser;
    soMotionModule& motionModule = *modules.getMotionModule();
    if (isInInvisTechWindow(motionModule)) {
        fighter.setCursor(0);
        fighter.setNameCursor(0);
        return 1;
    } else {
        return 0;
    }
}

}

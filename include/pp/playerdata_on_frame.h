#pragma once

#include "pp/common.h"
#include "memory.h"

#include <so/controller/so_controller_impl.h>
#include <ip/Input.h>

namespace PP {

// 0 indexed unlike the 
union InterruptGroupStates {
    InterruptGroupStates() {
        DEBUG_CTOR("InterruptGroupStates ctor @ 0x%0x\n", this);
        memset(asArray, 0, 0x13);
    }
    ~InterruptGroupStates() {
        DEBUG_CTOR("InterruptGroupStates dtor @ 0x%0x\n", this);
    }

    struct {
        bool groundSpecial;
        bool groundItem;
        bool groundGrab;
        bool groundAttack;
        bool groundDodge;
        bool groundGuard;
        bool groundJump;
        bool groundMisc;
        bool airLand;
        bool airGrabLedge;
        bool airSpecial;
        bool airThrowItem;
        bool airTether;
        bool airDodge;
        bool airAttack;
        bool airFloat;
        bool airWalljump;
        bool airDoublejump;
        bool fallThroughPlat;
    };

    bool asArray[0x13];
};

enum HurtboxStatus {
    NORMAL = 0,
    INVIN = 1,
    INTAN = 2,
    INTAN_NO_FLASH = 3,
    INTAN_FAST_FLASH = 4
};

// This is stuff that changes on every frame. This struct gets
// set with memset, so making it virtual is a bad idea(tm).
class PlayerDataOnFrame {
public:
    PlayerDataOnFrame();
    ~PlayerDataOnFrame() {
        DEBUG_CTOR("PlayerDataOnFrame dtor @ 0x%0x\n", this);
    }

    int action;                  // 0x0
    const char* actionName;      // 0x4
    u32 subaction;               // 0x8
    const char* subactionName;   // 0xC

    float subactionFrame;        // 0x10
    float subactionTotalFrames;  // 0x14
    int ledgeIntan;              // 0x18
    u32 lowRABits;               // 0x1C
    u16 actionFrame;             // 0x20
    u16 hitstun;                 // 0x22

    u16 shieldstun;              // 0x24

    HurtboxStatus bodyHurtboxType; // 0x28

    bool canCancel;              // 0x29
    bool isAirborne;             // 0x2A
    bool didConnectAttack;       // 0x2B

    // These flags help with cases where we go through more than one state in the same frame.
    // in setAction(), we check the new action and set these flags if the new action is actionable.
    // This flag is also set at the beginning of the new frame based on the current action.
    bool occupiedGroundedIasa; // 0x2C
    bool occupiedActionableState; // 0x2D
    bool occupiedWaitingState; // 0x2E
    bool actionChanged;

    InterruptGroupStates interruptGroups; // 0x30?

    ipButton controllerBtnHeld;
    ipButton controllerBtnPressed;
    ipButton controllerBtnReleased;
    Coord2DF stick;
    Coord2DF substick;

    bool inIasa() const;
    bool inGroundedIasa() const;
    bool canAutocancel() const;
    const char* subactionStr() const;
    bool getLowRABit(u32 idx) const;
    bool isShielding() const;
    void recordControllerStatus(const soControllerImpl& controller);
};

} // namespace PP
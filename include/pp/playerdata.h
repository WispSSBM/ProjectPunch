#pragma once

#include <cstring>
#include <OS/OSError.h>

#include "pp/actions.h"
#include "pp/common.h"
#include "pp/popup.h"

namespace PP {

bool startsWith(const char* testStr, const char* prefix);

struct PlayerDataOnFrame;
struct PlayerData;
struct PlayerDisplayOptions;

class AnimCmdWatcher;
class StatusChangeWatcher;
class LedgeTechWatcher;

enum HurtboxStatus {
    NORMAL = 0,
    INVIN = 1,
    INTAN = 2,
    INTAN_NO_FLASH = 3,
    INTAN_FAST_FLASH = 4
};

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

    bool inIasa() const;
    bool inGroundedIasa() const;
    bool canAutocancel() const;
    const char* subactionStr() const;
    bool getLowRABit(u32 idx) const;
    bool isShielding() const;

};

// This is mostly meta-data that we track over multiple frames.
// It doesn't auto-clear and needs to be cleared manually.
struct PlayerData {
    PlayerData();
    ~PlayerData();
    u16 maxHitstun;    // 0x0
    u16 maxShieldstun; // 0x2
    u8 playerNumber;   // 0x4
    ftKind charId;     // 0x8, bad padding
    int taskId;        // 0xC
    int entryId;       // 0x10

    char* fighterName; // 0x14

    /* Targeting bookkeeping */
    PlayerData* attackTarget;      // 0x18
    char advantageBonusCounter;    // 0x1C
    u32 becameActionableOnFrame;   // 0x20
    u32 lastAttackEndedOnFrame;    // 0x24

    /* Swapped every frame */
    PlayerDataOnFrame* prev;       // 0x28
    PlayerDataOnFrame* current;    // 0x2C

    int attackStartFrame;        // 0x30

    /* flags that persist between frames. These are mostly targeting related. */
    u32 didConnectAttack : 1; // 0x34
    u32 isAttackingShield : 1;
    u32 isAttackingFighter : 1;

    /* flags that clear between frames*/
    // Set by the event system.
    u32 didActionChange: 1;

    /* display flags */
    // These aren't bitfields because you can't take a pointer to a bitfield, and that
    // would mess with the menu system.
    bool showOnHitAdvantage;       // 0x34
    bool showOnShieldAdvantage;    // 0x35
    bool showActOutOfLag;          // 0x36
    bool showFighterState;         // 0x47
    bool enableWaitOverlay;        // 0x48
    bool enableDashOverlay;        // 0x49
    bool enableIasaOverlay;        // 0x4A
    bool enableLedgeTechGalintPopup;
    bool enableLedgeTechFrameDisplay;
    bool enableLedgeTechFramesOnLedgePopup;

    AnimCmdWatcher* animCmdWatcher;
    StatusChangeWatcher* statusChangeWatcher;
    LedgeTechWatcher* ledgeTechWatcher;

    /* aliases for fields on Current*/
    u16 action() const;
    const char* actionStr() const;
    u32 raLowBits() const;

    u16 subaction() const;
    const char* subactionStr() const;
    float subactionFrame() const;
    float subactionTotalFrames() const;

    int writeLowRABoolStr(char* buffer) const;

    /* Derived information. */
    bool hasStartedAttack() const;
    bool didReceiveHitstun() const;
    bool didReceiveShieldstun() const;
    bool canCancel() const;
    bool didSubactionChange() const;
    bool didEnterShield() const;
    bool inActionableState() const;
    bool inAttackState() const;
    bool inIasa() const;
    bool inGroundedIasa() const;
    bool isGroundedActionable();
    void preFrame();
    void prepareNextFrame();
    void setAction(u16 newAction);

    void initLedgeTechWatcher(Fighter& fighter);

    /* Lifecycle methods */
    void resetTargeting();
    bool resolvePlayerActionable();
    bool resolveTargetActionable();
    Popup* createPopup(const char* fmt, ...);
    int debugStr(char* buffer);
};

struct PlayerDisplayOptions {
};


extern PlayerData* allPlayerData;

} // namespace
#pragma once

#include <cstring>
#include <OS/OSError.h>

#include <StaticAssert.h>
#include <ip/Input.h>

#include "pp/actions.h"
#include "pp/common.h"
#include "pp/popup.h"
#include "pp/playerdata_on_frame.h"

namespace PP {

struct PlayerData;
struct PlayerDisplayOptions;

class AnimCmdWatcher;
class StatusChangeWatcher;
class LedgeTechWatcher;


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
    void printFighterState() const;
    void resetTargeting();
    bool resolvePlayerActionable();
    bool resolveTargetActionable();
    Popup* createPopup(const char* fmt, ...);
    int debugStr(char* buffer) const;
    inline PlayerSettings& settings() { return globalSettings.playerSettings[this->playerNumber]; };
};

struct PlayerDisplayOptions {
};


extern PlayerData* allPlayerData;

} // namespace
#pragma once

#include <cstring>

#include "pp/actions.h"
#include "pp/common.h"

namespace ProjectPunch {

bool startsWith(const char* testStr, const char* prefix);

struct PlayerDataOnFrame;
struct PlayerData;
struct PlayerDisplayOptions;

// This is stuff that changes on every frame.
struct PlayerDataOnFrame {
    PlayerDataOnFrame();

    int action;
    char* actionName;
    u32 subaction;
    char* subactionName;
    float subactionFrame;
    float subactionTotalFrames;

    u32 lowRABits;
    u16 actionFrame;
    u16 hitstun;
    u16 shieldstun;

    u32 canCancel: 1;
    u32 didConnectAttack: 1;

    bool getLowRABit(u32 idx) const;
    inline bool isShielding() const;

};

// This is mostly meta-data that we track over multiple frames.
// It doesn't auto-clear and needs to be cleared manually.
struct PlayerData {
    PlayerData();
    u16 maxHitstun;
    u16 maxShieldstun;
    u8 playerNumber;
    ftKind charId;
    int taskId;
    int entryId;

    char* fighterName;

    /* Targeting bookkeeping */
    PlayerData* attackTarget;
    char advantageBonusCounter;
    u32 becameActionableOnFrame;
    u32 lastAttackEndedOnFrame;
    u16 attackingAction;

    /* Swapped every frame */
    PlayerDataOnFrame* prev;
    PlayerDataOnFrame* current;

    /* interactive flags */
    u32 didStartAttack : 1;
    u32 didConnectAttack : 1;
    u32 isAttackingShield : 1;
    u32 isAttackingFighter : 1;

    /* display flags */
    bool showOnHitAdvantage;
    bool showOnShieldAdvantage;
    bool showActOutOfLag;
    bool showFighterState;


    /* aliases for fields on Current*/
    u16 action() const;
    const char* actionStr() const;
    u16 actionFrame() const;
    u32 raLowBits() const;

    u16 subaction() const;
    const char* subactionStr() const;
    float subactionFrame() const;
    float subactionTotalFrames() const;

    int writeLowRABoolStr(char* buffer) const;

    /* Derived information. */
    bool didReceiveHitstun() const;
    bool didReceiveShieldstun() const;
    bool didBecomeActionable() const;
    bool didEnableCancel() const;
    bool didActionChange() const;
    bool didSubactionChange() const;
    bool didEnterShield() const;
    void prepareNextFrame();

    /* Lifecycle methods */
    void resetTargeting();
    bool resolvePlayerActionable();
    bool resolveTargetActionable();
    int debugStr(char* buffer);
};

struct PlayerDisplayOptions {
};


extern PlayerData* allPlayerData;

} // namespace
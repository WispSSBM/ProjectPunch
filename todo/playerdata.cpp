#include "playerdata.h"
#include "fighterNames.h"

extern unsigned int frameCounter;
PlayerData* allPlayerData = new PlayerData[PP_MAX_PLAYERS];


bool startsWith(const char* testStr, const char* prefix) {
    while (*prefix != '\0') {
        if (*testStr == '\0') {
            return false;
        }

        if (*prefix != *testStr) {
            return false;
        }

        prefix++;
        testStr++;
    }
    return true;
}

void PlayerData::resetTargeting() {
    attackingAction = -1;
    attackTarget = nullptr;
    becameActionableOnFrame = -1;
    advantageBonusCounter = 0;
    isAttackingShield = false;
    isAttackingFighter = false;
}


int PlayerData::debugStr(char* buffer) {
    PlayerDataOnFrame& f = *(this->current);

    return snprintf(buffer, PP_STR_MANIP_SIZE,
    "Player %d: %s\n"
    "  Action:     %s(0x%X)      Frames: %d/%d\n"
    "  Subaction: %s(0x%X)\n"
    "  Hitstun: %d/%d        Shieldstun: %d/%d        Shielding: %c\n"
    ,
    playerNumber, fighterName(charId),
    actionName(f.action), f.action, f.actionFrame, f.actionTotalFrames,
    f.subactionName, f.subaction, 
    f.hitstun, maxHitstun, f.shieldstun, maxShieldstun, (f.isShielding() ? 'T' : 'F')
    );
}

bool PlayerDataOnFrame::getLowRABit(u32 idx) const {
    if (idx >= 32) {
        OSReport("WARNING: Asked for ra-bit that's too high.\n");
        return false;
    }

    return 1 == ((this->lowRABits & (1 << idx)) >> idx);
}


bool PlayerData::resolvePlayerActionable() {
    auto& player = *this;
    if (player.becameActionableOnFrame != -1) {
        return true; // already became actionable..
    }

    if (isDefinitelyActionable(player.current->action)) {
        OSReport("Attacker %d became actionable.\n  - Prev Act/Subact: %s/%s\n  - Cur Act/Subact: %s/%s\n",
            player.playerNumber,
            actionName(player.prev->action), player.prev->subactionName,
            actionName(player.current->action), player.current->subactionName
        );
        player.becameActionableOnFrame = frameCounter;
        return true;
    }

    if (player.current->canCancel) {
        OSReport("Attacker %d became actionable through the cancel module.\n", player.playerNumber);
        player.becameActionableOnFrame = frameCounter;
        return true;
    }

    if (player.current->getLowRABit(RA_BIT_ENABLE_ACTION_TRANSITION) && !isEATBitExclusion(player.charId, player.current->action)) {
        OSReport("Attacker %d became actionable through the EnableActionTransition RA-Bit.\n", player.playerNumber);
        player.becameActionableOnFrame = frameCounter + 1;
        return true;
    }

    return false;
}


bool PlayerData::resolveTargetActionable() {
    auto& target = *this;

    if (target.becameActionableOnFrame != -1) {
        return true; // already happened.
    }

    if (isDefinitelyActionable(target.current->action)) {
            OSReport("target %d became actionable.\n  - Prev Act/Subact: %s/%s\n  - Cur Act/Subact: %s/%s\n", 
                target.playerNumber,
                actionName(target.prev->action), target.prev->subactionName,
                actionName(target.current->action), target.current->subactionName
            );
            target.becameActionableOnFrame = frameCounter;
            return true;
    }

    if (target.current->hitstun == 0 && target.current->shieldstun == 0) {
        OSReport("Target %d became actionable because hitstun ran out.\n", target.playerNumber);
        target.becameActionableOnFrame = frameCounter;
        return true;

    }

    if (target.current->getLowRABit(RA_BIT_ENABLE_ACTION_TRANSITION)) {
        OSReport("Target %d became actionable through the EnableActionTransition RA-Bit.\n", target.playerNumber);
        target.becameActionableOnFrame = frameCounter + 1;
        return true;
    }

    return false;
}
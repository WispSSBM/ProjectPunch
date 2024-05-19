#pragma once

#include <OS/OSError.h>
#include <stdio.h>
#include <stdarg.h>
#include <memory.h>

#include "pp/playerdata.h"
#include "pp/anim_cmd_watcher.h"
#include "pp/status_change_watcher.h"
#include "pp/ledge_tech.h"

namespace PP {

extern u32 frameCounter;
PlayerData* allPlayerData = NULL;


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

PlayerData::PlayerData() {
    becameActionableOnFrame = -1;
    lastAttackEndedOnFrame = -1; // currently unused.
    prev = new PlayerDataOnFrame();
    current = new PlayerDataOnFrame();
    fighterName = NULL;
    attackTarget = NULL;

    didStartAttack = false;
    didConnectAttack = false;
    isAttackingShield = false;
    isAttackingFighter = false;

    didActionChange = false;

    showOnHitAdvantage = false;
    showOnShieldAdvantage = false;
    showFighterState = false;
    showActOutOfLag = false;

    enableWaitOverlay = false;
    enableDashOverlay = false;
    enableIasaOverlay = false;
    enableLedgeTechFrameDisplay = false;
    enableLedgeTechFramesOnLedgePopup = false;
    enableLedgeTechGalintPopup = false;

    animCmdWatcher = NULL;
    statusChangeWatcher = NULL;
    ledgeTechWatcher = NULL;

    charId = Fighter_Mario;
    taskId = -1;
    entryId = -1;

    DEBUG_CTOR("PlayerData ctor: this=0x%x, current=0x%x, prev=0x%x\n", this, current, prev);
};

PlayerData::~PlayerData() {
    DEBUG_CTOR("PlayerData dtor: this=0x%x\n", this);
    if (animCmdWatcher != NULL) {
        delete animCmdWatcher;
    }

    if (statusChangeWatcher != NULL) {
        delete statusChangeWatcher;
    }

    if (ledgeTechWatcher != NULL) {
        delete ledgeTechWatcher;
    }

    delete current;
    delete prev;
}

PlayerDataOnFrame::PlayerDataOnFrame() {
        DEBUG_CTOR("PlayerDataOnFrame ctor: this=0x%x\n", this);


        action = 0;
        actionName = NULL;
        subaction = 0;
        subactionFrame = 0;
        subactionName = NULL;
        subactionTotalFrames = 0;

        lowRABits = 0;
        hitstun = 0;
        shieldstun = 0;

        canCancel = false;
        didConnectAttack = false;
        isAirborne = false;
}

void PlayerData::resetTargeting() {
    attackTarget = NULL;
    becameActionableOnFrame = -1;
    advantageBonusCounter = 0;
    isAttackingShield = false;
    isAttackingFighter = false;
}


int PlayerData::debugStr(char* buffer) {
    PlayerDataOnFrame& f = *(this->current);
    char raBits[50];
    this->writeLowRABoolStr(raBits);

    return snprintf(buffer, PP_STR_MANIP_SIZE,
    "Player %d: %s               Action:    %s(0x%X)\n"
    "  Subaction: %s(0x%X)       Frames: %d/%d\n"
    "  Hitstun: %d/%d        Shieldstun: %d/%d        Shielding: %c\n"
    "  Frame: %d             Airborne: %c          ModCancel: %c\n"
    "  RA: %s"
    ,
    playerNumber, this->fighterName, actionName(f.action), f.action,
    f.subactionName, f.subaction, (int)f.subactionFrame, (int)f.subactionTotalFrames,
    f.hitstun, maxHitstun, f.shieldstun, maxShieldstun, (f.isShielding() ? 'T' : 'F'),
    frameCounter, (f.isAirborne ? 'T' : 'F'), (f.canCancel ? 'T' : 'F'),
    raBits
    );
}

int PlayerData::writeLowRABoolStr(char* buffer) const {
    /* sizeof(buffer) should probably be at least 50. */
    char boolVal;
    char* start = buffer;

    for (int j = 0; j < 32; j++) { // one int's worth of data
        *(buffer++) = ((this->current->lowRABits & (0x1 << j)) >> j) == 1 ? '1' : '0';

        if ((j % 8) == 7 && j != 31) {
            *(buffer++) = ' ';
        }
    }

    *(buffer--) = '\0';

    return buffer - start;
}


bool PlayerDataOnFrame::getLowRABit(u32 idx) const {
    if (idx >= 32) {
        OSReport("WARNING: Asked for ra-bit that's too high.\n");
        return false;
    }

    return 1 == ((this->lowRABits & (1 << idx)) >> idx);
}


const char* strAttackActionable = "ATKR %d now actionable via ";
bool PlayerData::resolvePlayerActionable() {
    PlayerData& player = *this;
    int currentAction = player.current->action;
    if (player.becameActionableOnFrame != (u32)(-1)) {
        return true; // already became actionable..
    }

    if (current->occupiedActionableStateThisFrame) {
        OSReport(strAttackActionable, player.playerNumber);
        OSReport("action\n  - Prev Act/Subact: %s/%s\n  - Cur Act/Subact: %s/%s\n",
            actionName(player.prev->action), player.prev->subactionStr(),
            actionName(currentAction), player.current->subactionStr()
        );
        player.becameActionableOnFrame = frameCounter;
        return true;
    }

    if (player.current->canCancel) {
        OSReport(strAttackActionable, player.playerNumber);
        OSReport("%s\n", "ftCancelModule");
        player.becameActionableOnFrame = frameCounter;
        return true;
    }

    // Airborne IASA doesn't imply actionability, just that you'll
    // get an autocancel. Technically there are also iasa frames
    // in the air, that are separate from the AC window,
    // but most people conflate the two.
    if (player.inIasa() && !(player.current->isAirborne)) {
        OSReport(strAttackActionable, player.playerNumber);
        OSReport("via IASA\n");
        player.becameActionableOnFrame = frameCounter + 1;
        return true;
    }

    return false;
}


bool PlayerData::inAttackState() const {
    return isAttackingAction(this->current->action);
};

bool PlayerData::inActionableState() const {
    return isDefinitelyActionable(this->current->action);
};


const char* strDefActionable = "TGT %d now actionable via ";
bool PlayerData::resolveTargetActionable() {
    PlayerData& target = *this;

    if (target.becameActionableOnFrame != (u32)(-1)) {
        return true; // already happened.
    }

    if (target.current->occupiedActionableStateThisFrame) {
        OSReport(strDefActionable, target.playerNumber);
        OSReport("action\n\t- Prev Act/Sub: %s/%s\n\t- Cur Act/Sub: %s/%s\n",
            actionName(target.prev->action), target.prev->subactionName,
            actionName(target.current->action), target.current->subactionName
        );
        target.becameActionableOnFrame = frameCounter;
        return true;
    }

    if (target.current->hitstun == 0 && target.current->shieldstun == 0) {
        OSReport(strDefActionable, target.playerNumber);
        OSReport("hitstun\n");
        target.becameActionableOnFrame = frameCounter;
        return true;

    }

    return false;
}


Popup* PlayerData::createPopup(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);

    Popup* popup = new Popup();
    popup->vprintf(fmt, args);
    playerPopups[playerNumber].append(*popup);

    va_end(args);
    return popup;

}

#pragma region current_aliases
u16 PlayerData::action() const {
    return (u16)this->current->action;
}

const char* PlayerData::actionStr() const {
    return actionName(this->action());
}

u32 PlayerData::raLowBits() const {
    return current->lowRABits;
}

u16 PlayerData::subaction() const {
    return current->subaction;
}

const char* PlayerDataOnFrame::subactionStr() const {
    if (subactionName == NULL || subactionName == (char*)0xCCCCCCCC) {
        return "UNKNOWN";
    } else {
        return subactionName;
    };

}
const char* PlayerData::subactionStr() const {
    return current->subactionStr();
}

float PlayerData::subactionFrame() const {
    return current->subactionFrame;
}

float PlayerData::subactionTotalFrames() const {
    return current->subactionTotalFrames;
}
#pragma endregion

#pragma region predicates

bool PlayerData::didEnterShield() const {
    return (didActionChange && this->current->action == ACTION_GUARDON);
}

bool PlayerData::didReceiveHitstun() const {
        return current->hitstun != 0 && (current->hitstun > prev->hitstun);
}

bool PlayerData::didReceiveShieldstun() const {
        return current->shieldstun != 0 && (current->shieldstun > prev->shieldstun);
}

bool PlayerData::canCancel() const {
    return current->canCancel && !(prev->canCancel);
}

/* TODO: Maybe use event system for this.*/
bool PlayerData::didSubactionChange() const {
    return (prev->subaction != current->subaction);
}

bool PlayerDataOnFrame::isShielding() const {
    return (action == ACTION_GUARD || action == ACTION_GUARDDAMAGE || action == ACTION_GUARDON);
}

bool PlayerDataOnFrame::inIasa() const {
    const InterruptGroupStates& ig = interruptGroups;
    if (isAirborne == true) {
        // IASA in the air is more for people being able to cancel tumble rather than something that's used a lot.
        // It comes up occasionally with peach float dair.
        return canAutocancel();
    } else {
        return inGroundedIasa();
    }
}

bool PlayerDataOnFrame::inGroundedIasa() const {
    const InterruptGroupStates& ig = interruptGroups;
    return !isAirborne && (canCancel || (ig.groundAttack | ig.groundDodge | ig.groundGrab | ig.groundGuard | ig.groundJump | ig.groundSpecial));
}

bool PlayerData::inIasa() const {
    return current->inIasa();
}

bool PlayerData::inGroundedIasa() const
{
    return current->inGroundedIasa();
}

bool PlayerData::isGroundedActionable()
{
    bool _groundedActionable = _groundedActionable || (current->occupiedActionableStateThisFrame);
    _groundedActionable = _groundedActionable || (inGroundedIasa());

    return _groundedActionable;
}

bool PlayerDataOnFrame::canAutocancel() const {
    return getLowRABit(RA_BIT_ENABLE_LANDING_LAG) != true;
}

void PlayerData::prepareNextFrame() {
    PlayerDataOnFrame* tmp = current;

    current = prev;
    prev = tmp;

    memset(current, 0, sizeof(PlayerDataOnFrame));
    // These get reset every time there's an action change.
    memcpy(&current->interruptGroups, &prev->interruptGroups, sizeof(current->interruptGroups));
    current->canCancel = prev->canCancel;
    current->action = prev->action;
    current->actionName = prev->actionName;
    current->isAirborne = prev->isAirborne;
    current->occupiedGroundedIasaThisFrame = false;
    current->occupiedGroundedIasaThisFrame = prev->inGroundedIasa(); // This method uses the field it is setting.
    current->ledgeIntan = 0;
    didActionChange = false;
    current->occupiedActionableStateThisFrame = isDefinitelyActionable(prev->action);
    current->occupiedWaitingStateThisFrame = (prev->action == ACTION_WAIT);

    _computedGroundedActionable = false;
}

void PlayerData::setAction(u16 newAction) {
    current->action = newAction;
    current->actionName = actionName(newAction);
    current->canCancel = false;
    current->isAirborne = false;
    didActionChange = true;
    if (isDefinitelyActionable(newAction)) {
        occupiedActionableStateThisFrame = true;
    }

    if (current->inGroundedIasa()) {
        current->occupiedGroundedIasaThisFrame = true;
    }

    if (newAction == ACTION_WAIT) {
        occupiedWaitingStateThisFrame = true;
    }
    memset(&current->interruptGroups, 0, sizeof(InterruptGroupStates));
}

void PlayerData::initLedgeTechWatcher(Fighter& fighter)
{
    LedgeTechWatcher* ledgeTechWatcher = new LedgeTechWatcher(&allPlayerData[playerNumber]);
    ledgeTechWatcher->registerWith(&fighter);
    allPlayerData[playerNumber].ledgeTechWatcher = ledgeTechWatcher;
}

#pragma endregion



} // namespace
#pragma once

#include <OS/OSError.h>
#include <stdio.h>
#include <stdarg.h>
#include <memory.h>

#include "pp/playerdata.h"
#include "pp/anim_cmd_watcher.h"
#include "pp/status_change_watcher.h"
#include "pp/ledge_tech.h"
#include "pp/graphics/text_printer.h"
#include "pp/ui.h"
#include "pp/main.h"

namespace PP {

extern u32 frameCounter;

PlayerData* allPlayerData = NULL;

PlayerData::PlayerData() {
    becameActionableOnFrame = -1;
    lastAttackEndedOnFrame = -1; // currently unused.
    attackStartFrame = -1;

    prev = new PlayerDataOnFrame();
    current = new PlayerDataOnFrame();
    fighterName = NULL;
    attackTarget = NULL;

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
    enableLedgeTechAirdodgeAngle = false;

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

void PlayerData::setAction(u16 newAction) {
    current->action = newAction;
    current->actionName = actionName(newAction);
    current->canCancel = false;
    current->isAirborne = false;
    didActionChange = true;
    if (isDefinitelyActionable(newAction)) {
        current->occupiedActionableState = true;
    }

    if (current->inGroundedIasa()) {
        current->occupiedGroundedIasa = true;
    }

    if (newAction == ACTION_WAIT) {
        current->occupiedWaitingState = true;
    }
    memset(&current->interruptGroups, 0, sizeof(InterruptGroupStates));
}

#pragma region display

int PlayerData::debugStr(char* buffer) const {
    PlayerDataOnFrame& f = PP_IS_PAUSED ? *(this->prev) : *(this->current);
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
    PlayerDataOnFrame&f = PP_IS_PAUSED ? *(this->prev) : *(this->current);
    return printBinaryString(buffer, f.lowRABits);
}

void PlayerData::printFighterState() const {
    this->debugStr(strManipBuffer);
    printer.renderPre = true;
    printer.boxPadding = 10;
    printer.boxBgColor = 0x00000000;
    printer.boxHighlightColor = 0x00000000;
    printer.boxBorderWidth = 0;
    printer.opacity = 0xBB;
    printer.setTextColor(0xFFFFFFFF);
    printer.setScale(punchMenu.baseFontScale, punchMenu.fontScaleMultiplier, punchMenu.lineHeightMultiplier);
    printer.setTextBorder(0x000000FF, 1.0f);
    printer.setPosition(35,35);

    printer.begin();
    printer.print(strManipBuffer);
    printer.renderBoundingBox();
}

Popup* PlayerData::createPopup(const char* fmt, ...)
{
    if (playerPopups[playerNumber].length >= GlobalSettings::maxOsdLimit) {
        playerPopups->removeEnd();
    }

    va_list args;
    va_start(args, fmt);

    Popup* popup = new Popup();
    popup->vprintf(fmt, args);
    playerPopups[playerNumber].prepend(*popup);

    va_end(args);
    return popup;
}

#pragma endregion

#pragma region targeting

void PlayerData::resetTargeting() {
    attackTarget = NULL;
    becameActionableOnFrame = -1;
    advantageBonusCounter = 0;
    attackStartFrame = frameCounter;
    isAttackingShield = false;
    isAttackingFighter = false;
}

const char* strAttackActionable = "[f%d] ATKR %d now actionable via ";
bool PlayerData::resolvePlayerActionable() {
    PlayerData& player = *this;
    int currentAction = player.current->action;
    if (player.becameActionableOnFrame != (u32)(-1)) {
        if (player.didActionChange && isAttackingAction(current->action)) {
            player.becameActionableOnFrame = -1;
        } else {
            return true; // already became actionable..
        }
    }

    if (current->occupiedActionableState) {
        OSReport(strAttackActionable, frameCounter, player.playerNumber);
        OSReport("action\n  - Prev Act/Subact: %s/%s\n  - Cur Act/Subact: %s/%s\n",
            actionName(player.prev->action), player.prev->subactionStr(),
            actionName(currentAction), player.current->subactionStr()
        );
        player.becameActionableOnFrame = frameCounter;
        return true;
    }

    if (player.current->canCancel) {
        OSReport(strAttackActionable, frameCounter, player.playerNumber);
        OSReport("%s\n", "ftCancelModule");
        player.becameActionableOnFrame = frameCounter;
        return true;
    }

    // Airborne IASA doesn't imply actionability, just that you'll
    // get an autocancel. Technically there are also iasa frames
    // in the air, that are separate from the AC window,
    // but most people conflate the two.
    if (player.current->occupiedGroundedIasa) {
        OSReport(strAttackActionable, frameCounter, player.playerNumber);
        OSReport("via IASA\n");
        player.becameActionableOnFrame = frameCounter;
        return true;
    }

    return false;
}

const char* strDefActionable = "[f%d] TGT %d now actionable via ";
bool PlayerData::resolveTargetActionable() {
    PlayerData& target = *this;
    
    if (target.didActionChange && target.current->action == ACTION_GUARDDAMAGE) {
        target.becameActionableOnFrame = -1;
        return false;
    }

    if (target.becameActionableOnFrame == -1) {
        if (target.current->hitstun == 0 && target.current->shieldstun == 0) {
            OSReport(strDefActionable, frameCounter, target.playerNumber);
            OSReport("hitstun\n");
            if (target.becameActionableOnFrame == -1) {
                target.becameActionableOnFrame = frameCounter;
            }
        } else if (target.current->occupiedActionableState) {
            OSReport(strDefActionable, frameCounter, target.playerNumber);
            OSReport("action\n\t- Prev Act/Sub: %s/%s\n\t- Cur Act/Sub: %s/%s\n",
                actionName(target.prev->action), target.prev->subactionName,
                actionName(target.current->action), target.current->subactionName
            );
            if (target.becameActionableOnFrame == -1) {
                target.becameActionableOnFrame = frameCounter;
            }
        }
    }

    if (target.becameActionableOnFrame != -1 && 
        (frameCounter - target.becameActionableOnFrame) > GlobalSettings::shieldActionabilityTolerance) {
        return true;
    } else {
        return false;
    }

}
#pragma endregion

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

bool PlayerData::inAttackState() const {
    return isAttackingAction(this->current->action);
};

bool PlayerData::inActionableState() const {
    return isDefinitelyActionable(this->current->action);
};

bool PlayerData::didEnterShield() const {
    return (didActionChange && this->current->action == ACTION_GUARDON);
}

bool PlayerData::hasStartedAttack() const {
    /* On the frame an attack has begun, the attacker is considered actionable
     * since they were able to input an attack that frame. Thus we check to make
     * sure we're not checking actionability on the same frame.
     */
    return (attackStartFrame >= 0 && attackStartFrame != frameCounter);
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

bool PlayerData::inIasa() const {
    return current->inIasa();
}

bool PlayerData::inGroundedIasa() const
{
    return current->inGroundedIasa();
}

bool PlayerData::isGroundedActionable()
{
    return (
        current->occupiedActionableState
        || current->occupiedGroundedIasa
    );
}

#pragma endregion

#pragma region lifecycle
void PlayerData::preFrame() {
    if (!didActionChange) {
        current->occupiedActionableState = isDefinitelyActionable(current->action);
        current->occupiedGroundedIasa = current->inGroundedIasa(); // This method uses the field it is setting.
        current->occupiedWaitingState = (current->action == ACTION_WAIT);
    }
}

void PlayerData::prepareNextFrame() {
    PlayerDataOnFrame* tmp = current;

    current = prev;
    prev = tmp;

    memset(current, 0, sizeof(PlayerDataOnFrame));
    // These get reset every time there's an action change.
    memcpy(&current->interruptGroups, &prev->interruptGroups, sizeof(current->interruptGroups));
    current->action = prev->action;
    current->actionName = prev->actionName;
    current->isAirborne = prev->isAirborne;
    current->ledgeIntan = 0;

    didActionChange = false;
}

void PlayerData::initLedgeTechWatcher(Fighter& fighter)
{
    LedgeTechWatcher* ledgeTechWatcher = new LedgeTechWatcher(&allPlayerData[playerNumber]);
    ledgeTechWatcher->registerWith(&fighter);
    allPlayerData[playerNumber].ledgeTechWatcher = ledgeTechWatcher;
}
#pragma endregion

} // namespace
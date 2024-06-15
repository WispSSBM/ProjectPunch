#include "pp/ledge_tech.h"

#include "pp/common.h"
#include "pp/actions.h"
#include "pp/main.h"
#include "pp/graphics/draw.h"
#include "pp/graphics/drawable.h"
#include "pp/graphics/text_printer.h"

#include <gf/gf_scene.h>
#include <it/it_manager.h>

/* TODO: Clean up all the fighterFrame stuff. */

PP::LedgeTechDisplayDrawable* PP::LedgeTechDisplayDrawable::instance = NULL;

const char* g_frameTypeLabels[] = {
    "Ledge", 
    "Falling",
    "Jumping",
    "T.Winner",
    "Airdodge",
    "Landing",
    "Attack",
    "Galint",
    "Other"
};

bool PP::LedgeTechWatcher::isEnabled(const PlayerData& playerData) { 
    return playerData.enableLedgeTechFrameDisplay
        || playerData.enableLedgeTechGalintPopup
        || playerData.enableLedgeTechFramesOnLedgePopup
        || playerData.enableLedgeTechAirdodgeAngle;
}

/* 
 * In all of these watchers, it's important to note that the current frame is
 * the first frame of the new action. This means that if you want to calculate the
 * duration of a previous state you are at current frame - 1.
 * 
 * Additionally, all of these handlers run at the *end* of the frame. 
*/

void PP::LedgeTechWatcher::didCatchCliff(int fighterFrame, soModuleAccesser& modules) {
    _isOnLedge = true;
    _wasReset = false;
    _cliffCatchStartFrame = fighterFrame;
    DEBUG_LEDGETECH("LEDGETECH f%d: Started cliffCatch\n", fighterFrame);
}

void PP::LedgeTechWatcher::didStartCliffWait(int fighterFrame, soModuleAccesser& modules)
{
    if (!_isOnLedge) {
        _isOnLedge = true;
        _wasReset = false;
        _cliffCatchStartFrame = fighterFrame;
    }

    _cliffWaitStartFrame = fighterFrame;
    _cliffWaitFrames = 0;
    _currentFrameCounter = &_cliffWaitFrames;
    DEBUG_LEDGETECH("LEDGETECH f%d: Started cliffWait\n", fighterFrame);
}

void PP::LedgeTechWatcher::didLeaveCliff(int fighterFrame, soModuleAccesser& modules, int newStatusKind)
{
    int ledgeFrames = fighterFrame - _cliffCatchStartFrame;
    int actionableLedgeFrames = _cliffWaitFrames;
    if (!(newStatusKind >= ACTION_CLIFFCATCHSTART && newStatusKind <= ACTION_CLIFFJUMPSTART)) {
        /* Falling off the ledge instead of builtin ledge options isn't actionable until frame 3 of cliffwait. */
        actionableLedgeFrames -= 2;
    }

    if (actionableLedgeFrames > GlobalSettings::maxLedgedashVizFrames) {
        return; // This will cause overflows, but shouldn't be able to happen.
    }

    for (int i = 0; i < actionableLedgeFrames; i++) {
        this->_framesList[i] = LEDGE_FT_CLIFFWAIT;
    }
    _framesRecorded = actionableLedgeFrames;

    if (playerData->enableLedgeTechFramesOnLedgePopup) {
        playerData->createPopup("OffLedge: Frame %d\n", actionableLedgeFrames + 1);
    }

    DEBUG_LEDGETECH("LEDGETECH f%d: Left ledge on frame relative to CliffCatch %d (%d)\n", ledgeFrames, actionableLedgeFrames);
    _isOnLedge = false;
}

void PP::LedgeTechWatcher::didStartCliffJump(int fighterFrame, soModuleAccesser& modules) {
    DEBUG_LEDGETECH("LEDGETECH f%d: Started cliffjump on frame %d\n", fighterFrame, fighterFrame - _cliffWaitStartFrame);
}

void PP::LedgeTechWatcher::didStartCliffRoll(int fighterFrame, soModuleAccesser& modules) {
    DEBUG_LEDGETECH("LEDGETECH f%d: Started cliffroll on frame %d\n", fighterFrame, fighterFrame - _cliffWaitStartFrame);
}

void PP::LedgeTechWatcher::didStartCliffAttack(int fighterFrame, soModuleAccesser& modules) {
    DEBUG_LEDGETECH("LEDGETECH f%d: Started cliffattack on frame %d\n", fighterFrame, fighterFrame - _cliffWaitStartFrame);
}

void PP::LedgeTechWatcher::didStartCliffClimb(int fighterFrame, soModuleAccesser& modules) {
    DEBUG_LEDGETECH("LEDGETECH f%d: Started neutral getup on frame %d\n", fighterFrame, fighterFrame - _cliffWaitStartFrame);
}

void PP::LedgeTechWatcher::didStartFall(int fighterFrame, soModuleAccesser& modules) {}

void PP::LedgeTechWatcher::didStartAirdodge(int fighterFrame, soModuleAccesser& modules) 
{
    _showAirdodgeNextUpdate = true;
    DEBUG_LEDGETECH("LEDGETECH f%d: Started airdodge on frame %d\n", fighterFrame, fighterFrame - _cliffWaitStartFrame);
}

void PP::LedgeTechWatcher::didFinishLedgeDash(int fighterFrame, soModuleAccesser& modules)
{
    DEBUG_LEDGETECH(
        "LEDGETECH f%d: Ledgedash detected\n"
        "\tGALINT: %d\n"
        "\ton ledge: %d\n"
        "\tfalling: %d\n"
        "\ttourney winner startup: %d\n"
        "\tjumping: %d\n"
        "\tair dodging: %d\n"
        "\tlanding: %d\n"
        "\tother: %d\n",
        frameCounter, playerData->current->ledgeIntan,
        _cliffWaitFrames, _fallingFrames,
        _twStartupFrames,
        _jumpingFrames, _airDodgeFrames,
        _specialLandFrames, _otherFrames
    );

    if (playerData->enableLedgeTechFrameDisplay) {
        drawLedgeDash();
    }

    if (playerData->enableLedgeTechGalintPopup) {
        playerData->createPopup("GALINT: %d\n", playerData->current->ledgeIntan);
    }

    _didShowLedgeDash = true;
}

void PP::LedgeTechWatcher::notifyEventChangeStatus(int statusKind, int prevStatusKind, soStatusData* statusData, soModuleAccesser* moduleAccesser)
{
    if (playerData == NULL || !isEnabled(*playerData)) { return; }

    if (statusKind == ACTION_CLIFFCATCHSTART ) {
        didCatchCliff(frameCounter, *moduleAccesser);
    } else if (statusKind == ACTION_CLIFFWAIT) {
        // Tether grabs can skip cliffcatch.
        didStartCliffWait(frameCounter, *moduleAccesser);
    } 

    if (_wasReset) { return; }
    
    if (prevStatusKind == ACTION_CLIFFWAIT) {
        didLeaveCliff(frameCounter, *moduleAccesser, statusKind);

        if (statusKind == ACTION_CLIFFESCAPE) {
            didStartCliffRoll(frameCounter, *moduleAccesser);
        } else if  (statusKind == ACTION_CLIFFATTACK) {
            didStartCliffAttack(frameCounter, *moduleAccesser);
        } else if (statusKind == ACTION_CLIFFCLIMB) {
            didStartCliffClimb(frameCounter, *moduleAccesser);
        } else if (statusKind == ACTION_FALL) {
            didStartFall(frameCounter, *moduleAccesser);
        }
    }

    if (statusKind == ACTION_ESCAPEAIR) {
        didStartAirdodge(frameCounter, *moduleAccesser);
    }

    updateCurrentFrameCounter(statusKind);
}

void PP::LedgeTechWatcher::updateCurrentFrameCounter(int statusKind)
{
    switch(statusKind) {
    case ACTION_ESCAPEAIR:
        _currentFrameCounter = &_airDodgeFrames; 
        _currentFrameType = LEDGE_FT_AIRDODGE;
        return;
    case ACTION_FALL: 
        _currentFrameCounter = &_fallingFrames;
        _currentFrameType = LEDGE_FT_FALLING;
        return;
    case ACTION_CLIFFWAIT:
        _currentFrameCounter = &_cliffWaitFrames;
        _currentFrameType = LEDGE_FT_CLIFFWAIT;
        return;
    case ACTION_CLIFFJUMPSTART: 
        _currentFrameCounter = &_twStartupFrames; 
        _currentFrameType = LEDGE_FT_TWSTARTUP;
        return;
    case ACTION_JUMPAERIAL:
    case ACTION_CLIFFJUMPEND: 
        _currentFrameCounter = &_jumpingFrames;
        _currentFrameType = LEDGE_FT_JUMPING;
        return;
    case ACTION_LANDINGHEAVY:
    case ACTION_LANDINGLIGHT:
    case ACTION_LANDINGLAGAERIALATTACK:
    case ACTION_LANDINGFALLSPECIAL:
        _currentFrameCounter = &_specialLandFrames;
        _currentFrameType = LEDGE_FT_LANDING;
        return;
    }

    if (statusKind >= ACTION_JAB && statusKind <= ACTION_CATCH) {
        _currentFrameCounter = &_attackingFrames;
        _currentFrameType = LEDGE_FT_ATTACK;
    } else {
        _currentFrameCounter = &_otherFrames;
        _currentFrameType = LEDGE_FT_OTHER;
    }
}

bool PP::LedgeTechWatcher::shouldStopWatching() {
    return _totalFrames >= GlobalSettings::maxLedgedashVizFrames || playerData->current->action == ACTION_UNLOADED;
}

void PP::LedgeTechWatcher::resetState() {
    _totalFrames = -1;
    _totalFramesPrev = -1;
    _cliffCatchStartFrame = -1;
    _cliffWaitStartFrame = -1;
    _isOnLedge = false;
    _framesRecorded = 0;

    _cliffWaitFrames = 0;
    _fallingFrames = 0;
    _twStartupFrames = 0;
    _jumpingFrames = 0;
    _airDodgeFrames = 0;
    _specialLandFrames = 0;
    _otherFrames = 0;

    _showAirdodgeNextUpdate = false;
    _currentFrameCounter = &_cliffWaitFrames;
    _currentFrameType = LEDGE_FT_CLIFFWAIT;
    _didShowLedgeDash = false;
    _wasReset = true;
    DEBUG_LEDGETECH("LEDGETECH f%d: Resetting status.\n", frameCounter);
}

void PP::LedgeTechDisplayDrawable::addFrame(FrameType frameType) {
    Color color = LedgeTechWatcher::getFrameColor(frameType);
    int idx = this->frames->size();
    float startPosX = this->framesStartPosX + (idx*LEDGEDASH_BOX_WIDTH);
    float endPosX = startPosX + LEDGEDASH_BOX_WIDTH;

    LedgeTechFrameDrawable* ltfd = new LedgeTechFrameDrawable(
        color.gxColor, 0, 9999,
        LEDGEDASH_START_Y, LEDGEDASH_START_Y + LEDGEDASH_BOX_HEIGHT,
        startPosX, endPosX
    );

    this->frames->push(ltfd);
}

PP::Color PP::LedgeTechWatcher::getFrameColor(FrameType frameType) {
    switch(frameType) {
        case LEDGE_FT_CLIFFWAIT: return PP_COLOR_GREY;
        case LEDGE_FT_FALLING:   return PP_COLOR_GREEN;
        case LEDGE_FT_TWSTARTUP: return PP_COLOR_CYAN;
        case LEDGE_FT_JUMPING:   return PP_COLOR_YELLOW;
        case LEDGE_FT_LANDING:   return PP_COLOR_RED;
        case LEDGE_FT_GALINT:    return PP_COLOR_BLUE;
        case LEDGE_FT_OTHER:     return PP_COLOR_ORANGE;
        case LEDGE_FT_ATTACK:    return PP_COLOR_LIGHT_GREY;
        case LEDGE_FT_AIRDODGE:  return PP_COLOR_MAGENTA;
        default: return PP_COLOR_WHITE;
    }
}

void PP::LedgeTechWatcher::drawLedgeDash() {
    int frameIdx, i, j, centerScreenX, totalWidth;

    totalWidth = _framesRecorded * LEDGEDASH_BOX_WIDTH;
    float startPosX = PP_CENTER_SCREEN_X - (totalWidth / 2.0f);

    LedgeTechDisplayDrawable* ltd = new LedgeTechDisplayDrawable(
        _framesRecorded + max(0, playerData->current->ledgeIntan),
        0, _visualDurationFrames, LEDGEDASH_START_Y - 50, LEDGEDASH_LEGEND_START_X, opacity
    );

    for (i = 0; i < _framesRecorded; i++) {
        ltd->addFrame(this->_framesList[i]);
    }

    for (int j = 0; j < playerData->current->ledgeIntan; j++) {
        ltd->addFrame(LEDGE_FT_GALINT);
    }

    LedgeTechDisplayDrawable::setInstance(ltd);
}

namespace PP {
    bool detectLedgeDash(PlayerData& playerData) {
        DEBUG_LEDGETECH("LEDGETECH f%d: Airborne %d GroundedIASA %d ActionableState %d\n",
            frameCounter,
            playerData.current->isAirborne,
            playerData.current->occupiedGroundedIasa,
            playerData.current->occupiedActionableState
        );

        return (
            !(playerData.current->isAirborne) 
            && (
                playerData.current->occupiedGroundedIasa
                || playerData.current->occupiedActionableState));
    }
};

void PP::LedgeTechWatcher::process(Fighter& fighter)
{
    if ( !_wasReset && _cliffWaitStartFrame != -1 && !PP_IS_PAUSED) {
        int fighterFrame = frameCounter;
        DEBUG_LEDGETECH("LEDGETECH f%d: action: 0x%X XLU: %d \n", frameCounter, playerData->current->action, playerData->current->ledgeIntan);

        _totalFrames = (fighterFrame - _cliffWaitStartFrame) + 1;
        if (_totalFramesPrev < 0) {
            _totalFramesPrev = _totalFrames - 1;
        }

        if (_showAirdodgeNextUpdate) {
            _showAirdodgeNextUpdate = false;

            if (playerData->enableLedgeTechAirdodgeAngle) {
                playerData->createPopup("Airdodge: %0.0fdeg\n", playerData->current->stick.quadrantDegrees());
            }
        }

        if (shouldStopWatching()) {
            resetState();
            return;
        }

        if (!_didShowLedgeDash && detectLedgeDash(*playerData)) {
            didFinishLedgeDash(fighterFrame, *fighter.m_moduleAccesser);
            resetState();
            return;
        }

        int frameDiff = _totalFrames - _totalFramesPrev;;
        _totalFramesPrev = _totalFrames;

        if (frameDiff == 1) {
            *_currentFrameCounter += 1;

            DEBUG_LEDGETECH("LEDGETECH f%d: setting frame %d = type %d\n", frameCounter, _framesRecorded, _currentFrameType);
            _framesList[_framesRecorded++] = _currentFrameType;
        } else if (frameDiff != 0) { 
            OSReport("WARNING f%d: LEDGE TECH WATCHER INCREMENTED BY %d frames\n", frameCounter, frameDiff);
        }
    }
}

void PP::LedgeTechFrameDrawable::draw() {
    Graphics::draw2DRectangle(color, top, bottom, left-1, right, 0);
    Graphics::draw2DRectOutline(Color(0x000000FF).gxColor, top, bottom, left-1, right, 6);
}

/* static */ void PP::LedgeTechDisplayDrawable::setInstance(LedgeTechDisplayDrawable* newInstance) {
    if (instance != NULL) {
        delete instance;
    }

    instance = newInstance;
}

/* static */ void PP::LedgeTechDisplayDrawable::drawInstance() {
    if (instance != NULL) {
        Graphics::drawItem(instance);

        if (instance->lifeTime <= 0) {
            setInstance(NULL);
        }
    }
}

void PP::LedgeTechDisplayDrawable::draw() {
    Coord2DF pos = Coord2DF(left, top);

    for (int i = 0; i < sizeof(g_frameTypeLabels)/4; i++) {
        Color color = LedgeTechWatcher::getFrameColor((FrameType)i);
        color.a = opacity;

        LedgeTechFrameDrawable(
            color.gxColor,
            0, 0,
            pos.y, pos.y + LEDGEDASH_BOX_HEIGHT,
            pos.x, pos.x + LEDGEDASH_BOX_WIDTH
        ).draw();

        pos.x += LEDGEDASH_LEGEND_ITEM_PADDING;
    }

    /* Frames */
    for (int i = 0; i < this->frames->size(); i++) {
        LedgeTechFrameDrawable* frame = reinterpret_cast<LedgeTechFrameDrawable*>((*this->frames)[i]);
        frame->draw();
    }

    pos = Coord2DF(left, top);
    Graphics::TextPrinter& printer = Graphics::printer;
    printer.renderPre = true;

    printer.boxBgColor = 0x00000000;
    printer.boxBorderColor = 0;
    printer.boxHighlightColor = 0;
    printer.boxBorderWidth = 0;

    printer.lineHeight = 10;
    printer.setTextBorder(1);
    printer.textBorderColor = 0x000000FF;
    printer.setScale(gPopupConfig.fontScale, gPopupConfig.fontScaleMult, gPopupConfig.lineHeightMult);
    printer.opacity = opacity;

    printer.begin();
    for (int i = 0; i < sizeof(g_frameTypeLabels)/4; i++) {
        printer.setPosition(
            pos.x - 15,
            pos.y + LEDGEDASH_BOX_HEIGHT + LEDGEDASH_LEGEND_INTERNAL_PADDING
        );
        printer.print(g_frameTypeLabels[i]);
        pos.x += LEDGEDASH_LEGEND_ITEM_PADDING;
    }
    printer.renderBoundingBox();

    Graphics::setupDrawPrimitives();
    Graphics::start2DDraw();
}

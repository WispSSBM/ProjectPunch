#include "pp/ledge_tech.h"

#include "pp/common.h"
#include "pp/actions.h"
#include "pp/graphics/draw.h"
#include "pp/graphics/drawable.h"

#include <it/it_manager.h>

#define LA_INT_FRAME_COUNT 78


void PP::LedgeTechWatcher::didCatchCliff(int fighterFrame)
{
    resetState();
    _isOnLedge = true;
    _cliffWaitStartFrame = fighterFrame;
}

void PP::LedgeTechWatcher::didLeaveCliff(int fighterFrame)
{
    DEBUG_LEDGETECH("Left ledge on frame %d\n", fighterFrame - _cliffWaitStartFrame);
    _isOnLedge = false;
}

void PP::LedgeTechWatcher::didFinishLedgeDash(int fighterFrame)
{
    DEBUG_LEDGETECH(
        "Ledgedash detected on frame %d:\n"
        "\tGALINT: %d\n"
        "\ton ledge: %d\n"
        "\tfalling: %d\n"
        "\ttourney winner startup: %d\n"
        "\tjumping: %d\n"
        "\tair dodging: %d\n"
        "\tlanding: %d\n"
        "\tother: %d\n",
        fighterFrame, _remainingLedgeIntan,
        _cliffWaitFrames, _fallingFrames,
        _twStartupFrames,
        _jumpingFrames, _airDodgeFrames,
        _specialLandFrames, _otherFrames
    );
    drawLedgeDash();
    Popup& popup = *playerData->createPopup();
    popup.printf("GALINT: %d\n", _remainingLedgeIntan);
    _didShowLedgeDash = true;
}

void PP::LedgeTechWatcher::notifyEventChangeStatus(int statusKind, int prevStatusKind, soStatusData* statusData, soModuleAccesser* moduleAccesser)
{
    if (!isEnabled()) { return; }

    int fighterFrame = itManager::getInstance()->m_framesIntoCurrentGame;

    if (prevStatusKind == ACTION_CLIFFCATCH && statusKind == ACTION_CLIFFWAIT) {
        didCatchCliff(fighterFrame);
        return;
    } 

    updateCurrentFrameCounter(statusKind);
    
    
    if (prevStatusKind == ACTION_CLIFFWAIT) {
        didLeaveCliff(fighterFrame);
    }
}

void PP::LedgeTechWatcher::updateCurrentFrameCounter(int statusKind)
{
    switch(statusKind) {
    case ACTION_ESCAPEAIR:
        _currentFrameCounter = &_airDodgeFrames; 
        _currentFrameType = LEDGE_FT_AIRDODGE;
        break;
    case ACTION_FALL: 
        _currentFrameCounter = &_fallingFrames;
        _currentFrameType = LEDGE_FT_FALLING;
        break;
    case ACTION_CLIFFWAIT:
        _currentFrameCounter = &_cliffWaitFrames;
        _currentFrameType = LEDGE_FT_CLIFFWAIT;
    case ACTION_CLIFFJUMPSTART: 
        _currentFrameCounter = &_twStartupFrames; 
        _currentFrameType = LEDGE_FT_TWSTARTUP;
        break;
    case ACTION_JUMPAERIAL:
    case ACTION_CLIFFJUMPEND: 
        _currentFrameCounter = &_jumpingFrames;
        _currentFrameType = LEDGE_FT_JUMPING;
        break;
    case ACTION_LANDINGHEAVY:
    case ACTION_LANDINGLIGHT:
    case ACTION_LANDINGLAGAERIALATTACK:
    case ACTION_LANDINGFALLSPECIAL:
        _currentFrameCounter = &_specialLandFrames;
        _currentFrameType = LEDGE_FT_LANDING;
        break;
    default: 
        _currentFrameCounter = &_otherFrames;
        _currentFrameType = LEDGE_FT_OTHER;
    }
}

bool PP::LedgeTechWatcher::shouldStopWatching() {
    return _totalFrames >= 64;
}

void PP::LedgeTechWatcher::resetState() {
    _totalFrames = -1;
    _totalFramesPrev = -1;
    _cliffWaitStartFrame = -1;
    _remainingLedgeIntan = -1;
    _isOnLedge = false;

    _cliffWaitFrames = 0;
    _fallingFrames = 0;
    _twStartupFrames = 0;
    _jumpingFrames = 0;
    _airDodgeFrames = 0;
    _specialLandFrames = 0;
    _otherFrames = 0;

    _currentFrameCounter = &_cliffWaitFrames;
    _currentFrameType = LEDGE_FT_CLIFFWAIT;
    _didShowLedgeDash = false;
}

#define LEDGEDASH_START_Y 50.0f
#define LEDGEDASH_BOX_WIDTH 10
void PP::LedgeTechWatcher::drawFrame(float startAllPosX, int idx) const {
    float startPosX =  startAllPosX + (idx*LEDGEDASH_BOX_WIDTH);
    float endPosX = startPosX + LEDGEDASH_BOX_WIDTH;
    Color color = getFrameColor(_framesList[idx]);
    Graphics::Drawable* d = new Graphics::Rect(
        0, 120, color.gxColor,
        LEDGEDASH_START_Y, LEDGEDASH_START_Y + LEDGEDASH_BOX_WIDTH,
        startPosX, endPosX,
        true
    );

    Graphics::renderables.items.preFrame.push(d);

    d = new Graphics::RectOutline(
        0, 120, Color(0x000000FF).gxColor,
        LEDGEDASH_START_Y, LEDGEDASH_START_Y + LEDGEDASH_BOX_WIDTH,
        startPosX, endPosX,
        12,
        true
    );

    Graphics::renderables.items.preFrame.push(d);
}

PP::Color PP::LedgeTechWatcher::getFrameColor(FrameType frameType) const {
    switch(frameType) {
        case LEDGE_FT_CLIFFWAIT: return 0xFF00FFFF; // magenta
        case LEDGE_FT_FALLING: return 0xFF0000FF;   // red
        case LEDGE_FT_TWSTARTUP: return 0x555555FF; // grey
        case LEDGE_FT_JUMPING: return 0x00ccccFF; // cyan
        case LEDGE_FT_LANDING: return 0x555555FF; // grey
        case LEDGE_FT_GALINT: return 0x00FF00FF; // green
        case LEDGE_FT_OTHER: return 0xFFAA00FF; // orange
        case LEDGE_FT_AIRDODGE: return 0xad876cFF; // brown
        default: return 0xFFFFFFFF;
    }
}

void PP::LedgeTechWatcher::drawLedgeDash() {
    int frameIdx, i, j, centerScreenX, totalWidth;

    centerScreenX = 320;
    /*
    if (g_GameGlobal->m_record->m_menuData.m_isWidescreen) {
        centerScreenX = 426;
    }
    */
    totalWidth = _totalFrames * LEDGEDASH_BOX_WIDTH;
    float startPosX = centerScreenX - (totalWidth / 2.0f);

    for (i = 0; i < _totalFrames; i++) {
        drawFrame(startPosX, i);
    }

    for (int j = 0; j < _remainingLedgeIntan; j++) {
        _framesList[i+j] = LEDGE_FT_GALINT;
        drawFrame(startPosX, i+j);
    }
}

void PP::LedgeTechWatcher::process(Fighter& fighter)
{
    if (!isEnabled()) { return; }

    if ( _isOnLedge || _cliffWaitStartFrame != -1 ) {
        int fighterFrame = itManager::getInstance()->m_framesIntoCurrentGame;

        _totalFrames = (fighterFrame - _cliffWaitStartFrame) + 1;
        if (_totalFramesPrev == -1) {
            _totalFramesPrev = _totalFrames - 1;
        }
        DEBUG_LEDGETECH("Ledge tech frame info: fighterFrame: %d totalFrames: %d totalFramesPrev: %d\n", fighterFrame, _totalFrames, _totalFramesPrev);

        int frameDiff = _totalFrames - _totalFramesPrev;;
        _totalFramesPrev = _totalFrames;

        if (frameDiff == 1) {
            *_currentFrameCounter += 1;

            DEBUG_LEDGETECH("LEDGETECH: setting frame %d = type %d\n", _totalFrames-1, _currentFrameType);
            _framesList[_totalFrames-1] = _currentFrameType;
        } else if (frameDiff != 0) { 
            OSReport("WARNING: LEDGE TECH WATCHER INCREMENTED BY %d frames\n", _totalFrames - _totalFramesPrev);
        }

        _remainingLedgeIntan = 29 - _totalFrames;

        if (shouldStopWatching()) {
            resetState();
            return;
        }

        if (!_didShowLedgeDash && playerData->isGroundedActionable()) {
            didFinishLedgeDash(fighterFrame);
            resetState();
            return;
        }

        if (_remainingLedgeIntan >= 0) {
            /*
            float timeRemainingPct = (float)(_remainingLedgeIntan) / 29.0f;
            OSReport("Ledge intan: %d\n", _remainingLedgeIntan);
            // debugWorkModule(fighter);

            Color rectColor = 0x00FF00FF;
            rectColor.r = (1 - timeRemainingPct) * 255;
            Graphics::renderables.items.preFrame.push(new Graphics::Rect(
                0, 1, rectColor.gxColor,
                50, 80,
                50, 50 + (100 * timeRemainingPct),
                true
            ));
            */
        }

    }
}

#include "pp/ledge_tech.h"

#include "pp/common.h"
#include "pp/actions.h"
#include "pp/graphics/draw.h"
#include "pp/graphics/drawable.h"
#include "pp/graphics/text_printer.h"

#include <it/it_manager.h>

#define LA_INT_FRAME_COUNT 78

const char* g_frameTypeLabels[] = {
    "Ledge", 
    "Falling",
    "Jumping",
    "T.Winner",
    "Airdodge",
    "Attack",
    "Landing",
    "Galint",
    "Other"
};

void PP::LedgeTechWatcher::didCatchCliff(int fighterFrame) {
    resetState();
    _isOnLedge = true;
}

void PP::LedgeTechWatcher::didCatchCliffEnd(int fighterFrame)
{
    DEBUG_LEDGETECH("Left ledge on frame %d\n", fighterFrame - _cliffWaitStartFrame);
    _cliffWaitStartFrame = fighterFrame;
    _currentFrameCounter = &_cliffWaitFrames;
}

void PP::LedgeTechWatcher::didLeaveCliff(int fighterFrame)
{
    DEBUG_LEDGETECH("Left ledge on frame %d\n", fighterFrame - _cliffWaitStartFrame);
    _isOnLedge = false;
}

void PP::LedgeTechWatcher::didStartCliffJump(int fighterFrame) {
    DEBUG_LEDGETECH("Started cliffjump on frame %d\n", fighterFrame - _cliffWaitStartFrame);
}

void PP::LedgeTechWatcher::didStartCliffRoll(int fighterFrame) {
    DEBUG_LEDGETECH("Started cliffroll on rame %d\n", fighterFrame - _cliffWaitStartFrame);
}

void PP::LedgeTechWatcher::didStartCliffAttack(int fighterFrame) {
    DEBUG_LEDGETECH("Started cliffattack on frame %d\n", fighterFrame - _cliffWaitStartFrame);
}

void PP::LedgeTechWatcher::didStartCliffClimb(int fighterFrame) {
    DEBUG_LEDGETECH("Started neutral getup on frame %d\n", fighterFrame - _cliffWaitStartFrame);
}

void PP::LedgeTechWatcher::didStartFall(int fighterFrame) {}

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


    if (statusKind == ACTION_CLIFFCATCHSTART) {
        didCatchCliff(fighterFrame);
    }
    else if (statusKind == ACTION_CLIFFWAIT) {
        didCatchCliffEnd(fighterFrame);
    } 
    else if (prevStatusKind == ACTION_CLIFFWAIT) {
        didLeaveCliff(fighterFrame);

        if (statusKind == ACTION_CLIFFESCAPE) {
            didStartCliffRoll(fighterFrame);
        } else if  (statusKind == ACTION_CLIFFATTACK) {
            didStartCliffAttack(fighterFrame);
        } else if (statusKind == ACTION_CLIFFCLIMB) {
            didStartCliffClimb(fighterFrame);
        } else if (statusKind == ACTION_FALL) {
            didStartFall(fighterFrame);
        }
    }

    updateCurrentFrameCounter(statusKind);
    
    
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
        break;
    case ACTION_CLIFFJUMPSTART: 
        _currentFrameCounter = &_twStartupFrames; 
        _currentFrameType = LEDGE_FT_TWSTARTUP;
        break;
    case ACTION_JUMPAERIAL:
    case ACTION_CLIFFJUMPEND: 
        _currentFrameCounter = &_jumpingFrames;
        _currentFrameType = LEDGE_FT_JUMPING;
        break;
    case ACTION_AERIALATTACK:
        _currentFrameCounter = &_attackingFrames;
        _currentFrameType = LEDGE_FT_ATTACK;
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

void PP::LedgeTechWatcher::drawFrame(float startAllPosX, int idx) const {
    float startPosX =  startAllPosX + (idx*LEDGEDASH_BOX_WIDTH);
    float endPosX = startPosX + LEDGEDASH_BOX_WIDTH;
    Color color = getFrameColor(_framesList[idx]);

    Graphics::renderables.items.preFrame.push(new LedgeTechFrameDrawable(
        color.gxColor, 0, _visualDurationFrames,
        LEDGEDASH_START_Y, LEDGEDASH_START_Y + LEDGEDASH_BOX_HEIGHT,
        startPosX, endPosX
    ));
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

    totalWidth = _totalFrames * LEDGEDASH_BOX_WIDTH;
    float startPosX = PP_CENTER_SCREEN_X - (totalWidth / 2.0f);

    for (i = 0; i < _totalFrames; i++) {
        drawFrame(startPosX, i);
    }

    for (int j = 0; j < _remainingLedgeIntan; j++) {
        _framesList[i+j] = LEDGE_FT_GALINT;
        drawFrame(startPosX, i+j);
    }

    Graphics::renderables.items.preFrame.push(new LedgeTechLegendDrawable(
        0, _visualDurationFrames, LEDGEDASH_START_Y - 50, LEDGEDASH_LEGEND_START_X, opacity
    ));
}

void PP::LedgeTechWatcher::process(Fighter& fighter)
{
    if (!isEnabled()) { return; }

    if ( _isOnLedge && _cliffWaitStartFrame == -1) {
        /* Still in cliffcatch */
    }

    if ( _cliffWaitStartFrame != -1 ) {
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

void PP::LedgeTechFrameDrawable::draw() {
    Graphics::draw2DRectangle(color, top, bottom, left-1, right, 0);
    Graphics::draw2DRectOutline(Color(0x000000FF).gxColor, top, bottom, left-1, right, 6);
}

void PP::LedgeTechLegendDrawable::draw() {
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

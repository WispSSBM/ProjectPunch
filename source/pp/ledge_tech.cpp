#include "pp/actions.h"
#include "pp/ledge_tech.h"
#include "pp/graphics/draw.h"
#include "pp/graphics/drawable.h"

#define LA_INT_LEDGE_INTAN 2
#define LA_INT_FRAME_COUNT 78


int fighterFrameCounter(soGeneralWorkAbstract& laVars) {
    return laVars.getIntWork(78);
}

void PP::LedgeTechWatcher::didCatchCliff(int fighterFrame)
{
    _didGrabLedge = true;
    _cliffWaitStartFrame = fighterFrame;
    _currentFrameCounter = &_cliffWaitFrames;
}

void PP::LedgeTechWatcher::didLeaveCliffOther(int fighterFrame)
{
    OSReport("LeftCliff on frame %d\n", fighterFrame - _cliffWaitStartFrame);
    _didGrabLedge = false;
}

void PP::LedgeTechWatcher::notifyEventChangeStatus(int statusKind, int prevStatusKind, soStatusData* statusData, soModuleAccesser* moduleAccesser)
{
    if (!isEnabled()) { return; }

    soWorkManageModuleImpl& workModule = *static_cast<soWorkManageModuleImpl*>(moduleAccesser->getWorkManageModule());
    soGeneralWorkAbstract& laVars = *workModule.m_generalWorks[LA_VARS];
    int fighterFrame = fighterFrameCounter(laVars);

    if (prevStatusKind == ACTION_CLIFFCATCH && statusKind == ACTION_CLIFFWAIT) {
        didCatchCliff(fighterFrame);
        return;
    } 

    updateCurrentFrameCounter(statusKind);
    
    
    if (prevStatusKind == ACTION_CLIFFWAIT) {
        didLeaveCliffOther(fighterFrame);
    }

    if (prevStatusKind == ACTION_LANDINGFALLSPECIAL) {
        OSReport(
            "Ledgedash detected:\n"
            "\tGALINT: %d\n"
            "\ton ledge: %d\n"
            "\tfalling: %d\n"
            "\ttourney winner startup: %d\n"
            "\tjumping: %d\n"
            "\tair dodging: %d\n"
            "\tlanding: %d\n"
            "\tother: %d\n",
            _remainingLedgeIntan,
            _cliffWaitFrames, _fallingFrames,
            _twStartupFrames,
            _jumpingFrames, _airDodgeFrames,
            _specialLandFrames, _otherFrames
        );
        drawLedgeDash();
    }
}

void PP::LedgeTechWatcher::updateCurrentFrameCounter(int statusKind)
{
    switch(statusKind) {
    case ACTION_ESCAPEAIR: _currentFrameCounter = &_airDodgeFrames; break;
    case ACTION_FALL: _currentFrameCounter = &_fallingFrames; break;
    case ACTION_CLIFFJUMPSTART: _currentFrameCounter = &_twStartupFrames; break;
    case ACTION_JUMPAERIAL:
    case ACTION_CLIFFJUMPEND: _currentFrameCounter = &_jumpingFrames; break;
    case ACTION_LANDINGFALLSPECIAL: _currentFrameCounter = &_specialLandFrames; break;
    default: _currentFrameCounter = &_otherFrames;
    }
}

bool PP::LedgeTechWatcher::shouldStopWatching() {
    return _cliffElapsed >= 59;
}

void PP::LedgeTechWatcher::resetState() {
    _cliffElapsed = -1;
    _cliffElapsedPrev = -1;
    _cliffWaitStartFrame = -1;
    _remainingLedgeIntan = -1;

    _cliffWaitFrames = 0;
    _fallingFrames = 0;
    _twStartupFrames = 0;
    _jumpingFrames = 0;
    _airDodgeFrames = 0;
    _specialLandFrames = 0;
    _otherFrames = 0;
}

#define LEDGEDASH_START_X 50
#define LEDGEDASH_START_Y 50
#define LEDGEDASH_BOX_WIDTH 10
void PP::LedgeTechWatcher::drawRectForFrame(int idx, Color color) const {
    int startPosX = LEDGEDASH_START_X + (idx*LEDGEDASH_BOX_WIDTH);
    int endPosX = startPosX + LEDGEDASH_BOX_WIDTH;
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

void PP::LedgeTechWatcher::drawLedgeDash() const {
    int frameIdx = 0;
    int i;

    for (i = 0; i < _cliffWaitFrames; i++) {
        drawRectForFrame(frameIdx++, 0xFF00FFFF);
    }
    for (i = 0; i < _fallingFrames; i++) {
        drawRectForFrame(frameIdx++, 0xFF0000FF);
    }
    for (i = 0; i < _twStartupFrames; i++) {
        drawRectForFrame(frameIdx++, 0x555555FF);
    }
    for (i = 0; i < _jumpingFrames; i++) {
        drawRectForFrame(frameIdx++, 0x0000FFFF);
    }
    for (i = 0; i < _airDodgeFrames; i++) {
        drawRectForFrame(frameIdx++, 0x00CCCCFF);
    }
    for (i = 0; i < _specialLandFrames; i++) {
        drawRectForFrame(frameIdx++, 0x555555FF);
    }
    for (i = 0; i < _remainingLedgeIntan; i++) {
        drawRectForFrame(frameIdx++, 0x00FF00FF);
    }
}

soGeneralWorkSimple* getWorkVars(const Fighter& fighter, PP::WorkModuleVarType varType) {
    // assume the work manage module is of the normal impl type. Not technically typesafe, could use the proper brawl
    // api surface later.
    const soWorkManageModuleImpl& workModule = *reinterpret_cast<const soWorkManageModuleImpl*>(fighter.m_moduleAccesser->getWorkManageModule());
    return reinterpret_cast<soGeneralWorkSimple*>(workModule.m_generalWorks[varType]);
}
void debugWorkModule(const Fighter& fighter) {
    u32 i;

    soGeneralWorkSimple& raVars = *getWorkVars(fighter, PP::RA_VARS);
    soGeneralWorkSimple& laVars = *getWorkVars(fighter, PP::LA_VARS);
    int* raBasics = raVars.m_intWorks;
    float* raFloats = raVars.m_floatWorks;
    int* laBasics = laVars.m_intWorks;
    float* laFloats = laVars.m_floatWorks;

    for(i = 0; i < raVars.m_intWorkSize; i++) {
        if (raBasics[i] != 0 && raBasics[i] != -1){
            OSReport("\tRABasic #%d: %d \n", i, raBasics[i]);
        }
    }

    for (i = 0; i < raVars.m_floatWorkSize; i++) {
        if (raFloats[i] != 0 && raFloats[i] != -1) {
            OSReport("\tRAFloat #%d: %0.2f\n", i, raFloats[i]);
        }
    }

    for(i = 0; i < laVars.m_intWorkSize; i++) {
        if (laBasics[i] != 0 && laBasics[i] != -1) {
            OSReport("\tLABasic #%d: %d \n", i, laBasics[i]);
        }
    }

    for (i = 0; i < laVars.m_floatWorkSize; i++) {
        if (laFloats[i] != 0 && laFloats[i] != -1) {
            OSReport("\tLAFloat #%d: %0.2f\n", i, laFloats[i]);
        }
    }

    return;
}

void PP::LedgeTechWatcher::process(Fighter& fighter)
{
    if (!isEnabled()) { return; }

    if ( _didGrabLedge || _cliffWaitStartFrame != -1 ) {
        soWorkManageModuleImpl& workModule = *static_cast<soWorkManageModuleImpl*>(fighter.m_moduleAccesser->getWorkManageModule());
        soGeneralWorkAbstract& laVars = *workModule.m_generalWorks[LA_VARS];


        int fighterFrame = fighterFrameCounter(laVars);
        _cliffElapsed = (fighterFrame - _cliffWaitStartFrame);
        *_currentFrameCounter += (_cliffElapsed - _cliffElapsedPrev);
        _remainingLedgeIntan = 29 - _cliffElapsed;

        if (shouldStopWatching()) {
            resetState();
            return;
        }

        if (_remainingLedgeIntan >= 0) {
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
        }

        _cliffElapsedPrev = _cliffElapsed;
    }
}

// processTourneyWinner
// processFall
// processOther

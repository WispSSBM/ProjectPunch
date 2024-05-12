#pragma once

#include "pp/common.h"
#include "pp/graphics/drawable.h"
#include "pp/graphics/draw.h"
#include "pp/status_change_watcher.h"
#include "pp/collections/vector.h"
#include "pp/graphics/text_printer.h"

#define LEDGEDASH_START_Y 60.0f
#define LEDGEDASH_BOX_WIDTH 11
#define LEDGEDASH_BOX_HEIGHT 18

#define LEDGEDASH_LEGEND_START_X 60.0f
#define LEDGEDASH_LEGEND_INTERNAL_PADDING 5
#define LEDGEDASH_LEGEND_ITEM_PADDING 65

namespace PP {

// Must be in same order as g_frameTypeLabels
enum FrameType {
    LEDGE_FT_CLIFFWAIT,
    LEDGE_FT_FALLING,
    LEDGE_FT_JUMPING,
    LEDGE_FT_TWSTARTUP,
    LEDGE_FT_AIRDODGE,
    LEDGE_FT_LANDING,
    LEDGE_FT_ATTACK,
    LEDGE_FT_GALINT,
    LEDGE_FT_OTHER,
};

class LedgeTechFrameDrawable: public Graphics::Drawable {
public:
    LedgeTechFrameDrawable(GXColor color, int delay, int lifetime, float top, float bottom, float left, float right) {
        this->color = color;
        this->delay = delay;
        this->lifeTime = lifetime;
        this->top = top;
        this->bottom = bottom;
        this->left = left;
        this->right = right;
        this->is2D = true;
    }

    void draw();

    float top, bottom, left, right;
};

class LedgeTechLegendDrawable: public Graphics::Drawable {
public:
    LedgeTechLegendDrawable(int delay, int lifetime, float top, float left): Graphics::Drawable() {
        this->delay = delay;
        this->lifeTime = lifetime;
        this->top = top;
        this->left = left;
        this->is2D = true;
    }

    void draw();

    float top, left;
};

class LedgeTechWatcher: public StatusChangeWatcher {
    public:
        LedgeTechWatcher(PlayerData* playerData): StatusChangeWatcher(playerData) {
            _currentFrameCounter = &_cliffWaitFrames;
            _cliffWaitStartFrame = -1;
        };

        void didCatchCliff(int fighterFrame);
        void didCatchCliffEnd(int fighterFrame);
        void didLeaveCliff(int fighterFrame);
        void didFinishLedgeDash(int fighterFrame);
        void didStartCliffJump(int fighterFrame);
        void didStartCliffRoll(int fighterFrame);
        void didStartCliffAttack(int fighterFrame);
        void didStartCliffClimb(int fighterFrame);
        void didStartFall(int fighterFrame);

        bool shouldStopWatching();
        void resetState();
        void updateCurrentFrameCounter(int statusKind);

        static Color getFrameColor(FrameType frameType);
        void drawLedgeDash();
        void drawFrame(float startAllPosX, int idx) const;

        virtual void notifyEventChangeStatus(int statusKind, int prevStatusKind, soStatusData* statusData, soModuleAccesser* moduleAccesser);
        virtual void process(Fighter& fighter);
        virtual bool isEnabled() { return playerData->enableLedgeTechWatcher; };
    private:
        bool _isOnLedge;
        bool _didShowLedgeDash;

        int* _currentFrameCounter;
        FrameType _currentFrameType;
        int _remainingLedgeIntan;

        // frames on which things happened.
        int _cliffWaitStartFrame;
        int _tourneyWinnerActionableFrame;

        // counters
        int _totalFrames;
        int _totalFramesPrev;
        int _cliffWaitFrames;
        int _fallingFrames;
        int _jumpingFrames;
        int _twStartupFrames;
        int _airDodgeFrames;
        int _specialLandFrames;
        int _attackingFrames;
        int _otherFrames;

        FrameType _framesList[0x80];
};
} // namespace

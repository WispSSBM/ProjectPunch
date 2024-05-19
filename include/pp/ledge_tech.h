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
#define LEDGEDASH_DEFAULT_DISPLAY_DURATION 300

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
    LedgeTechFrameDrawable(GXColor color, int delay, int lifetime, float top, float bottom, float left, float right): Graphics::Drawable() {
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

class LedgeTechDisplayDrawable: public Graphics::Drawable {
public:
    LedgeTechDisplayDrawable(int totalFrames, int delay, int lifetime, float top, float left, u8 opacity): Graphics::Drawable() {
        this->delay = delay;
        this->lifeTime = lifetime;
        this->top = top;
        this->left = left;
        this->is2D = true;
        this->opacity = opacity;
        this->frames = new Collections::vector(totalFrames);
        this->framesTotalWidth = totalFrames * LEDGEDASH_BOX_WIDTH;
        this->framesStartPosX = PP_CENTER_SCREEN_X - (framesTotalWidth / 2.0f);
    }

    ~LedgeTechDisplayDrawable() {
        for (int i = 0; i < this->frames->size(); i++) {
            delete reinterpret_cast<LedgeTechFrameDrawable*>((*frames)[i]);
        }

        delete this->frames;
    }

    static void setInstance(LedgeTechDisplayDrawable* newInstance);
    static void drawInstance();
    void draw();
    void addFrame(FrameType frameType);

    Collections::vector* frames;
    float top, left, framesStartPosX;
    int framesTotalWidth;
    u8 opacity;

private:
    static LedgeTechDisplayDrawable* instance;
};

class LedgeTechWatcher: public StatusChangeWatcher {
    public:
        LedgeTechWatcher(PlayerData* playerData): StatusChangeWatcher(playerData) {
            _currentFrameCounter = &_cliffWaitFrames;
            _cliffWaitStartFrame = -1;
            _visualDurationFrames = LEDGEDASH_DEFAULT_DISPLAY_DURATION;
            opacity = 0xBB;
        };

        void didCatchCliff(int fighterFrame, soModuleAccesser& modules);
        void didCatchCliffEnd(int fighterFrame, soModuleAccesser& modules);
        void didLeaveCliff(int fighterFrame, soModuleAccesser& modules, int newStatusKind);
        void didFinishLedgeDash(int fighterFrame, soModuleAccesser& modules);
        void didStartCliffJump(int fighterFrame, soModuleAccesser& modules);
        void didStartCliffRoll(int fighterFrame, soModuleAccesser& modules);
        void didStartCliffAttack(int fighterFrame, soModuleAccesser& modules);
        void didStartCliffClimb(int fighterFrame, soModuleAccesser& modules);
        void didStartFall(int fighterFrame, soModuleAccesser& modules);

        bool shouldStopWatching();
        void resetState();
        void updateCurrentFrameCounter(int statusKind);

        static Color getFrameColor(FrameType frameType);
        void drawLedgeDash();
        void drawFrame(float startAllPosX, int idx) const;

        virtual void notifyEventChangeStatus(int statusKind, int prevStatusKind, soStatusData* statusData, soModuleAccesser* moduleAccesser);
        virtual void process(Fighter& fighter);
        virtual bool isEnabled() { 
            return (playerData != NULL
                && (playerData->enableLedgeTechFrameDisplay 
                    || playerData->enableLedgeTechGalintPopup 
                    || playerData->enableLedgeTechFramesOnLedgePopup));
        };

        u8 opacity;
    private:
        bool _isOnLedge;
        bool _didShowLedgeDash;

        int* _currentFrameCounter;
        FrameType _currentFrameType;
        int _visualDurationFrames;

        // frames on which things happened.
        int _cliffCatchStartFrame;
        int _cliffWaitStartFrame;
        int _tourneyWinnerActionableFrame;

        // counters
        int _framesRecorded;
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

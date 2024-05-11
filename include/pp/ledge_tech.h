#include "pp/common.h"
#include "pp/status_change_watcher.h"
#include "pp/collections/vector.h"

namespace PP {

enum FrameType {
    LEDGE_FT_OTHER = 0,
    LEDGE_FT_CLIFFWAIT,
    LEDGE_FT_FALLING,
    LEDGE_FT_JUMPING,
    LEDGE_FT_TWSTARTUP,
    LEDGE_FT_AIRDODGE,
    LEDGE_FT_LANDING,
    LEDGE_FT_GALINT
};

class LedgeTechWatcher: public StatusChangeWatcher {
    public:
        LedgeTechWatcher(PlayerData* playerData): StatusChangeWatcher(playerData) {
            _currentFrameCounter = &_cliffWaitFrames;
            _cliffWaitStartFrame = -1;
        };

        void didCatchCliff(int fighterFrame);
        void didLeaveCliff(int fighterFrame);
        void didFinishLedgeDash(int fighterFrame);
        void didStartCliffJump(int fighterFrame);
        void didStartFall(int fighterFrame);

        bool shouldStopWatching();
        void resetState();
        void updateCurrentFrameCounter(int statusKind);

        Color getFrameColor(FrameType frameType) const;
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
        int _otherFrames;

        FrameType _framesList[0x80];
};
} // namespace

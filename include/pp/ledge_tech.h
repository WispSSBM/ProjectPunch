#include "pp/common.h"
#include "pp/status_change_watcher.h"

namespace PP {

class LedgeTechWatcher: public StatusChangeWatcher {
    public:
        LedgeTechWatcher(PlayerData* playerData): StatusChangeWatcher(playerData) {
            _currentFrameCounter = &_cliffWaitFrames;
            _cliffWaitStartFrame = -1;
        };

        void didCatchCliff(int fighterFrame);
        void didLeaveCliffOther(int fighterFrame);
        void didStartCliffJump(int fighterFrame);
        void didStartFall(int fighterFrame);
        bool shouldStopWatching();
        void resetState();
        void updateCurrentFrameCounter(int statusKind);

        void drawRectForFrame(int idx, Color color) const;
        void drawLedgeDash() const;

        virtual void notifyEventChangeStatus(int statusKind, int prevStatusKind, soStatusData* statusData, soModuleAccesser* moduleAccesser);
        virtual void process(Fighter& fighter);
        virtual bool isEnabled() { return playerData->enableLedgeTechWatcher; };
    private:
        int cliffCatchFrame;
        bool _didGrabLedge;

        int* _currentFrameCounter;
        int _cliffWaitStartFrame;
        int _cliffElapsedPrev;
        int _cliffElapsed;
        int _remainingLedgeIntan;
        int _tourneyWinnerActionableFrame;

        int _cliffWaitFrames;
        int _fallingFrames;
        int _jumpingFrames;
        int _twStartupFrames;
        int _airDodgeFrames;
        int _specialLandFrames;
        int _otherFrames;

};

} // namespace
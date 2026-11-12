#ifndef PP_WAVEDASH_INFO_H
#define PP_WAVEDASH_INFO_H

#include "pp/common.h"
#include "pp/status_change_watcher.h"

#define PP_WAVEDASH_INFO_FRAMES_NONE 0
#define PP_WAVEDASH_INFO_FRAMES_JUMP 1
#define PP_WAVEDASH_INFO_FRAMES_AIRBORNE 2
namespace PP {


class WavedashInfoModule: public StatusChangeWatcher {
    public:
        WavedashInfoModule(PlayerData* playerData): StatusChangeWatcher(playerData) {
            reset();
        }
        virtual void notifyEventChangeStatus(int statusKind, int prevStatusKind, soStatusData* statusData, soModuleAccesser* moduleAccesser);
        virtual void process(Fighter& fighter);
        static bool isEnabled(const PlayerData& playerData);

    private:
        void reset();
        void didEnterJumpsquat(soModuleAccesser* moduleAccesser);
        void didGoAirborne(soModuleAccesser* moduleAccesser);
        void didAirdodge(soModuleAccesser* moduleAccesser);
        void didLand(soModuleAccesser* moduleAccesser);
        bool isJumpInputReleased();

        bool timedOut();

        bool didShorthop;
        u32 frameEnteredJumpsquat;
        u32 frameEnteredAir;
        u32 frameEnteredAirdodge;
        u32 frameEnteredLanding;
        u32 frameAirdodgeInput;
        u32 framePressedShield;
        float airdodgeAngle;
};

};
#endif
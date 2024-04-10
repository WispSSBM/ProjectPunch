#pragma once

union CancelGroups {
    unsigned short value;
    struct {
        /* I think these are the 4 transition groups, I've only seen 0 and 2 used. */
        unsigned short cancelGroup1 : 1;
        unsigned short cancelGroup2 : 1;
        unsigned short cancelGroup3 : 1;
        unsigned short cancelGroup4 : 1;
        unsigned short unknown : 12;
    };
};

struct ftCancelModule {
    int isEnableCancel();
    int unableCancelStatus();
    void enableCancelTransitionGroup(int groupID);

    char _spacer[0x34];
    int unknown0;

    CancelGroups cancelGroups;
};
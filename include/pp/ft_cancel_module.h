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
    virtual int isEnableCancel() = 0;
    virtual int unableCancelStatus() = 0;

    char _spacer[0x34];
    int unknown0;

    CancelGroups cancelGroups;
};

/*
class ftCancelModuleImpl:
    public ftCancelModule,
    public soStatusEventObserver,
    public soAnimCmdEventObserver,
    public soSituationEventObserver
{
public:
    virtual void postInitialize();
    virtual int isEnableCancel();
    virtual void processFixPosition();
    virtual void unableCancelStatus();
    void enableCancelTransitionGroup(int groupID);

    // 0x28, 40
    bool _unk40;
    bool _unk41;
    char _padding42[2];

    // 0x2C, 44
    soModuleAccesser* m_soModuleAccesser;
    // 0x30, 48
    soTransitionModuleEntity* _unk48; // goes to an array somewhere?
    // 0x34, 52
    int _unk52;
    // 0x38, 56
    bool canCancelAttack;
    // 0x39, 57
    bool _unk57; // Must be 0x1 for enableCancelTransitionGroup to do anything.
};
static_assert(sizeof(ftCancelModuleImpl) == 0x3C, "Class is the wrong size!");
*/
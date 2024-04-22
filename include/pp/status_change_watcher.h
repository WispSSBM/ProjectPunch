#pragma once
#include <types.h>
#include <OS/OSError.h>
#include <so/event/so_event_observer.h>
#include <so/status/so_status_event_observer.h>

#include "pp/common.h"
#include "pp/playerdata.h"

namespace ProjectPunch {

#define STATUS_EVENT_CTOR_ID 0x4

class StatusChangeWatcher: public soStatusEventObserver {
public:
    StatusChangeWatcher(PlayerData* pdata): soStatusEventObserver(STATUS_EVENT_CTOR_ID) {
        playerData = pdata;
        m_unitID = 558 + pdata->playerNumber;
        m_eventUnit = NULL;
    }

    virtual ~StatusChangeWatcher() { }

    virtual void addObserver(int param1, int param2) { }
    virtual void notifyEventChangeStatus(int statusKind, int prevStatusKind, soStatusData* statusData, soModuleAccesser* moduleAccesser);
    void registerWith(Fighter* fighter);
    void unregister();

    PlayerData* playerData;
    soEventUnitWrapper<soStatusEventObserver>* m_eventUnit;
};

}
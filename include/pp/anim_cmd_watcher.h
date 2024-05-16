#pragma once
#include <cstdlib>
#include <so/anim_cmd/so_anim_cmd_event_observer.h>
#include <ft/fighter.h>

#include "pp/playerdata.h"

#define ANIM_CMD_BASE_CTOR_ID 5

namespace PP {
class AnimCmdWatcher: public soAnimCmdEventObserver {
public:
    AnimCmdWatcher(PlayerData* pdata, Fighter* fighter): soAnimCmdEventObserver(ANIM_CMD_BASE_CTOR_ID) {
        m_playerData = pdata;
        m_fighter = fighter;
        this->m_unitID = 558 + pdata->playerNumber;
        this->m_eventUnit = NULL;
    };

    virtual ~AnimCmdWatcher() { }
    virtual void addObserver(int, int) { return ; }
    virtual u32 isObserv(char unk);
    virtual bool notifyEventAnimCmd(acAnimCmd* acmd, soModuleAccesser* moduleAccesser, int interpreterId);
    void registerWith(Fighter* fighter);
    void unregister();

    PlayerData* m_playerData;
    Fighter* m_fighter;
    soEventUnitWrapper<soAnimCmdEventObserver>* m_eventUnit;
};
}
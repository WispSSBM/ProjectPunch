#include "pp/status_change_watcher.h"

#include <OS/OSError.h>
#include "pp/actions.h"


namespace PP {

void StatusChangeWatcher::notifyEventChangeStatus(
    int statusKind, 
    int prevStatusKind, 
    soStatusData* statusData, 
    soModuleAccesser* moduleAccesser) 
{
    OSReport("ChangeActionEvent[f%d, P%d, %s]: %s[0x%0X] -> %s[0x%0X]\n",
        frameCounter, playerData->playerNumber, playerData->fighterName, 
        actionName(prevStatusKind), prevStatusKind, actionName(statusKind), statusKind
    );

    memset(&(playerData->current->interruptGroups), 0, sizeof(playerData->current->interruptGroups));
    playerData->setAction(statusKind);

}

void StatusChangeWatcher::registerWith(Fighter* fighter) {
    soEventManageModule* eventManager = fighter->m_moduleAccesser->getEventManageModule();
    if (eventManager != NULL) {
        int manageId = eventManager->getManageId();
        short fighterUnitId = dynamic_cast<soStatusEventObserver*>(fighter)->m_unitID;
        m_eventUnit = dynamic_cast<soEventUnitWrapper<soStatusEventObserver>*>(eventManager->getEventUnit(fighterUnitId));
        if (m_eventUnit != NULL) {
            m_sendID = m_eventUnit->addObserverSub((soStatusEventObserver*)this, this->m_unitID);
            OSReport("Added StatusChange observer @ 0x%0x to unitId: %d\n", this, fighterUnitId);
        }
    }
}

void StatusChangeWatcher::unregister() {
    if (m_eventUnit != NULL) {
        m_eventUnit->eraseObserver(m_unitID, m_sendID);
        m_eventUnit = NULL;
        m_sendID = -1;
    }
}

} // namespace
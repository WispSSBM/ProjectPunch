#include <OS/OSError.h>
#include <so/so_module_accesser.h>
#include <so/stageobject.h>
#include <gf/gf_task.h>
#include <ft/ft_manager.h>

#include "pp/common.h"
#include "pp/anim_cmd_watcher.h"
#include "pp/anim_cmd_utils.h"


namespace PP {
enum InterpreterId {
    INTERPRETER_ID_ACTION_MAIN,
    INTERPRETER_ID_SUBACTION_MAIN
};

bool AnimCmdWatcher::notifyEventAnimCmd(acAnimCmd* acmd, soModuleAccesser* moduleAccessor, int interpreterId) {
    /*
    if (interpreterId != INTERPRETER_ID_ACTION_MAIN && interpreterId != INTERPRETER_ID_SUBACTION_MAIN) {
        return false;
    }
    */

    soAnimCmd* pCmd = dynamic_cast<acAnimCmdImpl*>(acmd)->m_latestWaitCmd;
    if (pCmd != NULL) {
        soAnimCmd& cmd = *pCmd;
        #ifdef PP_DEBUG_ANIMCMD
        snprint_anim_cmd(strManipBuffer, PP_STR_MANIP_SIZE, cmd);
        OSReport("AnimCmd[f%d, P%d, %s, %d]: %s\n", frameCounter, m_playerData->playerNumber, m_playerData->fighterName, interpreterId, strManipBuffer);
        #endif

        if (cmd.m_module == AnimCmdModuleNum::soStatusModule) {
            if (cmd.m_code == 10 /* EnableInterruptGroup */ || cmd.m_code == 11 /* DisableInterruptGroup */) {
                if (cmd.m_argCount > 0) {
                    int cmdArgVal = cmd.m_args[0].m_rawValue;
                    if (cmdArgVal > 0x13) { OSReport("Unknown interrupt group: %d", cmdArgVal); }
                    else {
                        DEBUG_ANIMCMD("Setting interrupt group %d = %d\n", cmdArgVal-1, cmd.m_code);
                        m_playerData->current->interruptGroups.asArray[cmdArgVal-1] = cmd.m_code == 10 ? true : false;

                        // PSA scripts are pretty good about enabling ground ints on land and
                        // air ints in the air.
                        if (cmdArgVal <= 8) {
                            m_playerData->current->isAirborne = false;
                        } else if (cmdArgVal <= 16) {
                            m_playerData->current->isAirborne = true;
                        }
                    }
                }
            }
        } else if (cmd.m_module == AnimCmdModuleNum::ftCancelModule) {
            DEBUG_ANIMCMD("[f%d, P%d, %s, %d] Setting canCancel= %c\n", 
                frameCounter, 
                m_playerData->playerNumber, 
                m_playerData->fighterName, 
                interpreterId, 
                cmd.m_code == 0 ? 'T' : 'F'
            );
            if (cmd.m_code == 0) {

                m_playerData->current->canCancel = true;
            } else if (cmd.m_code == 1) {
                m_playerData->current->canCancel = false;
            }
        }
    }

    return false;
}

void AnimCmdWatcher::registerWith(Fighter* fighter) {
    soEventManageModule* eventManager = fighter->m_moduleAccesser->getEventManageModule();
    int manageId = eventManager->getManageId();
    if (eventManager != NULL) {
        short animCmdUnitId = dynamic_cast<soAnimCmdEventObserver*>(fighter)->m_unitID;
        soEventUnitWrapper<soAnimCmdEventObserver>* m_eventUnit = dynamic_cast<soEventUnitWrapper<soAnimCmdEventObserver>*>(eventManager->getEventUnit(animCmdUnitId));
        if (m_eventUnit != NULL) {
            m_sendID = m_eventUnit->addObserverSub((soAnimCmdEventObserver*)this, m_unitID);
            OSReport("Added AnimCmd observer @ 0x%0x to unitId: %d\n", this, animCmdUnitId);
        }
    }
}

void AnimCmdWatcher::unregister() {
    if (m_eventUnit != NULL) {
        m_eventUnit->eraseObserver(m_unitID, m_sendID);
        m_eventUnit = NULL;
        m_sendID = -1;
    }
}

u32 AnimCmdWatcher::isObserv(char unk) {
    return true;
}
}
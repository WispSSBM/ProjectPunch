#include <OS/OSError.h>
#include <cstdio>
#include <types.h>
#include "pp/anim_cmd_utils.h"

#define _SNPRINTFC(buffer, len, fmt) \
sprReturn = snprintf(buffer, len, fmt); \
if (sprReturn < (len - written)) { buffer += sprReturn; written += sprReturn; }
#define _SNPRINTF(buffer, len, fmt, ...) \
sprReturn = snprintf(buffer, len, fmt, __VA_ARGS__); \
if (sprReturn < (len - written)) { buffer += sprReturn; written += sprReturn; }

namespace PP {

class AnimCmdVariableArg {
public:
    AnimCmdVariableArg(const soAnimCmdArgument& rawArg) {
        if (rawArg.m_varType != ANIM_CMD_ARG_VARIABLE) {
            m_invalid = true;
            OSReport("WARNING: Got animcmd arg with wrong var type: type: 0x%8X, arg: 0x%8X\n", rawArg.m_varType, rawArg.m_rawValue);
            return;
        }

        m_invalid = false;
        m_mem_type = (rawArg.m_rawValue & 0xF0000000) >> 28;
        m_data_type = (rawArg.m_rawValue & 0x0F000000) >> 24;
        m_value = rawArg.m_rawValue & 0x00FFFFFF;
    }

    int snprint_self(char* buffer, size_t maxLen) {
        const char* _memType;
        const char* _varType;
        int written = 0;
        int sprReturn;

        switch (this->m_mem_type) {
        case ANIM_CMD_VAR_TYPE_IC:
            _memType = "IC";
            break;
        case ANIM_CMD_VAR_TYPE_LA:
            _memType = "LA";
            break;
        case ANIM_CMD_VAR_TYPE_RA:
            _memType = "RA";
            break;
        default:
            _memType = "UNK";
        }

        switch (this->m_data_type) {
        case ANIM_CMD_INT:
            _varType = "INT";
            break;
        case ANIM_CMD_FLOAT:
            _varType = "FLOAT";
            break;
        case ANIM_CMD_BOOL:
            _varType = "BOOL";
            break;
        default:
            _varType = NULL;
        }

        if (_varType == NULL) {
            _SNPRINTF(buffer, maxLen - written, "%s-UNK<%d>[0x%x]", _memType, this->m_data_type, m_value);

        } else {
            _SNPRINTF(buffer, maxLen - written, "%s-%s[0x%x]", _memType, _varType, m_value);
        }


        if (m_invalid) {
            _SNPRINTFC(buffer, maxLen - written, "(invalid)");
        }

        if (written == maxLen) {
            buffer[-1] = '\0';
        }

        return written;
    }

    int m_data_type;
    int m_mem_type;
    u32 m_value;
    bool m_invalid;
};

int snprint_anim_cmd_name(char* buffer, size_t maxLen, const soAnimCmd& animCmd) {
    int sprReturn;
    int written = 0;
    const char* modName;
    const char* fnName = NULL;

    switch (animCmd.m_module) {
    case AnimCmdModuleNum::CONTROL_FLOW: modName = "controlFlow"; break;
    case AnimCmdModuleNum::soStatusModule: modName = "soStatusModule"; break;
    case AnimCmdModuleNum::soModelModule: modName = "soModelModule"; break;
    case AnimCmdModuleNum::soMotionModule: modName = "soMotionModule"; break;
    case AnimCmdModuleNum::soPostureModule: modName = "soPostureModule"; break;
    case AnimCmdModuleNum::soCollisionModule: modName = "soCollisionModule"; break;
    case AnimCmdModuleNum::ftControllerModule: modName = "ftControllerModule"; break;
    case AnimCmdModuleNum::soGroundModule: modName = "soGroundModule"; break;
    case AnimCmdModuleNum::soSituationmodule: modName = "soSituationmodule"; break;
    case AnimCmdModuleNum::soSoundModule: modName = "soSoundModule"; break;
    case AnimCmdModuleNum::soVisibilityModule: modName = "soVisibilityModule"; break;
    case AnimCmdModuleNum::ftFighter: modName = "ftFighter"; break;
    case AnimCmdModuleNum::soAnimCmdModule: modName = "soAnimCmdModule"; break;
    case AnimCmdModuleNum::soKineticModule: modName = "soKineticModule"; break;
    case AnimCmdModuleNum::soLinkModule: modName = "soLinkModule"; break;
    case AnimCmdModuleNum::soGenerateArticleManageModule: modName = "soGenerateArticleManageModule"; break;
    case AnimCmdModuleNum::soEffectModule: modName = "soEffectModule"; break;
    case AnimCmdModuleNum::soWorkManageModule: modName = "soWorkManageModule"; break;
    case AnimCmdModuleNum::soComboModule: modName = "soComboModule"; break;
    case AnimCmdModuleNum::ftAreaModule: modName = "ftAreaModule"; break;
    case AnimCmdModuleNum::soTerritoryModule: modName = "soTerritoryModule"; break;
    case AnimCmdModuleNum::soTargetSearchModule: modName = "soTargetSearchModule"; break;
    case AnimCmdModuleNum::soPhysicsModule: modName = "soPhysicsModule"; break;
    case AnimCmdModuleNum::soSlopeModule: modName = "soSlopeModule"; break;
    case AnimCmdModuleNum::soShadowModule: modName = "soShadowModule"; break;
    case AnimCmdModuleNum::soCameraModule: modName = "soCameraModule"; break;
    case AnimCmdModuleNum::BRAWL: modName = "BRAWL"; break;
    case AnimCmdModuleNum::ftStopMOdule: modName = "ftStopMOdule"; break;
    case AnimCmdModuleNum::soShakeModule: modName = "soShakeModule"; break;
    case AnimCmdModuleNum::soDamageModule: modName = "soDamageModule"; break;
    case AnimCmdModuleNum::soItemManageModule: modName = "soItemManageModule"; break;
    case AnimCmdModuleNum::soTurnModule: modName = "soTurnModule"; break;
    case AnimCmdModuleNum::soColorBlendModule: modName = "soColorBlendModule"; break;
    case AnimCmdModuleNum::soTeamModule: modName = "soTeamModule"; break;
    case AnimCmdModuleNum::soSlowModule: modName = "soSlowModule"; break;
    case AnimCmdModuleNum::ftCancelModule: modName = "ftCancelModule"; break;
    default: modName = NULL;
    };

    if (animCmd.m_module == AnimCmdModuleNum::CONTROL_FLOW) {
        switch (animCmd.m_code) {
        case 0: fnName = "Wait"; break;
        case 2: fnName = "WaitFromStart"; break;
        case 4: fnName = "Loop"; break;
        case 5: fnName = "LoopBegin"; break;
        case 6: fnName = "?LoopBreak"; break;
        case 7: fnName = "Call"; break;
        case 8: fnName = "Return"; break;
        case 9: fnName = "GOTO"; break;
        case 10: fnName = "If"; break;
        case 11: fnName = "And"; break;
        case 12: fnName = "Or"; break;
        case 13: fnName = "ElseIf"; break;
        case 14: fnName = "Else"; break;
        case 15: fnName = "EndIf"; break;
        case 16: fnName = "Switch"; break;
        case 17: fnName = "Case"; break;
        case 18: fnName = "CaseDefault"; break;
        case 19: fnName = "SwitchEnd"; break;
        case 0x18: fnName = "CaseUnknown18"; break;
        }
    } else if (animCmd.m_module == AnimCmdModuleNum::soWorkManageModule) {
        switch (animCmd.m_code) {
        case 0x01: fnName = "IntSet"; break;
        case 0x02: fnName = "IntAdd"; break;
        case 0x03: fnName = "IntSub"; break;
        case 0x04: fnName = "IntDecr"; break;
        case 0x05: fnName = "IntRndm"; break;
        case 0x06: fnName = "FloatSet"; break;
        case 0x07: fnName = "FloatAdd"; break;
        case 0x08: fnName = "FloatSub"; break;
        case 0x0A: fnName = "BitSet"; break;
        case 0xB: fnName = "BitClear"; break;
        case 0xD: fnName = "IntMult"; break;
        case 0xE: fnName = "IntDiv"; break;
        case 0xF: fnName = "FloatMult"; break;
        case 0x10: fnName = "FloatDiv"; break;
        case 0x11: fnName = "IntSetAbsVal"; break;
        case 0x12: fnName = "FloatSetAbsVal"; break;
        }
    } else if (animCmd.m_module == AnimCmdModuleNum::soStatusModule) {
        switch (animCmd.m_code){
        case 0: fnName = "CreateInterrupt"; break;
        case 1: fnName = "CreateNamedInterrupt"; break;
        case 2: fnName = "UnknownPrimid"; break;
            //case 3 UNUSED?
        case 4: fnName = "AddInterruptReq"; break;
        case 6: fnName = "EnableNamedInterrupt"; break;
        case 8: fnName = "DisableNamedInterrupt"; break;
        case 9: fnName = "ToggleNamedInterrupt"; break;
        case 10: fnName = "EnableInterruptGroup"; break;
        case 11: fnName = "DisableInterruptGroup"; break;
        case 12: fnName = "ToggleInterruptGroup"; break;
        case 14: fnName = "UnknownMaybeDisableAll"; break;
        }
    } else if (animCmd.m_module == AnimCmdModuleNum::soCollisionModule) {
        switch (animCmd.m_code) {
        case 0: fnName = "CreateHitbox"; break;
        case 1: fnName = "ChangeHitboxDamage"; break;
        case 2: fnName = "ChangeHitboxSize"; break;
        case 3: fnName = "DeleteHitbox";
        case 4: fnName = "ClearHitboxes"; break;
        case 5: fnName = "AlterOwnHurtbox"; break;
        case 6: fnName = "ResetOwnHurtbox"; break;
        case 7: fnName = "SimpleEnemyHitbox?"; break;
        case 8: fnName = "AlterBoneHurtbox"; break;
        case 0xA: fnName = "CreateGrabHitbox"; break;
        case 0xC: fnName = "DeleteGrabHitbox"; break;
        case 0xD: fnName = "ClearGrabHitboxes"; break;
        case 0xE: fnName = "SetThrowProps"; break;
        case 0xF: fnName = "ApplyThrow"; break;
        case 0x10: fnName = "CreateDetectBox"; break;
        case 0x14: fnName = "SetChargeDmg?"; break;
        case 0x15: fnName = "CreateSpecialHitbox"; break;
        case 0x17: fnName = "CreateDefenseHurtbox"; break;
        case 0x18: fnName = "ClearDefenseHurtboxes"; break;
        case 0x19: fnName = "CreateEnemyHitbox"; break;
        case 0x1A: fnName = "EnemyPummel"; break; // apparently used by master hand
        case 0x1B: fnName = "MoveHitbox?"; break;
        case 0x2B: fnName = "CreateGettingThrownHitbox"; break;
        case 0x2C: fnName = "CreateGettingHitHitbox"; break;
        }

    } else if (animCmd.m_module == AnimCmdModuleNum::ftCancelModule) {
        switch(animCmd.m_code) {
            case 0: fnName = "EnableCancel"; break;
            case 1: fnName = "DisableCancel"; break;
            case 2: fnName = "ToggleCancel?"; break;
        }
    } else if (animCmd.m_module == AnimCmdModuleNum::soMotionModule) {
        switch (animCmd.m_code) {
            case 0: fnName = "ChangeSubAct"; break;
            case 6: fnName = "SetAnimFrame"; break;
            case 7: fnName = "SetFrameSpeed"; break;
            case 0x14: fnName = "SetAnimationAndTimerFrame"; break;
        }
    } else if (animCmd.m_module == AnimCmdModuleNum::soAnimCmdModule) {
        switch (animCmd.m_code) {
            case 0: fnName = "RunEachFrame"; break;
            case 1: fnName = "StopRunEachFrame"; break;
        }
    }


    if (modName == NULL) {
        _SNPRINTF(buffer, maxLen - written, "%d", animCmd.m_module);
    } else {
        _SNPRINTF(buffer, maxLen - written, "%s", modName);
    }

    if (animCmd.m_option != 0) {
        _SNPRINTFC(buffer, maxLen - written, "<%d>");
    }

    if (fnName == NULL) {
        _SNPRINTF(buffer, maxLen - written, "::%d", animCmd.m_code);
    } else {
        _SNPRINTF(buffer, maxLen - written, "::%s", fnName);
    }

    return written;
}

int snprint_anim_cmd(char* buffer, size_t maxLen, const soAnimCmd& animCmd) {
    int sprReturn;
    int written = 0;

    sprReturn = snprint_anim_cmd_name(buffer, maxLen - written, animCmd);
    if (sprReturn < (maxLen - written)) { buffer += sprReturn; written += sprReturn; }
    _SNPRINTFC(buffer, maxLen - written, "(");

    for (int i = 0; i < animCmd.m_argCount; i++) {
        if (i != 0) {
            sprReturn = snprintf(buffer, maxLen - written, ", ");
            if (sprReturn < (maxLen - written)) {buffer += sprReturn; written += sprReturn;}
        }

        soAnimCmdArgument& arg = animCmd.m_args[i];
        switch (arg.m_varType) {
        case(ANIM_CMD_ARG_INT):
            _SNPRINTF(buffer, maxLen - written, "%d", arg.m_rawValue);
            break;
        case(ANIM_CMD_ARG_SCALAR):
            _SNPRINTF(buffer, maxLen - written, "%0.2f", ((float)arg.m_rawValue) / 60000.f);
            break;
        case(ANIM_CMD_ARG_PTR):
            _SNPRINTF(buffer, maxLen - written, "PTR(0x%0X)", arg.m_rawValue);
            break;
        case(ANIM_CMD_ARG_BOOL):
            _SNPRINTF(buffer, maxLen - written, "%s", arg.m_rawValue == 0 ? "false" : "true");
            break;
        case(ANIM_CMD_ARG_VARIABLE):
            sprReturn = AnimCmdVariableArg(arg).snprint_self(buffer, maxLen - written);
            if (sprReturn < (maxLen - written)) {buffer += sprReturn; written += sprReturn;}
            break;
        case(ANIM_CMD_ARG_REQUIREMENT):
            _SNPRINTF(buffer, maxLen - written, "REQ(0x%0X)", arg.m_rawValue);
            break;
        default:
            _SNPRINTF(buffer, maxLen - written, "UNKNOWN<%d>(0x%0X)", arg.m_varType, arg.m_rawValue);
            break;
        }
    }

    _SNPRINTFC(buffer, maxLen - written, ")");

    if (written == maxLen) {
        buffer[-1] = '\0';
    }

    return written;
}

bool isLastAnimCmd(const soAnimCmd& cmd) {
    return !(cmd.m_module == 0 && cmd.m_code == 0) && !(cmd.m_module == 0xFF && cmd.m_code == 0xFF);
}

/* 
* Don't want to just throw this away, this is an example of parsing the currently loaded animcmd unit for a given fighter
*
*   soAnimCmd* getCommand(const soAnimCmdAddressPackArraySeparate& animPack, int idx) {
*       if (animPack.m_startCmd == NULL || animPack.m_startCmd == reinterpret_cast<soAnimCmd*>(0xCCCCCCCC)) {
*           return NULL;
*       }
*       // OSReport("COMMAND START ADDR: %08x\n", &arrayAddressPack->start);
*       return &animPack.m_startCmd[idx];
*   }
*
*   
*   void readAnimCmds() {
*       soAnimCmdModuleImpl* acModule = dynamic_cast<soAnimCmdModuleImpl*>(modules.getAnimCmdModule());
*       soAnimCmdAddressPackArraySeparate& animCmds = *acModule->m_animCmdThreads->m_subactionMain.m_element.m_animCmdAddressPackArraySeparate;
*   
*       int currentIdx = 0;
*       soAnimCmd* cmd = getCommand(animCmds, currentIdx++);
*       int counter = 0;
*       if (frameCounter % 15 == 0) {
*           if (cmd != NULL) {
*               while (!(cmd->m_module == 0 && cmd->m_code == 0) && !(cmd->m_module == 0xFF && cmd->m_code == 0xFF)) {
*                   counter++;
*                   snprint_anim_cmd(strManipBuffer, PP_STR_MANIP_SIZE, *cmd);
*                   OSReport("%s\n", strManipBuffer);
*                   //OSReport("ANIMCMD: MOD: %d, CODE: %d, OPTION:%d\n", cmd->m_module, cmd->m_code, cmd->m_option);
*                   for (int i = 0; i < cmd->m_argCount; i++) {
*                       soAnimCmdArgument& cmdArg = (*cmd->m_args)[i];
*                       //OSReport("  Arg %d TYPE: %d VALUE: %d\n", i, cmdArg.m_type, cmdArg.m_value);
*                   }
*   
*                   cmd = getCommand(animCmds, currentIdx++);
*               }
*           }
*           OSReport("Processed %d animCmds this frame for player %d\n", counter, playerNumber);
*       }
*   }
*/

} // namespace
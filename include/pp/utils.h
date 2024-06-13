#pragma once

#include "pp/common.h"

#include <so/work/so_general_work_simple.h>
#include <ft/fighter.h>

namespace PP {

soGeneralWorkSimple* getWorkVars(const Fighter& fighter, WorkModuleVarType varType);
bool getRABit(const Fighter& fighter, u32 idx);
void debugWorkModule(const Fighter& fighter);

} // namespace PP
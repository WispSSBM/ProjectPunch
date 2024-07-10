#pragma once

#include "pp/common.h"

#define PP_INVIS_TECHCHASE_MAIN_HOOK_ADDR 
#define PP_INVIS_TECHCHASE_CURSOR_HOOK_ADDR

namespace PP {

void invisTechsMainHook();
bool invisTechsMain(const soModuleAccesser* moduleAccesser);
void invisTechsCursorHook();
bool invisTechsCursorMain(Fighter& fighter);

} // namespace PP
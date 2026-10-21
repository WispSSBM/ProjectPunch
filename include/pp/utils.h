#pragma once

#include "pp/common.h"

#include <so/work/so_general_work_simple.h>
#include <ft/fighter.h>
#include <gf/gf_file_io_handle.h>
#include <memory.h>

namespace PP {

soGeneralWorkSimple* getWorkVars(const Fighter& fighter, WorkModuleVarType varType);
bool getRABit(const Fighter& fighter, u32 idx);
void debugWorkModule(const Fighter& fighter);
gfFileIOHandle* readFileBlocking(const char* file_name, Heaps::HeapType heapType, size_t len);

} // namespace PP
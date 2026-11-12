#include "pp/utils.h"

#include "pp/ui.h"
#include "pp/graphics/text_printer.h"


namespace PP {

soGeneralWorkSimple* getWorkVars(const Fighter& fighter, WorkModuleVarType varType) {
    return getWorkVars(fighter.m_moduleAccesser, varType);
}

soGeneralWorkSimple* getWorkVars(soModuleAccesser* moduleAccesser, WorkModuleVarType varType) {
    // assume the work manage module is of the normal impl type. Not technically typesafe, could use the proper brawl
    // api surface later.
    const soWorkManageModuleImpl& workModule = *reinterpret_cast<const soWorkManageModuleImpl*>(moduleAccesser->getWorkManageModule());
    return reinterpret_cast<soGeneralWorkSimple*>(workModule.m_generalWorks[varType]);
}

bool getRABit(soModuleAccesser* moduleAccessor, u32 idx) {
    /*
     *   Note that this is different than the RA *flags*. Those are boolean variables,
     *   but a lot of things are stored as bitflags on the lower indexed int variables
     *   in the RA int variables.
     */

    soGeneralWorkSimple& raVars = *getWorkVars(moduleAccessor, RA_VARS);
    // array
    u32* raBools =  reinterpret_cast<u32*>(raVars.m_intWorks);
    int intFieldCount = raVars.m_intWorkSize;

    // printRABools(workModule);

    if (!(idx < intFieldCount*32)) {
        OSReport("Warning: asked for invalid RA bit %d from workModule %x.\n", idx, (void*)moduleAccessor->getWorkManageModule());
        return false;
    }

    u32 bitsChunk = raBools[idx / 32];
    char remainder = idx % 32;
    if ((bitsChunk & (1 << remainder) >> remainder) == 0) {
        return false;
    } else {
        return true;
    }
}


bool getRABit(const Fighter& fighter, u32 idx) {
    return getRABit(fighter.m_moduleAccesser, idx);
}

void debugWorkModule(const Fighter& fighter) {
    return debugWorkModule(fighter.m_moduleAccesser);
}

void debugWorkModule(soModuleAccesser* moduleAccesser) {
    u32 i;

    soGeneralWorkSimple& raVars = *getWorkVars(moduleAccesser, RA_VARS);
    soGeneralWorkSimple& laVars = *getWorkVars(moduleAccesser, LA_VARS);
    int* raBasics = raVars.m_intWorks;
    float* raFloats = raVars.m_floatWorks;
    int* laBasics = laVars.m_intWorks;
    float* laFloats = laVars.m_floatWorks;

    for(i = 0; i < raVars.m_intWorkSize; i++) {
        if (raBasics[i] != 0 && raBasics[i] != -1){
            OSReport("\tRABasic #%d: %d \n", i, raBasics[i]);
        }
    }

    for (i = 0; i < raVars.m_floatWorkSize; i++) {
        if (raFloats[i] != 0 && raFloats[i] != -1) {
            OSReport("\tRAFloat #%d: %0.2f\n", i, raFloats[i]);
        }
    }

    for(i = 0; i < laVars.m_intWorkSize; i++) {
        if (laBasics[i] != 0 && laBasics[i] != -1) {
            OSReport("\tLABasic #%d: %d \n", i, laBasics[i]);
        }
    }

    for (i = 0; i < laVars.m_floatWorkSize; i++) {
        if (laFloats[i] != 0 && laFloats[i] != -1) {
            OSReport("\tLAFloat #%d: %0.2f\n", i, laFloats[i]);
        }
    }

    return;
}

void printMessage(char const* msg, float xPos, float yPos, Color color = PP_COLOR_WHITE){
    OSReport("%s\n", msg);
    printer.setTextColor(color);
    printer.renderPre = true;
    printer.boxBgColor = PP_COLOR_TRANSPARENT_GREY;
    printer.boxPadding = 10;

    printer.lineHeight = punchMenu.lineHeight();
    printer.setScale(punchMenu.baseFontScale, punchMenu.fontScaleMultiplier, punchMenu.lineHeightMultiplier);
    printer.setPosition(xPos, yPos);

    printer.begin();
    printer.print(msg);
    printer.renderBoundingBox();
}

/* Reads a file to a specified heap, returns the file handle after waiting for the read to finish.*/
gfFileIOHandle* readFileBlocking(const char* file_name, Heaps::HeapType heapType, size_t len) {
    gfFileIOHandle* handle = new gfFileIOHandle();

    if (!handle->checkFileRequest(file_name)) {
        return false;
    }
    while (!handle->isReady()) {
        OSSleepTicks(0, defaultSleepTicks());
    }
    if (handle->getReturnStatus() != 0) {
        return false;
    }

    handle->read(file_name, heapType, len);
    while (!handle->isReady()) {
        OSSleepTicks(0, defaultSleepTicks());
    }
    if (handle->getReturnStatus() != 0) {
        handle->release();
        return NULL;
    }

    return handle;
}



} // namespace PP
#include "pp/utils.h"

#include "pp/ui.h"
#include "pp/graphics/text_printer.h"


namespace PP {

soGeneralWorkSimple* getWorkVars(const Fighter& fighter, WorkModuleVarType varType) {
    // assume the work manage module is of the normal impl type. Not technically typesafe, could use the proper brawl
    // api surface later.
    const soWorkManageModuleImpl& workModule = *reinterpret_cast<const soWorkManageModuleImpl*>(fighter.m_moduleAccesser->getWorkManageModule());
    return reinterpret_cast<soGeneralWorkSimple*>(workModule.m_generalWorks[varType]);
}

bool getRABit(const Fighter& fighter, u32 idx) {
    soGeneralWorkSimple& raVars = *getWorkVars(fighter, RA_VARS);
    // array
    u32* raBools =  raVars.m_flagWorks;
    int bitFieldsSize = raVars.m_flagWorkSize;

    // printRABools(workModule);

    if (!(idx < bitFieldsSize*8*4)) {
        OSReport("Warning: asked for invalid RA bit %d from workModule %x.\n", idx, (void*)fighter.m_moduleAccesser->getWorkManageModule());
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

void debugWorkModule(const Fighter& fighter) {
    u32 i;

    soGeneralWorkSimple& raVars = *getWorkVars(fighter, RA_VARS);
    soGeneralWorkSimple& laVars = *getWorkVars(fighter, LA_VARS);
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

} // namespace PP
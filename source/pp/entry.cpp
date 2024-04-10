#include <sy_core.h>

#include "pp/entry.h"
#include "pp/feature_tester.h"

namespace ProjectPunch {
namespace Entry {

void Init() {
    // 0x800177B0
    SyringeCore::syInlineHook(0x800177B0, (void*)featureTester);
}

void Destroy() {

}

}
}
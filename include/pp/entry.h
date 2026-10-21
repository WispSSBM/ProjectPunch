#pragma once

#define PP_VERSION "1.3.0"

#include <sy_core.h>

namespace PP {
    namespace Entry {
        void Init(CoreApi* plugin);
        void Destroy();
    }
}
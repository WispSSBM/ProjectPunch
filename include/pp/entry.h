#pragma once

#define PP_VERSION "1.2.3"

#include <sy_core.h>

namespace PP {
    namespace Entry {
        void Init(CoreApi* plugin);
        void Destroy();
    }
}
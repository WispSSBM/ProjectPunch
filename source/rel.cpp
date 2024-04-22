#include <gf/gf_file_io_request.h>
#include <gf/gf_memory_pool.h>
#include <sy_core.h>

#include "pp/entry.h"

namespace Syringe {
    const PluginMeta META = {
        "ProjectPunch",
        "Wisp",
        Version("0.0.1"),
        Version(SYRINGE_VERSION)
    };

    extern "C" {
    typedef void (*PFN_voidfunc)();
    __attribute__((section(".ctors"))) extern PFN_voidfunc _ctors[];
    __attribute__((section(".ctors"))) extern PFN_voidfunc _dtors[];

    const PluginMeta* _prolog();
    void _epilog();
    void _unresolved();
    }

    const PluginMeta* _prolog()
    {
        // Run global constructors
        PFN_voidfunc* ctor;
        for (ctor = _ctors; *ctor; ctor++)
        {
            (*ctor)();
        }

        PP::Entry::Init();
        return &META;
    }

    void _epilog()
    {
        // run the global destructors
        PFN_voidfunc* dtor;
        for (dtor = _dtors; *dtor; dtor++)
        {
            (*dtor)();
        }
		
		PP::Entry::Destroy();
    }

    void _unresolved(void)
    {
    }

}
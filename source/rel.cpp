#include <gf/gf_file_io_request.h>
#include <gf/gf_memory_pool.h>
#include <sy_core.h>
#include <plugin.h>

#include "pp/entry.h"

const Syringe::PluginMeta META = {
    "ProjectPunch",
    "Wisp",
    Syringe::Version(PP_VERSION),
    Syringe::Version(SYRINGE_VERSION),
};

extern "C" {
typedef void (*PFN_voidfunc)();
__attribute__((section(".ctors"))) extern PFN_voidfunc _ctors[];
__attribute__((section(".ctors"))) extern PFN_voidfunc _dtors[];

const Syringe::PluginMeta* _prolog(CoreApi* api);
void _epilog();
void _unresolved();
}

const Syringe::PluginMeta* _prolog(CoreApi* api)
{
    // Run global constructors
    PFN_voidfunc* ctor;
    for (ctor = _ctors; *ctor; ctor++)
    {
        (*ctor)();
    }

    PP::Entry::Init(api);
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

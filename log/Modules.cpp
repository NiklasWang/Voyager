#include "Modules.h"
#include "string.h"

namespace pandora {


static const char *const gModuleName[] = {
    [MODULE_OTHERS]                  = "others",
    [MODULE_PANDORA]                 = "pandora_intf",
    [MODULE_PANDORA_IMPL]            = "pandora_impl",
    [MODULE_CORE]                    = "pandora_core",
    [MODULE_PAL]                     = "pal",
    [MODULE_PAL_IMPL]                = "pal_impl",
    [MODULE_PLATFORM_OPS]            = "platform_ops",
    [MODULE_ALGORITHM_BASE_TEMPLATE] = "algorithm_base_template",
    [MODULE_ALGORITHM_TEMPLATE]      = "algorithm_template",
    [MODULE_ALGORITHMS]              = "algorithm",
    [MODULE_FRAME_HELPER]            = "algorithm_helper",
    [MODULE_PIPELINE]                = "pipeline",
    [MODULE_CONVERTER]               = "converter",
    [MODULE_THREAD_POOL]             = "thread_pool",
    [MODULE_MEMORY_POOL]             = "memory_pool",
    [MODULE_STATUS]                  = "status",
    [MODULE_STATUS_MANAGER]          = "status_manager",
    [MODULE_XML]                     = "xml",
    [MODULE_CAMERA]                  = "camera",
    [MODULE_PERFORMANCE_CHECKER]     = "performance_checker",
    [MODULE_SYNC_CONTROLLER]         = "sync_controller",
    [MODULE_BUFFER_MANAGER]          = "buffer_manager",
    [MODULE_FD_TRACKER]              = "fd_tracker",
    [MODULE_OBJECT_BUFFER]           = "object_buffer",
    [MODULE_UTILS]                   = "utils",
    [MODULE_COMMON]                  = "common",
    [MODULE_RENDER]                  = "render",
    [MODULE_TESTER]                  = "tester",
    [MODULE_SIRIUS]                  = "sirius",
    [MODULE_EXTERNAL]                = "external",
    [MODULE_MAX_INVALID]             = "max_invalid",
};

static const char *const gModuleShortName[] = {
    [MODULE_OTHERS]                  = "[OTHER]",
    [MODULE_PANDORA]                 = "[ INTF]",
    [MODULE_PANDORA_IMPL]            = "[ IMPL]",
    [MODULE_CORE]                    = "[ CORE]",
    [MODULE_PAL]                     = "[  PAL]",
    [MODULE_PAL_IMPL]                = "[PIMPL]",
    [MODULE_PLATFORM_OPS]            = "[ PLTO]",
    [MODULE_ALGORITHM_BASE_TEMPLATE] = "[ALGBT]",
    [MODULE_ALGORITHM_TEMPLATE]      = "[ ALGT]",
    [MODULE_ALGORITHMS]              = "[ ALGS]",
    [MODULE_FRAME_HELPER]            = "[ ALGH]",
    [MODULE_PIPELINE]                = "[ PIPE]",
    [MODULE_CONVERTER]               = "[CONVE]",
    [MODULE_THREAD_POOL]             = "[ THRP]",
    [MODULE_MEMORY_POOL]             = "[ MEMP]",
    [MODULE_STATUS]                  = "[ STAT]",
    [MODULE_STATUS_MANAGER]          = "[STMGR]",
    [MODULE_XML]                     = "[  XML]",
    [MODULE_CAMERA]                  = "[  CAM]",
    [MODULE_PERFORMANCE_CHECKER]     = "[ PERF]",
    [MODULE_SYNC_CONTROLLER]         = "[ SYNC]",
    [MODULE_BUFFER_MANAGER]          = "[  BUF]",
    [MODULE_FD_TRACKER]              = "[FDTRA]",
    [MODULE_OBJECT_BUFFER]           = "[OBJBF]",
    [MODULE_UTILS]                   = "[UTILS]",
    [MODULE_COMMON]                  = "[ COMN]",
    [MODULE_RENDER]                  = "[RENDR]",
    [MODULE_TESTER]                  = "[ TEST]",
    [MODULE_SIRIUS]                  = "[SIRIU]",
    [MODULE_EXTERNAL]                = "[  EXT]",
    [MODULE_MAX_INVALID]             = "[INVAL]",
};

static bool checkValid(ModuleType type)
{
    bool rc = false;

    if (type >= 0 && type < MODULE_MAX_INVALID) {
        rc = true;
    }

    return rc;
}

ModuleType getValidType(ModuleType type)
{
    return checkValid(type) ? type : MODULE_MAX_INVALID;
}

uint32_t getMaxLenofShortName()
{
    uint32_t result = 0, len = 0;
    for (uint32_t i = 0; i <= MODULE_MAX_INVALID; i++) {
        len = strlen(gModuleShortName[i]);
        result = result < len ? len : result;
    }

    return result;
}

const char *getModuleName(ModuleType type)
{
    return gModuleName[getValidType(type)];
}

const char *getModuleShortName(ModuleType type)
{
    return gModuleShortName[getValidType(type)];
}

};


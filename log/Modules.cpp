#include "Modules.h"
#include "string.h"

namespace voyager {

static const char *const gModuleName[] = {
    [MODULE_OTHERS]          = "others",
    [MODULE_SOCKET]          = "socket",
    [MODULE_CLIENT]          = "client",
    [MODULE_SERVER]          = "server",
    [MODULE_SERVER_IMPL]     = "server impl",
    [MODULE_CLIENT_CORE]     = "client core",
    [MODULE_SERVER_CORE]     = "server core",
    [MODULE_OVERALL_CONTROL] = "overall controller",
    [MODULE_BUFFER_MANAGER]  = "buffer manager",
    [MODULE_ION_HELPER]      = "android ion helper",
    [MODULE_FS_HELPER]       = "fs helper",
    [MODULE_SERVER_HANDLER]  = "request handler",
    [MODULE_CLIENT_HANDLER]  = "request handler",
    [MODULE_DATA_SERVER]     = "data server",
    [MODULE_DATA_CLIENT]     = "data client",
    [MODULE_FD_SERVER]       = "fd server",
    [MODULE_FD_CLIENT]       = "fd client",
    [MODULE_FRAME_SERVER]    = "frame server",
    [MODULE_FRAME_CLIENT]    = "frame client",
    [MODULE_EVENT_SERVER]    = "event server",
    [MODULE_EVENT_CLIENT]    = "event client",

    [MODULE_COMMON]          = "common",
    [MODULE_UTILS]           = "utils",
    [MODULE_THREAD_POOL]     = "thread pool",
    [MODULE_MEMORY_POOL]     = "memory pool",
    [MODULE_MAX_INVALID]     = "max_invalid",
};

static const char *const gModuleShortName[] = {
    [MODULE_OTHERS]          = "[OTHER]",
    [MODULE_SOCKET]          = "[  SOK]",
    [MODULE_CLIENT]          = "[CLIEN]",
    [MODULE_SERVER]          = "[SERVE]",
    [MODULE_SERVER_IMPL]     = "[SIMPL]",
    [MODULE_CLIENT_CORE]     = "[CCORE]",
    [MODULE_SERVER_CORE]     = "[SCORE]",
    [MODULE_OVERALL_CONTROL] = "[  OAC]",
    [MODULE_BUFFER_MANAGER]  = "[ BUFM]",
    [MODULE_ION_HELPER]      = "[IONHP]",
    [MODULE_FS_HELPER]       = "[ FSHP]",
    [MODULE_SERVER_HANDLER]  = "[SERVH]",
    [MODULE_CLIENT_HANDLER]  = "[ CLIH]",
    [MODULE_DATA_SERVER]     = "[ DATS]",
    [MODULE_DATA_CLIENT]     = "[ DATC]",
    [MODULE_FD_SERVER]       = "[  FDS]",
    [MODULE_FD_CLIENT]       = "[  FDC]",
    [MODULE_FRAME_SERVER]    = "[FRAMS]",
    [MODULE_FRAME_CLIENT]    = "[FRAMC]",
    [MODULE_EVENT_SERVER]    = "[ EVTS]",
    [MODULE_EVENT_CLIENT]    = "[ EVTC]",

    [MODULE_COMMON]          = "[ COMN]",
    [MODULE_UTILS]           = "[ UTLS]",
    [MODULE_THREAD_POOL]     = "[THREA]",
    [MODULE_MEMORY_POOL]     = "[  MEM]",
    [MODULE_MAX_INVALID]     = "[INVAL]",
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


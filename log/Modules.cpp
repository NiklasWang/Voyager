#include "Modules.h"
#include "string.h"

namespace pandora {


static const char *const gModuleName[] = {
    [MODULE_OTHERS]          = "others",
    [MODULE_CLIENT]          = "client",
    [MODULE_SERVER]          = "server",
    [MODULE_CLIENT_IMPL]     = "client impl",
    [MODULE_SERVER_IMPL]     = "server impl",
    [MODULE_CLIENT_CORE]     = "client core",
    [MODULE_SERVER_CORE]     = "server core",
    [MODULE_OVERALL_CONTROL] = "overall controller",
    [MODULE_BUFFER_MANAGER]  = "buffer manager",
    [MODULE_ION_HELPER]      = "android ion helper",
    [MODULE_FS_HELPER]       = "fs helper",
    [MODULE_MAX_INVALID]     = "max_invalid",
};

static const char *const gModuleShortName[] = {
    [MODULE_OTHERS]          = "[OTHER]",
    [MODULE_CLIENT]          = "[CLIEN]",
    [MODULE_SERVER]          = "[SERVE]",
    [MODULE_CLIENT_IMPL]     = "[CIMPL]",
    [MODULE_SERVER_IMPL]     = "[SIMPL]",
    [MODULE_CLIENT_CORE]     = "[CCORE]",
    [MODULE_SERVER_CORE]     = "[SCORE]",
    [MODULE_OVERALL_CONTROL] = "[  OAC]",
    [MODULE_BUFFER_MANAGER]  = "[ BUFM]",
    [MODULE_ION_HELPER]      = "[IONHP]",
    [MODULE_FS_HELPER]       = "[ FSHP]",
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


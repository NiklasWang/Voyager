#ifndef _MODULES_H_
#define _MODULES_H_

#include <stdint.h>

namespace voyager {

enum ModuleType {
    MODULE_OTHERS,
    MODULE_SOCKET,
    MODULE_CLIENT,
    MODULE_SERVER,
    MODULE_CLIENT_IMPL,
    MODULE_SERVER_IMPL,
    MODULE_CLIENT_CORE,
    MODULE_SERVER_CORE,
    MODULE_OVERALL_CONTROL,
    MODULE_BUFFER_MANAGER,
    MODULE_ION_HELPER,
    MODULE_FS_HELPER,
    MODULE_MAX_INVALID,
};

ModuleType  getValidType(ModuleType type);
const char *getModuleName(ModuleType type);
const char *getModuleShortName(ModuleType type);
uint32_t getMaxLenofShortName();

};

#endif

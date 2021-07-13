#include <dlfcn.h>

#include "Common.h"
#include "IonOperator.h"

#define LIBION_LIBRARY_NAME "libion.so"

namespace voyager {

int32_t IonOperator::init()
{
    int32_t rc = NO_ERROR;

    if (SUCCEED(rc)) {
        if (NOTNULL(mDlHandler)) {
            rc = ALREADY_INITED;
        }
    }

    if (SUCCEED(rc)) {
        mDlHandler = dlopen(LIBION_LIBRARY_NAME, RTLD_LAZY | RTLD_GLOBAL);
        if (ISNULL(mDlHandler)) {
            LOGE(mModule, "Failed to dlopen lib %s %s.",
                LIBION_LIBRARY_NAME, dlerror());
            rc = PARAM_INVALID;
        }
    }

    if (SUCCEED(rc)) {
        ion_open_func     = (ion_open)dlsym(mDlHandler,     "ion_open");
        ion_close_func    = (ion_close)dlsym(mDlHandler,    "ion_close");
        ion_alloc_func    = (ion_alloc)dlsym(mDlHandler,    "ion_alloc");
        ion_alloc_fd_func = (ion_alloc_fd)dlsym(mDlHandler, "ion_alloc_fd");
        ion_sync_fd_func  = (ion_sync_fd)dlsym(mDlHandler,  "ion_sync_fd");
        ion_free_func     = (ion_free)dlsym(mDlHandler,     "ion_free");
        ion_map_func      = (ion_map)dlsym(mDlHandler,      "ion_map");
        ion_share_func    = (ion_share)dlsym(mDlHandler,    "ion_share");
        ion_import_func   = (ion_import)dlsym(mDlHandler,   "ion_import");
        if (ISNULL(ion_open_func)     ||
            ISNULL(ion_close_func)    ||
            ISNULL(ion_alloc_func)    ||
            ISNULL(ion_alloc_fd_func) ||
            ISNULL(ion_sync_fd_func)  ||
            ISNULL(ion_free_func)     ||
            ISNULL(ion_map_func)      ||
            ISNULL(ion_share_func)    ||
            ISNULL(ion_import_func)) {
            LOGE(mModule, "Failed to find func operations %s",
                dlerror());
            rc = NOT_FOUND;
        }
    }

    return rc;
}

bool IonOperator::inited()
{
    return NOTNULL(mDlHandler);
}

int32_t IonOperator::deinit()
{
    int32_t rc = NO_ERROR;

    if (SUCCEED(rc)) {
        if (NOTNULL(mDlHandler)) {
            dlclose(mDlHandler);
            mDlHandler        = nullptr;
            ion_open_func     = nullptr;
            ion_close_func    = nullptr;
            ion_alloc_func    = nullptr;
            ion_alloc_fd_func = nullptr;
            ion_sync_fd_func  = nullptr;
            ion_free_func     = nullptr;
            ion_map_func      = nullptr;
            ion_share_func    = nullptr;
            ion_import_func   = nullptr;
        }
    }

    return rc;
}

IonOperator::IonOperator() :
    Identifier(MODULE_ION_HELPER, "IonBufferMgr", "1.0.0"),
    ion_open_func(nullptr),
    ion_close_func(nullptr),
    ion_alloc_func(nullptr),
    ion_alloc_fd_func(nullptr),
    ion_sync_fd_func(nullptr),
    ion_free_func(nullptr),
    ion_map_func(nullptr),
    ion_share_func(nullptr),
    ion_import_func(nullptr),
    mDlHandler(nullptr)
{
}

IonOperator::~IonOperator()
{
    deinit();
}

}


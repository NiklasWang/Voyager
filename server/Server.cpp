#include "Server.h"
#include "ServerImpl.h"

namespace voyager {

#define CHECK_VALID_IMPL() \
    ({ \
        int32_t __rc = NO_ERROR; \
        if (ISNULL(mImpl)) { \
            LOGD(MODULE_SERVER, " impl not created"); \
            __rc = NOT_INITED; \
        } \
        __rc; \
    })

#define CONSTRUCT_IMPL() \
    ({ \
        int32_t __rc = NO_ERROR; \
        if (ISNULL(mImpl)) { \
            mImpl = new ServerImpl(); \
            if (ISNULL(mImpl)) { \
                LOGE(MODULE_SERVER, "Failed to create impl."); \
                __rc = NO_MEMORY; \
            } else { \
                __rc = mImpl->construct(mName, mEnableOverallControl); \
                if (FAILED(__rc)) { \
                    LOGE(MODULE_SERVER, "Failed to construct  impl"); \
                    delete mImpl; \
                    mImpl = NULL; \
                } else { \
                    LOGI(MODULE_SERVER, " impl constructed."); \
                } \
            } \
        } \
        __rc; \
    })


#define CONSTRUCT_IMPL_ONCE() \
    ({ \
        int32_t __rc = CHECK_VALID_IMPL(); \
        if (__rc == NOT_INITED) { \
            __rc = CONSTRUCT_IMPL(); \
        } \
        __rc; \
    })

int32_t Server::request(DataCbFunc dataCbFunc, SyncMode mode)
{
    int32_t rc = CONSTRUCT_IMPL_ONCE();
    return SUCCEED(rc) ? mImpl->request(dataCbFunc, mode) : rc;
}

int32_t Server::enqueue(void *dat)
{
    int32_t rc = CONSTRUCT_IMPL_ONCE();
    return SUCCEED(rc) ? mImpl->enqueue(dat) : rc;
}

int32_t Server::request(FdCbFunc fdCbFunc, SyncMode mode)
{
    int32_t rc = CONSTRUCT_IMPL_ONCE();
    return SUCCEED(rc) ? mImpl->request(fdCbFunc, mode) : rc;
}

int32_t Server::enqueue(int32_t fd)
{
    int32_t rc = CONSTRUCT_IMPL_ONCE();
    return SUCCEED(rc) ? mImpl->enqueue(fd) : rc;
}

int32_t Server::request(FrameCbFunc frameCbFunc, SyncMode mode)
{
    int32_t rc = CONSTRUCT_IMPL_ONCE();
    return SUCCEED(rc) ? mImpl->request(frameCbFunc, mode) : rc;
}

int32_t Server::enqueue(void *dat, int32_t format)
{
    int32_t rc = CONSTRUCT_IMPL_ONCE();
    return SUCCEED(rc) ? mImpl->enqueue(dat, format) : rc;
}

int32_t Server::request(EventCbFunc eventCbFunc, SyncMode mode)
{
    int32_t rc = CONSTRUCT_IMPL_ONCE();
    return SUCCEED(rc) ? mImpl->request(eventCbFunc, mode) : rc;
}

int32_t Server::cancel(RequestType type)
{
    int32_t rc = CONSTRUCT_IMPL_ONCE();
    return SUCCEED(rc) ? mImpl->cancel(type) : rc;
}

Server::Server(const char *name, bool enableOverallControl) :
    mName(name),
    mEnableOverallControl(enableOverallControl),
    mImpl(nullptr)
{
}

Server::~Server()
{
    if (NOTNULL(mImpl)) {
        mImpl->destruct();
        delete mImpl;
        mImpl = NULL;
    }
}

};


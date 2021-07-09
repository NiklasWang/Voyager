#include "Server.h"
#include "ServerImpl.h"

namespace voyager {

#define CHECK_VALID_IMPL() \
    ({ \
        int32_t __rc = NO_ERROR; \
        if (ISNULL(mImpl)) { \
            LOGD(MODULE_VOYAGER, " impl not created"); \
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
                LOGE(MODULE_VOYAGER, "Failed to create  impl"); \
                __rc = NOT_INITED; \
            } else { \
                __rc = mImpl->construct(); \
                if (!SUCCEED(__rc)) { \
                    LOGE(MODULE_VOYAGER, "Failed to construct  impl"); \
                    delete mImpl; \
                    mImpl = NULL; \
                } else { \
                    LOGI(MODULE_VOYAGER, " impl constructed."); \
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

int32_t Server::request(RequestType type)
{
    int32_t rc = CONSTRUCT_IMPL_ONCE();
    return SUCCEED(rc) ? mImpl->request(type) : rc;
}

int32_t Server::abort(RequestType type)
{
    int32_t rc = CONSTRUCT_IMPL_ONCE();
    return SUCCEED(rc) ? mImpl->abort(type) : rc;
}

int32_t Server::enqueue(RequestType type, int32_t id)
{
    int32_t rc = CONSTRUCT_IMPL_ONCE();
    return SUCCEED(rc) ? mImpl->enqueue(type, id) : rc;
}

int32_t Server::setCallback(RequestCbFunc requestCb)
{
    int32_t rc = CONSTRUCT_IMPL_ONCE();
    return SUCCEED(rc) ? mImpl->setCallback(requestCb) : rc;
}

int32_t Server::setCallback(EventCbFunc eventCb)
{
    int32_t rc = CONSTRUCT_IMPL_ONCE();
    return SUCCEED(rc) ? mImpl->setCallback(eventCb) : rc;
}

int32_t Server::setCallback(DataCbFunc dataCb)
{
    int32_t rc = CONSTRUCT_IMPL_ONCE();
    return SUCCEED(rc) ? mImpl->setCallback(dataCb) : rc;
}

Server::Server() :
    mImpl(NULL)
{
}

Server::~Server()
{
    if (!ISNULL(mImpl)) {
        mImpl->destruct();
        delete mImpl;
        mImpl = NULL;
    }
}

};


#include "Common.h"
#include "Client.h"
#include "ClientCore.h"

namespace voyager {

#define CHECK_VALID_CORE() \
    ({ \
        int32_t __rc = NO_ERROR; \
        if (ISNULL(mCore)) { \
            __rc = NOT_INITED; \
        } \
        __rc; \
    })

#define CONSTRUCT_CORE() \
    ({ \
        int32_t __rc = NO_ERROR; \
        if (ISNULL(mCore)) { \
            mCore = new ClientCore(mName); \
            if (ISNULL(mCore)) { \
                LOGE(MODULE_CLIENT, "Failed to create client core."); \
                __rc = NOT_INITED; \
            } else { \
                __rc = mCore->construct(); \
                if (FAILED(__rc)) { \
                    LOGE(MODULE_CLIENT, "Failed to construct client core, %d", __rc); \
                } \
            }\
        } \
        __rc; \
    })

#define CONSTRUCT_CORE_ONCE() \
    ({ \
        int32_t __rc = CHECK_VALID_CORE(); \
        if (__rc == NOT_INITED) { \
            __rc = CONSTRUCT_CORE(); \
        } \
        __rc; \
    })

int32_t Client::send(void *dat, int64_t len)
{
    int32_t rc = CONSTRUCT_CORE_ONCE();
    return SUCCEED(rc) ? mCore->send(dat, len) : rc;
}

int32_t Client::send(int32_t fd, int64_t len)
{
    int32_t rc = CONSTRUCT_CORE_ONCE();
    return SUCCEED(rc) ? mCore->send(fd, len) : rc;
}

int32_t Client::send(int32_t fd, int64_t len, int32_t format)
{
    int32_t rc = CONSTRUCT_CORE_ONCE();
    return SUCCEED(rc) ? mCore->send(fd, len, format) : rc;
}

int32_t Client::send(int32_t event, int32_t arg1, int32_t arg2)
{
    int32_t rc = CONSTRUCT_CORE_ONCE();
    return SUCCEED(rc) ? mCore->send(event, arg1, arg2) : rc;
}

bool Client::requested(RequestType type)
{
    int32_t rc = CONSTRUCT_CORE_ONCE();
    return SUCCEED(rc) ? mCore->requested(type) : false;
}

Client::Client(const char *name) :
    mCore(nullptr),
    mName(name)
{
}

Client::~Client()
{
    if (NOTNULL(mCore)) {
        SECURE_DELETE(mCore);
    }
}

};


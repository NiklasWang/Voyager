#include "IonBufferMgr.h"
#include "IonBufferMgrImpl.h"

namespace voyager {

IonBufferMgr::IonBufferMgr() :
    Identifier(MODULE_ION_HELPER, "IonBufferMgr", "1.0.0"),
    mImpl(nullptr),
    mLock(PTHREAD_MUTEX_INITIALIZER)
{
}

IonBufferMgr::~IonBufferMgr()
{
    if (mImpl) {
        deinit();
    }
}

int32_t IonBufferMgr::init()
{
    int32_t rc = NO_ERROR;

    if (ISNULL(mImpl)) {
        AutoExternalLock l(&mLock);
        if (ISNULL(mImpl)) {
            mImpl = new IonBufferMgrImpl();
            if (FAILED(rc)) {
                LOGE(mModule, "Failed to new ion buffer mgr impl.");
                rc = NO_MEMORY;
            } else {
                rc = mImpl->init();
                if (FAILED(rc)) {
                    LOGE(mModule, "Failed to init buffer mgr impl, %d", rc);
                }
            }
        }
    }

    return rc;
}

int32_t IonBufferMgr::deinit()
{
    int32_t rc = NO_ERROR;

    if (NOTNULL(mImpl)) {
        AutoExternalLock l(&mLock);
        if (NOTNULL(mImpl)) {
            rc = mImpl->deinit();
            if (FAILED(rc)) {
                LOGE(mModule, "Failed to deinit ion buffer mgr impl, %d", rc);
            }
            SECURE_DELETE(mImpl);
        }
    }

    return rc;
}

#define INIT_IMPL_ONCE() \
    ({ \
        int32_t __rc = NO_ERROR; \
        if (ISNULL(mImpl)) { \
            __rc = init(); \
            if (FAILED(__rc)) { \
                LOGE(mModule, "Failed to init ion buffer mgr, %d", rc); \
            } \
        } \
        __rc; \
    })

int32_t IonBufferMgr::allocate(void **buf, int64_t len)
{
    int32_t rc = INIT_IMPL_ONCE();
    return SUCCEED(rc) ? mImpl->allocate(buf, len) : rc;
}

int32_t IonBufferMgr::allocate(void **buf, int64_t len, int32_t *fd)
{
    int32_t rc = INIT_IMPL_ONCE();
    return SUCCEED(rc) ? mImpl->allocate(buf, len, fd) : rc;
}

int32_t IonBufferMgr::import(void **buf, int32_t fd, int64_t len)
{
    int32_t rc = INIT_IMPL_ONCE();
    return SUCCEED(rc) ? mImpl->import(buf, fd, len) : rc;
}

int32_t IonBufferMgr::flush(void *buf)
{
    int32_t rc = INIT_IMPL_ONCE();
    return SUCCEED(rc) ? mImpl->flush(buf) : rc;
}

int32_t IonBufferMgr::flush(int32_t fd)
{
    int32_t rc = INIT_IMPL_ONCE();
    return SUCCEED(rc) ? mImpl->flush(fd) : rc;
}

int32_t IonBufferMgr::release(void *buf)
{
    int32_t rc = INIT_IMPL_ONCE();
    return SUCCEED(rc) ? mImpl->release(buf) : rc;
}

int32_t IonBufferMgr::release(int32_t fd)
{
    int32_t rc = INIT_IMPL_ONCE();
    return SUCCEED(rc) ? mImpl->release(fd) : rc;
}

};


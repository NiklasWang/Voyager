#include "BufferMgr.h"

#if defined(BUILD_LINUX_X86_64) || defined(BUILD_LINUX_X86)
#include "FSBufferMgr.h"
#else defined(BUILD_ANDROID_AP)
#include "IonBufferMgr.h"
#else
#error Not supported this platform, supported BUILD_LINUX_X86_64, BUILD_LINUX_X86 or BUILD_ANDROID_AP.
#endif

#define CREATE_IMPL() \
    ({ \
#if defined(BUILD_LINUX_X86_64) || defined(BUILD_LINUX_X86) \
        new IonBufferMgr(); \
#else defined(BUILD_ANDROID_AP) \
        new FSBufferMgr(); \
#endif
    })

#define CONSTRUCT_IMPL() \
    ({ \
        int32_t __rc = NO_ERROR; \
        if (ISNULL(mMgr)) { \
            CREATE_IMPL(); \
            if (ISNULL(mMgr)) { \
                LOGE(MODULE_BUF_MANAGER, "Failed to create buffer manager."); \
                __rc = NOT_INITED; \
            } \
        } \
        __rc; \
    })

#define CHECK_VALID_IMPL() \
        ({ \
            int32_t __rc = NO_ERROR; \
            if (ISNULL(mMgr)) { \
                __rc = NOT_INITED; \
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


namespace voyager {

BufferMgr::BufferMgr() :
    Identifier(MODULE_BUFFER_MANAGER, "BufferMgr", "1.0.0"),
    mMgr(NULL)
{
}

BufferMgr::~BufferMgr()
{
    if (NOTNULL(mMgr)) {
        SECURE_DELETE(mMgr);
    }
}

int32_t BufferMgr::alloc(void **buf, int64_t len)
{
    int32_t rc = CONSTRUCT_IMPL_ONCE();
    return SUCCEED(rc) ? mMgr->alloc(buf, len) : rc;
}

int32_t BufferMgr::alloc(void **buf, int64_t len, int32_t *fd)
{
    int32_t rc = CONSTRUCT_IMPL_ONCE();
    return SUCCEED(rc) ? mMgr->alloc(buf, len, fd) : rc;
}

int32_t BufferMgr::import(void **buf, int32_t fd, int64_t len)
{
    int32_t rc = CONSTRUCT_IMPL_ONCE();
    return SUCCEED(rc) ? mMgr->import(buf, fd, len) : rc;
}

int32_t BufferMgr::flush(void *buf)
{
    int32_t rc = CONSTRUCT_IMPL_ONCE();
    return SUCCEED(rc) ? mMgr->flush(buf) : rc;
}

int32_t BufferMgr::flush(int32_t fd)
{
    int32_t rc = CONSTRUCT_IMPL_ONCE();
    return SUCCEED(rc) ? mMgr->flush(fd) : rc;
}

int32_t BufferMgr::release(void *buf)
{
    int32_t rc = CONSTRUCT_IMPL_ONCE();
    return SUCCEED(rc) ? mMgr->release(buf) : rc;
}

int32_t BufferMgr::release(int32_t fd)
{
    int32_t rc = CONSTRUCT_IMPL_ONCE();
    return SUCCEED(rc) ? mMgr->release(fd) : rc;
}

};


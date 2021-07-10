#include "OverallControlAllocator.h"

namespace voyager {

int32_t OverallControlAllocator::alloc()
{
    int32_t rc = NO_ERROR;

    if (SUCCEED(rc)) {
        if (mBufFd > 0) {
            LOGE(mModule, "Already allcated.");
            rc = ALREADY_INITED;
        }
    }

    if (SUCCEED(rc)) {
        rc = mBufMgr.alloc(&mLayout, sizeof(OverallControlLayout), &mBufFd);
        if (FAILED(rc)) {
            LOGE(mModule, "Failed to alloc %d bytes for overall control, %d",
                sizeof(OverallControlLayout), rc);
        }
    }

    return rc;
}

int32_t OverallControlAllocator::import(int32_t fd, int64_t len)
{
    int32_t rc = NO_ERROR;

    if (SUCCEED(rc)) {
        if (mBufFd > 0) {
            LOGE(mModule, "Already allcated.");
            rc = ALREADY_INITED;
        }
    }

    if (SUCCEED(rc)) {
        if (fd <= 0 || len != sizeof(OverallControlLayout)) {
            LOGE(mModule, "Invalid fd %d or len %d/%d", fd,
                len, sizeof(OverallControlLayout));
            rc = PARAM_INVALID;
        }
    }

    if (SUCCEED(rc)) {
        rc = mBufMgr.import(&mLayout, fd, len);
        if (FAILED(rc)) {
            LOGE(mModule, "Failed to import %d bytes for overall control, %d",
                sizeof(OverallControlLayout), rc);
        }
    }

    return rc;
}

void OverallControlAllocator::flush()
{
    mBufMgr.flush(mBufFd);
}

int32_t OverallControlAllocator::free()
{
    int32_t rc = NO_ERROR;

    if (SUCCEED(rc)) {
        if (mBufFd <= 0) {
            LOGE(mModule, "Not allcated.");
            rc = ALREADY_INITED;
        }
    }

    if (SUCCEED(rc)) {
        rc = mBufMgr.release(mBufFd);
        if (FAILED(rc)) {
            LOGE(mModule, "Failed to release buf, %d", rc);
        }
    }

    return rc;
}

int32_t OverallControlAllocator::getFd()
{
    return mBufFd <= 0 ? -1 : mBufFd;
}

int32_t OverallControlAllocator::getPtr()
{
    return mLayout;
}

OverallControlAllocator::OverallControlAllocator() :
    Identifier(MODULE_OVERALL_CONTROL, "OverallControlAllocator", "1.0.0"),
    mBufMgr(nullptr),
    mBufFd(0)
{
}

OverallControlAllocator::~OverallControlAllocator()
{
    if (mBufFd > 0) {
        free();
    }
}


#define CHECK_ALLOCATION_FIRST() \
    ({ \
        int32_t __rc = NO_ERROR; \
        if (mBufFd <= 0) { \
            __rc = alloc(); \
            if (FAILED(__rc) { \
                LOGE(mModule, "Overall control alloc failed, %d", __rc); \
            } \
        } \
        __rc; \
    })

int32_t OverallControlAllocator::addServer(const char *path,
    const char *name, int32_t maxConnection)
{
    int32_t rc = CHECK_ALLOCATION_FIRST();
    return SUCCEED(rc) ? OverallControl::addServer(path, name, maxConnection) : rc;
}

int32_t OverallControlAllocator::removeServer(const char *path, const char *name)
{
    int32_t rc = CHECK_ALLOCATION_FIRST();
    return SUCCEED(rc) ? OverallControl::removeServer(path, name) : rc;
}

int32_t OverallControlAllocator::addClient(const char *path, const char *name)
{
    int32_t rc = CHECK_ALLOCATION_FIRST();
    return SUCCEED(rc) ? OverallControl::addClient(path, name) : rc;
}

int32_t OverallControlAllocator::removeClient(const char *path, const char *name)
{
    int32_t rc = CHECK_ALLOCATION_FIRST();
    return SUCCEED(rc) ? OverallControl::removeClient(path, name) : rc;
}

int32_t OverallControlAllocator::addServer(const char *ip, int32_t port, int32_t maxConnection)
{
    int32_t rc = CHECK_ALLOCATION_FIRST();
    return SUCCEED(rc) ? OverallControl::addServer(ip, port, maxConnection) : rc;
}

int32_t OverallControlAllocator::removeServer(const char *ip, int32_t port)
{
    int32_t rc = CHECK_ALLOCATION_FIRST();
    return SUCCEED(rc) ? OverallControl::removeServer(ip, port) : rc;
}

int32_t OverallControlAllocator::addClient(const char *ip, int32_t port)
{
    int32_t rc = CHECK_ALLOCATION_FIRST();
    return SUCCEED(rc) ? OverallControl::addClient(ip, port) : rc;
}

int32_t OverallControlAllocator::removeClient(const char *ip, int32_t port)
{
    int32_t rc = CHECK_ALLOCATION_FIRST();
    return SUCCEED(rc) ? OverallControl::removeClient(ip, port) : rc;
}

void OverallControlAllocator::setLayout(void *layout)
{
    int32_t rc = CHECK_ALLOCATION_FIRST();
    return SUCCEED(rc) ? OverallControl::setLayout(layout) : rc;
}

int32_t OverallControlAllocator::initLayout()
{
    int32_t rc = CHECK_ALLOCATION_FIRST();
    return SUCCEED(rc) ? OverallControl::initLayout() : rc;
}

void OverallControlAllocator::dump(const char *prefix)
{
    int32_t rc = CHECK_ALLOCATION_FIRST();
    flush();
    return SUCCEED(rc) ? OverallControl::dump(prefix) : rc;
}

};

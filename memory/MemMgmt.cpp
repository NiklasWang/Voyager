#include "MemMgmt.h"
#include "MemMgmtImpl.h"

namespace voyager {

MemMgmt gDefaultMemPool;

#define CHECK_IMPL_INSTANCE() \
    ({ \
        int32_t rc = NO_ERROR; \
        if (ISNULL(mImpl)) { \
            mImpl = new MemMgmtImpl(); \
            if (ISNULL(mImpl)) { \
                LOGE(mModule, "Failed to new memory management impl."); \
                rc = NO_MEMORY; \
            } \
        } \
        rc; \
    })

void *MemMgmt::malloc(size_t size, const char *file,
    const char *func, const int32_t line)
{
    int32_t rc = CHECK_IMPL_INSTANCE();
    return SUCCEED(rc) ? mImpl->malloc(size, file, func, line) : nullptr;
}

void MemMgmt::free(void *data, const char *file,
    const char *func, const int32_t line)
{
    int32_t rc = CHECK_IMPL_INSTANCE();
    if (SUCCEED(rc)) {
        mImpl->free(data, file, func, line);
    }
}

void MemMgmt::dump(const char *file,
    const char *func, const int32_t line)
{
    int32_t rc = CHECK_IMPL_INSTANCE();
    if (SUCCEED(rc)) {
        mImpl->dump(file, func, line);
    }
}

MemMgmt::MemMgmt(std::string name, bool pool) :
    Identifier(MODULE_MEMORY_POOL, "MemMgmt", "v1.2.0"),
    mName(name),
    mPool(pool),
    mImpl(nullptr)
{
}

MemMgmt::~MemMgmt()
{
    if (NOTNULL(mImpl)) {
        SECURE_DELETE(mImpl);
    }
}

};


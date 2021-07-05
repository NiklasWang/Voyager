#ifndef MEM_MGMT_IMPL_H_
#define MEM_MGMT_IMPL_H_

#include <string>
#include <list>

#include "Common.h"
#include "RWLock.h"

namespace voyager {

struct MemoryPool;

class MemMgmtImpl :
    virtual public noncopyable {

public:
    void *malloc(size_t size, const char *file = "",
        const char *func = "", const int32_t line = 0);
    void free(void *data, const char *file = "",
        const char *func = "", const int32_t line = 0);
    void dump(const char *file = "",
        const char *func = "", const int32_t line = 0);

public:
    MemMgmtImpl(std::string name = "defaultMemPool", bool pool = true);
    ~MemMgmtImpl();

private:
    void *__malloc(size_t size);
    void __free(void *ptr);

private:
    struct MemInfo {
        int32_t       id;
        void         *data;
        size_t        size;
        const char   *file;
        const char   *func;
        const int32_t line;
    };

private:
    ModuleType    mModule;
    RWLock        mMgrLock;
    std::string   mName;
    std::list<MemInfo> mMemDB;
    RWLock        mDBLock;
    int32_t       mCnt;
    int32_t       mId;
    size_t        mReserveLen;
    size_t        mTotalSize;
    bool          mEnablePool;
    MemoryPool   *mPool;
    static const int64_t mMaxTotalSize;
    static const int64_t mMaxAllocSize;
};

};

#endif

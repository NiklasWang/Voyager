#ifndef MEMORY_MEMORY_H_
#define MEMORY_MEMORY_H_

#include "MemMgmt.h"

namespace voyager {

#ifdef ENABLE_MEMORY_POOL

extern MemMgmt gDefaultMemPool;

#define Malloc(size)  gDefaultMemPool.malloc((size), __FILE__, __FUNCTION__, __LINE__)
#define Free(ptr)     gDefaultMemPool.free((ptr), __FILE__, __FUNCTION__, __LINE__)
#define MemoryDump()  gDefaultMemPool.dump(__FILE__, __FUNCTION__, __LINE__)

#else

#define Malloc(size)  malloc((size))
#define Free(ptr)     free((ptr))
#define MemoryDump()

#endif


#define DeclareMemPool(pool)   (pool) = new MemMgmt("MemPool")
#define DeleteMemPool(pool)    do { delete (pool); (pool) = NULL; } while(0)
#define MallocFrom(pool, size) (pool).malloc((size), __FILE__, __FUNCTION__, __LINE__)
#define FreeFrom(pool, ptr)    (pool).free((ptr), __FILE__, __FUNCTION__, __LINE__)
#define DumpFrom(pool)         (pool).dump(__FILE__, __FUNCTION__, __LINE__)


enum MemoryRecycleStrategy {
    MEMORY_RECYCLE_STRATEGY_NORMAL,
    MEMORY_RECYCLE_STRATEGY_LAZY,
    MEMORY_RECYCLE_STRATEGY_SAVING,
    MEMORY_RECYCLE_STRATEGY_MAX_INVALID,
};

};

#endif

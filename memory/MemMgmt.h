#ifndef MEM_MGMT_H_
#define MEM_MGMT_H_

#include <string>
#include <list>

#include "Common.h"

namespace voyager {

struct MemMgmtImpl;

class MemMgmt :
    public Identifier,
    virtual public noncopyable {

public:
    void *malloc(size_t size, const char *file = "",
        const char *func = "", const int32_t line = 0);
    void free(void *data, const char *file = "",
        const char *func = "", const int32_t line = 0);
    void dump(const char *file = "",
        const char *func = "", const int32_t line = 0);

public:
    MemMgmt(std::string name = "defaultMemPool", bool pool = true);
    ~MemMgmt();

private:
    std::string  mName;
    bool         mPool;
    MemMgmtImpl *mImpl;
};

};

#endif

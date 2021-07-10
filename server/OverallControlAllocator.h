#ifndef _OVERALL_CONTROL_ALLOCATOR_H_
#define _OVERALL_CONTROL_ALLOCATOR_H_

#include "Common.h"
#include "OverallControl.h"

namespace voyager {

class OverallControlAllocator :
    public OverallControl,
    public Identifier {
public:

    int32_t alloc();
    int32_t import(int32_t fd, int64_t len);
    void    flush();
    int32_t free();
    int32_t getFd();
    int32_t getPtr();

public:
    int32_t addServer(const char *path, const char *name, int32_t maxConnection = 1);
    int32_t removeServer(const char *path, const char *name);
    int32_t addClient(const char *path, const char *name);
    int32_t removeClient(const char *path, const char *name);
    int32_t addServer(const char *ip, int32_t port, int32_t maxConnection = 1);
    int32_t removeServer(const char *ip, int32_t port);
    int32_t addClient(const char *ip, int32_t port);
    int32_t removeClient(const char *ip, int32_t port);
    void    setLayout(const OverallControlLayout *layout);
    int32_t initLayout();
    void    dump(const char *prefix = "");

public:
    OverallControlAllocator();
    virtual ~OverallControlAllocator();

private:
    BufferMgr mBufMgr;
    int32_t   mBufFd;
};

};

#endif

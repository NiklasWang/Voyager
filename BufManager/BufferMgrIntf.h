#ifndef _BUFFER_MANAGER_INTF_H_
#define _BUFFER_MANAGER_INTF_H_

#include <stdint.h>

class BufferMgrIntf
{
public:
    virtual int32_t alloc(void **buf, int64_t len) = 0;
    virtual int32_t alloc(void **buf, int64_t len, int32_t *fd) = 0;
    virtual int32_t import(void **buf, int32_t fd, int64_t len) = 0;
    virtual int32_t flush(void *buf) = 0;
    virtual int32_t flush(int32_t fd) = 0;
    virtual int32_t release(void *buf) = 0;
    virtual int32_t release(int32_t fd) = 0;

public:
    virtual ~BufferMgrIntf() {}
};

#endif


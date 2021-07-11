#ifndef _BUFFER_MANAGER_H__
#define _BUFFER_MANAGER_H__

#include "Common.h"
#include "BufferMgrIntf.h"

namespace voyager {

class BufferMgr :
    public BufferMgrIntf,
    public Identifier,
    public noncopyable  {
public:

    virtual int32_t alloc(void **buf, int64_t len) override;
    virtual int32_t alloc(void **buf, int64_t len, int32_t *fd) override;
    virtual int32_t import(void **buf, int32_t fd, int64_t len) override;
    virtual int32_t flush(void *buf) override;
    virtual int32_t flush(int32_t fd) override;
    virtual int32_t release(void *buf) override;
    virtual int32_t release(int32_t fd) override;

public:
    BufferMgr();
    virtual ~BufferMgr();

private:
    BufferMgrIntf *mMgr;
};

};

#endif


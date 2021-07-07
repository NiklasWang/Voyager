#ifndef _FS_BUFFER_MANAGER_IMPL_H__
#define _FS_BUFFER_MANAGER_IMPL_H__

#include <list>
#include <string>

#include "common.h"
#include "BufferMgrIntf.h"

namespace voyager {

class FSBufferMgr :
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
    FSBufferMgr();
    virtual ~FSBufferMgr();

private:
    struct Buffer {
        int32_t  fd;
        void    *ptr;
        int64_t  len;
        std::string fsName;
    };

private:
    Buffer *findBuf(void *buf);
    Buffer *findBuf(int32_t fd);
    int32_t allocate(Buffer *buf, int64_t len);
    int32_t import(Buffer *buf, int32_t fd, int64_t len);
    int32_t release(Buffer *buf);
    void    clear();
    int32_t removeNotOccupiedFiles(const std::string &dir);

private:
    std::list<Buffer> mBuffers;
    int64_t mPageSize;
};

};

#endif


#ifndef _ION_BUFFER_MANAGER_H__
#define _ION_BUFFER_MANAGER_H__

#include <list>

#include "common.h"
#include "BufferMgrIntf.h"
#include "IonOperator.h"

namespace voyager {

class IonBufferMgr :
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
    int32_t init();
    int32_t deinit();
    IonBufferMgr();
    virtual ~IonBufferMgr();

private:
    IonBufferMgrImpl *mImpl;
    pthread_mutex_t   mLock;
};

};

#endif


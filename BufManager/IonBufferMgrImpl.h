#ifndef _ION_BUFFER_MANAGER_IMPL_H__
#define _ION_BUFFER_MANAGER_IMPL_H__

#include <list>

#include "common.h"
#include "BufferMgrIntf.h"
#include "IonOperator.h"

namespace voyager {

class IonBufferMgrImpl :
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
    IonBufferMgrImpl();
    virtual ~IonBufferMgrImpl();

private:
    struct Buffer {
        int32_t  fd;
        void    *ptr;
        int64_t  len;
        ion_user_handle_t handler;
    };

private:
    Buffer *findBuf(void *buf);
    Buffer *findBuf(int32_t fd);
    int32_t allocate(Buffer *buf, int64_t len);
    int32_t import(Buffer *buf, int32_t fd, int64_t len);
    int32_t cacheIoctl(void *buf, uint32_t cmd);
    int32_t cacheIoctl(Buffer *buf, uint32_t cmd);
    int32_t release(Buffer *buf);
    void    clear();

private:
    std::list<Buffer> mBuffers;

    static int32_t     kIonFd;
    static IonOperator kIonHelper;
    static int32_t     kIonRefs;
    static pthread_mutex_t kInitLocker;
};

};

#endif


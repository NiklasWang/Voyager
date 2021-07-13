#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/ioctl.h>

#include "IonBufferMgrImpl.h"
#include "IonOperator.h"

namespace voyager {

int32_t     IonBufferMgrImpl::kIonFd = -1;
IonOperator IonBufferMgrImpl::kIonHelper;
int32_t     IonBufferMgrImpl::kIonRefs = 0;
pthread_mutex_t IonBufferMgrImpl::kInitLocker = PTHREAD_MUTEX_INITIALIZER;

IonBufferMgrImpl::IonBufferMgrImpl() :
    Identifier(MODULE_ION_HELPER, "IonBufferMgrImpl", "1.0.0"),
    mPageSize(0)
{
}

IonBufferMgrImpl::~IonBufferMgrImpl()
{
}

int32_t IonBufferMgrImpl::init()
{
    int32_t rc = NO_ERROR;

    if (SUCCEED(rc)) {
        if (!kIonHelper.inited()) {
            rc = kIonHelper.init();
            if (FAILED(rc)) {
                LOGE(mModule, "Failed to init ion helper, %d", rc);
            }
        }
    }

    if (SUCCEED(rc)) {
        if (kIonFd < 0) {
            kIonFd = open("/dev/ion", O_RDONLY);
            if (kIonFd < 0) {
                LOGE(mModule, "Failed to open /dev/ion, %s", strerror(errno));
                kIonHelper.deinit();
                rc = SYS_ERROR;
            }
        }
    }

    if (SUCCEED(rc)) {
        kIonRefs++;
    }

    if (SUCCEED(rc)) {
        mPageSize = sysconf(_SC_PAGESIZE);
        if (mPageSize <= 0) {
            LOGE(mModule, "Failed to get page size, %d", mPageSize);
            rc = SYS_ERROR;
        }
    }

    return rc;
}

int32_t IonBufferMgrImpl::deinit()
{
    int32_t rc = NO_ERROR;
    kIonRefs--;

    if (SUCCEED(rc)) {
        if (kIonRefs == 0) {
            rc = kIonHelper.deinit();
            if (FAILED(rc)) {
                LOGE(mModule, "Failed to deinit ion helper, %d", rc);
            }
        }
    }

    if (SUCCEED(rc)) {
        if (kIonRefs == 0 && kIonFd > 0) {
            close(kIonFd);
            kIonFd = -1;
        }
    }

    return rc;
}

int32_t IonBufferMgrImpl::alloc(void **buf, int64_t len)
{
    int32_t fd = -1;
    return alloc(buf, len, &fd);
}

int32_t IonBufferMgrImpl::alloc(void **buf, int64_t len, int32_t *fd)
{
    int32_t rc = NO_ERROR;
    Buffer buffer;
    int64_t aligned = len;

    if (SUCCEED(rc)) {
        aligned = align_len_to_size(len, mPageSize);
        rc = alloc(&buffer, aligned);
        if (SUCCEED(rc)) {
            *buf = buffer.ptr;
            *fd  = buffer.fd;
            mBuffers.push_back(buffer);
        }
    }

    return rc;
}

int32_t IonBufferMgrImpl::alloc(Buffer *buf, int64_t len)
{
    int32_t rc = NO_ERROR;
    int32_t fd = -1;
    void *addr = NULL;
    ion_user_handle_t handle;

    if (SUCCEED(rc)) {
        rc = kIonHelper.ion_alloc_func(kIonFd, len, 0,
            ION_HEAP_SYSTEM_MASK, ION_FLAG_CACHED, &handle);
        if (FAILED(rc)) {
            LOGE(mModule, "Failed to alloc %d bytes from ion", len);
        }
    }

    if (SUCCEED(rc)) {
        rc = kIonHelper.ion_share_func(kIonFd, handle, &fd);
        if (FAILED(rc)) {
            LOGE(mModule, "Failed to share, free allocated ion mem, %d", rc);
            rc = kIonHelper.ion_free_func(kIonFd, handle);
            if (FAILED(rc)) {
                LOGE(mModule, "Failed to free ion memory, %d", rc);
            }
            rc = SYS_ERROR;
        }
    }

    if (SUCCEED(rc)) {
        addr = mmap(NULL, len, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
        if (addr == MAP_FAILED) {
            LOGE(mModule, "Failed to mmap on fd %d len %d", fd, len);
            rc = kIonHelper.ion_free_func(kIonFd, handle);
            if (FAILED(rc)) {
                LOGE(mModule, "Failed to free ion memory, %d", rc);
            }
            rc = SYS_ERROR;
        }
    }

    if (SUCCEED(rc)) {
        buf->ptr = addr;
        buf->len = len;
        buf->fd  = fd;
        buf->handler = handle;
    }

    return rc;
}

int32_t IonBufferMgrImpl::import(void **buf, int32_t fd, int64_t len)
{
    int32_t rc = NO_ERROR;
    Buffer buffer;

    if (SUCCEED(rc)) {
        rc = import(&buffer, fd, len);
        if (SUCCEED(rc)) {
            *buf = buffer.ptr;
            mBuffers.push_back(buffer);
        }
    }

    return rc;
}

int32_t IonBufferMgrImpl::import(Buffer *buf, int32_t fd, int64_t len)
{
    int32_t rc = NO_ERROR;
    void *addr = NULL;
    ion_user_handle_t handle;

    if (SUCCEED(rc)) {
        rc = kIonHelper.ion_import_func(kIonFd, fd, &handle);
        if (FAILED(rc)) {
            LOGE(mModule, "Failed to import ion memory with fd %d", fd);
        }
    }

    if (SUCCEED(rc)) {
        addr = mmap(NULL, len, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
        if (addr == MAP_FAILED) {
            LOGE(mModule, "Failed to mmap with fd %d", fd);
            rc = SYS_ERROR;
        }
    }

    if (SUCCEED(rc)) {
        buf->ptr = addr;
        buf->len = len;
        buf->fd = fd;
        buf->handler = handle;
    }

    return rc;
}

int32_t IonBufferMgrImpl::flush(void *buf)
{
    int32_t rc = NO_ERROR;
    Buffer *buffer = findBuf(buf);

    if (SUCCEED(rc)) {
        rc = kIonHelper.ion_sync_fd_func(buffer->fd, buffer->handler);
        if (FAILED(rc)) {
            LOGE(mModule, "Failed to sync ion fd, %d", rc);
        }
    }

    return rc;
}

int32_t IonBufferMgrImpl::flush(int32_t fd)
{
    int32_t rc = NO_ERROR;
    Buffer *buffer = findBuf(fd);

    if (SUCCEED(rc)) {
        rc = kIonHelper.ion_sync_fd_func(buffer->fd, buffer->handler);
        if (FAILED(rc)) {
            LOGE(mModule, "Failed to sync ion fd, %d", rc);
        }
    }

    return rc;
}

IonBufferMgrImpl::Buffer *IonBufferMgrImpl::findBuf(void *buf)
{
    Buffer *result = NULL;

    for (auto &itr : mBuffers) {
        if (buf == itr.ptr) {
            result = &itr;
            break;
        }
    }

    return result;
}

IonBufferMgrImpl::Buffer *IonBufferMgrImpl::findBuf(int32_t fd)
{
    Buffer *result = NULL;

    for (auto &itr : mBuffers) {
        if (fd == itr.fd) {
            result = &itr;
            break;
        }
    }

    return result;
}

int32_t IonBufferMgrImpl::release(void *buf)
{
    int32_t rc = NO_ERROR;
    Buffer *buffer = findBuf(buf);

    if (ISNULL(buffer)) {
        LOGE(mModule, "Failed to find memory.");
        rc = NO_MEMORY;
    }

    if (SUCCEED(rc)) {
        rc = release(buffer);
        if (FAILED(rc)) {
            LOGE(mModule, "Failed to release buf, %d", rc);
        }
    }

    return rc;
}

int32_t IonBufferMgrImpl::release(int32_t fd)
{
    Buffer *buffer = findBuf(fd);
    return release(buffer->ptr);
}

int32_t IonBufferMgrImpl::release(Buffer *buf)
{
    int32_t rc = NO_ERROR;

    if (SUCCEED(rc)) {
        if (NOTNULL(buf->ptr)) {
            munmap(buf->ptr, buf->len);
        } else {
            rc = PARAM_INVALID;
        }
        if (buf->fd > 0) {
            close(buf->fd);
        } else {
            rc = PARAM_INVALID;
        }
    }

    if (SUCCEED(rc)) {
        rc = kIonHelper.ion_free_func(kIonFd, buf->handler);
        if (FAILED(rc)) {
            LOGE(mModule, "Failed to free ion mem, %d", rc);
        }
    }

    if (SUCCEED(rc)) {
        buf->ptr = NULL;
        buf->len = 0;
        buf->fd = -1;
        buf->handler = -1;
    }

    return rc;
}

void IonBufferMgrImpl::clear()
{
    while (mBuffers.begin() != mBuffers.end()) {
        auto iter = mBuffers.begin();
        Buffer *buf = &(*iter);
        if (NOTNULL(buf)) {
            release(buf);
        }
        mBuffers.erase(iter);
    }
}

};


#include "Logs.h"
#include "Atomic.h"
#include "FragementBuffer.h"
#include "ObjectBuffer.h"

namespace voyager {

template<typename T>
T *FragementBuffer::get(const T &obj, BlockType block)
{
    int32_t rc = NO_ERROR;
    T *result = nullptr;
    auto found = mMap.find(typeid(obj).hash_code());

    if (SUCCEED(rc)) {
        if (found == mMap.end()) {
            ObjectBuffer<T> *buf = new ObjectBuffer<T>(mEachCapacity);
            if (NOTNULL(buf)) {
                RWLock::AutoWLock l(mMapLock);
                mMap[typeid(obj).hash_code()] = buf;
                found = mMap.find(typeid(obj).hash_code());
            } else {
                LOGE(mModule, "Failed to create ObjectBuffer<%s>",
                    typeid(obj).name());
            }
        }
    }

    if (SUCCEED(rc)) {
        if (found != mMap.end()) {
            ObjectBuffer<T> *buf = static_cast<ObjectBuffer<T> *>(found->second);
            result = buf->get(block);
            if (ISNULL(result)) {
                LOGE(mModule, "Failed to get fragement for %s",
                    typeid(*buf).name());
            }
        }
    }

    return result;
}

template<typename T>
T *FragementBuffer::get(const T &obj, BlockType block, const char *file, const char *func, const int32_t line)
{
    int32_t rc = NO_ERROR;
    T *result = nullptr;
    auto found = mMap.find(typeid(obj).hash_code());

    if (SUCCEED(rc)) {
        if (found == mMap.end()) {
            ObjectBuffer<T> *buf = new ObjectBuffer<T>(mEachCapacity);
            if (NOTNULL(buf)) {
                RWLock::AutoWLock l(mMapLock);
                mMap[typeid(obj).hash_code()] = buf;
                found = mMap.find(typeid(obj).hash_code());
            } else {
                LOGE(mModule, "Failed to create ObjectBuffer<%s>",
                    typeid(obj).name());
            }
        }
    }

    if (SUCCEED(rc)) {
        if (found != mMap.end()) {
            ObjectBuffer<T> *buf = static_cast<ObjectBuffer<T> *>(found->second);
            result = buf->get(block, file, func, line);
            if (ISNULL(result)) {
                LOGE(mModule, "Failed to get fragement for %s",
                    typeid(*buf).name());
            }
        }
    }

    return result;
}

template<typename T>
int32_t FragementBuffer::put(const T *obj)
{
    int32_t rc = NO_ERROR;
    auto found = mMap.find(typeid(*obj).hash_code());

    if (SUCCEED(rc)) {
        if (found == mMap.end()) {
            LOGE(mModule, "Object %s not belongs to here", typeid(*obj).name());
            rc = NOT_FOUND;
        }
    }

    if (SUCCEED(rc)) {
        ObjectBuffer<T> *buf = static_cast<ObjectBuffer<T> *>(found->second);
        rc = buf->put(obj);
        if (FAILED(rc)) {
            LOGE(mModule, "Failed to put fragement to %s",
                typeid(*buf).name());
        }
    }

    return rc;
}

template<typename T>
int32_t FragementBuffer::put(const T *obj, const char *file, const char *func, const int32_t line)
{
    int32_t rc = NO_ERROR;
    auto found = mMap.find(typeid(obj).hash_code());

    if (SUCCEED(rc)) {
        if (found == mMap.end()) {
            LOGE(mModule, "Object %s not belongs to here", typeid(obj).name());
            rc = NOT_FOUND;
        }
    }

    if (SUCCEED(rc)) {
        ObjectBuffer<T> *buf = static_cast<ObjectBuffer<T> *>(found->second);
        rc = buf->put(obj, file, func, line);
        if (FAILED(rc)) {
            LOGE(mModule, "Failed to put fragement to %s",
                typeid(*buf).name());
        }
    }

    return rc;

}

template<typename T>
bool FragementBuffer::full(const T &obj)
{
    bool result = true;
    RWLock::AutoRLock l(mMapLock);

    auto iter = mMap.find(typeid(obj).hash_code());
    if (iter != mMap.end()) {
        result = iter->second->full();
    }

    return result;
}

template<typename T>
int32_t FragementBuffer::flush(const T &obj)
{
    int32_t rc = NOT_FOUND;
    RWLock::AutoRLock l(mMapLock);

    auto iter = mMap.find(typeid(obj).hash_code());
    if (iter != mMap.end()) {
        rc = iter->second->flush();
    }

    return rc;
}

template<typename T>
int32_t FragementBuffer::dump(const T &obj)
{
    int32_t rc = NOT_FOUND;
    RWLock::AutoRLock l(mMapLock);

    auto iter = mMap.find(typeid(obj).hash_code());
    if (iter != mMap.end()) {
        rc = iter->second->dump();
    }

    return rc;
}


};


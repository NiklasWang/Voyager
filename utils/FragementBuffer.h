#ifndef __FRAGEMENT_BUFFER_H_
#define __FRAGEMENT_BUFFER_H_

#include <map>

#include "Common.h"
#include "RWLock.h"
#include "Semaphore.h"
#include "ObjectBufferHandler.h"

//#define TRACKER_ALL_OBJECT

namespace voyager {

class FragementBuffer :
    virtual public noncopyable {

public:
    template<typename T>
    T *get(const T &obj, BlockType block = BLOCK_TYPE);

    template<typename T>
    T *get(const T &obj, BlockType block, const char *file, const char *func, const int32_t line);

    template<typename T>
    int32_t put(const T *obj);

    template<typename T>
    int32_t put(const T *obj, const char *file, const char *func, const int32_t line);

    template<typename T>
    bool full(const T &obj);

    template<typename T>
    int32_t flush(const T &obj);
    int32_t flush();

    template<typename T>
    int32_t dump(const T &obj);
    int32_t dump();
    int32_t dump(const char *file, const char *func, const int32_t line);

public:
    explicit FragementBuffer(uint32_t eachCapacity = 0);
    ~FragementBuffer();

private:
    ModuleType mModule;
    uint32_t   mEachCapacity;
    RWLock     mMapLock;
    std::map<std::size_t, ObjectBufferHandler *> mMap;
};

};

#ifdef TRACKER_ALL_OBJECT

#define GET_FRAGEMENT_FROM_BUFFERP(buffer, T)    \
    (buffer)->get(T, BLOCK_TYPE, __FILE__, __FUNCTION__, __LINE__)

#define PUT_FRAGEMENT_TO_BUFFERP(buffer, obj) \
    (buffer)->put((obj), __FILE__, __FUNCTION__, __LINE__)

#define GET_FRAGEMENT_FROM_BUFFER(buffer, T)    \
    (buffer).get(T, BLOCK_TYPE, __FILE__, __FUNCTION__, __LINE__)

#define PUT_FRAGEMENT_TO_BUFFER(buffer, obj) \
    (buffer).put((obj), __FILE__, __FUNCTION__, __LINE__)

#else

#define GET_FRAGEMENT_FROM_BUFFERP(buffer, T)    \
    (buffer)->get(T)

#define PUT_FRAGEMENT_TO_BUFFERP(buffer, obj) \
    (buffer)->put(obj)

#define GET_FRAGEMENT_FROM_BUFFER(buffer, T)    \
    (buffer).get(T)

#define PUT_FRAGEMENT_TO_BUFFER(buffer, obj) \
    (buffer).put(obj)

#endif


#include "FragementBuffer.hpp"

#endif

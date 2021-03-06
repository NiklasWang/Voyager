#ifndef __OBJECT_BUFFER_H_
#define __OBJECT_BUFFER_H_

#include <list>

#include "Semaphore.h"
#include "RefBase.h"
#include "QueueC.h"
#include "RWLock.h"
#include "ObjectBufferHandler.h"

//#define TRACKER_ALL_OBJECT

namespace voyager {

template<typename T>
class ObjectBuffer :
    public ObjectBufferHandler,
    virtual public noncopyable {

public:
    T *get(BlockType block = BLOCK_TYPE);
    T *get(BlockType block, const char *file, const char *func, const int32_t line);

    int32_t put(const T *obj);
    int32_t put(const T *obj, const char *file, const char *func, const int32_t line);

    int32_t dump() override;
    int32_t dump(const char *file, const char *func, const int32_t line) override;

    int32_t flush() override;

    bool full() override;

public:
    explicit ObjectBuffer(uint32_t capacity = 0);
    ~ObjectBuffer();

private:
    struct ObjectInfo :
        public RefBase {
        T *obj;
        const char *file;
        const char *func;
        const int32_t line;
        ObjectInfo(T *aobj, const char *afile = NULL,
            const char *afunc = NULL, const int32_t aline = 0);
    };

private:
    uint32_t mCapacity;
    int32_t  mCnt;
    QueueC   mFree;
    QueueC   mSems;
    int32_t  mFreeCnt;
    ModuleType mModule;
    RWLock   mTakenInfLock;
    std::list<sp<ObjectInfo> > mTakenInf;
};

};

#ifdef TRACKER_ALL_OBJECT

#define GET_OBJ_FROM_BUFFERP(buffer)    \
    (buffer)->get(BLOCK_TYPE, __FILE__, __FUNCTION__, __LINE__)

#define PUT_OBJ_TO_BUFFERP(buffer, obj) \
    (buffer)->put((obj), __FILE__, __FUNCTION__, __LINE__)

#define GET_OBJ_FROM_BUFFER(buffer)    \
    (buffer).get(BLOCK_TYPE, __FILE__, __FUNCTION__, __LINE__)

#define PUT_OBJ_TO_BUFFER(buffer, obj) \
    (buffer).put((obj), __FILE__, __FUNCTION__, __LINE__)

#else

#define GET_OBJ_FROM_BUFFERP(buffer)    \
    (buffer)->get()

#define PUT_OBJ_TO_BUFFERP(buffer, obj) \
    (buffer)->put(obj)

#define GET_OBJ_FROM_BUFFER(buffer)    \
    (buffer).get()

#define PUT_OBJ_TO_BUFFER(buffer, obj) \
    (buffer).put(obj)

#endif


#include "ObjectBuffer.hpp"

#endif

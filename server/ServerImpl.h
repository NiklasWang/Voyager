#ifndef _VOYAGER_SERVER_IMPL_H_
#define _VOYAGER_SERVER_IMPL_H_

#include "common.h"
#include "ServerImpl.h"
#include "SyncType.h"

namespace voyager {

class ServerCore;
class ThreadPoolEx;

class ServerImpl :
    public ServerIntf,
    public Identifier,
    public noncopyable  {
public:

    virtual int32_t request(DataCbFunc dataCbFunc, SyncMode mode) override;
    virtual int32_t enqueue(void *dat) override;
    virtual int32_t request(FdCbFunc fdCbFunc, SyncMode mode) override;
    virtual int32_t enqueue(int32_t fd) override;
    virtual int32_t request(FrameCbFunc frameCbFunc, SyncMode mode) override;
    virtual int32_t enqueue(void *dat, int32_t format) override;
    virtual int32_t request(EventCbFunc eventCbFunc, SyncMode mode) override;
    virtual int32_t cancel(RequestType type) override;

private:
    int32_t coreRequest(void *request);
    int32_t coreAbort(void *enqueue);

public:
    ServerImpl();
    virtual ~ServerImpl();
    int32_t construct(const std::string &name, bool enableOverallControl);
    int32_t destruct();

private:
    enum TaskType {
        TYPE_REQUEST,
        TYPE_ENQUEUE,
        TYPE_MAX_INVALID,
    };

    struct RequestInfo {
        RequestType type;
        SyncMode sync;
        union Cb {
            DataCbFunc  dataCb;
            FdCbFunc    fdCb;
            FrameCbFunc frameCb;
            EventCbFunc eventCb;
        } cb;
    };

    struct EnqueueInfo {
        RequestType type;
        union Object {
            struct Data {
                void *ptr;
            } data;
            struct Fd {
                int32_t fd;
            } fd;
            struct Frame {
                void   *ptr;
                int32_t format;
            } frame;
        } object;
    };

public:
    struct TaskBase {
        uint32_t id;
        TaskType type;
        SyncType sync;
        int32_t  final_rc;
        ModuleType module;
        static const char * const kTaskString[];
     public:
        const char *whoamI();
        TaskType getType() { return type; }
        SyncType *getSync() { return &sync; }
        ModuleType getModule() { return module; }
        void setFinalRc(int32_t rc) { final_rc = rc; }
        int32_t getFinalRc() { return final_rc; }
        void setid(uint32_t _id) { id = _id; }
        uint32_t getid() { return id; }
        virtual void *getTask() { return NULL; }
        explicit TaskBase(
            TaskType  _type = TYPE_MAX_INVALID,
            sync_type _sync = SYNC_TYPE) :
            type(_type), sync(_sync), module(MODULE_SERVER_IMPL) {}
        virtual ~TaskBase() {}
    };

    template <class T>
    struct Task :
        public TaskBase {
        T arg;
        explicit Task(
            TaskType  _type = TYPE_MAX_INVALID,
            sync_type _sync = SYNC_TYPE) :
            TaskBase(_type, _sync) {}
        ~Task() = default;
        void *getTask() override { return &arg; };
        void setTask(T *_arg) { arg = *_arg; };
    };

private:
    typedef int32_t (ServerImpl::*PushToThreadFunc)(TaskType type, void *arg);

    template <typename T, sync_type sync = SYNC_TYPE>
    int32_t pushToThread(TaskType type, void *value);

    int32_t processTask(TaskBase *task);
    int32_t taskDone(TaskBase *task, int32_t processRC);

private:
    bool          mConstructed;
    uint32_t      mTaskCnt;
    ThreadPoolEx *mThreads;
    ServerCore   *mCore;
    static const PushToThreadFunc gAddThreadTaskFunc[];
};

};

#endif

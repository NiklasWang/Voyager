#ifndef _SERVER_CALLBACK_THREAD_H_
#define _SERVER_CALLBACK_THREAD_H_

#include "common.h"
#include "ServerIntf.h"

namespace voyager {

class CallbackIntf {
    virtual int32_t send(void *dat, int64_t len) = 0;
    virtual int32_t send(int32_t fd, int64_t len) = 0;
    virtual int32_t send(void *dat, int64_t len, int32_t format) = 0;
    virtual int32_t send(int32_t event, int32_t arg1, int32_t arg2) = 0;
};

class ThreadPoolEx;

class CallbackThread :
    public CallbackIntf,
    public Identifier,
    public noncopyable {
public:

    int32_t send(void *dat, int64_t len) override;
    int32_t send(int32_t fd, int64_t len) override;
    int32_t send(void *dat, int64_t len, int32_t format) override;
    int32_t send(int32_t event, int32_t arg1, int32_t arg2) override;
    int32_t set(DataCbFunc dataCb, SyncMode mode);
    int32_t set(FdCbFunc fdCb, SyncMode mode);
    int32_t set(FrameCbFunc frameCb, SyncMode mode);
    int32_t set(EventCbFunc eventCb, SyncMode mode);

public:
    CallbackThread();
    virtual ~CallbackThread();

public:
    int32_t construct();
    int32_t destruct();

private:
    bool          mConstructed;
    DataCbFunc    mDataCbFunc;
    SyncMode      mDataCbSyncMode;
    FdCbFunc      mFdCbFunc;
    SyncMode      mFdCbSyncMode;
    FrameCbFunc   mFrameCbFunc;
    SyncMode      mFrameCbSyncMode;
    EventCbFunc   mEventCbFunc;
    SyncMode      mEventCbSyncMode;
    ThreadPoolEx *mThreads;
    uint32_t      mDataCnt;
    uint32_t      mFdCnt;
    uint32_t      mFrameCnt;
    uint32_t      mEvtCnt;
};

};

#endif

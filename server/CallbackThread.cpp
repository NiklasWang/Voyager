#include "CallbackThread.h"
#include "ThreadPoolEx.h"

namespace voyager {

int32_t CallbackThread::send(void *dat, int64_t len)
{
    int32_t rc = NO_ERROR;

    if (SUCCEED(rc)) {
        auto func =
            [=]() -> int32_t {
                int32_t _rc = NO_ERROR;
                if (NOTNULL(mDataCbFunc)) {
                    mDataCnt++;
                    _rc = mDataCbFunc(dat, len);
                } else {
                    LOGE(mModule, "Data callback func not set, can't send, drop.");
                    _rc = NOT_INITED;
                }
                return _rc;
            };
        rc = (mDataCbSyncMode == SYNC)
            ? mThreads->runWait(func)
            : mThreads->run(func);
        if (FAILED(rc)) {
            LOGE(mModule, "Failed to send data, %d", rc);
        }
    }

    return rc;
}

int32_t CallbackThread::send(int32_t fd, int64_t len)
{
    int32_t rc = NO_ERROR;

    if (SUCCEED(rc)) {
        auto func =
            [=]() -> int32_t {
                int32_t _rc = NO_ERROR;
                if (NOTNULL(mFdCbFunc)) {
                    mFdCnt++;
                    _rc = mFdCbFunc(fd, len);
                } else {
                    LOGE(mModule, "Fd callback func not set, can't send, drop.");
                    _rc = NOT_INITED;
                }
                return _rc;
            };
        rc = (mFdCbSyncMode == SYNC)
            ? mThreads->runWait(func)
            : mThreads->run(func);
        if (FAILED(rc)) {
            LOGE(mModule, "Failed to send fd, %d", rc);
        }
    }

    return rc;
}

int32_t CallbackThread::send(void *dat, int64_t len, int32_t format)
{
    int32_t rc = NO_ERROR;

    if (SUCCEED(rc)) {
        auto func =
            [=]() -> int32_t {
                int32_t _rc = NO_ERROR;
                if (NOTNULL(mFrameCbFunc)) {
                    mFrameCnt++;
                    _rc = mFrameCbFunc(dat, len, format);
                } else {
                    LOGE(mModule, "Frame callback func not set, can't send, drop.");
                    _rc = NOT_INITED;
                }
                return _rc;
            };
        rc = (mFrameCbSyncMode == SYNC)
            ? mThreads->runWait(func)
            : mThreads->run(func);
        if (FAILED(rc)) {
            LOGE(mModule, "Failed to send frame, %d", rc);
        }
    }

    return rc;
}

int32_t CallbackThread::send(int32_t event, int32_t arg1, int32_t arg2)
{
    int32_t rc = NO_ERROR;

    if (SUCCEED(rc)) {
        auto func =
            [=]() -> int32_t {
                int32_t _rc = NO_ERROR;
                if (NOTNULL(mEventCbFunc)) {
                    mEvtCnt++;
                    _rc = mEventCbFunc(event, arg1, arg2);
                } else {
                    LOGE(mModule, "Event callback func not set, can't send.");
                    _rc = NOT_INITED;
                }
                return _rc;
            };
        rc = (mEventCbSyncMode == SYNC)
            ? mThreads->runWait(func)
            : mThreads->run(func);
        if (FAILED(rc)) {
            LOGE(mModule, "Failed to send event, %d", rc);
        }
    }

    return rc;
}

int32_t CallbackThread::set(DataCbFunc dataCb, SyncMode mode)
{
    mDataCbFunc = dataCb;
    mDataCbSyncMode= mode;

    return NO_ERROR;
}

int32_t CallbackThread::set(FdCbFunc fdCb, SyncMode mode)
{
    mFdCbFunc = fdCb;
    mFdCbSyncMode = mode;

    return NO_ERROR;
}

int32_t CallbackThread::set(FrameCbFunc frameCb, SyncMode mode)
{
    mFrameCbFunc = frameCb;
    mFrameCbSyncMode = mode;

    return NO_ERROR;
}

int32_t CallbackThread::set(EventCbFunc eventCb, SyncMode mode)
{
    mEventCbFunc = eventCb;
    mEventCbSyncMode = mode;

    return NO_ERROR;
}

CallbackThread::CallbackThread() :
    mConstructed(false),
    mDataCbFunc(nullptr),
    mDataCbSyncMode(ASYNC),
    mFdCbFunc(nullptr),
    mFdCbSyncMode(ASYNC),
    mFrameCbFunc(nullptr),
    mFrameCbSyncMode(ASYNC),
    mEventCbFunc(nullptr),
    mEventCbSyncMode(ASYNC),
    mThreads(nullptr),
    mDataCnt(0),
    mFdCnt(0),
    mFrameCnt(0),
    mEvtCnt(0)
{
}

CallbackThread::~CallbackThread()
{
    if (mConstructed) {
        destruct();
    }
}

int32_t CallbackThread::construct()
{
    int32_t rc = NO_ERROR;

    if (mConstructed) {
        rc = ALREADY_INITED;
    }

    if (SUCCEED(rc)) {
        mThreads = ThreadPoolEx::getInstance();
        if (ISNULL(mThreads)) {
            LOGE(mModule, "Failed to get thread pool");
            rc = NOT_READY;
        }
    }

    if (SUCCEED(rc)) {
        mConstructed = true;
    }

    return rc;
}

int32_t CallbackThread::destruct()
{
    int32_t rc = NO_ERROR;

    if (!mConstructed) {
        rc = NOT_INITED;
    } else {
        mConstructed = false;
    }

    if (SUCCEED(rc)) {
        if (NOTNULL(mThreads)) {
            mThreads->removeInstance();
            mThreads = nullptr;
        }
    }

    if (FAILED(rc)) {
        LOGE(mModule, "Cb thread destructed with error %d", rc);
    } else {
        LOGD(mModule, "Cb thread destructed");
    }

    return rc;
}

};


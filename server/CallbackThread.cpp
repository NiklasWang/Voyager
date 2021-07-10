#include "CallbackThread.h"
#include "ThreadPoolEx.h"

namespace voyager {

int32_t CallbackThread::send(void *dat, int64_t len)
{
    int32_t rc = NO_ERROR;

    if (SUCCEED(rc)) {
        rc = mThreads->run(
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
            }
        );
        if (FAILED(rc)) {
            LOGE(mModule, "Failed to send request, %d", rc);
        }
    }

    return rc;
}

int32_t CallbackThread::send(int32_t fd, int64_t len)
{
    int32_t rc = NO_ERROR;

    if (SUCCEED(rc)) {
        rc = mThreads->run(
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
            }
        );
        if (FAILED(rc)) {
            LOGE(mModule, "Failed to send request, %d", rc);
        }
    }

    return rc;
}

int32_t CallbackThread::send(void *dat, int64_t len, int32_t format)
{
    int32_t rc = NO_ERROR;

    if (SUCCEED(rc)) {
        rc = mThreads->runWait(
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
            }
        );
        if (FAILED(rc)) {
            LOGE(mModule, "Failed to send data, %d", rc);
        }
    }

    return rc;
}

int32_t CallbackThread::send(int32_t event, int32_t arg1, int32_t arg2)
{
    int32_t rc = NO_ERROR;

    if (SUCCEED(rc)) {
        rc = mThreads->run(
            [=]() -> int32_t {
                int32_t _rc = NO_ERROR;
                if (NOTNULL(mEvtCbFunc)) {
                    mEvtCnt++;
                    _rc = mEvtCbFunc(event, arg1, arg2);
                } else {
                    LOGE(mModule, "Event callback func not set, can't send.");
                    _rc = NOT_INITED;
                }
                return _rc;
            }
        );
        if (FAILED(rc)) {
            LOGE(mModule, "Failed to send event, %d", rc);
        }
    }

    return rc;
}

int32_t CallbackThread::set(DataCbFunc dataCb)
{
    mDataCbFunc = dataCb;
    return NO_ERROR;
}

int32_t CallbackThread::set(FdCbFunc fdCb)
{
    mFdCbFunc = fdCb;
    return NO_ERROR;
}

int32_t CallbackThread::set(FrameCbFunc frameCb)
{
    mFrameCbFunc = frameCb;
    return NO_ERROR;
}

int32_t CallbackThread::set(EventCbFunc eventCb)
{
    mEventCbFunc = eventCb;
    return NO_ERROR;
}

CallbackThread::CallbackThread() :
    mConstructed(false),
    mDataCbFunc(nullptr),
    mFdCbFunc(nullptr),
    mFrameCbFunc(nullptr),
    mEventCbFunc(nullptr),
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


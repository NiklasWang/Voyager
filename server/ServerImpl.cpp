#include "ServerImpl.h"
#include "ServerCore.h"
#include "IntfImpl.h"
#include "ThreadPoolEx.h"

namespace voyager {

ServerImpl::ServerImpl() :
    Identifier(MODULE_SERVER_IMPL, "ServerImpl", "1.0.0"),
    mConstructed(false),
    mTaskCnt(0),
    mCore(NULL),
{
    LOGI(mModule, "%s %s initializing...", PROJNAME, VERSION);
}

ServerImpl::~ServerImpl()
{
    if (mConstructed) {
        destruct();
    }
}

int32_t ServerImpl::construct(const std::string &name, bool enableOverallControl)
{
    int32_t rc = NO_ERROR;

    if (mConstructed) {
        rc = ALREADY_INITED;
    }

    if (SUCCEED(rc)) {
        mCore = new ServerCore();
        if (ISNULL(mCore)) {
            LOGE(mModule, "Failed to create core.");
            rc = UNKNOWN_ERROR;
        }
    }

    if (SUCCEED(rc)) {
        rc = mCore->construct(name, enableOverallControl);
        if (!SUCCEED(rc)) {
            LOGE(mModule, "Failed to construct core.");
        }
    }

    if (SUCCEED(rc)) {
        mThreads = ThreadPoolEx::getInstance();
        if (ISNULL(mThreads)) {
            LOGE(mModule, "Failed to get thread pool.");
            rc = NOT_READY;
        }
    }

    if (SUCCEED(rc)) {
        mConstructed = true;
        LOGD(mModule, "Impl constructed.");
    }

    return RETURNIGNORE(rc, ALREADY_EXISTS);
}

int32_t ServerImpl::destruct()
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
            mThreads = NULL;
        }
    }

    if (SUCCEED(rc)) {
        rc = mCore->destruct();
        if (!SUCCEED(rc)) {
            LOGE(mModule, "Failed to destruct core.");
        } else {
            SECURE_DELETE(mCore);
        }
    }

    if (!SUCCEED(rc)) {
        mConstructed = true;
        LOGE(mModule, "Failed to destruct impl.");
    } else {
        LOGD(mModule, "Impl destructed");
    }

    return RETURNIGNORE(rc, NOT_INITED);

}

const char * const ServerImpl::TaskBase::kTaskString[] = {
    [ServerImpl::TYPE_REQUEST]     = "request",
    [ServerImpl::TYPE_ENQUEUE]     = "enqueue buf",
    [ServerImpl::TYPE_MAX_INVALID] = "max invalid",
};

const char *ServerImpl::TaskBase::whoamI()
{
    return kTaskString[
        (type < 0 || type > TYPE_MAX_INVALID) ?
        TYPE_MAX_INVALID : type];
}

int32_t ServerImpl::processTask(TaskBase *task)
{
    assert(!ISNULL(dat));
    uint32_t rc = NO_ERROR;
    TaskType type = NOTNULL(task) ? task->getType() : TYPE_MAX_INVALID;
    void *arg = NOTNULL(task) ? task->getTask() : NULL;

    switch (type) {
        case TYPE_REQUEST: {
            rc = coreRequest(arg);
        } break;
        case TYPE_ENQUEUE: {
            rc = coreEnqueue(arg);
        } break;
        case TYPE_MAX_INVALID:
        default: {
            LOGE(mModule, "Invalid task type %s", task->whoamI());
            rc = UNKNOWN_ERROR;
        } break;
    }

    task->setFinalRc(rc);

    return rc;
}

int32_t ServerImpl::taskDone(TaskBase *task, int32_t processRC)
{
    int32_t rc = NO_ERROR;
    SyncType *sync = NOTNULL(task) ? task->getSync() : NULL;

    if (!SUCCEED(processRC)) {
        LOGE(mModule, "Failed to process task %s with %d, trying to ignore",
            task->whoamI(), processRC);
    }

    if (NOTNULL(sync) && *sync == SYNC_TYPE) {
        sync->signal();
    } else {
        SECURE_DELETE(task);
    }

    return rc;
}

template <typename T, sync_type sync>
int32_t ServerImpl::pushToThread(TaskType type, void *value)
{
    int32_t rc = NO_ERROR;
    Task<T> *task = NULL;
    T *arg = static_cast<T *>(value);

    task = new Task<T>(type, sync);
    if (ISNULL(task)) {
        LOGE(mModule, "Failed to create new task");
        rc = NO_MEMORY;
    } else {
        task->setid(mTaskCnt++);
        task->setFinalRc(NO_ERROR);
        if (NOTNULL(arg)) {
            task->setTask(arg);
        }
    }

    if (SUCCEED(rc)) {
        rc = mThreads->run<TaskBase>(
            [this](TaskBase *_task) -> int32_t {
                int32_t _rc = processTask(_task);
                taskDone(_task, _rc);
                return _rc;
            },
            task,
            sync
        );
        if (!SUCCEED(rc)) {
            LOGE(mModule, "Failed to run task %d %s",
                task->getid(), task->whoamI());
        }
    }

    if (SUCCEED(rc)) {
        SyncType *syncc = task->getSync();
        if (NOTNULL(syncc) && *syncc == SYNC_TYPE) {
            syncc->wait();
            rc = task->getFinalRc();
            SECURE_DELETE(task);
        }
    }

    return rc;
}

const ServerImpl::PushToThreadFunc
    ServerImpl::gAddThreadTaskFunc[] = {
    [ServerImpl::TYPE_REQUEST] = &ServerImpl::pushToThread<RequestInfo, SYNC_TYPE>,
    [ServerImpl::TYPE_ENQUEUE] = &ServerImpl::pushToThread<EnqueueInfo, SYNC_TYPE>,
};

#define CONSTRUCT_IMPL() \
    do { \
        int32_t rc = NO_ERROR; \
        if (!mConstructed) { \
            rc = construct(); \
            if (!SUCCEED(rc)) { \
                LOGE(mModule, "Failed to construct  impl %d", rc); \
                return rc; \
            } \
        } \
    } while(0)

int32_t ServerImpl::request(DataCbFunc dataCbFunc)
{
    int32_t rc = CONSTRUCT_IMPL();

    RequestInfo request = {
        .type = DATA,
        .cb = {
            .dataCb = dataCbFunc,
        },
    }

    return SUCCEED(rc) ? (this->*(gAddThreadTaskFunc[TYPE_REQUEST]))(TYPE_REQUEST, &request) : NO_ERROR;
}

int32_t ServerImpl::request(FdCbFunc fdCbFunc)
{
    int32_t rc = CONSTRUCT_IMPL();

    RequestInfo request = {
        .type = FD,
        .cb = {
            .fdCb = fdCbFunc,
        },
    }

    return SUCCEED(rc) ? (this->*(gAddThreadTaskFunc[TYPE_REQUEST]))(TYPE_REQUEST, &request) : NO_ERROR;
}

int32_t ServerImpl::request(FrameCbFunc frameCbFunc)
{
    int32_t rc = CONSTRUCT_IMPL();
    RequestInfo request = {
        .type = FRAME,
        .cb = {
            .frameCb = frameCbFunc,
        },
    }

    return SUCCEED(rc) ? (this->*(gAddThreadTaskFunc[TYPE_REQUEST]))(TYPE_REQUEST, &request) : NO_ERROR;
}

int32_t ServerImpl::request(EventCbFunc eventCbFunc)
{
    int32_t rc = CONSTRUCT_IMPL();

    RequestInfo request = {
        .type = EVENT,
        .cb = {
            .eventCb = eventCbFunc,
        },
    }

    return SUCCEED(rc) ? (this->*(gAddThreadTaskFunc[TYPE_REQUEST]))(TYPE_REQUEST, &request) : NO_ERROR;
}

int32_t ServerImpl::cancel(RequestType type)
{
    int32_t rc = CONSTRUCT_IMPL();
    RequestInfo request;

    if (SUCCEED(rc)) {
        request.type = type;
        switch (type) {
            case DATA : {
                request.cb.dataCb = nullptr;
            }; break;
            case FD : {
                request.cb.fdCb = nullptr;
            }; break;
            case FRAME : {
                request.cb.frameCb = nullptr;
            }; break;
            case EVENT : {
                request.cb.eventCb = nullptr;
            }; break;
            case REQUEST_TYPE_MAX_INVALID : {
                request.cb.dataCb  = nullptr;
                request.cb.fdCb    = nullptr;
                request.cb.frameCb = nullptr;
                request.cb.eventCb = nullptr;
            }; break;
        }
    }

    if (SUCCEED(rc)) {
        rc = this->*(gAddThreadTaskFunc[TYPE_REQUEST]))(TYPE_REQUEST, &request);
        if (FAILED(rc)) {
            LOGE(mModule, "Failed to cancel %s on core, %d",
                getRequestName(type), rc);
        }
    }

    return rc;
}

int32_t ServerImpl::enqueue(void *_dat)
{
    int32_t rc = CONSTRUCT_IMPL();

    EnqueueInfo info = {
        .type = DATA,
        .object = {
            .data = {
                .ptr = _dat,
            },
        },
    }

    return SUCCEED(rc) ? (this->*(gAddThreadTaskFunc[TYPE_ENQUEUE]))(TYPE_ENQUEUE, &info) : NO_ERROR;
}

int32_t ServerImpl::enqueue(int32_t _fd)
{
    int32_t rc = CONSTRUCT_IMPL();

    EnqueueInfo info = {
        .type = FD,
        .object = {
            .fd = {
                .fd = _fd,
            },
        },
    }

    return SUCCEED(rc) ? (this->*(gAddThreadTaskFunc[TYPE_ENQUEUE]))(TYPE_ENQUEUE, &info) : NO_ERROR;
}

int32_t ServerImpl::enqueue(void *_dat, int32_t _format)
{
    int32_t rc = CONSTRUCT_IMPL();
    EnqueueInfo info = {
        .type = FRAME,
        .object = {
            .frame = {
                .ptr = _dat,
                .format = _format,
            },
        },
    }

    return SUCCEED(rc) ? (this->*(gAddThreadTaskFunc[TYPE_ENQUEUE]))(TYPE_ENQUEUE, &info) : NO_ERROR;
}


int32_t ServerImpl::coreRequest(void *_request)
{
    int32_t rc = NO_ERROR;
    RequestInfo *info = static_cast<RequestInfo *>(_request);

    if (SUCCEED(rc)) {
        switch (info.type) {
            case DATA : {
                rc = mCore->request(info.cb.dataCb);
            }; break;
            case FD : {
                rc = mCore->request(info.cb.fdCb);
            }; break;
            case FRAME : {
                rc = mCore->request(info.cb.frameCb);
            }; break;
            case EVENT : {
                rc = mCore->request(info.cb.eventCb);
            }; break;
            case REQUEST_TYPE_MAX_INVALID : {
                LOGE(mModule, "Invalid request %d", info.type);
                rc = PARAM_INVALID;
            }; break;
        }
        if (FAILED(rc)) {
            LOGE(mModule, "Failed to request on core, %d", rc);
        }
    }

    return rc;
}

int32_t ServerImpl::coreEnqueue(void *_info)
{
    int32_t rc = NO_ERROR;
    EnqueueInfo *info = static_cast<EnqueueInfo *>(_info);

    if (SUCCEED(rc)) {
        switch (info.type) {
            case DATA : {
                rc = mCore->enqueue(info.object.data.ptr);
            }; break;
            case FD : {
                rc = mCore->request(info.object.fd.fd);
            }; break;
            case FRAME : {
                rc = mCore->request(info.object.frame.ptr, info.object.frame.format);
            }; break;
            case EVENT : {
                // Not required
            }; break;
            case REQUEST_TYPE_MAX_INVALID : {
                LOGE(mModule, "Invalid request %d", info.type);
                rc = PARAM_INVALID;
            }; break;
        }
        if (FAILED(rc)) {
            LOGE(mModule, "Failed to enqueue on core, %d", rc);
        }
    }

    return rc;
}

};


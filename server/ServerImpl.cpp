#include "ServerImpl.h"
#include "Core.h"
#include "ThreadPoolEx.h"

namespace voyager {

ServerImpl::ServerImpl() :
    mConstructed(false),
    mModule(MODULE_VOYAGER_IMPL),
    mTaskCnt(0),
    mCore(NULL)
{
    LOGI(mModule, "%s %s initializing...", PROJNAME, VERSION);
}

ServerImpl::~ServerImpl()
{
    if (mConstructed) {
        destruct();
    }
}

int32_t ServerImpl::construct()
{
    int32_t rc = NO_ERROR;

    if (mConstructed) {
        rc = ALREADY_INITED;
    }

    if (SUCCEED(rc)) {
        mCore = new Core();
        if (ISNULL(mCore)) {
            LOGE(mModule, "Failed to create core");
            rc = UNKNOWN_ERROR;
        }
    }

    if (SUCCEED(rc)) {
        rc = mCore->construct();
        if (!SUCCEED(rc)) {
            LOGE(mModule, "Failed to construct core");
        }
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
        LOGD(mModule, " impl constructed");
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
            LOGE(mModule, "Failed to destruct core");
        } else {
            SECURE_DELETE(mCore);
        }
    }

    if (!SUCCEED(rc)) {
        mConstructed = true;
        LOGE(mModule, "Failed to destruct  impl");
    } else {
        LOGD(mModule, " impl destructed");
    }

    return RETURNIGNORE(rc, NOT_INITED);

}

const char * const ServerImpl::TaskBase::kTaskString[] = {
    [ServerImpl::TYPE_REQUEST]     = "request",
    [ServerImpl::TYPE_ABORT]       = "abort",
    [ServerImpl::TYPE_ENQUEUE]     = "enqueue buf",
    [ServerImpl::TYPE_SET_CB]      = "set callback",
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
        case TYPE_ABORT: {
            rc = coreAbort(arg);
        } break;
        case TYPE_ENQUEUE: {
            rc = coreEnqueue(arg);
        } break;
        case TYPE_SET_CB: {
            rc = coreSetCallback(arg);
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
    [ServerImpl::TYPE_REQUEST] = &ServerImpl::pushToThread<RequestType, SYNC_TYPE>,
    [ServerImpl::TYPE_ABORT]   = &ServerImpl::pushToThread<RequestType, SYNC_TYPE>,
    [ServerImpl::TYPE_ENQUEUE] = &ServerImpl::pushToThread<BufferInfo,  SYNC_TYPE>,
    [ServerImpl::TYPE_SET_CB]  = &ServerImpl::pushToThread<CbInfo,      SYNC_TYPE>,
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

int32_t ServerImpl::request(RequestType type)
{
    CONSTRUCT_IMPL();
    return (this->*(gAddThreadTaskFunc[TYPE_REQUEST]))(TYPE_REQUEST, &type);
}

int32_t ServerImpl::abort(RequestType type)
{
    CONSTRUCT_IMPL();
    return (this->*(gAddThreadTaskFunc[TYPE_ABORT]))(TYPE_ABORT, &type);
}

int32_t ServerImpl::enqueue(RequestType type, int32_t id)
{
    CONSTRUCT_IMPL();
    BufferInfo buf = {
        .type = type,
        .id   = id,
    };
    return (this->*(gAddThreadTaskFunc[TYPE_ENQUEUE]))(TYPE_ENQUEUE, &buf);
}

int32_t ServerImpl::setCallback(RequestCbFunc requestCb)
{
    CONSTRUCT_IMPL();
    CbInfo func = {
        .requestCb = requestCb,
        .eventCb   = NULL,
        .dataCb    = NULL,
    };
    return (this->*(gAddThreadTaskFunc[TYPE_SET_CB]))(TYPE_SET_CB, &func);
}

int32_t ServerImpl::setCallback(EventCbFunc eventCb)
{
    CONSTRUCT_IMPL();
    CbInfo func = {
        .requestCb = NULL,
        .eventCb   = eventCb,
        .dataCb    = NULL,
    };
    return (this->*(gAddThreadTaskFunc[TYPE_SET_CB]))(TYPE_SET_CB, &func);
}

int32_t ServerImpl::setCallback(DataCbFunc dataCb)
{
    CONSTRUCT_IMPL();
    CbInfo func = {
        .requestCb = NULL,
        .eventCb   = NULL,
        .dataCb    = dataCb,
    };
    return (this->*(gAddThreadTaskFunc[TYPE_SET_CB]))(TYPE_SET_CB, &func);
}

int32_t ServerImpl::coreRequest(void *_type)
{
    RequestType *type = static_cast<RequestType *>(_type);
    return ISNULL(mCore) ?
        NOT_INITED : mCore->request(*type);
}

int32_t ServerImpl::coreAbort(void *_type)
{
    RequestType *type = static_cast<RequestType *>(_type);
    return ISNULL(mCore) ?
        NOT_INITED : mCore->abort(*type);
}

int32_t ServerImpl::coreEnqueue(void *_info)
{
    BufferInfo *inf = static_cast<BufferInfo *>(_info);
    return ISNULL(mCore) ?
        NOT_INITED : mCore->enqueue(inf->type, inf->id);
}

int32_t ServerImpl::coreSetCallback(void *_func)
{
    int32_t rc = NO_ERROR;

    CbInfo *func = static_cast<CbInfo *>(_func);
    if (ISNULL(mCore)) {
        rc = NOT_INITED;
    }

    if (SUCCEED(rc)) {
        if (NOTNULL(func->requestCb)) {
            rc |= mCore->setCallback(func->requestCb);
        }
        if (NOTNULL(func->eventCb)) {
            rc |= mCore->setCallback(func->eventCb);
        }
        if (NOTNULL(func->dataCb)) {
            rc |= mCore->setCallback(func->dataCb);
        }
    }

    if (!SUCCEED(rc)) {
        LOGE(mModule, "Failed to set callback, %d", rc);
    }

    return rc;
}

};


#include "ServerCore.h"

namespace voyager {

int32_t ServerCore::construct()
{
    int32_t rc = NO_ERROR;
    int32_t size = 0;

    if (mConstructed) {
        rc = ALREADY_INITED;
    }

    if (SUCCEED(rc)) {
        rc = mBuffer.init();
        if (!SUCCEED(rc)) {
            LOGE(mModule, "Failed to init ion buf mgr, %d", rc);
        }
    }

    if (SUCCEED(rc)) {
        size = mCtl.getTotoalSize();
        if (size <= 0) {
            LOGE(mModule, "Invalid control blcok size %d", size);
            rc = PARAM_INVALID;
        }
    }

    if (SUCCEED(rc)) {
        rc = mBuffer.allocate(&mCtlMem, size, &mCtlFd);
        if (!SUCCEED(rc)) {
            LOGE(mModule, "Failed to alloc %dB ion buf", size);
        }
    }

    if (SUCCEED(rc)) {
        rc = mCtl.init(mCtlMem, size, true);
        if (!SUCCEED(rc)) {
            LOGE(mModule, "Failed to set memory to control mgr, %d", rc);
        }
    }

    if (SUCCEED(rc)) {
        rc = mSS.construct();
        if (!SUCCEED(rc)) {
            LOGE(mModule, "Failed to construct server state machine, %d", rc);
        }
    }

    if (SUCCEED(rc)) {
        rc = mCb.construct();
        if (!SUCCEED(rc)) {
            LOGE(mModule, "Failed to construct callback thread, %d", rc);
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
        rc = mThreads->run(
            [this]() -> int32_t {
                return startServerLoop();
            }
        );
        if (!SUCCEED(rc)) {
            LOGE(mModule, "Failed to run once thread, %d", rc);
        }
    }

    if (SUCCEED(rc)) {
        mConstructed = true;
        LOGD(mModule, " core constructed");
    }

    return rc;
}

int32_t ServerCore::startServerLoop()
{
    int32_t rc = NO_ERROR;

    if (SUCCEED(rc)) {
        rc = mSS.startServer();
        if (rc == USER_ABORTED) {
            LOGI(mModule, "Cancelled to wait connection, exit task.");
        } else if (!SUCCEED(rc)) {
            LOGE(mModule, "Failed to start socket server");
        }
    }

    if (SUCCEED(rc)) {
        rc = mSS.waitForConnect();
        if (!SUCCEED(rc)) {
            LOGE(mModule, "Failed to wait for client connection");
        }
    }

    if (SUCCEED(rc)) {
        if (!mSS.connected()) {
            LOGD(mModule, "Client not connected, exit.");
            rc = NOT_EXIST;
        }
    }

    if (SUCCEED(rc)) {
        mSocketMsg[0] = '\0';
        rc = mSS.receiveMsg(mSocketMsg, sizeof(mSocketMsg));
        if (!SUCCEED(rc)) {
            LOGE(mModule, "Failed to receive msg from socket sm, %d", rc);
        }
    }

    if (SUCCEED(rc)) {
        if (!COMPARE_SAME_STRING(mSocketMsg, SOCKET_CLIENT_GREETING_STR)) {
            mSocketMsg[sizeof(mSocketMsg) - 1] = '\0';
            LOGE(mModule, "Unknown msg received, \"%s\"", mSocketMsg);
            rc = NOT_READY;
        }
    }

    if (SUCCEED(rc)) {
        rc = mSS.sendFd(mCtlFd);
        if (!SUCCEED(rc)) {
            LOGE(mModule, "Failed to send fd %d to client, %d", mCtlFd, rc);
        }
    }

    if (SUCCEED(rc)) {
        mSocketMsg[0] = '\0';
        rc = mSS.receiveMsg(mSocketMsg, sizeof(mSocketMsg));
        if (!SUCCEED(rc)) {
            LOGE(mModule, "Failed to receive msg from socket sm, %d", rc);
        }
    }

    if (SUCCEED(rc)) {
        if (!COMPARE_SAME_STRING(mSocketMsg, SOCKET_CLIENT_REPLY_STR)) {
            mSocketMsg[sizeof(mSocketMsg) - 1] = '\0';
            LOGE(mModule, "Unknown msg received, \"%s\"", mSocketMsg);
            rc = NOT_READY;
        } else {
            mClientReady = true;
        }
    }

    if (SUCCEED(rc)) {
        rc = enableCachedRequests();
        if (!SUCCEED(rc)) {
            LOGE(mModule, "Failed to enable cached requests, %d", rc);
        }
    }

    if (SUCCEED(rc)) {
        rc = mSS.sendMsg(SOCKET_CLIENT_REPLY_STR,
            strlen(SOCKET_CLIENT_REPLY_STR));
        if (!SUCCEED(rc)) {
            LOGE(mModule, "Failed to send msg \"%s\" to server, %d",
                SOCKET_CLIENT_REPLY_STR, rc);
        }
    }

    if (SUCCEED(rc)) {
        do {
            int32_t clientfd = -1;
            RequestType type = REQUEST_TYPE_MAX_INVALID;
            RESETRESULT(rc);

            if (SUCCEED(rc)) {
                rc = mSS.waitForConnect(&clientfd);
                if (!SUCCEED(rc)) {
                    LOGE(mModule, "Failed to wait for client connection");
                }
                if (rc == USER_ABORTED) {
                    LOGI(mModule, "Stop wait connect, aborted.");
                    break;
                }
            }

            if (SUCCEED(rc)) {
                mSocketMsg[0] = '\0';
                rc = mSS.receiveMsg(clientfd, mSocketMsg, sizeof(mSocketMsg));
                if (!SUCCEED(rc)) {
                    LOGE(mModule, "Failed to receive msg, %d", rc);
                }
            }

            if (SUCCEED(rc)) {
                rc = convertToRequestType(mSocketMsg, &type);
                if (!SUCCEED(rc) || type == REQUEST_TYPE_MAX_INVALID) {
                    LOGE(mModule, "Invalid socket msg, %s", mSocketMsg);
                }
            }

            if (SUCCEED(rc)) {
                if (ISNULL(mRequests[type])) {
                    LOGE(mModule, "Request %d not created, should't be here.", type);
                    rc = BAD_PROTOCAL;
                }
            }

            if (SUCCEED(rc)) {
                bool requested = mCtl.requested(type);
                if (!requested) {
                    LOGE(mModule, "Request not requested, shouldn't be here");
                    rc = BAD_PROTOCAL;
                }
            }

            if (SUCCEED(rc)) {
                rc = mRequests[type]->setSocketFd(clientfd);
                if (!SUCCEED(rc) && rc != JUMP_DONE) {
                    LOGE(mModule, "Failed to set socket fd %d to %s",
                        clientfd, mRequests[type]->getName());
                }
            }

            if (SUCCEED(rc)) {
                rc = mRequests[type]->onClientReady();
                if (!SUCCEED(rc)) {
                    LOGE(mModule, "Failed to notify client connected to %s",
                        rc, mRequests[type]->getName());
                }
            }
        } while(rc != USER_ABORTED);
   }

    return rc;
}

int32_t ServerCore::convertToRequestType(
    char *msg, RequestType *type)
{
    int32_t rc = NO_ERROR;
    int32_t value = atoi(msg + strlen(SOCKET_CLIENT_CONNECT_TYPE) + 1);

    if (value < 0) {
        *type = REQUEST_TYPE_MAX_INVALID;
        LOGE(mModule, "Invalid msg, %s", msg);
        rc = PARAM_INVALID;
    } else {
        *type = ::voyager::convertToRequestType(value);
    }

    return rc;
}

int32_t ServerCore::enableCachedRequests()
{
    int32_t rc = NO_ERROR;

    for (int32_t i = 0; i < REQUEST_TYPE_MAX_INVALID; i++) {
        if (mCachedRequest[i]) {
            LOGD(mModule, "Enable cached request %d", i);
            rc = request(::voyager::convertToRequestType(i));
            if (!SUCCEED(rc)) {
                LOGE(mModule, "Failed to create cached request %d", rc);
            } else {
                mCachedRequest[i] = false;
            }
        }
    }

    return rc;
}

int32_t ServerCore::exitServerLoop()
{
    int32_t rc = mSS.cancelWaitConnect();
    if (!SUCCEED(rc)) {
        LOGE(mModule, "Failed to cancel wait client");
    }

    return rc;
}

int32_t ServerCore::destruct()
{
    int32_t rc = NO_ERROR;
    int32_t final = NO_ERROR;

    if (!mConstructed) {
        rc = NOT_INITED;
    } else {
        mConstructed = false;
    }

    if (SUCCEED(rc)) {
        for (int32_t i = 0; i < REQUEST_TYPE_MAX_INVALID; i++) {
            rc = mCtl.setRequest(
                ::voyager::convertToRequestType(i), DISABLE_REQUEST);
            if (!SUCCEED(rc)) {
                final |= rc;
                LOGE(mModule, "Failed to cancel request %d", i);
                RESETRESULT(rc);
            }
        }
    }

    if (SUCCEED(rc)) {
        rc = exitServerLoop();
        if (!SUCCEED(rc)) {
            final |= rc;
            LOGE(mModule, "Failed to abort run once thread");
            RESETRESULT(rc);
        }
    }

    if (SUCCEED(rc)) {
        for (int32_t i = 0; i < REQUEST_TYPE_MAX_INVALID; i++) {
            if (NOTNULL(mRequests[i])) {
                rc = mRequests[i]->abort();
                if (!SUCCEED(rc)) {
                    final |= rc;
                    LOGE(mModule, "Failed to abort request handler %s",
                        mRequests[i]->getName());
                    RESETRESULT(rc);
                }
                rc = mRequests[i]->destruct();
                if (!SUCCEED(rc)) {
                    final |= rc;
                    LOGE(mModule, "Failed to destruct request handler %s",
                        mRequests[i]->getName());
                    RESETRESULT(rc);
                }
                SECURE_DELETE(mRequests[i]);
            }
        }
    }

    if (SUCCEED(rc)) {
        rc = mSS.destruct();
        if (!SUCCEED(rc)) {
            final |= rc;
            LOGE(mModule, "Failed to destruct socket state machine, %d", rc);
            RESETRESULT(rc);
        }
    }

    if (SUCCEED(rc)) {
        rc = mCb.destruct();
        if (!SUCCEED(rc)) {
            final |= rc;
            LOGE(mModule, "Failed to destruct callback thread, %d", rc);
            RESETRESULT(rc);
        }
    }

    if (SUCCEED(rc)) {
        if (NOTNULL(mThreads)) {
            mThreads->removeInstance();
            mThreads = NULL;
        }
    }

    if (SUCCEED(rc)) {
        if (NOTNULL(mCtlMem)) {
            rc = mBuffer.release(mCtlMem);
            if (!SUCCEED(rc)) {
                final |= rc;
                LOGE(mModule, "Failed to release ion buf, %d", rc);
                RESETRESULT(rc);
            }
        }
    }

    if (SUCCEED(rc)) {
        if (NOTNULL(mCtlMem)) {
            // control memory freed by munmap() call
            mCtlMem = NULL;
        }
    }

    if (SUCCEED(rc)) {
        mBuffer.clear_all();
        rc = mBuffer.deinit();
        if (!SUCCEED(rc)) {
            final |= rc;
            LOGE(mModule, "Failed to deinit ion buf mgr, %d", rc);
            RESETRESULT(rc);
        }
    }

    if (!SUCCEED(final)) {
        LOGE(mModule, " core destructed with error %d", final);
    } else {
        LOGD(mModule, " core destructed");
    }

    return rc;
}

int32_t ServerCore::createRequestHandler(RequestType type)
{
    int32_t rc = NO_ERROR;
    RequestHandler *requestHandler = NULL;

    if (SUCCEED(rc)) {
        if (NOTNULL(mRequests[type])) {
            LOGI(mModule, "%s alreay requested",
                mRequests[type]->getName());
            rc = ALREADY_EXISTS;
        }
    }

    if (SUCCEED(rc)) {
        requestHandler = createHandler(type);
        if (ISNULL(requestHandler)) {
            LOGE(mModule, "Failed to create request handler");
            rc = NO_MEMORY;
        }
    }

    if (SUCCEED(rc)) {
        rc = requestHandler->construct();
        if (!SUCCEED(rc)) {
            LOGE(mModule, "Failed to const request handler, %d", rc);
        } else {
            mRequests[type] = requestHandler;
        }
    }

    if (SUCCEED(rc)) {
        rc = setRequestedMark(type, ENABLE_REQUEST);
        if (!SUCCEED(rc)) {
            LOGE(mModule, "Failed to set enable request %d "
                "for client, %d", type, rc);
        }
    }

    if (!SUCCEED(rc)) {
        if (ISNULL(requestHandler)) {
            mRequests[type] = NULL;
            SECURE_DELETE(requestHandler);
        }
    }

    return rc;
}

int32_t ServerCore::request(RequestType type)
{
    int32_t rc = NO_ERROR;

    if (SUCCEED(rc)) {
        type = getRequestType(type);
        if (type == REQUEST_TYPE_MAX_INVALID) {
            LOGE(mModule, "Invalid request type %d", type);
            rc = PARAM_INVALID;
        }
    }

    if (SUCCEED(rc)) {
        if (!mClientReady) {
            mCachedRequest[type] = true;
            LOGD(mModule, "Client not ready, request %d cached.", type);
            rc = JUMP_DONE;
        }
    }

    if (SUCCEED(rc)) {
        rc = createRequestHandler(type);
        if (!SUCCEED(rc)) {
            LOGE(mModule, "Failed to create request %d", type);
        }
    }

    if (SUCCEED(rc)) {
        rc = mCtl.setRequest(type, ENABLE_REQUEST);
        if (!SUCCEED(rc)) {
            LOGE(mModule, "Failed to enable request %d", rc);
        }
    }

    return RETURNIGNORE(rc, JUMP_DONE);
}

int32_t ServerCore::abort(RequestType type)
{
    int32_t rc = NO_ERROR;
    RequestHandlerIntf *requestHandler = NULL;

    if (SUCCEED(rc)) {
        if (!requested(type)) {
            LOGI(mModule, "%d not requested", type);
            rc = NOT_INITED;
        }
    }

    if (SUCCEED(rc)) {
        rc = setRequestedMark(type, DISABLE_REQUEST);
        if (!SUCCEED(rc)) {
            LOGE(mModule, "Failed to set disable request %d "
                "for client, %d", type, rc);
        }
    }

    if (SUCCEED(rc)) {
        rc = mCtl.resetCtrlMem(type);
        if (!SUCCEED(rc)) {
            LOGE(mModule, "Failed to reset memory type %d, %d",
                type, rc);
        }
    }

    if (SUCCEED(rc)) {
        requestHandler = mRequests[type];
        rc = requestHandler->abort();
        if (!SUCCEED(rc)) {
            LOGE(mModule, "Failed to exit request handler %s, %d",
                requestHandler->getName(), rc);
        }
    }

    if (SUCCEED(rc)) {
        rc = requestHandler->destruct();
        if (!SUCCEED(rc)) {
            LOGE(mModule, "Failed to destruct request handler %s, %d",
                requestHandler->getName(), rc);
        }
    }

    if (SUCCEED(rc)) {
        SECURE_DELETE(requestHandler);
        mRequests[type] = NULL;
    }

    return RETURNIGNORE(rc, NOT_INITED);
}

bool ServerCore::requested(RequestType type)
{
    int32_t rc = NO_ERROR;
    bool result = false;

    if (SUCCEED(rc)) {
        type = getRequestType(type);
        if (type == REQUEST_TYPE_MAX_INVALID) {
            LOGE(mModule, "Invalid request type %d", type);
            rc = PARAM_INVALID;
        } else {
            result = mCtl.requested(type);
        }
    }

    return result;
}

int32_t ServerCore::enqueue(RequestType type, int32_t id)
{
    int32_t rc = NO_ERROR;

    if (SUCCEED(rc)) {
        type = getRequestType(type);
        if (type == REQUEST_TYPE_MAX_INVALID) {
            LOGE(mModule, "Invalid request type %d", type);
            rc = PARAM_INVALID;
        }
    }

    if (SUCCEED(rc)) {
        if (ISNULL(mRequests[type])) {
            LOGW(mModule, "Request not created, should't be here.");
            rc = NOT_REQUIRED;
        }
    }

    if (SUCCEED(rc)) {
        bool requested = mCtl.requested(type);
        if (!requested) {
            LOGE(mModule, "Request not requested, shouldn't be here");
            rc = NOT_REQUIRED;
        }
    }

    if (SUCCEED(rc)) {
        rc = mRequests[type]->enqueue(id);
        if (!SUCCEED(rc)) {
            LOGE(mModule, "Failed to enqueue %d to %s", id,
                mRequests[type]->getName());
        }
    }

    return RETURNIGNORE(rc, NOT_REQUIRED);
}

int32_t ServerCore::setCallback(RequestCbFunc requestCb)
{
    return mCb.setCallback(requestCb);
}

int32_t ServerCore::setCallback(EventCbFunc eventCb)
{
    return mCb.setCallback(eventCb);
}

int32_t ServerCore::setCallback(DataCbFunc dataCb)
{
    return mCb.setCallback(dataCb);
}

int32_t ServerCore::send(RequestType type, int32_t id, void *head, void *dat)
{
    return mCb.send(type, id, head, dat);
}

int32_t ServerCore::send(int32_t event, int32_t arg1, int32_t arg2)
{
    return mCb.send(event, arg1, arg2);
}

int32_t ServerCore::send(int32_t type, void *data, int32_t size)
{
    return mCb.send(type, data, size);
}

int32_t ServerCore::allocateBuf(void **buf, int32_t len, int32_t *fd)
{
    return mBuffer.allocate(buf, len, fd);
}

int32_t ServerCore::releaseBuf(void *buf)
{
    return mBuffer.release(buf);
}

int32_t ServerCore::setMemStatus(RequestType type, int32_t fd, bool fresh)
{
    return mCtl.setMemStatus(type, fd, fresh);
}

int32_t ServerCore::getMemStatus(RequestType type, int32_t fd, bool *fresh)
{
    return mCtl.getMemStatus(type, fd, fresh);
}

int32_t ServerCore::setMemSize(RequestType type, int32_t size)
{
    return mCtl.setMemSize(type, size);
}

int32_t ServerCore::getMemSize(RequestType type, int32_t *size)
{
    return mCtl.getMemSize(type, size);
}

int32_t ServerCore::addMemory(RequestType type, int32_t clientfd, bool fresh)
{
    return mCtl.addMemory(type, clientfd, fresh);
}

int32_t ServerCore::setRequestedMark(RequestType type, bool enable)
{
    return mCtl.setRequest(type, enable);
}

int32_t ServerCore::getHeader(Header &header)
{
    return mCtl.getHeader(header);
}

ServerCore::ServerCore() :
    mConstructed(false),
    mModule(MODULE_VOYAGER_CORE),
    mClientReady(false),
    mCtlFd(-1),
    mCtlMem(NULL)
{
    for (int32_t i = 0; i < REQUEST_TYPE_MAX_INVALID; i++) {
        mRequests[i] = NULL;
        mCachedRequest[i] = false;
    }
}

ServerCore::~ServerCore()
{
    if (mConstructed) {
        destruct();
    }
}

};

#include "PreviewServer.h"
#include "YuvPictureServer.h"
#include "BayerPictureServer.h"
#include "EventServer.h"
#include "DataServer.h"

namespace voyager {

RequestHandler *ServerCore::createHandler(RequestType type)
{
    RequestHandler *request = NULL;

    switch (type) {
        case PREVIEW_NV21: {
            request = new PreviewServer(this);
        } break;
        case PICTURE_NV21: {
            request = new YuvPictureServer(this);
        } break;
        case PICTURE_BAYER: {
            request = new BayerPictureServer(this);
        } break;
        case EXTENDED_EVENT: {
            request = new EventServer(this);
        } break;
        case CUSTOM_DATA: {
            request = new DataServer(this);
        } break;
        default: {
            LOGE(mModule, "Invalid request type %d", type);
        } break;
    };

    return request;
}

};


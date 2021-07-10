#include "ServerCore.h"
#include "protocol.h"

namespace voyager {

int32_t ServerCore::construct(const std::string &name, bool enableOverallControl)
{
    int32_t rc = NO_ERROR;
    int32_t size = 0;
    mName = name;
    mEnableOverallControl = enableOverallControl;

    if (mConstructed) {
        rc = ALREADY_INITED;
    }

    if (SUCCEED(rc)) {
        if (mEnableOverallControl) {
            mOverallControl = OverallControlSingleton::getInstance();
            if (ISNULL(mOverallControl)) {
                LOGE(mModule, "Failed to create overall control.");
                rc = NO_MEMORY;
            }
        }
    }

    if (SUCCEED(rc)) {
        if (NOTNULL(mOverallControl)) {
            rc = mOverallControl->addServer(
                SERVER_SOCKET_PATH, mName, MAX_CLIENT_ALLOWED);
            if (FAILED(rc)) {
                LOGE(mModule, "Failed to add main server into overall control, %d", rc);
            }
        }
    }

    if (SUCCEED(rc)) {
        if (NOTNULL(mOverallControl)) {
            mOverallControlFd = mOverallControl->getFd();
            if (mOverallControlFd <= 0) {
                LOGE(mModule, "Invalid overall control fd, %d", mOverallControlFd);
                rc = INTERNAL_ERROR;
            }
        }
    }

    if (SUCCEED(rc)) {
        rc = mSS.construct((mName + SOCKET_SUFFIX_AFTER_SERVER_NAME).c_str());
        if (FAILED(rc)) {
            LOGE(mModule, "Failed to construct server state machine, %d", rc);
        }
    }

    if (SUCCEED(rc)) {
        rc = mCb.construct();
        if (FAILED(rc)) {
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
        if (FAILED(rc)) {
            LOGE(mModule, "Failed to run once thread, %d", rc);
        }
    }

    if (SUCCEED(rc)) {
        mConstructed = true;
        LOGD(mModule, "Server core constructed");
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
        } else if (FAILED(rc)) {
            LOGE(mModule, "Failed to start socket server");
        }
    }

    if (SUCCEED(rc)) {
        rc = mSS.waitForConnect();
        if (FAILED(rc)) {
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
        if (FAILED(rc)) {
            LOGE(mModule, "Failed to receive msg from socket sm, %d", rc);
        }
    }

    if (SUCCEED(rc)) {
        if (!COMPARE_SAME_STRING(mSocketMsg, SOCKET_CLIENT_GREETING_STR)) {
            LOGE(mModule, "Unknown msg received, \"%s\"", mSocketMsg);
            rc = BAD_PROTOCOL;
        }
    }

    if (SUCCEED(rc)) {
        if (validFd(mOverallControlFd)) {
            rc = shareOverallControl();
            if (FAILED(rc)) {
                LOGE(mModule, "Failed to share overall control to client, %d", rc);
            }
        } else {
            mClientReady = true;
        }
    }

    if (SUCCEED(rc)) {
        rc = enableAllRequestedRequests();
        if (FAILED(rc)) {
            LOGE(mModule, "Failed to enable cached requests, %d", rc);
        }
    }

    if (SUCCEED(rc)) {
        rc = mSS.sendMsg(SOCKET_START_REQUEST_CONNECTION,
            strlen(SOCKET_START_REQUEST_CONNECTION));
        if (FAILED(rc)) {
            LOGE(mModule, "Failed to send msg \"%s\" to server, %d",
                SOCKET_START_REQUEST_CONNECTION, rc);
        }
    }

    if (SUCCEED(rc)) {
        do {
            int32_t clientfd = -1;
            RequestType type = REQUEST_TYPE_MAX_INVALID;
            std::string privateMsg;
            RESETRESULT(rc);

            if (SUCCEED(rc)) {
                rc = mSS.waitForConnect(&clientfd);
                if (FAILED(rc)) {
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
                if (FAILED(rc)) {
                    LOGE(mModule, "Failed to receive msg, %d", rc);
                }
            }

            if (SUCCEED(rc)) {
                rc = revealRequestTypeAndPrivateArgFromMsg(mSocketMsg, type, privateMsg);
                if (FAILED(rc) ||
                    type == REQUEST_TYPE_MAX_INVALID ||
                    privateMsg == "") {
                    LOGE(mModule, "Invalid socket msg, %s", mSocketMsg);
                }
            }

            if (SUCCEED(rc)) {
                if (requested(type)) {
                    rc = mRequests[type]->onClientReady(clientfd, privateMsg);
                    if (FAILED(rc)) {
                        LOGE(mModule, "Failed to notify client connected to %s",
                            rc, mRequests[type]->getName());
                    }
                }
            }

            if (SUCCEED(rc)) {
                if (!requested(type)) {
                    rc = replyClientRequestNotRequested();
                    if (FAILED(rc)) {
                        LOGE(mModule, "Failed to reply client not requested msg, %d", rc);
                    }
                }
            }

        } while(rc != USER_ABORTED);
   }

    return rc;
}

int32_t ServerCore::shareOverallControl()
{
    int32_t rc = NO_ERROR;
    int32_t clientfd = mOverallControlFd;

    if (SUCCEED(rc)) {
        rc = mSS.sendMsg(SOCKET_SEND_SHARE_OVERALL_CONTROL,
            strlen(SOCKET_SEND_SHARE_OVERALL_CONTROL));
        if (FAILED(rc)) {
            LOGE(mModule, "Failed to send msg %s to client, %d", mSocketMsg, rc);
        }
    }

    if (SUCCEED(rc)) {
        mSocketMsg[0] = '\0';
        rc = mSS.receiveMsg(mSocketMsg, sizeof(mSocketMsg));
        if (FAILED(rc)) {
            LOGE(mModule, "Failed to receive msg from socket sm, %d", rc);
        }
    }

    if (SUCCEED(rc)) {
        if (!COMPARE_SAME_STRING(mSocketMsg, SOCKET_REPLY_OVERALL_CONTROL)) {
            LOGE(mModule, "Unknown msg received, \"%s\"", mSocketMsg);
            rc = BAD_PROTOCOL;
        }
    }

    if (SUCCEED(rc)) {
        rc = mSS.sendFd(clientfd);
        if (FAILED(rc)) {
            LOGE(mModule, "Failed to send fd %d to client, %d", clientfd, rc);
        }
    }

    if (SUCCEED(rc)) {
        mSocketMsg[0] = '\0';
        rc = mSS.receiveMsg(mSocketMsg, sizeof(mSocketMsg));
        if (FAILED(rc)) {
            LOGE(mModule, "Failed to receive msg from socket sm, %d", rc);
        }
    }

    if (SUCCEED(rc)) {
        if (!COMPARE_SAME_STRING(mSocketMsg, SOCKET_DONE_OVERALL_CONTROL)) {
            LOGE(mModule, "Unknown msg received, \"%s\"", mSocketMsg);
            rc = BAD_PROTOCOL;
        }
    }

    return rc;
}

int32_t ServerCore::replyClientRequestNotRequested()
{
    int32_t rc = NO_ERROR;

    if (SUCCEED(rc)) {
        rc = mSS.sendMsg(SOCKET_SERVER_REPLY_REQUEST_NO,
            strlen(SOCKET_SERVER_REPLY_REQUEST_NO));
        if (FAILED(rc)) {
            LOGE(mModule, "Failed to send msg %s to client, %d",
                SOCKET_SERVER_REPLY_REQUEST_NO, rc);
        }
    }

    if (SUCCEED(rc)) {
        mSocketMsg[0] = '\0';
        rc = mSS.receiveMsg(mSocketMsg, sizeof(mSocketMsg));
        if (FAILED(rc)) {
            LOGE(mModule, "Failed to receive msg from socket sm, %d", rc);
        }
    }

    if (SUCCEED(rc)) {
        if (!COMPARE_SAME_STRING(mSocketMsg, SOCKET_CLIENT_SEND_REQUEST_DONE)) {
            LOGE(mModule, "Unknown msg received, \"%s\"", mSocketMsg);
            rc = BAD_PROTOCOL;
        }
    }

    return rc;
}

int32_t ServerCore::revealRequestTypeAndPrivateArgFromMsg(
    char *msg, RequestType &type, std::string &privateArg)
{
    int32_t rc = NO_ERROR;
    std::vector<std::string> words;
    std::string typeStr;
    std::string privateMsgStr;

    if (SUCCEED(rc)) {
        type = REQUEST_TYPE_MAX_INVALID;
        privateArg = "";
    }

    if (SUCCEED(rc)) {
        std::string str = msg;
        std::regex wsre(" ");
        std::vector<std::string> _words(
            std::sregex_token_iterator(str.begin(), str.end(), wsre, -1),
            std::sregex_token_iterator());
        _words.erase(
            std::remove_if(
                _words.begin(), _words.end(),
                [](std::string const &s) { return s.empty(); }),
                _words.end());
        words = _words;
    }

    if (SUCCEED(rc)) {
        bool first = true;
        for (auto &&word : words) {
            if (word[0] == "<" &&
                word[word.length() - 1] == ">") {
                if (first) {
                    typeStr = word;
                    first = false;
                } else {
                    privateMsgStr = word;
                    break;
                }
            }
        }
    }

    if (SUCCEED(rc)) {
        int32_t value = atoi(typeStr.c_str());
        if (!checkValid(static_cast<RequestType>(value)) ||
            privateMsgStr == "") {
            LOGE(mModule, "Failed to convert msg to request type, %d", value);
            rc = PARAM_INVALID;
        }
    }

    if (SUCCEED(rc)) {
        type = atoi(typeStr.c_str());
        privateArg = privateMsg;
    }

    return rc;
}

int32_t ServerCore::enableAllRequestedRequests()
{
    int32_t rc = NO_ERROR;

    if (SUCCEED(rc)) {
        if (!mClientReady) {
            LOGE(mModule, "Client not ready, abort.");
            rc = NOT_INITED;
        }
    }

    if (SUCCEED(rc)) {
        for (int32_t i = 0; i < ARRAYSIZE(mCachedRequest); i++) {
            if (mCachedRequest[i]) {
                rc = request(static_cast<RequestType>(i));
                if (FAILED(rc)) {
                    LOGE(mModule, "Failed to create cached %s request %d",
                        getRequestName(static_cast<RequestType>(i)), rc);
                }
            }
        }
    }

    return rc;
}

bool ServerCore::validFd(int32_t fd)
{
    return fd > 0;
}

bool ServerCore::requested(RequestType type)
{
    return NOTNULL(mRequests[type]);
}

int32_t ServerCore::exitServerLoop()
{
    int32_t rc = mSS.cancelWaitConnect();
    if (FAILED(rc)) {
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
        rc = exitServerLoop();
        if (FAILED(rc)) {
            final |= rc;
            LOGE(mModule, "Failed to abort run once thread");
            RESETRESULT(rc);
        }
    }

    if (SUCCEED(rc)) {
        for (int32_t i = 0; i < REQUEST_TYPE_MAX_INVALID; i++) {
            if (NOTNULL(mRequests[i])) {
                rc = mRequests[i]->abort();
                if (FAILED(rc)) {
                    final |= rc;
                    LOGE(mModule, "Failed to abort request handler %s",
                        mRequests[i]->getName());
                    RESETRESULT(rc);
                }
                rc = mRequests[i]->destruct();
                if (FAILED(rc)) {
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
        if (FAILED(rc)) {
            final |= rc;
            LOGE(mModule, "Failed to destruct socket state machine, %d", rc);
            RESETRESULT(rc);
        }
    }

    if (SUCCEED(rc)) {
        rc = mCb.destruct();
        if (FAILED(rc)) {
            final |= rc;
            LOGE(mModule, "Failed to destruct callback thread, %d", rc);
            RESETRESULT(rc);
        }
    }

    if (SUCCEED(rc)) {
        if (NOTNULL(mThreads)) {
            mThreads->removeInstance();
            mThreads = nullptr;
        }
    }

    if (SUCCEED(rc)) {
        if (NOTNULL(mOverallControl)) {
            mOverallControl->removeInstance();
        }
    }

    if (FAILED(final)) {
        LOGE(mModule, "Server core destructed with error %d", final);
    } else {
        LOGD(mModule, "Server core destructed");
    }

    return rc;
}

int32_t ServerCore::request(RequestType type)
{
    int32_t rc = NO_ERROR;

    if (SUCCEED(rc)) {
        if (!checkValid(type)) {
            LOGE(mModule, "Invalid request type %d", type);
            rc = PARAM_INVALID;
        }
    }

    if (SUCCEED(rc)) {
        if (!mClientReady) {
            mCachedRequest[type] = true;
            LOGD(mModule, "Client not ready, request %s cached.",
                getRequestName(type));
         }
    }

    if (SUCCEED(rc)) {
        if (mClientReady) {
            rc = createRequestHandler(type);
            if (FAILED(rc)) {
                LOGE(mModule, "Failed to create request %d", type);
            }
        }
    }

    return rc
}

int32_t ServerCore::createRequestHandler(RequestType type)
{
    int32_t rc = NO_ERROR;
    RequestHandler *requestHandler = nullptr;

    if (SUCCEED(rc)) {
        if (requested(type))) {
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
        if (FAILED(rc)) {
            LOGE(mModule, "Failed to const request handler, %d", rc);
        } else {
            mRequests[type] = requestHandler;
            mCachedRequest[type] = false;
        }
    }

    if (FAILED(rc)) {
        if (NOTNULL(requestHandler)) {
            requestHandler->destruct();
            SECURE_DELETE(requestHandler);
            mRequests[type] = nullptr;
        }
    }

    return rc;
}

int32_t ServerCore::abort(RequestType type)
{
    int32_t rc = NO_ERROR;
    RequestHandlerIntf *requestHandler = nullptr;

    if (SUCCEED(rc)) {
        if (!checkValid(type)) {
            LOGE(mModule, "Invalid request type %d", type);
            rc = PARAM_INVALID;
        }
    }

    if (SUCCEED(rc)) {
        mCachedRequest[type] = false;
        if (!requested(type)) {
            LOGD(mModule, "%d not requested", type);
            rc = NOT_INITED;
        }
    }

    if (SUCCEED(rc)) {
        requestHandler = mRequests[type];
        rc = requestHandler->abort();
        if (FAILED(rc)) {
            LOGE(mModule, "Failed to exit request handler %s, %d",
                requestHandler->getName(), rc);
        }
    }

    if (SUCCEED(rc)) {
        rc = requestHandler->destruct();
        if (FAILED(rc)) {
            LOGE(mModule, "Failed to destruct request handler %s, %d",
                requestHandler->getName(), rc);
        }
    }

    if (SUCCEED(rc)) {
        SECURE_DELETE(requestHandler);
        mRequests[type] = nullptr;
    }

    return RETURNIGNORE(rc, NOT_INITED);
}

template <typename T>
int32_t ServerCore::request(T cbFunc, RequestType type)
{
    int32_t rc = NO_ERROR;

    if (SUCCEED(rc)) {
        rc = mCb.set(cbFunc);
        if (FAILED(rc)) {
            LOGE(mModule, "Failed to set cb thread, %d", rc);
        }
    }

    if (SUCCEED(rc)) {
        if (NOTNULL(cbFunc)) {
            rc = request(type);
            if (FAILED(rc)) {
                LOGE(mModule, "Failed to request %s, %d",
                    getRequestName(type), rc);
            }
        }
    }

    if (SUCCEED(rc)) {
        if (ISNULL(cbFunc)) {
            rc = abort(type);
            if (FAILED(rc)) {
                LOGE(mModule, "Failed to abort %s, %d",
                    getRequestName(type), rc);
            }
        }
    }

    return rc;
}

int32_t ServerCore::request(DataCbFunc dataCbFunc)
{
    return request<DataCbFunc>(dataCbFunc, DATA);
}

int32_t ServerCore::request(FdCbFunc fdCbFunc)
{
    return request<FdCbFunc>(fdCbFunc, FD);
}

int32_t ServerCore::request(FrameCbFunc frameCbFunc)
{
    return request<FrameCbFunc>(frameCbFunc, FRAME);
}

int32_t ServerCore::request(EventCbFunc eventCbFunc)
{
    return request<EventCbFunc>(eventCbFunc, EVENT);
}

int32_t ServerCore::enqueue(void *dat)
{
    int32_t rc = NO_ERROR;

    if (SUCCEED(rc)) {
        if (!requested(DATA)) {
            LOGE(mModule, "Data request not created or aborted, "
                "enqueue failed.");
            rc = NOT_EXIST;
        }
    }

    if (SUCCEED(rc)) {
        rc = mRequests[DATA]->enqueue(dat);
        if (FAILED(rc)) {
            LOGE(mModule, "Failed to enqueue %p to %s",
                dat, mRequests[DATA]->getName());
        }
    }

    return RETURNIGNORE(rc, NOT_REQUIRED);
}


int32_t ServerCore::enqueue(int32_t fd)
{
    int32_t rc = NO_ERROR;

    if (SUCCEED(rc)) {
        if (!requested(FD)) {
            LOGE(mModule, "Fd request not created or aborted, "
                "enqueue failed.");
            rc = NOT_EXIST;
        }
    }

    if (SUCCEED(rc)) {
        rc = mRequests[FD]->enqueue(fd);
        if (FAILED(rc)) {
            LOGE(mModule, "Failed to enqueue fd %d to %s",
                fd, mRequests[FD]->getName());
        }
    }

    return RETURNIGNORE(rc, NOT_REQUIRED);
}

int32_t ServerCore::enqueue(void *dat, int32_t format)
{
    int32_t rc = NO_ERROR;

    if (SUCCEED(rc)) {
        if (!requested(FRAME)) {
            LOGE(mModule, "Frame request not created or aborted, "
                "enqueue failed.");
            rc = NOT_EXIST;
        }
    }

    if (SUCCEED(rc)) {
        rc = mRequests[FRAME]->enqueue(dat, format);
        if (FAILED(rc)) {
            LOGE(mModule, "Failed to enqueue fd %p %d to %s",
                dat, format, mRequests[FRAME]->getName());
        }
    }

    return RETURNIGNORE(rc, NOT_REQUIRED);
}

int32_t ServerCore::cancel(RequestType type)
{
    return abort(type);
}

int32_t ServerCore::send(void *dat, int64_t len)
{
    return mCb.send(dat, len);
}

int32_t ServerCore::send(int32_t fd, int64_t len)
{
    return mCb.send(fd, len);
}

int32_t ServerCore::send(void *dat, int64_t len, int32_t format)
{
    return mCb.send(dat, len, format);
}

int32_t ServerCore::send(int32_t event, int32_t arg1, int32_t arg2)
{
    return mCb.send(event, arg1, arg2);
}

ServerCore::ServerCore() :
    mConstructed(false),
    mClientReady(false),
    mOverallControl(nullptr),
    mOverallControlFd(-1),
    mThreads(nullptr)
{
    mSocketMsg[0] = '\0';
    for (int32_t i = 0; i < REQUEST_TYPE_MAX_INVALID; i++) {
        mRequests[i] = nullptr;
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


#include "ClientCore.h"

namespace voyager {

int32_t ClientCore::send(void *dat, int64_t len)
{
    int32_t rc = NO_ERROR;

    if (SUCCEED(rc)) {
        rc = createHandlerIfRequired(DATA);
        if (FAILED(rc)) {
            LOGE(mModule, "Failed to create data client, %d", rc);
        }
    }

    if (SUCCEED(rc)) {
        RequestHandler *handler = mRequests[DATA];
        if (NOTNULL(handler))
        rc = handler->send(dat, len);
        if (FAILED(rc)) {
            LOGE(mModule, "Send data %p %d failed, %d", dat, len, rc);
        }
    }

    return rc;
}

int32_t ClientCore::send(int32_t fd, int64_t len)
{
    int32_t rc = NO_ERROR;

    if (SUCCEED(rc)) {
        rc = createHandlerIfRequired(FD);
        if (FAILED(rc)) {
            LOGE(mModule, "Failed to create fd client, %d", rc);
        }
    }

    if (SUCCEED(rc)) {
        RequestHandler *handler = mRequests[FD];
        if (NOTNULL(handler))
        rc = handler->send(fd, len);
        if (FAILED(rc)) {
            LOGE(mModule, "Send fd %d %d failed, %d", fd, len, rc);
        }
    }

    return rc;
}


int32_t ClientCore::send(void *dat, int64_t len, int32_t format)
{
    int32_t rc = NO_ERROR;

    if (SUCCEED(rc)) {
        rc = createHandlerIfRequired(FRAME);
        if (FAILED(rc)) {
            LOGE(mModule, "Failed to create frame client, %d", rc);
        }
    }

    if (SUCCEED(rc)) {
        RequestHandler *handler = mRequests[FRAME];
        if (NOTNULL(handler))
        rc = handler->send(dat, len);
        if (FAILED(rc)) {
            LOGE(mModule, "Send frame %p %d %d failed, %d", dat, len, format, rc);
        }
    }

    return rc;
}


int32_t ClientCore::send(int32_t event, int32_t arg1, int32_t arg2)
{
    int32_t rc = NO_ERROR;

    if (SUCCEED(rc)) {
        rc = createHandlerIfRequired(EVENT);
        if (FAILED(rc)) {
            LOGE(mModule, "Failed to create event client, %d", rc);
        }
    }

    if (SUCCEED(rc)) {
        RequestHandler *handler = mRequests[EVENT];
        if (NOTNULL(handler))
        rc = handler->send(event, arg1, arg2);
        if (FAILED(rc)) {
            LOGE(mModule, "Send event %d %d %d failed, %d", event, arg1, arg2, rc);
        }
    }

    return rc;
}

int32_t ClientCore::createHandlerIfRequired(RequestType type)
{
    int32_t rc = NO_ERROR;

    if (SUCCEED(rc)) {
        if (!requested(type)) {
            LOGI(mModule, "%s not requested.", getRequestName(type));
            rc = NOT_REQUIRED;
        }
    }

    if (SUCCEED(rc)) {
        if (ISNULL(mRequests[type])) {
            mRequests[type] = createClientRequestHandler(type, mName);
            if (ISNULL(mRequests[type])) {
                LOGE(mModule, "Failed to create %s client.",
                    getRequestName(type));
                rc = NO_MEMORY;
            } else {
                rc = mRequests[type]->construct();
                if (FAILED(rc)) {
                    LOGE(mModule, "Failed to construct %s client, %d",
                        getRequestName(type), rc);
                }
            }
        }
    }

    return rc;
}

int32_t ClientCore::connectServer()
{
    int32_t rc = NO_ERROR;

    if (SUCCEED(rc)) {
        if (mConnected) {
            LOGE(mModule, "Already connected to %s", mName.c_str());
            rc = ALREADY_EXISTS;
        }
    }

    if (SUCCEED(rc)) {
        rc = mSC.connectServer();
        if (FAILED(rc)) {
            LOGD(mModule, "Failed to connect server %s, may not started, "
                "%s %d", mName.c_str(), strerror(errno), rc);
            rc = NOT_EXIST;
        }
    }

    if (SUCCEED(rc)) {
        rc = mSC.sendMsg(SOCKET_CLIENT_GREETING_STR,
            strlen(SOCKET_CLIENT_GREETING_STR));
        if (FAILED(rc)) {
            LOGE(mModule, "Failed to send msg \"%s\" to server, %d",
                SOCKET_CLIENT_GREETING_STR, rc);
        }
    }

    if (SUCCEED(rc)) {
        mConnected = true;
    }

    return rc;
}

bool ClientCore::requested(RequestType type)
{
    int32_t rc = NO_ERROR;
    bool result = false;
    char socketMsg[SOCKET_DATA_MAX_LEN];

    if (SUCCEED(rc)) {
        if (!mConnected) {
            AutoMutex mutex;
            if (!mConnected) {
                rc = connectServer();
                if (FAILED(rc)) {
                    LOGE(mModule, "Failed to import overall control, %d", rc);
                }
            }
        }
    }

    if (SUCCEED(rc)) {
        if (ISNULL(mOverallControlSingleton) && !mSkipOverallControl) {
            AutoMutex mutex;
            if (ISNULL(mOverallControlSingleton) && !mSkipOverallControl) {
                rc = importOverallControl();
                if (FAILED(rc)) {
                    LOGE(mModule, "Failed to import overall control, %d", rc);
                }
            }
        }
    }

    if (SUCCEED(rc)) {
        std::string msg = SOCKET_CLIENT_QUERY_REQUEST;
        msg.replace("%TYPE%", std::to_string(type));
        rc = mSC.sendMsg(msg.c_str(), msg.length());
        if (FAILED(rc)) {
            LOGE(mModule, "Failed to send msg \"%s\" to server, %d",
                msg.c_str(), rc);
        }
    }

    if (SUCCEED(rc)) {
        socketMsg[0] = '\0';
        rc = mSC.receiveMsg(socketMsg, sizeof(socketMsg));
        if (FAILED(rc)) {
            LOGE(mModule, "Failed to receive msg from server, %d", rc);
        } else {
            result = COMPARE_SAME_STRING(socketMsg, SOCKET_SERVER_REPLY_REQUEST_OK);
        }
    }

    if (SUCCEED(rc)) {
        std::string msg = SOCKET_CLIENT_SEND_REQUEST_DONE;
        rc = mSC.sendMsg(msg.c_str(), msg.length());
        if (FAILED(rc)) {
            LOGE(mModule, "Failed to send msg \"%s\" to server, %d",
                msg.c_str(), rc);
        }
    }

    return rc;
}

int32_t ClientCore::importOverallControl()
{
    int32_t rc = NO_ERROR;
    int32_t overallControlFd;
    int32_t size = 0;
    bool locked  = false;
    char msg[SOCKET_DATA_MAX_LEN];

    if (SUCCEED(rc)) {
        if (NOTNULL(mOverallControlSingleton) || mSkipOverallControl) {
            LOGE(mModule, "Overall control already imported or skipped.");
            rc = ALREADY_EXISTS;
        }
    }

    if (SUCCEED(rc)) {
        msg[0] = '\0';
        rc = mSC.receiveMsg(msg, sizeof(msg));
        if (FAILED(rc)) {
            LOGE(mModule, "Failed to receive msg from server %s, %d",
                mName.c_str(), rc);
        }
    }

    if (SUCCEED(rc)) {
        if (COMPARE_SAME_STRING(msg, SOCKET_START_QUERY_REQUEST)) {
            LOGI(mModule, "Skip overall control share.");
            mSkipOverallControl = true;
            rc = JUMP_DONE;
        }
    }

    if (SUCCEED(rc)) {
        if (!COMPARE_SAME_STRING(msg, SOCKET_REPLY_OVERALL_CONTROL)) {
            LOGE(mModule, "Wrong msg received, %s.", msg);
            rc = BAD_PROTOCOL;
        }
    }

    if (SUCCEED(rc)) {
        rc = mSC.sendMsg(SOCKET_REPLY_OVERALL_CONTROL,
            strlen(SOCKET_REPLY_OVERALL_CONTROL));
        if (FAILED(rc)) {
            LOGE(mModule, "Failed to send msg \"%s\" to server, %d",
                SOCKET_REPLY_OVERALL_CONTROL, rc);
        }
    }

    if (SUCCEED(rc)) {
        rc = mSC.receiveFd(&overallControlFd);
        if (FAILED(rc)) {
            LOGE(mModule, "Failed to receive fd from server %s, %d", mName.c_str(), rc);
        }
        if (overallControlFd == -1) {
            LOGE(mModule, "Invalid fd received from server %s", mName.c_str());
            rc = mSC.sendMsg(SOCKET_DONE_OVERALL_CONTROL,
                strlen(SOCKET_DONE_OVERALL_CONTROL));
            if (FAILED(rc)) {
                LOGE(mModule, "Failed to send msg \"%s\" to server, %d",
                    SOCKET_DONE_OVERALL_CONTROL, rc);
            }
            rc = BAD_PROTOCOL;
        }
    }

    if (SUCCEED(rc)) {
        rc = setOverallControlMemory(overallControlFd);
        if (FAILED(rc)) {
            LOGE(mModule, "Failed to set overall control, %d", rc);
        }
    }

    if (SUCCEED(rc)) {
        rc = mSC.sendMsg(SOCKET_DONE_OVERALL_CONTROL,
            strlen(SOCKET_DONE_OVERALL_CONTROL));
        if (FAILED(rc)) {
            LOGE(mModule, "Failed to send msg \"%s\" to server, %d",
                SOCKET_DONE_OVERALL_CONTROL, rc);
        }
    }

    if (SUCCEED(rc)) {
        msg[0] = '\0';
        rc = mSC.receiveMsg(msg, sizeof(msg));
        if (FAILED(rc)) {
            LOGE(mModule, "Failed to receive msg from server %s, %d",
                mName.c_str(), rc);
        }
    }

    if (SUCCEED(rc)) {
        if (!COMPARE_SAME_STRING(msg, SOCKET_START_QUERY_REQUEST)) {
            LOGI(mModule, "Invalid msg %s received.", msg);
            rc = BAD_PROTOCOL;
        }
    }

    return RETURNIGNORE(rc, JUMP_DONE);
}

int32_t ClientCore::setOverallControlMemory(int32_t fd)
{
    int32_t rc = NO_ERROR;

    if (SUCCEED(rc)) {
        if (NOTNULL(mOverallControlSingleton)) {
            LOGE(mModule, "Overall control already exist.");
            rc = ALREADY_EXISTS;
        }
    }

    if (SUCCEED(rc)) {
        mOverallControlSingleton = OverallControlSingleton::getInstance();
        if (ISNULL(mOverallControlSingleton)) {
            LOGE(mModule, "Failed to create overall control singleton.");
            rc = NO_MEMORY;
        }
    }

    if (SUCCEED(rc)) {
        rc = mOverallControlSingleton->import(fd, sizeof(OverallControlLayout));
        if (FAILED(rc)) {
            LOGE(mModule, "Failed to import fd %d to overall control, %d", fd, rc);
        }
    }

    return rc;
}

int32_t ClientCore::construct()
{
    int32_t rc = NO_ERROR;

    if (mConstructed) {
        LOGD(mModule, "Already constructed.");
        rc = ALREADY_INITED;
    }

    if (SUCCEED(rc)) {
        rc = mSC.construct((mName + SOCKET_SUFFIX_AFTER_SERVER_NAME).c_str());
        if (FAILED(rc)) {
            LOGE(mModule, "Failed to construct ssm, %d", rc);
        }
    }

    if (SUCCEED(rc)) {
        mConstructed = true;
        LOGD(mModule, "Client core constructed");
    }

    return rc;
 }

int32_t ClientCore::destruct()
{
    int32_t rc = NO_ERROR;
    int32_t final = rc;

    if (!mConstructed) {
        rc = NOT_INITED;
    } else {
        mConstructed = false;
    }

    if (SUCCEED(rc)) {
        for (int32_t i = 0; i < REQUEST_TYPE_MAX_INVALID; i++) {
            RequestHandler *handler = mRequests[static_cast<RequestType>(i)];
            if (NOTNULL(handler)) {
                rc = handler->destruct();
                if (FAILED(rc)) {
                    LOGE(mModule, "Failed to destruct %s client, %d",
                        getRequestName(static_cast<RequestType>(i)), rc);
                }
            }
        }
    }

    if (SUCCEED(rc)) {
        rc = mSC.destruct();
        if (FAILED(rc)) {
            final |= rc;
            LOGE(mModule, "Failed to destruct socket state machine, %d", rc);
            RESETRESULT(rc);
        }
    }

    if (SUCCEED(rc)) {
        if (NOTNULL(mOverallControlSingleton)) {
            mOverallControlSingleton->removeInstance();
        }
    }

    if (FAILED(final)) {
        LOGE(mModule, "Client core destructed with error %d", final);
    } else {
        LOGD(mModule, "Client core destructed");
    }

    return rc;
}

ClientCore::ClientCore(const char *name) :
    Identifier(MODULE_CLIENT_CORE, "ClientCore", "1.0.0")
    mConstructed(false),
    mConnected(false),
    mSkipOverallControl(false),
    mOverallControlSingleton(nullptr)
{
    for (int32_t i = 0; i < REQUEST_TYPE_MAX_INVALID; i++) {
        mRequests[i] = nullptr;
    }
}

ClientCore::~ClientCore()
{
    if (mConstructed) {
        destruct();
    }
}

};


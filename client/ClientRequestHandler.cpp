#include <sstream>

#include "ClientRequestHandler.h"
#include "Times.h"
#include "protocol.h"
#include "IntfImpl.h"

#define CONNECT_SERVER_RETRY_COUNT 10
#define CONNECT_SERVER_RETRY_TIME  10 // ms

namespace voyager {

int32_t RequestHandler::send(void *dat, int64_t len)
{
    int32_t rc = NO_ERROR;
    int32_t fd = -1;

    if (SUCCEED(rc)) {
        if (!mConnected) {
            rc = connectServer();
            if (FAILED(rc)) {
                LOGE(mModule, "Failed to connect server, %d", rc);
            }
        }
    }

    if (SUCCEED(rc)) {
        rc = allocFdAndCopy(dat, len, fd);
        if (FAILED(rc) || !validFd(fd)) {
            LOGE(mModule, "Failed to alloc %d memory from buf mgr, %d", len, rc);
        }
    }

    if (SUCCEED(rc)) {
        rc = sendPrivateMsg(DATA, len);
        if (FAILED(rc)) {
            LOGE(mModule, "Failed to send private msg, %d", rc);
        }
    }

    if (SUCCEED(rc)) {
        rc = sendServerFdAndWaitReply(fd);
        if (FAILED(rc)) {
            LOGE(mModule, "Failed to send fd and wait reply, %d", rc);
        }
    }

    if (SUCCEED(rc) || FAILED(rc)) {
        if (validFd(fd)) {
            rc = mBufMgr.release(fd);
            if (FAILED(rc)) {
                LOGE(mModule, "Failed to release fd %d to buf mgr, %d", fd, rc);
            }
        }
    }

    return rc;
}

int32_t RequestHandler::send(int32_t fd, int64_t len)
{
    int32_t rc = NO_ERROR;

    if (SUCCEED(rc)) {
        if (!mConnected) {
            rc = connectServer();
            if (FAILED(rc)) {
                LOGE(mModule, "Failed to connect server, %d", rc);
            }
        }
    }

    if (SUCCEED(rc)) {
        rc = sendPrivateMsg(FD, len);
        if (FAILED(rc)) {
            LOGE(mModule, "Failed to send private msg, %d", rc);
        }
    }

    if (SUCCEED(rc)) {
        rc = sendServerFdAndWaitReply(fd);
        if (FAILED(rc)) {
            LOGE(mModule, "Failed to send fd and wait reply, %d", rc);
        }
    }

    return rc;
}

int32_t RequestHandler::send(int32_t fd, int64_t len, int32_t format)
{
    int32_t rc = NO_ERROR;

    if (SUCCEED(rc)) {
        if (!mConnected) {
            rc = connectServer();
            if (FAILED(rc)) {
                LOGE(mModule, "Failed to connect server, %d", rc);
            }
        }
    }

    if (SUCCEED(rc)) {
        rc = sendPrivateMsg(FRAME, len, format);
        if (FAILED(rc)) {
            LOGE(mModule, "Failed to send private msg, %d", rc);
        }
    }

    if (SUCCEED(rc)) {
        rc = sendServerFdAndWaitReply(fd);
        if (FAILED(rc)) {
            LOGE(mModule, "Failed to send fd and wait reply, %d", rc);
        }
    }

    return rc;
}

int32_t RequestHandler::send(int32_t event, int32_t arg1, int32_t arg2)
{
    int32_t rc = NO_ERROR;

    if (SUCCEED(rc)) {
        if (!mConnected) {
            rc = connectServer();
            if (FAILED(rc)) {
                LOGE(mModule, "Failed to connect server, %d", rc);
            }
        }
    }

    if (SUCCEED(rc)) {
        rc = sendPrivateMsg(event, arg1, arg2);
        if (FAILED(rc)) {
            LOGE(mModule, "Failed to send private msg, %d", rc);
        }
    }

    if (SUCCEED(rc)) {
        rc = waitServerReply();
        if (FAILED(rc)) {
            LOGE(mModule, "Failed to wait reply, %d", rc);
        }
    }

    return rc;
}

bool RequestHandler::requested(RequestType type)
{
    return false;
}

int32_t RequestHandler::connectServer()
{
    int32_t rc = NO_ERROR;

    if (SUCCEED(rc)) {
        if (mConnected) {
            LOGE(mModule, "Already connected.");
            rc = ALREADY_EXISTS;
        }
    }

    if (SUCCEED(rc)) {
        int32_t retryCnt = 0;
        while (!mConnected && retryCnt++ < 10) {
            rc = mSC.connectServer();
            if (FAILED(rc)) {
                LOGD(mModule, "Failed to connect server, "
                    "may not started, try again, %s %d", strerror(errno), rc);
                usleep(Times::Us(Times::Ms(10))());
            } else {
                mConnected = true;
            }
        }
    }

    return rc;
}

int32_t RequestHandler::sendPrivateMsg(RequestType type, int64_t len)
{
    int32_t rc = NO_ERROR;
    std::string msg = SOCKET_CLIENT_SEND_REQUEST;

    if (SUCCEED(rc)) {
        size_t pos1 = msg.find("%TYPE%");
        msg.replace(pos1, strlen("%TYPE%"), std::to_string(type));
        size_t pos2 = msg.find("%PRIVATE%");
        std::stringstream ss;
        ss << len;
        msg.replace(pos2, strlen("%PRIVATE%"), ss.str());
    }

    if (SUCCEED(rc)) {
        rc = mSC.sendMsg(msg.c_str(), msg.length());
        if (FAILED(rc)) {
            LOGE(mModule, "Failed to send msg %s to client, %d",
                msg.c_str(), rc);
        }
    }

    return rc;
}

int32_t RequestHandler::sendPrivateMsg(RequestType type, int64_t len, int32_t format)
{
    int32_t rc = NO_ERROR;
    std::string msg = SOCKET_CLIENT_SEND_REQUEST;

    if (SUCCEED(rc)) {
        size_t pos1 = msg.find("%TYPE%");
        msg.replace(pos1, strlen("%TYPE%"), std::to_string(type));
        size_t pos2 = msg.find("%PRIVATE%");
        std::stringstream ss;
        ss << len << ";" << std::to_string(format);
        msg.replace(pos2, strlen("%PRIVATE%"), ss.str());
    }

    if (SUCCEED(rc)) {
        rc = mSC.sendMsg(msg.c_str(), msg.length());
        if (FAILED(rc)) {
            LOGE(mModule, "Failed to send msg %s to client, %d",
                msg.c_str(), rc);
        }
    }

    return rc;
}

int32_t RequestHandler::sendPrivateMsg(int32_t event, int32_t arg1, int32_t arg2)
{
    int32_t rc = NO_ERROR;
    std::string msg = SOCKET_CLIENT_EVENT_FORMAT;

    if (SUCCEED(rc)) {
        size_t pos1 = msg.find("%EVENT%");
        msg.replace(pos1, strlen("%EVENT%"), std::to_string(event));
        size_t pos2 = msg.find("%ARG1%");
        msg.replace(pos2, strlen("%ARG1%"), std::to_string(arg1));
        size_t pos3 = msg.find("%ARG2%");
        msg.replace(pos3, strlen("%ARG2%"), std::to_string(arg2));
    }

    if (SUCCEED(rc)) {
        rc = mSC.sendMsg(msg.c_str(), msg.length());
        if (FAILED(rc)) {
            LOGE(mModule, "Failed to send msg %s to client, %d",
                msg.c_str(), rc);
        }
    }

    return rc;
}

int32_t RequestHandler::allocFdAndCopy(void *dat, int64_t len, int32_t &fd)
{
    int32_t rc = NO_ERROR;
    void *localBuf = nullptr;
    int32_t localFd = -1;

    if (SUCCEED(rc)) {
        rc = mBufMgr.alloc(&localBuf, len, &localFd);
        if (FAILED(rc) || ISNULL(localBuf) || !validFd(localFd)) {
            LOGE(mModule, "Failed to alloc buffer, %d", rc);
            rc = NO_MEMORY;
        }
    }

    if (SUCCEED(rc)) {
        memcpy(localBuf, dat, len);
        fd = localFd;
    }

    return rc;
}

int32_t RequestHandler::waitServerReply()
{
    int32_t rc = NO_ERROR;
    char socketMsg[SOCKET_DATA_MAX_LEN];

    if (SUCCEED(rc)) {
        socketMsg[0] = '\0';
        rc = mSC.receiveMsg(socketMsg, sizeof(socketMsg));
        if (FAILED(rc)) {
            LOGE(mModule, "Failed to receive msg from socket sm, %d", rc);
        }
    }

    if (SUCCEED(rc)) {
        if (COMPARE_SAME_STRING(socketMsg, SOCKT_SERVER_FAILED_RECEIVE)) {
            LOGE(mModule, "Server receive msg failed.");
            rc = SERVER_ERROR;
        }
    }

    if (SUCCEED(rc)) {
        if (COMPARE_SAME_STRING(socketMsg, SOCKET_SERVER_PROCESS_FAILED)) {
            LOGE(mModule, "Server receive request data failed.");
            rc = SERVER_ERROR;
        }
    }

    if (SUCCEED(rc)) {
        if (!COMPARE_SAME_STRING(socketMsg, SOCKET_SERVER_PROCESSED)) {
            LOGE(mModule, "Server sent unknown message.");
            rc = SERVER_ERROR;
        }
    }

    return rc;
}

int32_t RequestHandler::sendServerFdAndWaitReply(int32_t fd)
{
    int32_t rc = NO_ERROR;

    if (SUCCEED(rc)) {
        rc = mSC.sendFd(fd);
        if (FAILED(rc)) {
            LOGE(mModule, "Failed to send fd %d to server, %d", fd, rc);
        }
    }

    if (SUCCEED(rc)) {
        rc = waitServerReply();
        if (FAILED(rc)) {
            LOGE(mModule, "Failed to wait server reply, %d", rc);
        }
    }

    return rc;
}

bool RequestHandler::validFd(int32_t fd)
{
    return fd > 0;
}

const char *RequestHandler::getName()
{
    return mName.c_str();
}

RequestType RequestHandler::getType()
{
    return mType;
}

int32_t RequestHandler::construct()
{
    int32_t rc = NO_ERROR;

    if (mConstructed) {
        rc = ALREADY_INITED;
    }

    if (SUCCEED(rc)) {
        std::string name = mName;
        name += "_";
        name += getRequestName(getType());
        name += "_server";
        rc = mSC.construct(name.c_str());
        if (FAILED(rc)) {
            LOGE(mModule, "Failed to construct socket state machine, %d", rc);
        }
    }

    if (SUCCEED(rc)) {
        mConstructed = true;
        LOGD(mModule, "Request client handler constructed");
    }

    return rc;
}

int32_t RequestHandler::destruct()
{
    int32_t rc = NO_ERROR;
    int32_t final = rc;

    if (!mConstructed) {
        rc = NOT_INITED;
    } else {
        mConstructed = false;
    }

    if (SUCCEED(rc)) {
        rc = mSC.cancelWaitMsg();
        if (FAILED(rc)) {
            final |= rc;
            LOGE(mModule, "Failed to cancel wait msg for socket sm, %d", rc);
            RESETRESULT(rc);
        }
    }

    if (SUCCEED(rc)) {
        rc = mSC.destruct();
        if (FAILED(rc)) {
            final |= rc;
            LOGE(mModule, "Failed to destruct socket sm, %d", rc);
            RESETRESULT(rc);
        }
    }

    if (FAILED(final)) {
        LOGE(mModule, "Request client handler %s destructed "
            "with error %d", getRequestName(mType), final);
    } else {
        LOGD(mModule, "Request client handler %s destructed",
            getRequestName(mType));
    }

    return rc;
}

RequestHandler::RequestHandler(RequestType type, const std::string &name) :
    Identifier(MODULE_CLIENT_HANDLER, "ClientRequestHandler", "1.0.0"),
    mConstructed(false),
    mType(type),
    mName(name),
    mConnected(false)
{
    ASSERT_LOG(mModule, checkValid(type), "Invalid request type %d", type);
    if (mName == "") {
        mName = "generic request client handler";
    }
}

RequestHandler::~RequestHandler()
{
    if (mConstructed) {
        destruct();
    }
}

};


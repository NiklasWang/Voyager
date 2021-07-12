#include "protocol.h"
#include "RequestHandler.h"

namespace voyager {

int32_t RequestHandler::onClientReady(int32_t clientfd, const std::string &privateMsg)
{
    int32_t rc = NO_ERROR;

    if (SUCCEED(rc)) {
        mThreads = ThreadPoolEx::getInstance();
        if (ISNULL(mThreads)) {
            LOGE(mModule, "Failed to get thread pool");
            rc = UNKNOWN_ERROR;
        }
    }

    if (SUCCEED(rc)) {
        rc = mThreads->run(
            [this, =]() -> int32_t {
                return startServerLoop(clientfd, privateMsg);
            }
        );
        if (FAILED(rc)) {
            LOGE(mModule, "Failed to run on thread, %d", rc);
        }
    }

    return rc;
}

int32_t RequestHandler::startServerLoop(int32_t clientfd, const std::string &privateMsg)
{
    int32_t rc = NO_ERROR;

    do {
        int32_t fd = 0;
        int32_t processRc = NO_ERROR;
        RESETRESULT(rc);

        if (SUCCEED(rc)) {
            rc = mSSSM.setClientFd(clientfd);
            if (FAILED(rc)) {
                LOGE(mModule, "Failed to construct ssm, %d", rc);
            }
        }

        if (SUCCEED(rc)) {
            rc = mSSSM.receiveFd(&fd);
            if (rc == USER_ABORTED) {
                LOGI(mModule, "Abort to accept data fd.");
            } else if (FAILED(rc) && RESULTNOT(rc, USER_ABORTED)) {
                rc = mSSSM.sendMsg(SOCKT_SERVER_FAILED_RECEIVE,
                    strlen(SOCKT_SERVER_FAILED_RECEIVE));
                if (FAILED(rc)) {
                    LOGE(mModule, "Failed to send msg %s to client, %d",
                        SOCKT_SERVER_FAILED_RECEIVE, rc);
                }
            }
        }

        if (SUCCEED(rc)) {
            rc = processRc = onClientSent(fd, privateMsg);
            if (FAILED(rc)) {
                LOGE(mModule, "Failed process received fd, %d", rc);
            }
        }

        if (SUCCEED(rc)) {
            std::string msg = SUCCEED(processRc) ?
                SOCKET_SERVER_PROCESSED : SOCKET_SERVER_PROCESS_FAILED;
            rc = mSSSM.sendMsg(msg.c_str(), msg.length());
            if (FAILED(rc)) {
                LOGE(mModule, "Failed to send msg %s to client, %d",
                    msg.c_str(), rc);
            }
        }
    } while (rc != USER_ABORTED);

    close(clientfd);

    return rc;
}

bool RequestHandler::checkFdExists(int32_t fd)
{
    return mFds.find(fd) != mFds.end();
}

bool RequestHandler::checkFdExists(void *ptr)
{
    bool found = false;

    for (auto &fd : mFds) {
        if (ptr == fd.second) {
            found = true;
            break;
        }
    }

    return found;
}

int32_t RequestHandler::removeFdRecord(int32_t fd)
{
    int32_t rc = NO_ERROR;

    if (SUCCEED(rc)) {
        if (checkFdExists(dat)) {
            removeFdRecord(dat);
        } else {
            LOGE(mModule, "Fd %d not exist.", fd);
            rc = PARAM_INVALID;
        }
    }

    return rc;
}

int32_t RequestHandler::removeFdRecord(void *ptr)
{
    bool found = false;

    auto itr = mFds.begin();
    while (itr != mFds.end()) {
        if (itr->second == ptr) {
            itr = mFds.erase(itr);
            found = true;
        } else {
            itr++;
        }
    }

    return found ? NO_ERROR : NOT_EXIST;
}

void RequestHandler::addFdRecord(int32_t fd)
{
    addFdRecord(fd, nullptr);
    return;
}

void RequestHandler::addFdRecord(int32_t fd, void *ptr)
{
    mFds.push_back(std::make_pair(fd, ptr));
    return;
}

int32_t RequestHandler::enqueue(int32_t fd)
{
    close(fd);
    return NO_ERROR;
}

int32_t RequestHandler::abort()
{
    int32_t rc = NO_ERROR;

    if (ISNULL(mThreads)) {
        LOGE(mModule, "Server thread alreay exited.");
        rc = NOT_EXIST;
    }

    if (SUCCEED(rc)) {
        rc = exitServerLoop();
        if (FAILED(rc)) {
            LOGE(mModule, "Failed to abort thread loop, %d", rc);
        }
    }

    return rc;
}

int32_t RequestHandler::exitServerLoop()
{
    int32_t rc = mSSSM.cancelWaitMsg();
    if (FAILED(rc)) {
        LOGE(mModule, "Failed to cancel waitting msg, %d");
    }

    return rc;
}

RequestType RequestHandler::getType()
{
    return mType;
}

const char *RequestHandler::getName()
{
    return mName.c_str();
}

int32_t RequestHandler::construct()
{
    int32_t rc = NO_ERROR;

    if (mConstructed) {
        rc = ALREADY_INITED;
    }

    if (SUCCEED(rc)) {
        rc = mSSSM.construct();
        if (FAILED(rc)) {
            LOGE(mModule, "Failed to construct ssm, %d", rc);
        }
    }

    if (SUCCEED(rc)) {
        mConstructed = true;
        LOGD(mModule, "%s constructed", getName());
    }

    return rc;
}

int32_t RequestHandler::destruct()
{
    int32_t rc = NO_ERROR;
    int32_t final = NO_ERROR;

    if (!mConstructed) {
        rc = NOT_INITED;
    } else {
        mConstructed = false;
    }

    if(SUCCEED(rc)){
        rc = mSSSM.cancelWaitMsg();
        if (FAILED(rc)) {
            final |= rc;
            LOGE(mModule, "Failed to cancel ssm wait msg, %d", rc);
        }
    }

    if (SUCCEED(rc)) {
        rc = mSSSM.cancelWaitConnect();
        if (FAILED(rc)) {
            final |= rc;
            LOGE(mModule, "Failed to cancel ssm connect, %d", rc);
        }
    }

    if (SUCCEED(rc)) {
        rc = mSSSM.destruct();
        if (FAILED(rc)) {
            final |= rc;
            LOGE(mModule, "Failed to destruct ssm, %d", rc);
        }
    }

    if (SUCCEED(rc)) {
        if (NOTNULL(mThreads)) {
            mThreads->removeInstance();
            mThreads = nullptr;
        }
    }

    if (FAILED(final)) {
        LOGE(mModule, "%s destructed with error %d", getName(), rc);
    } else {
        LOGD(mModule, "%s destructed", getName());
    }

    return final;
}

RequestHandler::RequestHandler(RequestType type, const char *name, CallbackIntf *cb) :
    Identifier(MODULE_REQUEST_HANDLER, "RequestHandler", "1.0.0"),
    mConstructed(false),
    mName(name),
    mType(type),
    mThreads(NULL),
    mCb(cb)
{
    ASSERT_LOG(mModule, NOTNULL(mCb), "Ops shouldn't be NULL");
    ASSERT_LOG(mModule, checkValid(mType), "Invalid request type %d", type);
    if (mName == "") {
        mName = "generic request handler";
    }
}

RequestHandler::~RequestHandler()
{
    if (mConstructed) {
        destruct();
    }
}

};


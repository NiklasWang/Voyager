#include <sstream>

#include "DataServer.h"

namespace voyager {

int32_t DataServer::onClientSent(int32_t fd, const std::string &privateMsg)
{
    int32_t rc = NO_ERROR;
    void *data = nullptr;
    int64_t len = -1LL;

    if (SUCCEED(rc)) {
        std::stringstream ss(privateMsg);
        ss >> len;
    }

    if (SUCCEED(rc)) {
        rc = mBufMgr.import(&data, fd, len);
        if (FAILED(rc)) {
            LOGE(mModule, "Failed to import fd %d to buf mgr, %d", fd, rc);
        }
    }

    if (SUCCEED(rc)) {
        rc = mCb->send(fd, len);
        if (FAILED(rc)) {
            LOGE(mModule, "Failed to send fd %d len %d cb, %d",
                fd, len, rc);
        }
    }

    return rc;
}

int32_t DataServer::enqueue(int32_t fd)
{
    return NOT_SUPPORTED;
}

int32_t DataServer::enqueue(void *dat)
{
    int32_t rc = NO_ERROR;

    if (SUCCEED(rc)) {
        rc = removeFdRecord(dat);
        if (FAILED(rc)) {
            LOGE(mModule, "Failed to remove fd record, %d", rc);
        }
    }

    if (SUCCEED(rc)) {
        rc = mBufMgr.release(dat);
        if (FAILED(rc)) {
            LOGE(mModule, "Failed to release buf mgr, %d", rc);
        }
    }

    return rc;
}

int32_t DataServer::enqueue(void *dat, int32_t format)
{
    return NOT_SUPPORTED;
}

DataServer::DataServer(CallbackIntf *cb) :
    RequestHandler(DATA, cb)
{
}

DataServer::~DataServer()
{
}

};


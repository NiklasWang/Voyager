#include <sstream>

#include "FdServer.h"

namespace voyager {

int32_t FdServer::onClientSent(int32_t fd, const std::string &privateMsg)
{
    int32_t rc = NO_ERROR;
    int64_t len = -1LL;

    if (SUCCEED(rc)) {
        addFdRecord(fd);
        std::stringstream ss(privateMsg);
        ss >> len;
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

int32_t FdServer::enqueue(int32_t fd)
{
    int32_t rc = NO_ERROR;

    if (SUCCEED(rc)) {
        rc = removeFdRecord(fd);
        if (FAILED(rc)) {
            LOGE(mModule, "Failed to remove fd record, %d", rc);
        }
    }

    return rc;
}

int32_t FdServer::enqueue(void *dat)
{
    return NOT_SUPPORTED;
}

int32_t FdServer::enqueue(void *dat, int32_t format)
{
    return NOT_SUPPORTED;
}

FdServer::FdServer(CallbackIntf *cb) :
    RequestHandler(FD, cb)
{
}

FdServer::~FdServer()
{
}

};


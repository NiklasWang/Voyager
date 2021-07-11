#include <sstream>

#include "FrameServer.h"

namespace voyager {

int32_t FrameServer::onClientSent(int32_t fd, const std::string &privateMsg)
{
    int32_t rc = NO_ERROR;
    void *data = nullptr;
    int32_t format = -1;
    int64_t len = -1LL;
    std::vector<std::string> words;

    if (SUCCEED(rc)) {
        std::regex wsre("\\;");
        std::vector<std::string> _words(
            std::sregex_token_iterator(privateMsg.begin(), privateMsg.end(), wsre, -1),
            std::sregex_token_iterator());
        _words.erase(
            std::remove_if(
                _words.begin(), _words.end(),
                [](std::string const &s) { return s.empty(); }),
                _words.end());
        if (_words.size() != 2) {
            LOGE(mModule, "Wrong private msg format, %s", privateMsg.c_str());
            rc = BAD_PROTOCOL;
        } else {
            words = _words;
        }
    }

    if (SUCCEED(rc)) {
        std::stringstream lenss(words[0]);
        std::stringstream formatss(words[1]);
        lenss >> len;
        formatss >> format;
    }

    if (SUCCEED(rc)) {
        rc = mBufMgr.import(&data, fd, len);
        if (FAILED(rc)) {
            LOGE(mModule, "Failed to import fd %d to buf mgr, %d", fd, rc);
        }
    }

    if (SUCCEED(rc)) {
        rc = mCb.send(fd, len, format);
        if (FAILED(rc)) {
            LOGE(mModule, "Failed to send fd %d len %d cb, %d",
                fd, len, rc);
        }
    }

    return rc;
}

int32_t FrameServer::enqueue(int32_t fd)
{
    return NOT_SUPPORTED;
}

int32_t FrameServer::enqueue(void *dat)
{
    return NOT_SUPPORTED;
}

int32_t FrameServer::enqueue(void *dat, int32_t format)
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

    if (SUCCEED(rc)) {
        rc = mBufMgr.release(dat);
        if (FAILED(rc)) {
            LOGE(mModule, "Failed to release buf mgr, %d", rc);
        }
    }

    return rc;
}

FrameServer::FrameServer(CallbackIntf *cb) :
    RequestHandler(FRAME, "rquest handler for frame server", cb),
    Identifier(MODULE_FRAME_SERVER, "FrameServer", "1.0.0")
{
}

FrameServer::~FrameServer()
{
}

};


#include "FrameClient.h"

namespace voyager {

int32_t FrameClient::send(void *dat, int64_t len)
{
    return NOT_SUPPORTED;
}

int32_t FrameClient::send(int32_t fd, int64_t len)
{
    return RequestHandler::send(fd, len);
}

int32_t FrameClient::send(int32_t fd, int64_t len, int32_t format)
{
    return NOT_SUPPORTED;
}

int32_t FrameClient::send(int32_t event, int32_t arg1, int32_t arg2)
{
    return NOT_SUPPORTED;
}

FrameClient::FrameClient(const std::string &name) :
    RequestHandler(FRAME, name)
{
}

FrameClient::~FrameClient()
{
}

};


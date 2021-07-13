#include "EventClient.h"

namespace voyager {

int32_t EventClient::send(void *dat, int64_t len)
{
    return NOT_SUPPORTED;
}

int32_t EventClient::send(int32_t fd, int64_t len)
{
    return NOT_SUPPORTED;
}

int32_t EventClient::send(int32_t fd, int64_t len, int32_t format)
{
    return NOT_SUPPORTED;
}

int32_t EventClient::send(int32_t event, int32_t arg1, int32_t arg2)
{
    return RequestHandler::send(event, arg1, arg2);
}

EventClient::EventClient(const std::string &name) :
    RequestHandler(EVENT, name)
{
}

EventClient::~EventClient()
{
}

};


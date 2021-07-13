#include "DataClient.h"

namespace voyager {

int32_t DataClient::send(void *dat, int64_t len)
{
    return RequestHandler::send(dat, len);
}

int32_t DataClient::send(int32_t fd, int64_t len)
{
    return NOT_SUPPORTED;
}

int32_t DataClient::send(int32_t fd, int64_t len, int32_t format)
{
    return NOT_SUPPORTED;
}

int32_t DataClient::send(int32_t event, int32_t arg1, int32_t arg2)
{
    return NOT_SUPPORTED;
}

DataClient::DataClient(const std::string &name) :
    RequestHandler(DATA, name)
{
}

DataClient::~DataClient()
{
}

};


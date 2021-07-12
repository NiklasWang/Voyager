#include "ClientIntfImpl.h"

namespace voyager {

static const char * const gRequestTypeStr[] = {
    [DATA]  = "data",
    [FD]    = "file_descriptor",
    [FRAME] = "frame",
    [EVENT] = "event",
    [REQUEST_TYPE_MAX_INVALID] = "MAX_INVALID",
};

bool checkValid(RequestType type)
{
    bool rc = false;

    if (type >= 0 && type < REQUEST_TYPE_MAX_INVALID) {
        rc = true;
    }

    return rc;
}

RequestType getValidType(RequestType type)
{
    return checkValid(type) ? type : REQUEST_TYPE_MAX_INVALID;
}

const char *getRequestName(RequestType type)
{
    return gRequestTypeStr[getValidType(type)];
}

};

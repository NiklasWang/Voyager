#ifndef _REQUEST_HANDLER_INTF_H_
#define _REQUEST_HANDLER_INTF_H_

#include "ServerIntf.h"

namespace voyager {

class RequestHandlerIntf {
public:

    virtual int32_t enqueue(int32_t id) = 0;
    virtual int32_t abort() = 0;
    virtual int32_t onClientReady(int32_t clientfd, std::string privateMsg) = 0;

    virtual RequestType getType() = 0;
    virtual const char *getName() = 0;

    virtual int32_t construct() = 0;
    virtual int32_t destruct() = 0;

public:
    virtual ~RequestHandlerIntf() {}
};

};

#endif

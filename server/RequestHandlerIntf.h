#ifndef _REQUEST_HANDLER_INTF_H_
#define _REQUEST_HANDLER_INTF_H_

#include "ServerIntf.h"

namespace voyager {

class RequestHandlerIntf {
public:

    virtual int32_t onClientReady(const std::string &serverName, Semaphore &serverReadySem) = 0;
    virtual int32_t enqueue(void *dat) = 0;
    virtual int32_t enqueue(int32_t fd) = 0;
    virtual int32_t enqueue(void *dat, int32_t format) = 0;
    virtual int32_t abort() = 0;

    virtual RequestType getType() = 0;
    virtual const char *getName() = 0;

public:
    virtual ~RequestHandlerIntf() {}
};

};

#endif

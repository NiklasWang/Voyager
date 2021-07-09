#ifndef _VOYAGER_CLIENT_INTF_H_
#define _VOYAGER_CLIENT_INTF_H_

#include <stdint.h>

namespace voyager {

enum RequestType {
    DATA,
    FD,
    FRAME,
    EVENT,
    REQUEST_TYPE_MAX_INVALID,
};

class ClientIntf {
public:
    virtual int32_t send(void *dat, int64_t len) = 0;
    virtual int32_t send(int32_t fd, int64_t len) = 0;
    virtual int32_t send(void *dat, int64_t len, int32_t format) = 0;
    virtual int32_t send(int32_t event, int32_t arg1, int32_t arg2) = 0;
    virtual bool    requested(RequestType type) = 0;

public:
    virtual ~ClientIntf();
};

};

#endif

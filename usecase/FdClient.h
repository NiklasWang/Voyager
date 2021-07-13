#ifndef _FD_CLIENT_H_
#define _FD_CLIENT_H_

#include "ClientRequestHandler.h"

namespace voyager {

class FdClient :
    public RequestHandler,
    virtual public Identifier,
    virtual public noncopyable {
protected:

    virtual int32_t send(void *dat, int64_t len) override;
    virtual int32_t send(int32_t fd, int64_t len) override;
    virtual int32_t send(int32_t fd, int64_t len, int32_t format) override;
    virtual int32_t send(int32_t event, int32_t arg1, int32_t arg2) override;

public:
    FdClient(const std::string &name);
    virtual ~FdClient();
};

};

#endif


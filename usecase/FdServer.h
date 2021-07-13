#ifndef _FD_SERVER_H_
#define _FD_SERVER_H_

#include "ServerRequestHandler.h"

namespace voyager {

class FdServer :
    public RequestHandler,
    public Identifier,
    public noncopyable {
protected:

    virtual int32_t onClientSent(int32_t fd, const std::string &privateMsg) override;

public:
    virtual int32_t enqueue(int32_t fd) override;
    virtual int32_t enqueue(void *dat) override;
    virtual int32_t enqueue(void *dat, int32_t format) override;

public:
    FdServer(CallbackIntf *cb);
    virtual ~FdServer();
};

};

#endif


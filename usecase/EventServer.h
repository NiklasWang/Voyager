#ifndef _FD_SERVER_H_
#define _FD_SERVER_H_

#include "RequestHandler.h"

namespace voyager {

class EventServer :
    public RequestHandler,
    public Identifier,
    public noncopyable {
protected:

    virtual int32_t startServerLoop(int32_t clientfd, const std::string &privateMsg);
    virtual int32_t onClientSent(int32_t fd, const std::string &privateMsg) override;

public:
    virtual int32_t enqueue(int32_t fd) override;
    virtual int32_t enqueue(void *dat) override;
    virtual int32_t enqueue(void *dat, int32_t format) override;

public:
    EventServer(CallbackIntf *cb);
    virtual ~EventServer();
};

};

#endif


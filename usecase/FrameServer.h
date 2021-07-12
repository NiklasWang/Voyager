#ifndef _FRAME_SERVER_H_
#define _FRAME_SERVER_H_

#include "RequestHandler.h"

namespace voyager {

class FrameServer :
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
    FrameServer(CallbackIntf *cb);
    virtual ~FrameServer();
};

};

#endif

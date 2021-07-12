#ifndef _VOYAGER_SERVER_H_
#define _VOYAGER_SERVER_H_

#include <string>

#include "ServerIntf.h"

namespace voyager {

class ServerImpl;

class Server :
    public ServerIntf {
public:

    virtual int32_t request(DataCbFunc dataCbFunc, SyncMode mode = ASYNC) override;
    virtual int32_t enqueue(void *dat) override;
    virtual int32_t request(FdCbFunc fdCbFunc, SyncMode mode = ASYNC) override;
    virtual int32_t enqueue(int32_t fd) override;
    virtual int32_t request(FrameCbFunc frameCbFunc, SyncMode mode = ASYNC) override;
    virtual int32_t enqueue(void *dat, int32_t format) override;
    virtual int32_t request(EventCbFunc eventCbFunc, SyncMode mode = ASYNC) override;
    virtual int32_t cancel(RequestType type) override;

public:
    Server(const char *name, bool enableOverallControl = true);
    virtual ~Server();

private:
    Server(const Server &rhs);
    Server &operator=(const Server &rhs);

private:
    std::string mName;
    bool        mEnableOverallControl;
    ServerImpl *mImpl;
};

};

#endif

#ifndef _VOYAGER_CLIENT_CORE_H_
#define _VOYAGER_CLIENT_CORE_H_

#include "Common.h"
#include "ClientIntf.h"
#include "BufferMgr.h"
#include "OverallControlSingleton.h"
#include "SocketClientStateMachine.h"
#include "ClientRequestHandler.h"

namespace voyager {

class ClientCore :
    public ClientIntf,
    public Identifier,
    public noncopyable {
public:

    virtual int32_t send(void *dat, int64_t len) override;
    virtual int32_t send(int32_t fd, int64_t len) override;
    virtual int32_t send(int32_t fd, int64_t len, int32_t format) override;
    virtual int32_t send(int32_t event, int32_t arg1, int32_t arg2) override;
    virtual bool    requested(RequestType type) override;

public:
    int32_t construct();
    int32_t destruct();
    ClientCore(const std::string &name);
    virtual ~ClientCore();

private:
    int32_t connectServer();
    int32_t importOverallControl();
    int32_t createHandlerIfRequired(RequestType type);
    RequestHandler *createHandler(RequestType type, const std::string &name);
    int32_t setOverallControlMemory(int32_t fd);

private:
    std::string mName;
    bool        mConstructed;
    bool        mConnected;
    SocketClientStateMachine mSC;
    bool        mSkipOverallControl;
    OverallControlSingleton *mOverallControlSingleton;
    RequestHandler *mRequests[REQUEST_TYPE_MAX_INVALID];
};

};

#endif

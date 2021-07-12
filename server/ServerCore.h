#ifndef _VOYAGER_SERVER_CORE_H_
#define _VOYAGER_SERVER_CORE_H_

#include "common.h"
#include "ServerIntf.h"
#include "OverallControlSingleton.h"
#include "SocketServerStateMachine.h"
#include "CallbackThread.h"
#include "RequestHandler.h"
#include "ThreadPoolEx.h"

namespace voyager {

class ServerCore :
    public ServerIntf,
    public CallbackIntf,
    public Identifier,
    public noncopyable {
public:
    virtual int32_t request(DataCbFunc dataCbFunc, SyncMode mode) override;
    virtual int32_t enqueue(void *dat) override;
    virtual int32_t request(FdCbFunc fdCbFunc, SyncMode mode) override;
    virtual int32_t enqueue(int32_t fd) override;
    virtual int32_t request(FrameCbFunc frameCbFunc, SyncMode mode) override;
    virtual int32_t enqueue(void *dat, int32_t format) override;
    virtual int32_t request(EventCbFunc eventCbFunc, SyncMode mode) override;
    virtual int32_t cancel(RequestType type) override;

public:
    int32_t send(void *dat, int64_t len) override;
    int32_t send(int32_t fd, int64_t len) override;
    int32_t send(void *dat, int64_t len, int32_t format) override;
    int32_t send(int32_t event, int32_t arg1, int32_t arg2) override;

public:
    ServerCore();
    virtual ~ServerCore();
    int32_t construct(const std::string &name, bool enableOverallControl);
    int32_t destruct();

private:
    int32_t startServerLoop();
    int32_t exitServerLoop();
    int32_t shareOverallControl();
    int32_t replyClientRequestNotRequested();
    int32_t request(RequestType type);
    template <typename T>
    int32_t request(T cbFunc, RequestType type);
    int32_t createRequestHandler(RequestType type);
    RequestHandler *createHandler(RequestType type);
    int32_t abort(RequestType type);
    int32_t enableAllRequestedRequests();
    int32_t revealRequestTypeAndPrivateArgFromMsg(
        char *msg, RequestType &type, std::string &privateArg);
    bool    validFd(int32_t fd);
    bool    requested(RequestType type);

private:
    bool                     mConstructed;
    std::string              mName;
    bool                     mEnableOverallControl;
    bool                     mClientReady;
    bool                     mCachedRequest[REQUEST_TYPE_MAX_INVALID];
    OverallControlSingleton *mOverallControl;
    int32_t                  mOverallControlFd;
    SocketServerStateMachine mSS;
    char                     mSocketMsg[SOCKET_DATA_MAX_LEN];
    CallbackThread           mCb;
    ThreadPoolEx            *mThreads;
    RequestHandlerIntf      *mRequests[REQUEST_TYPE_MAX_INVALID];
    bool                     mCachedRequest[REQUEST_TYPE_MAX_INVALID];
};

};

#endif

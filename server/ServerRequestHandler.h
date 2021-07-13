#ifndef _REQUEST_HANDLER_H_
#define _REQUEST_HANDLER_H_

#include <map>

#include "Common.h"
#include "ServerRequestHandlerIntf.h"
#include "ServerIntf.h"
#include "CallbackThread.h"
#include "ThreadPoolEx.h"
#include "SocketServerStateMachine.h"
#include "BufferMgr.h"

namespace voyager {

class RequestHandler :
    public RequestHandlerIntf,
    virtual public Identifier,
    virtual public noncopyable {
public:

    virtual int32_t onClientReady(const std::string &serverName, Semaphore &serverReadySem) override;
    virtual int32_t enqueue(int32_t fd) override;
    virtual int32_t enqueue(void *dat) = 0;
    virtual int32_t enqueue(void *dat, int32_t format) = 0;
    virtual int32_t abort() override;

    virtual RequestType getType() override;
    virtual const char *getName() override;

public:
    virtual int32_t construct();
    virtual int32_t destruct();
    RequestHandler(RequestType type, CallbackIntf *cb);
    virtual ~RequestHandler();

protected:
    virtual int32_t onClientSent(int32_t fd, const std::string &privateMsg) = 0;
    virtual int32_t startServerLoop(Semaphore &clientReadySem);
    virtual int32_t exitServerLoop();
    int32_t revealPrivateArgFromMsg(const char *msg, std::string &privateArg);
    bool checkFdExists(int32_t fd);
    bool checkFdExists(void *ptr);
    void addFdRecord(int32_t fd);
    void addFdRecord(int32_t fd, void *ptr);
    int32_t removeFdRecord(int32_t fd);
    int32_t removeFdRecord(void *ptr);

protected:
    bool          mConstructed;
    std::string   mName;
    RequestType   mType;
    CallbackIntf *mCb;
    ThreadPoolEx *mThreads;
    SocketServerStateMachine  mSSSM;
    BufferMgr     mBufMgr;
    std::map<int32_t, void *> mFds;
};

};

#endif


#ifndef _CHECK_POINT_LEGACY_H_
#define _CHECK_POINT_LEGACY_H_

#include <list>
#include <functional>

#include "Identifier.h"
#include "Times.h"
#include "Semaphore.h"
#include "SemaphoreTimeout.h"
#include "CheckPointInterface.h"

namespace pandora {

class ThreadPoolEx;

namespace legacy {

class CheckPoint :
    virtual public CheckPointInterface,
    public Identifier {
public:

    int32_t request(const std::string &name) override;
    int32_t request(bool fmt, const char *format, ...) override;
    int32_t requestBlur(const std::string &name) override;
    int32_t requestNone(const std::string &name) override;
    int32_t requestNoneBlur(const std::string &name) override;

    int32_t reach(const std::string &name) override;
    int32_t reach(const char *format, ...) override;

    int32_t track(std::function<int32_t ()> func, bool ignoreFuncReturn = false) override;
    int32_t dump() override;
    void    clear() override;

public:
    CheckPoint();
    virtual ~CheckPoint();

private:
    int32_t request(const std::string &name, bool blur, bool none);
    int32_t trackRandomOrder(Semaphore &sem, int32_t &cnt);

private:
    struct TrackNode {
        std::string name;
        bool blur;
        bool none;
        Semaphore sem;
        Semaphore recordedSem;
        Semaphore allDoneSem;
        bool cancelled;
        TrackNode(const std::string name);
    };

private:
    ThreadPoolEx   *mThreads;
    int32_t         mFuncRc;
    std::list<std::pair<std::string, TrackNode *> > mList;
    std::list<std::pair<std::string, std::string> > mExceptionCheckPoints;
};

};

};

#endif

#ifndef _CHECK_POINT_H_
#define _CHECK_POINT_H_

#include <list>
#include <functional>

#include "Identifier.h"
#include "CheckPointInterface.h"

namespace pandora {

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
    int32_t trackRandomOrder();

private:
    struct TrackNode {
        std::string name;
        bool blur;
        TrackNode(const std::string name);
    };

private:
    int32_t mFuncRc;
    std::list<std::pair<std::string, TrackNode *> > mList;
    std::list<std::pair<std::string, TrackNode *> > mNoneList;
    std::list<std::pair<std::string, std::string> > mExceptionalCheckPoints;
};

};

#endif

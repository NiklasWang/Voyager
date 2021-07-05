#ifndef _CHECK_POINTS_IMPL_H_
#define _CHECK_POINTS_IMPL_H_

#include <map>
#include <functional>

#include "Identifier.h"
#include "RefBase.h"
#include "CheckPointInterface.h"
#include "CheckPoint.h"
#include "CheckPointOrderFixed.h"

namespace pandora {

enum CheckPointType {
    CHECK_POINT_TYPE_RANDOM_ORDER,
    CHECK_POINT_TYPE_FIXED_ORDER,
    CHECK_POINT_TYPE_MAX_INVALID,
};

class CheckPointsImpl :
    public Identifier {
public:

    int32_t request(CheckPointType type, const std::string &name);
    int32_t request(CheckPointType type, bool fmt, const char *format, ...);
    int32_t requestBlur(CheckPointType type, const std::string &name);
    int32_t requestNone(CheckPointType type, const std::string &name);
    int32_t requestNoneBlur(CheckPointType type, const std::string &name);

    int32_t reach(CheckPointType type, const std::string &name);
    int32_t reach(CheckPointType type, const char *format, ...);

    int32_t track(CheckPointType type, std::function<int32_t ()> func, bool ignoreFuncReturn = false);
    int32_t dump(CheckPointType type);
    void    clear(CheckPointType type);
    void    clearAll();
public:
    CheckPointsImpl();
    virtual ~CheckPointsImpl();

private:
    class LocalCheckPointInterface :
        virtual public CheckPointInterface,
        public RefBase {
    };

    class LocalCheckPoint :
        public CheckPoint,
        public LocalCheckPointInterface {
    };

    class LocalOrderFixedCheckPoint :
        public CheckPointOrderFixed,
        public LocalCheckPointInterface {
    };

private:
    sp<LocalCheckPointInterface> getOrCreateLocalCheckPoint();
    sp<LocalCheckPointInterface> getOrCreateLocalOrderFixedCheckPoint();
    sp<LocalCheckPointInterface> getOrCreateLocalCheckPoint(CheckPointType type);

private:
    std::map<std::size_t, sp<LocalCheckPointInterface> > mCheckPoints;
    std::map<std::size_t, sp<LocalCheckPointInterface> > mOrderFixedCheckPoints;
};

};

#endif

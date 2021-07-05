#ifndef _CHECK_POINT_INTERFACE_H_
#define _CHECK_POINT_INTERFACE_H_

#include <stdint.h>

namespace pandora {


class CheckPointInterface
{
public:

    virtual int32_t request(const std::string &name) = 0;
    virtual int32_t request(bool fmt, const char *format, ...) = 0;
    virtual int32_t requestBlur(const std::string &name) = 0;
    virtual int32_t requestNone(const std::string &name) = 0;
    virtual int32_t requestNoneBlur(const std::string &name) = 0;

    virtual int32_t reach(const std::string &name) = 0;
    virtual int32_t reach(const char *format, ...) = 0;

    virtual int32_t track(std::function<int32_t ()> func, bool ignoreFuncReturn = false) = 0;
    virtual int32_t dump() = 0;
    virtual void    clear() = 0;

public:
    virtual ~CheckPointInterface() = default;
};

};

#endif

#include "Times.h"

namespace voyager {

Times::S::S(int32_t s) :
    value(s)
{
}

Times::S::S(const Times::S &s) :
    value(s.value)
{
}

Times::S::S(const Times::Ms &ms) :
    value(ms() / 1000)
{
}

Times::S::S(const Times::Us &us) :
    Times::S(Times::Ms(us()))
{
}

Times::S::S(const Times::Ns &ns) :
    Times::S(Times::Ms(ns()))
{
}

Times::S &Times::S::operator=(const int32_t s)
{
    value = s;
    return *this;
}

Times::S &Times::S::operator=(const Times::S &s)
{
    if (this != &s) {
        value = s.value;
    }

    return *this;
}

Times::S &Times::S::operator=(const Times::Ms &ms)
{
    value = ms() / 1000;
    return *this;
}

Times::S &Times::S::operator=(const Times::Us &us)
{
    *this = Times::Ms(us());
    return *this;
}

Times::S &Times::S::operator=(const Times::Ns &ns)
{
    *this = Times::Ms(ns());
    return *this;
}

int32_t Times::S::operator()() const
{
    return value;
}

Times::Ms::Ms(int32_t ms) :
    value(ms)
{
}

Times::Ms::Ms(const Times::S &s) :
    Times::Ms(Times::Us(s()))
{
}

Times::Ms::Ms(const Times::Ms &s) :
    value(s.value)
{
}

Times::Ms::Ms(const Times::Us &us) :
    Times::Ms(us() / 1000)
{
}

Times::Ms::Ms(const Times::Ns &ns) :
    Times::Ms(Times::Us(ns()))
{
}

Times::Ms &Times::Ms::operator=(const int32_t ms)
{
    value = ms;
    return *this;
}

Times::Ms &Times::Ms::operator=(const Times::S &s)
{
    *this = Times::Us(s());
    return *this;
}

Times::Ms &Times::Ms::operator=(const Times::Ms &ms)
{
    if (this != &ms) {
        value = ms.value;
    }

    return *this;
}

Times::Ms &Times::Ms::operator=(const Times::Us &us)
{
    value = us() / 1000;
    return *this;
}

Times::Ms &Times::Ms::operator=(const Times::Ns &ns)
{
    *this = Times::Us(ns());
    return *this;
}

int32_t Times::Ms::operator()() const
{
    return value;
}

Times::Us::Us(int32_t us) :
    value(us)
{
}

Times::Us::Us(const Times::S &s) :
    Times::Us(Times::Ns(s()))
{
}

Times::Us::Us(const Times::Ms &ms) :
    Times::Us(Times::Ns(ms()))
{
}

Times::Us::Us(const Times::Us &s) :
    value(s.value)
{
}

Times::Us::Us(const Times::Ns &ns) :
    Times::Us(ns() / 1000)
{
}

Times::Us &Times::Us::operator=(const int32_t us)
{
    value = us;
    return *this;
}

Times::Us &Times::Us::operator=(const Times::S &s)
{
    *this = Times::Ns(s());
    return *this;
}

Times::Us &Times::Us::operator=(const Times::Ms &ms)
{
    *this = Times::Ns(ms());
    return *this;
}

Times::Us &Times::Us::operator=(const Times::Us &us)
{
    if (this != &us) {
        value = us.value;
    }

    return *this;
}

Times::Us &Times::Us::operator=(const Times::Ns &ns)
{
    value = ns() / 1000;
    return *this;
}

int32_t Times::Us::operator()() const
{
    return value;
}

Times::Ns::Ns(int64_t ns) :
    value(ns)
{
}

Times::Ns::Ns(const S &s) :
    Times::Ns(Times::Us(s()))
{
}

Times::Ns::Ns(const Ms &ms) :
    Times::Ns(Times::Us(ms()))
{
}

Times::Ns::Ns(const Us &ns) :
    Times::Ns(ns() * 1000LL)
{
}

Times::Ns::Ns(const Ns &s) :
    value(s.value)
{
}

Times::Ns &Times::Ns::operator=(const int64_t ns)
{
    value = ns;
    return *this;
}

Times::Ns &Times::Ns::operator=(const Times::S &s)
{
    *this = Times::Us(s());
    return *this;
}

Times::Ns &Times::Ns::operator=(const Times::Ms &ms)
{
    *this = Times::Us(ms());
    return *this;
}

Times::Ns &Times::Ns::operator=(const Times::Us &us)
{
    value = us() * 1000LL;
    return *this;
}

Times::Ns &Times::Ns::operator=(const Times::Ns &ns)
{
    if (this != &ns) {
        value = ns.value;
    }

    return *this;
}

int64_t Times::Ns::operator()() const
{
    return value;
}

Times::Times(const int32_t ms) :
    Times(Ms(ms))
{
}

Times::Times(const Times::S &_s) :
    s(_s),
    type(TIME_TYPE_S)
{
}

Times::Times(const Times::Ms &_ms) :
    ms(_ms),
    type(TIME_TYPE_MS)
{
}

Times::Times(const Times::Us &_us) :
    us(_us),
    type(TIME_TYPE_US)
{
}

Times::Times(const Times::Ns &_ns) :
    ns(_ns),
    type(TIME_TYPE_NS)
{
}

Times &Times::operator=(const int32_t _ms)
{
    *this = Ms(_ms);

    return *this;
}

Times &Times::operator=(const S &_s)
{
    s    = _s;
    type = TIME_TYPE_S;

    return *this;
}

Times &Times::operator=(const Ms &_ms)
{
    ms   = _ms;
    type = TIME_TYPE_MS;

    return *this;
}

Times &Times::operator=(const Us &_us)
{
    us   = _us;
    type = TIME_TYPE_US;

    return *this;
}

Times &Times::operator=(const Ns &_ns)
{
    ns   = _ns;
    type = TIME_TYPE_NS;

    return *this;
}

Times::Ms Times::operator()() const
{
    Ms result = 0;

    switch (type) {
        case TIME_TYPE_S: {
            result = s;
        }; break;
        case TIME_TYPE_MS: {
            result = ms;
        }; break;
        case TIME_TYPE_US: {
            result = us;
        }; break;
        case TIME_TYPE_NS: {
            result = ns;
        }; break;
        default: {
        }; break;
    }

    return result;
}

};


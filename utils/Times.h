#ifndef _UTILS_TIME_H_
#define _UTILS_TIME_H_

#include <stdint.h>

namespace pandora {

class Times {
public:
    struct S;
    struct Ms;
    struct Us;
    struct Ns;

    struct S {
        S(int32_t s = 0);
        S(const S &s);
        S(const Ms &ms);
        S(const Us &us);
        S(const Ns &ns);
        S &operator=(const int32_t s);
        S &operator=(const S &s);
        S &operator=(const Ms &ms);
        S &operator=(const Us &us);
        S &operator=(const Ns &ns);
        int32_t operator()() const;

    private:
        int32_t value;
    };

    struct Ms {
        Ms(int32_t ms = 0);
        Ms(const S &s);
        Ms(const Ms &s);
        Ms(const Us &us);
        Ms(const Ns &ns);
        Ms &operator=(const int32_t ms);
        Ms &operator=(const S &s);
        Ms &operator=(const Ms &ms);
        Ms &operator=(const Us &us);
        Ms &operator=(const Ns &ns);
        int32_t operator()() const;

    private:
        int32_t value;
    };

    struct Us {
        Us(int32_t us = 0);
        Us(const S &s);
        Us(const Ms &ms);
        Us(const Us &s);
        Us(const Ns &ns);
        Us &operator=(const int32_t us);
        Us &operator=(const S &s);
        Us &operator=(const Ms &ms);
        Us &operator=(const Us &us);
        Us &operator=(const Ns &ns);
        int32_t operator()() const;

    private:
        int32_t value;
    };

    struct Ns {
        Ns(int64_t ns = 0LL);
        Ns(const S &s);
        Ns(const Ms &ms);
        Ns(const Us &ns);
        Ns(const Ns &s);
        Ns &operator=(const int64_t ns);
        Ns &operator=(const S &s);
        Ns &operator=(const Ms &ms);
        Ns &operator=(const Us &us);
        Ns &operator=(const Ns &ns);
        int64_t operator()() const;

    private:
        int64_t value;
    };

public:
    Times(const int32_t ms);
    Times(const S &s);
    Times(const Ms &ms);
    Times(const Us &us);
    Times(const Ns &ns);
    Times &operator=(const int32_t ms);
    Times &operator=(const S &s);
    Times &operator=(const Ms &ms);
    Times &operator=(const Us &us);
    Times &operator=(const Ns &ns);
    Ms operator()() const;

private:
    enum TimeType {
        TIME_TYPE_S,
        TIME_TYPE_MS,
        TIME_TYPE_US,
        TIME_TYPE_NS,
        TIME_TYPE_MAX_INVALID,
    };

private:
    S  s;
    Ms ms;
    Us us;
    Ns ns;
    TimeType type;
};

};

#endif

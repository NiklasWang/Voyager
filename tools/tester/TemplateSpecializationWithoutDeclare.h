#ifndef _TEMPLATE_SPEC_WO_DECLARE_H_
#define _TEMPLATE_SPEC_WO_DECLARE_H_

#include <stdint.h>

class TestClass {
public:
    template <typename T>
    void run(const T &t);
};

class Test {
public:
    int32_t value;
};

#endif

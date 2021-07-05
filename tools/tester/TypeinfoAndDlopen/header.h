#ifndef _TYPEINFO_AND_DLOPEN_H_
#define _TYPEINFO_AND_DLOPEN_H_

#include <stdint.h>

typedef int32_t _INT32_T;
typedef float   _FLOAT_T;
typedef int64_t _INT64_T;

struct StructA {
    _INT32_T reserveStructA;
};

struct StructB {
    _INT64_T reserveStructB;
};

struct StructC :
    public StructB {
};

class ClassA {
    _FLOAT_T reserveClassA;
};

class ClassB {
    _INT32_T reserveClassB;
};

class ClassC :
    public ClassB {
    _INT32_T reserveClassB;
};

typedef StructA _STRUCTA_T;
typedef StructB _STRUCTB_T;
typedef StructC _STRUCTC_T;
typedef ClassA _CLASSA_T;
typedef ClassB _CLASSB_T;
typedef ClassC _CLASSC_T;

typedef void (*PrintTemplateFunc)(void *value);

#endif

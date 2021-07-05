#include <iostream>
#include <typeinfo>
#include <map>

#include "header.h"

template <typename T>
void printTypeInfoTemplate(T *value)
{
    std::cout << "T "
        << typeid(T).name() << " "
        << typeid(*value).name() << std::endl;

    return;
}

static std::map<std::size_t, PrintTemplateFunc> gPrintTemplateFuncs;

extern "C"
std::map<std::size_t, PrintTemplateFunc> getPrintTemplateFuncs()
{
    gPrintTemplateFuncs[typeid(_INT32_T).hash_code()] = (PrintTemplateFunc)&printTypeInfoTemplate<_INT32_T>;
    gPrintTemplateFuncs[typeid(_INT64_T).hash_code()] = (PrintTemplateFunc)&printTypeInfoTemplate<_INT64_T>;
    gPrintTemplateFuncs[typeid(_FLOAT_T).hash_code()] = (PrintTemplateFunc)&printTypeInfoTemplate<_FLOAT_T>;
    gPrintTemplateFuncs[typeid(StructA).hash_code()] = (PrintTemplateFunc)&printTypeInfoTemplate<StructA>;
    gPrintTemplateFuncs[typeid(StructB).hash_code()] = (PrintTemplateFunc)&printTypeInfoTemplate<StructB>;
    gPrintTemplateFuncs[typeid(StructC).hash_code()] = (PrintTemplateFunc)&printTypeInfoTemplate<StructC>;
    gPrintTemplateFuncs[typeid(_CLASSA_T).hash_code()] = (PrintTemplateFunc)&printTypeInfoTemplate<_CLASSA_T>;
    gPrintTemplateFuncs[typeid(_CLASSB_T).hash_code()] = (PrintTemplateFunc)&printTypeInfoTemplate<_CLASSB_T>;
    gPrintTemplateFuncs[typeid(_CLASSC_T).hash_code()] = (PrintTemplateFunc)&printTypeInfoTemplate<_CLASSC_T>;

    return gPrintTemplateFuncs;
}

extern "C"
void printTypeInfo()
{
    _INT32_T v_int32_t;
    _INT64_T v_int64_t;
    _FLOAT_T v_float_t;
    StructA v_structa;
    StructB v_structb;
    StructC v_structc;
    _CLASSA_T v_classa;
    _CLASSB_T v_classb;
    _CLASSC_T v_classc;

    std::cout << "int32_t "
        << typeid(int32_t).name() << " "
        << typeid(_INT32_T).name() << " "
        << typeid(v_int32_t).name() << std::endl;
    std::cout << "int64_t "
        << typeid(int64_t).name() << " "
        << typeid(_INT64_T).name() << " "
        << typeid(v_int64_t).name() << std::endl;
    std::cout << "float "
        << typeid(float).name() << " "
        << typeid(_FLOAT_T).name() << " "
        << typeid(v_float_t).name() << std::endl;

    std::cout << "StructA "
        << typeid(StructA).name() << " "
        << typeid(_STRUCTA_T).name() << " "
        << typeid(v_structa).name() << std::endl;
    std::cout << "StructB "
        << typeid(StructB).name() << " "
        << typeid(_STRUCTB_T).name() << " "
        << typeid(v_structb).name() << std::endl;
    std::cout << "StructC "
        << typeid(StructC).name() << " "
        << typeid(_STRUCTC_T).name() << " "
        << typeid(v_structc).name() << std::endl;

    std::cout << "ClassA "
        << typeid(ClassA).name() << " "
        << typeid(_CLASSA_T).name() << " "
        << typeid(v_classa).name() << std::endl;
    std::cout << "ClassB "
        << typeid(ClassB).name() << " "
        << typeid(_CLASSB_T).name() << " "
        << typeid(v_classb).name() << std::endl;
    std::cout << "ClassC "
        << typeid(ClassC).name() << " "
        << typeid(_CLASSC_T).name() << " "
        << typeid(v_classc).name() << std::endl;

    return;
}


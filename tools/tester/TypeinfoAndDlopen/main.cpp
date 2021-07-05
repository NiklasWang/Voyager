#include <iostream>
#include <typeinfo>
#include <map>

#include <dlfcn.h>

#include "header.h"

using namespace std;

int32_t main(int argc,char *argv[])
{
    int32_t rc = 0;
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

    void *lib = dlopen("./libTypeinfoInDlopen.so", RTLD_LAZY);
    if (lib == nullptr) {
        std::cout << "Failed to dlopen lib " << dlerror() << std::endl;
        return -1;
    }

    typedef void (*printFunc)();
    printFunc func = (printFunc)dlsym(lib, "printTypeInfo");
    const char* dlsymError = dlerror();
    if (dlsymError != nullptr) {
        std::cout << "Failed to find func printTypeInfo()" << std::endl;
        return -1;
    }

    func();

    std::cout << "-------------------------------" << std::endl;

    typedef std::map<std::size_t, PrintTemplateFunc> (*getPrintTemplateFunc)();
    getPrintTemplateFunc func2 = (getPrintTemplateFunc)dlsym(lib, "getPrintTemplateFuncs");
    dlsymError = dlerror();
    if (dlsymError != nullptr) {
        std::cout << "Failed to find func getPrintTemplateFuncs()" << std::endl;
        return -1;
    }

    std::map<std::size_t, PrintTemplateFunc> printTemplateFuncMap;
    printTemplateFuncMap = func2();
    printTemplateFuncMap[typeid(_INT32_T).hash_code()](&v_int32_t);
    printTemplateFuncMap[typeid(_INT64_T).hash_code()](&v_int64_t);
    printTemplateFuncMap[typeid(_FLOAT_T).hash_code()](&v_float_t);
    printTemplateFuncMap[typeid(_STRUCTA_T).hash_code()](&v_structa);
    printTemplateFuncMap[typeid(_STRUCTB_T).hash_code()](&v_structb);
    printTemplateFuncMap[typeid(_STRUCTC_T).hash_code()](&v_structc);
    printTemplateFuncMap[typeid(_CLASSA_T).hash_code()](&v_classa);
    printTemplateFuncMap[typeid(_CLASSB_T).hash_code()](&v_classb);
    printTemplateFuncMap[typeid(_CLASSC_T).hash_code()](&v_classc);

    dlclose(lib);    

    return rc;
}


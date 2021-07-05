#include <iostream>

#include "TemplateSpecializationWithoutDeclare.h"

template <typename T>
void TestClass::run(const T &t)
{
    std::cout << t << std::endl;
}

/*
 TemplateSpecializationWithoutDeclare.cpp:12:6: error: template-id ‘run<>’ for ‘void TestClass::run(const Test*)
 ’ does not match any template declaration
    12 | void TestClass::run(const Test *t)
       |      ^~~~~~~~~
 In file included from TemplateSpecializationWithoutDeclare.cpp:3:
 TemplateSpecializationWithoutDeclare.h:9:10: note: candidate is: ‘template<class T> void TestClass::run(const T&)’
     9 |     void run(const T &t);
       |          ^~~

template <>
void TestClass::run(const Test *t)
{
    std::cout << t.value << std::endl;
}
*/

int main(int argc,char *argv[])
{
    TestClass testClass;
    /*Test test;
    test.value = 1;*/

    testClass.run(int(0));
    /* testClass.run(test); */

    return 0;
}


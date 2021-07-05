#include <typeinfo>
#include <iostream>

namespace tester {

struct A {
};

struct B {
    virtual ~B() {}
};

struct C :
    public A {
};

struct D :
    public B {
};

int _main()
{
    A a;
    B b;
    C c;
    D d;
    std::cout << "----------------------------------------"  << std::endl;
    std::cout << "typeid(a).name = " << typeid(a).name() << std::endl;
    std::cout << "typeid(b).name = " << typeid(b).name() << std::endl;
    std::cout << "typeid(c).name = " << typeid(c).name() << std::endl;
    std::cout << "typeid(d).name = " << typeid(d).name() << std::endl;

    A *pa1 = &a;
    B *pb1 = &b;
    A *pc1 = &c;
    B *pd1 = &d;
    std::cout << "----------------------------------------"  << std::endl;
    std::cout << "typeid(pa1).name = "  << typeid(pa1).name()  << std::endl;
    std::cout << "typeid(*pa1).name = " << typeid(*pa1).name() << std::endl;
    std::cout << "typeid(*pb1).name = " << typeid(*pb1).name() << std::endl;
    std::cout << "typeid(*pc1).name = " << typeid(*pc1).name() << std::endl;
    std::cout << "typeid(*pd1).name = " << typeid(*pd1).name() << std::endl;

    A *pa2 = new A();
    B *pb2 = new B();
    A *pc2 = new C();
    B *pd2 = new D();
    std::cout << "----------------------------------------"  << std::endl;
    std::cout << "typeid(pa2).name = "  << typeid(pa2).name()  << std::endl;
    std::cout << "typeid(*pa2).name = " << typeid(*pa2).name() << std::endl;
    std::cout << "typeid(*pb2).name = " << typeid(*pb2).name() << std::endl;
    std::cout << "typeid(*pc2).name = " << typeid(*pc2).name() << std::endl;
    std::cout << "typeid(*pd2).name = " << typeid(*pd2).name() << std::endl;
    delete pa2;
    delete pb2;
    delete pc2;
    delete pd2;

    return 0;
}

}

int main()
{
    return tester::_main();
}


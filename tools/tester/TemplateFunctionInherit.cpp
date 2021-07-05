#include <iostream>

struct A {
public:
    template <typename T>
    int32_t action(const T &t) {
        std::cout << "Class A, t=" << t << std::endl;
        return 0;
    }
    /* Following block will raise compile error:
     * templates may not be ‘virtual’
    template <typename T>
    virtual int32_t actionB(const T &t) {
        std::cout << "Class A, virtual actionB, t=" << t << std::endl;
        return 0;
    }
     */
};

struct B :
    public A {
public:
    template <typename T>
    int32_t action(const T &t) {
        std::cout << "Class B, t=" << t << std::endl;
        return 0;
    }
};

int main(int argc,char *argv[])
{
    B b;
    A *a = &b;
    a->action(int(1));
    b.action(int(2));

    return 0;
}

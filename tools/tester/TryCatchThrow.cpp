#include <iostream>

#include <stdint.h>

void OccurError()
{
    throw -1;
}

void CallOccurError()
{
    OccurError();
}

int main(int argc,char *argv[])
{
    try {
        CallOccurError();
    } catch (int32_t err) {
        std::cout << "Catched int32_t error : " << err << std::endl;
    } catch (...) {
        std::cout << "Catched unknown error." << std::endl;
    }

    std::cout << "Exit." << std::endl;

    return 0;
}


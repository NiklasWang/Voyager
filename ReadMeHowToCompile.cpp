#include "iostream"

int main(int argc,char *argv[])
{
    std::cout <<
        "Compile Instructions:" << std::endl <<
        "  Install make utility and compile toolchain accordingly." << std::endl <<
        std::endl <<
        "- For Linux:" << std::endl <<
        "run `make`" << std::endl <<
        std::endl <<
        "- Android NDK:" << std::endl <<
        "1, Download NDK toolchain, https://dl.google.com/android/repository/android-ndk-r21e-linux-x86_64.zip" << std::endl <<
        "2, Export env ANDROID_NDK_ROOT=[NDK_PATH] PATH=[NDK_PATH]:$PATH NDK_PROJECT_PATH=[PANDORA_SRC_PATH]" << std::endl <<
        "3, run `ndk-build -jX`" << std::endl;

    return 0;
}


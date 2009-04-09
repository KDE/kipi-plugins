#include <cv.h>
#include <cstdio>

int main (int argc, char const* argv[])
{
    const char* libs = 0;
    const char* modules = 0;
    cvGetModuleInfo(0, &libs, &modules);
    printf("Libraries: %s\nModules: %s\n", libs, modules);
    return 0;
}


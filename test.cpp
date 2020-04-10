#include <iostream>
#include "test1/test1.h"
#include <dlfcn.h>

using namespace std;

int func()
{
    static int a = 1;
    a++;
    return a;
}

int main(int argc, char *argv[])
{
    GetModuleFileName();
    std::cout << func() << std::endl;
    Dl_info info;
    int rc = dladdr((void*)fun1, &info);
    fun1();
    std::cout << info.dli_fname << std::endl;
}
#include <iostream>
#include "test1/test1.h"
#include <dlfcn.h>
#include <iomanip>

using namespace std;

int fun1(int a)
{
	float b = 5.0;
	fun2(b);
}

int fun2(float b)
{
    int32_t c = 3;
	fun3(c);
}

int fun3(int32_t c)
{
	int fun4();
}


int main(int argc, char *argv[])
{
    fun1(5);
}
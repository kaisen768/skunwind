#include "skunwind.h"
#include <stdio.h>
#include <string.h>

void func2(void)
{
    int *p = NULL;

    *p = 10;
}

void func1(void)
{
    char buf[100] = "hello";

    memcpy(&buf[20], "world", strlen("world"));

    func2();
}

int main(int argc, const char *argv[])
{
    skunwind_register("/tmp/error.log");

    func1();

    return 0;
}

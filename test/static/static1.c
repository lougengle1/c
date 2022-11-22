#include "static1.h"
static int i;

void func3()
{
    static int j;
    j++;
    i++;
}

void func5()
{
    y++;
}

static int func6()
{
    int i_count=10;
    i_count++;
}
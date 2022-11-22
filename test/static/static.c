#include "static1.h"
static int i =0;

static int func6()
{
    int i_count=10;
    i_count++;
}

void func1()
{
    i++;
}

void func2()
{
    static int j =0;
    int x = 0;
    j++;
}

void func4()
{
    y++;
}

int main()
{
    func1();
    func1();
    func2();
    func2();
    func3();
    func4();
    func5();
    func6();

    return 0;
}
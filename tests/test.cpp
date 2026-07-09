#include <cstring>

__attribute__((pure)) 
unsigned short calculate_x()
{
    return -42;
}

int foo(int x)
{
    if (x > 0)
    {
        char buf[10];
        memset(buf, 0, 10);
    }

    return 0;
}

__attribute__((annotate("pure"))) 
int bar()
{
    unsigned short x = calculate_x();
    x = foo(x); // expected warning
    return x;
}

int main()
{
    bar();
    return 0;
}
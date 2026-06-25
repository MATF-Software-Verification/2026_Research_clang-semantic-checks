int globalCounter = 0;

void update_global_if_negative(int x)
{
    if (x < 0) {
        globalCounter = 10;
    }
}

[[clang::annotate("pure")]]
int pure_safe()
{
    unsigned short x = 10;
    update_global_if_negative(x); // no warning
    return x;
}

[[clang::annotate("pure")]]
int pure_unsafe(int x)
{
    update_global_if_negative(x); // expected-warning
    return x;
}

[[clang::annotate("pure")]]
int pure_direct_global_write()
{
    globalCounter = 10; // expected-warning
    return globalCounter;
}

int regular_updates_global()
{
    globalCounter = 20; // no warning
    return globalCounter;
}
int globalCounter = 0;
static int staticGlobalCounter = 0;

[[clang::annotate("pure")]]
int pure_updates_global()
{
    globalCounter = 10; // expected-warning
    return globalCounter;
}

[[gnu::pure]]
int gnu_pure_updates_static_global()
{
    staticGlobalCounter++; // expected-warning
    return staticGlobalCounter;
}

int pure_updates_global_by_prefix()
{
    globalCounter += 1; // expected-warning
    return globalCounter;
}

int regular_updates_global()
{
    globalCounter = 20; // no warning, function is not pure
    return globalCounter;
}
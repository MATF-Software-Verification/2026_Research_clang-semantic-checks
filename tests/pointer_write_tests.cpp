[[clang::annotate("pure")]]
void pure_pointer_safe_null()
{
    int *p = nullptr;

    if (p) {
        *p = 5; // no warning, unreachable
    }
}

[[clang::annotate("pure")]]
void pure_pointer_unsafe(int *p)
{
    if (p) {
        *p = 5; // expected-warning
    }
}

[[clang::annotate("pure")]]
void pure_pointer_read(int *p)
{
    int x = *p; // no warning
    x += 5;
    (void)x;
}

[[clang::annotate("pure")]]
void pure_reference_write(int &x)
{
    x = 42; // expected-warning
}

void regular_pointer_write(int *p)
{
    if (p) {
        *p = 10; // no warning, function is not pure
    }
}

void helper_mix(int *p, int &r)
{
    *p = 10; // Pointer write
    r = 20;  // Reference write
}

[[clang::annotate("pure")]]
void foo_mix(int *p, int &r)
{
    helper_mix(p, r);
}
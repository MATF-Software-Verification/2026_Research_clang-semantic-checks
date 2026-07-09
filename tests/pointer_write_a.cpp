void helper(int *p, int x)
{
    if(x > 0)
        *p = 10;
}

[[clang::annotate("pure")]]
void foo(int *p)
{
    int x = 10;
    helper(p, x); // expected warning, calling function with reachable side effects
}
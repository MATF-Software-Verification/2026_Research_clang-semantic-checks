
int helper(int x)
{
    return x + 42;
}

[[clang::annotate("pure")]]
int pure_helper()
{
    return 1;
}

[[clang::annotate("pure")]]
int pure_leaf()
{
    return 10;
}

[[clang::annotate("pure")]]
int pure_middle()
{
    return pure_leaf();
}

[[clang::annotate("pure")]]
int pure_middle2()
{
    return helper(3);
}

// Should trigger warning
[[clang::annotate("pure")]]
int pure_calls_impure()
{
    return helper(5);
}

// Should NOT trigger warning
[[clang::annotate("pure")]]
int pure_calls_pure()
{
    return pure_helper();
}

// Should NOT trigger warning
[[clang::annotate("pure")]]
int pure_nested_calls()
{
    return pure_middle();
}

// Should trigger warning
[[clang::annotate("pure")]]
int impure_nested_calls()
{
    return pure_middle2();
}

// Should trigger warning once
[[clang::annotate("pure")]]
int mixed_call()
{
    return pure_helper() + helper(1);
}


int main()
{
    pure_calls_impure();
    pure_calls_pure();
    pure_nested_calls();
    impure_nested_calls();
    mixed_call();

    return 0;
}
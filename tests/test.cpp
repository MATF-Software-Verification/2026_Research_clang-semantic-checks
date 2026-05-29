int pure_add(int a, int b)
{
    return a + b;
}

[[clang::annotate("pure")]]
int f(int x)
{
    return x * 2;
}

int main()
{
    pure_add(1, 2);
    f(5);
}
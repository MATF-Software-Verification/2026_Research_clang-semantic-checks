int pure_add(int a, int b)
{
    return a + b;
}

[[clang::annotate("pure")]]
int f1(int x)
{
    return x * 1;
}

__attribute__((annotate("pure"))) 
int f2(int x)
{
    return x + 2;
}

__attribute__((pure))
int f3(int x) 
{ 
    return x + 3; 
}

[[gnu::pure]]
int f4(int x)
{
    return x + 4;
}


int main()
{
    pure_add(1, 2);
    f1(5);
    f2(5);
    f3(5);
    f4(5);
}
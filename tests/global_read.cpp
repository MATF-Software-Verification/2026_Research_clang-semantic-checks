int g = 42;

[[clang::annotate("const")]]
int read_global()
{
    int x = g;

    if (g > 0)
        return x;

    return g + 1;
}

int main()
{
    return read_global();
}
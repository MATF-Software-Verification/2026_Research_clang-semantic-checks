int g = 42;

[[clang::annotate("const")]]
int read_global(int arg)
{
    if (arg > 0) {
        int x = g;
        return x;
    }

    return g + 1;
}

int main()
{
    return read_global(10);
}
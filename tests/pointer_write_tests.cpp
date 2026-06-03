[[clang::annotate("pure")]]
void pure_pointer_write(int *p)
{
    *p = 5; // expected warning
}

[[clang::annotate("pure")]]
void pure_pointer_read(int *p)
{
    int x = *p; // no warning
    x += 5;
    (void)x;
}

int main()
{
    int value = 0;
    pure_pointer_write(&value);
    return 0;
}
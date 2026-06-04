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

[[clang::annotate("pure")]]
void pure_reference_write(int &x)
{
    x = 42; // expected warning
}

int main()
{
    int value = 0;
    pure_pointer_write(&value);
    pure_reference_write(value);
    
    return 0;
}
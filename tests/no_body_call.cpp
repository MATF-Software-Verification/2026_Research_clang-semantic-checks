extern void helper();

[[clang::annotate("pure")]]
void foo()
{
    helper(); // expected warning
}
[[gnu::pure]]
int pureHelper();

[[gnu::const]]
int constHelper();

int ordinaryHelper();

[[gnu::pure]]
int pureCallsPure()
{
    return pureHelper(); // bez warning-a
}

[[gnu::pure]]
int pureCallsConst()
{
    return constHelper(); // bez warning-a
}

[[gnu::const]]
int constCallsPure()
{
    return pureHelper(); // warning
}

[[gnu::const]]
int constCallsOrdinary()
{
    return ordinaryHelper(); // warning
}

[[gnu::pure]]
int pureCallsOrdinary()
{
    return ordinaryHelper(); // warning
}
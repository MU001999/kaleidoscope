#include <cstdio>

extern "C" double putchard(double c)
{
    fputc((char)c, stderr);
    return 0.0;
}

extern "C" double printd(double x)
{
    printf("%lf\n", x);
    return 0.0;
}

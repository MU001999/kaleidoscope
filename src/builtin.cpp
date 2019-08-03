#include <cstdio>

extern "C" double putchard(double c)
{
    fputc((char)c, stderr);
    return 0.0;
}

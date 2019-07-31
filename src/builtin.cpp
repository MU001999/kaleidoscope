extern "C" double fib(double n)
{
    if (n < 3.0)
    {
        return 1.0;
    }
    else
    {
        return fib(n - 1) + fib(n - 2);
    }
}

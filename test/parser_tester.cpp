#include <cstdio>

#include "../src/lexer.cpp"
#include "../src/parser.cpp"

using namespace std;
using namespace kaleidoscope;

int main()
{
    Parser().main_loop();
    return 0;
}

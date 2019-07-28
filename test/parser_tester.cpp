#include <cstdio>

#include "../src/lexer.cpp"
#include "../src/parser.cpp"

using namespace std;
using namespace kaleidoscope;

int main()
{
    Parser parser;
    fprintf(stderr, "ready> ");
    parser.get_next_token();
    parser.main_loop();
    return 0;
}

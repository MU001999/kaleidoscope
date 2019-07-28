#include <cstdio>

#include "../src/lexer.cpp"
#include "../src/parser.cpp"
#include "../src/codegen.cpp"

using namespace std;
using namespace kaleidoscope;

int main()
{
    TheModule = std::make_unique<Module>("My cool jit", TheContext);
    Parser().main_loop();
    TheModule->print(errs(), nullptr);
    return 0;
}

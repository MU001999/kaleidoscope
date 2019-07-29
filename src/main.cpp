#include "node.hpp"
#include "lexer.hpp"
#include "parser.hpp"

using namespace std;
using namespace llvm;
using namespace kaleidoscope;

int main()
{
    initialize_module_and_pass_manager();
    Parser().main_loop();
    TheModule->print(errs(), nullptr);
    return 0;
}

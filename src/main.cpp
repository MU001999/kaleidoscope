#include "node.hpp"
#include "lexer.hpp"
#include "parser.hpp"

using namespace std;
using namespace llvm;
using namespace kaleidoscope;

int main(int argc, char *argv[])
{
    Interpret = (argc == 1);

    InitializeNativeTarget();
    InitializeNativeTargetAsmPrinter();
    InitializeNativeTargetAsmParser();

    TheJIT = llvm::make_unique<orc::KaleidoscopeJIT>();
    initialize_module_and_pass_manager();

    Parser().main_loop();

    return 0;
}

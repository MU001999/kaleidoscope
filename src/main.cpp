#include "node.hpp"
#include "lexer.hpp"
#include "parser.hpp"

using namespace std;
using namespace llvm;
using namespace kaleidoscope;

bool check_args(int argc, char *argv[]);

int main(int argc, char *argv[])
{
    Interpret = check_args(argc, argv);

    if (Interpret)
    {
        InitializeNativeTarget();
        InitializeNativeTargetAsmPrinter();
        InitializeNativeTargetAsmParser();
        TheJIT = llvm::make_unique<orc::KaleidoscopeJIT>();
    }

    initialize_module_and_pass_manager();

    Parser().main_loop();

    if (!Interpret)
    {
        // ... emit code
    }

    return 0;
}

bool check_param(int argc, char *argv[])
{
    return argc > 1;
}

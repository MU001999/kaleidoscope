#include "node.hpp"
#include "lexer.hpp"
#include "parser.hpp"

using namespace std;
using namespace llvm;
using namespace kaleidoscope;

int main()
{
    InitializeNativeTarget();
    InitializeNativeTargetAsmPrinter();
    InitializeNativeTargetAsmParser();

    TheJIT = llvm::make_unique<orc::KaleidoscopeJIT>();
    initialize_module_and_pass_manager();

    Parser().main_loop();
    TheModule->print(errs(), nullptr);

    return 0;
}

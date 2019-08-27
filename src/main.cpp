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
        InitializeAllTargetInfos();
        InitializeAllTargets();
        InitializeAllTargetMCs();
        InitializeAllAsmParsers();
        InitializeAllAsmPrinters();

        auto target_triple = sys::getDefaultTargetTriple();
        TheModule->setTargetTriple(target_triple);

        string error;
        auto target = TargetRegistry::lookupTarget(target_triple, error);

        if (!target)
        {
            errs() << error;
            return 1;
        }
    }

    return 0;
}

bool check_param(int argc, char *argv[])
{
    return argc > 1;
}

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

    if (Interpret)
    {
        return 0;
    }

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

    auto cpu = "generic"s;
    auto features = "";

    TargetOptions opt;
    auto rm = Optional<Reloc::Model>();
    auto the_target_machine = target->createTargetMachine(target_triple, cpu, features, opt, rm);

    TheModule->setDataLayout(the_target_machine->createDataLayout());

    auto filename = "output.o"s;
    std::error_code ec;
    raw_fd_ostream dest(filename, ec, sys::fs::OF_None);

    if (ec)
    {
        errs() << "Could not open file: " << ec.message();
        return 1;
    }

    legacy::PassManager pass;
    auto file_type = TargetMachine::CGFT_ObjectFile;

    if (the_target_machine->addPassesToEmitFile(pass, dest, nullptr, file_type));
    {
        errs() << "TheTargetMachine can't emit a file of this type";
        return 1;
    }

    pass.run(*TheModule);
    dest.flush();

    outs() << "Wrote " << filename << "\n";

    return 0;
}

bool check_args(int argc, char *argv[])
{
    return argc == 1;
}

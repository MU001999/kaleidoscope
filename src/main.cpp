#include <tuple>
#include <iostream>

#include "node.hpp"
#include "lexer.hpp"
#include "parser.hpp"
#include "argparse.hpp"

using namespace std;
using namespace llvm;
using namespace kaleidoscope;

tuple<bool, string, string> args_parse(int argc, char *agrv[]);

int main(int argc, char *argv[])
{
    auto [res, infile, outfile] = args_parse(argc, argv);
    Interpret = res;

    if (Interpret)
    {
        InitializeNativeTarget();
        InitializeNativeTargetAsmPrinter();
        InitializeNativeTargetAsmParser();
        TheJIT = llvm::make_unique<orc::KaleidoscopeJIT>();
    }
    else
    {
        freopen(infile.c_str(), "r", stdin);
        freopen("/dev/null", "w", stdout);
        // freopen("/dev/null", "w", stderr);
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

    std::error_code ec;
    raw_fd_ostream dest(outfile, ec, sys::fs::OF_None);

    if (ec)
    {
        errs() << "Could not open file: " << ec.message();
        return 1;
    }

    legacy::PassManager pass;
    auto file_type = TargetMachine::CGFT_ObjectFile;

    if (the_target_machine->addPassesToEmitFile(pass, dest, nullptr, file_type))
    {
        errs() << "The target machine can't emit a file of this type";
        return 1;
    }

    pass.run(*TheModule);
    dest.flush();

    // outs() << "Wrote " << outfile << "\n";

    return 0;
}

tuple<bool, string, string> args_parse(int argc, char *argv[])
{
    argparse::ArgumentParser program("kaleidoscope");

    program.add_argument("input_file")
        .help("files ready to compile")
        .default_value(""s)
        .action([](const string &value) { return value; });

    program.add_argument("-c", "--compile")
        .help("files ready to compile")
        .default_value(""s)
        .action([](const string &value) { return value; });

    program.add_argument("-o")
        .help("file to output")
        .default_value("a.o"s)
        .action([](const string &value) { return value; });

    try
    {
        program.parse_args(argc, argv);
    }
    catch (const runtime_error &err)
    {
        cout << err.what() << endl;
        program.print_help();
        exit(0);
    }

    auto input_file = program.get("input_file").empty()
                    ? program.get("-c")
                    : program.get("input_file");

    return make_tuple(input_file.empty(), input_file, program.get("-o"));
}

#include "node.hpp"

using namespace std;
using namespace llvm;

namespace kaleidoscope
{
Value *NumberExprAST::codegen()
{
    return ConstantFP::get(TheContext, APFloat(value_));
}

Value *VariableExprAST::codegen()
{
    auto V = NamedValues[name_];
    if (!V)
    {
        log_error_v("Unknown variable name");
    }
    return V;
}

Value *BinaryExprAST::codegen()
{
    auto lhs = lhs_->codegen();
    auto rhs = rhs_->codegen();
    if (!lhs || !rhs)
    {
        return nullptr;
    }
    switch (op_)
    {
    case '+':
        return Builder.CreateFAdd(lhs, rhs, "addtmp");
    case '-':
        return Builder.CreateFSub(lhs, rhs, "subtmp");
    case '*':
        return Builder.CreateFMul(lhs, rhs, "multmp");
    case '<':
        lhs = Builder.CreateFCmpULT(lhs, rhs, "cmptmp");
        return Builder.CreateUIToFP(lhs, Type::getDoubleTy(TheContext), "booltmp");
    default:
        return log_error_v("invalid binary operator");
    }
}

Value *CallExprAST::codegen()
{
    auto callee = TheModule->getFunction(callee_);
    if (!callee)
    {
        return log_error_v("Unknown function referenced");
    }
    if (callee->arg_size() != args_.size())
    {
        return log_error_v("Incorrect # arguments passed");
    }

    vector<Value *> args;
    for (size_t i = 0; i != args_.size(); ++i)
    {
        args.push_back(args_[i]->codegen());
        if (!args.back())
        {
            return nullptr;
        }
    }
    return Builder.CreateCall(callee, args, "calltmp");
}

Function *PrototypeAST::codegen()
{
    std::vector<Type *> doubles(args_.size(), Type::getDoubleTy(TheContext));
    auto ft = FunctionType::get(Type::getDoubleTy(TheContext), doubles, false);
    auto f = Function::Create(ft, Function::ExternalLinkage, name_, TheModule.get());

    size_t idx = 0;
    for (auto &arg : f->args())
    {
        arg.setName(args_[idx++]);
    }
    return f;
}

Function *FunctionAST::codegen()
{
    auto the_function = TheModule->getFunction(proto_->get_name());
    if (!the_function)
    {
        the_function = proto_->codegen();
    }
    if (!the_function)
    {
        return nullptr;
    }
    if (!the_function->empty())
    {
        return log_error_f("Function cannot be redefined");
    }

    auto bb = BasicBlock::Create(TheContext, "entry", the_function);
    Builder.SetInsertPoint(bb);

    NamedValues.clear();
    for (auto &arg : the_function->args())
    {
        NamedValues[arg.getName()] = &arg;
    }

    if (auto ret_val = body_->codegen())
    {
        Builder.CreateRet(ret_val);
        verifyFunction(*the_function);
        TheFPM->run(*the_function);
        return the_function;
    }

    the_function->eraseFromParent();
    return nullptr;
}
} // namespace kaleidoscope

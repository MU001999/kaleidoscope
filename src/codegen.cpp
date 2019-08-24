#include <memory>
#include <string>
#include <algorithm>

#include "node.hpp"
#include "parser.hpp"

using namespace std;
using namespace llvm;

namespace kaleidoscope
{
Function *get_function(const string &name)
{
    if (auto f = TheModule->getFunction(name))
    {
        return f;
    }

    if (FunctionProtos.count(name))
    {
        return FunctionProtos[name]->codegen();
    }

    return nullptr;
}

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
    return Builder.CreateLoad(V, name_.c_str());
}

Value *UnaryExprAST::codegen()
{
    auto val = operand_->codegen();
    if (!val)
    {
        return nullptr;
    }

    auto f = get_function("unary"s + op_);
    if (!f)
    {
        return log_error_v("Unknown unary operator");
    }

    return Builder.CreateCall(f, val, "unop");
}

Value *BinaryExprAST::codegen()
{
    if (op_ == '=')
    {
        auto lhs = dynamic_cast<VariableExprAST*>(lhs_.get());
        if (!lhs)
        {
            return log_error_v("destination of '=' must be a variable");
        }

        auto val = rhs_->codegen();
        if (!val)
        {
            return nullptr;
        }

        auto variable = NamedValues[lhs->get_name()];
        if (!variable)
        {
            return log_error_v("Unknown variable name");
        }

        Builder.CreateStore(val, variable);
        return val;
    }

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
        break;
    }

    auto f = get_function("binary"s + op_);
    assert(f && "binary operator not found!");

    return Builder.CreateCall(f, { lhs, rhs }, "binop");
}

Value *CallExprAST::codegen()
{
    auto callee = get_function(callee_);
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

Value *IfExprAST::codegen()
{
    auto cond = cond_->codegen();
    if (!cond)
    {
        return nullptr;
    }

    cond = Builder.CreateFCmpONE(cond, ConstantFP::get(TheContext, APFloat(0.0)), "ifcond");

    Function *the_function = Builder.GetInsertBlock()->getParent();

    auto then_bb = BasicBlock::Create(TheContext, "then", the_function);
    auto else_bb = BasicBlock::Create(TheContext, "else");
    auto merge_bb = BasicBlock::Create(TheContext, "ifcont");

    Builder.CreateCondBr(cond, then_bb, else_bb);

    Builder.SetInsertPoint(then_bb);
    auto then = then_->codegen();
    if (!then)
    {
        return nullptr;
    }
    Builder.CreateBr(merge_bb);
    then_bb = Builder.GetInsertBlock();

    the_function->getBasicBlockList().push_back(else_bb);
    Builder.SetInsertPoint(else_bb);

    auto els = else_->codegen();
    if (!els)
    {
        return nullptr;
    }
    Builder.CreateBr(merge_bb);
    else_bb = Builder.GetInsertBlock();

    the_function->getBasicBlockList().push_back(merge_bb);
    Builder.SetInsertPoint(merge_bb);
    auto pn = Builder.CreatePHI(Type::getDoubleTy(TheContext), 2, "iftmp");

    pn->addIncoming(then, then_bb);
    pn->addIncoming(els, else_bb);
    return pn;
}

Value *ForExprAST::codegen()
{
    auto start = start_->codegen();
    if (!start)
    {
        return nullptr;
    }

    auto the_function = Builder.GetInsertBlock()->getParent();
    auto alloca = create_entry_block_alloca(the_function, var_name_);
    Builder.CreateStore(start, alloca);

    auto loop_bb = BasicBlock::Create(TheContext, "loop", the_function);

    Builder.CreateBr(loop_bb);
    Builder.SetInsertPoint(loop_bb);

    if (!body_->codegen())
    {
        return nullptr;
    }

    Value *step = nullptr;
    if (step_)
    {
        step = step_->codegen();
        if (!step)
        {
            return nullptr;
        }
    }
    else
    {
        step = ConstantFP::get(TheContext, APFloat(1.0));
    }

    auto end_cond = end_->codegen();
    if (!end_cond)
    {
        return nullptr;
    }

    auto cur_var = Builder.CreateLoad(alloca);
    auto next_var = Builder.CreateFAdd(cur_var, step, "nextvar");
    Builder.CreateStore(next_var, alloca);
    end_cond = Builder.CreateFCmpONE(end_cond, ConstantFP::get(TheContext, APFloat(0.0)), "loopcond");

    auto after_bb = BasicBlock::Create(TheContext, "afterloop", the_function);

    Builder.CreateCondBr(end_cond, loop_bb, after_bb);
    Builder.SetInsertPoint(after_bb);

    return Constant::getNullValue(Type::getDoubleTy(TheContext));
}

Value *VarExprAST::codegen()
{
    auto old_bindings = NamedValues;
    auto the_function = Builder.GetInsertBlock()->getParent();

    for (const auto &varname_exprast : var_names_)
    {
        const auto &var_name = varname_exprast.first;
        auto init = varname_exprast.second.get();
        Value *init_val;
        if (init)
        {
            init_val = init->codegen();
            if (!init_val)
            {
                return nullptr;
            }
        }
        else
        {
            init_val = ConstantFP::get(TheContext, APFloat(0.0));
        }

        auto alloca = create_entry_block_alloca(the_function, var_name);
        Builder.CreateStore(init_val, alloca);

        NamedValues[var_name] = alloca;
    }

    auto body = body_->codegen();
    if (!body)
    {
        return nullptr;
    }

    NamedValues = old_bindings;
    return body;
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
    auto &proto = *proto_;
    FunctionProtos[proto.get_name()] = move(proto_);
    auto the_function = get_function(proto.get_name());

    if (!the_function)
    {
        return nullptr;
    }

    if (proto.is_binary_op())
    {
        auto op = proto.get_operator_name();
        auto precedence = proto.get_binary_precedence();
        if (Parser::symbol_precedences_.count(op)
            && Parser::symbol_precedences_[op] != precedence)
        {
            Parser::precedence_symbols_[Parser::symbol_precedences_[op]].erase(op);
        }
        Parser::symbol_precedences_[op] = precedence;
        auto pos = lower_bound(Parser::precedences_.begin(),
            Parser::precedences_.end(),
            precedence);
        if (pos == Parser::precedences_.end()
            || *pos != precedence)
        {
            Parser::precedences_.insert(pos, precedence);
        }
        Parser::precedence_symbols_[precedence].insert(proto.get_operator_name());
    }

    auto bb = BasicBlock::Create(TheContext, "entry", the_function);
    Builder.SetInsertPoint(bb);

    NamedValues.clear();
    for (auto &arg : the_function->args())
    {
        auto alloca = create_entry_block_alloca(the_function, arg.getName());
        Builder.CreateStore(&arg, alloca);
        NamedValues[arg.getName()] = alloca;
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

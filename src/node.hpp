#ifndef KALEIDOSCOPE_NODE_HPP
#define KALEIDOSCOPE_NODE_HPP

#include <memory>
#include <string>
#include <vector>
#include <utility>

// ExprAST - Base class for all expression nodes
class ExprAST
{
  public:
    virtual ~ExprAST() {}
};

// NumberExprAST - Expression class for numeric literals like "1.0"
class NumberExprAST : public ExprAST
{
    double value_;

  public:
    NumberExprAST(double value) : value_(value) {}
};

// VariableExprAST - Expression class for referencing a variable, like "a".
class VariableExprAST : public ExprAST
{
    std::string name_;

  public:
    VariableExprAST(const std::string &name) : name_(name) {}
};

// BinaryExprAST - Expression class for a binary operator.
class BinaryExprAST : public ExprAST
{
    char op_;
    std::unique_ptr<ExprAST> lhs_, rhs_;

  public:
    BinaryExprAST(char op,
        std::unique_ptr<ExprAST> lhs,
        std::unique_ptr<ExprAST> rhs)
      : op_(op), lhs_(std::move(lhs)), rhs_(std::move(rhs)) {}
};

// CallExprAST - Expression class for function calls.
class CallExprAST : public ExprAST
{
    std::string callee_;
    std::vector<std::unique_ptr<ExprAST>> args_;

  public:
    CallExprAST(const std::string &callee,
        std::vector<std::unique_ptr<ExprAST>> args)
      : callee_(callee), args_(std::move(args)) {}
};

// PrototypeAST - This class represents the "prototype" for a function,
// which captures its name, and its argument names (thus implicitly the number
// of arguments the function takes).
class PrototypeAST
{
    std::string name_;
    std::vector<std::string> args_;

  public:
    PrototypeAST(const std::string &name,
        std::vector<std::string> args)
      : name_(name), args_(std::move(args)) {}

    const std::string &get_name() const { return name_; }
};

// FunctionAST - This class represents a function definition itself.
class FunctionAST
{
    std::unique_ptr<PrototypeAST> proto_;
    std::unique_ptr<ExprAST> body_;

  public:
    FunctionAST(std::unique_ptr<PrototypeAST> proto,
        std::unique_ptr<ExprAST> body)
      : proto_(std::move(proto)), body_(std::move(body)) {}
};

#endif // KALEIDOSCOPE_NODE_HPP

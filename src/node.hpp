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

#endif // KALEIDOSCOPE_NODE_HPP

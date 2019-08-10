#ifndef KALEIDOSCOPE_PARSER_HPP
#define KALEIDOSCOPE_PARSER_HPP

#include <set>
#include <list>
#include <memory>
#include <unordered_map>

#include "node.hpp"
#include "lexer.hpp"

namespace kaleidoscope
{
class Parser
{
  public:
    Parser() = default;
    ~Parser() = default;

    void main_loop();
    Token get_next_token();

  private:
    std::unique_ptr<ExprAST> parse_expression(std::list<size_t>::iterator precedence = precedences_.begin());
    std::unique_ptr<ExprAST> parse_primary();
    std::unique_ptr<ExprAST> parse_unary();
    std::unique_ptr<ExprAST> parse_number_expr();
    std::unique_ptr<ExprAST> parse_paren_expr();
    std::unique_ptr<ExprAST> parse_identifier_expr();
    std::unique_ptr<ExprAST> parse_if_expr();
    std::unique_ptr<ExprAST> parse_for_expr();
    std::unique_ptr<ExprAST> parse_var_expr();

    std::unique_ptr<PrototypeAST> parse_extern();
    std::unique_ptr<PrototypeAST> parse_prototype();
    std::unique_ptr<FunctionAST> parse_definition();
    std::unique_ptr<FunctionAST> parse_top_level_expr();

    void handle_definition();
    void handle_extern();
    void handle_top_level_expression();

    Lexer lexer_;
    Token cur_token_;

    static std::list<size_t> precedences_;
    static std::unordered_map<size_t, std::set<char>> precedence_symbols_;
    static std::unordered_map<char, size_t> symbol_precedences_;

    friend class FunctionAST;
};
} // namespace kaleidoscope

#endif // KALEIDOSCOPE_PARSER_HPP

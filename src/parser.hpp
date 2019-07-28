#ifndef KALEIDOSCOPE_PARSER_HPP
#define KALEIDOSCOPE_PARSER_HPP

#include <memory>

#include "node.hpp"
#include "lexer.hpp"

namespace kaleidoscope
{
namespace details
{
constexpr size_t precedences_size = 3;
const vector<unordered_set<int>> precedences
{
    { '<' },
    { '+', '-' },
    { '*' }
};
} // namespace details

class Parser
{
  public:
    Parser() = default;
    ~Parser() = default;

    void main_loop();
    Token get_next_token();

  private:
    template <size_t precedence = 0>
    std::unique_ptr<ExprAST> parse_expression()
    {
        if constexpr (precedence == details::precedences_size)
        {
            return parse_primary();
        }
        else
        {
            auto lhs = parse_expression<precedence+1>();
            auto op = cur_token_.type();
            while (details::precedences[precedence].count(op))
            {
                get_next_token();
                auto rhs = parse_expression<precedence+1>();
                lhs = make_unique<BinaryExprAST>(op, move(lhs), move(rhs));
                op = cur_token_.type();
            }
            return lhs;
        }
    }
    std::unique_ptr<ExprAST> parse_primary();
    std::unique_ptr<ExprAST> parse_number_expr();
    std::unique_ptr<ExprAST> parse_paren_expr();
    std::unique_ptr<ExprAST> parse_identifier_expr();

    std::unique_ptr<PrototypeAST> parse_extern();
    std::unique_ptr<PrototypeAST> parse_prototype();
    std::unique_ptr<FunctionAST> parse_definition();
    std::unique_ptr<FunctionAST> parse_top_level_expr();

    void handle_definition();
    void handle_extern();
    void handle_top_level_expression();

    Lexer lexer_;
    Token cur_token_;
};
} // namespace kaleidoscope

#endif // KALEIDOSCOPE_PARSER_HPP

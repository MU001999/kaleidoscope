#include <string>
#include <vector>
#include <utility>
#include <unordered_set>

#include "parser.hpp"

using namespace std;

namespace
{

} // namespace

namespace kaleidoscope
{
unique_ptr<ExprAST> Parser::log_error(const char *str)
{
    fprintf(stderr, "LogError: %s\n", str);
    return nullptr;
}

unique_ptr<PrototypeAST> Parser::log_error_p(const char *str)
{
    log_error(str);
    return nullptr;
}

void Parser::main_loop()
{
    fprintf(stderr, "ready> ");
    get_next_token();
    while (true)
    {
        switch (cur_token_)
        {
        case Token::END:
            return;
        case ';':
            get_next_token();
            break;
        case Token::DEF:
            handle_definition();
            break;
        case Token::EXTERN:
            handle_extern();
            break;
        default:
            handle_top_level_expression();
            break;
        }
        fprintf(stderr, "ready> ");
    }
}

Token Parser::get_next_token()
{
    return cur_token_ = lexer_.next();
}

unique_ptr<ExprAST> Parser::parse_expression()
{

}

unique_ptr<ExprAST> Parser::parse_primary()
{

}

unique_ptr<ExprAST> Parser::parse_number_expr()
{
    auto result = make_unique<NumberExprAST>(stod(cur_token_.value()));
    get_next_token();
    return result;
}

unique_ptr<ExprAST> Parser::parse_paren_expr()
{

}

unique_ptr<ExprAST> Parser::parse_identifier_expr()
{

}

unique_ptr<PrototypeAST> Parser::parse_extern()
{

}

unique_ptr<PrototypeAST> Parser::parse_prototype()
{

}

unique_ptr<FunctionAST> Parser::parse_definition()
{

}

unique_ptr<FunctionAST> Parser::parse_top_level_expr()
{

}

void Parser::handle_definition()
{

}

void Parser::handle_extern()
{

}

void Parser::handle_top_level_expression()
{

}
} // namespace kaleidoscope

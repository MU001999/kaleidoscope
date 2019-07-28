#include <cstdio>
#include <string>
#include <vector>
#include <utility>
#include <unordered_set>

#include "node.hpp"
#include "parser.hpp"

using namespace std;

namespace kaleidoscope
{
void Parser::main_loop()
{
    fprintf(stderr, "ready> ");
    get_next_token();
    while (true)
    {
        fprintf(stderr, "ready> ");
        switch (cur_token_.type())
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
    }
}

Token Parser::get_next_token()
{
    return cur_token_ = lexer_.next();
}

unique_ptr<ExprAST> Parser::parse_primary()
{
    switch (cur_token_.type())
    {
    case Token::IDENTIFIER:
        return parse_identifier_expr();
    case Token::NUMBER:
        return parse_number_expr();
    case '(':
        return parse_paren_expr();
    default:
        return log_error("unknown token when expecting an expression");
    }
}

unique_ptr<ExprAST> Parser::parse_number_expr()
{
    auto result = std::make_unique<NumberExprAST>(stod(cur_token_.value()));
    get_next_token();
    return result;
}

unique_ptr<ExprAST> Parser::parse_paren_expr()
{
    get_next_token();
    auto val = parse_expression();
    if (!val)
    {
        return nullptr;
    }
    if (cur_token_.type() != ')')
    {
        return log_error("expected ')'");
    }
    get_next_token();
    return val;
}

unique_ptr<ExprAST> Parser::parse_identifier_expr()
{
    auto name = cur_token_.value();
    get_next_token();
    if (cur_token_.type() != '(')
    {
        return std::make_unique<VariableExprAST>(name);
    }

    get_next_token();
    vector<unique_ptr<ExprAST>> args;
    if (cur_token_.type() != ')')
    {
        while (true)
        {
            if (auto arg = parse_expression())
            {
                args.push_back(move(arg));
            }
            else
            {
                return nullptr;
            }
            if (cur_token_.type() == ')')
            {
                break;
            }
            if (cur_token_.type() != ',')
            {
                return log_error("Expected ')' or ',' in argument list");
            }
            get_next_token();
        }
    }
    get_next_token();
    return std::make_unique<CallExprAST>(name, move(args));
}

unique_ptr<PrototypeAST> Parser::parse_extern()
{
    get_next_token();
    return parse_prototype();
}

unique_ptr<PrototypeAST> Parser::parse_prototype()
{
    if (cur_token_.type() != Token::IDENTIFIER)
    {
        return log_error_p("Expected function name in prototype");
    }
    auto name = cur_token_.value();
    get_next_token();
    if (cur_token_.type() != '(')
    {
        return log_error_p("expected '(' in prototype");
    }
    vector<string> args;
    while (get_next_token().type() == Token::IDENTIFIER)
    {
        args.push_back(cur_token_.value());
    }
    if (cur_token_.type() != ')')
    {
        return log_error_p("Expected ')' in prototype");
    }
    get_next_token();
    return std::make_unique<PrototypeAST>(name, move(args));
}

unique_ptr<FunctionAST> Parser::parse_definition()
{
    get_next_token();
    auto proto = parse_prototype();
    if (!proto)
    {
        return nullptr;
    }
    if (auto expr = parse_expression())
    {
        return std::make_unique<FunctionAST>(move(proto), move(expr));
    }
    return nullptr;
}

unique_ptr<FunctionAST> Parser::parse_top_level_expr()
{
    if (auto expr = parse_expression())
    {
        auto proto = std::make_unique<PrototypeAST>("__anno_expr", vector<string>());
        return std::make_unique<FunctionAST>(move(proto), move(expr));
    }
    return nullptr;
}

void Parser::handle_definition()
{
    if (auto fn_ast = parse_definition())
    {
        if (auto fn_ir = fn_ast->codegen())
        {
            fprintf(stderr, "Parsed a function definition\n");
            fn_ir->print(llvm::errs());
            fprintf(stderr, "\n");
        }
    }
    else
    {
        get_next_token();
    }
}

void Parser::handle_extern()
{
    if (auto proto_ast = parse_extern())
    {
        if (auto proto_ir = proto_ast->codegen())
        {
            fprintf(stderr, "Parsed an extern\n");
            proto_ir->print(llvm::errs());
            fprintf(stderr, "\n");
        }
    }
    else
    {
        get_next_token();
    }
}

void Parser::handle_top_level_expression()
{
    if (auto fn_ast = parse_top_level_expr())
    {
        if (auto fn_ir = fn_ast->codegen())
        {
            fprintf(stderr, "Parsed a top-level expr\n");
            fn_ir->print(llvm::errs());
            fprintf(stderr, "\n");
        }
    }
    else
    {
        get_next_token();
    }
}
} // namespace kaleidoscope

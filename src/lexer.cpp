#include <cctype>
#include <utility>
#include <unordered_map>

#include "lexer.hpp"

using namespace std;

namespace
{
using kaleidoscope::Token;
const unordered_map<string, Token::Type> symbols
{
    { "def", Token::DEF },
    { "extern", Token::EXTERN }
};
} // namespace

namespace kaleidoscope
{
Token::Token(int type, string value)
  : type_(type), value_(move(value))
{
    // ...
}

Token::operator bool() const
{
    return type_ != ERR && type_ != END;
}

int Token::type() const
{
    return type_;
}

string Token::value() const
{
    return value_;
}

Lexer::Lexer()
  : last_char_(' ')
{
    // ...
}

Token Lexer::next()
{
    string value;

    enum class State
    {
        Begin,

        InIdOrCmd,
        InNumber
    } state = State::Begin;

    while (isspace(last_char_))
    {
        last_char_ = getchar();
    }

    if (isalpha(last_char_)) // identifier: [a-zA-Z][a-zA-Z0-9]*
    {
        value = last_char_;
        while (isalnum((last_char_ = getchar())))
        {
            value += last_char_;
        }
        if (symbols.count(value))
        {
            return Token(symbols.at(value));
        }
        else
        {
            return Token(Token::IDENTIFIER, value);
        }

    }
    else if (isdigit(last_char_) || last_char_ == '.') // Number: [0-9.]+
    {
        do
        {
            value += last_char_;
            last_char_ = getchar();
        } while (isdigit(last_char_) || last_char_ == '.');
        return Token(Token::NUMBER, value);
    }
    else if (last_char_ == '#')
    {
        do
        {
            last_char_ = getchar();
        } while (last_char_ != EOF && last_char_ != '\n' && last_char_ != '\r');

        if (last_char_ != EOF)
        {
            return next();
        }
    }
    else
    {
        if (last_char_ == EOF)
        {
            return Token(Token::END);
        }
        else
        {
            int this_char = last_char_;
            last_char_ = getchar();
            return Token(this_char);
        }
    }
}
} // namespace kaleidoscope

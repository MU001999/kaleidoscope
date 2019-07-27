#ifndef KALEIDOSCOPE_LEXER_HPP
#define KALEIDOSCOPE_LEXER_HPP

#include <string>

namespace kaleidoscope
{
class Token
{
  public:
    enum Type
    {
        // commands
        DEF,
        EXTERN,

        // primary
        IDENTIFIER,
        NUMBER,

        ERR,
        END
    };

    Token() = default;
    ~Token() = default;

    Token(Type type, std::string value = "");

    // operator bool()
    // return false if type is ERR or END else true
    operator bool() const;
    // return type of current token
    Type type() const;
    // return value of current token
    std::string value() const;

  private:
    Type type_;
    std::string value_;
};

class Lexer
{
  public:
    Lexer();

    Token next();

  private:
    char last_char_;
};
} // namespace kaleidoscope

#endif // KALEIDOSCOPE_LEXER_HPP

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
        END         = -1,

        // commands
        DEF         = -2,
        EXTERN      = -3,

        // primary
        IDENTIFIER  = -4,
        NUMBER      = -5,

        // control
        IF          = -6,
        THEN        = -7,
        ELSE        = -8
    };

    Token() = default;
    ~Token() = default;

    Token(int type, std::string value = "");

    // operator bool()
    // return false if type is ERR or END else true
    explicit operator bool() const;
    // return type of current token
    int type() const;
    // return value of current token
    std::string value() const;

  private:
    int type_;
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

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
        DEF = -1,
        EXTERN = -2,

        // primary
        IDENTIFIER = -3,
        NUMBER = -4,

        END = -6
    };

    Token() = default;
    ~Token() = default;

    Token(int type, std::string value = "");

    // operator bool()
    // return false if type is ERR or END else true
    operator bool() const;
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

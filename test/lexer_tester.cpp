#include <iostream>
#include "../src/lexer.cpp"

using namespace kaleidoscope;

int main()
{
    Lexer lexer;
    Token token;
    while (token = lexer.next())
    {
        cout << token.type() << " " << token.value() << endl;
    }
    return 0;
}

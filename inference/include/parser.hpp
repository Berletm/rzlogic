#ifndef PARSER_HPP
#define PARSER_HPP

#include "logic.hpp"

namespace rzlogic {

class Parser
{
private:
    std::string input;
    char *cur_char;

    enum class TokenType
    {
        LPAREN,
        RPAREN,
        IDENTIFIER,
        END
    };

    std::string token_str;
    TokenType   token_type;

    void        ParseToken();
    Formula*    ParseFormula();
    Formula*    ParseTerm();
    
public:
    Parser(const std::string &s) : input(s), cur_char((char*)input.c_str()) {}
    Formula *Parse();
};

void ReadFormula(const char *str);

} // namespace rzlogic

#endif

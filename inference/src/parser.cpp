#include "parser.hpp"
#include <stdexcept>

namespace rzlogic {

Formula *Parser::ParseTerm()
{
    if (token_type == TokenType::IDENTIFIER) {
        Formula *f = new Formula();
        f->str = token_str;
        f->type = (token_str.size() >= 1 && tolower(token_str[0]) >= 'n') ? FormulaType::VARIABLE
                                                                          : FormulaType::CONSTANT;

        ParseToken();
        return f;
    }
    else if (token_type == TokenType::LPAREN)
    {
        Formula *f = new Formula();
        f->type = FormulaType::FUNCTION;
        
        ParseToken();
        if (token_type != TokenType::IDENTIFIER) {
            throw std::runtime_error("Expected function name");
        }
        f->str = token_str;
        ParseToken();

        while (token_type != TokenType::RPAREN) {
            f->children.push_back(ParseTerm());
        }
        ParseToken();
        return f;
    }

    throw std::runtime_error("Expected identifier or '(' at position " + std::to_string(cur_char - input.data()));
}

void Parser::ParseToken()
{
    token_str.clear();
    while (*cur_char == ' ') cur_char++;

    switch (*cur_char) {
    case '(': token_type = TokenType::LPAREN; cur_char++; return;
    case ')': token_type = TokenType::RPAREN; cur_char++; return;
    case '\0': token_type = TokenType::END; return;
        return;
    }

    char *start = cur_char;
    while (isalnum(*cur_char)) cur_char++;
    token_str = std::string(start, cur_char - start);
    token_type = TokenType::IDENTIFIER;
}

Formula *Parser::ParseFormula()
{
    if (token_type == TokenType::LPAREN)
    {
        ParseToken();
        if (token_type != TokenType::IDENTIFIER) {
            throw std::runtime_error("Expected identifier at position " + std::to_string(cur_char - input.data()));
        }
        std::string name = token_str;
        ParseToken();

        if (name == "forall" || name == "exists")
        {
            Formula *f = new Formula();
            if (token_type != TokenType::IDENTIFIER) {
                throw std::runtime_error("Expected variable name after quantifier");
            }
            std::string var_name = token_str;
            ParseToken();

            f->type = (name == "forall") ? FormulaType::FORALL : FormulaType::EXISTS;
            f->children.push_back(ParseFormula());
            f->str = var_name;
            
            if (token_type != TokenType::RPAREN) {
                throw std::runtime_error("Expected ')' after quantifier");
            }
            ParseToken();
            return f;
        }
        else if (name == "not")
        {
            Formula *f = new Formula();
            f->type = FormulaType::NOT;
            f->children.push_back(ParseFormula());
            
            if (token_type != TokenType::RPAREN) {
                throw std::runtime_error("Expected ')' after not");
            }
            ParseToken();
            return f;
        }
        else if (name == "implies")
        {
            Formula *f = new Formula();
            f->type = FormulaType::IMPLIES;
            f->children.push_back(ParseFormula());
            f->children.push_back(ParseFormula());
            
            if (token_type != TokenType::RPAREN) {
                throw std::runtime_error("Expected ')' after implies");
            }
            ParseToken();
            return f;
        }
        else if (name == "or" || name == "and")
        {
            Formula *f = new Formula();
            f->type = (name == "or") ? FormulaType::OR : FormulaType::AND;
            f->children.push_back(ParseFormula());
            f->children.push_back(ParseFormula());
            
            if (token_type != TokenType::RPAREN) {
                throw std::runtime_error("Expected ')' after binary operator");
            }
            ParseToken();
            return f;
        }
        else
        {
            Formula *f = new Formula();
            f->type = FormulaType::PREDICATE;
            f->str = name;

            while (token_type != TokenType::RPAREN) {
                f->children.push_back(ParseTerm());
            }
            
            ParseToken();
            return f;
        }
    }
    else {
        throw std::runtime_error("Expected '(' at position " + std::to_string(cur_char - input.data()));
    }
}

Formula *Parser::Parse()
{
    ParseToken();
    Formula *result = ParseFormula();
    if (token_type != TokenType::END) {
        throw std::runtime_error("Unexpected trailing characters");
    }

    return result;
}

} // namespace rzlogic
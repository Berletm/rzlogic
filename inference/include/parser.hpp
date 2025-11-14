#ifndef PARSER_HPP
#define PARSER_HPP

#include "logic.hpp"

class Parser
{
private:
    std::string input;
    size_t pos;

    inline void skip_whitespace() 
    { while (pos < input.size() and isspace(input[pos])) ++pos; }

    std::string read_atom();
    FormulaPtr  parse_term();
    FormulaPtr  parse_formula();
public:
    FormulaPtr parse(const std::string& s);
};


#endif

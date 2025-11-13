#include "parser.hpp"
#include <stdexcept>

std::string Parser::read_atom()
{
    skip_whitespace();
    size_t start = pos;

    while (pos < input.size() and !isspace(input[pos]) and input[pos] != '(' and input[pos] != ')')
    {
        pos++;
    }

    if (start == pos) throw std::runtime_error("Expected atom at pos " + std::to_string(pos));

    return input.substr(start, pos - start);
}

TermPtr Parser::parse_term()
{
    skip_whitespace();

    if (input[pos] == '(')
    {
        pos++;
        std::string name = read_atom();
        std::vector<TermPtr> args;

        while (input[pos] != ')')
        {
            args.push_back(parse_term());
        }
        pos++;

        return std::make_shared<Function>(name, std::move(args));
    }
    else 
    {
        std::string name = read_atom();

        if (name.size() == 1 and (name[0] - 109) < 0)
        {
            return std::make_shared<Constant>(name);
        }

        return std::make_shared<Variable>(name);
    }
}

FormulaPtr Parser::parse_formula()
{
    skip_whitespace();
    
    if (input[pos] == '(')
    {
        ++pos;
        std::string op = read_atom();

        if (op == "forall" or op == "exists")
        {
            std::string var  = read_atom();
            FormulaPtr body = parse_formula();
            ++pos;
            if (op == "forall") return std::make_shared<ForAll>(var, body);
            else return std::make_shared<Exists>(var, body);
        }
        else if (op == "not")
        {
            FormulaPtr f = parse_formula();
            ++pos;
            return std::make_shared<Not>(f);
        }
        else if (op == "or" || op == "and")
        {
            FormulaPtr left = parse_formula();
            FormulaPtr right = parse_formula();
            ++pos;

            if (op == "or") return std::make_shared<Or>(left, right);
            else return std::make_shared<And>(left, right);
        }
        else if (op == "implies")
        {
            FormulaPtr left = parse_formula();
            FormulaPtr right = parse_formula();
            ++pos;
            return std::make_shared<Implies>(left, right);
        }
        else
        {
            std::vector<TermPtr> args;
            while (input[pos] != ')')
            {
                args.push_back(parse_term());
            }
            ++pos;
            return std::make_shared<Atomic>(op, args);
        }
    }
    else
    {
        throw std::runtime_error("Unexpected token at position " + std::to_string(pos));
    }
}

FormulaPtr Parser::parse(const std::string& s)
{
    input = s;
    pos = 0;
    FormulaPtr result = parse_formula();
    skip_whitespace();
    if (pos != input.size()) throw std::runtime_error("Unexpected trailing characters");
    return result;
}
#include <iostream>
#include "logic.hpp"
#include "parser.hpp"


int main(int argc, char* argv[])
{
    std::vector<std::string> premises = 
    {
        "(forall x (implies (P x) (Q (f x))))",
        "(P a)",
        "(not (Q y))",
        "(exists x (or (x) (a)))"
    };

    Parser parser;
    for (const auto& s : premises) 
    {
        try 
        {
            FormulaPtr f = parser.parse(s);
            std::cout << "Parsed: " << f->to_string() << "\n";
        } 
        catch (const std::exception& e) 
        {
            std::cerr << "Parse error in '" << s << "': " << e.what() << "\n";
        }
    }

    return 0;
}
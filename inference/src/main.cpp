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

    for (const auto& s : premises) 
    {
        try 
        {
            Formula *f = Parser(s).Parse();
            std::cout << "Parsed: " << FormulaAsString(f) << "\n";
            DeleteFormula(f);
        } 
        catch (const std::exception& e) 
        {
            std::cerr << "Parse error in '" << s << "': " << e.what() << "\n";
        }
    }

    return 0;
}
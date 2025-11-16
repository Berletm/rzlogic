#include <iostream>
#include <vector>
#include <string>
#include "logic.hpp"
#include "parser.hpp"


int main(int argc, char* argv[])
{
    std::vector<std::string> premises = 
    {
        "(forall x (implies (P x) (Q (f x))))",
        "(P a)",
        "(not (Q y))",
        "(exists x (or x a))"
    };

    Formula *f = Parser(premises[0]).Parse();

    // 1. Remove implications

    NormalizeFormula(f);

    // 2. Make PNF
    
    MakePrenexNormalForm(f);

    // 3. Make SNF (remove ∃, ∀)

    MakeSkolemNormalForm(f);

    // 4. Make CNF

    MakeConjunctiveNormalForm(f);
    
    std::cout << FormulaAsString(f) << "\n";

    // 5. Make Resolution

    return 0;
}
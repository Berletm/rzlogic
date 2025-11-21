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

    rzlogic::Formula *f = rzlogic::Parser(premises[0]).Parse();

    // 1. Remove implications
    rzlogic::NormalizeFormula(f);

    // 2. Make PNF
    rzlogic::MakePrenexNormalForm(f);

    // 3. Make SNF (remove ∃, ∀)
    rzlogic::MakeSkolemNormalForm(f);

    // 4. Make CNF
    rzlogic::MakeConjunctiveNormalForm(f);
    
    std::cout << rzlogic::FormulaAsString(f) << "\n";

    // 5. Make Resolution

    return 0;
}
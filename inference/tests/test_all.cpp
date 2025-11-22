#include <gtest/gtest.h>
#include "logic.hpp"
#include "utils.hpp"
#include "parser.hpp"


TEST(AllTest, ResolutionTest)
{
    std::vector<std::string> premises = 
    {
        "(forall x (implies (H x) (M x)))",
        "(H a)",
        "(not (M a))"
    };

    std::vector<Formula*> formuls;

    for (const auto& str: premises)
    {
        Formula *f = Parser(str).Parse();

        // 1. Remove implications
        NormalizeFormula(f);

        // 2. Make PNF
        MakePrenexNormalForm(f);

        // 3. Make SNF (remove ∃, ∀)
        MakeSkolemNormalForm(f);

        // 4. Make CNF
        MakeConjunctiveNormalForm(f);

        // 5. Split conjunctions
        SplitConjunctions(f, formuls);
    }

    std::vector<ResolutionStepInfo> hist;

    bool ans = MakeResolution(formuls, hist);

    for (auto info: hist)
    {
        std::cout << "Q1: " << FormulaAsString(info.premise1) << " Q2: " << FormulaAsString(info.premise2) << " R: " << FormulaAsString(info.resolvent) << "\n";
    }

    ASSERT_EQ(ans, true);
}

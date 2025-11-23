#include <gtest/gtest.h>
#include "logic.hpp"
#include "utils.hpp"
#include "parser.hpp"

using namespace rzlogic;

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
    
    ASSERT_EQ(ans, true);
}


TEST(AllTest, ResolutionHardTest)
{
    std::vector<std::string> premises = 
    {
        "(exists x (and (P x) (forall y (implies (D y) (L x y)))))",
        "(forall x (forall y (implies (and (P x) (Z y)) (not (L x y)))))",
        "(exists x (and (D x) (Z x)))"
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
    
    ASSERT_EQ(ans, true);
}

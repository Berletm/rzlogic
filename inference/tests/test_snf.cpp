#include <gtest/gtest.h>
#include "utils.hpp"

using namespace rzlogic;

TEST(FormsTest, DropQuntifiersTest)
{
    Formula *f = ForAll("x", ForAll("y", Predicate("P", {Var("x")})));

    // removes ∀
    DropUniversalQuantifiers(f);

    ASSERT_EQ(FormulaAsString(f), "(P x)");

    DeleteFormula(f);
}

TEST(FormsTest, SkolemizationTest)
{
    Formula *f = ForAll("y", Exists("x", And(Predicate("Q", {Var("x")}), Predicate("P", {Var("y")}))));


    // removes ∃
    std::vector<std::string> vars;
    int counter = 0;
    Skolemize(f, vars, counter);

    ASSERT_EQ(FormulaAsString(f), "(forall y (and (Q (n y)) (P y)))");

    DeleteFormula(f);
}

TEST(FormsTest, MakeSkolemNormalFormTest)
{
    Formula *f = ForAll("y", Exists("x", And(Predicate("Q", {Var("x")}), Predicate("P", {Var("y")}))));

    // removes ∃ + removes ∀
    MakeSkolemNormalForm(f);

    ASSERT_EQ(FormulaAsString(f), "(and (Q (n y)) (P y))");

    DeleteFormula(f);
}

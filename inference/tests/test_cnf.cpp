#include <gtest/gtest.h>
#include "logic.hpp"
#include "utils.hpp"

TEST(FormsTest, NormalizeFormulaTest)
{
    Formula *f1 = Implies(Var("x"), Var("y"));

    NormalizeFormula(f1);

    ASSERT_EQ(FormulaAsString(f1), "(or (not x) y)");

    DeleteFormula(f1);

    Formula *f2 = ForAll("x", 
                    ForAll("y", 
                        ForAll("z", 
                            Implies(
                                And(Predicate("R", {Var("x"), Var("y")}), Predicate("R", {Var("y"), Var("z")})), 
                                    Predicate("R", {Var("x"), Var("z")})))));
    
    NormalizeFormula(f2);

    ASSERT_EQ(FormulaAsString(f2), "(forall x (forall y (forall z (or (not (and (R x y) (R y z))) (R x z)))))");
    DeleteFormula(f2);
}

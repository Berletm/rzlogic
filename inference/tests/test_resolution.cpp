#include <gtest/gtest.h>
#include "utils.hpp"

TEST(ResolutionTEST, SimpleResolver)
{
    Formula *f1 = Or(Not(Predicate("P", {Const("a")})), Predicate("Q", {Const("b")}));
    Formula *f2 = Or(Predicate("P", {Const("a")}), Predicate("Q", {Const("b")}));

    Formula *res = FindResolver(f1, f2);

    ASSERT_EQ(FormulaAsString(res), "(P a)");

    DeleteFormula(f1);
    DeleteFormula(f2);
    DeleteFormula(res);
}

TEST(ResolutionTEST, LongResolver)
{
    Formula *f1 = Or(Predicate("T", {Const("a")}),Or(Predicate("R", {Const("c")}), Or(Not(Predicate("P", {Const("a")})), Predicate("Q", {Const("b")}))));
    Formula *f2 = Or(Predicate("P", {Const("a")}), Predicate("Q", {Const("b")}));

    Formula *res = FindResolver(f1, f2);

    ASSERT_EQ(FormulaAsString(res), "(P a)");

    DeleteFormula(f1);
    DeleteFormula(f2);
    DeleteFormula(res);
}

TEST(ResoltuionTEST, RemoveResolverTest)
{
    Formula *f1 = Or(Predicate("T", {Const("a")}),Or(Predicate("R", {Const("c")}), Or(Not(Predicate("P", {Const("a")})), Predicate("Q", {Const("b")}))));
    Formula *f2 = Or(Predicate("P", {Const("a")}), Predicate("Q", {Const("b")}));

    Formula *res = FindResolver(f1, f2);

    RemoveResolver(f1, res);

    ASSERT_EQ(FormulaAsString(f1), "(or (T a) (or (R c) (Q b)))");

    DeleteFormula(f1);
    DeleteFormula(f2);
    DeleteFormula(res);
}

TEST(ResoltuionTEST, ResolutionStepTest)
{
    Formula *f1 = Or(Predicate("T", {Const("a")}),Or(Predicate("R", {Const("c")}), Or(Not(Predicate("P", {Const("a")})), Predicate("Q", {Const("b")}))));
    Formula *f2 = Or(Predicate("P", {Const("a")}), Predicate("Q", {Const("b")}));

    Formula *res  = FindResolver(f1, f2);
    Formula *step = ResolutionStep(f1, f2, res);

    ASSERT_EQ(FormulaAsString(step), "(or (T a) (or (R c) (or (Q b) (Q b))))");

    DeleteFormula(res);
    DeleteFormula(step);
}

TEST(ResoltuionTEST, ResolutionStepEmptyFormulaTest)
{
    Formula *f1 = Predicate("P", {Const("a")});
    Formula *f2 = Not(Predicate("P", {Const("a")}));

    Formula *res  = FindResolver(f1, f2);
    Formula *step = ResolutionStep(f1, f2, res);

    ASSERT_EQ(FormulaAsString(step), "");

    DeleteFormula(res);
    DeleteFormula(step);
}

TEST(ResolutionTEST, SplitConjunctionsSimpleTest)
{
    Formula *f = And(Or(Predicate("R", {Const("a")}), Predicate("P", {Const("b")})), Or(Predicate("Q", {Const("c")}), Predicate("T", {Const("d")})));

    std::vector<Formula*> premises;

    SplitConjunctions(f, premises);

    ASSERT_EQ(premises.size(), 2);
    ASSERT_EQ(FormulaAsString(premises[0]), "(or (R a) (P b))");
    ASSERT_EQ(FormulaAsString(premises[1]), "(or (Q c) (T d))");

    DeleteFormula(f);
}

TEST(ResolutionTEST, SplitConjunctionsHardTest)
{
    Formula *f = 
    And(
        And(
            Or(
                Predicate("P", {Const("a")}),
                Or(
                    Predicate("Q", {Const("b")}),
                    Predicate("Z", {Const("g")})
                )
            ),
            Or(
                Predicate("R", {Const("c")}),
                Predicate("S", {Const("d")})
            )
        ),
        Or(
            Predicate("T", {Const("e")}),
            Predicate("H", {Const("f")})
        )
    );

    std::vector<Formula*> premises;

    SplitConjunctions(f, premises);

    ASSERT_EQ(premises.size(), 3);
    ASSERT_EQ(FormulaAsString(premises[0]), "(or (P a) (or (Q b) (Z g)))");
    ASSERT_EQ(FormulaAsString(premises[1]), "(or (R c) (S d))");
    ASSERT_EQ(FormulaAsString(premises[2]), "(or (T e) (H f))");

    DeleteFormula(f);
}

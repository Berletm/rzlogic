#include <gtest/gtest.h>
#include "logic.hpp"
#include "utils.hpp"

using namespace rzlogic;

TEST(FormsTest, PNFTest)
{
    
    // ∀∃

    // ∃x P(x) or ∀x Q(x) ---> ∃x ∀x1 (Q(x1) or P(x))
    Formula *f1 = Or(
        Exists("x", Function("P", {Var("x")})),
        ForAll("x", Function("Q", {Var("x")}))
    );
    MakePrenexNormalForm(f1);
    ASSERT_EQ(FormulaAsString(f1), "(exists x (forall x1 (or (P x) (Q x1))))");
    DeleteFormula(f1);

    // !∀z S(z) ---> ∃z !S(z)
    Formula *f2 = Not(ForAll("z", Predicate("S", {Var("z")})));
    MakePrenexNormalForm(f2);
    ASSERT_EQ(FormulaAsString(f2), "(exists z (not (S z)))");
    DeleteFormula(f2);

    // !!P(c) ---> P(c)
    Formula *f3 = Not(Not(Predicate("P", {Const("c")})));
    MakePrenexNormalForm(f3);
    ASSERT_EQ(FormulaAsString(f3), "(P c)");
    DeleteFormula(f3);

    // !∀x (∃y(!∀z (P(f(x,y), z) and (Q(x) or !R(y)))) OR ∀z(∃w (S(z, h(w)) and !T(w)) ))
    Formula *f4 = Not(ForAll("x",                                             // !∀x
        Or(
            Exists("y",                                                       // ∃y
                Not(ForAll("z",                                               // !∀z
                    And(
                        Predicate("P", {                                      // P(f(x, y), z)
                            Function("f", {Var("x"), Var("y")}), Var("z")     // and
                        }),
                        Or(
                            Predicate("Q", {Var("x")}),                       // Q(x) or
                            Not(Predicate("R", {Var("y")}))                   // !R(y)
                        )
                    )
                ))                                                            // OR
            ),
            ForAll("z", Exists("w",                                           // ∀z ∃w
                    And(
                        Predicate("S", {                                      // S(z, h(w))
                            Var("z"), Function("h", {Var("w")})
                        }),                                                   // and
                        Not(Predicate("T", {Var("w")}))                       // !T(w)
                    )
                )
            )
        )
    ));

    // !∀x (∃y(∃z (!P(f(x,y), z) or (!Q(x) and R(y)))) OR ∀z(∃w (S(z, h(w)) and !T(w)) ))
    // !∀x (∃y(∃z (!P(f(x,y), z) or (!Q(x) and R(y)))) OR ∀z1(∃w (S(z1, h(w)) and !T(w)) ))
    // ∃x (!∃y(∃z (!P(f(x,y), z) or (!Q(x) and R(y)))) AND !∀z1(∃w (S(z1, h(w)) and !T(w)) ))
    // ∃x (∀y!(∃z (!P(f(x,y), z) or (!Q(x) and R(y)))) AND ∃z1!(∃w (S(z1, h(w)) and !T(w)) ))
    // ∃x (∀y (∀z (P(f(x,y), z) and (Q(x) or !R(y)))) AND ∃z1!(∃w (S(z1, h(w)) and !T(w)) ))
    // ∃x (∀y (∀z (P(f(x,y), z) and (Q(x) or !R(y)))) AND ∃z1(∀w (!S(z1, h(w)) or T(w)) ))
    // ∃x (∀y (∀z ∃z1 (P(f(x,y), z) and (Q(x) or !R(y)))) AND (∀w (!S(z1, h(w)) or T(w)) ))
    // ∃x (∀y (∀z ∃z1 ∀w (P(f(x,y), z) and (Q(x) or !R(y))) AND ((!S(z1, h(w)) or T(w)) ) ))
    // ???? OK?

    MakePrenexNormalForm(f4);
    ASSERT_EQ(FormulaAsString(f4), "(exists x (forall y (forall z (exists z1 (forall w (and (and (P (f x y) z) (or (Q x) (not (R y)))) (or (not (S z1 (h w))) (T w))))))))");
    DeleteFormula(f4);
}

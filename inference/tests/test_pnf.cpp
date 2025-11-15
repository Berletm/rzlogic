#include <gtest/gtest.h>
#include "logic.hpp"

Formula *And(Formula *A, Formula *B)
{
    Formula *res = new Formula(FormulaType::AND);
    res->children.push_back(A);
    res->children.push_back(B);
    return res;
}

Formula *Or(Formula *A, Formula *B)
{
    Formula *res = new Formula(FormulaType::OR);
    res->children.push_back(A);
    res->children.push_back(B);
    return res;
}

Formula *ForAll(std::string var, Formula *A)
{
    Formula *res = new Formula(FormulaType::FORALL, var);
    res->children.push_back(A);
    return res;
}

Formula *Exists(std::string var, Formula *A)
{
    Formula *res = new Formula(FormulaType::EXISTS, var);
    res->children.push_back(A);
    return res;
}

Formula *Not(Formula *A)
{
    Formula *res = new Formula(FormulaType::NOT);
    res->children.push_back(A);
    return res;
}

Formula *Predicate(std::string name, std::initializer_list<Formula*> terms) {
    Formula *res = new Formula(FormulaType::PREDICATE, name);
    for (auto f : terms) {
        res->children.push_back(f);
    }
    return res;
}

Formula *Function(std::string name, std::initializer_list<Formula*> terms) {
    Formula *res = new Formula(FormulaType::FUNCTION, name);
    for (auto f : terms) {
        res->children.push_back(f);
    }
    return res;
}

Formula *Var(std::string name) {
    return new Formula(FormulaType::VARIABLE, name);
}

Formula *Const(std::string name) {
    return new Formula(FormulaType::CONSTANT, name);
}

TEST(ParserTest, PNFTest)
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
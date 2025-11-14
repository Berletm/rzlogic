#include <gtest/gtest.h>
#include "logic.hpp"

#define $ std::make_shared
#define VAR(x) std::make_shared<BasicTerm>(x, BasicTerm::Type::VARIABLE)
#define CONST(x) std::make_shared<BasicTerm>(x, BasicTerm::Type::CONSTANT)

TEST(ParserTest, PNFTest)
{
    using V = std::vector<FormulaPtr>;
    using T = BasicTerm;
    
    // ∀∃

    // ∃x P(x) or ∃x Q(x)
    FormulaPtr f1 = $<Or>(
        $<Exists>("x", $<Function>("P", V{VAR("x")})),
        $<ForAll>("x", $<Function>("Q", V{VAR("x")}))
    );
    ASSERT_EQ(MakePrenexNormalForm(f1)->to_string(), "(exists x (forall x1 (or (P x)(Q x1))))");

    // !∀z S(z)   ---> ∃z !S(z)
    FormulaPtr f2 = $<Not>($<ForAll>("z", $<Function>("S", V{VAR("z")})));
    ASSERT_EQ(MakePrenexNormalForm(f2)->to_string(), "(exists z (not (S z)))");

    // !!P(c)
    FormulaPtr f3 = $<Not>($<Not>($<Function>("P", V{CONST("c")})));
    ASSERT_EQ(MakePrenexNormalForm(f3)->to_string(), "(P c)");

    // !∀x (∃y(!∀z (P(f(x,y), z) and (Q(x) or !R(y)))) OR ∀z(∃w (S(z, h(w)) and !T(w)) ))
    FormulaPtr f4 = $<Not>($<ForAll>("x",                                           // !∀x
        $<Or>(
            $<Exists>("y",                                                          // ∃y
                $<Not>($<ForAll>("z",                                               // !∀z
                    $<And>(
                        $<Function>("P", V{                                         // P(f(x, y), z)
                            $<Function>("f", V{VAR("x"), VAR("y")}), VAR("z")       // and
                        }),
                        $<Or>(
                            $<Function>("Q", V{VAR("x")}),                          // Q(x) or
                            $<Not>($<Function>("R", V{VAR("y")}))                   // !R(y)
                        )
                    )
                ))                                                                  // OR
            ),
            $<ForAll>("z", $<Exists>("w",                                           // ∀z ∃w
                    $<And>(
                        $<Function>("S", V{                                         // S(z, h(w))
                            VAR("z"), $<Function>("h", V{VAR("w")})
                        }),                                                         // and
                        $<Not>($<Function>("T", V{VAR("w")}))                       // !T(w)
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

    ASSERT_EQ(MakePrenexNormalForm(f4)->to_string(), "(exists x (forall y (forall z (exists z1 (forall w (and (and (P (f x y) z)(or (Q x)(not (R y))))(or (not (S z1 (h w)))(T w))))))))");
}
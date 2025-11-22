#include <gtest/gtest.h>
#include "logic.hpp"
#include "utils.hpp"

using namespace rzlogic;

std::string MappingsAsString(std::map<std::string, Formula*> &mappings)
{
    std::string result = "{";
    bool first = true;
    for (auto &[var_name, formula] : mappings)
    {
        if (first) first = false;
        else result += ", ";
        result += var_name + " = " + FormulaAsString(formula);
    }
    result += "}";
    return result;
}

class UnificationTest : public testing::Test {
private:
    Formula *p1_save, *p2_save;
    std::map<std::string, Formula*> expected_mappings_save;
    std::map<std::string, Formula*> mappings;

protected:
    void Check(Formula *p1, Formula *p2, bool expected_result, std::map<std::string, Formula*> expected_mappings)
    {
        p1_save = p1;
        p2_save = p2;
        expected_mappings_save = expected_mappings;

        bool result = MapPredicateToPredicate(p1, p2, mappings);

        ASSERT_EQ(result, expected_result) << MappingsAsString(mappings);
        if (result)
        {
            ASSERT_EQ(mappings.size(), expected_mappings.size())
                << "Got: " << MappingsAsString(mappings) \
                << ". Expected: " << MappingsAsString(mappings);
            
            for (auto &[res_var_name, res_formula] : mappings)
            {
                auto it = expected_mappings.find(res_var_name);
                ASSERT_TRUE(it != expected_mappings.end())
                    << "Unexpected mapping " << res_var_name << " = " << FormulaAsString(res_formula);

                ASSERT_TRUE(FormulasEqual(res_formula, it->second))
                    << "Expected mapping " << res_var_name << " = " << FormulaAsString(it->second)
                    << " but got " << FormulaAsString(res_formula);
            }

            for (auto &[exp_var_name, exp_formula] : expected_mappings)
            {
                auto it = mappings.find(exp_var_name);
                ASSERT_TRUE(it != mappings.end())
                    << "Missing mapping " << exp_var_name << " = " << FormulaAsString(exp_formula);
            }
        }   
    }

    void TearDown() override
    {
        DeleteFormula(p1_save);
        DeleteFormula(p2_save);
        
        for (auto &[var_name, formula] : expected_mappings_save)
            DeleteFormula(formula);

        for (auto &[var_name, formula] : mappings)
            DeleteFormula(formula);
    }
};


TEST_F(UnificationTest, OkPredicate)
{
    
    // P(f(x, b))
    // P(f(g(y), z))
    // x -> g(y)
    // z -> b
    Check(
        Predicate("P", {Function("f", {Var("x"),                  Const("b") })}),
        Predicate("P", {Function("f", {Function("g", {Var("y")}), Var("z")   })}),
        true,
        {
            {"x", Function("g", {Var("y")})},
            {"z", Const("b")}
        }
    );
}

TEST_F(UnificationTest, WrongArgsCountPredicate)
{
    
    // P(x, y)
    // P(z)
    Check(
        Predicate("P", {Var("x"), Var("y")}),
        Predicate("P", {Var("z")}),
        false, {}
    );
}

TEST_F(UnificationTest, WrongArgsCountFunction)
{
    
    // f(x, y)
    // f(z)
    Check(
        Function("f", {Var("x"), Var("y")}),
        Function("f", {Var("z")}),
        false, {}
    );
}

TEST_F(UnificationTest, RecursiveFail)
{
    
    // x
    // f(g(x))
    Check(
        Var("x"),
        Function("f", {Function("g", {Var("x")})}),
        false, {}
    );
}

TEST_F(UnificationTest, ComposeMappings)
{
    
    // P(x, y)
    // P(f(y), a)
    // x --> f(a)
    // y --> a
    Check(
        Predicate("P", {Var("x"), Var("y")}),
        Predicate("P", {Function("f", {Var("y")}), Const("a")}),
        true,
        {
            {"x", Function("f", {Const("a")})},
            {"y", Const("a")}
        }
    );

    // сначала делается замена x = f(y)
    // потом при y = a не забываем сделать x = f(a)
}

TEST_F(UnificationTest, ApplyMapping)
{
    // P(x, x)
    // P(f(a), c)
    Check(
        Predicate("P", {Var("x"), Var("x")}),
        Predicate("P", { Function("f", {Const("a")}), Const("c") }),
        false, {}
    );

    // заменяем x --> f(a)
    // потом видим x и c, но уже сделали замену, поэтому никак...
}

TEST_F(UnificationTest, Huge)
{
    // пример с практики самый первый
    Formula *f1 = Predicate("P", {
        Function("p", {
            Function("q", {
                Function("f", {Const("a")})
            })
        }),
        Function("p", {
            Function("p", {
                Function("p", {Var("x")})
            })
        }),
        Function("p", {
            Function("r", {
                Var("u"),
                Function("h", {Var("u")}),
                Function("g", {Var("u")})
            })
        }),
        Function("p", {
            Function("q", {
                Function("p", {
                    Function("h", {Var("u")})
                })
            })
        }),
        Function("g", {Var("t")})
    });

    Formula *f2 = Predicate("P", {
        Function("p", {
            Function("q", {Var("y")})
        }),
        Function("p", {
            Function("p", {
                Function("p", {
                    Function("s", {Var("y")})
                })
            })
        }),
        Function("p", {
            Function("r", {
                Const("c"),
                Var("z"),
                Function("g", {Const("c")})
            })
        }),
        Function("p", {
            Function("q", {
                Function("p", {Var("z")})
            })
        }),
        Function("g", {
            Function("psi", {Const("b")})    
        })
    });

    Check(
        f1,
        f2,
        true,
        {
            {"y", Function("f", {Const("a")})},
            {"x", Function("s", {Function("f", {Const("a")})})},
            {"u", Const("c")},
            {"z", Function("h", {Const("c")})},
            {"t", Function("psi", {Const("b")})}
        }
    );
}
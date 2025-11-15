#include <gtest/gtest.h>
#include "parser.hpp"
#include <memory>

TEST(ParserTest, BigFormulaTest)
{
    std::vector<std::string> premises = 
    {
        "(exists x (and (Patient x) (forall y (implies (Doctor y) (Love x y)))))",
        "(forall x (forall z (implies (and (Patient x) (Healer z))(not (Love x z)))))",
        "(forall y (implies (Doctor y) (not (Healer y))))"
    };

    Formula *f1 = Parser(premises[0]).Parse();
    ASSERT_TRUE(f1 != nullptr);
    ASSERT_EQ(FormulaAsString(f1), premises[0]);

    //Formula *f2 = Parser(premises[1]).Parse();
    //ASSERT_TRUE(f2 != nullptr);
    //ASSERT_EQ(FormulaAsString(f2), premises[0]);
//
    //Formula *f3 = Parser(premises[2]).Parse();
    //ASSERT_TRUE(f3 != nullptr);
    //ASSERT_EQ(FormulaAsString(f3), premises[0]);

    DeleteFormula(f1);
    //DeleteFormula(f2);
    //DeleteFormula(f3);
}
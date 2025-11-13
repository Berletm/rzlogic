#include <gtest/gtest.h>
#include "parser.hpp"
#include <memory>

TEST(ParserTest, BigFormulaTest)
{
    Parser parser;

    std::vector<std::string> premises = 
    {
        "(exists x (and (Patient x)(forall y (implies (Doctor y)(Love x y)))))",
        "(forall x (forall z (implies (and (Patient x)(Healer z))(not (Love x z)))))",
        "(forall y (implies (Doctor y)(not (Healer y))))"
    };

    auto f = parser.parse(premises[0]);
    ASSERT_TRUE(f != nullptr);
    ASSERT_EQ(f->to_string(), premises[0]);

    f = parser.parse(premises[1]);
    ASSERT_TRUE(f != nullptr);
    ASSERT_EQ(f->to_string(), premises[1]);

    f = parser.parse(premises[2]);
    ASSERT_TRUE(f != nullptr);
    ASSERT_EQ(f->to_string(), premises[2]);
}
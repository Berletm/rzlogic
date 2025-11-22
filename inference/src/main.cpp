#include <iostream>
#include <vector>
#include <string>
#include "logic.hpp"
#include "../tests/utils.hpp"
#include "parser.hpp"

int main(int argc, char* argv[])
{
    std::vector<std::string> premises = 
    {
        "(forall x (implies (P x) (Q (f x))))",
        "(P a)",
        "(not (Q y))",
        "(exists x (or x a))"
    };
    
    return 0;
}
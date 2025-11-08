#include <iostream>
#include "../include/logic.hpp"


int main(int argc, char* argv[])
{
    auto a = std::make_shared<Constant>("a");
    auto b = std::make_shared<Constant>("b");

    auto f_b = std::make_shared<Function>("f", std::vector<TermPtr>{b});

    std::vector<TermPtr> args = {a, f_b};

    auto atom = std::make_shared<Atomic>("R", args);

    auto not_atom = std::make_shared<Not>(atom);
    auto forall_x = std::make_shared<ForAll>("x", not_atom);

    std::cout << forall_x->to_string() << std::endl;

    return 0;
}
#include "utils.hpp"

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

Formula *Implies(Formula *A, Formula *B)
{
    Formula *res = new Formula(FormulaType::IMPLIES);
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

Formula *Predicate(std::string name, std::initializer_list<Formula*> terms) 
{
    Formula *res = new Formula(FormulaType::PREDICATE, name);
    for (auto f : terms) {
        res->children.push_back(f);
    }
    return res;
}

Formula *Function(std::string name, std::initializer_list<Formula*> terms) 
{
    Formula *res = new Formula(FormulaType::FUNCTION, name);
    for (auto f : terms) {
        res->children.push_back(f);
    }
    return res;
}

Formula *Var(std::string name) 
{
    return new Formula(FormulaType::VARIABLE, name);
}

Formula *Const(std::string name) 
{
    return new Formula(FormulaType::CONSTANT, name);
}

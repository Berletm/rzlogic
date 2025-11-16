#ifndef UTILS_HPP
#define UTILS_HPP
#include "logic.hpp"

Formula *And(Formula *A, Formula *B);

Formula *Or(Formula *A, Formula *B);

Formula *Implies(Formula *A, Formula *B);

Formula *ForAll(std::string var, Formula *A);

Formula *Exists(std::string var, Formula *A);

Formula *Not(Formula *A);

Formula *Predicate(std::string name, std::initializer_list<Formula*> terms);

Formula *Function(std::string name, std::initializer_list<Formula*> terms);

Formula *Var(std::string name);

Formula *Const(std::string name);

#endif

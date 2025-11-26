#ifndef UTILS_HPP
#define UTILS_HPP
#include "logic.hpp"

rzlogic::Formula *And(rzlogic::Formula *A, rzlogic::Formula *B);
rzlogic::Formula *Or(rzlogic::Formula *A, rzlogic::Formula *B);
rzlogic::Formula *Implies(rzlogic::Formula *A, rzlogic::Formula *B);
rzlogic::Formula *ForAll(std::string var, rzlogic::Formula *A);
rzlogic::Formula *Exists(std::string var, rzlogic::Formula *A);
rzlogic::Formula *Not(rzlogic::Formula *A);
rzlogic::Formula *Predicate(std::string name, std::initializer_list<rzlogic::Formula*> terms);
rzlogic::Formula *Function(std::string name, std::initializer_list<rzlogic::Formula*> terms);
rzlogic::Formula *Var(std::string name);
rzlogic::Formula *Const(std::string name);

#endif

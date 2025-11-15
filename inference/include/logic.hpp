#ifndef LOGIC_HPP
#define LOGIC_HPP

#include <string>
#include <vector>
#include <memory>
#include <iostream>
#include <functional>
#include <unordered_set>

// X(enum_name, str_name)
#define FOR_ALL_FORMULA_TYPES \
    X(NOT, "not") \
    X(AND, "and") \
    X(OR, "or") \
    X(IMPLIES, "implies") \
    X(EXISTS, "exists") \
    X(FORALL, "forall") \
    X(PREDICATE, "predicate") \
    X(FUNCTION, "function") \
    X(VARIABLE, "variable") \
    X(CONSTANT, "constant")

enum class FormulaType {
    #define X(enum_name, str_name) enum_name,
    FOR_ALL_FORMULA_TYPES
    #undef X
};


struct Formula {
    FormulaType type;
    std::vector<Formula*> children;
    std::string str; // function name / predicate name / variable name / constant name

    Formula(FormulaType type, std::string str) : type(type), str(str) {}
    Formula(FormulaType type) : type(type) {}
    Formula() {}
};

std::string FormulaAsString(Formula *f);
void        DeleteFormula(Formula *f);
void        MakePrenexNormalForm(Formula *f);
void DoForAll(Formula *f, std::function<void(Formula*)> func);
Formula* CloneFormula(Formula* f);

// Преобразования
void RemoveImplications(Formula* f);
void ToNNF(Formula* f);
void PushNegations(Formula *f);
void MakePrenexNormalForm(Formula *f);
void Skolemize(Formula* f, std::vector<std::string>& universal_vars, int& skolem_counter);
void DropUniversalQuantifiers(Formula* f, std::vector<std::string>& free_vars);
Formula* AddUniversalQuantifiers(Formula* f, const std::vector<std::string>& free_vars);
void ToCNF(Formula* f);
void SimplifyFormula(Formula* f);
Formula* ToSkolemNormalForm(Formula* f);

#endif
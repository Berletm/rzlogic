#ifndef LOGIC_HPP
#define LOGIC_HPP

#include <map>
#include <string>
#include <vector>

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
    X(CONSTANT, "constant") \
    X(EMPTY, "")

enum class FormulaType {
    #define X(enum_name, str_name) enum_name,
    FOR_ALL_FORMULA_TYPES
    #undef X
};


struct Formula 
{
    FormulaType type = FormulaType::EMPTY;
    std::vector<Formula*> children;
    std::string str; // function name / predicate name / variable name / constant name

    Formula(FormulaType type, std::string str) : type(type), str(str) {}
    Formula(FormulaType type) : type(type) {}
    Formula() {}
};

struct ResolutionStepInfo
{
    Formula *premise1;
    Formula *premise2;
    Formula *resolvent;
};

// PNF
std::string FormulaAsString(Formula *f);
void        DeleteFormula(Formula *f);
void        MakePrenexNormalForm(Formula *f);

// SNF
void        Skolemize(Formula *f, std::vector<std::string> &universal_vars, int &skolem_counter);
void        DropUniversalQuantifiers(Formula *f);
void        MakeSkolemNormalForm(Formula *f);

// CNF
Formula*    CloneFormula(Formula *f);
void        NormalizeFormula(Formula* f);
void        MakeConjunctiveNormalForm(Formula *f);

// Unification
bool FormulasEqual(Formula *f1, Formula *f2);
bool MapPredicateToPredicate(Formula *p1, Formula *p2, std::map<std::string, Formula*> &mappings);
bool Unificate(Formula *p1, Formula *p2);

// Resolution
void     SplitConjunctions(Formula *f, std::vector<Formula*> &premises);
Formula *FindResolver(Formula *f1, Formula *f2);
void     RemoveResolver(Formula *f, Formula *resolver);
Formula *ResolutionStep(Formula *f1, Formula *f2, Formula *resolver);
bool     IsTautology(Formula *f);
bool     MakeResolution(std::vector<Formula*> &premises, std::vector<ResolutionStepInfo> &history);

#endif
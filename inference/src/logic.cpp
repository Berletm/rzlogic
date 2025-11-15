#include "logic.hpp"
#include <functional>

std::string FunctionAsString(std::string_view name, std::vector<Formula*> &args)
{
    std::string result = "(" + std::string(name);
    for (Formula *arg : args) {
        result += " " + FormulaAsString(arg);
    }

    result += ")";
    return std::move(result);
}

std::string GetFormulaTypeStr(FormulaType type)
{
    switch (type) {
        #define X(type, str) case FormulaType::type: return str;
        FOR_ALL_FORMULA_TYPES
        #undef X
    }

    return "";
}

std::string FormulaAsString(Formula *f)
{
    switch (f->type) {
    case FormulaType::NOT:
    case FormulaType::AND:
    case FormulaType::OR:
    case FormulaType::IMPLIES:
        return FunctionAsString(GetFormulaTypeStr(f->type), f->children);

    case FormulaType::EXISTS:
    case FormulaType::FORALL:
        return FunctionAsString(GetFormulaTypeStr(f->type) + " " + f->str, f->children);
    
    case FormulaType::PREDICATE:
    case FormulaType::FUNCTION:
        return FunctionAsString(f->str, f->children);

    case FormulaType::VARIABLE:
    case FormulaType::CONSTANT:
        return f->str;
    }
    return "";
}

void DeleteFormula(Formula *f)
{
    for (Formula *child : f->children) {
        DeleteFormula(child);
    }
    delete f;
}

void DoForAll(Formula *f, std::function<void(Formula*)> func)
{
    std::vector<Formula*> stack;

    stack.push_back(f);
    while (!stack.empty()) {
        Formula *formula = stack.back();
        stack.pop_back();

        func(formula);

        for (Formula *child : formula->children) {
            stack.push_back(child);
        }
    }
}

void PushNegations(Formula *f)
{
    if (f->type == FormulaType::NOT) {
        Formula *child = f->children[0];
        
        switch (child->type) {
        case FormulaType::NOT: // !!A = A
            {
                Formula *A = child->children[0];
                *f = std::move(*A);
                delete child;
                delete A;
                break;
            }

        case FormulaType::OR:   // !(A v B) = !A ^ !B
        case FormulaType::AND:  // !(A ^ B) = !A v !B
            {
                Formula *A = child->children[0], *B = child->children[1];
                
                Formula *not_A = new Formula(FormulaType::NOT); not_A->children.push_back(A);
                Formula *not_B = new Formula(FormulaType::NOT); not_B->children.push_back(B);
                
                f->type = (child->type == FormulaType::OR) ? FormulaType::AND : FormulaType::OR;
                f->children.resize(2);
                f->children[0] = not_A;
                f->children[1] = not_B;
                delete child;
                break;
            }
        case FormulaType::FORALL:   // !forall_x A = exists_x !A
        case FormulaType::EXISTS:   // !exists_x A = forall_x !A
            {
                Formula *A = child->children[0];
                
                f->type = (child->type == FormulaType::FORALL) ? FormulaType::EXISTS : FormulaType::FORALL;
                f->str = child->str;
                child->type = FormulaType::NOT;
                child->str.clear();
                break;
            }
        default:
            break;
        }
    }

    for (Formula *child : f->children) {
        PushNegations(child);
    }
}

std::string GenerateUniqueName(std::string name, std::vector<std::string> &names)
{
    std::string cur_name = name;
    int name_idx = 0;

    for (int i = 0; i < names.size(); i++) {
        if (names[i] == cur_name) {
            name_idx++;
            cur_name = name + std::to_string(name_idx);
            i = 0;
        }
    }

    return cur_name;
}

void RenameVariable(Formula *f, std::string &old_name, std::string &new_name)
{
    DoForAll(f, [&old_name, &new_name](Formula *ff) {
        if (ff->type == FormulaType::VARIABLE && ff->str == old_name) {
            ff->str = new_name;
        }
    });
}

void UnifyNames(Formula *f, std::vector<std::string> &names)
{
    if (f->type == FormulaType::FORALL || f->type == FormulaType::EXISTS) {
        std::string new_name = GenerateUniqueName(f->str, names);
        names.push_back(new_name);
        RenameVariable(f->children[0], f->str, new_name);
        f->str = new_name;
    }

    for (Formula *child : f->children) {
        UnifyNames(child, names);
    }
}

enum class Operation {
    And, Or
};

void ExtractQuantifiers(Formula *f)
{
    Formula *A = f->children[0];
    Formula *B = f->children[1];
    FormulaType op = f->type;

    if (A->type == FormulaType::FORALL || A->type == FormulaType::EXISTS) {
        // A v B = (forall_x Q) v B   --->   forall_x (Q v B)
        Formula *Q = A->children[0];

        f->type = A->type;
        f->str  = A->str;
        f->children.resize(1);
        f->children[0] = A;

        A->type = op;
        A->str.clear();
        A->children.resize(2);
        A->children[0] = Q;
        A->children[1] = B;

        ExtractQuantifiers(A);
        return;
    }

    if (B->type == FormulaType::FORALL || B->type == FormulaType::EXISTS) {
        // A v B = A v (forall_x Q)   --->   forall_x (A v Q)
        Formula *Q = B->children[0];

        f->type = B->type;
        f->str  = B->str;
        f->children.resize(1);
        f->children[0] = B;

        B->type = op;
        B->str.clear();
        B->children.resize(2);
        B->children[0] = A;
        B->children[1] = Q;

        ExtractQuantifiers(B);
        return;
    }
}

void MoveQuantifiers(Formula *f)
{
    for (Formula *child : f->children) {
        MoveQuantifiers(child);
    }

    if (f->type == FormulaType::AND || f->type == FormulaType::OR) {
        ExtractQuantifiers(f);   
    }
}

void MakePrenexNormalForm(Formula *f)
{
    std::vector<std::string> names;

    UnifyNames(f, names);
    PushNegations(f);
    MoveQuantifiers(f);
}
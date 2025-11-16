#include "logic.hpp"
#include <functional>
#include <iostream>
#include <algorithm>
#include <unordered_map>

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
    if (f->type == FormulaType::NOT) 
    {
        Formula *child = f->children[0];
        
        switch (child->type) 
        {
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

    for (Formula *child : f->children) 
    {
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

void  MakePrenexNormalForm(Formula *f)
{
    std::vector<std::string> names;

    UnifyNames(f, names);
    PushNegations(f);
    MoveQuantifiers(f);
}

void ReplaceVariable(Formula *f, const std::string &old_var, Formula *new_term, const std::vector<std::string> &bound_vars) 
{
    if (!f) return;
 
    if ((f->type == FormulaType::FORALL || f->type == FormulaType::EXISTS) && f->str == old_var) 
    {
        return;
    }

    if (f->type == FormulaType::VARIABLE && f->str == old_var) 
    {
        if (std::find(bound_vars.begin(), bound_vars.end(), old_var) == bound_vars.end()) 
        {
            Formula* clone = CloneFormula(new_term);
            *f = *clone;
            delete clone;
        }
    } 
    else 
    {
        for (Formula* child : f->children) 
        {
            ReplaceVariable(child, old_var, new_term, bound_vars);
        }
    }
}

void Skolemize(Formula *f, std::vector<std::string> &universal_vars, int &skolem_counter) 
{
    if (!f) return;
    
    if (f->type == FormulaType::EXISTS) 
    {
        std::string var_name = f->str;
        Formula* body = f->children[0];

        Formula* skolem_term;
        
        if (universal_vars.empty()) 
        {
            skolem_term = new Formula(FormulaType::CONSTANT);
            skolem_term->str = char(97 + skolem_counter++);
        }
        else 
        {
            skolem_term = new Formula(FormulaType::FUNCTION);
            skolem_term->str = char(110 + skolem_counter++);
            
            for (const auto& uv : universal_vars) 
            {
                Formula* var = new Formula(FormulaType::VARIABLE);
                var->str = uv;
                skolem_term->children.push_back(var);
            }
        }

        ReplaceVariable(body, var_name, skolem_term, universal_vars);

        f->type = body->type;
        f->str = body->str;
        f->children = body->children;

        delete body;
        delete skolem_term;

        Skolemize(f, universal_vars, skolem_counter);
    }
    else if (f->type == FormulaType::FORALL) 
    {
        universal_vars.push_back(f->str);
        Skolemize(f->children[0], universal_vars, skolem_counter);
        universal_vars.pop_back();
    }
    else 
    {
        for (Formula* child : f->children) 
        {
            Skolemize(child, universal_vars, skolem_counter);
        }
    }
}

void DropUniversalQuantifiers(Formula *f) 
{
    if (!f) return;
    
    if (f->type == FormulaType::FORALL) 
    {
        Formula* body = f->children[0];
    
        f->type = body->type;
        f->str = std::move(body->str);
        f->children = std::move(body->children);
        
        delete body;

        DropUniversalQuantifiers(f);
    }
    else 
    {
        for (Formula* child : f->children) 
        {
            DropUniversalQuantifiers(child);
        }
    }
}

void DistributeOrOverAnd(Formula *f) 
{
    if (!f) return;

    for (Formula* child : f->children) {
        DistributeOrOverAnd(child);
    }
    
    if (f->type == FormulaType::OR) {
        Formula* left = f->children[0];
        Formula* right = f->children[1];
        
        // A ∨ (B ∧ C) = (A ∨ B) ∧ (A ∨ C)
        if (right->type == FormulaType::AND) {
            Formula* A = left;
            Formula* B = right->children[0];
            Formula* C = right->children[1];
            
            Formula* or1 = new Formula(FormulaType::OR);
            or1->children.push_back(CloneFormula(A));
            or1->children.push_back(CloneFormula(B));
            
            Formula* or2 = new Formula(FormulaType::OR);
            or2->children.push_back(CloneFormula(A));
            or2->children.push_back(CloneFormula(C));
            
            DeleteFormula(f->children[0]);
            DeleteFormula(f->children[1]);
            f->children.clear();
            
            f->type = FormulaType::AND;
            f->children.push_back(or1);
            f->children.push_back(or2);
        }
        // (A ∧ B) ∨ C = (A ∨ C) ∧ (B ∨ C)
        else if (left->type == FormulaType::AND) {
            Formula* A = left->children[0];
            Formula* B = left->children[1];
            Formula* C = right;
            
            Formula* or1 = new Formula(FormulaType::OR);
            or1->children.push_back(CloneFormula(A));
            or1->children.push_back(CloneFormula(C));
            
            Formula* or2 = new Formula(FormulaType::OR);
            or2->children.push_back(CloneFormula(B));
            or2->children.push_back(CloneFormula(C));

            DeleteFormula(f->children[0]);
            DeleteFormula(f->children[1]);
            f->children.clear();
            
            f->type = FormulaType::AND;
            f->children.push_back(or1);
            f->children.push_back(or2);
        }
    }
}

void ToCNF(Formula *f) 
{
    if (!f) return;

    bool changed;
    do {
        changed = false;

        std::function<bool(Formula*)> needsDistribution = [&](Formula* formula) -> bool {
            if (!formula) return false;
            
            if (formula->type == FormulaType::OR) {
                for (Formula* child : formula->children) {
                    if (child->type == FormulaType::AND) {
                        return true;
                    }
                    if (needsDistribution(child)) {
                        return true;
                    }
                }
            }
            return false;
        };
        
        if (needsDistribution(f)) {
            DistributeOrOverAnd(f);
            changed = true;
        }
    } while (changed);
}

Formula* CloneFormula(Formula *f) 
{
    if (!f) return nullptr;
    
    Formula* new_f = new Formula(f->type, f->str);
    for (Formula* child : f->children) {
        new_f->children.push_back(CloneFormula(child));
    }
    return new_f;
}

void MakeSkolemNormalForm(Formula *f) 
{
    std::vector<std::string> universal_vars;
    int skolem_counter = 0;

    Skolemize(f, universal_vars, skolem_counter);

    DropUniversalQuantifiers(f);
}
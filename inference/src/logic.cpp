#include "logic.hpp"
#include <functional>
#include <algorithm>

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
    case FormulaType::EMPTY:
        return f->str;
    }
    return "";
}

void DeleteFormula(Formula *f)
{
    for (Formula *child : f->children) 
    {
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
            std::vector<Formula*> old_children = f->children;
            for (Formula* old_child : old_children) 
            {
                DeleteFormula(old_child);
            }

            f->str = new_term->str;
            f->type = new_term->type;
            f->children = new_term->children;
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

void MakeSkolemNormalForm(Formula *f) 
{
    std::vector<std::string> universal_vars;
    int skolem_counter = 0;

    Skolemize(f, universal_vars, skolem_counter);

    DropUniversalQuantifiers(f);
}

void DistributeOrOverAnd(Formula *f) 
{
    if (!f) return;

    for (Formula* child : f->children) 
    {
        DistributeOrOverAnd(child);
    }
    
    if (f->type == FormulaType::OR) 
    {
        Formula* left = f->children[0];
        Formula* right = f->children[1];
        
        // A ∨ (B ∧ C) = (A ∨ B) ∧ (A ∨ C)
        // (OR A (AND В С))
        if (right->type == FormulaType::AND) 
        {
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
        // (OR (AND A B) C)
        else if (left->type == FormulaType::AND) 
        {
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

void MakeConjunctiveNormalForm(Formula *f) 
{
    if (!f) return;

    bool changed;
    do {
        changed = false;

        std::function<bool(Formula*)> needsDistribution = [&](Formula* formula) -> bool 
        {
            if (!formula) return false;
            
            if (formula->type == FormulaType::OR) 
            {
                for (Formula* child : formula->children) 
                {
                    if (child->type == FormulaType::AND) 
                    {
                        return true;
                    }
                    if (needsDistribution(child)) 
                    {
                        return true;
                    }
                }
            }
            return false;
        };
        
        if (needsDistribution(f)) 
        {
            DistributeOrOverAnd(f);
            changed = true;
        }
    } while (changed);
}

Formula* CloneFormula(Formula *f) 
{
    if (!f) return nullptr;
    
    Formula* new_f = new Formula(f->type, f->str);

    for (Formula* child : f->children) 
    {
        new_f->children.push_back(CloneFormula(child));
    }
    return new_f;
}

void NormalizeFormula(Formula *f)
{
    if (!f) return;

    for (Formula *child: f->children)
    {
        NormalizeFormula(child);
    }

    // A -> B = !A ∨ B
    // (IMPLIES A B) = (OR (NOT A) B)
    if (f->type == FormulaType::IMPLIES)
    {
        Formula *left = f->children[0];
        Formula *right = f->children[1];

        f->type = FormulaType::OR;
        f->children.clear();

        Formula *not_left = new Formula(FormulaType::NOT);
        not_left->children.push_back(left);

        f->children.push_back(not_left);
        f->children.push_back(right);
    }
}

bool FormulaContainsVariable(Formula *f, const std::string &var)
{
    std::vector<Formula*> stack;
    stack.push_back(f);

    while (!stack.empty()) {
        Formula *cur = stack.back();
        stack.pop_back();

        if (cur->type == FormulaType::VARIABLE && cur->str == var) {
            return true;
        }

        for (Formula *child : cur->children) {
            stack.push_back(child);
        }
    }

    return false;
}

void ApplyMapping(Formula *p1, std::map<std::string, Formula*> &mappings)
{
    DoForAll(p1, [&mappings](Formula* f) {
        if (f->type != FormulaType::VARIABLE) return;
        auto it = mappings.find(f->str);
        if (it == mappings.end()) return;

        Formula *cloned = CloneFormula(it->second);
        *f = std::move(*cloned);
        delete cloned; // stupid
    });
}

void UpdateMappings(std::map<std::string, Formula*> &mappings, std::map<std::string, Formula*> &new_mappings)
{
    for (auto& [var_name, formula] : mappings) {
        ApplyMapping(formula, new_mappings);
    }

    for (auto& [var_name, formula] : new_mappings) {
        if (mappings.find(var_name) == mappings.end()) {
            mappings[var_name] = formula;
        }
    }

    // remove x --> x
    for (auto it = mappings.begin(); it != mappings.end();) {
        if (it->second->type == FormulaType::VARIABLE && it->first == it->second->str) {
            it = mappings.erase(it);
        }
        else {
            ++it;
        }
    }
}

bool MapPredicateToPredicate(Formula *p1, Formula *p2, std::map<std::string, Formula*> &mappings)
{
    if ((p1->type == FormulaType::PREDICATE || p1->type == FormulaType::FUNCTION) && p2->type == p1->type) {
        if (p1->str != p2->str || p1->children.size() != p2->children.size()) {
            return false;
        }
        
        for (int i = 0; i < p1->children.size(); i++) {
            Formula *child1 = CloneFormula(p1->children[i]);
            Formula *child2 = CloneFormula(p2->children[i]);
            ApplyMapping(child1, mappings);
            ApplyMapping(child2, mappings);

            std::map<std::string, Formula*> new_mappings;
            if (!MapPredicateToPredicate(child1, child2, new_mappings)) {
                DeleteFormula(child1);
                DeleteFormula(child2);
                return false;
            }

            UpdateMappings(mappings, new_mappings);

            DeleteFormula(child1);
            DeleteFormula(child2);
        }

        return true;
    }

    if (p1->type == FormulaType::VARIABLE) {
        if (p2->type == FormulaType::VARIABLE && p1->str == p2->str) { // x = x. good
            return true;
        }

        if (FormulaContainsVariable(p2, p1->str)) { // x = F(x). baaaad
            return false;
        }

        mappings[p1->str] = CloneFormula(p2);
        return true;
    }  
    
    if (p2->type == FormulaType::VARIABLE) {
        return MapPredicateToPredicate(p2, p1, mappings);
    }

    if (p1->type == FormulaType::CONSTANT && p2->type == FormulaType::CONSTANT) {
        return (p1->str == p2->str);
    }

    /*
    function predicate && predicate function
    constant predicate && predicate constant
    constant function && function constant
    */
    return false;
}

bool Unificate(Formula *p1, Formula *p2)
{
    std::map<std::string, Formula*> mappings;

    bool res = MapPredicateToPredicate(p1, p2, mappings);

    if (!res)
    {
        return res;
    }

    ApplyMapping(p1, mappings);
    ApplyMapping(p2, mappings);

    return res;
}

bool FormulasEqual(Formula *f1, Formula *f2)
{
    if (f1->type != f2->type) return false;

    switch (f1->type) {
    case FormulaType::NOT:
    case FormulaType::AND:
    case FormulaType::OR:
    case FormulaType::IMPLIES:
    case FormulaType::EMPTY:
        break;

    case FormulaType::EXISTS:
    case FormulaType::FORALL:
    case FormulaType::PREDICATE:
    case FormulaType::FUNCTION:
    case FormulaType::VARIABLE:
    case FormulaType::CONSTANT:
        if (f1->str != f2->str) return false;
        break;
    }

    if (f1->children.size() != f2->children.size()) return false;
    for (int i = 0; i < f1->children.size(); i++)
    {
        if (!FormulasEqual(f1->children[i], f2->children[i])) return false;
    }

    return true;
}

Formula* FindResolver(Formula *f1, Formula *f2)
{   
    if (!f1 or !f2) return nullptr;

    if (f1->type == FormulaType::NOT && 
        f1->children[0]->type == FormulaType::PREDICATE &&
        f2->type == FormulaType::PREDICATE &&
        FormulasEqual(f1->children[0], f2))
    {
        return CloneFormula(f2);
    }

    if (f1->type == FormulaType::PREDICATE &&
        f2->type == FormulaType::NOT && 
        f2->children[0]->type == FormulaType::PREDICATE &&
        FormulasEqual(f1, f2->children[0])) 
    {
        return CloneFormula(f1);
    }

    Formula* found = nullptr;
    
    if (f1->type == FormulaType::OR)
    {
        for (Formula *child : f1->children)
        {
            found = FindResolver(child, f2);
            if (found) return found;
        }
    }

    if (f2->type == FormulaType::OR)
    {
        for (Formula *child : f2->children)
        {
            found = FindResolver(f1, child);
            if (found) return found;
        }
    }

    return nullptr;
}

void RemoveResolver(Formula *f, Formula *resolver)
{
    if (!f) return;

    switch (f->type)
    {
       case FormulaType::OR: 
       {
            Formula *left  = f->children[0];
            Formula *right = f->children[1];

            if (FormulasEqual(left, resolver))
            {
                DeleteFormula(left);
                f->type = right->type;
                f->str  = right->str;
                f->children = std::move(right->children);
                delete right;
                
                RemoveResolver(f, resolver);
                return;
            }
            else if (FormulasEqual(right, resolver))
            {
                DeleteFormula(right);
                f->type = left->type;
                f->str  = left->str;
                f->children = std::move(left->children);
                delete left;
                
                RemoveResolver(f, resolver);
                return;
            }
            else
            {
                RemoveResolver(left, resolver);
                RemoveResolver(right, resolver);

                if (left->type == FormulaType::EMPTY && right->type == FormulaType::EMPTY)
                {
                    DeleteFormula(left);
                    DeleteFormula(right);
                    f->children.clear();
                    f->type = FormulaType::EMPTY;
                    f->str = "";
                }
                else if (left->type == FormulaType::EMPTY)
                {
                    DeleteFormula(left);
                    f->type = right->type;
                    f->str = right->str;
                    f->children = std::move(right->children);
                    delete right;
                }
                else if (right->type == FormulaType::EMPTY)
                {
                    DeleteFormula(right);
                    f->type = left->type;
                    f->str = left->str;
                    f->children = std::move(left->children);
                    delete left;
                }
            }
            break;
       }
       case FormulaType::NOT:
       {
            if (FormulasEqual(f->children[0], resolver))
            {
                DeleteFormula(f->children[0]);
                f->children.clear();
                f->str = "";
                f->type = FormulaType::EMPTY;
            }
            else 
            {
                RemoveResolver(f->children[0], resolver);
                
                if (f->children[0]->type == FormulaType::EMPTY)
                {
                    DeleteFormula(f->children[0]);
                    f->children.clear();
                    f->str = "";
                    f->type = FormulaType::EMPTY;
                }
            }
            break;
       }
       case FormulaType::PREDICATE: 
       {
            if (FormulasEqual(f, resolver))
            {
                for (Formula *child: f->children) DeleteFormula(child);
                f->children.clear();
                f->str = "";
                f->type = FormulaType::EMPTY;
            }
            break;
       }
       default: 
            for (Formula *child: f->children) RemoveResolver(child, resolver);
            break;
    }
}

Formula *ResolutionStep(Formula *f1, Formula *f2, Formula *resolver)
{
    RemoveResolver(f1, resolver);
    RemoveResolver(f2, resolver);

    if (f1->type == FormulaType::EMPTY)
    {
        return f2;
    }
    if (f2->type == FormulaType::EMPTY)
    {
        return f1;
    }

    std::vector<Formula*> stack;
    std::vector<Formula*> predicates;

    stack.push_back(f1);
    stack.push_back(f2);

    while (!stack.empty())
    {
        Formula *f = stack.back();
        stack.pop_back();
        
        switch (f->type)
        {
            case FormulaType::OR:
            {
                stack.push_back(f->children[0]);
                stack.push_back(f->children[1]);
                break;
            }
            case FormulaType::PREDICATE:
            case FormulaType::NOT:
            {
                predicates.push_back(f);
                break;
            }
            default: break;
        }

    }

    // more beatiful formula
    std::reverse(predicates.begin(), predicates.end());

    while (predicates.size() > 1)
    {
        Formula *f_left = predicates.back();
        predicates.pop_back();
        Formula *f_right = predicates.back();
        predicates.pop_back();

        Formula *temp = new Formula;

        temp->type = FormulaType::OR;
        temp->str  = GetFormulaTypeStr(FormulaType::OR);
        temp->children.push_back(f_right);
        temp->children.push_back(f_left);

        predicates.push_back(temp);
    }

    return predicates.back();
}

bool ContainsAnd(Formula *f)
{
    if (!f) return false;

    if (f->type == FormulaType::AND) return true;

    for (Formula *child: f->children)
    {
        ContainsAnd(child);
    }

    return false;
}

void SplitConjunctions(Formula *f, std::vector<Formula*> &premises)
{
    std::vector<Formula*> stack;

    stack.push_back(f);

    while (!stack.empty())
    {
        Formula *temp = stack.back();
        stack.pop_back();

        switch (temp->type)
        {   
            case FormulaType::AND:
            {
                stack.push_back(temp->children[0]);
                stack.push_back(temp->children[1]);
                break;
            }
            case FormulaType::OR:
            {
                if (temp->children[0]->type == FormulaType::PREDICATE and
                    temp->children[1]->type == FormulaType::PREDICATE
                )
                {
                    premises.push_back(temp);
                }

                if (temp->children[0]->type == FormulaType::PREDICATE and
                    temp->children[1]->type == FormulaType::OR and
                    !ContainsAnd(temp->children[1])
                )
                {
                    premises.push_back(temp);
                }
                else 
                {
                    stack.push_back(temp->children[1]);
                }
                if (temp->children[1]->type == FormulaType::PREDICATE and
                    temp->children[0]->type == FormulaType::OR and
                    !ContainsAnd(temp->children[0])
                )
                {
                    premises.push_back(temp);
                }
                else
                {
                    stack.push_back(temp->children[0]);
                }
                break;
            }
            default: break;
        }
    }
    
    std::reverse(premises.begin(), premises.end());
}

void Resolution(std::vector<Formula*> &premises)
{

}


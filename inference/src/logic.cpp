#include "logic.hpp"
#include <functional>

void DoForAll(FormulaPtr f, std::function<void(Formula*)> func)
{
    std::vector<FormulaPtr> stack;

    stack.push_back(f);
    while (!stack.empty()) {
        FormulaPtr formula = stack.back();
        func(formula.get());
        stack.pop_back();

        size_t children_count = formula->children_count();
        for (int i = 0; i < children_count; i++) {
            stack.push_back(formula->get_child(i));
        }
    }
}

FormulaPtr PushNegations(FormulaPtr f)
{
    Not *not_f = dynamic_cast<Not*>(f.get());
    
    if (not_f) {
        Formula *child = not_f->get_child(0).get();
        Not *not_child = dynamic_cast<Not*>(child);
        Or *or_child = dynamic_cast<Or*>(child);
        And *and_child = dynamic_cast<And*>(child);
        ForAll *forall_child = dynamic_cast<ForAll*>(child);
        Exists *exists_child = dynamic_cast<Exists*>(child);

        if (not_child) {
            return PushNegations(not_child->f);
        }
        if (or_child) {
            return std::make_shared<And>(
                PushNegations(std::make_shared<Not>(or_child->left_f)),
                PushNegations(std::make_shared<Not>(or_child->right_f))
            );
        }
        if (and_child) {
            return std::make_shared<Or>(
                PushNegations(std::make_shared<Not>(and_child->left_f)),
                PushNegations(std::make_shared<Not>(and_child->right_f))
            );
        }
        if (forall_child) {
            return std::make_shared<Exists>(
                forall_child->var_name,
                PushNegations(std::make_shared<Not>(forall_child->body))
            );
        }
        if (exists_child) {
            return std::make_shared<ForAll>(
                exists_child->var_name,
                PushNegations(std::make_shared<Not>(exists_child->body))
            );
        }
    }

    FormulaPtr new_f = f->copy();
    size_t children_count = new_f->children_count();
    for (int i = 0; i < children_count; i++) {
        new_f->set_child(i, PushNegations(f->get_child(i)));
    }

    return new_f;
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

void RenameBasicTerm(FormulaPtr f, std::string &old_name, std::string &new_name)
{
    DoForAll(f, [&old_name, &new_name](Formula *f) {
        BasicTerm *bt = dynamic_cast<BasicTerm*>(f);
        if (bt && bt->name == old_name) bt->name = new_name;
    });
}

FormulaPtr UnifyNames(FormulaPtr f, std::vector<std::string> &names)
{
    ForAll *forall_child = dynamic_cast<ForAll*>(f.get());
    Exists *exists_child = dynamic_cast<Exists*>(f.get());

    if (forall_child)
    {
        std::string new_name = GenerateUniqueName(forall_child->var_name, names);
        names.push_back(new_name);
        FormulaPtr new_f = std::make_shared<ForAll>(new_name, UnifyNames(forall_child->body, names));
        RenameBasicTerm(new_f->get_child(0), forall_child->var_name, new_name);
        return new_f;
    }
    if (exists_child)
    {
        std::string new_name = GenerateUniqueName(exists_child->var_name, names);
        names.push_back(new_name);
        FormulaPtr new_f = std::make_shared<Exists>(new_name, UnifyNames(exists_child->body, names));
        RenameBasicTerm(new_f->get_child(0), exists_child->var_name, new_name);
        return new_f;
    }

    FormulaPtr new_f = f->copy();
    size_t children_count = new_f->children_count();
    for (int i = 0; i < children_count; i++) {
        new_f->set_child(i, UnifyNames(f->get_child(i), names));
    }
    return new_f;
}

enum class Operation {
    And, Or
};

FormulaPtr ExtractQuantifiers(FormulaPtr a, FormulaPtr b, Operation op)
{
    ForAll *forall_a = dynamic_cast<ForAll*>(a.get());
    Exists *exists_a = dynamic_cast<Exists*>(a.get());
    ForAll *forall_b = dynamic_cast<ForAll*>(b.get());
    Exists *exists_b = dynamic_cast<Exists*>(b.get());

    if (forall_a) return std::make_shared<ForAll>(forall_a->var_name, ExtractQuantifiers(forall_a->body, b, op));
    if (exists_a) return std::make_shared<Exists>(exists_a->var_name, ExtractQuantifiers(exists_a->body, b, op));
    if (forall_b) return std::make_shared<ForAll>(forall_b->var_name, ExtractQuantifiers(a, forall_b->body, op));
    if (exists_b) return std::make_shared<Exists>(exists_b->var_name, ExtractQuantifiers(a, exists_b->body, op));

    if (op == Operation::And) return std::make_shared<And>(a, b);
    if (op == Operation::Or) return std::make_shared<Or>(a, b);
    throw std::runtime_error("Unknown operation");
}

FormulaPtr MoveQuantifiers(FormulaPtr f)
{
    And *and_f = dynamic_cast<And*>(f.get());
    Or *or_f = dynamic_cast<Or*>(f.get());

    if (and_f)
    {
        FormulaPtr a = MoveQuantifiers(and_f->left_f);
        FormulaPtr b = MoveQuantifiers(and_f->right_f);
        return ExtractQuantifiers(a, b, Operation::And);   
    }
    if (or_f)
    {
        FormulaPtr a = MoveQuantifiers(or_f->left_f);
        FormulaPtr b = MoveQuantifiers(or_f->right_f);
        return ExtractQuantifiers(a, b, Operation::Or);
    }

    FormulaPtr new_f = f->copy();
    size_t children_count = new_f->children_count();
    for (int i = 0; i < children_count; i++) {
        new_f->set_child(i, MoveQuantifiers(f->get_child(i)));
    }
    return new_f;
}

FormulaPtr MakePrenexNormalForm(FormulaPtr f)
{
    std::vector<std::string> names;

    FormulaPtr result = UnifyNames(f, names);
    result = PushNegations(result);
    result = MoveQuantifiers(result);

    return result;
}
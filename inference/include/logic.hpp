#ifndef LOGIC_HPP
#define LOGIC_HPP

#include <stdexcept>
#include <string>
#include <vector>
#include <memory>

struct Term;
struct Formula;

using FormulaPtr = std::shared_ptr<Formula>;

struct Formula 
{
    virtual ~Formula() = default;
    virtual std::string to_string() const = 0;
    virtual size_t children_count() const = 0;
    virtual FormulaPtr get_child(size_t i) const = 0;
    virtual void set_child(size_t i, FormulaPtr f) = 0;
    virtual FormulaPtr copy() = 0;
};

struct BasicTerm : public Formula
{
    enum Type {
        VARIABLE,
        CONSTANT
    };

    std::string name;
    Type type;

    inline BasicTerm(std::string name, Type type) : name(name), type(type) {}

    virtual std::string to_string() const override { return name; }
    virtual size_t children_count() const override { return 0; }
    virtual FormulaPtr get_child(size_t i) const override { throw std::out_of_range("BasicTerm has no children"); }
    virtual void set_child(size_t i, FormulaPtr f) override { throw std::out_of_range("BasicTerm has no children"); }
    virtual FormulaPtr copy() override { return std::make_shared<BasicTerm>(*this); }
};

struct Unary: public Formula
{
    FormulaPtr f;

    inline Unary(FormulaPtr f): f(f) {}

    inline size_t children_count() const override { return 1; }
    inline FormulaPtr get_child(size_t i) const override
    {
        if (i != 0) throw std::out_of_range("Unary has only one child");
        return f;
    }

    inline void set_child(size_t i, FormulaPtr f) override
    {
        if (i != 0) throw std::out_of_range("Unary has only one child");
        this->f = f;
    }
};

struct Binary: public Formula
{
    FormulaPtr left_f, right_f;

    inline Binary(FormulaPtr left_f, FormulaPtr right_f): left_f(left_f), right_f(right_f) {}

    inline size_t children_count() const override { return 2; }
    inline FormulaPtr get_child(size_t i) const override
    {
        if (i >= 2) throw std::out_of_range("Not has only one child");
        return (i == 0) ? left_f : right_f;
    }

    inline void set_child(size_t i, FormulaPtr f) override
    {
        if (i >= 2) throw std::out_of_range("Not has only one child");
        if (i == 0) left_f = f;
        else        right_f = f;
    }
};

struct Function : public Formula
{
    std::string predicate_name;
    std::vector<FormulaPtr> args;

    inline Function(std::string name, std::vector<FormulaPtr> args) : predicate_name(name), args(args) {}
    
    inline std::string to_string() const override 
    {
        if (args.empty()) return predicate_name;

        std::string s = "(" + predicate_name;
        for (const auto& arg : args) s += " " + arg->to_string();
        return s + ")";
    }

    inline size_t children_count() const override { return args.size(); }
    inline FormulaPtr get_child(size_t i) const override
    {
        if (i < args.size()) return args[i];
        throw std::out_of_range("Wrong child index for funciton");
    }

    inline void set_child(size_t i, FormulaPtr f) override
    {
        if (i >= args.size()) throw std::out_of_range("Wrong child index for funciton");
        args[i] = f;
    }

    FormulaPtr copy() override { return std::make_shared<Function>(*this); }
};

struct Not: public Unary
{
    inline Not(FormulaPtr f): Unary{f} {}
    inline std::string to_string() const override { return "(not " + f->to_string() + ")"; }
    FormulaPtr copy() override { return std::make_shared<Not>(f->copy()); }
};

struct Or: public Binary
{
    inline Or(FormulaPtr lhs, FormulaPtr rhs): Binary(lhs, rhs) {}
    inline std::string to_string() const override { return "(or " + left_f->to_string() + right_f->to_string() + ")"; }
    FormulaPtr copy() override { return std::make_shared<Or>(left_f->copy(), right_f->copy()); }
};

struct And: public Binary
{
    inline And(FormulaPtr lhs, FormulaPtr rhs): Binary(lhs, rhs) {}
    inline std::string to_string() const override { return "(and " + left_f->to_string() + right_f->to_string() + ")"; }
    FormulaPtr copy() override { return std::make_shared<And>(left_f->copy(), right_f->copy()); }
};

struct Implies: public Binary
{
    inline Implies(FormulaPtr lhs, FormulaPtr rhs): Binary(lhs, rhs) {}
    inline std::string to_string() const override { return "(implies " + left_f->to_string() + right_f->to_string() + ")"; }
    FormulaPtr copy() override { return std::make_shared<Implies>(left_f->copy(), right_f->copy()); }
};

struct Quantifier : public Formula
{
    std::string var_name;
    FormulaPtr body;

    inline Quantifier(std::string v, FormulaPtr b) : var_name(v), body(b) {}

    inline size_t children_count() const override { return 1; }
    inline FormulaPtr get_child(size_t i) const override
    {
        if (i != 0) throw std::out_of_range("Quantifier has only one child");
        return body;
    }

    inline void set_child(size_t i, FormulaPtr f) override
    {
        if (i != 0) throw std::out_of_range("Quantifier has only one child");
        this->body = f;
    }
};

struct ForAll : public Quantifier 
{
    inline ForAll(std::string v, FormulaPtr b) : Quantifier(std::move(v), std::move(b)) {}
    inline std::string to_string() const override { return "(forall " + var_name + " " + body->to_string() + ")"; }
    FormulaPtr copy() override { return std::make_shared<ForAll>(var_name, body->copy()); }
};

struct Exists : public Quantifier
{
    inline Exists(std::string v, FormulaPtr b) : Quantifier(std::move(v), std::move(b)) {}
    inline std::string to_string() const override { return "(exists " + var_name + " " + body->to_string() + ")"; }
    FormulaPtr copy() override { return std::make_shared<Exists>(var_name, body->copy()); }
};

FormulaPtr MakePrenexNormalForm(FormulaPtr f);

#endif // LOGIC_HPP

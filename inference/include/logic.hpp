#ifndef LOGIC_HPP
#define LOGIC_HPP

#include <string>
#include <vector>
#include <memory>

struct Term;
struct Formula;

using TermPtr = std::shared_ptr<Term>;
using FormulaPtr = std::shared_ptr<Formula>;

struct Term
{
    virtual ~Term() = default;

    virtual std::string to_string() const = 0;
};

struct Variable: public Term
{
    std::string var_name;
    
    inline Variable(std::string name): var_name(name) {}

    inline std::string to_string() const override { return var_name; }
};

struct Constant: public Term
{
    std::string const_name;

    inline Constant(std::string name): const_name(name) {}

    inline std::string to_string() const override { return const_name; }
};

// mb useless ?
struct Function: public Term
{
    std::vector<TermPtr> args;
    std::string       func_name;

    inline Function(std::string name, std::vector<TermPtr> args): func_name(name), args(args) {}

    inline std::string to_string() const override 
    {
        if (args.empty()) return func_name;

        std::string s = "(" + func_name;

        for (const auto& arg : args) s += " " + arg->to_string();

        return s + ")";
    }
};

struct Formula 
{
    virtual ~Formula() = default;
    virtual std::string to_string() const = 0;
};

struct Atomic: public Formula
{
    std::string predicate_name;
    std::vector<TermPtr> args;

    inline Atomic(std::string name, std::vector<TermPtr> args) : predicate_name(name), args(args) {}
    
    inline std::string to_string() const override 
    {
        if (args.empty()) return predicate_name;

        std::string s = "(" + predicate_name;

        for (const auto& arg : args) s += " " + arg->to_string();

        return s + ")";
    }
};

struct Not: public Formula
{
    FormulaPtr f;

    inline Not(FormulaPtr f): f(f) {}

    inline std::string to_string() const override { return "(not " + f->to_string() + ")"; }
};

struct Or: public Formula
{
    FormulaPtr left_f, right_f;

    inline Or(FormulaPtr lhs, FormulaPtr rhs): left_f(lhs), right_f(rhs) {}

    inline std::string to_string() const override { return "(or " + left_f->to_string() + right_f->to_string() + ")"; }
};

struct And: public Formula
{
    FormulaPtr left_f, right_f;

    inline And(FormulaPtr lhs, FormulaPtr rhs): left_f(lhs), right_f(rhs) {}

    inline std::string to_string() const override { return "(and " + left_f->to_string() + right_f->to_string() + ")"; }
};

struct Implies: public Formula
{
    FormulaPtr left_f, right_f;

    inline Implies(FormulaPtr lhs, FormulaPtr rhs): left_f(lhs), right_f(rhs) {}

    inline std::string to_string() const override { return "(implies " + left_f->to_string() + right_f->to_string() + ")"; }
};

struct ForAll : public Formula 
{
    std::string var_name;
    FormulaPtr body;

    inline ForAll(std::string v, FormulaPtr b) : var_name(v), body(b) {}

    inline std::string to_string() const override { return "(forall " + var_name + " " + body->to_string() + ")"; }
};

struct Exists : public Formula 
{
    std::string var_name;
    FormulaPtr body;

    inline Exists(std::string v, FormulaPtr b) : var_name(std::move(v)), body(std::move(b)) {}
    inline std::string to_string() const override { return "(exists " + var_name + " " + body->to_string() + ")"; }
};

#endif // LOGIC_HPP

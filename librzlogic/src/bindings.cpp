#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include "logic.hpp"
#include "parser.hpp"

using namespace rzlogic;
namespace py = pybind11;

using StepWrapper = std::tuple<std::string, std::string, std::string>;

std::tuple<bool, std::vector<StepWrapper>> MakeResolutionWrapper(const std::vector<std::string> &premises) 
{
    std::vector<ResolutionStepInfo> history;
    std::vector<Formula*> formuls;
    std::vector<StepWrapper> history_out;

    for (const auto &str: premises)
    {
        Formula *f = Parser(str).Parse();

        NormalizeFormula(f);
        MakePrenexNormalForm(f);
        MakeSkolemNormalForm(f);
        MakeConjunctiveNormalForm(f);
        SplitConjunctions(f, formuls);
    }

    bool result = MakeResolution(formuls, history);
    
    for (const auto& step : history) 
    {
        StepWrapper info = {FormulaAsString(step.premise1), 
                            FormulaAsString(step.premise2), 
                            FormulaAsString(step.resolvent)};
        history_out.push_back(info);
    }

    for (Formula *f: formuls) DeleteFormula(f);
    
    return std::make_tuple(result, history_out);
}

PYBIND11_MODULE(rzlogic, rz)
{
    rz.doc() = R"pbdoc(
        ===== RZ Logic Module =====
        
        Python binding for C++ rzlogic library implementing resolution method
        for logical formulas.
        
        This module provides automated theorem proving using the resolution
        method for first-order logic.
        
        Key features:
        - Parsing of logical formulas
        - Normalization (Prenex, Skolem, CNF)
        - Resolution proof procedure
        - Step-by-step proof history
    )pbdoc";
    
    rz.def("make_resolution", &MakeResolutionWrapper, R"pbdoc(
        Perform resolution method on logical premises.
        
        Args:
            premises: List of strings representing logical formulas in text form.
                     Each string should be a valid logical formula that can be
                     parsed by the module.
                     
                     Example: ["(forall x (implies (H x) (M x)))",
                                "(H a)",
                                "(not (M a))"]
        
        Returns:
            tuple: (success, proof_history)
            - success (bool): True if contradiction was found (proof successful),
                            False otherwise
            - proof_history (list): List of resolution steps as tuples
                            (premise1, premise2, resolvent)
        
        Raises:
            RuntimeError: If formula parsing fails
        
        Example:
            >>> import rzlogic
            >>> premises = ["(or (P x) (Q x))", "(or (not (P x)) (Q x))"]
            >>> success, history = rzlogic.make_resolution(premises)
            >>> print(f"Proof successful: {success}")
            >>> for step in history:
            ...     print(f"Resolved {step[0]} and {step[1]} to get {step[2]}")
    )pbdoc",
    py::arg("premises"));
}

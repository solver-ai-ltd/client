#include "SolverAiComputeInput.h"

SolverAiComputeInput::SolverAiComputeInput(int problem_id) : problem_id(problem_id) {}

void SolverAiComputeInput::addInput(const std::string &name, float Min, float Max, bool isConstant, bool isInteger)
{
    inputs[name] = {{"Min", Min}, {"Max", Max}, {"Constant", isConstant}, {"Integer", isInteger}};
}

void SolverAiComputeInput::addConstraint(const std::string &name, CONSTRAINT operation, float Value1, float Value2)
{
    std::string op;
    switch (operation)
    {
    case SMALLER_THAN:
        op = "smaller than";
        break;
    case GREATER_THAN:
        op = "greater than";
        break;
    case EQUAL_TO:
        op = "equal to";
        break;
    case INSIDE_RANGE:
        op = "inside range";
        break;
    case OUTSIDE_RANGE:
        op = "outside range";
        break;
    }
    constraints[name] = {{"Operation", op}, {"Value1", Value1}, {"Value2", Value2}};
}

void SolverAiComputeInput::addObjective(const std::string &name, OBJECTIVE operation)
{
    std::string op = operation == MINIMIZE ? "minimize" : "maximize";
    objectives[name] = {{"Operation", op}};
}

nlohmann::json SolverAiComputeInput::getJson() const
{
    return {
        {"id", problem_id},
        {"inputs", inputs},
        {"constraints", constraints},
        {"objectives", objectives}};
}

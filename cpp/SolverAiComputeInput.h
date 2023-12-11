#ifndef COMPUTEINPUT_H
#define COMPUTEINPUT_H

#include <string>
#include <map>

#include "json.hpp"

class SolverAiComputeInput
{
public:
    enum CONSTRAINT
    {
        SMALLER_THAN,
        GREATER_THAN,
        EQUAL_TO,
        INSIDE_RANGE,
        OUTSIDE_RANGE
    };
    enum OBJECTIVE
    {
        MINIMIZE,
        MAXIMIZE
    };

    SolverAiComputeInput(int problem_id);

    void addInput(const std::string &name, float Min, float Max, bool isConstant, bool isInteger);
    void addConstraint(const std::string &name, CONSTRAINT operation, float Value1, float Value2 = 0);
    void addObjective(const std::string &name, OBJECTIVE operation);
    nlohmann::json getJson() const;

private:
    int problem_id;
    std::map<std::string, nlohmann::json> inputs;
    std::map<std::string, nlohmann::json> constraints;
    std::map<std::string, nlohmann::json> objectives;
};

#endif // COMPUTEINPUT_H

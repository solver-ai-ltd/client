#ifndef COMPUTERESULTS_H
#define COMPUTERESULTS_H

#include <any>
#include <variant>
#include <vector>
#include <string>
#include <map>

#include "json.hpp"

class SolverAiComputeResults {
public:
    SolverAiComputeResults(nlohmann::json &j);
    int getNumberOfResults() const;
    const std::vector<std::string> &getObjectiveVariableNames() const;
    const std::vector<std::string> &getConstraintVariableNames() const;
    const std::vector<std::string> &getInputVariableNames() const;
    const std::vector<std::string> &getOutputVariableNames() const;
    const std::vector<std::vector<std::variant<float, std::string>>> &getX() const;
    const std::vector<std::vector<std::variant<float, std::string>>> &getY() const;
private:
    void parseString(
        std::string key,
        nlohmann::json &obj,
        std::vector<std::string> &vec
    );
    void parseVector(
        std::string key,
        nlohmann::json &obj,
        std::vector<std::variant<float, std::string>> &vec
    );
    int numberOfResults = 0;
    std::vector<std::string> objectiveVariableNames;
    std::vector<std::string> constraintVariableNames;
    std::vector<std::string> inputVariableNames;
    std::vector<std::string> outputVariableNames;
    std::vector<std::vector<std::variant<float, std::string>>> X;
    std::vector<std::vector<std::variant<float, std::string>>> Y;
};

#endif  // COMPUTERESULTS_H

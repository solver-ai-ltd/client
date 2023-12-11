#include "SolverAiComputeResults.h"

SolverAiComputeResults::SolverAiComputeResults(nlohmann::json &j)
{
    numberOfResults = j["Number Of Results"];
    parseString("Objective Variable Names", j, objectiveVariableNames);
    parseString("Constraint Variable Names ", j, constraintVariableNames);
    parseString("Input Variable Names", j, inputVariableNames);
    parseString("Output Variable Names", j, outputVariableNames);

    // Resize X and Y to hold the correct number of results
    X.resize(numberOfResults);
    Y.resize(numberOfResults);

    // Parse X and Y values based on numberOfResults
    for (int i = 0; i < numberOfResults; ++i)
    {
        std::string xKey = "X" + std::to_string(i);
        std::string yKey = "Y" + std::to_string(i);
        parseVector(xKey, j, X[i]);
        parseVector(yKey, j, Y[i]);
    }
}

void SolverAiComputeResults::parseString(
    std::string key,
    nlohmann::json &obj,
    std::vector<std::string> &vec)
{
    if (obj.contains(key))
    {
        std::string str = obj[key];
        str = str.substr(1, str.size() - 2); // Remove '[' and ']'
        str.erase(std::remove(str.begin(), str.end(), ' '), str.end());
        std::stringstream ss(str);
        std::string token;

        while (std::getline(ss, token, ','))
        {
            token = token.substr(1, token.size() - 2); // Remove '\'' characters
            vec.push_back(token);
        }
    }
}

void SolverAiComputeResults::parseVector(
    std::string key, nlohmann::json &obj,
    std::vector<std::variant<float,
                             std::string>> &vec)
{
    if (obj.contains(key))
    {
        std::string str = obj[key];
        str = str.substr(1, str.size() - 2); // Remove '[' and ']'
        str.erase(std::remove(str.begin(), str.end(), ' '), str.end());
        std::stringstream ss(str);
        std::string token;

        while (std::getline(ss, token, ','))
        {
            try
            {
                vec.push_back(std::stof(token));
            }
            catch (...)
            {
                token.erase(std::remove(token.begin(), token.end(), '\''), token.end());
                vec.push_back(token);
            }
        }
    }
}

int SolverAiComputeResults::getNumberOfResults() const
{
    return numberOfResults;
}

const std::vector<std::string> &SolverAiComputeResults::getObjectiveVariableNames() const
{
    return objectiveVariableNames;
}

const std::vector<std::string> &SolverAiComputeResults::getConstraintVariableNames() const
{
    return constraintVariableNames;
}

const std::vector<std::string> &SolverAiComputeResults::getInputVariableNames() const
{
    return inputVariableNames;
}

const std::vector<std::string> &SolverAiComputeResults::getOutputVariableNames() const
{
    return outputVariableNames;
}

const std::vector<std::vector<std::variant<float, std::string>>> &SolverAiComputeResults::getX() const
{
    return X;
}

const std::vector<std::vector<std::variant<float, std::string>>> &SolverAiComputeResults::getY() const
{
    return Y;
}

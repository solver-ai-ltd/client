#include <variant>
#include <string>

#include "SolverAiResultsWriter.h"

SolverAiResultsWriter::SolverAiResultsWriter(const SolverAiComputeResults& results) : results(results) {}

void SolverAiResultsWriter::write(const std::string& path) const {
    std::ofstream file(path);
    if (file.is_open()) {
        file << "Number Of Results," << results.getNumberOfResults() << "\n";
        file << "Objective Variable Names,";
        for (const auto& name : results.getObjectiveVariableNames()) {
            file << name << ",";
        }
        file << "\nConstraint Variable Names,";
        for (const auto& name : results.getConstraintVariableNames()) {
            file << name << ",";
        }
        file << "\n\nInputs\n#,";
        for (const auto& name : results.getInputVariableNames()) {
            file << name << ",";
        }
        file << "\n";
        int i = 0;
        for (const auto& x : results.getX()) {
            file << "X" << i++ << ",";
            for (const auto& val : x) {
                file << getVariantAsString(val) << ",";
            }
            file << "\n";
        }
        file << "\nOutputs\n#,";
        for (const auto& name : results.getOutputVariableNames()) {
            file << name << ",";
        }
        file << "\n";
        i = 0;
        for (const auto& y : results.getY()) {
            file << "Y" << i++ << ",";
            for (const auto& val : y) {
                file << getVariantAsString(val) << ",";
            }
            file << "\n";
        }
        file.close();
    }
}

std::string SolverAiResultsWriter::getVariantAsString(
    const std::variant<float, std::string>& val
) const
{
    std::string string;
    if (std::holds_alternative<std::string>(val))
    {
        string = std::get<std::string>(val);
    }
    else if (std::holds_alternative<float>(val))
    {
        string = std::to_string(std::get<float>(val));
    }
    return string;
}

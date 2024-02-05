#ifndef SOLVERAIRESULTSWRITER_H
#define SOLVERAIRESULTSWRITER_H

#include <fstream>
#include <variant>
#include <string>
#include "SolverAiComputeResults.h"

class SolverAiResultsWriter {
public:
    SolverAiResultsWriter(const SolverAiComputeResults& results);
    void write(const std::string& path) const;
private:
    std::string getVariantAsString(const std::variant<float, std::string>& val) const;

    const SolverAiComputeResults& results;
};

#endif  // SOLVERAIRESULTSWRITER_H

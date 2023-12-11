#ifndef SOLVERAICLIENTCOMPUTE_H
#define SOLVERAICLIENTCOMPUTE_H

#include <string>
// #include <map>

#include "json.hpp"
#include "SolverAiComputeInput.h"
#include "SolverAiComputeResults.h"

class SolverAiClientCompute
{
public:
    SolverAiClientCompute(std::string computerUrl, std::string token, int problemId);

    void getProblemSetup(
        std::vector<std::string> &inputs,
        std::vector<std::string> &outputs
    );

    SolverAiComputeResults runSolver(const SolverAiComputeInput &input);

    static size_t writeCallback(void *contents, size_t size, size_t nmemb, std::string *userp);

private:
    bool isStatusCodeOk(int statusCode);

    std::string __base_url_Computer;
    int __problemId;
    std::string __headers;
};

#endif // SOLVERAICLIENTCOMPUTE_H

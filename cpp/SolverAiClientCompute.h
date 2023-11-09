#ifndef SOLVERAICLIENTCOMPUTE_H
#define SOLVERAICLIENTCOMPUTE_H

#include <string>
// #include <map>

#include "json.hpp"
// #include "httplib.h"

class SolverAiClientCompute {
public:
    SolverAiClientCompute(std::string computerUrl, std::string token, int problemId);

    nlohmann::json getProblemSetup();

    nlohmann::json runSolver(nlohmann::json data);

    static size_t writeCallback(void* contents, size_t size, size_t nmemb, std::string* userp);

private:
    bool isStatusCodeOk(int statusCode);

    std::string __base_url_Computer;
    int __problemId;
    // httplib::Headers __headers;
    std::string __headers;
};

#endif // SOLVERAICLIENTCOMPUTE_H

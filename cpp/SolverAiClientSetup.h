#ifndef SOLVERAICLIENTSETUP_H
#define SOLVERAICLIENTSETUP_H

#include <string>
#include <vector>

#include "json.hpp"

class SolverAiClientSetup {
public:
    SolverAiClientSetup(std::string datamanagerUrl, std::string token);

    void deleteAll(
        std::vector<int> equationIds = {},
        std::vector<int> codeIds = {},
        std::vector<int> hardIds = {},
        std::vector<int> softIds = {},
        int problemId = -1
    );

    int postEquation(
        std::string name,
        std::string equationString,
        std::string variablesString,
        std::string vectorizationIndices = ""
    );

    int postCode(
        std::string name,
        std::string filePath,
        std::string variablesStringIn,
        std::string variablesStringOut,
        std::string vectorizationIndices = ""
    );

    int postHardData(
        std::string name,
        std::string filePath,
        std::string vectorizationIndices = ""
    );

    int postSoftData(
        std::string name,
        std::string filePath,
        std::string variablesStringIn,
        std::string variablesStringOut,
        std::string vectorizationIndices = ""
    );

    int postProblem(
        std::string problemName,
        std::vector<int> equationIds = {},
        std::vector<int> codeIds = {},
        std::vector<int> hardIds = {},
        std::vector<int> softIds = {}
    );

    void patchHardData(
        int id,
        std::string filePath
    );

    void patchSoftData(
        int id,
        std::string filePath
    );

    static size_t writeCallback(void* contents, size_t size, size_t nmemb, std::string* userp);

private:
    bool isStatusCodeOk(int statusCode);

    int post(
        std::string urlSuffix,
        nlohmann::json jsonData,
        std::string filePath = "",
        std::string fileKey = ""
    );

    void patchFile(
        std::string urlSuffix,
        int id,
        std::string filePath,
        std::string fileKey
    );

    void deleteIds(
        std::string urlSuffix,
        std::vector<int> ids,
        std::string& errors
    );

    std::string __base_url_DM;
    std::string __headers;
    std::string __equationSuffix;
    std::string __codeSuffix;
    std::string __hardDataSuffix;
    std::string __softDataSuffix;
    std::string __problemSuffix;
};

#endif // SOLVERAICLIENTSETUP_H

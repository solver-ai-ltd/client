#ifndef SOLVERAICLIENTSETUP_H
#define SOLVERAICLIENTSETUP_H

#include <string>
#include <vector>

#include "json.hpp"

class SolverAiClientSetup
{
public:
    SolverAiClientSetup(std::string datamanagerUrl, std::string token);

    int postEquation(
        std::string name,
        std::string equationString,
        std::string variablesString,
        std::string vectorizationIndices = "");

    int patchEquation(
        int id,
        std::string name = "",
        std::string equationString = "",
        std::string variablesString = "",
        std::string vectorizationIndices = "");

    int postCode(
        std::string name,
        std::string filePath,
        std::string variablesStringIn,
        std::string variablesStringOut,
        std::string vectorizationIndices = "");

    int patchCode(
        int id,
        std::string name = "",
        std::string filePath = "",
        std::string variablesStringIn = "",
        std::string variablesStringOut = "",
        std::string vectorizationIndices = "");

    int postHardData(
        std::string name,
        std::string filePath,
        std::string vectorizationIndices = "");

    int patchHardData(
        int id,
        std::string name = "",
        std::string filePath = "",
        std::string vectorizationIndices = "");

    int postSoftData(
        std::string name,
        std::string filePath,
        std::string variablesStringIn,
        std::string variablesStringOut,
        std::string vectorizationIndices = "");

    int patchSoftData(
        int id,
        std::string name = "",
        std::string filePath = "",
        std::string variablesStringIn = "",
        std::string variablesStringOut = "",
        std::string vectorizationIndices = "");

    int postProblem(
        std::string problemName,
        std::vector<int> equationIds = {},
        std::vector<int> codeIds = {},
        std::vector<int> hardIds = {},
        std::vector<int> softIds = {});

    int patchProblem(
        int id,
        std::string problemName = "",
        std::vector<int> equationIds = {},
        std::vector<int> codeIds = {},
        std::vector<int> hardIds = {},
        std::vector<int> softIds = {});

    std::string deleteEquation(int id);

    std::string deleteCode(int id);

    std::string deleteHardData(int id);

    std::string deleteSoftData(int id);

    std::string deleteProblem(int id);

    std::string deleteEquations(std::string nameRegex = ".*");

    std::string deleteCodes(std::string nameRegex = ".*");

    std::string deleteHardDatas(std::string nameRegex = ".*");

    std::string deleteSoftDatas(std::string nameRegex = ".*");

    std::string deleteProblems(std::string nameRegex = ".*");

    void deleteAll(
        std::vector<int> equationIds = {},
        std::vector<int> codeIds = {},
        std::vector<int> hardIds = {},
        std::vector<int> softIds = {},
        int problemId = -1);

    static size_t writeCallback(
        void *contents,
        size_t size,
        size_t nmemb,
        std::string *userp
    );

private:
    bool isStatusCodeOk(int statusCode);

    int postPatch(
        std::string urlSuffix,
        nlohmann::json jsonData,
        std::string filePath = "",
        std::string fileKey = "",
        int id = -1);

    std::vector<int> getIds(
        std::string urlSuffix,
        std::string nameRegex);

    void deleteIds(
        std::string urlSuffix,
        std::vector<int> ids,
        std::string &errors);

    void deleteModules(
        std::string urlSuffix,
        std::string nameRegex,
        std::string &errors);

    std::string __base_url_DM;
    std::string __headers;
    std::string __equationSuffix;
    std::string __codeSuffix;
    std::string __hardDataSuffix;
    std::string __softDataSuffix;
    std::string __problemSuffix;
};

#endif // SOLVERAICLIENTSETUP_H

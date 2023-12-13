#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>

#include <curl/curl.h>

#include "json.hpp"
// #include "cppcodec/base64_rfc4648.hpp"

#include "SolverAiClientSetup.h"

SolverAiClientSetup::SolverAiClientSetup(std::string datamanagerUrl, std::string token)
{
    this->__base_url_DM = datamanagerUrl + "/api/data/";
    this->__headers = "Authorization: Token " + token;
    this->__equationSuffix = "equations";
    this->__codeSuffix = "code";
    this->__hardDataSuffix = "hard-datas";
    this->__softDataSuffix = "soft-datas";
    this->__problemSuffix = "problems";
}

bool SolverAiClientSetup::isStatusCodeOk(int statusCode)
{
    return 200 <= statusCode && statusCode < 300;
}

int SolverAiClientSetup::postPatch(
    std::string urlSuffix,
    nlohmann::json data,
    std::string filePath,
    std::string fileKey,
    int id)
{
    std::string errors;

    bool isPost = (id == -1) ? true : false;

    CURL *curl;
    CURLcode res;
    std::string url;
    if (isPost)
    {
        url = this->__base_url_DM + urlSuffix + "/";
    }
    else
    {
        url = this->__base_url_DM + urlSuffix + "/" + std::to_string(id) + "/";
    }
    std::string response_string;
    long response_code;

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    struct curl_slist *header_list = NULL;
    struct curl_httppost *formpost = NULL;
    try
    {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        if (isPost)
        {
            curl_easy_setopt(curl, CURLOPT_POST, 1L);
        }
        else
        {
            curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PATCH");
        }

        header_list = curl_slist_append(header_list, this->__headers.c_str());

        if (data.contains("vectorizationIndices") &&
            data["vectorizationIndices"].get<std::string>().empty())
        {
            data.erase("vectorizationIndices");
        }

        if (filePath.empty())
        {
            std::string json_str = data.dump();
            curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, json_str.size());
            curl_easy_setopt(curl, CURLOPT_COPYPOSTFIELDS, json_str.c_str());

            header_list = curl_slist_append(header_list, "Content-Type: application/json; charset: utf-8");
        }
        else
        {
            struct curl_httppost *lastptr = NULL;
            for (const auto &element : data.items())
            {
                std::string key = element.key();
                std::string value = element.value().get<std::string>();
                curl_formadd(&formpost,
                             &lastptr,
                             CURLFORM_COPYNAME, key.c_str(),
                             CURLFORM_COPYCONTENTS, value.c_str(),
                             CURLFORM_END);
            }
            curl_formadd(&formpost,
                         &lastptr,
                         CURLFORM_COPYNAME, fileKey.c_str(),
                         CURLFORM_FILE, filePath.c_str(),
                         CURLFORM_END);

            curl_easy_setopt(curl, CURLOPT_HTTPPOST, formpost);
        }
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, header_list);

        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_string);

        // Comment out when done
        // curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

        res = curl_easy_perform(curl);

        if (res != CURLE_OK)
        {
            if (isPost)
            {
                throw std::runtime_error("Post failed");
            }
            else
            {
                throw std::runtime_error("Patch failed");
            }
        }

        data = nlohmann::json::parse(response_string);
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);

        if (!isStatusCodeOk(response_code))
            throw std::runtime_error(response_string);
        else
            id = data["id"].get<int>();
    }
    catch (const std::exception &e)
    {
        errors = "Exception: " + std::string(e.what());
    }

    if (filePath.empty())
        curl_formfree(formpost);
    curl_slist_free_all(header_list);
    curl_easy_cleanup(curl);
    curl_global_cleanup();

    if (!errors.empty())
    {
        throw std::runtime_error(errors.c_str());
    }

    return id;
}

size_t SolverAiClientSetup::writeCallback(void *contents, size_t size, size_t nmemb, std::string *userp)
{
    ((std::string *)userp)->append((char *)contents, size * nmemb);
    return size * nmemb;
}

void SolverAiClientSetup::deleteAll(
    std::vector<int> equationIds,
    std::vector<int> codeIds,
    std::vector<int> hardIds,
    std::vector<int> softIds,
    int problemId)
{
    std::string errors;
    try
    {
        if (problemId != -1)
        {
            // Problem must be deleted first or will not allow deletion of models
            deleteIds(this->__problemSuffix, {problemId}, errors);
        }
        deleteIds(this->__equationSuffix, equationIds, errors);
        deleteIds(this->__codeSuffix, codeIds, errors);
        deleteIds(this->__hardDataSuffix, hardIds, errors);
        deleteIds(this->__softDataSuffix, softIds, errors);
    }
    catch (const std::exception &e)
    {
        std::cerr << "Failed Deleting: " << e.what() << std::endl;
    }
    if (!errors.empty())
    {
        throw std::runtime_error(errors.c_str());
    }
}

std::string SolverAiClientSetup::deleteEquation(int id)
{
    std::string error;
    deleteIds(this->__equationSuffix, {id}, error);
    return error;
}

std::string SolverAiClientSetup::deleteCode(int id)
{
    std::string error;
    deleteIds(this->__codeSuffix, {id}, error);
    return error;
}

std::string SolverAiClientSetup::deleteHardData(int id)
{
    std::string error;
    deleteIds(this->__hardDataSuffix, {id}, error);
    return error;
}

std::string SolverAiClientSetup::deleteSoftData(int id)
{
    std::string error;
    deleteIds(this->__softDataSuffix, {id}, error);
    return error;
}

std::string SolverAiClientSetup::deleteProblem(int id)
{
    std::string error;
    deleteIds(this->__problemSuffix, {id}, error);
    return error;
}

void SolverAiClientSetup::deleteIds(std::string urlSuffix, std::vector<int> ids, std::string &errors)
{
    CURL *curl;
    CURLcode res;
    std::string response_string;
    long response_code;

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    struct curl_slist *header_list = NULL;
    header_list = curl_slist_append(header_list, this->__headers.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, header_list);
    for (const int &id : ids)
    {
        try
        {
            std::string url = this->__base_url_DM + urlSuffix + "/" + std::to_string(id) + "/";
            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELETE");
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_string);

            // Comment out when done
            // curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

            res = curl_easy_perform(curl);

            if (res != CURLE_OK)
            {
                throw std::runtime_error("Post failed");
            }

            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);

            if (!isStatusCodeOk(response_code))
                throw std::runtime_error(response_string);
        }
        catch (const std::exception &e)
        {
            errors += "Exception: " + std::string(e.what()) + "\n";
        }
    }
    curl_slist_free_all(header_list);
    curl_easy_cleanup(curl);
    curl_global_cleanup();
}

int SolverAiClientSetup::postEquation(
    std::string name,
    std::string equationString,
    std::string variablesString,
    std::string vectorizationIndices)
{
    nlohmann::json data = {
        {"name", name},
        {"equationString", equationString},
        {"variablesString", variablesString},
        {"vectorizationIndices", vectorizationIndices}};
    return postPatch(this->__equationSuffix, data);
}

int SolverAiClientSetup::patchEquation(
    int id,
    std::string name,
    std::string equationString,
    std::string variablesString,
    std::string vectorizationIndices)
{
    nlohmann::json data;
    if (!name.empty())
        data["name"] = name;
    if (!equationString.empty())
        data["equationString"] = equationString;
    if (!variablesString.empty())
        data["variablesString"] = variablesString;
    if (!vectorizationIndices.empty())
        data["vectorizationIndices"] = vectorizationIndices;
    return postPatch(this->__equationSuffix, data, "", "", id);
}

int SolverAiClientSetup::postCode(
    std::string name,
    std::string filePath,
    std::string variablesStringIn,
    std::string variablesStringOut,
    std::string vectorizationIndices)
{
    nlohmann::json data = {
        {"name", name},
        {"variablesStringIn", variablesStringIn},
        {"variablesStringOut", variablesStringOut},
        {"vectorizationIndices", vectorizationIndices}};
    return postPatch(this->__codeSuffix, data, filePath, "code");
}

int SolverAiClientSetup::patchCode(
    int id,
    std::string name,
    std::string filePath,
    std::string variablesStringIn,
    std::string variablesStringOut,
    std::string vectorizationIndices)
{
    nlohmann::json data;
    if (!name.empty())
        data["name"] = name;
    if (!variablesStringIn.empty())
        data["variablesStringIn"] = variablesStringIn;
    if (!variablesStringOut.empty())
        data["variablesStringOut"] = variablesStringOut;
    if (!vectorizationIndices.empty())
        data["vectorizationIndices"] = vectorizationIndices;
    return postPatch(this->__codeSuffix, data, filePath, "code", id);
}

int SolverAiClientSetup::postHardData(
    std::string name,
    std::string filePath,
    std::string vectorizationIndices)
{
    nlohmann::json data = {
        {"name", name},
        {"vectorizationIndices", vectorizationIndices}};
    return postPatch(this->__hardDataSuffix, data, filePath, "csv");
}

int SolverAiClientSetup::patchHardData(
    int id,
    std::string name,
    std::string filePath,
    std::string vectorizationIndices)
{
    nlohmann::json data;
    if (!name.empty())
        data["name"] = name;
    if (!vectorizationIndices.empty())
        data["vectorizationIndices"] = vectorizationIndices;
    return postPatch(this->__hardDataSuffix, data, filePath, "csv", id);
}

int SolverAiClientSetup::postSoftData(
    std::string name,
    std::string filePath,
    std::string variablesStringIn,
    std::string variablesStringOut,
    std::string vectorizationIndices)
{
    nlohmann::json data = {
        {"name", name},
        {"variablesStringIn", variablesStringIn},
        {"variablesStringOut", variablesStringOut},
        {"vectorizationIndices", vectorizationIndices}};
    return postPatch(this->__softDataSuffix, data, filePath, "csv");
}

int SolverAiClientSetup::patchSoftData(
    int id,
    std::string name,
    std::string filePath,
    std::string variablesStringIn,
    std::string variablesStringOut,
    std::string vectorizationIndices)
{
    nlohmann::json data;
    if (!name.empty())
        data["name"] = name;
    if (!variablesStringIn.empty())
        data["variablesStringIn"] = variablesStringIn;
    if (!variablesStringOut.empty())
        data["variablesStringOut"] = variablesStringOut;
    if (!vectorizationIndices.empty())
        data["vectorizationIndices"] = vectorizationIndices;
    return postPatch(this->__softDataSuffix, data, filePath, "csv", id);
}

int SolverAiClientSetup::postProblem(
    std::string problemName,
    std::vector<int> equationIds,
    std::vector<int> codeIds,
    std::vector<int> hardIds,
    std::vector<int> softIds)
{
    nlohmann::json data = {
        {"name", problemName},
        {"equations", equationIds},
        {"codes", codeIds},
        {"harddatas", hardIds},
        {"softdatas", softIds},
    };
    return postPatch(this->__problemSuffix, data);
}

int SolverAiClientSetup::patchProblem(
    int id,
    std::string problemName,
    std::vector<int> equationIds,
    std::vector<int> codeIds,
    std::vector<int> hardIds,
    std::vector<int> softIds)
{
    nlohmann::json data;
    if (!problemName.empty())
        data["name"] = problemName;
    if (!equationIds.empty())
        data["equations"] = equationIds;
    if (!codeIds.empty())
        data["codes"] = codeIds;
    if (!hardIds.empty())
        data["harddatas"] = hardIds;
    if (!softIds.empty())
        data["softdatas"] = softIds;
    return postPatch(this->__problemSuffix, data, "", "", id);
}

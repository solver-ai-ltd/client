#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>

#include <curl/curl.h>

#include "json.hpp"
// #include "cppcodec/base64_rfc4648.hpp"

#include "SolverAiClientSetup.h"

SolverAiClientSetup::SolverAiClientSetup(std::string datamanagerUrl, std::string token) {
    this->__base_url_DM = datamanagerUrl + "/api/data/";
    this->__headers = "Authorization: Token " + token;
    this->__equationSuffix = "equations";
    this->__codeSuffix = "code";
    this->__hardDataSuffix = "hard-datas";
    this->__softDataSuffix = "soft-datas";
    this->__problemSuffix = "problems";
}

bool SolverAiClientSetup::isStatusCodeOk(int statusCode) {
    return 200 <= statusCode && statusCode < 300;
}

int SolverAiClientSetup::post(
    std::string urlSuffix,
    nlohmann::json data,
    std::string filePath,
    std::string fileKey
) {
    std::string errors;
    int id = -1;

    CURL *curl;
    CURLcode res;
    std::string url = this->__base_url_DM + urlSuffix + "/";
    std::string response_string;
    long response_code;

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    struct curl_slist *header_list = NULL;
    struct curl_httppost *formpost = NULL;
    try {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_POST, 1L);

        header_list = curl_slist_append(header_list, this->__headers.c_str());

        if (data.contains("vectorizationIndices") &&
            data["vectorizationIndices"].get<std::string>().empty())
        {
            data.erase("vectorizationIndices");
        }

        if (filePath.empty()) {
            std::string json_str = data.dump();
            curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, json_str.size());
            curl_easy_setopt(curl, CURLOPT_COPYPOSTFIELDS, json_str.c_str());

            header_list = curl_slist_append(header_list, "Content-Type: application/json; charset: utf-8");
        }
        else {
            struct curl_httppost *lastptr = NULL;
            for (const auto &element : data.items()) {
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

        if(res != CURLE_OK) {
            throw std::runtime_error("Post failed");
        }

        data = nlohmann::json::parse(response_string);
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);

        if(!isStatusCodeOk(response_code))
            throw std::runtime_error(response_string);
        else
            id = data["id"].get<int>();

    } catch (const std::exception& e) {
        errors = "Exception: " + std::string(e.what());
    }

    if (filePath.empty())
        curl_formfree(formpost);
    curl_slist_free_all(header_list);
    curl_easy_cleanup(curl);
    curl_global_cleanup();

    if (!errors.empty()) {
        throw std::runtime_error(errors.c_str());
    }

    return id;
}

void SolverAiClientSetup::patchFile(
    std::string urlSuffix,
    int id, std::string filePath,
    std::string fileKey
) {
    std::string errors;

    CURL *curl;
    CURLcode res;
    std::string url = this->__base_url_DM + urlSuffix + "/" + std::to_string(id) + "/";
    std::string response_string;
    long response_code;

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    struct curl_slist *header_list = NULL;
    struct curl_httppost *formpost = NULL;
    try {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PATCH");

        header_list = curl_slist_append(header_list, this->__headers.c_str());

        struct curl_httppost *lastptr = NULL;
        curl_formadd(&formpost,
                     &lastptr,
                     CURLFORM_COPYNAME, fileKey.c_str(),
                     CURLFORM_FILE, filePath.c_str(),
                     CURLFORM_END);

        curl_easy_setopt(curl, CURLOPT_HTTPPOST, formpost);
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, header_list);

        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_string);

        res = curl_easy_perform(curl);

        if(res != CURLE_OK) {
            throw std::runtime_error("Patch failed");
        }

        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);

        if(!isStatusCodeOk(response_code))
            throw std::runtime_error(response_string);

    } catch (const std::exception& e) {
        errors = "Exception: " + std::string(e.what());
    }

    curl_formfree(formpost);
    curl_slist_free_all(header_list);
    curl_easy_cleanup(curl);
    curl_global_cleanup();

    if (!errors.empty()) {
        throw std::runtime_error(errors.c_str());
    }
}

size_t SolverAiClientSetup::writeCallback(void* contents, size_t size, size_t nmemb, std::string* userp) {
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

void SolverAiClientSetup::deleteAll(
    std::vector<int> equationIds,
    std::vector<int> codeIds,
    std::vector<int> hardIds,
    std::vector<int> softIds,
    int problemId
) {
    std::string errors;
    try {
        if (problemId != -1) {
            // Problem must be deleted first or will not allow deletion of models
            deleteIds(this->__problemSuffix, {problemId}, errors);
        }
        deleteIds(this->__equationSuffix, equationIds, errors);
        deleteIds(this->__codeSuffix, codeIds, errors);
        deleteIds(this->__hardDataSuffix, hardIds, errors);
        deleteIds(this->__softDataSuffix, softIds, errors);
    } catch (const std::exception& e) {
        std::cerr << "Failed Deleting: " << e.what() << std::endl;
    }
    if (!errors.empty()) {
        throw std::runtime_error(errors.c_str());
    }
}

void SolverAiClientSetup::deleteIds(std::string urlSuffix, std::vector<int> ids, std::string& errors) {
    CURL *curl;
    CURLcode res;
    std::string response_string;
    long response_code;

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    struct curl_slist *header_list = NULL;
    header_list = curl_slist_append(header_list, this->__headers.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, header_list);
    for (const int& id : ids) {
        try {
            std::string url = this->__base_url_DM + urlSuffix + "/" + std::to_string(id) + "/";
            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELETE");
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_string);

            // Comment out when done
            // curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

            res = curl_easy_perform(curl);
            
            if(res != CURLE_OK) {
                throw std::runtime_error("Post failed");
            }

            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);

            if(!isStatusCodeOk(response_code))
                throw std::runtime_error(response_string);

        } catch (const std::exception& e) {
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
    std::string vectorizationIndices
) {
    nlohmann::json data = {
        {"name", name},
        {"equationString", equationString},
        {"variablesString", variablesString},
        {"vectorizationIndices", vectorizationIndices}
    };
    return post(this->__equationSuffix, data);
}

int SolverAiClientSetup::postCode(
    std::string name,
    std::string filePath,
    std::string variablesStringIn,
    std::string variablesStringOut,
    std::string vectorizationIndices
) {
    nlohmann::json data = {
        {"name", name},
        {"variablesStringIn", variablesStringIn},
        {"variablesStringOut", variablesStringOut},
        {"vectorizationIndices", vectorizationIndices}
    };
    return post(this->__codeSuffix, data, filePath, "code");
}

int SolverAiClientSetup::postHardData(
    std::string name,
    std::string filePath,
    std::string vectorizationIndices
) {
    nlohmann::json data = {
        {"name", name},
        {"vectorizationIndices", vectorizationIndices}
    };
    return post(this->__hardDataSuffix, data, filePath, "csv");
}

int SolverAiClientSetup::postSoftData(std::string name, std::string filePath, std::string variablesStringIn, std::string variablesStringOut, std::string vectorizationIndices) {
    nlohmann::json data = {
        {"name", name},
        {"variablesStringIn", variablesStringIn},
        {"variablesStringOut", variablesStringOut},
        {"vectorizationIndices", vectorizationIndices}
    };
    return post(this->__softDataSuffix, data, filePath, "csv");
}

void SolverAiClientSetup::patchHardData(int id, std::string filePath) {
    patchFile(this->__hardDataSuffix, id, filePath, "csv");
}

void SolverAiClientSetup::patchSoftData(int id, std::string filePath) {
    patchFile(this->__softDataSuffix, id, filePath, "csv");
}

int SolverAiClientSetup::postProblem(
    std::string problemName,
    std::vector<int> equationIds,
    std::vector<int> codeIds,
    std::vector<int> hardIds,
    std::vector<int> softIds
) {
    nlohmann::json data = {
        {"name", problemName},
        {"equations", equationIds},
        {"codes", codeIds},
        {"harddatas", hardIds},
        {"softdatas", softIds},
        {"tags", std::vector<int>()}
    };
    return post(this->__problemSuffix, data);
}

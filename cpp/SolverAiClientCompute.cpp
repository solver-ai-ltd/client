#include <iostream>
#include <string>
#include <curl/curl.h>

#include "json.hpp"

#include "SolverAiComputeInput.h"
#include "SolverAiComputeResults.h"

using json = nlohmann::json;

#include "SolverAiClientCompute.h"

SolverAiClientCompute::SolverAiClientCompute(std::string computerUrl, std::string token, int problemId)
{
    this->__base_url_Computer = computerUrl + "/";
    this->__problemId = problemId;
    this->__headers = "Authorization: Token " + token;
}

bool SolverAiClientCompute::isStatusCodeOk(int statusCode)
{
    return 200 <= statusCode && statusCode < 300;
}

void SolverAiClientCompute::getProblemSetup(
    std::vector<std::string> &inputs,
    std::vector<std::string> &outputs
) {
    CURL *curl;
    CURLcode res;
    std::string url = this->__base_url_Computer + "problem_setup/" + std::to_string(this->__problemId);
    std::string response_string;
    long response_code;

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    struct curl_slist *header_list = NULL;
    try
    {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

        header_list = curl_slist_append(header_list, this->__headers.c_str());
        header_list = curl_slist_append(header_list, "Content-Type: application/json; charset: utf-8");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, header_list);

        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_string);

        // Comment out when done
        // curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

        res = curl_easy_perform(curl);

        if (res != CURLE_OK)
        {
            throw std::runtime_error("Post failed");
        }

        nlohmann::json data = nlohmann::json::parse(response_string);
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);

        if (!isStatusCodeOk(response_code))
            throw std::runtime_error(response_string);

        curl_slist_free_all(header_list);
        curl_easy_cleanup(curl);
        curl_global_cleanup();

        inputs.clear();
        for (const auto& input : data["inputs"]) {
            inputs.push_back(input);
        }

        outputs.clear();
        for (const auto& input : data["outputs"]) {
            outputs.push_back(input);
        }
    }
    catch (const std::exception &e)
    {
        curl_slist_free_all(header_list);
        curl_easy_cleanup(curl);
        curl_global_cleanup();

        throw std::runtime_error("Exception: " + std::string(e.what()));
    }
}

SolverAiComputeResults SolverAiClientCompute::runSolver(const SolverAiComputeInput &input)
{
    nlohmann::json data = input.getJson();

    CURL *curl;
    CURLcode res;
    std::string url = this->__base_url_Computer + "solvejson/";
    std::string response_string;
    long response_code;

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    struct curl_slist *header_list = NULL;
    try
    {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_POST, 1L);

        header_list = curl_slist_append(header_list, this->__headers.c_str());
        header_list = curl_slist_append(header_list, "Content-Type: application/json; charset: utf-8");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, header_list);

        std::string json_str = data.dump();
        curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, json_str.size());
        curl_easy_setopt(curl, CURLOPT_COPYPOSTFIELDS, json_str.c_str());

        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_string);

        // Comment out when done
        // curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

        res = curl_easy_perform(curl);

        if (res != CURLE_OK)
        {
            throw std::runtime_error("Post failed");
        }

        data = nlohmann::json::parse(response_string);
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);

        if (!isStatusCodeOk(response_code))
            throw std::runtime_error(response_string);

        data = data["results"];

        curl_slist_free_all(header_list);
        curl_easy_cleanup(curl);
        curl_global_cleanup();

        return SolverAiComputeResults(data);
    }
    catch (const std::exception &e)
    {
        curl_slist_free_all(header_list);
        curl_easy_cleanup(curl);
        curl_global_cleanup();

        throw std::runtime_error("Exception: " + std::string(e.what()));
    }
}

size_t SolverAiClientCompute::writeCallback(void *contents, size_t size, size_t nmemb, std::string *userp)
{
    ((std::string *)userp)->append((char *)contents, size * nmemb);
    return size * nmemb;
}

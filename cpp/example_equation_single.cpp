#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <exception>
#include <filesystem>

#include "SolverAiClientSetup.h"
#include "SolverAiClientCompute.h"

#include "json.hpp"
#include "setup.h"

int main() {
    std::map<std::string, std::string> config = readSetupFile(data_file_folder_path);
    const std::string datamanagerUrl = config["datamanagerUrl"];
    const std::string computerUrl = config["computerUrl"];
    const std::string token = config["token"];

    SolverAiClientSetup solverAiClientSetup(datamanagerUrl, token);

    std::vector<int> equation_ids;
    std::vector<int> code_ids;
    std::vector<int> hard_data_ids;
    std::vector<int> soft_data_ids;
    int problem_id = -1;

    try {
        int id = solverAiClientSetup.postEquation(
            "test equation",
            "y = x",
            "x"
        );
        equation_ids.push_back(id);

        problem_id = solverAiClientSetup.postProblem(
            "Test Problem",
            equation_ids,
            code_ids,
            hard_data_ids,
            soft_data_ids
        );

        SolverAiClientCompute solverAiClientCompute(computerUrl, token, problem_id);

        nlohmann::json problemSetupJson = solverAiClientCompute.getProblemSetup();

        nlohmann::json expectedProblemSetupJson = {
            {"id", problem_id},
            {"inputs", {"x"}},
            {"outputs", {"y"}}
        };

        if (problemSetupJson != expectedProblemSetupJson) {
            throw std::runtime_error("Problem Setup JSON does not match expected value.");
        }

        nlohmann::json inputJson = {
            {"id", problem_id},
            {"inputs", {
                {"x",
                    {
                        {"Min", -2},
                        {"Max", 2},
                        {"Constant", 0},
                        {"Integer", 0}
                    }
                }
            }},
            {"constraints", {
                {"y",
                    {
                        {"Operation", "greater than"},
                        {"Value1", 1},
                        {"Value2", 0}
                        // Operation options are:
                        // - 'smaller than': requires Value1
                        // - 'greater than': requires Value1
                        // - 'equal to': requires Value1
                        // - 'inside range': requires Value1 and Value2
                        // - 'outside range': requires Value1 and Value2
                    }
                }
            }},
            {"objectives", {
                {"y",
                    {
                        {"Operation", "minimize"}
                        // Operation options are:
                        // - 'minimize'
                        // - 'maximize'
                    }
                }
            }}
        };

        nlohmann::json results = solverAiClientCompute.runSolver(inputJson);

        nlohmann::json expectedResults = {
            {"Number Of Results", 1},
            {"Objective Variable Names", "['y']"},
            {"F0", "[1.]"},
            {"Constraint Variable Names ", "['y']"},
            {"G0", "[1.]"},
            {"Input Variable Names", "['x']"},
            {"X0", "[1.0000000000200555]"},
            {"Output Variable Names", "['y']"},
            {"Y0", "[1.0000000000200555]"}
        };

        if (results != expectedResults) {
            throw std::runtime_error("Results do not match expected value.");
        }

        std::cout << "Test was successful!!!" << std::endl;

        solverAiClientSetup.deleteAll(equation_ids, code_ids, hard_data_ids, soft_data_ids, problem_id);
    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;

        solverAiClientSetup.deleteAll(equation_ids, code_ids, hard_data_ids, soft_data_ids, problem_id);
    }

    return 0;
}

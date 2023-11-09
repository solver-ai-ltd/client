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
        int id = solverAiClientSetup.postSoftData(
            "soft_data_basic",
            (std::filesystem::path(data_file_folder_path) / "soft_data_basic.csv").string(),
            "in1, in2",
            "out1, out2"
        );
        soft_data_ids.push_back(id);

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
            {"inputs", {"in1", "in2"}},
            {"outputs", {"out1", "out1_std", "out2", "out2_std"}}
        };

        if (problemSetupJson != expectedProblemSetupJson) {
            throw std::runtime_error("Problem Setup JSON does not match expected value.");
        }

        nlohmann::json inputJson = {
            {"id", problem_id},
            {"inputs", {
                {"in1",
                    {
                        {"Min", 0},
                        {"Max", 3.141592654},
                        {"Constant", 0},
                        {"Integer", 0}
                    }
                },
                {"in2",
                    {
                        {"Min", 0},
                        {"Max", 3.141592654},
                        {"Constant", 0},
                        {"Integer", 0}
                    }
                }
            }},
            {"constraints", {
                {"out1",
                    {
                        {"Operation", "greater than"},
                        {"Value1", 0.4999999},
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
                {"out1",
                    {
                        {"Operation", "minimize"}
                        // Operation options are:
                        // - 'minimize'
                        // - 'maximize'
                    }
                },
                {"out2",
                    {
                        {"Operation", "minimize"}
                    }
                }
            }}
        };

        nlohmann::json results = solverAiClientCompute.runSolver(inputJson);

        nlohmann::json expectedResults = {
            {"Number Of Results", 1},
            {"Objective Variable Names", "['out1', 'out2']"},
            {"F0", "[ 0.4999999  -0.86585586]"},
            {"Constraint Variable Names ", "['out1']"},
            {"G0", "[0.4999999]"},
            {"Input Variable Names", "['in1', 'in2']"},
            {"X0", "[0.5215092495460986, 2.6159045950892796]"},
            {"Output Variable Names", "['out1', 'out1_std', 'out2', 'out2_std']"},
            {"Y0", "[0.4999999001579293, 0.0016245602954137266, -0.865855861022438, 0.002297475202666934]"}
        };

        if (results != expectedResults) {
            throw std::runtime_error("Results do not match expected value.");
        }

        solverAiClientSetup.patchSoftData(
            id,
            (std::filesystem::path(data_file_folder_path) / "soft_data_basic_mod.csv").string()
        );

        results = solverAiClientCompute.runSolver(inputJson);

        expectedResults = {
            {"Number Of Results", 1},
            {"Objective Variable Names", "['out1', 'out2']"},
            {"F0", "[0.54110187 0.69113252]"},
            {"Constraint Variable Names ", "['out1']"},
            {"G0", "[0.54110187]"},
            {"Input Variable Names", "['in1', 'in2']"},
            {"X0", "[3.1415728561037772, 1.2296692581770965]"},
            {"Output Variable Names", "['out1', 'out1_std', 'out2', 'out2_std']"},
            {"Y0", "[0.5411018748171588, 0.3187753180868706, 0.691132523453972, 0.35776990343208503]"}
        };

        if (results != expectedResults) {
            throw std::runtime_error("Results do not match expected value.");
        }

        std::cout << "Test was successful!!!" << std::endl;

        solverAiClientSetup.deleteAll(equation_ids, code_ids, hard_data_ids, soft_data_ids, problem_id);
    } catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;

        solverAiClientSetup.deleteAll(equation_ids, code_ids, hard_data_ids, soft_data_ids, problem_id);
    }

    return 0;
}

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
        int id = solverAiClientSetup.postHardData(
            "hard_data_basic_1",
            (std::filesystem::path(data_file_folder_path) / "hard_data_basic_1.csv").string()
        );
        hard_data_ids.push_back(id);

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
            {"inputs", std::vector<std::string>()},
            {"outputs", {"C1", "T1", "var1", "var2"}}
        };

        if (problemSetupJson != expectedProblemSetupJson) {
            throw std::runtime_error("Problem Setup JSON does not match expected value.");
        }

        auto noData = std::map<std::string, std::map<std::string, float>>();
        nlohmann::json inputJson = {
            {"id", problem_id},
            {"inputs", noData},
            {"constraints", noData},
            {"objectives", {
                {"T1",
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
            {"Objective Variable Names", "['T1']"},
            {"F0", "[1.]"},
            {"Constraint Variable Names ", "[]"},
            {"G0", "[]"},
            {"Input Variable Names", "[]"},
            {"X0", "[]"},
            {"Output Variable Names", "['C1', 'T1', 'var1', 'var2']"},
            {"Y0", "[4.1, 1.0, 'A', 'G']"}
        };

        if (results != expectedResults) {
            throw std::runtime_error("Results do not match expected value.");
        }

        solverAiClientSetup.patchHardData(
            id,
            (std::filesystem::path(data_file_folder_path) / "hard_data_basic_1_mod.csv").string()
        );

        results = solverAiClientCompute.runSolver(inputJson);

        expectedResults = {
            {"Number Of Results", 1},
            {"Objective Variable Names", "['T1']"},
            {"F0", "[1.2]"},
            {"Constraint Variable Names ", "[]"},
            {"G0", "[]"},
            {"Input Variable Names", "[]"},
            {"X0", "[]"},
            {"Output Variable Names", "['C1', 'T1', 'var1', 'var2']"},
            {"Y0", "[4.1, 1.2, 'B', 'H']"}
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

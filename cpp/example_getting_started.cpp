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
            "Batteries_API",
            (std::filesystem::path(data_file_folder_path) / "getting_started/Batteries.csv").string()
        );
        hard_data_ids.push_back(id);

        id = solverAiClientSetup.postHardData(
            "Bodies_API",
            (std::filesystem::path(data_file_folder_path) / "/getting_started/Bodies.csv").string()
        );
        hard_data_ids.push_back(id);

        id = solverAiClientSetup.postHardData(
            "Motors_API",
            (std::filesystem::path(data_file_folder_path) / "getting_started/Motors.csv").string()
        );
        hard_data_ids.push_back(id);

        id = solverAiClientSetup.postSoftData(
            "range_unit_energy_API",
            (std::filesystem::path(data_file_folder_path) / "getting_started/range_unit_energy.csv").string(),
            "motor_power, battery_capacity, body_weight",
            "range_unit_energy"
        );
        soft_data_ids.push_back(id);

        int id_total_cost = solverAiClientSetup.postEquation(
            "total_cost_API",
            "total_cost = motor_price + battery_price * efficiency_price_factor + body_price",
            "motor_price, battery_price, body_price, efficiency_price_factor"
        );
        equation_ids.push_back(id_total_cost);

        int id_range = solverAiClientSetup.postEquation(
            "range_API",
            "range = motor_power * battery_capacity * range_unit_energy",
            "motor_power, battery_capacity, range_unit_energy"
        );
        equation_ids.push_back(id_range);

        id = solverAiClientSetup.postCode(
            "efficiency_price_factor_API",
            (std::filesystem::path(data_file_folder_path) / "getting_started/efficiency_price_factor.py").string(),
            "battery_efficiency",
            "efficiency_price_factor"
        );
        code_ids.push_back(id);

        problem_id = solverAiClientSetup.postProblem(
            "Getting Started API",
            equation_ids,
            code_ids,
            hard_data_ids,
            soft_data_ids
        );

        SolverAiClientCompute solverAiClientCompute(computerUrl, token, problem_id);

        nlohmann::json problemSetupJson = solverAiClientCompute.getProblemSetup();

        auto noData = std::map<std::string, std::map<std::string, float>>();
        nlohmann::json inputJson = {
            {"id", problem_id},
            {"inputs", noData},
            {"constraints", noData},
            {"objectives", {
                {"range",
                    {
                        {"Operation", "maximize"}
                        // Operation options are:
                        // - 'minimize'
                        // - 'maximize'
                    }
                },
                {"total_cost",
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

        if (results.find("Number Of Results") == results.end() || results["Number Of Results"] < 1) {
            throw std::runtime_error("Results not as expected.");
        }

        // results should have value similar to
        // {
        //     {"Number Of Results", 1},
        //     {"Objective Variable Names", "['T1']"},
        //     {"F0", "[1.]"},
        //     {"Constraint Variable Names ", "[]"},
        //     {"G0", "[]"},
        //     {"Input Variable Names", "[]"},
        //     {"X0", "[]"},
        //     {"Output Variable Names", "['C1', 'T1', 'var1', 'var2']"},
        //     {"Y0", "[4.1, 1.0, 'A', 'G']"}
        // };

        solverAiClientSetup.patchEquation(
            id_total_cost,
            "",
            "total_cost = motor_price + battery_num * battery_price * efficiency_price_factor + body_price",
            "motor_price, battery_price, body_price, efficiency_price_factor, battery_num",
            ""
        );

        solverAiClientSetup.patchEquation(
            id_range,
            "",
            "range = motor_power * battery_num * battery_capacity * range_unit_energy",
            "motor_power, battery_capacity, range_unit_energy, battery_num",
            ""
        );

        inputJson = {
            {"id", problem_id},
            {"inputs", {
                {"battery_num",
                    {
                        {"Min", 1},
                        {"Max", 3},
                        {"Constant", 0},
                        {"Integer", 1}
                    }
                }
            }},
            {"constraints", {
                {"range",
                    {
                        {"Operation", "greater than"},
                        {"Value1", 200000},
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
                {"range",
                    {
                        {"Operation", "maximize"}
                        // Operation options are:
                        // - 'minimize'
                        // - 'maximize'
                    }
                },
                {"total_cost",
                    {
                        {"Operation", "minimize"}
                    }
                }
            }}
        };

        results = solverAiClientCompute.runSolver(inputJson);

        if (results.find("Number Of Results") == results.end() || results["Number Of Results"] < 1) {
            throw std::runtime_error("Results not as expected.");
        }

        // results should have value similar to
        // {
        //     {"Number Of Results", 1},
        //     {"Objective Variable Names", "['T1']"},
        //     {"F0", "[1.2]"},
        //     {"Constraint Variable Names ", "[]"},
        //     {"G0", "[]"},
        //     {"Input Variable Names", "[]"},
        //     {"X0", "[]"},
        //     {"Output Variable Names", "['C1', 'T1', 'var1', 'var2']"},
        //     {"Y0", "[4.1, 1.2, 'B', 'H']"}
        // };

        std::cout << "Test was successful!!!" << std::endl;

        solverAiClientSetup.deleteAll(equation_ids, code_ids, hard_data_ids, soft_data_ids, problem_id);
    } catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;

        solverAiClientSetup.deleteAll(equation_ids, code_ids, hard_data_ids, soft_data_ids, problem_id);
    }

    return 0;
}

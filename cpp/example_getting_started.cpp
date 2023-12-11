#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <exception>
#include <filesystem>

#include "SolverAiComputeInput.h"
#include "SolverAiComputeResults.h"
#include "SolverAiClientSetup.h"
#include "SolverAiClientCompute.h"

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
            (std::filesystem::path(data_file_folder_path) / "getting_started/Bodies.csv").string()
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

        std::vector<std::string> inputs, outputs;
        solverAiClientCompute.getProblemSetup(inputs, outputs);

        SolverAiComputeInput input(problem_id);
        input.addObjective("range", SolverAiComputeInput::OBJECTIVE::MAXIMIZE);
        input.addObjective("total_cost", SolverAiComputeInput::OBJECTIVE::MINIMIZE);

        auto results = solverAiClientCompute.runSolver(input);

        if (results.getNumberOfResults() < 1) {
            throw std::runtime_error("Results not as expected.");
        }

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

        SolverAiComputeInput newInput(problem_id);
        newInput.addInput("battery_num", 1, 3, false, true);
        newInput.addConstraint("range", SolverAiComputeInput::CONSTRAINT::GREATER_THAN, 200000);
        newInput.addObjective("range", SolverAiComputeInput::OBJECTIVE::MAXIMIZE);
        newInput.addObjective("total_cost", SolverAiComputeInput::OBJECTIVE::MINIMIZE);

        results = solverAiClientCompute.runSolver(newInput);

        if (results.getNumberOfResults() < 1) {
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

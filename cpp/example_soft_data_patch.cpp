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
    std::map<std::string, std::string> config = readSetupFile();
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

        std::vector<std::string> inputs, outputs;
        solverAiClientCompute.getProblemSetup(inputs, outputs);

        if (
            inputs != std::vector<std::string>({"in1", "in2"}) ||
            outputs != std::vector<std::string>({"out1", "out1_std", "out2", "out2_std"})
        ) {
            throw std::runtime_error("Problem Setup JSON does not match expected value.");
        }

        SolverAiComputeInput input(problem_id);
        input.addInput("in1", 0, 3.141592654, false, false);
        input.addInput("in2", 0, 3.141592654, false, false);
        input.addConstraint("out1", SolverAiComputeInput::CONSTRAINT::GREATER_THAN, 0.4999999);
        input.addObjective("out1", SolverAiComputeInput::OBJECTIVE::MINIMIZE);
        input.addObjective("out2", SolverAiComputeInput::OBJECTIVE::MINIMIZE);

        auto results = solverAiClientCompute.runSolver(input);

        if (results.getNumberOfResults() < 1) {
            throw std::runtime_error("Results not as expected.");
        }

        solverAiClientSetup.patchSoftData(
            id,
            "",
            (std::filesystem::path(data_file_folder_path) / "soft_data_basic_mod.csv").string(),
            ""
        );

        results = solverAiClientCompute.runSolver(input);

        if (results.getNumberOfResults() < 1) {
            throw std::runtime_error("Results not as expected.");
        }

        std::cout << "Test was successful!!!" << std::endl;

        solverAiClientSetup.deleteAll(equation_ids, code_ids, hard_data_ids, soft_data_ids, problem_id);
    } catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;

        solverAiClientSetup.deleteAll(equation_ids, code_ids, hard_data_ids, soft_data_ids, problem_id);
    }

    return 0;
}

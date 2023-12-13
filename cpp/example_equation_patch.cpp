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

        std::vector<std::string> inputs, outputs;
        solverAiClientCompute.getProblemSetup(inputs, outputs);

        if (
            inputs != std::vector<std::string>({"x"}) ||
            outputs != std::vector<std::string>({"y"})
        ) {
            throw std::runtime_error("Problem Setup JSON does not match expected value.");
        }

        SolverAiComputeInput input(problem_id);
        input.addInput("x", -2, 2, false, false);
        input.addConstraint("y", SolverAiComputeInput::CONSTRAINT::GREATER_THAN, 1);
        input.addObjective("y", SolverAiComputeInput::OBJECTIVE::MINIMIZE);

        auto results = solverAiClientCompute.runSolver(input);

        if (results.getNumberOfResults() < 1) {
            throw std::runtime_error("Results not as expected.");
        }

        solverAiClientSetup.patchEquation(
            equation_ids[0],
            "",
            "y1 = x1",
            "x1",
            ""
        );

        solverAiClientCompute.getProblemSetup(inputs, outputs);

        if (
            inputs != std::vector<std::string>({"x1"}) ||
            outputs != std::vector<std::string>({"y1"})
        ) {
            throw std::runtime_error("Problem Setup JSON does not match expected value.");
        }

        std::cout << "Test was successful!!!" << std::endl;

        solverAiClientSetup.deleteAll(equation_ids, code_ids, hard_data_ids, soft_data_ids, problem_id);
    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;

        solverAiClientSetup.deleteAll(equation_ids, code_ids, hard_data_ids, soft_data_ids, problem_id);
    }

    return 0;
}

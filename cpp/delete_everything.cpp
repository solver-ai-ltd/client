#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <exception>
#include <filesystem>

#include "SolverAiClientSetup.h"

#include "setup.h"

int main() {
    std::map<std::string, std::string> config = readSetupFile();
    const std::string datamanagerUrl = config["datamanagerUrl"];
    const std::string token = config["token"];

    SolverAiClientSetup solverAiClientSetup(datamanagerUrl, token);

    try {
        std::string errors;
        
        errors += solverAiClientSetup.deleteProblems();
        errors += solverAiClientSetup.deleteEquations();
        errors += solverAiClientSetup.deleteCodes();
        errors += solverAiClientSetup.deleteHardDatas();
        errors += solverAiClientSetup.deleteSoftDatas();

        if (errors.length() > 0) {
            throw std::runtime_error(errors);
        }
    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}

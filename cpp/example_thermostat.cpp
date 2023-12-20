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

        int numRooms = 8;
        int numForecastIntervals = 4;

        for (int jInt = 1; jInt < numForecastIntervals; ++jInt) {
            std::string j = std::to_string(jInt);
            int id = solverAiClientSetup.postEquation(
                "th_" + j,
                "th_" + j + " = (th_0 + " + j + " * Dth)%24",
                "th_0, Dth"
            );
            equation_ids.push_back(id);

            id = solverAiClientSetup.postEquation(
                "weekday_" + j,
                "weekday_" + j + " = (weekday_0 + np.floor((th_0 + " + j + " * Dth)/24))%7",
                "th_0, Dth, weekday_0"
            );
            equation_ids.push_back(id);
        }

        for (int jInt = 0; jInt < numForecastIntervals - 1; ++jInt) {
            std::string j = std::to_string(jInt);
            int id = solverAiClientSetup.postEquation(
                "Te_" + j,
                "Te_" + j + " = Te",
                "Te"
            );
            equation_ids.push_back(id);
        }

        int id = solverAiClientSetup.postSoftData(
            "Motion Sensor Data",
            (std::filesystem::path(data_file_folder_path) / "thermostat/motionSensorData.csv").string(),
            "weekday, th",
            "motion_0,motion_1,motion_2,motion_3,motion_4,motion_5,motion_6,motion_7",
            "2-" + std::to_string(numForecastIntervals - 1)
        );
        soft_data_ids.push_back(id);

        id = solverAiClientSetup.postSoftData(
            "Temperature Sensor Data",
            (std::filesystem::path(data_file_folder_path) / "thermostat/temperatureSensorData.csv").string(),
            "C_0,C_1,C_2,C_3,C_4,C_5,C_6,C_7,T_0,T_1,T_2,T_3,T_4,T_5,T_6,T_7,Te",
            "DTh_0,DTh_1,DTh_2,DTh_3,DTh_4,DTh_5,DTh_6,DTh_7,power",
            "0-" + std::to_string(numForecastIntervals - 2)
        );
        soft_data_ids.push_back(id);

        for (int iInt = 0; iInt < numRooms; ++iInt) {
            std::string i = std::to_string(iInt);
            for (int jInt = 1; jInt < numForecastIntervals; ++jInt) {
                std::string j = std::to_string(jInt);
                std::string j_1 = std::to_string(jInt - 1);
                int id = solverAiClientSetup.postEquation(
                    "T_" + i + "_" + j,
                    "T_" + i + "_" + j + " = T_" + i + "_" + j_1 + " + DTh_" + i + "_" + j_1 + "*Dth",
                    "T_" + i + "_" + j_1 + ", DTh_" + i + "_" + j_1 + ", Dth"
                );
                equation_ids.push_back(id);
            }
        }

        for (int iInt = 0; iInt < numRooms; ++iInt) {
            std::string i = std::to_string(iInt);
            for (int jInt = 2; jInt < numForecastIntervals; ++jInt) {
                std::string j = std::to_string(jInt);
                int id = solverAiClientSetup.postEquation(
                    "Tconstr_" + i + "_" + j,
                    "Tconstr_" + i + "_" + j + " = (T_" + i + "_" + j + " - requiredT_" + i + ") if motion_" + i + "_" + j + " > 0.4 else 999",
                    "T_" + i + "_" + j + ", motion_" + i + "_" + j + ", requiredT_" + i
                );
                equation_ids.push_back(id);

                id = solverAiClientSetup.postEquation(
                    "TconstrMin_" + i + "_" + j,
                    "TconstrMin_" + i + "_" + j + " = T_" + i + "_" + j + " - Tmin_" + i,
                    "T_" + i + "_" + j + ", Tmin_" + i
                );
                equation_ids.push_back(id);
            }
        }

        std::string powerEquation = "power = power_1";
        std::string powerVarNames = "power_1";
        for (int jInt = 2; jInt < numForecastIntervals - 1; ++jInt) {
            std::string j = std::to_string(jInt);
            powerEquation += "+power_" + j;
            powerVarNames += ",power_" + j;
        }
        id = solverAiClientSetup.postEquation(
            "power",
            powerEquation,
            powerVarNames
        );
        equation_ids.push_back(id);

        problem_id = solverAiClientSetup.postProblem(
            "Thermostat API",
            equation_ids,
            code_ids,
            hard_data_ids,
            soft_data_ids
        );

        SolverAiComputeInput input(problem_id);

        // Current weekday and time at which we are performing the calculation
        input.addInput("weekday_0", 0, 0, true, false);
        input.addInput("th_0", 7.0, 0, true, false);

        // Current external temperature
        input.addInput("Te", 5, 0, true, false);

        // Current status of the radiators and temperature in each room
        for (int iInt = 0; iInt < numRooms; ++iInt) {
            std::string i = std::to_string(iInt);
            input.addInput("C_" + i + "_0", 0, 0, true, true);
            input.addInput("T_" + i + "_0", 25, 0, true, false);
        }

        // Required temperature when motion is detected and minimum temperature
        // when no motion is detected
        for (int iInt = 0; iInt < numRooms; ++iInt) {
            std::string i = std::to_string(iInt);
            input.addInput("requiredT_" + i, 21, 0, true, false);
            input.addInput("Tmin_" + i, 15, 0, true, false);
        }

        // Time interval to be considered between predictions
        input.addInput("Dth", 1.0/6, 0, true, false);

        // Setting up the ranges allowed for the radiators 0 is off 1 is on.
        for (int iInt = 0; iInt < numRooms; ++iInt) {
            std::string i = std::to_string(iInt);
            for (int jInt = 1; jInt < numForecastIntervals - 1; ++jInt) {
                std::string j = std::to_string(jInt);
                input.addInput("C_" + i + "_" + j, 0, 1, false, true);
            }
        }

        // Setting up of constraints requiring that for at future times
        // the temperature is greater than the minimum or greater than
        // the temperature when motion is detected
        for (int iInt = 0; iInt < numRooms; ++iInt) {
            std::string i = std::to_string(iInt);
            for (int jInt = 2; jInt < numForecastIntervals; ++jInt) {
                std::string j = std::to_string(jInt);
                input.addConstraint("Tconstr_" + i + "_" + j, SolverAiComputeInput::CONSTRAINT::GREATER_THAN, 0.0);
                input.addConstraint("TconstrMin_" + i + "_" + j, SolverAiComputeInput::CONSTRAINT::GREATER_THAN, 0.0);
            }
        }

        input.addObjective("power", SolverAiComputeInput::OBJECTIVE::MINIMIZE);

        input.setSolverSetup(true, 1);

        SolverAiClientCompute solverAiClientCompute(computerUrl, token, problem_id);

        auto results = solverAiClientCompute.runSolver(input);

        if (results.getNumberOfResults() < 1) {
            throw std::runtime_error("Results not as expected.");
        }

        std::cout << "Test was successful!!!" << std::endl;

        solverAiClientSetup.deleteAll(equation_ids, code_ids, hard_data_ids, soft_data_ids, problem_id);
    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;

        solverAiClientSetup.deleteAll(equation_ids, code_ids, hard_data_ids, soft_data_ids, problem_id);
    }

    return 0;
}

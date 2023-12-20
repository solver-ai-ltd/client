from os import path

from SolverAiComputeInput import SolverAiComputeInput, \
    OBJECTIVE, CONSTRAINT
from SolverAiClientSetup import SolverAiClientSetup
from SolverAiClientCompute import SolverAiClientCompute

from setup import token, datamanagerUrl, computerUrl, \
    data_file_folder_path


def main():
    solverAiClientSetup = \
        SolverAiClientSetup(datamanagerUrl, token)

    equation_ids = list()
    code_ids = list()
    hard_data_ids = list()
    soft_data_ids = list()
    problem_id = None

    try:

        numRooms = 8
        numForecastIntervals = 4

        for j in range(1, numForecastIntervals):

            id = solverAiClientSetup.postEquation(
                name=f'th_{j}',
                equationString=f'th_{j} = (th_0 + {j} * Dth)%24',
                variablesString='th_0, Dth'
            )
            equation_ids.append(id)

            id = solverAiClientSetup.postEquation(
                name=f'weekday_{j}',
                equationString=f'weekday_{j} = (weekday_0 + np.floor((th_0 + {j} * Dth)/24))%7',
                variablesString='th_0, Dth, weekday_0'
            )
            equation_ids.append(id)

        for j in range(0, numForecastIntervals-1):

            id = solverAiClientSetup.postEquation(
                name=f'Te_{j}',
                equationString=f'Te_{j} = Te',
                variablesString='Te'
            )
            equation_ids.append(id)

        id = solverAiClientSetup.postSoftData(
            'Motion Sensor Data',
            path.join(data_file_folder_path,
                      'thermostat/motionSensorData.csv'),
            'weekday, th',
            'motion_0,motion_1,motion_2,motion_3,motion_4,motion_5,motion_6,motion_7',
            vectorizationIndices=f'2-{numForecastIntervals-1}'
        )
        soft_data_ids.append(id)

        id = solverAiClientSetup.postSoftData(
            name='Temperature Sensor Data',
            filePath=path.join(data_file_folder_path,
                               'thermostat/temperatureSensorData.csv'),
            variablesStringIn='C_0,C_1,C_2,C_3,C_4,C_5,C_6,C_7,T_0,T_1,T_2,T_3,T_4,T_5,T_6,T_7,Te',
            variablesStringOut='DTh_0,DTh_1,DTh_2,DTh_3,DTh_4,DTh_5,DTh_6,DTh_7,power',
            vectorizationIndices=f'0-{numForecastIntervals-2}'
        )
        soft_data_ids.append(id)

        for i in range(numRooms):
            for j in range(1, numForecastIntervals):

                id = solverAiClientSetup.postEquation(
                    name=f'T_{i}_{j}',
                    equationString=f'T_{i}_{j} = T_{i}_{j-1} + DTh_{i}_{j-1}*Dth',
                    variablesString=f'T_{i}_{j-1}, DTh_{i}_{j-1}, Dth'
                )
                equation_ids.append(id)

        for i in range(numRooms):
            for j in range(2, numForecastIntervals):

                id = solverAiClientSetup.postEquation(
                    name=f'Tconstr_{i}_{j}',
                    equationString=f'Tconstr_{i}_{j} = (T_{i}_{j} - requiredT_{i}) if motion_{i}_{j} > 0.4 else 999',
                    variablesString=f'T_{i}_{j}, motion_{i}_{j}, requiredT_{i}'
                )
                equation_ids.append(id)

                id = solverAiClientSetup.postEquation(
                    name=f'TconstrMin_{i}_{j}',
                    equationString=f'TconstrMin_{i}_{j} = T_{i}_{j} - Tmin_{i}',
                    variablesString=f'T_{i}_{j}, Tmin_{i}'
                )
                equation_ids.append(id)

        powerEquation = 'power = power_1'
        powerVarNames = 'power_1'
        for j in range(2, numForecastIntervals-1):
            powerEquation += f'+power_{j}'
            powerVarNames += f',power_{j}'
        id = solverAiClientSetup.postEquation(
            name='power',
            equationString=powerEquation,
            variablesString=powerVarNames
        )
        equation_ids.append(id)

        problem_id = solverAiClientSetup.postProblem(
            'Thermostat API',
            equationIds=equation_ids,
            codeIds=code_ids,
            hardIds=hard_data_ids,
            softIds=soft_data_ids
        )

        input = SolverAiComputeInput(problem_id)

        # Current weekday and time at which we are performing the calculation
        input.addInput(
            name='weekday_0',
            Min=0,
            Max=0,
            is_constant=True,
            is_integer=False
        )
        input.addInput(
            name='th_0',
            Min=7.0,
            Max=0,
            is_constant=True,
            is_integer=False
        )

        # Current external temperature
        input.addInput(
            name='Te',
            Min=5,
            Max=0,
            is_constant=True,
            is_integer=False
        )

        # Current status of the radiators and temperature in each room
        for i in range(numRooms):
            input.addInput(
                name=f'C_{i}_0',
                Min=0,
                Max=0,
                is_constant=True,
                is_integer=True
            )
            input.addInput(
                name=f'T_{i}_0',
                Min=25,
                Max=0,
                is_constant=True,
                is_integer=False
            )

        # Required temperature when motion is detected and minimum temperature
        # when no motion is detected
        for i in range(numRooms):
            input.addInput(
                name=f'requiredT_{i}',
                Min=21,
                Max=0,
                is_constant=True,
                is_integer=False
            )
            input.addInput(
                name=f'Tmin_{i}',
                Min=15,
                Max=0,
                is_constant=True,
                is_integer=False
            )

        # Time interval to be considered between predicitons
        input.addInput('Dth', 1/6, 0, True, False)

        # Setting up the ranges allowed for the radiators 0 is off 1 is on.
        for i in range(numRooms):
            for j in range(1, numForecastIntervals-1):
                input.addInput(
                    name=f'C_{i}_{j}',
                    Min=0,
                    Max=1,
                    is_constant=False,
                    is_integer=True
                )

        # Setting up of constraints requireing that for at future times
        # the temperature is greater than the minimum or greater than
        # the temperature when motion is detected
        for i in range(numRooms):
            for j in range(2, numForecastIntervals):
                input.addConstraint(
                    name=f'Tconstr_{i}_{j}',
                    operation=CONSTRAINT.GREATER_THAN,
                    value1=0.0
                )
                input.addConstraint(
                    name=f'TconstrMin_{i}_{j}',
                    operation=CONSTRAINT.GREATER_THAN,
                    value1=0.0
                )

        input.addObjective("power", OBJECTIVE.MINIMIZE)

        input.setSolverSetup(
            includeLeastInfeasible=True,
            solutionQuality=1
        )

        solverAiClientCompute = \
            SolverAiClientCompute(computerUrl, token, problem_id)

        results = solverAiClientCompute.runSolver(input)

        if results.getNumberOfResults() < 1:
            raise Exception('Results not as expected.')

        print('Test was successful!!!')

    except Exception as e:
        print(f'Exception: {str(e)}')

    finally:
        solverAiClientSetup.deleteAll(
            equationIds=equation_ids,
            codeIds=code_ids,
            hardIds=hard_data_ids,
            softIds=soft_data_ids,
            problemId=problem_id
        )


if __name__ == "__main__":
    main()
    exit(0)

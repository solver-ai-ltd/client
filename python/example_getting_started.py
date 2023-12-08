from os import path

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

        id = solverAiClientSetup.postHardData(
            'Batteries_API',
            path.join(data_file_folder_path, 'getting_started/Batteries.csv')
        )
        hard_data_ids.append(id)

        id = solverAiClientSetup.postHardData(
            'Bodies_API',
            path.join(data_file_folder_path, 'getting_started/Bodies.csv')
        )
        hard_data_ids.append(id)

        id = solverAiClientSetup.postHardData(
            'Motors_API',
            path.join(data_file_folder_path, 'getting_started/Motors.csv')
        )
        hard_data_ids.append(id)

        id = solverAiClientSetup.postSoftData(
            'range_unit_energy_API',
            path.join(data_file_folder_path, 'getting_started/range_unit_energy.csv'),
            'motor_power, battery_capacity, body_weight',
            'range_unit_energy'
        )
        soft_data_ids.append(id)

        id = solverAiClientSetup.postEquation(
            'total_API',
            'total_cost = motor_price + battery_n * battery_price * efficiency_price_factor + body_price',
            'motor_price, battery_price, body_price, efficiency_price_factor, battery_n'
        )
        equation_ids.append(id)

        id = solverAiClientSetup.postEquation(
            'range_API',
            'range = motor_power * battery_n * battery_capacity * range_unit_energy',
            'motor_power, battery_capacity, range_unit_energy, battery_n'
        )
        equation_ids.append(id)

        id = solverAiClientSetup.postCode(
            'efficiency_price_factor_API',
            path.join(data_file_folder_path, 'getting_started/efficiency_price_factor.py'),
            'battery_efficiency',
            'efficiency_price_factor'
        )
        code_ids.append(id)

        problem_id = solverAiClientSetup.postProblem(
            'Test Problem_API',
            equationIds=equation_ids,
            codeIds=code_ids,
            hardIds=hard_data_ids,
            softIds=soft_data_ids
        )

        solverAiClientCompute = \
            SolverAiClientCompute(computerUrl, token, problem_id)

        problem_setup_json = solverAiClientCompute.getProblemSetup()

        input_json = {
            "id": problem_id,
            "inputs": {
                "battery_n": {
                    "Min": "-2",
                    "Max": "2",
                    "Constant": 0,
                    "Integer": 0
                }
            },
            "constraints": {
            },
            "objectives": {
                "range": {
                    "Operation": 'maximize'
                    # Operation options are:
                    # - 'minimize'
                    # - 'maximize'
                },
                "total_cost": {
                    "Operation": 'minimize'
                    # Operation options are:
                    # - 'minimize'
                    # - 'maximize'
                }
            }
        }

        results = solverAiClientCompute.runSolver(input_json)

        if 'Number Of Results' not in results \
                or results['Number Of Results'] < 1:
            raise Exception('Results not as expected.')

        # results should have value similar to
        #     {
        #         'Number Of Results': 1,
        #         'Objective Variable Names': "['T3']",
        #         'F0': '[3.1]',
        #         'Constraint Variable Names ': "[]",
        #         'G0': '[]',
        #         'Input Variable Names': "[]",
        #         'X0': '[]',
        #         'Output Variable Names':
        #             "['C1', 'T1', 'T2', 'T3', 'var1', 'var2', 'var3']",
        #         'Y0': "[4.1, 1.2, 2.2, 3.1, 'B', 'H', 'P']"
        #     }

        solverAiClientSetup.postEquation



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

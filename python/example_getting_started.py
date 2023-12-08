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
            path.join(data_file_folder_path,
                      'getting_started/range_unit_energy.csv'),
            'motor_power, battery_capacity, body_weight',
            'range_unit_energy'
        )
        soft_data_ids.append(id)

        id_total_cost = solverAiClientSetup.postEquation(
            'total_cost_API',
            'total_cost = motor_price + battery_price * efficiency_price_factor + body_price',
            'motor_price, battery_price, body_price, efficiency_price_factor'
        )
        equation_ids.append(id_total_cost)

        id_range = solverAiClientSetup.postEquation(
            'range_API',
            'range = motor_power * battery_capacity * range_unit_energy',
            'motor_power, battery_capacity, range_unit_energy'
        )
        equation_ids.append(id_range)

        id = solverAiClientSetup.postCode(
            'efficiency_price_factor_API',
            path.join(data_file_folder_path, 'getting_started/efficiency_price_factor.py'),
            'battery_efficiency',
            'efficiency_price_factor'
        )
        code_ids.append(id)

        problem_id = solverAiClientSetup.postProblem(
            'Getting Started API',
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

        solverAiClientSetup.patchEquation(
            id_total_cost,
            equationString='total_cost = motor_price + battery_num * battery_price * efficiency_price_factor + body_price',
            variablesString='motor_price, battery_price, body_price, efficiency_price_factor, battery_num'
        )

        solverAiClientSetup.patchEquation(
            id_range,
            equationString='range = motor_power * battery_num * battery_capacity * range_unit_energy',
            variablesString='motor_power, battery_capacity, range_unit_energy, battery_num'
        )

        input_json = {
            "id": problem_id,
            "inputs": {
                "battery_num": {
                    "Min": "1",
                    "Max": "3",
                    "Constant": 0,
                    "Integer": 1
                }
            },
            "constraints": {
                "range": {
                    "Operation": 'greater than',
                    "Value1": "200000",
                    "Value2": ""
                    # Operation options are:
                    # - 'smaller than': requires Value1
                    # - 'greater than': requires Value1
                    # - 'equal to': requires Value1
                    # - 'inside range': requires Value1 and Value2
                    # - 'outside range': requires Value1 and Value2
                }
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

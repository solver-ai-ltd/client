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

        inputs, outputs = solverAiClientCompute.getProblemSetup()

        input = SolverAiComputeInput(problem_id)
        input.add_objective('range', OBJECTIVE.MAXIMIZE)
        input.add_objective('total_cost', OBJECTIVE.MINIMIZE)

        results = solverAiClientCompute.runSolver(input)

        if results.get_number_of_results() < 1:
            raise Exception('Results not as expected.')

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

        input = SolverAiComputeInput(problem_id)
        input.add_input('battery_num', 1, 3, False, True)
        input.add_constraint('range', CONSTRAINT.GREATER_THAN, 200000)
        input.add_objective('range', OBJECTIVE.MAXIMIZE)
        input.add_objective('total_cost', OBJECTIVE.MINIMIZE)

        results = solverAiClientCompute.runSolver(input)

        if results.get_number_of_results() < 1:
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

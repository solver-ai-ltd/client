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
            'hard_data_basic_1',
            path.join(data_file_folder_path, 'hard_data_basic_1.csv')
        )
        hard_data_ids.append(id)

        id = solverAiClientSetup.postHardData(
            'hard_data_basic_2',
            path.join(data_file_folder_path, 'hard_data_basic_2.csv')
        )
        hard_data_ids.append(id)

        id = solverAiClientSetup.postHardData(
            'hard_data_basic_3',
            path.join(data_file_folder_path, 'hard_data_basic_3.csv')
        )
        hard_data_ids.append(id)

        problem_id = solverAiClientSetup.postProblem(
            'Test Problem',
            equationIds=equation_ids,
            codeIds=code_ids,
            hardIds=hard_data_ids,
            softIds=soft_data_ids
        )

        solverAiClientCompute = \
            SolverAiClientCompute(computerUrl, token, problem_id)

        problem_setup_json = solverAiClientCompute.getProblemSetup()

        expected_problem_setup_json = {
            'id': problem_id,
            'inputs': [],
            'outputs': ['C1', 'T1', 'T2', 'T3']
        }

        assert problem_setup_json == expected_problem_setup_json

        input_json = {
            "id": problem_id,
            "inputs": {
            },
            "constraints": {
            },
            "objectives": {
                "T3": {
                    "Operation": 'minimize'
                    # Operation options are:
                    # - 'minimize'
                    # - 'maximize'
                },
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

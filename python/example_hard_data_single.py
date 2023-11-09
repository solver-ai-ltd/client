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
            'outputs': ['C1', 'T1', 'var1', 'var2']
        }

        assert problem_setup_json == expected_problem_setup_json

        input_json = {
            "id": problem_id,
            "inputs": {
            },
            "constraints": {
            },
            "objectives": {
                "T1": {
                    "Operation": 'minimize'
                    # Operation options are:
                    # - 'minimize'
                    # - 'maximize'
                },
            }
        }

        results = solverAiClientCompute.runSolver(input_json)

        expected_results = \
            {
                'Number Of Results': 1,
                'Objective Variable Names': "['T1']",
                'F0': '[1.]',
                'Constraint Variable Names ': "[]",
                'G0': '[]',
                'Input Variable Names': "[]",
                'X0': '[]',
                'Output Variable Names': "['C1', 'T1', 'var1', 'var2']",
                'Y0': "[4.1, 1.0, 'A', 'G']"
            }

        assert results == expected_results

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

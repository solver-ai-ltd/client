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

        id = solverAiClientSetup.postCode(
            'code_basic',
            path.join(data_file_folder_path, 'code_basic.py'),
            'x1, x2',
            'y1, y2'
        )
        code_ids.append(id)

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
            'inputs': ['x1', 'x2'],
            'outputs': ['y1', 'y2']
        }

        assert problem_setup_json == expected_problem_setup_json

        input_json = {
            "id": problem_id,
            "inputs": {
                "x1": {
                    "Min": "-2",
                    "Max": "2",
                    "Constant": 0,
                    "Integer": 0
                },
                "x2": {
                    "Min": "-2",
                    "Max": "2",
                    "Constant": 0,
                    "Integer": 0
                }
            },
            "constraints": {
                "y1": {
                    "Operation": 'greater than',
                    "Value1": "1",
                    "Value2": ""
                    # Operation options are:
                    # - 'smaller than': requires Value1
                    # - 'greater than': requires Value1
                    # - 'equal to': requires Value1
                    # - 'inside range': requires Value1 and Value2
                    # - 'outside range': requires Value1 and Value2
                },
            },
            "objectives": {
                "y1": {
                    "Operation": 'minimize'
                    # Operation options are:
                    # - 'minimize'
                    # - 'maximize'
                },
                "y2": {
                    "Operation": 'maximize'
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
        #         'Objective Variable Names': "['y1', 'y2']",
        #         'F0': '[0.9999999999999996, 2.0]',
        #         'Constraint Variable Names ': "['y1']",
        #         'G0': '[0.9999999999999996]',
        #         'Input Variable Names': "['x1', 'x2']",
        #         'X0': '[0.9999999999999996, 2.0]',
        #         'Output Variable Names': "['y1', 'y2']",
        #         'Y0': '[0.9999999999999996, 2.0]'
        #     }

        solverAiClientSetup.patchCode(
            code_ids[0],
            filePath=path.join(data_file_folder_path, 'code_basic_mod.py'),
            variablesStringOut='y1_mod, y2_mod'
        )

        problem_setup_json = solverAiClientCompute.getProblemSetup()

        expected_problem_setup_json = {
            'id': problem_id,
            'inputs': ['x1', 'x2'],
            'outputs': ['y1_mod', 'y2_mod']
        }

        assert problem_setup_json == expected_problem_setup_json

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

from SolverAiClientSetup import SolverAiClientSetup
from SolverAiClientCompute import SolverAiClientCompute

from setup import token, datamanagerUrl, computerUrl


def main():
    solverAiClientSetup = \
        SolverAiClientSetup(datamanagerUrl, token)

    equation_ids = list()
    code_ids = list()
    hard_data_ids = list()
    soft_data_ids = list()
    problem_id = None

    try:

        id = solverAiClientSetup.postEquation(
            'test equation',
            'y = x',
            'x'
        )
        equation_ids.append(id)

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
            'inputs': ['x'],
            'outputs': ['y']
        }

        assert problem_setup_json == expected_problem_setup_json

        input_json = {
            "id": problem_id,
            "inputs": {
                "x": {
                    "Min": "-2",
                    "Max": "2",
                    "Constant": 0,
                    "Integer": 0
                }
            },
            "constraints": {
                "y": {
                    "Operation": 'greater than',
                    "Value1": "1",
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
                "y": {
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
        #         'Objective Variable Names':
        #         "['y']",
        #         'F0': '[0.9999999999999996]',
        #         'Constraint Variable Names ': "['y']",
        #         'G0': '[0.9999999999999996]',
        #         'Input Variable Names': "['x']",
        #         'X0': '[0.9999999999999996]',
        #         'Output Variable Names': "['y']",
        #         'Y0': '[0.9999999999999996]'
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

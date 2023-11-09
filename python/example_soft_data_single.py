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

        id = solverAiClientSetup.postSoftData(
            'soft_data_basic',
            path.join(data_file_folder_path, 'soft_data_basic.csv'),
            'in1, in2',
            'out1, out2'
        )
        soft_data_ids.append(id)

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
            'inputs': ['in1', 'in2'],
            'outputs': ['out1', 'out1_std', 'out2', 'out2_std']
        }

        assert problem_setup_json == expected_problem_setup_json

        input_json = {
            "id": problem_id,
            "inputs": {
                "in1": {
                    "Min": "0",
                    "Max": "3.141592654",
                    "Constant": 0,
                    "Integer": 0
                },
                "in2": {
                    "Min": "0",
                    "Max": "3.141592654",
                    "Constant": 0,
                    "Integer": 0
                }
            },
            "constraints": {
                "out1": {
                    "Operation": 'greater than',
                    "Value1": "0.4999999",
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
                "out1": {
                    "Operation": 'minimize'
                    # Operation options are:
                    # - 'minimize'
                    # - 'maximize'
                },
                "out2": {
                    "Operation": 'minimize'
                    # Operation options are:
                    # - 'minimize'
                    # - 'maximize'
                }
            }
        }

        results = solverAiClientCompute.runSolver(input_json)

        expected_results = \
            {
                'Number Of Results': 1,
                'Objective Variable Names': "['out1', 'out2']",
                'F0': '[ 0.4999999  -0.86585586]',
                'Constraint Variable Names ': "['out1']",
                'G0': '[0.4999999]',
                'Input Variable Names': "['in1', 'in2']",
                'X0': '[0.5215092495460986, 2.6159045950892796]',
                'Output Variable Names':
                    "['out1', 'out1_std', 'out2', 'out2_std']",
                'Y0': ('[0.4999999001579293, 0.0016245602954137266, '
                       '-0.865855861022438, 0.002297475202666934]')
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

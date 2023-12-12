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

        inputs, outputs = solverAiClientCompute.getProblemSetup()

        if inputs != ['in1', 'in2'] or \
                outputs != ['out1', 'out1_std', 'out2', 'out2_std']:
            raise \
                Exception("Problem Setup JSON does not match expected value.")

        input = SolverAiComputeInput(problem_id)
        input.addInput('in1', 0, 3.141592654, False, False)
        input.addInput('in2', 0, 3.141592654, False, False)
        input.addConstraint('out1', CONSTRAINT.GREATER_THAN, 0.4999999)
        input.addObjective('out1', OBJECTIVE.MINIMIZE)
        input.addObjective('out2', OBJECTIVE.MINIMIZE)

        results = solverAiClientCompute.runSolver(input)

        if results.getNumberOfResults() < 1:
            raise Exception('Results not as expected.')

        solverAiClientSetup.patchSoftData(
            id,
            filePath=path.join(data_file_folder_path,
                               'soft_data_basic_mod.csv')
        )

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

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

        inputs, outputs = solverAiClientCompute.getProblemSetup()

        if inputs != [] or outputs != ['C1', 'T1']:
            raise \
                Exception("Problem Setup JSON does not match expected value.")

        input = SolverAiComputeInput(problem_id)
        input.addObjective('T1', OBJECTIVE.MINIMIZE)

        results = solverAiClientCompute.runSolver(input)

        if results.getNumberOfResults() < 1:
            raise Exception('Results not as expected.')

        solverAiClientSetup.patchHardData(
            id,
            filePath=path.join(data_file_folder_path,
                               'hard_data_basic_1_mod.csv')
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

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

        inputs, outputs = solverAiClientCompute.getProblemSetup()

        if inputs != ['x1', 'x2'] or outputs != ['y1', 'y2']:
            raise \
                Exception("Problem Setup JSON does not match expected value.")

        input = SolverAiComputeInput(problem_id)
        input.addInput('x1', -2, 2, False, False)
        input.addInput('x2', -2, 2, False, False)
        input.addConstraint('y1', CONSTRAINT.GREATER_THAN, 1)
        input.addObjective('y1', OBJECTIVE.MINIMIZE)
        input.addObjective('y2', OBJECTIVE.MAXIMIZE)

        results = solverAiClientCompute.runSolver(input)

        if results.getNumberOfResults() < 1:
            raise Exception('Results not as expected.')

        solverAiClientSetup.patchCode(
            code_ids[0],
            filePath=path.join(data_file_folder_path, 'code_basic_mod.py'),
            variablesStringOut='y1_mod, y2_mod'
        )

        inputs, outputs = solverAiClientCompute.getProblemSetup()

        if inputs != ['x1', 'x2'] or outputs != ['y1_mod', 'y2_mod']:
            raise \
                Exception("Problem Setup JSON does not match expected value.")

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

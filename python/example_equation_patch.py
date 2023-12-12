from SolverAiComputeInput import SolverAiComputeInput, \
    OBJECTIVE, CONSTRAINT
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

        inputs, outputs = solverAiClientCompute.getProblemSetup()

        if inputs != ['x'] or outputs != ['y']:
            raise \
                Exception("Problem Setup JSON does not match expected value.")

        input = SolverAiComputeInput(problem_id)
        input.addInput('x', -2, 2, False, False)
        input.addConstraint('y', CONSTRAINT.GREATER_THAN, 1)
        input.addObjective('y', OBJECTIVE.MINIMIZE)

        results = solverAiClientCompute.runSolver(input)

        if results.getNumberOfResults() < 1:
            raise Exception('Results not as expected.')

        solverAiClientSetup.patchEquation(
            equation_ids[0],
            equationString='y1 = x1',
            variablesString='x1'
        )

        inputs, outputs = solverAiClientCompute.getProblemSetup()

        if inputs != ['x1'] or outputs != ['y1']:
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

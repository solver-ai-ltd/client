const {SolverAiComputeInput, CONSTRAINT, OBJECTIVE} = require('./SolverAiComputeInput');
const SolverAiClientSetup = require('./SolverAiClientSetup');
const SolverAiClientCompute = require('./SolverAiClientCompute');

const config = require('./setup.js');


const solverAiClientSetup = 
    new SolverAiClientSetup(config.datamanagerUrl, config.token);

async function main() {
    var exitCode = 1;

    const equation_ids = [];
    const code_ids = [];
    const hard_data_ids = [];
    const soft_data_ids = [];
    let problem_id = null;

    try {

        const id = await solverAiClientSetup.postEquation(
            'test equation',
            'y = x',
            'x'
        );
        equation_ids.push(id);

        problem_id = await solverAiClientSetup.postProblem(
            'Test Problem',
            equation_ids,
            code_ids,
            hard_data_ids,
            soft_data_ids
        );

        const solverAiClientCompute = new SolverAiClientCompute(config.computerUrl, config.token, problem_id);

        let [inputs, outputs] = await solverAiClientCompute.getProblemSetup();

        if (JSON.stringify(inputs) !== JSON.stringify(['x']) || 
            JSON.stringify(outputs) !== JSON.stringify(['y']))
        {
            throw new Error('Problem Setup JSON does not match expected value.');
        }

        let input = new SolverAiComputeInput(problem_id);
        input.addInput("x", -2, 2, false, false);
        input.addConstraint("y", CONSTRAINT.GREATER_THAN, 1);
        input.addObjective("y", OBJECTIVE.MINIMIZE);

        const results = await solverAiClientCompute.runSolver(input);

        if (results.getNumberOfResults() < 1) {
            throw new Error('Results not as expected.');
        }

        await solverAiClientSetup.patchEquation(
            equation_ids[0],
            undefined,
            'y1 = x1',
            'x1',
            undefined
        );

        [inputs, outputs] = await solverAiClientCompute.getProblemSetup();

        if (JSON.stringify(inputs) !== JSON.stringify(['x1']) || 
            JSON.stringify(outputs) !== JSON.stringify(['y1']))
        {
            throw new Error('Problem Setup JSON does not match expected value.');
        }

        console.log('Test was successful!!!');

        exitCode = 0;
    } catch (error) {
        console.error(`Exception: ${error.message}`);
    } finally {
        await solverAiClientSetup.deleteAll(
            equation_ids,
            code_ids,
            hard_data_ids,
            soft_data_ids,
            problem_id
        );
    }

    return exitCode;
}

(async () => {
    const exitCode = await main();
    process.exitCode = exitCode;
})();

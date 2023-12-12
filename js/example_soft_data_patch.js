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

        const id = await solverAiClientSetup.postSoftData(
            'soft_data_basic',
            `${config.data_file_folder_path}/soft_data_basic.csv`,
            'in1, in2',
            'out1, out2'
            )
        soft_data_ids.push(id);

        problem_id = await solverAiClientSetup.postProblem(
            'Test Problem',
            equation_ids,
            code_ids,
            hard_data_ids,
            soft_data_ids
        );

        const solverAiClientCompute = new SolverAiClientCompute(config.computerUrl, config.token, problem_id);

        let [inputs, outputs] = await solverAiClientCompute.getProblemSetup();

        if (JSON.stringify(inputs) !== JSON.stringify(['in1', 'in2']) || 
            JSON.stringify(outputs) !== JSON.stringify(['out1', 'out1_std', 'out2', 'out2_std']))
        {
            throw new Error('Problem Setup JSON does not match expected value.');
        }

        let input = new SolverAiComputeInput(problem_id);
        input.addInput("in1", 0, 3.141592654, false, false);
        input.addInput("in2", 0, 3.141592654, false, false);
        input.addConstraint("out1", CONSTRAINT.GREATER_THAN, 0.4999999);
        input.addObjective("out1", OBJECTIVE.MINIMIZE);
        input.addObjective("out2", OBJECTIVE.MINIMIZE);

        let results = await solverAiClientCompute.runSolver(input);

        if (results.getNumberOfResults() < 1) {
            throw new Error('Results not as expected.');
        }
        
        await solverAiClientSetup.patchSoftData(
            id,
            undefined,
            `${config.data_file_folder_path}/soft_data_basic_mod.csv`,
            undefined,
            undefined,
            undefined
        )

        results = await solverAiClientCompute.runSolver(input);

        if (results.getNumberOfResults() < 1) {
            throw new Error('Results not as expected.');
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

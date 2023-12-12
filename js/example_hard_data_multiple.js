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

        let id = await solverAiClientSetup.postHardData(
            'hard_data_basic_1',
            `${config.data_file_folder_path}/hard_data_basic_1.csv`
        )
        hard_data_ids.push(id);

        id = await solverAiClientSetup.postHardData(
            'hard_data_basic_2',
            `${config.data_file_folder_path}/hard_data_basic_2.csv`
        )
        hard_data_ids.push(id);

        id = await solverAiClientSetup.postHardData(
            'hard_data_basic_3',
            `${config.data_file_folder_path}/hard_data_basic_3.csv`
        )
        hard_data_ids.push(id);

        problem_id = await solverAiClientSetup.postProblem(
            'Test Problem',
            equation_ids,
            code_ids,
            hard_data_ids,
            soft_data_ids
        );

        const solverAiClientCompute = new SolverAiClientCompute(config.computerUrl, config.token, problem_id);

        let [inputs, outputs] = await solverAiClientCompute.getProblemSetup();

        if (JSON.stringify(inputs) !== JSON.stringify([]) || 
            JSON.stringify(outputs) !== JSON.stringify(['C1', 'T1', 'T2', 'T3']))
        {
            throw new Error('Problem Setup JSON does not match expected value.');
        }

        let input = new SolverAiComputeInput(problem_id);
        input.addObjective("T3", OBJECTIVE.MINIMIZE);

        const results = await solverAiClientCompute.runSolver(input);

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

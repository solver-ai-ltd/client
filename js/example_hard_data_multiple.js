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

        const problemSetupJson = await solverAiClientCompute.getProblemSetup();

        const expectedProblemSetupJson = {
            id: problem_id,
            inputs: [],
            outputs: ['C1', 'T1', 'T2', 'T3', 'var1', 'var2', 'var3']
        };

        if (JSON.stringify(problemSetupJson) !== JSON.stringify(expectedProblemSetupJson)) {
            throw new Error('Problem Setup JSON does not match expected value.');
        }

        const inputJson = {
            id: problem_id,
            inputs: {
            },
            constraints: {
            },
            objectives: {
                T3: {
                    Operation: 'minimize'
                    // Operation options are:
                    // - 'minimize'
                    // - 'maximize'
                },
            }
        };

        const results = await solverAiClientCompute.runSolver(inputJson);

        const expectedResults = {
            'Number Of Results': 1,
            'Objective Variable Names': "['T3']",
            'F0': '[3.1]',
            'Constraint Variable Names ': "[]",
            'G0': '[]',
            'Input Variable Names': "[]",
            'X0': '[]',
            'Output Variable Names':
                "['C1', 'T1', 'T2', 'T3', 'var1', 'var2', 'var3']",
            'Y0': "[4.1, 1.2, 2.2, 3.1, 'B', 'H', 'P']"
        };

        if (JSON.stringify(results) !== JSON.stringify(expectedResults)) {
            throw new Error('Results do not match expected value.');
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

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

        const id = await solverAiClientSetup.postCode(
            'test code',
            `${config.data_file_folder_path}/code_basic.py`,
            'x1, x2',
            'y1, y2'
        )
        code_ids.push(id);

        problem_id = await solverAiClientSetup.postProblem(
            'Test Problem',
            equation_ids,
            code_ids,
            hard_data_ids,
            soft_data_ids
        );

        const solverAiClientCompute = new SolverAiClientCompute(config.computerUrl, config.token, problem_id);

        let problemSetupJson = await solverAiClientCompute.getProblemSetup();

        let expectedProblemSetupJson = {
            id: problem_id,
            inputs: ['x1', 'x2'],
            outputs: ['y1', 'y2']
        };

        if (JSON.stringify(problemSetupJson) !== JSON.stringify(expectedProblemSetupJson)) {
            throw new Error('Problem Setup JSON does not match expected value.');
        }

        const inputJson = {
            id: problem_id,
            inputs: {
                x1: {
                    Min: "-2",
                    Max: "2",
                    Constant: 0,
                    Integer: 0
                },
                x2: {
                    Min: "-2",
                    Max: "2",
                    Constant: 0,
                    Integer: 0
                }
            },
            constraints: {
                y1: {
                    Operation: 'greater than',
                    Value1: "1",
                    Value2: ""
                    // Operation options are:
                    // - 'smaller than': requires Value1
                    // - 'greater than': requires Value1
                    // - 'equal to': requires Value1
                    // - 'inside range': requires Value1 and Value2
                    // - 'outside range': requires Value1 and Value2
                },
            },
            objectives: {
                y1: {
                    Operation: 'minimize'
                    // Operation options are:
                    // - 'minimize'
                    // - 'maximize'
                },
                y2: {
                    Operation: 'maximize'
                    // Operation options are:
                    // - 'minimize'
                    // - 'maximize'
                },
            }
        };

        const results = await solverAiClientCompute.runSolver(inputJson);

        if (!results.hasOwnProperty('Number Of Results') || results['Number Of Results'] < 1) {
            throw new Error('Results not as expected.');
        }
        
        // results should have value similar to
        // {
        //     'Number Of Results': 1,
        //     'Objective Variable Names': "['y1', 'y2']",
        //     'F0': '[1.0000000000000004, 2.0]',
        //     'Constraint Variable Names ': "['y1']",
        //     'G0': '[1.0000000000000004]',
        //     'Input Variable Names': "['x1', 'x2']",
        //     'X0': '[1.0000000000000004, 2.0]',
        //     'Output Variable Names': "['y1', 'y2']",
        //     'Y0': '[1.0000000000000004, 2.0]'
        // };

        await solverAiClientSetup.patchCode(
            code_ids[0],
            undefined,
            `${config.data_file_folder_path}/code_basic_mod.py`,
            undefined,
            'y1_mod, y2_mod'
        )

        problemSetupJson = await solverAiClientCompute.getProblemSetup();

        expectedProblemSetupJson = {
            id: problem_id,
            inputs: ['x1', 'x2'],
            outputs: ['y1_mod', 'y2_mod']
        };

        if (JSON.stringify(problemSetupJson) !== JSON.stringify(expectedProblemSetupJson)) {
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

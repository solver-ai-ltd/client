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

        const problemSetupJson = await solverAiClientCompute.getProblemSetup();

        const expectedProblemSetupJson = {
            id: problem_id,
            inputs: ['in1', 'in2'],
            outputs: ['out1', 'out1_std', 'out2', 'out2_std']
        };

        if (JSON.stringify(problemSetupJson) !== JSON.stringify(expectedProblemSetupJson)) {
            throw new Error('Problem Setup JSON does not match expected value.');
        }

        const inputJson = {
            id: problem_id,
            inputs: {
                in1: {
                    Min: "0",
                    Max: "3.141592654",
                    Constant: 0,
                    Integer: 0
                },
                in2: {
                    Min: "0",
                    Max: "3.141592654",
                    Constant: 0,
                    Integer: 0
                }
            },
            constraints: {
                out1: {
                    Operation: 'greater than',
                    Value1: "0.4999999",
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
                out1: {
                    Operation: 'minimize'
                    // Operation options are:
                    // - 'minimize'
                    // - 'maximize'
                },
                out2: {
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
            'Objective Variable Names': "['out1', 'out2']",
            'F0': '[ 0.4999999  -0.86585586]',
            'Constraint Variable Names ': "['out1']",
            'G0': '[0.4999999]',
            'Input Variable Names': "['in1', 'in2']",
            'X0': '[0.5215092495460986, 2.6159045950892796]',
            'Output Variable Names':
                "['out1', 'out1_std', 'out2', 'out2_std']",
            'Y0': '[0.4999999001579293, 0.0016245602954137266, -0.865855861022438, 0.002297475202666934]'
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

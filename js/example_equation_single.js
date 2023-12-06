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

        const problemSetupJson = await solverAiClientCompute.getProblemSetup();

        const expectedProblemSetupJson = {
            id: problem_id,
            inputs: ['x'],
            outputs: ['y']
        };

        if (JSON.stringify(problemSetupJson) !== JSON.stringify(expectedProblemSetupJson)) {
            throw new Error('Problem Setup JSON does not match expected value.');
        }

        const inputJson = {
            id: problem_id,
            inputs: {
                x: {
                    Min: "-2",
                    Max: "2",
                    Constant: 0,
                    Integer: 0
                }
            },
            constraints: {
                y: {
                    Operation: 'greater than',
                    Value1: "1",
                    Value2: ""
                },
            },
            objectives: {
                y: {
                    Operation: 'minimize'
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
        //     'Objective Variable Names': "['y']",
        //     'F0': '[1.0000000000200555]',
        //     'Constraint Variable Names ': "['y']",
        //     'G0': '[1.0000000000200555]',
        //     'Input Variable Names': "['x']",
        //     'X0': '[1.0000000000200555]',
        //     'Output Variable Names': "['y']",
        //     'Y0': '[1.0000000000200555]'
        // };

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

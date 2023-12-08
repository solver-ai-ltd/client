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
            'Batteries_API',
            `${config.data_file_folder_path}/getting_started/Batteries.csv`
        )
        hard_data_ids.push(id);

        id = await solverAiClientSetup.postHardData(
            'Bodies_API',
            `${config.data_file_folder_path}/getting_started/Bodies.csv`
        )
        hard_data_ids.push(id);

        id = await solverAiClientSetup.postHardData(
            'Motors_API',
            `${config.data_file_folder_path}/getting_started/Motors.csv`
        )
        hard_data_ids.push(id);

        id = await solverAiClientSetup.postSoftData(
            'range_unit_energy_API',
            `${config.data_file_folder_path}/getting_started/range_unit_energy.csv`,
            'motor_power, battery_capacity, body_weight',
            'range_unit_energy'
            )
        soft_data_ids.push(id);

        let id_total_cost = await solverAiClientSetup.postEquation(
            'total_cost_API',
            'total_cost = motor_price + battery_price * efficiency_price_factor + body_price',
            'motor_price, battery_price, body_price, efficiency_price_factor'
        );
        equation_ids.push(id_total_cost);

        let id_range = await solverAiClientSetup.postEquation(
            'range_API',
            'range = motor_power * battery_capacity * range_unit_energy',
            'motor_power, battery_capacity, range_unit_energy'
        );
        equation_ids.push(id_range);

        id = await solverAiClientSetup.postCode(
            'efficiency_price_factor_API',
            `${config.data_file_folder_path}/getting_started/efficiency_price_factor.py`,
            'battery_efficiency',
            'efficiency_price_factor'
        )
        code_ids.push(id);

        problem_id = await solverAiClientSetup.postProblem(
            'Getting Started API',
            equation_ids,
            code_ids,
            hard_data_ids,
            soft_data_ids
        );

        const solverAiClientCompute = new SolverAiClientCompute(config.computerUrl, config.token, problem_id);

        let problemSetupJson = await solverAiClientCompute.getProblemSetup();

        let inputJson = {
            id: problem_id,
            inputs: {
            },
            constraints: {
            },
            objectives: {
                range: {
                    Operation: 'maximize'
                    // Operation options are:
                    // - 'minimize'
                    // - 'maximize'
                },
                total_cost: {
                    Operation: 'minimize'
                    // Operation options are:
                    // - 'minimize'
                    // - 'maximize'
                }
            }
        };

        let results = await solverAiClientCompute.runSolver(inputJson);

        if (!results.hasOwnProperty('Number Of Results') || results['Number Of Results'] < 1) {
            throw new Error('Results not as expected.');
        }

        // results should have value similar to
        // {
        //     'Number Of Results': 1,
        //     'Objective Variable Names': "['T1']",
        //     'F0': '[1.0]',
        //     'Constraint Variable Names ': "[]",
        //     'G0': '[]',
        //     'Input Variable Names': "[]",
        //     'X0': '[]',
        //     'Output Variable Names': "['C1', 'T1', 'var1', 'var2']",
        //     'Y0': "[4.1, 1.0, 'A', 'G']"
        // };

        solverAiClientSetup.patchEquation(
            id_total_cost,
            undefined,
            'total_cost = motor_price + battery_num * battery_price * efficiency_price_factor + body_price',
            'motor_price, battery_price, body_price, efficiency_price_factor, battery_num',
            undefined
        )   

        solverAiClientSetup.patchEquation(
            id_range,
            undefined,
            'range = motor_power * battery_num * battery_capacity * range_unit_energy',
            'motor_power, battery_capacity, range_unit_energy, battery_num',
            undefined
        )   

        inputJson = {
            id: problem_id,
            inputs: {
                battery_num: {
                    Min: "1",
                    Max: "3",
                    Constant: 0,
                    Integer: 1
                }
            },
            constraints: {
                range: {
                    Operation: 'greater than',
                    Value1: "200000",
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
                range: {
                    Operation: 'maximize'
                    // Operation options are:
                    // - 'minimize'
                    // - 'maximize'
                },
                total_cost: {
                    Operation: 'minimize'
                    // Operation options are:
                    // - 'minimize'
                    // - 'maximize'
                }
            }
        };

        results = await solverAiClientCompute.runSolver(inputJson);

        if (!results.hasOwnProperty('Number Of Results') || results['Number Of Results'] < 1) {
            throw new Error('Results not as expected.');
        }
        
        // results should have value similar to
        // {
        //     'Number Of Results': 1,
        //     'Objective Variable Names': "['T1']",
        //     'F0': '[1.2]',
        //     'Constraint Variable Names ': "[]",
        //     'G0': '[]',
        //     'Input Variable Names': "[]",
        //     'X0': '[]',
        //     'Output Variable Names': "['C1', 'T1', 'var1', 'var2']",
        //     'Y0': "[4.1, 1.2, 'B', 'H']"
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

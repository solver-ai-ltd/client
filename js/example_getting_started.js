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

        let [inputs, outputs] = await solverAiClientCompute.getProblemSetup();

        let input = new SolverAiComputeInput(problem_id);
        input.addObjective("range", OBJECTIVE.MAXIMIZE);
        input.addObjective("total_cost", OBJECTIVE.MINIMIZE);

        let results = await solverAiClientCompute.runSolver(input);

        if (results.getNumberOfResults() < 1) {
            throw new Error('Results not as expected.');
        }

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

        let newInput = new SolverAiComputeInput(problem_id);
        newInput.addInput("battery_num", 1, 3, false, true);
        newInput.addConstraint("range", CONSTRAINT.GREATER_THAN, 200000);
        newInput.addObjective("range", OBJECTIVE.MAXIMIZE);
        newInput.addObjective("total_cost", OBJECTIVE.MINIMIZE);

        results = await solverAiClientCompute.runSolver(newInput);

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

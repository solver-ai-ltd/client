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
        const numRooms = 8;
        const numForecastIntervals = 4;

        let id = 0;

        for (let j = 1; j < numForecastIntervals; j++) {
            id = await solverAiClientSetup.postEquation(
                `th_${j}`,
                `th_${j} = (th_0 + ${j} * Dth)%24`,
                'th_0, Dth'
            );
            equation_ids.push(id);
        
            id = await solverAiClientSetup.postEquation(
                `weekday_${j}`,
                `weekday_${j} = (weekday_0 + np.floor((th_0 + ${j} * Dth)/24))%7`,
                'th_0, Dth, weekday_0'
            );
            equation_ids.push(id);
        }
        
        for (let j = 0; j < numForecastIntervals - 1; j++) {
            id = await solverAiClientSetup.postEquation(
                `Te_${j}`,
                `Te_${j} = Te`,
                'Te'
            );
            equation_ids.push(id);
        }
        
        id = await solverAiClientSetup.postSoftData(
            'Motion Sensor Data',
            `${config.data_file_folder_path}/thermostat/motionSensorData.csv`,
            'weekday, th',
            'motion_0,motion_1,motion_2,motion_3,motion_4,motion_5,motion_6,motion_7',
            `2-${numForecastIntervals - 1}`
        );
        soft_data_ids.push(id);
        
        id = await solverAiClientSetup.postSoftData(
            'Temperature Sensor Data',
            `${config.data_file_folder_path}/thermostat/temperatureSensorData.csv`,
            'C_0,C_1,C_2,C_3,C_4,C_5,C_6,C_7,T_0,T_1,T_2,T_3,T_4,T_5,T_6,T_7,Te',
            'DTh_0,DTh_1,DTh_2,DTh_3,DTh_4,DTh_5,DTh_6,DTh_7,power',
            `0-${numForecastIntervals - 2}`
        );
        soft_data_ids.push(id);
        
        for (let i = 0; i < numRooms; i++) {
            for (let j = 1; j < numForecastIntervals; j++) {
                id = await solverAiClientSetup.postEquation(
                    `T_${i}_${j}`,
                    `T_${i}_${j} = T_${i}_${j - 1} + DTh_${i}_${j - 1}*Dth`,
                    `T_${i}_${j - 1}, DTh_${i}_${j - 1}, Dth`
                );
                equation_ids.push(id);
            }
        }
        
        for (let i = 0; i < numRooms; i++) {
            for (let j = 2; j < numForecastIntervals; j++) {
                id = await solverAiClientSetup.postEquation(
                    `Tconstr_${i}_${j}`,
                    `Tconstr_${i}_${j} = (T_${i}_${j} - requiredT_${i}) if motion_${i}_${j} > 0.4 else 999`,
                    `T_${i}_${j}, motion_${i}_${j}, requiredT_${i}`
                );
                equation_ids.push(id);
        
                id = await solverAiClientSetup.postEquation(
                    `TconstrMin_${i}_${j}`,
                    `TconstrMin_${i}_${j} = T_${i}_${j} - Tmin_${i}`,
                    `T_${i}_${j}, Tmin_${i}`
                );
                equation_ids.push(id);
            }
        }
        
        let powerEquation = 'power = power_1';
        let powerVarNames = 'power_1';
        for (let j = 2; j < numForecastIntervals - 1; j++) {
            powerEquation += `+power_${j}`;
            powerVarNames += `,power_${j}`;
        }
        id = await solverAiClientSetup.postEquation(
            'power',
            powerEquation,
            powerVarNames
        );
        equation_ids.push(id);
        
        problem_id = await solverAiClientSetup.postProblem(
            'Thermostat API',
            equation_ids,
            code_ids,
            hard_data_ids,
            soft_data_ids
        );

        let input = new SolverAiComputeInput(problem_id);

        // Current weekday and time at which we are performing the calculation
        input.addInput('weekday_0', 0, 0, true, false);
        input.addInput('th_0', 7.0, 0, true, false);

        // Current external temperature
        input.addInput('Te', 5, 0, true, false);

        // Current status of the radiators and temperature in each room
        for (let i = 0; i < numRooms; i++) {
            input.addInput(`C_${i}_0`, 0, 0, true, true);
            input.addInput(`T_${i}_0`, 25, 0, true, false);
        }

        // Required temperature when motion is detected and minimum temperature
        // when no motion is detected
        for (let i = 0; i < numRooms; i++) {
            input.addInput(`requiredT_${i}`, 21, 0, true, false);
            input.addInput(`Tmin_${i}`, 15, 0, true, false);
        }

        // Time interval to be considered between predictions
        input.addInput('Dth', 1/6, 0, true, false);

        // Setting up the ranges allowed for the radiators 0 is off 1 is on.
        for (let i = 0; i < numRooms; i++) {
            for (let j = 1; j < numForecastIntervals - 1; j++) {
                input.addInput(`C_${i}_${j}`, 0, 1, false, true);
            }
        }

        // Setting up of constraints requiring that for at future times
        // the temperature is greater than the minimum or greater than
        // the temperature when motion is detected
        for (let i = 0; i < numRooms; i++) {
            for (let j = 2; j < numForecastIntervals; j++) {
                input.addConstraint(`Tconstr_${i}_${j}`, CONSTRAINT.GREATER_THAN, 0.0);
                input.addConstraint(`TconstrMin_${i}_${j}`, CONSTRAINT.GREATER_THAN, 0.0);
            }
        }

        input.addObjective("power", OBJECTIVE.MINIMIZE);

        input.setSolverSetup(true, 1);

        const solverAiClientCompute = new SolverAiClientCompute(config.computerUrl, config.token, problem_id);

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
    
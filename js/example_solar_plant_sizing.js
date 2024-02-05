const {SolverAiComputeInput, CONSTRAINT, OBJECTIVE} = require('./SolverAiComputeInput');
const SolverAiClientSetup = require('./SolverAiClientSetup');
const SolverAiClientCompute = require('./SolverAiClientCompute');
const SolverAiResultsWriter = require('./SolverAiResultsWriter');

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

        // ==== Solar panels ====

        // Table ouitputs:
        //  * sp_area_m2 (panel area)
        //  * sp_w (rated max power)
        //  * sp_prc (solar pannel price)
        //  * sp_eff_perc

        id = await solverAiClientSetup.postEquation(
            'SP total area',
            'sp_area_m2_tot = sp_area_m2 * sp_n_tot',
            'sp_area_m2, sp_n_tot'
        );
        equation_ids.push(id);
        id = await solverAiClientSetup.postEquation(
            'SP W total',
            'sp_w_tot = sp_n_tot * sp_w',
            'sp_n_tot, sp_w'
        );
        equation_ids.push(id);
        id = await solverAiClientSetup.postEquation(
            'SP W total with losses',
            'sp_w_tot_wloss = sp_w_tot * (1 - sp_eff_perc / 100)',
            'sp_w_tot, sp_eff_perc'
        );
        equation_ids.push(id);
        // Equations day:
        id = await solverAiClientSetup.postEquation(
            'SP W during the day',
            'sp_w_day = sp_w_tot_wloss',
            'sp_w_tot_wloss'
        );
        equation_ids.push(id);
        // Eqauations price:
        id = await solverAiClientSetup.postEquation(
            'SP total price',
            'sp_prc_tot = sp_prc * sp_n_tot',
            'sp_prc, sp_n_tot'
        );
        equation_ids.push(id);

        // ==== Solar inverter ====

        // Table outputs:
        //  * si_kw (power)
        //  * si_prc (inverter cost)
        //  * si_eff_prc

        // Equations:
        id = await solverAiClientSetup.postEquation(
            'SI W total',
            'si_w_tot = 1000 * si_kw * si_n_tot',
            'si_kw, si_n_tot'
        );
        equation_ids.push(id);
        id = await solverAiClientSetup.postEquation(
            'SI W with loss',
            'si_w_wloss = sp_w_day * (si_eff_perc /100)',
            'sp_w_day, si_eff_perc'
        );
        equation_ids.push(id);
        // Equations day:
        id = await solverAiClientSetup.postEquation(
            'SI W during the day',
            'si_w_day = si_w_wloss',
            'si_w_wloss'
        );
        equation_ids.push(id);
        // Eqauations price:
        id = await solverAiClientSetup.postEquation(
            'SI total price',
            'si_prc_tot = si_prc * si_n_tot',
            'si_prc, si_n_tot'
        );
        equation_ids.push(id);

        // ==== Battery ====

        // Table outputs:
        //  * b_kwh (power)
        //  * b_prc (battery cost)
        //  * b_v

        // Equations:
        id = await solverAiClientSetup.postEquation(
            'B V total',
            'b_v_tot = ic_battery_input_v',
            'ic_battery_input_v'
        )
        equation_ids.push(id);
        id = await solverAiClientSetup.postEquation(
            'B number of batteries in series',
            'b_ser_n  = b_v_tot / b_v',
            'b_v_tot, b_v'
        )
        equation_ids.push(id);
        id = await solverAiClientSetup.postEquation(
            'B number of batteries in parallel',
            'b_n_tot = b_par_n * b_ser_n',
            'b_par_n, b_ser_n'
        )
        equation_ids.push(id);
        id = await solverAiClientSetup.postEquation(
            'B Wh total',
            'b_wh_tot = 1000 * b_kwh * b_n_tot',
            'b_kwh, b_n_tot'
        )
        equation_ids.push(id);
        id = await solverAiClientSetup.postEquation(
            'B Wh total with losses',
            'b_wh_tot_wlos = b_wh_tot * (b_eff_perc_loss / 100)',
            'b_wh_tot, b_eff_perc_loss'
        )
        equation_ids.push(id);
        // Equations day:
        id = await solverAiClientSetup.postEquation(
            'B W during the day',
            'b_w_day = -b_wh_tot / day_h',
            'b_wh_tot, day_h'
        )
        equation_ids.push(id);
        // Eqauations night:
        id = await solverAiClientSetup.postEquation(
            'B W during the night',
            'b_w_ngt = b_wh_tot_wlos / ngt_h',
            'b_wh_tot_wlos, ngt_h'
        )
        equation_ids.push(id);
        // Eqauations price:
        id = await solverAiClientSetup.postEquation(
            'B total price',
            'b_prc_tot = b_prc * b_n_tot',
            'b_prc, b_n_tot'
        )
        equation_ids.push(id);

        // ==== Inverter charger ====

        // Table outputs:
        //  * ic_w
        //  * ic_prc
        //  * ic_n_phss
        //  * ic_battery_input_v
        //  * ic_eff_perc_loss

        // Equations:
        id = await solverAiClientSetup.postEquation(
            'IC total number',
            'ic_n_tot = ic_n_tot_0 * ic_n_phss',
            'ic_n_tot_0, ic_n_phss'
        );
        equation_ids.push(id);
        id = await solverAiClientSetup.postEquation(
            'IC W total',
            'ic_w_tot = ic_w * ic_n_tot',
            'ic_w, ic_n_tot'
        );
        equation_ids.push(id);
        // Equations day:
        id = await solverAiClientSetup.postEquation(
            'IC W 1 during the day',
            'ic_w1_day  = -ic_w2_day',
            'ic_w2_day'
        );
        equation_ids.push(id);
        id = await solverAiClientSetup.postEquation(
            'IC W 2 during the day',
            'ic_w2_day  = ic_w2_day_wloss / (ic_eff_perc_loss/100)',
            'ic_w2_day_wloss, ic_eff_perc_loss'
        );
        equation_ids.push(id);
        // Eqauations night:
        id = await solverAiClientSetup.postEquation(
            'IC W 1 during the night',
            'ic_w1_ngt  = -ic_w2_ngt_wloss',
            'ic_w2_ngt_wloss'
        );
        equation_ids.push(id);
        id = await solverAiClientSetup.postEquation(
            'IC W 2 during the night with loss',
            'ic_w2_ngt_wloss = ic_w2_ngt * (ic_eff_perc_loss/100)',
            'ic_w2_ngt, ic_eff_perc_loss'
        );
        equation_ids.push(id);
        // Eqauations price:
        id = await solverAiClientSetup.postEquation(
            'IC total price',
            'ic_prc_tot = ic_prc * ic_n_tot_0',
            'ic_prc, ic_n_tot_0'
        );
        equation_ids.push(id);

        // ==== Home ====

        // Equations:
        // Equations day:
        id = await solverAiClientSetup.postEquation(
            'Home W during the day',
            'h_wh_day = (si_w_day + ic_w1_day) * day_h',
            'si_w_day, ic_w1_day, day_h'
        );
        equation_ids.push(id);
        // Eqauations night:
        id = await solverAiClientSetup.postEquation(
            'Home W during the night',
            'h_wh_ngt = ic_w1_ngt * ngt_h',
            'ic_w1_ngt, ngt_h'
        );
        equation_ids.push(id);

        // ==== System equations ====

        // Equations:

        id = await solverAiClientSetup.postEquation(
            'W to battery',
            'ic_w2_day_wloss = -b_w_day',
            'b_w_day'
        );
        equation_ids.push(id);
        id = await solverAiClientSetup.postEquation(
            'W from battery',
            'ic_w2_ngt = -b_w_ngt',
            'b_w_ngt'
        );
        equation_ids.push(id);
        id = await solverAiClientSetup.postEquation(
            'Difference between SP W and SI supported W',
            'delta_w_sp_si = sp_w_day - si_w_tot',
            'sp_w_day, si_w_tot'
        );
        equation_ids.push(id);
        id = await solverAiClientSetup.postEquation(
            'Difference in percentage between SP W and SI supported W',
            'delta_w_sp_si_max_perc = 100 * (sp_w_day - si_w_tot) / si_w_tot',
            'sp_w_day, si_w_tot'
        );
        equation_ids.push(id);
        id = await solverAiClientSetup.postEquation(
            'Difference between IC W 1 during the day and IC W total supported',
            'delta_w_ic_max_day = np.abs(ic_w1_day) - ic_w_tot',
            'ic_w1_day, ic_w_tot'
        );
        equation_ids.push(id);
        id = await solverAiClientSetup.postEquation(
            'Difference between IC W 2 during the night and IC W total supported',
            'delta_w_ic_max_ngt = np.abs(ic_w1_ngt) - ic_w_tot',
            'ic_w1_ngt, ic_w_tot'
        );
        equation_ids.push(id);
        id = await solverAiClientSetup.postEquation(
            'Difference mawimum battery expansion in Wh',
            'delta_wh_max_expansion = 1000 * b_max_expansion_kwh - b_wh_tot',
            'b_max_expansion_kwh, b_wh_tot'
        );
        equation_ids.push(id);
        id = await solverAiClientSetup.postEquation(
            'Total cost of the solar plant',
            'tot_prc = sp_prc_tot + si_prc_tot + ic_prc_tot + b_prc_tot',
            'sp_prc_tot, si_prc_tot, ic_prc_tot, b_prc_tot'
        );
        equation_ids.push(id);

        // ==== Component Inventory ====

        id = await solverAiClientSetup.postHardData(
            'SP list',
            `${config.data_file_folder_path}/solar_plant_sizing/SP.csv`
        );
        hard_data_ids.push(id);
        id = await solverAiClientSetup.postHardData(
            'SI list',
            `${config.data_file_folder_path}/solar_plant_sizing/SI.csv`
        );
        hard_data_ids.push(id);
        id = await solverAiClientSetup.postHardData(
            'IC list',
            `${config.data_file_folder_path}/solar_plant_sizing/IC.csv`
        );
        hard_data_ids.push(id);
        id = await solverAiClientSetup.postHardData(
            'B list',
            `${config.data_file_folder_path}/solar_plant_sizing/B.csv`
        );
        hard_data_ids.push(id);

        // Problem setup

        problem_id = await solverAiClientSetup.postProblem(
            'Test Problem',
            equation_ids,
            code_ids,
            hard_data_ids,
            soft_data_ids
        );

        const solverAiClientCompute = new SolverAiClientCompute(config.computerUrl, config.token, problem_id);

        let [inputs, outputs] = await solverAiClientCompute.getProblemSetup();

        let input = new SolverAiComputeInput(problem_id);

        // ############################
        // Constant parameters

        // Number of hours
        input.addInput('day_h', 14, 14, true, false)
        input.addInput('ngt_h', 10, 10, true, false)

        // ############################
        // Parameters with ranges

        // Solar Pannels
        input.addInput('sp_n_tot', 1, 50, false, true)

        // Solar Inverter
        input.addInput('si_n_tot', 1, 10, false, true)

        // Inverter Charger
        input.addInput('ic_n_tot_0', 1, 10, false, true)

        // Battery
        input.addInput('b_par_n', 1, 10, false, true)

        // Constraints
        input.addConstraint('sp_area_m2_tot', CONSTRAINT.SMALLER_THAN, 100)
        input.addConstraint('delta_w_sp_si', CONSTRAINT.GREATER_THAN, 0)
        input.addConstraint('delta_w_sp_si_max_perc', CONSTRAINT.SMALLER_THAN, 33)
        input.addConstraint('delta_w_ic_max_day', CONSTRAINT.SMALLER_THAN, 0)
        input.addConstraint('delta_w_ic_max_ngt', CONSTRAINT.SMALLER_THAN, 0)
        input.addConstraint('delta_wh_max_expansion', CONSTRAINT.GREATER_THAN, -.001)

        input.addConstraint('h_wh_day', CONSTRAINT.GREATER_THAN, 5000)
        input.addConstraint('h_wh_ngt', CONSTRAINT.GREATER_THAN, 3000)

        // Objectives
        input.addObjective('tot_prc', OBJECTIVE.MINIMIZE)

        // Solver Setup
        includeLeastInfeasible = false
        solutionQuality = 10
        input.setSolverSetup(includeLeastInfeasible, solutionQuality)

        // Run Solver
        const results = await solverAiClientCompute.runSolver(input);

        // Write results to CSV
        const solverAiResultsWriter = new SolverAiResultsWriter(results);
        const solutionPath = `${config.data_file_folder_path}/results/solar_plant_sizing_js.csv`
        solverAiResultsWriter.write(solutionPath)

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

from os import path

from SolverAiComputeInput import SolverAiComputeInput, \
    OBJECTIVE, CONSTRAINT
from SolverAiClientSetup import SolverAiClientSetup
from SolverAiClientCompute import SolverAiClientCompute
from SolverAiResultsWriter import SolverAiResultsWriter

from setup import token, datamanagerUrl, computerUrl, \
    data_file_folder_path


def main():
    solverAiClientSetup = \
        SolverAiClientSetup(datamanagerUrl, token)

    equation_ids = list()
    code_ids = list()
    hard_data_ids = list()
    soft_data_ids = list()
    problem_id = None

    try:

        # ==== Solar panels ====

        # Table ouitputs:
        #  * sp_area_m2 (panel area)
        #  * sp_w (rated max power)
        #  * sp_prc (solar pannel price)
        #  * sp_eff_perc

        id = solverAiClientSetup.postEquation(
            'SP total area',
            'sp_area_m2_tot = sp_area_m2 * sp_n_tot',
            'sp_area_m2, sp_n_tot'
        )
        equation_ids.append(id)
        id = solverAiClientSetup.postEquation(
            'SP W total',
            'sp_w_tot = sp_n_tot * sp_w',
            'sp_n_tot, sp_w'
        )
        equation_ids.append(id)
        id = solverAiClientSetup.postEquation(
            'SP W total with losses',
            'sp_w_tot_wloss = sp_w_tot * (1 - sp_eff_perc / 100)',
            'sp_w_tot, sp_eff_perc'
        )
        equation_ids.append(id)
        # Equations day:
        id = solverAiClientSetup.postEquation(
            'SP W during the day',
            'sp_w_day = sp_w_tot_wloss',
            'sp_w_tot_wloss'
        )
        equation_ids.append(id)
        # Eqauations price:
        id = solverAiClientSetup.postEquation(
            'SP total price',
            'sp_prc_tot = sp_prc * sp_n_tot',
            'sp_prc, sp_n_tot'
        )
        equation_ids.append(id)

        # ==== Solar inverter ====

        # Table outputs:
        #  * si_kw (power)
        #  * si_prc (inverter cost)
        #  * si_eff_prc

        # Equations:
        id = solverAiClientSetup.postEquation(
            'SI W total',
            'si_w_tot = 1000 * si_kw * si_n_tot',
            'si_kw, si_n_tot'
        )
        equation_ids.append(id)
        id = solverAiClientSetup.postEquation(
            'SI W with loss',
            'si_w_wloss = sp_w_day * (si_eff_perc /100)',
            'sp_w_day, si_eff_perc'
        )
        equation_ids.append(id)
        # Equations day:
        id = solverAiClientSetup.postEquation(
            'SI W during the day',
            'si_w_day = si_w_wloss',
            'si_w_wloss'
        )
        equation_ids.append(id)
        # Eqauations price:
        id = solverAiClientSetup.postEquation(
            'SI total price',
            'si_prc_tot = si_prc * si_n_tot',
            'si_prc, si_n_tot'
        )
        equation_ids.append(id)

        # ==== Battery ====

        # Table outputs:
        #  * b_kwh (power)
        #  * b_prc (battery cost)
        #  * b_v

        # Equations:
        id = solverAiClientSetup.postEquation(
            'B V total',
            'b_v_tot = ic_battery_input_v',
            'ic_battery_input_v'
        )
        equation_ids.append(id)
        id = solverAiClientSetup.postEquation(
            'B number of batteries in series',
            'b_ser_n  = b_v_tot / b_v',
            'b_v_tot, b_v'
        )
        equation_ids.append(id)
        id = solverAiClientSetup.postEquation(
            'B number of batteries in parallel',
            'b_n_tot = b_par_n * b_ser_n',
            'b_par_n, b_ser_n'
        )
        equation_ids.append(id)
        id = solverAiClientSetup.postEquation(
            'B Wh total',
            'b_wh_tot = 1000 * b_kwh * b_n_tot',
            'b_kwh, b_n_tot'
        )
        equation_ids.append(id)
        id = solverAiClientSetup.postEquation(
            'B Wh total with losses',
            'b_wh_tot_wlos = b_wh_tot * (b_eff_perc_loss / 100)',
            'b_wh_tot, b_eff_perc_loss'
        )
        equation_ids.append(id)
        # Equations day:
        id = solverAiClientSetup.postEquation(
            'B W during the day',
            'b_w_day = -b_wh_tot / day_h',
            'b_wh_tot, day_h'
        )
        equation_ids.append(id)
        # Eqauations night:
        id = solverAiClientSetup.postEquation(
            'B W during the night',
            'b_w_ngt = b_wh_tot_wlos / ngt_h',
            'b_wh_tot_wlos, ngt_h'
        )
        equation_ids.append(id)
        # Eqauations price:
        id = solverAiClientSetup.postEquation(
            'B total price',
            'b_prc_tot = b_prc * b_n_tot',
            'b_prc, b_n_tot'
        )
        equation_ids.append(id)

        # ==== Inverter charger ====

        # Table outputs:
        #  * ic_w
        #  * ic_prc
        #  * ic_n_phss
        #  * ic_battery_input_v
        #  * ic_eff_perc_loss

        # Equations:
        id = solverAiClientSetup.postEquation(
            'IC total number',
            'ic_n_tot = ic_n_tot_0 * ic_n_phss',
            'ic_n_tot_0, ic_n_phss'
        )
        equation_ids.append(id)
        id = solverAiClientSetup.postEquation(
            'IC W total',
            'ic_w_tot = ic_w * ic_n_tot',
            'ic_w, ic_n_tot'
        )
        equation_ids.append(id)
        # Equations day:
        id = solverAiClientSetup.postEquation(
            'IC W 1 during the day',
            'ic_w1_day  = -ic_w2_day',
            'ic_w2_day'
        )
        equation_ids.append(id)
        id = solverAiClientSetup.postEquation(
            'IC W 2 during the day',
            'ic_w2_day  = ic_w2_day_wloss / (ic_eff_perc_loss/100)',
            'ic_w2_day_wloss, ic_eff_perc_loss'
        )
        equation_ids.append(id)
        # Eqauations night:
        id = solverAiClientSetup.postEquation(
            'IC W 1 during the night',
            'ic_w1_ngt  = -ic_w2_ngt_wloss',
            'ic_w2_ngt_wloss'
        )
        equation_ids.append(id)
        id = solverAiClientSetup.postEquation(
            'IC W 2 during the night with loss',
            'ic_w2_ngt_wloss = ic_w2_ngt * (ic_eff_perc_loss/100)',
            'ic_w2_ngt, ic_eff_perc_loss'
        )
        equation_ids.append(id)
        # Eqauations price:
        id = solverAiClientSetup.postEquation(
            'IC total price',
            'ic_prc_tot = ic_prc * ic_n_tot_0',
            'ic_prc, ic_n_tot_0'
        )
        equation_ids.append(id)

        # ==== Home ====

        # Equations:
        # Equations day:
        id = solverAiClientSetup.postEquation(
            'Home W during the day',
            'h_wh_day = (si_w_day + ic_w1_day) * day_h',
            'si_w_day, ic_w1_day, day_h'
        )
        equation_ids.append(id)
        # Eqauations night:
        id = solverAiClientSetup.postEquation(
            'Home W during the night',
            'h_wh_ngt = ic_w1_ngt * ngt_h',
            'ic_w1_ngt, ngt_h'
        )
        equation_ids.append(id)

        # ==== System equations ====

        # Equations:

        id = solverAiClientSetup.postEquation(
            'W to battery',
            'ic_w2_day_wloss = -b_w_day',
            'b_w_day'
        )
        equation_ids.append(id)
        id = solverAiClientSetup.postEquation(
            'W from battery',
            'ic_w2_ngt = -b_w_ngt',
            'b_w_ngt'
        )
        equation_ids.append(id)
        id = solverAiClientSetup.postEquation(
            'Difference between SP W and SI supported W',
            'delta_w_sp_si = sp_w_day - si_w_tot',
            'sp_w_day, si_w_tot'
        )
        equation_ids.append(id)
        id = solverAiClientSetup.postEquation(
            'Difference in percentage between SP W and SI supported W',
            'delta_w_sp_si_max_perc = 100 * (sp_w_day - si_w_tot) / si_w_tot',
            'sp_w_day, si_w_tot'
        )
        equation_ids.append(id)
        id = solverAiClientSetup.postEquation(
            'Difference between IC W 1 during the day and IC W total supported',
            'delta_w_ic_max_day = np.abs(ic_w1_day) - ic_w_tot',
            'ic_w1_day, ic_w_tot'
        )
        equation_ids.append(id)
        id = solverAiClientSetup.postEquation(
            'Difference between IC W 2 during the night and IC W total supported',
            'delta_w_ic_max_ngt = np.abs(ic_w1_ngt) - ic_w_tot',
            'ic_w1_ngt, ic_w_tot'
        )
        equation_ids.append(id)
        id = solverAiClientSetup.postEquation(
            'Difference mawimum battery expansion in Wh',
            'delta_wh_max_expansion = 1000 * b_max_expansion_kwh - b_wh_tot',
            'b_max_expansion_kwh, b_wh_tot'
        )
        equation_ids.append(id)
        id = solverAiClientSetup.postEquation(
            'Total cost of the solar plant',
            'tot_prc = sp_prc_tot + si_prc_tot + ic_prc_tot + b_prc_tot',
            'sp_prc_tot, si_prc_tot, ic_prc_tot, b_prc_tot'
        )
        equation_ids.append(id)

        # ==== Component Inventory ====

        id = solverAiClientSetup.postHardData(
            'SP list',
            path.join(data_file_folder_path, 'solar_plant_sizing/SP.csv')
        )
        hard_data_ids.append(id)
        id = solverAiClientSetup.postHardData(
            'SI list',
            path.join(data_file_folder_path, 'solar_plant_sizing/SI.csv')
        )
        hard_data_ids.append(id)
        id = solverAiClientSetup.postHardData(
            'IC list',
            path.join(data_file_folder_path, 'solar_plant_sizing/IC.csv')
        )
        hard_data_ids.append(id)
        id = solverAiClientSetup.postHardData(
            'B list',
            path.join(data_file_folder_path, 'solar_plant_sizing/B.csv')
        )
        hard_data_ids.append(id)

        # Problem setup

        problem_id = solverAiClientSetup.postProblem(
            'Test Problem',
            equationIds=equation_ids,
            codeIds=code_ids,
            hardIds=hard_data_ids,
            softIds=soft_data_ids
        )

        solverAiClientCompute = \
            SolverAiClientCompute(computerUrl, token, problem_id)

        inputs, outputs = solverAiClientCompute.getProblemSetup()

        input = SolverAiComputeInput(problem_id)

        # ############################
        # Constant parameters

        # Number of hours
        input.addInput('day_h', 14, is_constant=True)
        input.addInput('ngt_h', 10, is_constant=True)

        #############################
        # Parameters with ranges

        # Solar Pannels
        input.addInput('sp_n_tot', 1, 50, is_integer=True)

        # Solar Inverter
        input.addInput('si_n_tot', 1, 10, is_integer=True)

        # Inverter Charger
        input.addInput('ic_n_tot_0', 1, 10, is_integer=True)

        # Battery
        input.addInput('b_par_n', 1, 10, is_integer=True)

        # Constraints
        input.addConstraint('sp_area_m2_tot', CONSTRAINT.SMALLER_THAN, 100)
        input.addConstraint('delta_w_sp_si', CONSTRAINT.GREATER_THAN, 0)
        input.addConstraint('delta_w_sp_si_max_perc', CONSTRAINT.SMALLER_THAN, 33)
        input.addConstraint('delta_w_ic_max_day', CONSTRAINT.SMALLER_THAN, 0)
        input.addConstraint('delta_w_ic_max_ngt', CONSTRAINT.SMALLER_THAN, 0)
        input.addConstraint('delta_wh_max_expansion', CONSTRAINT.GREATER_THAN, -.001)

        input.addConstraint('h_wh_day', CONSTRAINT.GREATER_THAN, 5000)
        input.addConstraint('h_wh_ngt', CONSTRAINT.GREATER_THAN, 3000)

        # Objectives
        input.addObjective('tot_prc', OBJECTIVE.MINIMIZE)

        # Solver Setup
        input.setSolverSetup(solutionQuality=10)

        # Run Solver
        results = solverAiClientCompute.runSolver(input)

        # Write results to CSV
        solverAiResultsWriter = SolverAiResultsWriter(results)
        solutionPath = path.join(data_file_folder_path,
                                 'results/solar_plant_sizing_py.csv')
        solverAiResultsWriter.write(solutionPath)

        print('Test was successful!!!')

    except Exception as e:
        print(f'Exception: {str(e)}')

    finally:
        solverAiClientSetup.deleteAll(
            equationIds=equation_ids,
            codeIds=code_ids,
            hardIds=hard_data_ids,
            softIds=soft_data_ids,
            problemId=problem_id
        )


if __name__ == "__main__":
    main()
    exit(0)

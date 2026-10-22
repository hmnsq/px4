# Test internal resistance estimator on flight logs
# run with:
# python3 int_res_est_replay.py -f <pathToLogFile> -c <#batteryCells>
#   -u <(optional)fullCellVoltage> -e <(optional)emptyCellVoltage> -l <(optional)forgettingFactor> -d <(optional)filterMeasurements>
# Note: Can lead to slightly different results than the online estimation due to the fact that
# the log frequency of the voltage and current are not the same as the online frequency.

ತಃ pyulog ಆಮದು ULog
ಆಮದು matplotlib.pyplot ಇವ plt
ಆಮದು numpy ಇವ np
ಆಮದು argparse

ವ್ಯಾಖ್ಯಾನ get_ದತ್ತಾಂಶ(log, topic_name, variable_name, instance=0):
    ಗಾಗಿ elem ಒಳಗೆ log.ದತ್ತಾಂಶ_list:
        ಆದರೆ elem.name == topic_name ಮತ್ತು instance == elem.multi_id:
            ಹಿಂತಿರುಗಿ elem.ದತ್ತಾಂಶ[variable_name]
    ಹಿಂತಿರುಗಿ np.array([])

ವ್ಯಾಖ್ಯಾನ us2s(ಸಮಯ_us):
    ಹಿಂತಿರುಗಿ ಸಮಯ_us * 1e-6

ವ್ಯಾಖ್ಯಾನ getParam(log, param_name):
    ಆದರೆ param_name ಒಳಗೆ log.initial_parameters:
        ಹಿಂತಿರುಗಿ log.initial_parameters[param_name]
    ಇಲ್ಲದಿದ್ದರೆ:
        ಮುದ್ರಿಸು(f"Parameter {param_name} not found in log.")
    ಹಿಂತಿರುಗಿ ಏನೂಇಲ್ಲ

ವ್ಯಾಖ್ಯಾನ rls_update(theta, P, x, V, I, lam):
    gamma = P @ x / (lam + x.T @ P @ x)
    error = V - x.T @ theta
    ದತ್ತಾಂಶ_cov = x.T @ P @ x
    theta_ತಾತ್ಕಾಲಿಕ = theta + gamma * error
    P_ತಾತ್ಕಾಲಿಕ = (P - gamma @ x.T @ P) / lam
    ಆದರೆ (abs(np.linalg.norm(P)) < abs(np.linalg.norm(P_ತಾತ್ಕಾಲಿಕ))):
        theta_corr = np.array([V + theta[1] * I, theta[1]]) # Correct OCV estimation
        P_corr = P
        ಹಿಂತಿರುಗಿ theta_corr, P_corr, error, ದತ್ತಾಂಶ_cov, 0, 0
    ಹಿಂತಿರುಗಿ theta_ತಾತ್ಕಾಲಿಕ, P_ತಾತ್ಕಾಲಿಕ, error, ದತ್ತಾಂಶ_cov, gamma[0], gamma[1]

ವ್ಯಾಖ್ಯಾನ ಮುಖ್ಯ(log_name, n_cells, ತುಂಬಿದ_cell, ಖಾಲಿಗಳು_cell, lam):
    log = ULog(log_name)

    log_n_cells = getParam(log, 'BAT1_N_CELLS')
    log_ತುಂಬಿದ_cell = getParam(log, 'BAT1_V_CHARGED')
    log_ಖಾಲಿಗಳು_cell = getParam(log, 'BAT1_V_EMPTY')

    # Debug information
    ಮುದ್ರಿಸು(f"Extracted from log - BAT1_N_CELLS: {log_n_cells}, BAT1_V_CHARGED: {log_full_cell}, BAT1_V_EMPTY: {log_empty_cell}")

    # Use log parameters unless overridden
    ಆದರೆ n_cells ಆಗಿರುವ ಏನೂಇಲ್ಲ:
        n_cells = log_n_cells
    ಇಲ್ಲದಿದ್ದರೆ:
        ಮುದ್ರಿಸು(f"Using override for n_cells: {n_cells}")
    ಆದರೆ ತುಂಬಿದ_cell ಆಗಿರುವ ಏನೂಇಲ್ಲ:
        ತುಂಬಿದ_cell = log_ತುಂಬಿದ_cell
    ಇಲ್ಲದಿದ್ದರೆ:
        ಮುದ್ರಿಸು(f"Using override for full_cell: {full_cell}")
    ಆದರೆ ಖಾಲಿಗಳು_cell ಆಗಿರುವ ಏನೂಇಲ್ಲ:
        ಖಾಲಿಗಳು_cell = log_ಖಾಲಿಗಳು_cell
    ಇಲ್ಲದಿದ್ದರೆ:
        ಮುದ್ರಿಸು(f"Using override for empty_cell: {empty_cell}")

    # Debug information for final parameter values
    ಮುದ್ರಿಸು(f"Using parameters - n_cells: {n_cells}, full_cell: {full_cell}, empty_cell: {empty_cell}")

    timestamps = us2s(get_ದತ್ತಾಂಶ(log, 'battery_status', 'timestamp'))
    I = get_ದತ್ತಾಂಶ(log, 'battery_status', 'current_a')
    V = get_ದತ್ತಾಂಶ(log, 'battery_status', 'voltage_v')
    ಉಳಿದ = get_ದತ್ತಾಂಶ(log, 'battery_status', 'remaining')

    ಆದರೆ ಅಲ್ಲ timestamps.ಗಾತ್ರ_2 ಅಥವಾ ಅಲ್ಲ I.ಗಾತ್ರ_2 ಅಥವಾ ಅಲ್ಲ V.ಗಾತ್ರ_2 ಅಥವಾ ಅಲ್ಲ ಉಳಿದ.ಗಾತ್ರ_2:
        ಮುದ್ರಿಸು("Error: Incomplete data.")
        ಹಿಂತಿರುಗಿ

    # Initializations
    theta = np.array([[V[0] + 0.005 * n_cells * I[0]], [0.005 * n_cells]])  # Initial VOC and R
    P = np.diag([1.2 * n_cells, 0.1 * n_cells]) # Initial covariance
    error = 0

    # For plotting
    VOC_est = np.zeros_like(I)
    R_est = np.zeros_like(I)
    error_hist = np.zeros_like(I)
    v_est = np.zeros_like(I)
    ಆಂತರಿಕ_resistance_stable = np.zeros_like(I)
    ಆಂತರಿಕ_resistance_stable[-1] = 0.005
    cov_norm = np.zeros_like(I)
    r_cov = np.zeros_like(I)
    ocv_cov = np.zeros_like(I)
    mixed_cov = np.zeros_like(I)
    ದತ್ತಾಂಶ_cov_hist = np.zeros_like(I)
    gamma_voc_hist = np.zeros_like(I)
    gamma_r_hist = np.zeros_like(I)

    ಗಾಗಿ ಸೂಚ್ಯಂಕ ಒಳಗೆ ಶ್ರೇಣಿ(ಉದ್ದ(I)):
        # RLS algorithm
        x = np.array([[1.0], [-I[ಸೂಚ್ಯಂಕ]]]) # Input vector
        theta, P, error, ದತ್ತಾಂಶ_cov, gamma_voc_hist[ಸೂಚ್ಯಂಕ], gamma_r_hist[ಸೂಚ್ಯಂಕ] = rls_update(theta, P, x, V[ಸೂಚ್ಯಂಕ], I[ಸೂಚ್ಯಂಕ], lam) # Run RLS

        # For plotting
        VOC_est[ಸೂಚ್ಯಂಕ] = theta[0][0]
        R_est[ಸೂಚ್ಯಂಕ] = theta[1][0]
        error_hist[ಸೂಚ್ಯಂಕ] = error
        v_est[ಸೂಚ್ಯಂಕ] = x.T @ theta
        cov_norm[ಸೂಚ್ಯಂಕ] = abs(np.linalg.norm(P))
        ocv_cov[ಸೂಚ್ಯಂಕ] = P[0][0]
        r_cov[ಸೂಚ್ಯಂಕ] = P[1][1]
        mixed_cov[ಸೂಚ್ಯಂಕ] = P[0][1]
        ದತ್ತಾಂಶ_cov_hist[ಸೂಚ್ಯಂಕ] = ದತ್ತಾಂಶ_cov
        ಆಂತರಿಕ_resistance_stable[ಸೂಚ್ಯಂಕ] = max(R_est[ಸೂಚ್ಯಂಕ]/n_cells, 0.001)

    # Plot data
    ಮುದ್ರಿಸು("Internal Resistance mean (per cell): ", np.ಸರಾಸರಿ(R_est) / n_cells)

    # Summary plot
    ಮೊತ್ತ_Fig = plt.figure("Battery Estimation with RLS")

    volt = plt.subplot(2, 3, 1)
    volt.plot(timestamps, V, label='Measured voltage')
    volt.plot(timestamps, v_est, label='Estimated voltage')
    volt.plot(timestamps, np.array(V) + np.array(ಆಂತರಿಕ_resistance_stable) * np.array(I) * n_cells, label='OCV estimate')
    ocv_smoothed = np.convolve(np.array(V) + np.array(ಆಂತರಿಕ_resistance_stable) * np.array(I) * n_cells, np.ones(30)/30, ಬಹುಲಕ='full')[0:np.ಗಾತ್ರ_2(timestamps)]
    ocv_smoothed[0:30] = ocv_smoothed[31]
    volt.plot(timestamps, ocv_smoothed, label='OCV estimate smoothed')
    volt.plot(timestamps, np.ತುಂಬಿದ_like(I, ತುಂಬಿದ_cell * n_cells), label='100% SOC')
    volt.ಗಣ_title("Measured Voltage vs Estimated voltage vs Estimated Open circuit voltage [V]")
    volt.legend()

    intR = plt.subplot(2, 3, 2)
    intR.plot(timestamps, np.array(R_est) * 1000 / n_cells, label='Internal resistance estimate')
    intR.ಗಣ_title("Internal resistance estimate (per cell) [mOhm]")
    intR.legend()

    soc = plt.subplot(2, 3, 3)
    soc.plot(timestamps, ಉಳಿದ, label='SoC logged')
    soc.plot(timestamps, np.interp((np.array(V) + np.array(ಆಂತರಿಕ_resistance_stable) * n_cells * np.array(I)) / n_cells, [ಖಾಲಿಗಳು_cell, ತುಂಬಿದ_cell], [0, 1]), label='SoC with estimator')
    soc.plot(timestamps, np.interp(ocv_smoothed / n_cells, [ಖಾಲಿಗಳು_cell, ತುಂಬಿದ_cell], [0, 1]), label='SoC with estimator smoothed')
    soc.ಗಣ_title("State of charge")
    soc.legend()

    curr = plt.subplot(2, 3, 4)
    curr.plot(timestamps, I, label='Measured current')
    curr.ಗಣ_title("Measured Current [A]")
    curr.legend()

    err = plt.subplot(2, 3, 5)
    err.plot(timestamps, error_hist, label='$Error$')
    err.ಗಣ_title("Voltage estimation error [V]")
    err.legend()

    cov = plt.subplot(2, 3, 6)
    cov.plot(timestamps, cov_norm, label = 'Covariance norm')
    cov.ಗಣ_title("Covariance norm")
    cov.legend()

    # # SoC estimation plots
    # socFig = plt.figure("SoC estimation")
    # plt.plot(timestamps, np.interp((np.array(V) + np.array(I) * 0.005 * n_cells) / n_cells, [empty_cell, full_cell], [0, 1]), label='SoC with default $R_{int}$')
    # plt.plot(timestamps, remaining, label='SoC logged')
    # plt.plot(timestamps, np.interp((np.array(V) + np.array(internal_resistance_stable) * n_cells * np.array(I)) / n_cells, [empty_cell, full_cell], [0, 1]), label='SoC with estimator')
    # # plt.plot(timestamps, np.convolve(np.interp((np.array(V) + np.array(internal_resistance_stable) * n_cells * np.array(I)) / n_cells, [empty_cell, full_cell], [0, 1]), np.ones(500)/500, mode='full')[0:np.size(timestamps)], label='SoC with estimator smoothed')
    # # plt.plot(timestamps, np.interp((np.array(V) + np.array(I) * 0.0009 * n_cells) / n_cells, [empty_cell, full_cell], [0, 1]), label='SoC with $R_{int}$ measured beforehand')
    # # plt.plot(timestamps, np.interp(VOC_est/n_cells, [empty_cell, full_cell], [0, 1]), label='SoC with VOC estimate')
    # plt.legend()

    # # Covariance plots
    # covFig = plt.figure("Covariance plots")
    # covR = plt.subplot(2, 2, 1)
    # covR.plot(timestamps, r_cov, label = 'r_cov')
    # covR.set_title("Internal resistance covariance")
    # covR.legend()
    # covVOC = plt.subplot(2, 2, 2)
    # covVOC.plot(timestamps, ocv_cov, label = 'ocv_cov')
    # covVOC.set_title("Open circuit covariance")
    # covVOC.legend()
    # covM = plt.subplot(2, 2, 3)
    # covM.plot(timestamps, mixed_cov, label = 'mixed_cov')
    # covM.set_title("Mixed covariance")
    # covM.legend()
    # covM = plt.subplot(2, 2, 4)
    # covM.plot(timestamps, cov_norm, label = 'cov_norm')
    # covM.set_title("Covariance norm")
    # covM.legend()

    # # Gain plots
    # gainFig = plt.figure("Gain plots")
    # gainVoc = plt.subplot(1, 2, 1)
    # gainVoc.plot(timestamps, gamma_voc_hist, label = 'gain_voc')
    # gainVoc.set_title("Gain VOC")
    # gainVoc.legend()
    # gainR = plt.subplot(1, 2, 2)
    # gainR.plot(timestamps, gamma_r_hist, label = 'gain_r')
    # gainR.set_title("Gain R")
    # gainR.legend()

    plt.show()

ಆದರೆ __name__ == "__main__":
    parser = argparse.ವಾದ_Parser(description='Estimate battery parameters from ulog file.')
    parser.ಕೂಡಿಸು_ವಾದ('-f', type = str,   required = ನಿಜ,  help = 'Full path to ulog file')
    parser.ಕೂಡಿಸು_ವಾದ('-c', type = ದಶಮಾಂಶ, required = ಸುಳ್ಳು, help = 'Number of cells in battery')
    parser.ಕೂಡಿಸು_ವಾದ('-u', type = ದಶಮಾಂಶ, required = ಸುಳ್ಳು, ಪೂರ್ವನಿಯೋಜಿತ = ಏನೂಇಲ್ಲ, help = 'Full cell voltage')
    parser.ಕೂಡಿಸು_ವಾದ('-e', type = ದಶಮಾಂಶ, required = ಸುಳ್ಳು, ಪೂರ್ವನಿಯೋಜಿತ = ಏನೂಇಲ್ಲ,  help = 'Empty cell voltage')
    parser.ಕೂಡಿಸು_ವಾದ('-l', type = ದಶಮಾಂಶ, required = ಸುಳ್ಳು, ಪೂರ್ವನಿಯೋಜಿತ = 0.99, help = 'Forgetting factor')
    ವಾದಗಳು = parser.parse_ವಾದಗಳು()
    ಮುಖ್ಯ(ವಾದಗಳು.f, ವಾದಗಳು.c, ವಾದಗಳು.u, ವಾದಗಳು.e, ವಾದಗಳು.l)

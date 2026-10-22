# Test internal resistance estimator on flight logs
# run with:
# python3 int_res_est_replay.py -f <pathToLogFile> -c <#batteryCells>
#   -u <(optional)fullCellVoltage> -e <(optional)emptyCellVoltage> -l <(optional)forgettingFactor> -d <(optional)filterMeasurements>
# Note: Can lead to slightly different results than the online estimation due to the fact that
# the log frequency of the voltage and current are not the same as the online frequency.

తః pyulog దిగుమతి ULog
దిగుమతి matplotlib.pyplot ఇవ plt
దిగుమతి numpy ఇవ np
దిగుమతి argparse

నిర్వచనం get_దత్తాంశం(log, topic_name, variable_name, instance=0):
    కొరకు elem లో log.దత్తాంశం_list:
        ఒకవేళ elem.name == topic_name మరియు instance == elem.multi_id:
            తిరిగి elem.దత్తాంశం[variable_name]
    తిరిగి np.array([])

నిర్వచనం us2s(సమయం_us):
    తిరిగి సమయం_us * 1e-6

నిర్వచనం getParam(log, param_name):
    ఒకవేళ param_name లో log.initial_parameters:
        తిరిగి log.initial_parameters[param_name]
    లేకపోతే:
        ముద్రించు(f"Parameter {param_name} not found in log.")
    తిరిగి ఏమీలేదు

నిర్వచనం rls_update(theta, P, x, V, I, lam):
    gamma = P @ x / (lam + x.T @ P @ x)
    error = V - x.T @ theta
    దత్తాంశం_cov = x.T @ P @ x
    theta_తాత్కాలిక = theta + gamma * error
    P_తాత్కాలిక = (P - gamma @ x.T @ P) / lam
    ఒకవేళ (abs(np.linalg.norm(P)) < abs(np.linalg.norm(P_తాత్కాలిక))):
        theta_corr = np.array([V + theta[1] * I, theta[1]]) # Correct OCV estimation
        P_corr = P
        తిరిగి theta_corr, P_corr, error, దత్తాంశం_cov, 0, 0
    తిరిగి theta_తాత్కాలిక, P_తాత్కాలిక, error, దత్తాంశం_cov, gamma[0], gamma[1]

నిర్వచనం ప్రధాన(log_name, n_cells, నిండిన_cell, ఖాళీలు_cell, lam):
    log = ULog(log_name)

    log_n_cells = getParam(log, 'BAT1_N_CELLS')
    log_నిండిన_cell = getParam(log, 'BAT1_V_CHARGED')
    log_ఖాళీలు_cell = getParam(log, 'BAT1_V_EMPTY')

    # Debug information
    ముద్రించు(f"Extracted from log - BAT1_N_CELLS: {log_n_cells}, BAT1_V_CHARGED: {log_full_cell}, BAT1_V_EMPTY: {log_empty_cell}")

    # Use log parameters unless overridden
    ఒకవేళ n_cells అయిన ఏమీలేదు:
        n_cells = log_n_cells
    లేకపోతే:
        ముద్రించు(f"Using override for n_cells: {n_cells}")
    ఒకవేళ నిండిన_cell అయిన ఏమీలేదు:
        నిండిన_cell = log_నిండిన_cell
    లేకపోతే:
        ముద్రించు(f"Using override for full_cell: {full_cell}")
    ఒకవేళ ఖాళీలు_cell అయిన ఏమీలేదు:
        ఖాళీలు_cell = log_ఖాళీలు_cell
    లేకపోతే:
        ముద్రించు(f"Using override for empty_cell: {empty_cell}")

    # Debug information for final parameter values
    ముద్రించు(f"Using parameters - n_cells: {n_cells}, full_cell: {full_cell}, empty_cell: {empty_cell}")

    timestamps = us2s(get_దత్తాంశం(log, 'battery_status', 'timestamp'))
    I = get_దత్తాంశం(log, 'battery_status', 'current_a')
    V = get_దత్తాంశం(log, 'battery_status', 'voltage_v')
    మిగిలిన = get_దత్తాంశం(log, 'battery_status', 'remaining')

    ఒకవేళ కాదు timestamps.కొలత లేదా కాదు I.కొలత లేదా కాదు V.కొలత లేదా కాదు మిగిలిన.కొలత:
        ముద్రించు("Error: Incomplete data.")
        తిరిగి

    # Initializations
    theta = np.array([[V[0] + 0.005 * n_cells * I[0]], [0.005 * n_cells]])  # Initial VOC and R
    P = np.diag([1.2 * n_cells, 0.1 * n_cells]) # Initial covariance
    error = 0

    # For plotting
    VOC_est = np.zeros_like(I)
    R_est = np.zeros_like(I)
    error_hist = np.zeros_like(I)
    v_est = np.zeros_like(I)
    అంతర్గత_resistance_stable = np.zeros_like(I)
    అంతర్గత_resistance_stable[-1] = 0.005
    cov_norm = np.zeros_like(I)
    r_cov = np.zeros_like(I)
    ocv_cov = np.zeros_like(I)
    mixed_cov = np.zeros_like(I)
    దత్తాంశం_cov_hist = np.zeros_like(I)
    gamma_voc_hist = np.zeros_like(I)
    gamma_r_hist = np.zeros_like(I)

    కొరకు సూచీ లో శ్రేణి(పొడవు(I)):
        # RLS algorithm
        x = np.array([[1.0], [-I[సూచీ]]]) # Input vector
        theta, P, error, దత్తాంశం_cov, gamma_voc_hist[సూచీ], gamma_r_hist[సూచీ] = rls_update(theta, P, x, V[సూచీ], I[సూచీ], lam) # Run RLS

        # For plotting
        VOC_est[సూచీ] = theta[0][0]
        R_est[సూచీ] = theta[1][0]
        error_hist[సూచీ] = error
        v_est[సూచీ] = x.T @ theta
        cov_norm[సూచీ] = abs(np.linalg.norm(P))
        ocv_cov[సూచీ] = P[0][0]
        r_cov[సూచీ] = P[1][1]
        mixed_cov[సూచీ] = P[0][1]
        దత్తాంశం_cov_hist[సూచీ] = దత్తాంశం_cov
        అంతర్గత_resistance_stable[సూచీ] = max(R_est[సూచీ]/n_cells, 0.001)

    # Plot data
    ముద్రించు("Internal Resistance mean (per cell): ", np.సగటు(R_est) / n_cells)

    # Summary plot
    కూడిక_Fig = plt.figure("Battery Estimation with RLS")

    volt = plt.subplot(2, 3, 1)
    volt.plot(timestamps, V, label='Measured voltage')
    volt.plot(timestamps, v_est, label='Estimated voltage')
    volt.plot(timestamps, np.array(V) + np.array(అంతర్గత_resistance_stable) * np.array(I) * n_cells, label='OCV estimate')
    ocv_smoothed = np.convolve(np.array(V) + np.array(అంతర్గత_resistance_stable) * np.array(I) * n_cells, np.ones(30)/30, బహుళకం='full')[0:np.కొలత(timestamps)]
    ocv_smoothed[0:30] = ocv_smoothed[31]
    volt.plot(timestamps, ocv_smoothed, label='OCV estimate smoothed')
    volt.plot(timestamps, np.నిండిన_like(I, నిండిన_cell * n_cells), label='100% SOC')
    volt.సమితి_title("Measured Voltage vs Estimated voltage vs Estimated Open circuit voltage [V]")
    volt.legend()

    intR = plt.subplot(2, 3, 2)
    intR.plot(timestamps, np.array(R_est) * 1000 / n_cells, label='Internal resistance estimate')
    intR.సమితి_title("Internal resistance estimate (per cell) [mOhm]")
    intR.legend()

    soc = plt.subplot(2, 3, 3)
    soc.plot(timestamps, మిగిలిన, label='SoC logged')
    soc.plot(timestamps, np.interp((np.array(V) + np.array(అంతర్గత_resistance_stable) * n_cells * np.array(I)) / n_cells, [ఖాళీలు_cell, నిండిన_cell], [0, 1]), label='SoC with estimator')
    soc.plot(timestamps, np.interp(ocv_smoothed / n_cells, [ఖాళీలు_cell, నిండిన_cell], [0, 1]), label='SoC with estimator smoothed')
    soc.సమితి_title("State of charge")
    soc.legend()

    curr = plt.subplot(2, 3, 4)
    curr.plot(timestamps, I, label='Measured current')
    curr.సమితి_title("Measured Current [A]")
    curr.legend()

    err = plt.subplot(2, 3, 5)
    err.plot(timestamps, error_hist, label='$Error$')
    err.సమితి_title("Voltage estimation error [V]")
    err.legend()

    cov = plt.subplot(2, 3, 6)
    cov.plot(timestamps, cov_norm, label = 'Covariance norm')
    cov.సమితి_title("Covariance norm")
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

ఒకవేళ __name__ == "__main__":
    parser = argparse.వాదం_Parser(description='Estimate battery parameters from ulog file.')
    parser.కూడు_వాదం('-f', type = str,   required = నిజం,  help = 'Full path to ulog file')
    parser.కూడు_వాదం('-c', type = దశాంశం, required = అబద్ధం, help = 'Number of cells in battery')
    parser.కూడు_వాదం('-u', type = దశాంశం, required = అబద్ధం, అప్రమేయ = ఏమీలేదు, help = 'Full cell voltage')
    parser.కూడు_వాదం('-e', type = దశాంశం, required = అబద్ధం, అప్రమేయ = ఏమీలేదు,  help = 'Empty cell voltage')
    parser.కూడు_వాదం('-l', type = దశాంశం, required = అబద్ధం, అప్రమేయ = 0.99, help = 'Forgetting factor')
    వాదనలు = parser.parse_వాదనలు()
    ప్రధాన(వాదనలు.f, వాదనలు.c, వాదనలు.u, వాదనలు.e, వాదనలు.l)

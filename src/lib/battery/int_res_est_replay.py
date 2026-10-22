# Test internal resistance estimator on flight logs
# run with:
# python3 int_res_est_replay.py -f <pathToLogFile> -c <#batteryCells>
#   -u <(optional)fullCellVoltage> -e <(optional)emptyCellVoltage> -l <(optional)forgettingFactor> -d <(optional)filterMeasurements>
# Note: Can lead to slightly different results than the online estimation due to the fact that
# the log frequency of the voltage and current are not the same as the online frequency.

तः pyulog आनयन ULog
आनयन matplotlib.pyplot इव plt
आनयन numpy इव np
आनयन argparse

परिभाषा get_दत्त(log, topic_name, variable_name, instance=0):
    प्रति elem अन्तः log.दत्त_list:
        यदि elem.name == topic_name च instance == elem.multi_id:
            प्रत्यागम elem.दत्त[variable_name]
    प्रत्यागम np.array([])

परिभाषा us2s(time_us):
    प्रत्यागम time_us * 1e-6

परिभाषा getParam(log, param_name):
    यदि param_name अन्तः log.initial_parameters:
        प्रत्यागम log.initial_parameters[param_name]
    अन्यथा:
        मुद्रय(f"Parameter {param_name} not found in log.")
    प्रत्यागम किमपि_न

परिभाषा rls_update(theta, P, x, V, I, lam):
    gamma = P @ x / (lam + x.T @ P @ x)
    error = V - x.T @ theta
    दत्त_cov = x.T @ P @ x
    theta_अस्थायी = theta + gamma * error
    P_अस्थायी = (P - gamma @ x.T @ P) / lam
    यदि (abs(np.linalg.norm(P)) < abs(np.linalg.norm(P_अस्थायी))):
        theta_corr = np.array([V + theta[1] * I, theta[1]]) # Correct OCV estimation
        P_corr = P
        प्रत्यागम theta_corr, P_corr, error, दत्त_cov, 0, 0
    प्रत्यागम theta_अस्थायी, P_अस्थायी, error, दत्त_cov, gamma[0], gamma[1]

परिभाषा मुख्यम्(log_name, n_cells, पूर्णम्_cell, रिक्तानि_cell, lam):
    log = ULog(log_name)

    log_n_cells = getParam(log, 'BAT1_N_CELLS')
    log_पूर्णम्_cell = getParam(log, 'BAT1_V_CHARGED')
    log_रिक्तानि_cell = getParam(log, 'BAT1_V_EMPTY')

    # Debug information
    मुद्रय(f"Extracted from log - BAT1_N_CELLS: {log_n_cells}, BAT1_V_CHARGED: {log_full_cell}, BAT1_V_EMPTY: {log_empty_cell}")

    # Use log parameters unless overridden
    यदि n_cells अस्ति किमपि_न:
        n_cells = log_n_cells
    अन्यथा:
        मुद्रय(f"Using override for n_cells: {n_cells}")
    यदि पूर्णम्_cell अस्ति किमपि_न:
        पूर्णम्_cell = log_पूर्णम्_cell
    अन्यथा:
        मुद्रय(f"Using override for full_cell: {full_cell}")
    यदि रिक्तानि_cell अस्ति किमपि_न:
        रिक्तानि_cell = log_रिक्तानि_cell
    अन्यथा:
        मुद्रय(f"Using override for empty_cell: {empty_cell}")

    # Debug information for final parameter values
    मुद्रय(f"Using parameters - n_cells: {n_cells}, full_cell: {full_cell}, empty_cell: {empty_cell}")

    timestamps = us2s(get_दत्त(log, 'battery_status', 'timestamp'))
    I = get_दत्त(log, 'battery_status', 'current_a')
    V = get_दत्त(log, 'battery_status', 'voltage_v')
    remaining = get_दत्त(log, 'battery_status', 'remaining')

    यदि न timestamps.आकार वा न I.आकार वा न V.आकार वा न remaining.आकार:
        मुद्रय("Error: Incomplete data.")
        प्रत्यागम

    # Initializations
    theta = np.array([[V[0] + 0.005 * n_cells * I[0]], [0.005 * n_cells]])  # Initial VOC and R
    P = np.diag([1.2 * n_cells, 0.1 * n_cells]) # Initial covariance
    error = 0

    # For plotting
    VOC_est = np.zeros_like(I)
    R_est = np.zeros_like(I)
    error_hist = np.zeros_like(I)
    v_est = np.zeros_like(I)
    आन्तरिकम्_resistance_stable = np.zeros_like(I)
    आन्तरिकम्_resistance_stable[-1] = 0.005
    cov_norm = np.zeros_like(I)
    r_cov = np.zeros_like(I)
    ocv_cov = np.zeros_like(I)
    mixed_cov = np.zeros_like(I)
    दत्त_cov_hist = np.zeros_like(I)
    gamma_voc_hist = np.zeros_like(I)
    gamma_r_hist = np.zeros_like(I)

    प्रति सूचकांक अन्तः परिसरः(दीर्घता(I)):
        # RLS algorithm
        x = np.array([[1.0], [-I[सूचकांक]]]) # Input vector
        theta, P, error, दत्त_cov, gamma_voc_hist[सूचकांक], gamma_r_hist[सूचकांक] = rls_update(theta, P, x, V[सूचकांक], I[सूचकांक], lam) # Run RLS

        # For plotting
        VOC_est[सूचकांक] = theta[0][0]
        R_est[सूचकांक] = theta[1][0]
        error_hist[सूचकांक] = error
        v_est[सूचकांक] = x.T @ theta
        cov_norm[सूचकांक] = abs(np.linalg.norm(P))
        ocv_cov[सूचकांक] = P[0][0]
        r_cov[सूचकांक] = P[1][1]
        mixed_cov[सूचकांक] = P[0][1]
        दत्त_cov_hist[सूचकांक] = दत्त_cov
        आन्तरिकम्_resistance_stable[सूचकांक] = max(R_est[सूचकांक]/n_cells, 0.001)

    # Plot data
    मुद्रय("Internal Resistance mean (per cell): ", np.माध्यम्(R_est) / n_cells)

    # Summary plot
    योग_Fig = plt.figure("Battery Estimation with RLS")

    volt = plt.subplot(2, 3, 1)
    volt.plot(timestamps, V, label='Measured voltage')
    volt.plot(timestamps, v_est, label='Estimated voltage')
    volt.plot(timestamps, np.array(V) + np.array(आन्तरिकम्_resistance_stable) * np.array(I) * n_cells, label='OCV estimate')
    ocv_smoothed = np.convolve(np.array(V) + np.array(आन्तरिकम्_resistance_stable) * np.array(I) * n_cells, np.ones(30)/30, बहुलकम्='full')[0:np.आकार(timestamps)]
    ocv_smoothed[0:30] = ocv_smoothed[31]
    volt.plot(timestamps, ocv_smoothed, label='OCV estimate smoothed')
    volt.plot(timestamps, np.पूर्णम्_like(I, पूर्णम्_cell * n_cells), label='100% SOC')
    volt.समुच्चयः_title("Measured Voltage vs Estimated voltage vs Estimated Open circuit voltage [V]")
    volt.legend()

    intR = plt.subplot(2, 3, 2)
    intR.plot(timestamps, np.array(R_est) * 1000 / n_cells, label='Internal resistance estimate')
    intR.समुच्चयः_title("Internal resistance estimate (per cell) [mOhm]")
    intR.legend()

    soc = plt.subplot(2, 3, 3)
    soc.plot(timestamps, remaining, label='SoC logged')
    soc.plot(timestamps, np.interp((np.array(V) + np.array(आन्तरिकम्_resistance_stable) * n_cells * np.array(I)) / n_cells, [रिक्तानि_cell, पूर्णम्_cell], [0, 1]), label='SoC with estimator')
    soc.plot(timestamps, np.interp(ocv_smoothed / n_cells, [रिक्तानि_cell, पूर्णम्_cell], [0, 1]), label='SoC with estimator smoothed')
    soc.समुच्चयः_title("State of charge")
    soc.legend()

    curr = plt.subplot(2, 3, 4)
    curr.plot(timestamps, I, label='Measured current')
    curr.समुच्चयः_title("Measured Current [A]")
    curr.legend()

    err = plt.subplot(2, 3, 5)
    err.plot(timestamps, error_hist, label='$Error$')
    err.समुच्चयः_title("Voltage estimation error [V]")
    err.legend()

    cov = plt.subplot(2, 3, 6)
    cov.plot(timestamps, cov_norm, label = 'Covariance norm')
    cov.समुच्चयः_title("Covariance norm")
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

यदि __name__ == "__main__":
    parser = argparse.तर्कः_Parser(description='Estimate battery parameters from ulog file.')
    parser.सङ्कलय_तर्कः('-f', type = str,   required = सत्य,  help = 'Full path to ulog file')
    parser.सङ्कलय_तर्कः('-c', type = भिन्न, required = असत्य, help = 'Number of cells in battery')
    parser.सङ्कलय_तर्कः('-u', type = भिन्न, required = असत्य, स्वभाव = किमपि_न, help = 'Full cell voltage')
    parser.सङ्कलय_तर्कः('-e', type = भिन्न, required = असत्य, स्वभाव = किमपि_न,  help = 'Empty cell voltage')
    parser.सङ्कलय_तर्कः('-l', type = भिन्न, required = असत्य, स्वभाव = 0.99, help = 'Forgetting factor')
    तर्काः = parser.parse_तर्काः()
    मुख्यम्(तर्काः.f, तर्काः.c, तर्काः.u, तर्काः.e, तर्काः.l)

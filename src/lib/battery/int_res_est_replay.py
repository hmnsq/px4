# Test internal resistance estimator on flight logs
# run with:
# python3 int_res_est_replay.py -f <pathToLogFile> -c <#batteryCells>
#   -u <(optional)fullCellVoltage> -e <(optional)emptyCellVoltage> -l <(optional)forgettingFactor> -d <(optional)filterMeasurements>
# Note: Can lead to slightly different results than the online estimation due to the fact that
# the log frequency of the voltage and current are not the same as the online frequency.

ਤਃ pyulog ਆਯਾਤ ULog
ਆਯਾਤ matplotlib.pyplot ਇਵ plt
ਆਯਾਤ numpy ਇਵ np
ਆਯਾਤ argparse

ਪਰਿਭਾਸ਼ਾ get_ਅੰਕੜੇ(log, topic_name, variable_name, instance=0):
    ਲਈ elem ਵਿੱਚ log.ਅੰਕੜੇ_list:
        ਜੇ elem.name == topic_name ਅਤੇ instance == elem.multi_id:
            ਵਾਪਸ elem.ਅੰਕੜੇ[variable_name]
    ਵਾਪਸ np.array([])

ਪਰਿਭਾਸ਼ਾ us2s(ਸਮਾਂ_us):
    ਵਾਪਸ ਸਮਾਂ_us * 1e-6

ਪਰਿਭਾਸ਼ਾ getParam(log, param_name):
    ਜੇ param_name ਵਿੱਚ log.initial_parameters:
        ਵਾਪਸ log.initial_parameters[param_name]
    ਨਹੀਂਤਾਂ:
        ਛਾਪੋ(f"Parameter {param_name} not found in log.")
    ਵਾਪਸ ਕੁਝ_ਨਹੀਂ

ਪਰਿਭਾਸ਼ਾ rls_update(theta, P, x, V, I, lam):
    gamma = P @ x / (lam + x.T @ P @ x)
    error = V - x.T @ theta
    ਅੰਕੜੇ_cov = x.T @ P @ x
    theta_ਆਰਜ਼ੀ = theta + gamma * error
    P_ਆਰਜ਼ੀ = (P - gamma @ x.T @ P) / lam
    ਜੇ (abs(np.linalg.norm(P)) < abs(np.linalg.norm(P_ਆਰਜ਼ੀ))):
        theta_corr = np.array([V + theta[1] * I, theta[1]]) # Correct OCV estimation
        P_corr = P
        ਵਾਪਸ theta_corr, P_corr, error, ਅੰਕੜੇ_cov, 0, 0
    ਵਾਪਸ theta_ਆਰਜ਼ੀ, P_ਆਰਜ਼ੀ, error, ਅੰਕੜੇ_cov, gamma[0], gamma[1]

ਪਰਿਭਾਸ਼ਾ ਮੁੱਖ(log_name, n_cells, ਭਰਿਆ_cell, ਖਾਲੀ_ਥਾਵਾਂ_cell, lam):
    log = ULog(log_name)

    log_n_cells = getParam(log, 'BAT1_N_CELLS')
    log_ਭਰਿਆ_cell = getParam(log, 'BAT1_V_CHARGED')
    log_ਖਾਲੀ_ਥਾਵਾਂ_cell = getParam(log, 'BAT1_V_EMPTY')

    # Debug information
    ਛਾਪੋ(f"Extracted from log - BAT1_N_CELLS: {log_n_cells}, BAT1_V_CHARGED: {log_full_cell}, BAT1_V_EMPTY: {log_empty_cell}")

    # Use log parameters unless overridden
    ਜੇ n_cells ਹੈ ਕੁਝ_ਨਹੀਂ:
        n_cells = log_n_cells
    ਨਹੀਂਤਾਂ:
        ਛਾਪੋ(f"Using override for n_cells: {n_cells}")
    ਜੇ ਭਰਿਆ_cell ਹੈ ਕੁਝ_ਨਹੀਂ:
        ਭਰਿਆ_cell = log_ਭਰਿਆ_cell
    ਨਹੀਂਤਾਂ:
        ਛਾਪੋ(f"Using override for full_cell: {full_cell}")
    ਜੇ ਖਾਲੀ_ਥਾਵਾਂ_cell ਹੈ ਕੁਝ_ਨਹੀਂ:
        ਖਾਲੀ_ਥਾਵਾਂ_cell = log_ਖਾਲੀ_ਥਾਵਾਂ_cell
    ਨਹੀਂਤਾਂ:
        ਛਾਪੋ(f"Using override for empty_cell: {empty_cell}")

    # Debug information for final parameter values
    ਛਾਪੋ(f"Using parameters - n_cells: {n_cells}, full_cell: {full_cell}, empty_cell: {empty_cell}")

    timestamps = us2s(get_ਅੰਕੜੇ(log, 'battery_status', 'timestamp'))
    I = get_ਅੰਕੜੇ(log, 'battery_status', 'current_a')
    V = get_ਅੰਕੜੇ(log, 'battery_status', 'voltage_v')
    ਬਚਿਆ = get_ਅੰਕੜੇ(log, 'battery_status', 'remaining')

    ਜੇ ਨਹੀਂ timestamps.ਮਾਪ ਜਾਂ ਨਹੀਂ I.ਮਾਪ ਜਾਂ ਨਹੀਂ V.ਮਾਪ ਜਾਂ ਨਹੀਂ ਬਚਿਆ.ਮਾਪ:
        ਛਾਪੋ("Error: Incomplete data.")
        ਵਾਪਸ

    # Initializations
    theta = np.array([[V[0] + 0.005 * n_cells * I[0]], [0.005 * n_cells]])  # Initial VOC and R
    P = np.diag([1.2 * n_cells, 0.1 * n_cells]) # Initial covariance
    error = 0

    # For plotting
    VOC_est = np.zeros_like(I)
    R_est = np.zeros_like(I)
    error_hist = np.zeros_like(I)
    v_est = np.zeros_like(I)
    ਅੰਦਰੂਨੀ_resistance_stable = np.zeros_like(I)
    ਅੰਦਰੂਨੀ_resistance_stable[-1] = 0.005
    cov_norm = np.zeros_like(I)
    r_cov = np.zeros_like(I)
    ocv_cov = np.zeros_like(I)
    mixed_cov = np.zeros_like(I)
    ਅੰਕੜੇ_cov_hist = np.zeros_like(I)
    gamma_voc_hist = np.zeros_like(I)
    gamma_r_hist = np.zeros_like(I)

    ਲਈ ਸੂਚਕਾਂਕ ਵਿੱਚ ਸੀਮਾ(ਲੰਬਾਈ(I)):
        # RLS algorithm
        x = np.array([[1.0], [-I[ਸੂਚਕਾਂਕ]]]) # Input vector
        theta, P, error, ਅੰਕੜੇ_cov, gamma_voc_hist[ਸੂਚਕਾਂਕ], gamma_r_hist[ਸੂਚਕਾਂਕ] = rls_update(theta, P, x, V[ਸੂਚਕਾਂਕ], I[ਸੂਚਕਾਂਕ], lam) # Run RLS

        # For plotting
        VOC_est[ਸੂਚਕਾਂਕ] = theta[0][0]
        R_est[ਸੂਚਕਾਂਕ] = theta[1][0]
        error_hist[ਸੂਚਕਾਂਕ] = error
        v_est[ਸੂਚਕਾਂਕ] = x.T @ theta
        cov_norm[ਸੂਚਕਾਂਕ] = abs(np.linalg.norm(P))
        ocv_cov[ਸੂਚਕਾਂਕ] = P[0][0]
        r_cov[ਸੂਚਕਾਂਕ] = P[1][1]
        mixed_cov[ਸੂਚਕਾਂਕ] = P[0][1]
        ਅੰਕੜੇ_cov_hist[ਸੂਚਕਾਂਕ] = ਅੰਕੜੇ_cov
        ਅੰਦਰੂਨੀ_resistance_stable[ਸੂਚਕਾਂਕ] = max(R_est[ਸੂਚਕਾਂਕ]/n_cells, 0.001)

    # Plot data
    ਛਾਪੋ("Internal Resistance mean (per cell): ", np.ਔਸਤ(R_est) / n_cells)

    # Summary plot
    ਜੋੜ_Fig = plt.figure("Battery Estimation with RLS")

    volt = plt.subplot(2, 3, 1)
    volt.plot(timestamps, V, label='Measured voltage')
    volt.plot(timestamps, v_est, label='Estimated voltage')
    volt.plot(timestamps, np.array(V) + np.array(ਅੰਦਰੂਨੀ_resistance_stable) * np.array(I) * n_cells, label='OCV estimate')
    ocv_smoothed = np.convolve(np.array(V) + np.array(ਅੰਦਰੂਨੀ_resistance_stable) * np.array(I) * n_cells, np.ones(30)/30, ਬਹੁਲਕ='full')[0:np.ਮਾਪ(timestamps)]
    ocv_smoothed[0:30] = ocv_smoothed[31]
    volt.plot(timestamps, ocv_smoothed, label='OCV estimate smoothed')
    volt.plot(timestamps, np.ਭਰਿਆ_like(I, ਭਰਿਆ_cell * n_cells), label='100% SOC')
    volt.ਸੈੱਟ_title("Measured Voltage vs Estimated voltage vs Estimated Open circuit voltage [V]")
    volt.legend()

    intR = plt.subplot(2, 3, 2)
    intR.plot(timestamps, np.array(R_est) * 1000 / n_cells, label='Internal resistance estimate')
    intR.ਸੈੱਟ_title("Internal resistance estimate (per cell) [mOhm]")
    intR.legend()

    soc = plt.subplot(2, 3, 3)
    soc.plot(timestamps, ਬਚਿਆ, label='SoC logged')
    soc.plot(timestamps, np.interp((np.array(V) + np.array(ਅੰਦਰੂਨੀ_resistance_stable) * n_cells * np.array(I)) / n_cells, [ਖਾਲੀ_ਥਾਵਾਂ_cell, ਭਰਿਆ_cell], [0, 1]), label='SoC with estimator')
    soc.plot(timestamps, np.interp(ocv_smoothed / n_cells, [ਖਾਲੀ_ਥਾਵਾਂ_cell, ਭਰਿਆ_cell], [0, 1]), label='SoC with estimator smoothed')
    soc.ਸੈੱਟ_title("State of charge")
    soc.legend()

    curr = plt.subplot(2, 3, 4)
    curr.plot(timestamps, I, label='Measured current')
    curr.ਸੈੱਟ_title("Measured Current [A]")
    curr.legend()

    err = plt.subplot(2, 3, 5)
    err.plot(timestamps, error_hist, label='$Error$')
    err.ਸੈੱਟ_title("Voltage estimation error [V]")
    err.legend()

    cov = plt.subplot(2, 3, 6)
    cov.plot(timestamps, cov_norm, label = 'Covariance norm')
    cov.ਸੈੱਟ_title("Covariance norm")
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

ਜੇ __name__ == "__main__":
    parser = argparse.ਦਲੀਲ_Parser(description='Estimate battery parameters from ulog file.')
    parser.ਜੋੜੋ_ਦਲੀਲ('-f', type = str,   required = ਸੱਚ,  help = 'Full path to ulog file')
    parser.ਜੋੜੋ_ਦਲੀਲ('-c', type = ਦਸ਼ਮਲਵ, required = ਝੂਠ, help = 'Number of cells in battery')
    parser.ਜੋੜੋ_ਦਲੀਲ('-u', type = ਦਸ਼ਮਲਵ, required = ਝੂਠ, ਮੂਲ = ਕੁਝ_ਨਹੀਂ, help = 'Full cell voltage')
    parser.ਜੋੜੋ_ਦਲੀਲ('-e', type = ਦਸ਼ਮਲਵ, required = ਝੂਠ, ਮੂਲ = ਕੁਝ_ਨਹੀਂ,  help = 'Empty cell voltage')
    parser.ਜੋੜੋ_ਦਲੀਲ('-l', type = ਦਸ਼ਮਲਵ, required = ਝੂਠ, ਮੂਲ = 0.99, help = 'Forgetting factor')
    ਦਲੀਲਾਂ = parser.parse_ਦਲੀਲਾਂ()
    ਮੁੱਖ(ਦਲੀਲਾਂ.f, ਦਲੀਲਾਂ.c, ਦਲੀਲਾਂ.u, ਦਲੀਲਾਂ.e, ਦਲੀਲਾਂ.l)

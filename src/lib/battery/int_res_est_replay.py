# Test internal resistance estimator on flight logs
# run with:
# python3 int_res_est_replay.py -f <pathToLogFile> -c <#batteryCells>
#   -u <(optional)fullCellVoltage> -e <(optional)emptyCellVoltage> -l <(optional)forgettingFactor> -d <(optional)filterMeasurements>
# Note: Can lead to slightly different results than the online estimation due to the fact that
# the log frequency of the voltage and current are not the same as the online frequency.

من pyulog استورد ULog
استورد matplotlib.pyplot كـ plt
استورد numpy كـ np
استورد argparse

تعریف get_داده(log, topic_name, variable_name, instance=0):
    لكل elem في log.داده_list:
        إذا elem.name == topic_name و instance == elem.multi_id:
            أرجع elem.داده[variable_name]
    أرجع np.array([])

تعریف us2s(زمان_us):
    أرجع زمان_us * 1e-6

تعریف getParam(log, param_name):
    إذا param_name في log.initial_parameters:
        أرجع log.initial_parameters[param_name]
    وإلا:
        چاپ(f"Parameter {param_name} not found in log.")
    أرجع هیچ

تعریف rls_update(theta, P, x, V, I, lam):
    gamma = P @ x / (lam + x.T @ P @ x)
    error = V - x.T @ theta
    داده_cov = x.T @ P @ x
    theta_موقت = theta + gamma * error
    P_موقت = (P - gamma @ x.T @ P) / lam
    إذا (abs(np.linalg.norm(P)) < abs(np.linalg.norm(P_موقت))):
        theta_corr = np.array([V + theta[1] * I, theta[1]]) # Correct OCV estimation
        P_corr = P
        أرجع theta_corr, P_corr, error, داده_cov, 0, 0
    أرجع theta_موقت, P_موقت, error, داده_cov, gamma[0], gamma[1]

تعریف اصلی(log_name, n_cells, پر_cell, خالی_cell, lam):
    log = ULog(log_name)

    log_n_cells = getParam(log, 'BAT1_N_CELLS')
    log_پر_cell = getParam(log, 'BAT1_V_CHARGED')
    log_خالی_cell = getParam(log, 'BAT1_V_EMPTY')

    # Debug information
    چاپ(f"Extracted from log - BAT1_N_CELLS: {log_n_cells}, BAT1_V_CHARGED: {log_full_cell}, BAT1_V_EMPTY: {log_empty_cell}")

    # Use log parameters unless overridden
    إذا n_cells است هیچ:
        n_cells = log_n_cells
    وإلا:
        چاپ(f"Using override for n_cells: {n_cells}")
    إذا پر_cell است هیچ:
        پر_cell = log_پر_cell
    وإلا:
        چاپ(f"Using override for full_cell: {full_cell}")
    إذا خالی_cell است هیچ:
        خالی_cell = log_خالی_cell
    وإلا:
        چاپ(f"Using override for empty_cell: {empty_cell}")

    # Debug information for final parameter values
    چاپ(f"Using parameters - n_cells: {n_cells}, full_cell: {full_cell}, empty_cell: {empty_cell}")

    timestamps = us2s(get_داده(log, 'battery_status', 'timestamp'))
    I = get_داده(log, 'battery_status', 'current_a')
    V = get_داده(log, 'battery_status', 'voltage_v')
    مانده = get_داده(log, 'battery_status', 'remaining')

    إذا ليس timestamps.اندازه أو ليس I.اندازه أو ليس V.اندازه أو ليس مانده.اندازه:
        چاپ("Error: Incomplete data.")
        أرجع

    # Initializations
    theta = np.array([[V[0] + 0.005 * n_cells * I[0]], [0.005 * n_cells]])  # Initial VOC and R
    P = np.diag([1.2 * n_cells, 0.1 * n_cells]) # Initial covariance
    error = 0

    # For plotting
    VOC_est = np.zeros_like(I)
    R_est = np.zeros_like(I)
    error_hist = np.zeros_like(I)
    v_est = np.zeros_like(I)
    داخلی_resistance_stable = np.zeros_like(I)
    داخلی_resistance_stable[-1] = 0.005
    cov_norm = np.zeros_like(I)
    r_cov = np.zeros_like(I)
    ocv_cov = np.zeros_like(I)
    mixed_cov = np.zeros_like(I)
    داده_cov_hist = np.zeros_like(I)
    gamma_voc_hist = np.zeros_like(I)
    gamma_r_hist = np.zeros_like(I)

    لكل شاخص في دامنه(طول(I)):
        # RLS algorithm
        x = np.array([[1.0], [-I[شاخص]]]) # Input vector
        theta, P, error, داده_cov, gamma_voc_hist[شاخص], gamma_r_hist[شاخص] = rls_update(theta, P, x, V[شاخص], I[شاخص], lam) # Run RLS

        # For plotting
        VOC_est[شاخص] = theta[0][0]
        R_est[شاخص] = theta[1][0]
        error_hist[شاخص] = error
        v_est[شاخص] = x.T @ theta
        cov_norm[شاخص] = abs(np.linalg.norm(P))
        ocv_cov[شاخص] = P[0][0]
        r_cov[شاخص] = P[1][1]
        mixed_cov[شاخص] = P[0][1]
        داده_cov_hist[شاخص] = داده_cov
        داخلی_resistance_stable[شاخص] = max(R_est[شاخص]/n_cells, 0.001)

    # Plot data
    چاپ("Internal Resistance mean (per cell): ", np.میانگین(R_est) / n_cells)

    # Summary plot
    جمع_Fig = plt.figure("Battery Estimation with RLS")

    volt = plt.subplot(2, 3, 1)
    volt.plot(timestamps, V, label='Measured voltage')
    volt.plot(timestamps, v_est, label='Estimated voltage')
    volt.plot(timestamps, np.array(V) + np.array(داخلی_resistance_stable) * np.array(I) * n_cells, label='OCV estimate')
    ocv_smoothed = np.convolve(np.array(V) + np.array(داخلی_resistance_stable) * np.array(I) * n_cells, np.ones(30)/30, نما='full')[0:np.اندازه(timestamps)]
    ocv_smoothed[0:30] = ocv_smoothed[31]
    volt.plot(timestamps, ocv_smoothed, label='OCV estimate smoothed')
    volt.plot(timestamps, np.پر_like(I, پر_cell * n_cells), label='100% SOC')
    volt.مجموعه_title("Measured Voltage vs Estimated voltage vs Estimated Open circuit voltage [V]")
    volt.legend()

    intR = plt.subplot(2, 3, 2)
    intR.plot(timestamps, np.array(R_est) * 1000 / n_cells, label='Internal resistance estimate')
    intR.مجموعه_title("Internal resistance estimate (per cell) [mOhm]")
    intR.legend()

    soc = plt.subplot(2, 3, 3)
    soc.plot(timestamps, مانده, label='SoC logged')
    soc.plot(timestamps, np.interp((np.array(V) + np.array(داخلی_resistance_stable) * n_cells * np.array(I)) / n_cells, [خالی_cell, پر_cell], [0, 1]), label='SoC with estimator')
    soc.plot(timestamps, np.interp(ocv_smoothed / n_cells, [خالی_cell, پر_cell], [0, 1]), label='SoC with estimator smoothed')
    soc.مجموعه_title("State of charge")
    soc.legend()

    curr = plt.subplot(2, 3, 4)
    curr.plot(timestamps, I, label='Measured current')
    curr.مجموعه_title("Measured Current [A]")
    curr.legend()

    err = plt.subplot(2, 3, 5)
    err.plot(timestamps, error_hist, label='$Error$')
    err.مجموعه_title("Voltage estimation error [V]")
    err.legend()

    cov = plt.subplot(2, 3, 6)
    cov.plot(timestamps, cov_norm, label = 'Covariance norm')
    cov.مجموعه_title("Covariance norm")
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

إذا __name__ == "__main__":
    parser = argparse.آرگومان_Parser(description='Estimate battery parameters from ulog file.')
    parser.جمع_کن_آرگومان('-f', type = str,   required = درست,  help = 'Full path to ulog file')
    parser.جمع_کن_آرگومان('-c', type = عائم, required = خطأ, help = 'Number of cells in battery')
    parser.جمع_کن_آرگومان('-u', type = عائم, required = خطأ, افتراضي = هیچ, help = 'Full cell voltage')
    parser.جمع_کن_آرگومان('-e', type = عائم, required = خطأ, افتراضي = هیچ,  help = 'Empty cell voltage')
    parser.جمع_کن_آرگومان('-l', type = عائم, required = خطأ, افتراضي = 0.99, help = 'Forgetting factor')
    آرگومانها = parser.parse_آرگومانها()
    اصلی(آرگومانها.f, آرگومانها.c, آرگومانها.u, آرگومانها.e, آرگومانها.l)

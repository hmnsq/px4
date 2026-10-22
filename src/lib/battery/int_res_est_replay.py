# Test internal resistance estimator on flight logs
# run with:
# python3 int_res_est_replay.py -f <pathToLogFile> -c <#batteryCells>
#   -u <(optional)fullCellVoltage> -e <(optional)emptyCellVoltage> -l <(optional)forgettingFactor> -d <(optional)filterMeasurements>
# Note: Can lead to slightly different results than the online estimation due to the fact that
# the log frequency of the voltage and current are not the same as the online frequency.

তঃ pyulog আমদানি ULog
আমদানি matplotlib.pyplot ইব plt
আমদানি numpy ইব np
আমদানি argparse

def get_তথ্য(log, topic_name, variable_name, instance=0):
    জন্য elem মধ্যে log.তথ্য_list:
        যদি elem.name == topic_name এবং instance == elem.multi_id:
            ফেরত elem.তথ্য[variable_name]
    ফেরত np.array([])

def us2s(time_us):
    ফেরত time_us * 1e-6

def getParam(log, param_name):
    যদি param_name মধ্যে log.initial_parameters:
        ফেরত log.initial_parameters[param_name]
    অন্যথা:
        print(f"Parameter {param_name} not found in log.")
    ফেরত None

def rls_update(theta, P, x, V, I, lam):
    gamma = P @ x / (lam + x.T @ P @ x)
    error = V - x.T @ theta
    তথ্য_cov = x.T @ P @ x
    theta_অস্থায়ী = theta + gamma * error
    P_অস্থায়ী = (P - gamma @ x.T @ P) / lam
    যদি (abs(np.linalg.norm(P)) < abs(np.linalg.norm(P_অস্থায়ী))):
        theta_corr = np.array([V + theta[1] * I, theta[1]]) # Correct OCV estimation
        P_corr = P
        ফেরত theta_corr, P_corr, error, তথ্য_cov, 0, 0
    ফেরত theta_অস্থায়ী, P_অস্থায়ী, error, তথ্য_cov, gamma[0], gamma[1]

def প্রধান(log_name, n_cells, full_cell, empty_cell, lam):
    log = ULog(log_name)

    log_n_cells = getParam(log, 'BAT1_N_CELLS')
    log_full_cell = getParam(log, 'BAT1_V_CHARGED')
    log_empty_cell = getParam(log, 'BAT1_V_EMPTY')

    # Debug information
    print(f"Extracted from log - BAT1_N_CELLS: {log_n_cells}, BAT1_V_CHARGED: {log_full_cell}, BAT1_V_EMPTY: {log_empty_cell}")

    # Use log parameters unless overridden
    যদি n_cells is None:
        n_cells = log_n_cells
    অন্যথা:
        print(f"Using override for n_cells: {n_cells}")
    যদি full_cell is None:
        full_cell = log_full_cell
    অন্যথা:
        print(f"Using override for full_cell: {full_cell}")
    যদি empty_cell is None:
        empty_cell = log_empty_cell
    অন্যথা:
        print(f"Using override for empty_cell: {empty_cell}")

    # Debug information for final parameter values
    print(f"Using parameters - n_cells: {n_cells}, full_cell: {full_cell}, empty_cell: {empty_cell}")

    timestamps = us2s(get_তথ্য(log, 'battery_status', 'timestamp'))
    I = get_তথ্য(log, 'battery_status', 'current_a')
    V = get_তথ্য(log, 'battery_status', 'voltage_v')
    remaining = get_তথ্য(log, 'battery_status', 'remaining')

    যদি নয় timestamps.আকার_2 অথবা নয় I.আকার_2 অথবা নয় V.আকার_2 অথবা নয় remaining.আকার_2:
        print("Error: Incomplete data.")
        ফেরত

    # Initializations
    theta = np.array([[V[0] + 0.005 * n_cells * I[0]], [0.005 * n_cells]])  # Initial VOC and R
    P = np.diag([1.2 * n_cells, 0.1 * n_cells]) # Initial covariance
    error = 0

    # For plotting
    VOC_est = np.zeros_like(I)
    R_est = np.zeros_like(I)
    error_hist = np.zeros_like(I)
    v_est = np.zeros_like(I)
    internal_resistance_stable = np.zeros_like(I)
    internal_resistance_stable[-1] = 0.005
    cov_norm = np.zeros_like(I)
    r_cov = np.zeros_like(I)
    ocv_cov = np.zeros_like(I)
    mixed_cov = np.zeros_like(I)
    তথ্য_cov_hist = np.zeros_like(I)
    gamma_voc_hist = np.zeros_like(I)
    gamma_r_hist = np.zeros_like(I)

    জন্য সূচক মধ্যে range(len(I)):
        # RLS algorithm
        x = np.array([[1.0], [-I[সূচক]]]) # Input vector
        theta, P, error, তথ্য_cov, gamma_voc_hist[সূচক], gamma_r_hist[সূচক] = rls_update(theta, P, x, V[সূচক], I[সূচক], lam) # Run RLS

        # For plotting
        VOC_est[সূচক] = theta[0][0]
        R_est[সূচক] = theta[1][0]
        error_hist[সূচক] = error
        v_est[সূচক] = x.T @ theta
        cov_norm[সূচক] = abs(np.linalg.norm(P))
        ocv_cov[সূচক] = P[0][0]
        r_cov[সূচক] = P[1][1]
        mixed_cov[সূচক] = P[0][1]
        তথ্য_cov_hist[সূচক] = তথ্য_cov
        internal_resistance_stable[সূচক] = max(R_est[সূচক]/n_cells, 0.001)

    # Plot data
    print("Internal Resistance mean (per cell): ", np.mean(R_est) / n_cells)

    # Summary plot
    যোগ_Fig = plt.figure("Battery Estimation with RLS")

    volt = plt.subplot(2, 3, 1)
    volt.plot(timestamps, V, label='Measured voltage')
    volt.plot(timestamps, v_est, label='Estimated voltage')
    volt.plot(timestamps, np.array(V) + np.array(internal_resistance_stable) * np.array(I) * n_cells, label='OCV estimate')
    ocv_smoothed = np.convolve(np.array(V) + np.array(internal_resistance_stable) * np.array(I) * n_cells, np.ones(30)/30, mode='full')[0:np.আকার_2(timestamps)]
    ocv_smoothed[0:30] = ocv_smoothed[31]
    volt.plot(timestamps, ocv_smoothed, label='OCV estimate smoothed')
    volt.plot(timestamps, np.full_like(I, full_cell * n_cells), label='100% SOC')
    volt.set_title("Measured Voltage vs Estimated voltage vs Estimated Open circuit voltage [V]")
    volt.legend()

    intR = plt.subplot(2, 3, 2)
    intR.plot(timestamps, np.array(R_est) * 1000 / n_cells, label='Internal resistance estimate')
    intR.set_title("Internal resistance estimate (per cell) [mOhm]")
    intR.legend()

    soc = plt.subplot(2, 3, 3)
    soc.plot(timestamps, remaining, label='SoC logged')
    soc.plot(timestamps, np.interp((np.array(V) + np.array(internal_resistance_stable) * n_cells * np.array(I)) / n_cells, [empty_cell, full_cell], [0, 1]), label='SoC with estimator')
    soc.plot(timestamps, np.interp(ocv_smoothed / n_cells, [empty_cell, full_cell], [0, 1]), label='SoC with estimator smoothed')
    soc.set_title("State of charge")
    soc.legend()

    curr = plt.subplot(2, 3, 4)
    curr.plot(timestamps, I, label='Measured current')
    curr.set_title("Measured Current [A]")
    curr.legend()

    err = plt.subplot(2, 3, 5)
    err.plot(timestamps, error_hist, label='$Error$')
    err.set_title("Voltage estimation error [V]")
    err.legend()

    cov = plt.subplot(2, 3, 6)
    cov.plot(timestamps, cov_norm, label = 'Covariance norm')
    cov.set_title("Covariance norm")
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

যদি __name__ == "__main__":
    parser = argparse.ArgumentParser(description='Estimate battery parameters from ulog file.')
    parser.add_argument('-f', type = str,   required = সত্য,  help = 'Full path to ulog file')
    parser.add_argument('-c', type = দশমিক, required = মিথ্যা, help = 'Number of cells in battery')
    parser.add_argument('-u', type = দশমিক, required = মিথ্যা, পূর্বনির্ধারিত = None, help = 'Full cell voltage')
    parser.add_argument('-e', type = দশমিক, required = মিথ্যা, পূর্বনির্ধারিত = None,  help = 'Empty cell voltage')
    parser.add_argument('-l', type = দশমিক, required = মিথ্যা, পূর্বনির্ধারিত = 0.99, help = 'Forgetting factor')
    args = parser.parse_args()
    প্রধান(args.f, args.c, args.u, args.e, args.l)

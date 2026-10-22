#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
    Copyright (c) 2022 PX4 Development Team
    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions
    are met:

    1. Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in
    the documentation and/or other materials provided with the
    distribution.
    3. Neither the name PX4 nor the names of its contributors may be
    used to endorse or promote products derived from this software
    without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
    "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
    LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
    FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
    COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
    INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
    BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
    OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
    AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
    LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
    ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
    POSSIBILITY OF SUCH DAMAGE.

File: wind_estimator_replay.py
Author: Mathieu Bresciani <mathieu@auterion.com>
License: BSD 3-Clause
Description:
"""

استورد matplotlib.pylab كـ plt
من pyulog استورد ULog
استورد numpy كـ np

من generated.fuse_airspeed استورد fuse_airspeed

تعریف get_انکڑے(log, topic_name, variable_name, instance=0):
    variable_انکڑے = np.array([])
    لكل elem في log.انکڑے_list:
        إذا elem.name == topic_name:
            إذا instance == elem.multi_id:
                variable_انکڑے = elem.انکڑے[variable_name]
                اكسر

    أرجع variable_انکڑے

تعریف us2s(ویلا_ms):
    أرجع ویلا_ms * 1e-6

تعریف لگاتار_2(logfile, use_gnss, scale_init):
    log = ULog(logfile)

    إذا use_gnss و any(elem.name == 'vehicle_gnss' لكل elem في log.انکڑے_list):
        v_local = np.array([get_انکڑے(log, 'vehicle_gnss', 'receiver.vel_north'),
                  get_انکڑے(log, 'vehicle_gnss', 'receiver.vel_east'),
                  get_انکڑے(log, 'vehicle_gnss', 'receiver.vel_down')])
        t_v_local = us2s(get_انکڑے(log, 'vehicle_gnss', 'timestamp'))

    وإلاإذا use_gnss:
        # logs recorded before the rename to vehicle_gnss
        v_local = np.array([get_انکڑے(log, 'vehicle_gps_position', 'vel_n_m_s'),
                  get_انکڑے(log, 'vehicle_gps_position', 'vel_e_m_s'),
                  get_انکڑے(log, 'vehicle_gps_position', 'vel_d_m_s')])
        t_v_local = us2s(get_انکڑے(log, 'vehicle_gps_position', 'timestamp'))

    وإلا:
        v_local = np.array([get_انکڑے(log, 'vehicle_local_position', 'vx'),
                  get_انکڑے(log, 'vehicle_local_position', 'vy'),
                  get_انکڑے(log, 'vehicle_local_position', 'vz')])
        t_v_local = us2s(get_انکڑے(log, 'vehicle_local_position', 'timestamp'))

    true_airspeed = get_انکڑے(log, 'airspeed', 'true_airspeed_m_s')
    t_true_airspeed = us2s(get_انکڑے(log, 'airspeed', 'timestamp'))

    dist_bottom = get_انکڑے(log, 'vehicle_local_position', 'dist_bottom')
    t_dist_bottom = us2s(get_انکڑے(log, 'vehicle_local_position', 'timestamp'))

    إذا scale_init اے کجھ_نئیں:
        scale_init = 1.0

    # The estimator estimates the inverse scale factor to have a simpler measurement jacobian
    inverse_scale_init = 1 / scale_init
    state = np.array([0.0, 0.0, inverse_scale_init])
    P = np.diag([1.0, 1.0, 1e-4])
    wind_nsd = 1e-2
    scale_nsd = 1e-4
    Q = np.diag([wind_nsd**2, wind_nsd**2, scale_nsd**2])
    R = 1.4**2
    epsilon = 1e-8
    t_now = t_v_local[0]

    n = لمبائی(t_v_local)
    wind_est_n = np.zeros(n)
    wind_est_e = np.zeros(n)
    scale_est = np.zeros(n)
    i_airspeed = 0
    i_dist_bottom = 0

    لكل i في سلسلہ(n):
        dt = t_v_local[i] - t_now
        t_now = t_v_local[i] # run on local position updates

        طالما i_dist_bottom < لمبائی(t_dist_bottom) و t_dist_bottom[i_dist_bottom] <= t_now:
            i_dist_bottom += 1
        i_dist_bottom -= 1

        إذا dist_bottom[i_dist_bottom] > 20.0: # Don't start too low

            P += Q * dt

            إذا i_airspeed < لمبائی(t_true_airspeed) و t_true_airspeed[i_airspeed] < t_now:
                طالما i_airspeed < لمبائی(t_true_airspeed) و t_true_airspeed[i_airspeed] < t_now:
                    i_airspeed += 1
                i_airspeed -= 1

                (H, K, innov_var, innov) = fuse_airspeed(np.asarray(v_local[:,i]), state, P, true_airspeed[i_airspeed], R, epsilon)
                state += np.array(K) * innov
                P -= K * H * P
                i_airspeed += 1

        wind_est_n[i] = state[0]
        wind_est_e[i] = state[1]
        scale_est[i] = 1 / state[2]

    plt.figure(1)
    ax1 = plt.subplot(2, 1, 1)
    ax1.plot(t_v_local, wind_est_n)
    ax1.plot(t_v_local, wind_est_e)
    ax1.سیٹ_ylabel("wind speed (m/s)")
    ax1.legend(["north", "east"])

    ax2 = plt.subplot(2, 1, 2)
    ax2.plot(t_v_local, scale_est)
    ax2.سیٹ_xlabel("time (s)")
    ax2.سیٹ_ylabel("airspeed scale (-)")
    plt.show()

إذا __name__ == '__main__':
    استورد os
    استورد argparse

    # Get the path of this script (without file name)
    script_رستہ = os.رستہ.ونڈو(os.رستہ.realpath(__file__))[0]

    # Parse arguments
    parser = argparse.دلیل_Parser(
        description='Wind estimator with airspeed scale factor')

    # Provide parameter file path and name
    parser.جوڑو_دلیل('logfile', help='Full ulog file path, name and extension', type=str)
    parser.جوڑو_دلیل('--gnss', help='Use GNSS velocity instead of local velocity estimate',
                        action='store_true')
    parser.جوڑو_دلیل('--scale_init', help='Initial airsped scale factor (1.0 if not specified)', type=عائم)
    دلیلاں = parser.parse_دلیلاں()

    logfile = os.رستہ.abspath(دلیلاں.logfile) # Convert to absolute path

    لگاتار_2(logfile, دلیلاں.gnss, دلیلاں.scale_init)

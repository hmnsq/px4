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

ಆಮದು matplotlib.pylab ಇವ plt
ತಃ pyulog ಆಮದು ULog
ಆಮದು numpy ಇವ np

ತಃ generated.fuse_airspeed ಆಮದು fuse_airspeed

ವ್ಯಾಖ್ಯಾನ get_ದತ್ತಾಂಶ(log, topic_name, variable_name, instance=0):
    variable_ದತ್ತಾಂಶ = np.array([])
    ಗಾಗಿ elem ಒಳಗೆ log.ದತ್ತಾಂಶ_list:
        ಆದರೆ elem.name == topic_name:
            ಆದರೆ instance == elem.multi_id:
                variable_ದತ್ತಾಂಶ = elem.ದತ್ತಾಂಶ[variable_name]
                ನಿಲ್ಲಿಸು

    ಹಿಂತಿರುಗಿ variable_ದತ್ತಾಂಶ

ವ್ಯಾಖ್ಯಾನ us2s(ಸಮಯ_ms):
    ಹಿಂತಿರುಗಿ ಸಮಯ_ms * 1e-6

ವ್ಯಾಖ್ಯಾನ ಸತತ_2(logfile, use_gnss, scale_init):
    log = ULog(logfile)

    ಆದರೆ use_gnss ಮತ್ತು any(elem.name == 'vehicle_gnss' ಗಾಗಿ elem ಒಳಗೆ log.ದತ್ತಾಂಶ_list):
        v_local = np.array([get_ದತ್ತಾಂಶ(log, 'vehicle_gnss', 'receiver.vel_north'),
                  get_ದತ್ತಾಂಶ(log, 'vehicle_gnss', 'receiver.vel_east'),
                  get_ದತ್ತಾಂಶ(log, 'vehicle_gnss', 'receiver.vel_down')])
        t_v_local = us2s(get_ದತ್ತಾಂಶ(log, 'vehicle_gnss', 'timestamp'))

    ಅಥವಾಆದರೆ use_gnss:
        # logs recorded before the rename to vehicle_gnss
        v_local = np.array([get_ದತ್ತಾಂಶ(log, 'vehicle_gps_position', 'vel_n_m_s'),
                  get_ದತ್ತಾಂಶ(log, 'vehicle_gps_position', 'vel_e_m_s'),
                  get_ದತ್ತಾಂಶ(log, 'vehicle_gps_position', 'vel_d_m_s')])
        t_v_local = us2s(get_ದತ್ತಾಂಶ(log, 'vehicle_gps_position', 'timestamp'))

    ಇಲ್ಲದಿದ್ದರೆ:
        v_local = np.array([get_ದತ್ತಾಂಶ(log, 'vehicle_local_position', 'vx'),
                  get_ದತ್ತಾಂಶ(log, 'vehicle_local_position', 'vy'),
                  get_ದತ್ತಾಂಶ(log, 'vehicle_local_position', 'vz')])
        t_v_local = us2s(get_ದತ್ತಾಂಶ(log, 'vehicle_local_position', 'timestamp'))

    true_airspeed = get_ದತ್ತಾಂಶ(log, 'airspeed', 'true_airspeed_m_s')
    t_true_airspeed = us2s(get_ದತ್ತಾಂಶ(log, 'airspeed', 'timestamp'))

    dist_bottom = get_ದತ್ತಾಂಶ(log, 'vehicle_local_position', 'dist_bottom')
    t_dist_bottom = us2s(get_ದತ್ತಾಂಶ(log, 'vehicle_local_position', 'timestamp'))

    ಆದರೆ scale_init ಆಗಿರುವ ಏನೂಇಲ್ಲ:
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

    n = ಉದ್ದ(t_v_local)
    wind_est_n = np.zeros(n)
    wind_est_e = np.zeros(n)
    scale_est = np.zeros(n)
    i_airspeed = 0
    i_dist_bottom = 0

    ಗಾಗಿ i ಒಳಗೆ ಶ್ರೇಣಿ(n):
        dt = t_v_local[i] - t_now
        t_now = t_v_local[i] # run on local position updates

        ಆಗ i_dist_bottom < ಉದ್ದ(t_dist_bottom) ಮತ್ತು t_dist_bottom[i_dist_bottom] <= t_now:
            i_dist_bottom += 1
        i_dist_bottom -= 1

        ಆದರೆ dist_bottom[i_dist_bottom] > 20.0: # Don't start too low

            P += Q * dt

            ಆದರೆ i_airspeed < ಉದ್ದ(t_true_airspeed) ಮತ್ತು t_true_airspeed[i_airspeed] < t_now:
                ಆಗ i_airspeed < ಉದ್ದ(t_true_airspeed) ಮತ್ತು t_true_airspeed[i_airspeed] < t_now:
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
    ax1.ಗಣ_ylabel("wind speed (m/s)")
    ax1.legend(["north", "east"])

    ax2 = plt.subplot(2, 1, 2)
    ax2.plot(t_v_local, scale_est)
    ax2.ಗಣ_xlabel("time (s)")
    ax2.ಗಣ_ylabel("airspeed scale (-)")
    plt.show()

ಆದರೆ __name__ == '__main__':
    ಆಮದು os
    ಆಮದು argparse

    # Get the path of this script (without file name)
    script_ಮಾರ್ಗ = os.ಮಾರ್ಗ.ವಿಭಜಿಸು(os.ಮಾರ್ಗ.realpath(__file__))[0]

    # Parse arguments
    parser = argparse.ವಾದ_Parser(
        description='Wind estimator with airspeed scale factor')

    # Provide parameter file path and name
    parser.ಕೂಡಿಸು_ವಾದ('logfile', help='Full ulog file path, name and extension', type=str)
    parser.ಕೂಡಿಸು_ವಾದ('--gnss', help='Use GNSS velocity instead of local velocity estimate',
                        action='store_true')
    parser.ಕೂಡಿಸು_ವಾದ('--scale_init', help='Initial airsped scale factor (1.0 if not specified)', type=ದಶಮಾಂಶ)
    ವಾದಗಳು = parser.parse_ವಾದಗಳು()

    logfile = os.ಮಾರ್ಗ.abspath(ವಾದಗಳು.logfile) # Convert to absolute path

    ಸತತ_2(logfile, ವಾದಗಳು.gnss, ವಾದಗಳು.scale_init)

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

దిగుమతి matplotlib.pylab ఇవ plt
తః pyulog దిగుమతి ULog
దిగుమతి numpy ఇవ np

తః generated.fuse_airspeed దిగుమతి fuse_airspeed

నిర్వచనం get_దత్తాంశం(log, topic_name, variable_name, instance=0):
    variable_దత్తాంశం = np.array([])
    కొరకు elem లో log.దత్తాంశం_list:
        ఒకవేళ elem.name == topic_name:
            ఒకవేళ instance == elem.multi_id:
                variable_దత్తాంశం = elem.దత్తాంశం[variable_name]
                ఆపు

    తిరిగి variable_దత్తాంశం

నిర్వచనం us2s(సమయం_ms):
    తిరిగి సమయం_ms * 1e-6

నిర్వచనం నిరంతరం_2(logfile, use_gnss, scale_init):
    log = ULog(logfile)

    ఒకవేళ use_gnss మరియు any(elem.name == 'vehicle_gnss' కొరకు elem లో log.దత్తాంశం_list):
        v_local = np.array([get_దత్తాంశం(log, 'vehicle_gnss', 'receiver.vel_north'),
                  get_దత్తాంశం(log, 'vehicle_gnss', 'receiver.vel_east'),
                  get_దత్తాంశం(log, 'vehicle_gnss', 'receiver.vel_down')])
        t_v_local = us2s(get_దత్తాంశం(log, 'vehicle_gnss', 'timestamp'))

    లేదావేళ use_gnss:
        # logs recorded before the rename to vehicle_gnss
        v_local = np.array([get_దత్తాంశం(log, 'vehicle_gps_position', 'vel_n_m_s'),
                  get_దత్తాంశం(log, 'vehicle_gps_position', 'vel_e_m_s'),
                  get_దత్తాంశం(log, 'vehicle_gps_position', 'vel_d_m_s')])
        t_v_local = us2s(get_దత్తాంశం(log, 'vehicle_gps_position', 'timestamp'))

    లేకపోతే:
        v_local = np.array([get_దత్తాంశం(log, 'vehicle_local_position', 'vx'),
                  get_దత్తాంశం(log, 'vehicle_local_position', 'vy'),
                  get_దత్తాంశం(log, 'vehicle_local_position', 'vz')])
        t_v_local = us2s(get_దత్తాంశం(log, 'vehicle_local_position', 'timestamp'))

    true_airspeed = get_దత్తాంశం(log, 'airspeed', 'true_airspeed_m_s')
    t_true_airspeed = us2s(get_దత్తాంశం(log, 'airspeed', 'timestamp'))

    dist_bottom = get_దత్తాంశం(log, 'vehicle_local_position', 'dist_bottom')
    t_dist_bottom = us2s(get_దత్తాంశం(log, 'vehicle_local_position', 'timestamp'))

    ఒకవేళ scale_init అయిన ఏమీలేదు:
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

    n = పొడవు(t_v_local)
    wind_est_n = np.zeros(n)
    wind_est_e = np.zeros(n)
    scale_est = np.zeros(n)
    i_airspeed = 0
    i_dist_bottom = 0

    కొరకు i లో శ్రేణి(n):
        dt = t_v_local[i] - t_now
        t_now = t_v_local[i] # run on local position updates

        అయితే i_dist_bottom < పొడవు(t_dist_bottom) మరియు t_dist_bottom[i_dist_bottom] <= t_now:
            i_dist_bottom += 1
        i_dist_bottom -= 1

        ఒకవేళ dist_bottom[i_dist_bottom] > 20.0: # Don't start too low

            P += Q * dt

            ఒకవేళ i_airspeed < పొడవు(t_true_airspeed) మరియు t_true_airspeed[i_airspeed] < t_now:
                అయితే i_airspeed < పొడవు(t_true_airspeed) మరియు t_true_airspeed[i_airspeed] < t_now:
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
    ax1.సమితి_ylabel("wind speed (m/s)")
    ax1.legend(["north", "east"])

    ax2 = plt.subplot(2, 1, 2)
    ax2.plot(t_v_local, scale_est)
    ax2.సమితి_xlabel("time (s)")
    ax2.సమితి_ylabel("airspeed scale (-)")
    plt.show()

ఒకవేళ __name__ == '__main__':
    దిగుమతి os
    దిగుమతి argparse

    # Get the path of this script (without file name)
    script_మార్గం = os.మార్గం.విభజించు(os.మార్గం.realpath(__file__))[0]

    # Parse arguments
    parser = argparse.వాదం_Parser(
        description='Wind estimator with airspeed scale factor')

    # Provide parameter file path and name
    parser.కూడు_వాదం('logfile', help='Full ulog file path, name and extension', type=str)
    parser.కూడు_వాదం('--gnss', help='Use GNSS velocity instead of local velocity estimate',
                        action='store_true')
    parser.కూడు_వాదం('--scale_init', help='Initial airsped scale factor (1.0 if not specified)', type=దశాంశం)
    వాదనలు = parser.parse_వాదనలు()

    logfile = os.మార్గం.abspath(వాదనలు.logfile) # Convert to absolute path

    నిరంతరం_2(logfile, వాదనలు.gnss, వాదనలు.scale_init)

/****************************************************************************
 *
 *   Copyright (c) 2015 Roman Bapst. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 3. Neither the name PX4 nor the names of its contributors may be
 *    used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 ****************************************************************************/

/**
 * @file terrain_estimator.h
 */

#pragma once

#ଅନ୍ତର୍ଭାଵ <lib/mathlib/mathlib.h>
#ଅନ୍ତର୍ଭାଵ <matrix/math.hpp>
#ଅନ୍ତର୍ଭାଵ <uORB/topics/sensor_combined.h>
#ଅନ୍ତର୍ଭାଵ <uORB/topics/sensor_gnss.h>
#ଅନ୍ତର୍ଭାଵ <uORB/topics/vehicle_attitude.h>
#ଅନ୍ତର୍ଭାଵ <uORB/topics/distance_sensor.h>


/*
* This class can be used to estimate distance to the ground using a laser range finder.
* It's assumed that the laser points down vertically if the vehicle is in it's neutral pose.
* The predict(...) function will do a state prediciton based on accelerometer inputs. It also
* considers accelerometer bias.
* The measurement_update(...) function does a measurement update based on range finder and gps
* velocity measurements. Both functions should always be called together when there is new
* acceleration data available.
* The is_valid() function provides information whether the estimate is valid.
*/

class __EXPORT TerrainEstimator
{
ସର୍ବସାଧାରଣ:
	TerrainEstimator();
	~TerrainEstimator() = ପୂର୍ବନିର୍ଦ୍ଧାରିତ;

	bool is_valid() {ଫେରନ୍ତୁ _terrain_valid;}
	ଦଶମିକ get_distance_to_ground() {ଫେରନ୍ତୁ -_x(0);}
	ଦଶମିକ get_velocity() {ଫେରନ୍ତୁ _x(1);}

	ଶୂନ୍ୟ predict(ଦଶମିକ dt, ଅଚଳ ସଂରଚନା vehicle_attitude_s *attitude, ଅଚଳ ସଂରଚନା sensor_combined_s *sensor,
		     ଅଚଳ ସଂରଚନା distance_sensor_s *distance);
	ଶୂନ୍ୟ measurement_update(uint64_t time_ref, ଅଚଳ ସଂରଚନା sensor_gnss_s *gnss,
				ଅଚଳ ସଂରଚନା distance_sensor_s *distance,
				ଅଚଳ ସଂରଚନା vehicle_attitude_s *attitude);

ବ୍ୟକ୍ତିଗତ:
	ଗଣନା {n_x = 3};

	ଦଶମିକ _distance_last;
	bool _terrain_valid;

	// kalman filter variables
	matrix::Vector<ଦଶମିକ, n_x> _x;		// state: ground distance, velocity, accel bias in z direction
	ଦଶମିକ  _u_z;			// acceleration in earth z direction
	matrix::Matrix<ଦଶମିକ, 3, 3> _P;	// covariance matrix

	// timestamps
	uint64_t _time_last_distance;
	uint64_t _time_last_gnss;

	/*
	struct {
		float var_acc_z;
		float var_p_z;
		float var_p_vz;
		float var_lidar;
		float var_gps_vz;
	} _params;
	*/

	bool is_distance_valid(ଦଶମିକ distance);

};

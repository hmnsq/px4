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

#অন্তর্ভাব <lib/mathlib/mathlib.h>
#অন্তর্ভাব <matrix/math.hpp>
#অন্তর্ভাব <uORB/topics/sensor_combined.h>
#অন্তর্ভাব <uORB/topics/sensor_gnss.h>
#অন্তর্ভাব <uORB/topics/vehicle_attitude.h>
#অন্তর্ভাব <uORB/topics/distance_sensor.h>


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
সর্বজনীন:
	TerrainEstimator();
	~TerrainEstimator() = পূর্বনির্ধারিত;

	bool is_valid() {ফেরত _terrain_valid;}
	দশমিক get_distance_to_ground() {ফেরত -_x(0);}
	দশমিক get_velocity() {ফেরত _x(1);}

	শূন্য predict(দশমিক dt, ধ্রুবক কাঠামো vehicle_attitude_s *attitude, ধ্রুবক কাঠামো sensor_combined_s *sensor,
		     ধ্রুবক কাঠামো distance_sensor_s *distance);
	শূন্য measurement_update(uint64_t time_ref, ধ্রুবক কাঠামো sensor_gnss_s *gnss,
				ধ্রুবক কাঠামো distance_sensor_s *distance,
				ধ্রুবক কাঠামো vehicle_attitude_s *attitude);

ব্যক্তিগত:
	গণনা {n_x = 3};

	দশমিক _distance_last;
	bool _terrain_valid;

	// kalman filter variables
	matrix::Vector<দশমিক, n_x> _x;		// state: ground distance, velocity, accel bias in z direction
	দশমিক  _u_z;			// acceleration in earth z direction
	matrix::Matrix<দশমিক, 3, 3> _P;	// covariance matrix

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

	bool is_distance_valid(দশমিক distance);

};

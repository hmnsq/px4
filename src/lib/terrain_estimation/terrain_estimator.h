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

#هدايت once

#ضمن <lib/mathlib/mathlib.h>
#ضمن <matrix/math.hpp>
#ضمن <uORB/topics/sensor_combined.h>
#ضمن <uORB/topics/sensor_gnss.h>
#ضمن <uORB/topics/vehicle_attitude.h>
#ضمن <uORB/topics/distance_sensor.h>


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
عام:
	TerrainEstimator();
	~TerrainEstimator() = افتراضي;

	bool is_valid() {أرجع _terrain_valid;}
	عائم get_مفاصلو_منزل_ground() {أرجع -_x(0);}
	عائم get_velocity() {أرجع _x(1);}

	فراغ predict(عائم dt, ثابت بنية vehicle_attitude_s *attitude, ثابت بنية sensor_combined_s *sensor,
		     ثابت بنية مفاصلو_sensor_s *مفاصلو);
	فراغ measurement_update(uint64_t وقت_ref, ثابت بنية sensor_gnss_s *gnss,
				ثابت بنية مفاصلو_sensor_s *مفاصلو,
				ثابت بنية vehicle_attitude_s *attitude);

خاص:
	تعداد {n_x = 3};

	عائم مفاصلو_آخري;
	bool _terrain_valid;

	// kalman filter variables
	ميٽرڪس::ويڪٽر<عائم, n_x> _x;		// state: ground distance, velocity, accel bias in z direction
	عائم  _u_z;			// acceleration in earth z direction
	ميٽرڪس::ميٽرڪس_2<عائم, 3, 3> _P;	// covariance matrix

	// timestamps
	uint64_t وقت_آخري_مفاصلو;
	uint64_t وقت_آخري_gnss;

	/*
	struct {
		float var_acc_z;
		float var_p_z;
		float var_p_vz;
		float var_lidar;
		float var_gps_vz;
	} _params;
	*/

	bool is_مفاصلو_valid(عائم مفاصلو);

};

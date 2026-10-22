/****************************************************************************
 *
 *   Copyright (c) 2018-2024 PX4 Development Team. All rights reserved.
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
 * @file CollisionPrevention.hpp
 * @author Tanja Baumann <tanja@auterion.com>
 * @author Claudio Chies <claudio@chies.com>
 *
 * CollisionPrevention controller.
 *
 */

#pragma once

#अन्तर्भाव <float.h>

#अन्तर्भाव <commander/px4_custom_mode.h>
#अन्तर्भाव <drivers/drv_hrt.h>
#अन्तर्भाव <mathlib/mathlib.h>
#अन्तर्भाव <matrix/matrix/math.hpp>
#अन्तर्भाव <px4_platform_common/module_params.h>
#अन्तर्भाव <systemlib/mavlink_log.h>
#अन्तर्भाव <uORB/Publication.hpp>
#अन्तर्भाव <uORB/Subscription.hpp>
#अन्तर्भाव <uORB/SubscriptionMultiArray.hpp>
#अन्तर्भाव <uORB/topics/collision_constraints.h>
#अन्तर्भाव <uORB/topics/distance_sensor.h>
#अन्तर्भाव <uORB/topics/mavlink_log.h>
#अन्तर्भाव <uORB/topics/obstacle_distance.h>
#अन्तर्भाव <uORB/topics/vehicle_attitude.h>
#अन्तर्भाव <uORB/topics/vehicle_command.h>

using नामाकाश time_literals;

वर्ग CollisionPrevention : सार्वजनिक ModuleParams
{
सार्वजनिक:
	CollisionPrevention(ModuleParams *parent);
	~CollisionPrevention() override = पूर्वनिर्धारित;

	/**
	 * Returns true if Collision Prevention is running
	 */
	तार्किक is_active();

	/**
	 * Computes collision free setpoints
	 * @param setpoint_accel setpoint purely based on sticks, to be modified
	 * @param setpoint_vel current velocity setpoint as information to be able to stop in time, does not get changed
	 */
	शून्य modifySetpoint(matrix::Vector2f &setpoint_accel, स्थिरांक matrix::Vector2f &setpoint_vel);

	स्थिर constexpr पूर्णांक BIN_मोजणी =
		आकार(obstacle_distance_s::distances) / आकार(obstacle_distance_s::distances[0]); // 72
	स्थिर constexpr पूर्णांक BIN_SIZE = 360 / BIN_मोजणी; // cannot be lower than 5 degrees, should divide 360 evenly

संरक्षित:
	/** Aggregates the sensor data into an internal obstacle map in body frame */
	शून्य _updateObstacleMap();

	/** Updates the obstacle data based on stale data and calculates values from the map */
	शून्य _updateObstacleData();

	/** Calculate the constrained setpoint considering the current obstacle distances, acceleration setpoint and velocity setpoint */
	शून्य _calculateConstrainedSetpoint(matrix::Vector2f &setpoint_accel, स्थिरांक matrix::Vector2f &setpoint_vel);

	obstacle_distance_s _obstacle_map_body_frame{};
	तार्किक _data_fov[BIN_मोजणी] {};
	uint64_t _data_timestamps[BIN_मोजणी] {};
	uint16_t _data_maxranges[BIN_मोजणी] {}; /**< in cm */

	शून्य _addDistanceSensorData(distance_sensor_s &distance_sensor, स्थिरांक matrix::Quatf &vehicle_attitude);

	/**
	 * Updates obstacle distance message with measurement from offboard
	 * @param obstacle, obstacle_distance message to be updated
	 */
	शून्य _addObstacleSensorData(स्थिरांक obstacle_distance_s &obstacle, स्थिरांक दशांश vehicle_yaw);

	/**
	 * Computes an adaption to the setpoint direction to guide towards free space
	 * @param setpoint_dir, setpoint direction before collision prevention intervention
	 * @param setpoint_index, index of the setpoint in the internal obstacle map
	 * @param vehicle_yaw_angle_rad, vehicle orientation
	 */
	शून्य _adaptSetpointDirection(matrix::Vector2f &setpoint_dir, पूर्णांक &setpoint_index, दशांश vehicle_yaw_angle_rad);

	/**
	 * Constrain the acceleration setpoint based on the distance to the obstacle
	 * The Scaling of the acceleration setpoint is linear below the min_dist_to_keep and quadratic until the scale_distance above
	 *           +1          ________ _ _
	 * ┌─┐      │           //
	 * │X│      │          //
	 * │X│      │         //
	 * │X│      │       ///
	 * │X│      │     //
	 * │X│      │/////
	 * │X│──────┼─────────────┬─────────────
	 * │X│     /│             scale_distance
	 * │X│    / │
	 * │X│   /  │
	 * │X│  /   │
	 * │X│ /    │
	 * └─┘/     │
	 *           -1
	 */
	matrix::Vector2f _constrainAccelerationSetpoint(स्थिरांक दशांश &setpoint_length);

	शून्य _getVelocityCompensationAcceleration(स्थिरांक दशांश vehicle_yaw_angle_rad, स्थिरांक matrix::Vector2f &setpoint_vel,
			स्थिरांक hrt_abstime now, दशांश &vel_comp_accel, matrix::Vector2f &vel_comp_accel_dir);

	दशांश _getObstacleDistance(स्थिरांक matrix::Vector2f &direction);

	दशांश _getScale(स्थिरांक दशांश &reference_distance);

	/**
	 * Determines whether a new sensor measurement is used
	 * @param map_index, index of the bin in the internal map the measurement belongs in
	 * @param sensor_range, max range of the sensor in meters
	 * @param sensor_reading, distance measurement in meters
	 */
	तार्किक _enterData(पूर्णांक map_index, दशांश sensor_range, दशांश sensor_reading);

	तार्किक _checkSetpointDirectionFeasability();

	शून्य _transformSetpoint(स्थिरांक matrix::Vector2f &setpoint);


	//Timing functions. Necessary to mock time in the tests
	आभासी hrt_abstime getTime();
	आभासी hrt_abstime getElapsedTime(स्थिरांक hrt_abstime *ptr);

खाजगी:
	तार्किक _data_stale{सत्य}; 		/**< states if the data is stale */
	तार्किक _was_active{असत्य};		/**< states if the collision prevention interferes with the user input */
	तार्किक _obstacle_data_present{असत्य};	/**< states if obstacle data is present */

	पूर्णांक _setpoint_index{};			/**< index of the setpoint*/
	matrix::Vector2f _setpoint_dir{};		/**< direction of the setpoint*/

	दशांश _closest_dist{};			/**< closest distance to an obstacle  */
	matrix::Vector2f _closest_dist_dir{न_अंक, न_अंक};	/**< direction of the closest obstacle  */

	दशांश _min_dist_to_keep{};

	orb_advert_t _mavlink_log_pub{nullptr};	 	/**< Mavlink log uORB handle */

	uORB::Subscription _vehicle_attitude_sub{ORB_ID(vehicle_attitude)};
	matrix::Quatf _vehicle_attitude{};
	दशांश _vehicle_yaw{0.f};

	uORB::Publication<collision_constraints_s>	_constraints_pub{ORB_ID(collision_constraints)};		/**< constraints publication */
	uORB::Publication<obstacle_distance_s>		_obstacle_distance_fused_pub{ORB_ID(obstacle_distance_fused)};	/**< obstacle_distance publication */
	uORB::Publication<vehicle_command_s>	_vehicle_command_pub{ORB_ID(vehicle_command)};			/**< vehicle command do publication */

	uORB::SubscriptionData<obstacle_distance_s> _sub_obstacle_distance{ORB_ID(obstacle_distance)}; /**< obstacle distances received form a range sensor */
	uORB::SubscriptionMultiArray<distance_sensor_s> _distance_sensor_subs{ORB_ID::distance_sensor};

	स्थिर constexpr uint64_t RANGE_STREAM_TIMEOUT_US{500_ms};
	स्थिर constexpr uint64_t TIMEOUT_HOLD_US{5_s};

	hrt_abstime	_last_timeout_warning{0};
	hrt_abstime	_time_activated{0};

	DEFINE_PARAMETERS(
		(ParamFloat<px4::params::CP_DIST>) _param_cp_dist, 		/**< collision prevention keep minimum distance */
		(ParamFloat<px4::params::CP_DELAY>) _param_cp_delay, 		/**< delay of the range measurement data*/
		(ParamFloat<px4::params::CP_GUIDE_ANG>) _param_cp_guide_ang, 	/**< collision prevention change setpoint angle */
		(ParamBool<px4::params::CP_GO_NO_DATA>) _param_cp_go_no_data, 	/**< movement allowed where no data*/
		(ParamFloat<px4::params::MPC_XY_P>) _param_mpc_xy_p, 		/**< p gain from position controller*/
		(ParamFloat<px4::params::MPC_JERK_MAX>) _param_mpc_jerk_max, 	/**< vehicle maximum jerk*/
		(ParamFloat<px4::params::MPC_ACC_HOR>) _param_mpc_acc_hor, 	/**< vehicle maximum horizontal acceleration*/
		(ParamFloat<px4::params::MPC_XY_VEL_P_ACC>) _param_mpc_xy_vel_p_acc, /**< p gain from velocity controller*/
		(ParamFloat<px4::params::MPC_VEL_MANUAL>) _param_mpc_vel_manual   /**< maximum velocity in manual flight mode*/
	)

	/**
	 * Computes collision free setpoints
	 * @param setpoint, setpoint before collision prevention intervention
	 * @param curr_pos, current vehicle position
	 * @param curr_vel, current vehicle velocity
	 */
	शून्य _calculateConstrainedSetpoint(matrix::Vector2f &setpoint, स्थिरांक matrix::Vector2f &curr_pos,
					   स्थिरांक matrix::Vector2f &curr_vel);

	/**
	 * Publishes collision_constraints message
	 * @param original_setpoint, setpoint before collision prevention intervention
	 * @param adapted_setpoint, collision prevention adaped setpoint
	 */
	शून्य _publishConstrainedSetpoint(स्थिरांक matrix::Vector2f &original_setpoint, स्थिरांक matrix::Vector2f &adapted_setpoint);

	/**
	 * Publishes obstacle_distance message with fused data from offboard and from distance sensors
	 * @param obstacle, obstacle_distance message to be publsihed
	 */
	शून्य _publishObstacleDistance(obstacle_distance_s &obstacle);

	/**
	 * Publishes vehicle command.
	 */
	शून्य _publishVehicleCmdDoLoiter();

};

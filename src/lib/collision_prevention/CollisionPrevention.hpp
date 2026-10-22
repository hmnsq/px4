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

#ضمن <float.h>

#ضمن <commander/px4_custom_mode.h>
#ضمن <drivers/drv_hrt.h>
#ضمن <mathlib/mathlib.h>
#ضمن <matrix/matrix/math.hpp>
#ضمن <px4_platform_common/module_params.h>
#ضمن <systemlib/mavlink_log.h>
#ضمن <uORB/Publication.hpp>
#ضمن <uORB/Subscription.hpp>
#ضمن <uORB/SubscriptionMultiArray.hpp>
#ضمن <uORB/topics/collision_constraints.h>
#ضمن <uORB/topics/distance_sensor.h>
#ضمن <uORB/topics/mavlink_log.h>
#ضمن <uORB/topics/obstacle_distance.h>
#ضمن <uORB/topics/vehicle_attitude.h>
#ضمن <uORB/topics/vehicle_command.h>

using نطاق time_literals;

صنف CollisionPrevention : عام ModuleParams
{
عام:
	CollisionPrevention(ModuleParams *parent);
	~CollisionPrevention() override = افتراضي;

	/**
	 * Returns true if Collision Prevention is running
	 */
	منطقي is_active();

	/**
	 * Computes collision free setpoints
	 * @param setpoint_accel setpoint purely based on sticks, to be modified
	 * @param setpoint_vel current velocity setpoint as information to be able to stop in time, does not get changed
	 */
	فراغ modifySetpoint(matrix::Vector2f &setpoint_accel, ثابت matrix::Vector2f &setpoint_vel);

	ساكن constexpr صحيح BIN_COUNT =
		حجم(obstacle_distance_s::distances) / حجم(obstacle_distance_s::distances[0]); // 72
	ساكن constexpr صحيح BIN_SIZE = 360 / BIN_COUNT; // cannot be lower than 5 degrees, should divide 360 evenly

محمي:
	/** Aggregates the sensor data into an internal obstacle map in body frame */
	فراغ _updateObstacleMap();

	/** Updates the obstacle data based on stale data and calculates values from the map */
	فراغ _updateObstacleData();

	/** Calculate the constrained setpoint considering the current obstacle distances, acceleration setpoint and velocity setpoint */
	فراغ _calculateConstrainedSetpoint(matrix::Vector2f &setpoint_accel, ثابت matrix::Vector2f &setpoint_vel);

	obstacle_distance_s _obstacle_map_body_frame{};
	منطقي _data_fov[BIN_COUNT] {};
	uint64_t _data_timestamps[BIN_COUNT] {};
	uint16_t _data_maxranges[BIN_COUNT] {}; /**< in cm */

	فراغ _addDistanceSensorData(distance_sensor_s &distance_sensor, ثابت matrix::Quatf &vehicle_attitude);

	/**
	 * Updates obstacle distance message with measurement from offboard
	 * @param obstacle, obstacle_distance message to be updated
	 */
	فراغ _addObstacleSensorData(ثابت obstacle_distance_s &obstacle, ثابت عائم vehicle_yaw);

	/**
	 * Computes an adaption to the setpoint direction to guide towards free space
	 * @param setpoint_dir, setpoint direction before collision prevention intervention
	 * @param setpoint_index, index of the setpoint in the internal obstacle map
	 * @param vehicle_yaw_angle_rad, vehicle orientation
	 */
	فراغ _adaptSetpointDirection(matrix::Vector2f &setpoint_dir, صحيح &setpoint_index, عائم vehicle_yaw_angle_rad);

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
	matrix::Vector2f _constrainAccelerationSetpoint(ثابت عائم &setpoint_length);

	فراغ _getVelocityCompensationAcceleration(ثابت عائم vehicle_yaw_angle_rad, ثابت matrix::Vector2f &setpoint_vel,
			ثابت hrt_abstime now, عائم &vel_comp_accel, matrix::Vector2f &vel_comp_accel_dir);

	عائم _getObstacleDistance(ثابت matrix::Vector2f &direction);

	عائم _getScale(ثابت عائم &reference_distance);

	/**
	 * Determines whether a new sensor measurement is used
	 * @param map_index, index of the bin in the internal map the measurement belongs in
	 * @param sensor_range, max range of the sensor in meters
	 * @param sensor_reading, distance measurement in meters
	 */
	منطقي _enterData(صحيح map_index, عائم sensor_range, عائم sensor_reading);

	منطقي _checkSetpointDirectionFeasability();

	فراغ _transformSetpoint(ثابت matrix::Vector2f &setpoint);


	//Timing functions. Necessary to mock time in the tests
	virtual hrt_abstime getTime();
	virtual hrt_abstime getElapsedTime(ثابت hrt_abstime *ptr);

خاص:
	منطقي _data_stale{true}; 		/**< states if the data is stale */
	منطقي _was_active{خطأ};		/**< states if the collision prevention interferes with the user input */
	منطقي _obstacle_data_present{خطأ};	/**< states if obstacle data is present */

	صحيح _setpoint_index{};			/**< index of the setpoint*/
	matrix::Vector2f _setpoint_dir{};		/**< direction of the setpoint*/

	عائم _closest_dist{};			/**< closest distance to an obstacle  */
	matrix::Vector2f _closest_dist_dir{NAN, NAN};	/**< direction of the closest obstacle  */

	عائم _min_dist_to_keep{};

	orb_advert_t _mavlink_log_pub{nullptr};	 	/**< Mavlink log uORB handle */

	uORB::Subscription _vehicle_attitude_sub{ORB_ID(vehicle_attitude)};
	matrix::Quatf _vehicle_attitude{};
	عائم _vehicle_yaw{0.f};

	uORB::Publication<collision_constraints_s>	_constraints_pub{ORB_ID(collision_constraints)};		/**< constraints publication */
	uORB::Publication<obstacle_distance_s>		_obstacle_distance_fused_pub{ORB_ID(obstacle_distance_fused)};	/**< obstacle_distance publication */
	uORB::Publication<vehicle_command_s>	_vehicle_command_pub{ORB_ID(vehicle_command)};			/**< vehicle command do publication */

	uORB::SubscriptionData<obstacle_distance_s> _sub_obstacle_distance{ORB_ID(obstacle_distance)}; /**< obstacle distances received form a range sensor */
	uORB::SubscriptionMultiArray<distance_sensor_s> _distance_sensor_subs{ORB_ID::distance_sensor};

	ساكن constexpr uint64_t RANGE_STREAM_TIMEOUT_US{500_ms};
	ساكن constexpr uint64_t TIMEOUT_HOLD_US{5_s};

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
	فراغ _calculateConstrainedSetpoint(matrix::Vector2f &setpoint, ثابت matrix::Vector2f &curr_pos,
					   ثابت matrix::Vector2f &curr_vel);

	/**
	 * Publishes collision_constraints message
	 * @param original_setpoint, setpoint before collision prevention intervention
	 * @param adapted_setpoint, collision prevention adaped setpoint
	 */
	فراغ _publishConstrainedSetpoint(ثابت matrix::Vector2f &original_setpoint, ثابت matrix::Vector2f &adapted_setpoint);

	/**
	 * Publishes obstacle_distance message with fused data from offboard and from distance sensors
	 * @param obstacle, obstacle_distance message to be publsihed
	 */
	فراغ _publishObstacleDistance(obstacle_distance_s &obstacle);

	/**
	 * Publishes vehicle command.
	 */
	فراغ _publishVehicleCmdDoLoiter();

};

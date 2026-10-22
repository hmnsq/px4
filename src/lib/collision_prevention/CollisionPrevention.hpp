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

#ಅನ್ತರ್ಭಾವ <float.h>

#ಅನ್ತರ್ಭಾವ <commander/px4_custom_mode.h>
#ಅನ್ತರ್ಭಾವ <drivers/drv_hrt.h>
#ಅನ್ತರ್ಭಾವ <mathlib/mathlib.h>
#ಅನ್ತರ್ಭಾವ <matrix/matrix/math.hpp>
#ಅನ್ತರ್ಭಾವ <px4_platform_common/module_params.h>
#ಅನ್ತರ್ಭಾವ <systemlib/mavlink_log.h>
#ಅನ್ತರ್ಭಾವ <uORB/Publication.hpp>
#ಅನ್ತರ್ಭಾವ <uORB/Subscription.hpp>
#ಅನ್ತರ್ಭಾವ <uORB/SubscriptionMultiArray.hpp>
#ಅನ್ತರ್ಭಾವ <uORB/topics/collision_constraints.h>
#ಅನ್ತರ್ಭಾವ <uORB/topics/distance_sensor.h>
#ಅನ್ತರ್ಭಾವ <uORB/topics/mavlink_log.h>
#ಅನ್ತರ್ಭಾವ <uORB/topics/obstacle_distance.h>
#ಅನ್ತರ್ಭಾವ <uORB/topics/vehicle_attitude.h>
#ಅನ್ತರ್ಭಾವ <uORB/topics/vehicle_command.h>

using ನಾಮಾಕಾಶ time_literals;

ವರ್ಗ CollisionPrevention : ಸಾರ್ವಜನಿಕ ModuleParams
{
ಸಾರ್ವಜನಿಕ:
	CollisionPrevention(ModuleParams *parent);
	~CollisionPrevention() override = ಸ್ವಭಾವ;

	/**
	 * Returns true if Collision Prevention is running
	 */
	ತಾರ್ಕಿಕ is_active();

	/**
	 * Computes collision free setpoints
	 * @param setpoint_accel setpoint purely based on sticks, to be modified
	 * @param setpoint_vel current velocity setpoint as information to be able to stop in time, does not get changed
	 */
	ಶೂನ್ಯ modifySetpoint(matrix::Vector2f &setpoint_accel, ಧ್ರುವ matrix::Vector2f &setpoint_vel);

	ಸ್ಥಿರ constexpr ಪೂರ್ಣಾಙ್ಕ BIN_ಎಣಿಕೆ =
		ಪರಿಮಾಣ(obstacle_distance_s::distances) / ಪರಿಮಾಣ(obstacle_distance_s::distances[0]); // 72
	ಸ್ಥಿರ constexpr ಪೂರ್ಣಾಙ್ಕ BIN_SIZE = 360 / BIN_ಎಣಿಕೆ; // cannot be lower than 5 degrees, should divide 360 evenly

ರಕ್ಷಿತ:
	/** Aggregates the sensor data into an internal obstacle map in body frame */
	ಶೂನ್ಯ _updateObstacleMap();

	/** Updates the obstacle data based on stale data and calculates values from the map */
	ಶೂನ್ಯ _updateObstacleData();

	/** Calculate the constrained setpoint considering the current obstacle distances, acceleration setpoint and velocity setpoint */
	ಶೂನ್ಯ _calculateConstrainedSetpoint(matrix::Vector2f &setpoint_accel, ಧ್ರುವ matrix::Vector2f &setpoint_vel);

	obstacle_distance_s _obstacle_map_body_frame{};
	ತಾರ್ಕಿಕ _data_fov[BIN_ಎಣಿಕೆ] {};
	uint64_t _data_timestamps[BIN_ಎಣಿಕೆ] {};
	uint16_t _data_maxranges[BIN_ಎಣಿಕೆ] {}; /**< in cm */

	ಶೂನ್ಯ _addDistanceSensorData(distance_sensor_s &distance_sensor, ಧ್ರುವ matrix::Quatf &vehicle_attitude);

	/**
	 * Updates obstacle distance message with measurement from offboard
	 * @param obstacle, obstacle_distance message to be updated
	 */
	ಶೂನ್ಯ _addObstacleSensorData(ಧ್ರುವ obstacle_distance_s &obstacle, ಧ್ರುವ ಭಿನ್ನ vehicle_yaw);

	/**
	 * Computes an adaption to the setpoint direction to guide towards free space
	 * @param setpoint_dir, setpoint direction before collision prevention intervention
	 * @param setpoint_index, index of the setpoint in the internal obstacle map
	 * @param vehicle_yaw_angle_rad, vehicle orientation
	 */
	ಶೂನ್ಯ _adaptSetpointDirection(matrix::Vector2f &setpoint_dir, ಪೂರ್ಣಾಙ್ಕ &setpoint_index, ಭಿನ್ನ vehicle_yaw_angle_rad);

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
	matrix::Vector2f _constrainAccelerationSetpoint(ಧ್ರುವ ಭಿನ್ನ &setpoint_length);

	ಶೂನ್ಯ _getVelocityCompensationAcceleration(ಧ್ರುವ ಭಿನ್ನ vehicle_yaw_angle_rad, ಧ್ರುವ matrix::Vector2f &setpoint_vel,
			ಧ್ರುವ hrt_abstime now, ಭಿನ್ನ &vel_comp_accel, matrix::Vector2f &vel_comp_accel_dir);

	ಭಿನ್ನ _getObstacleDistance(ಧ್ರುವ matrix::Vector2f &direction);

	ಭಿನ್ನ _getScale(ಧ್ರುವ ಭಿನ್ನ &reference_distance);

	/**
	 * Determines whether a new sensor measurement is used
	 * @param map_index, index of the bin in the internal map the measurement belongs in
	 * @param sensor_range, max range of the sensor in meters
	 * @param sensor_reading, distance measurement in meters
	 */
	ತಾರ್ಕಿಕ _enterData(ಪೂರ್ಣಾಙ್ಕ map_index, ಭಿನ್ನ sensor_range, ಭಿನ್ನ sensor_reading);

	ತಾರ್ಕಿಕ _checkSetpointDirectionFeasability();

	ಶೂನ್ಯ _transformSetpoint(ಧ್ರುವ matrix::Vector2f &setpoint);


	//Timing functions. Necessary to mock time in the tests
	ಆಭಾಸ hrt_abstime getTime();
	ಆಭಾಸ hrt_abstime getElapsedTime(ಧ್ರುವ hrt_abstime *ptr);

ಗುಪ್ತ:
	ತಾರ್ಕಿಕ _data_stale{ಸತ್ಯ}; 		/**< states if the data is stale */
	ತಾರ್ಕಿಕ _was_active{ಅಸತ್ಯ};		/**< states if the collision prevention interferes with the user input */
	ತಾರ್ಕಿಕ _obstacle_data_present{ಅಸತ್ಯ};	/**< states if obstacle data is present */

	ಪೂರ್ಣಾಙ್ಕ _setpoint_index{};			/**< index of the setpoint*/
	matrix::Vector2f _setpoint_dir{};		/**< direction of the setpoint*/

	ಭಿನ್ನ _closest_dist{};			/**< closest distance to an obstacle  */
	matrix::Vector2f _closest_dist_dir{NAN, NAN};	/**< direction of the closest obstacle  */

	ಭಿನ್ನ _min_dist_to_keep{};

	orb_advert_t _mavlink_log_pub{nullptr};	 	/**< Mavlink log uORB handle */

	uORB::Subscription _vehicle_attitude_sub{ORB_ID(vehicle_attitude)};
	matrix::Quatf _vehicle_attitude{};
	ಭಿನ್ನ _vehicle_yaw{0.f};

	uORB::Publication<collision_constraints_s>	_constraints_pub{ORB_ID(collision_constraints)};		/**< constraints publication */
	uORB::Publication<obstacle_distance_s>		_obstacle_distance_fused_pub{ORB_ID(obstacle_distance_fused)};	/**< obstacle_distance publication */
	uORB::Publication<vehicle_command_s>	_vehicle_command_pub{ORB_ID(vehicle_command)};			/**< vehicle command do publication */

	uORB::SubscriptionData<obstacle_distance_s> _sub_obstacle_distance{ORB_ID(obstacle_distance)}; /**< obstacle distances received form a range sensor */
	uORB::SubscriptionMultiArray<distance_sensor_s> _distance_sensor_subs{ORB_ID::distance_sensor};

	ಸ್ಥಿರ constexpr uint64_t RANGE_STREAM_TIMEOUT_US{500_ms};
	ಸ್ಥಿರ constexpr uint64_t TIMEOUT_HOLD_US{5_s};

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
	ಶೂನ್ಯ _calculateConstrainedSetpoint(matrix::Vector2f &setpoint, ಧ್ರುವ matrix::Vector2f &curr_pos,
					   ಧ್ರುವ matrix::Vector2f &curr_vel);

	/**
	 * Publishes collision_constraints message
	 * @param original_setpoint, setpoint before collision prevention intervention
	 * @param adapted_setpoint, collision prevention adaped setpoint
	 */
	ಶೂನ್ಯ _publishConstrainedSetpoint(ಧ್ರುವ matrix::Vector2f &original_setpoint, ಧ್ರುವ matrix::Vector2f &adapted_setpoint);

	/**
	 * Publishes obstacle_distance message with fused data from offboard and from distance sensors
	 * @param obstacle, obstacle_distance message to be publsihed
	 */
	ಶೂನ್ಯ _publishObstacleDistance(obstacle_distance_s &obstacle);

	/**
	 * Publishes vehicle command.
	 */
	ಶೂನ್ಯ _publishVehicleCmdDoLoiter();

};

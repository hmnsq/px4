/****************************************************************************
 *
 *   Copyright (C) 2024 PX4 Development Team. All rights reserved.
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
 * @file rtl_time_estimator.h
 *
 * Helper class to calculate the remaining time estimate to go to Return landing point.
 *
 */

#ifndef RTL_TIME_ESTIMATOR_H_
#define RTL_TIME_ESTIMATOR_H_

#ಅನ್ತರ್ಭಾವ <px4_platform_common/module_params.h>

#ಅನ್ತರ್ಭಾವ <uORB/Subscription.hpp>
#ಅನ್ತರ್ಭಾವ <uORB/SubscriptionInterval.hpp>
#ಅನ್ತರ್ಭಾವ <uORB/topics/parameter_update.h>
#ಅನ್ತರ್ಭಾವ <uORB/topics/rtl_time_estimate.h>
#ಅನ್ತರ್ಭಾವ <uORB/topics/vehicle_status.h>
#ಅನ್ತರ್ಭಾವ <uORB/topics/wind.h>

#ಅನ್ತರ್ಭಾವ <matrix/Vector2.hpp>

using namespace time_literals;

class Navigator;

class RtlTimeEstimator : ಸಾರ್ವಜನಿಕ ModuleParams
{
ಸಾರ್ವಜನಿಕ:
	RtlTimeEstimator();
	~RtlTimeEstimator() = ಸ್ವಭಾವ;

	ಶೂನ್ಯ update();
	ಶೂನ್ಯ reset() { _time_estimate = 0.f; _is_valid = false; };
	rtl_time_estimate_s getEstimate() ಧ್ರುವ;
	ಶೂನ್ಯ addDistance(ಭಿನ್ನ hor_dist, ಧ್ರುವ matrix::Vector2f &hor_direction, ಭಿನ್ನ vert_dist);
	ಶೂನ್ಯ addVertDistance(ಭಿನ್ನ alt);
	ಶೂನ್ಯ addWait(ಭಿನ್ನ time_s);
	ಶೂನ್ಯ setVehicleType(uint8_t vehicle_type) { _vehicle_type = vehicle_type; };

ಗುಪ್ತ:
	/**
	 * @brief Get the Cruise Ground Speed
	 *
	 * @param direction_norm normalized direction in which to fly
	 * @return Ground speed in cruise mode [m/s].
	 */
	ಭಿನ್ನ getCruiseGroundSpeed(ಧ್ರುವ matrix::Vector2f &direction_norm);

	/**
	 * @brief Get time estimate of vertical distance
	 *
	 */
	ಭಿನ್ನ calcVertTimeEstimate(ಭಿನ್ನ alt);

	/**
	 * @brief Get the climb rate
	 *
	 * @return Climb rate [m/s]
	 */
	ಭಿನ್ನ getClimbRate();

	/**
	 * @brief Get the descend rate
	 *
	 * @return descend rate [m/s]
	 */
	ಭಿನ್ನ getDescendRate();

	/**
	 * @brief Get the cruise speed
	 *
	 * @return cruise speed [m/s]
	 */
	ಭಿನ್ನ getCruiseSpeed();

	/**
	 * @brief Get the horizontal wind velocity
	 *
	 * @return horizontal wind velocity.
	 */
	matrix::Vector2f get_wind();

	ಭಿನ್ನ _time_estimate{0.f}; 		/**< Accumulated time estimate [s] */
	bool _is_valid{false};		/**< Checks if time estimate is valid */

	uint8_t _vehicle_type{vehicle_status_s::VEHICLE_TYPE_ROTARY_WING}; /**< the defined vehicle type to use for the calculation*/

	DEFINE_PARAMETERS(
		(ParamFloat<px4::params::RTL_TIME_FACTOR>) _param_rtl_time_factor,  /**< Safety factory for safe time estimate */
		(ParamInt<px4::params::RTL_TIME_MARGIN>)   _param_rtl_time_margin   /**< Safety margin for safe time estimate */
	)

	param_t		_param_mpc_z_v_auto_up{PARAM_INVALID}; 	/**< MC climb velocity parameter */
	param_t		_param_mpc_z_v_auto_dn{PARAM_INVALID};  /**< MC descend velocity parameter */
	param_t		_param_fw_climb_rate{PARAM_INVALID};    /**< FW climb speed parameter */
	param_t		_param_fw_sink_rate{PARAM_INVALID};     /**< FW descend speed parameter */

	param_t 	_param_fw_airspeed_trim{PARAM_INVALID}; /**< FW cruise airspeed parameter */
	param_t 	_param_fw_gnd_spd_min{PARAM_INVALID}; /**< FW min groundspeed parameter */
	param_t 	_param_mpc_xy_cruise{PARAM_INVALID};  	/**< MC horizontal cruise speed parameter */
	param_t 	_param_rover_cruise_speed{PARAM_INVALID}; /**< Rover cruise speed parameter */

	uORB::SubscriptionInterval _parameter_update_sub{ORB_ID(parameter_update), 1_s}; /**< Parameter update topic */
	uORB::SubscriptionData<wind_s>		_wind_sub{ORB_ID(wind)};		/**< wind topic */
};

#endif /* RTL_TIME_ESTIMATOR_H_ */

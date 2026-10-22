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

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ RTL_ਸਮਾਂ_ESTIMATOR_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RTL_ਸਮਾਂ_ESTIMATOR_H

#ਅਨ੍ਤਰ੍ਭਾਵ <px4_platform_common/module_params.h>

#ਅਨ੍ਤਰ੍ਭਾਵ <uORB/Subscription.hpp>
#ਅਨ੍ਤਰ੍ਭਾਵ <uORB/SubscriptionInterval.hpp>
#ਅਨ੍ਤਰ੍ਭਾਵ <uORB/topics/parameter_update.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <uORB/topics/rtl_time_estimate.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <uORB/topics/vehicle_status.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <uORB/topics/wind.h>

#ਅਨ੍ਤਰ੍ਭਾਵ <matrix/Vector2.hpp>

using namespace ਸਮਾਂ_literals;

class Navigator;

class Rtl_ਸਮਾਂ_Estimator : ਜਨਤਕ ModuleParams
{
ਜਨਤਕ:
	Rtl_ਸਮਾਂ_Estimator();
	~Rtl_ਸਮਾਂ_Estimator() = ਮੂਲ;

	ਖਾਲੀ update();
	ਖਾਲੀ reset() { ਸਮਾਂ_estimate_2 = 0.f; _is_valid = false; };
	rtl_ਸਮਾਂ_estimate_s getEstimate() ਅਚਲ;
	ਖਾਲੀ ਜੋੜੋ_ਦੂਰੀ(ਦਸ਼ਮਲਵ hor_dist, ਅਚਲ ਮੈਟ੍ਰਿਕਸ::Vector2f &hor_direction, ਦਸ਼ਮਲਵ vert_dist);
	ਖਾਲੀ ਜੋੜੋ_Vert_ਦੂਰੀ(ਦਸ਼ਮਲਵ alt);
	ਖਾਲੀ ਜੋੜੋ_Wait(ਦਸ਼ਮਲਵ ਸਮਾਂ_s);
	ਖਾਲੀ ਸੈੱਟ_Vehicle_Type(uint8_t vehicle_type) { _vehicle_type = vehicle_type; };

ਨਿਜੀ:
	/**
	 * @brief Get the Cruise Ground Speed
	 *
	 * @param direction_norm normalized direction in which to fly
	 * @return Ground speed in cruise mode [m/s].
	 */
	ਦਸ਼ਮਲਵ get_Cruise_Ground_ਰਫਤਾਰ(ਅਚਲ ਮੈਟ੍ਰਿਕਸ::Vector2f &direction_norm);

	/**
	 * @brief Get time estimate of vertical distance
	 *
	 */
	ਦਸ਼ਮਲਵ calc_Vert_ਸਮਾਂ_Estimate(ਦਸ਼ਮਲਵ alt);

	/**
	 * @brief Get the climb rate
	 *
	 * @return Climb rate [m/s]
	 */
	ਦਸ਼ਮਲਵ get_Climb_ਦਰ();

	/**
	 * @brief Get the descend rate
	 *
	 * @return descend rate [m/s]
	 */
	ਦਸ਼ਮਲਵ get_Descend_ਦਰ();

	/**
	 * @brief Get the cruise speed
	 *
	 * @return cruise speed [m/s]
	 */
	ਦਸ਼ਮਲਵ get_Cruise_ਰਫਤਾਰ();

	/**
	 * @brief Get the horizontal wind velocity
	 *
	 * @return horizontal wind velocity.
	 */
	ਮੈਟ੍ਰਿਕਸ::Vector2f get_wind();

	ਦਸ਼ਮਲਵ ਸਮਾਂ_estimate_2{0.f}; 		/**< Accumulated time estimate [s] */
	bool _is_valid{false};		/**< Checks if time estimate is valid */

	uint8_t _vehicle_type{vehicle_ਸਥਿਤੀ_s::VEHICLE_TYPE_ROTARY_WING}; /**< the defined vehicle type to use for the calculation*/

	DEFINE_PARAMETERS(
		(ParamFloat<px4::params::RTL_ਸਮਾਂ_ਗੁਣਕ>) param_rtl_ਸਮਾਂ_ਗੁਣਕ,  /**< Safety factory for safe time estimate */
		(ParamInt<px4::params::RTL_ਸਮਾਂ_MARGIN>)   param_rtl_ਸਮਾਂ_margin   /**< Safety margin for safe time estimate */
	)

	param_t		param_mpc_z_v_auto_ਉੱਪਰ{PARAM_INVALID}; 	/**< MC climb velocity parameter */
	param_t		_param_mpc_z_v_auto_dn{PARAM_INVALID};  /**< MC descend velocity parameter */
	param_t		param_fw_climb_ਦਰ{PARAM_INVALID};    /**< FW climb speed parameter */
	param_t		param_fw_sink_ਦਰ{PARAM_INVALID};     /**< FW descend speed parameter */

	param_t 	_param_fw_airspeed_trim{PARAM_INVALID}; /**< FW cruise airspeed parameter */
	param_t 	_param_fw_gnd_spd_min{PARAM_INVALID}; /**< FW min groundspeed parameter */
	param_t 	_param_mpc_xy_cruise{PARAM_INVALID};  	/**< MC horizontal cruise speed parameter */
	param_t 	param_rover_cruise_ਰਫਤਾਰ{PARAM_INVALID}; /**< Rover cruise speed parameter */

	uORB::SubscriptionInterval _parameter_update_sub{ORB_ID(parameter_update), 1_s}; /**< Parameter update topic */
	uORB::Subscription_ਅੰਕੜੇ<wind_s>		_wind_sub{ORB_ID(wind)};		/**< wind topic */
};

#ਜੇ_ਅੰਤ /* RTL_TIME_ESTIMATOR_H_ */

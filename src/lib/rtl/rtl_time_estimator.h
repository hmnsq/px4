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

#که_نه_تعریف_شوی RTL_وخت_ESTIMATOR_H
#تعریف_کړه RTL_وخت_ESTIMATOR_H

#ضمن <px4_platform_common/module_params.h>

#ضمن <uORB/Subscription.hpp>
#ضمن <uORB/SubscriptionInterval.hpp>
#ضمن <uORB/topics/parameter_update.h>
#ضمن <uORB/topics/rtl_time_estimate.h>
#ضمن <uORB/topics/vehicle_status.h>
#ضمن <uORB/topics/wind.h>

#ضمن <matrix/Vector2.hpp>

using namespace وخت_literals;

class Navigator;

class Rtl_وخت_Estimator : عام ModuleParams
{
عام:
	Rtl_وخت_Estimator();
	~Rtl_وخت_Estimator() = افتراضي;

	فراغ update();
	فراغ reset() { وخت_estimate_2 = 0.f; _is_valid = false; };
	rtl_وخت_estimate_s getEstimate() ثابت;
	فراغ جمع_کړه_واټن(عائم hor_dist, ثابت ماتریکس::Vector2f &hor_direction, عائم vert_dist);
	فراغ جمع_کړه_Vert_واټن(عائم alt);
	فراغ جمع_کړه_Wait(عائم وخت_s);
	فراغ سېټ_Vehicle_Type(uint8_t vehicle_type) { _vehicle_type = vehicle_type; };

خاص:
	/**
	 * @brief Get the Cruise Ground Speed
	 *
	 * @param direction_norm normalized direction in which to fly
	 * @return Ground speed in cruise mode [m/s].
	 */
	عائم get_Cruise_Ground_چټکتیا(ثابت ماتریکس::Vector2f &direction_norm);

	/**
	 * @brief Get time estimate of vertical distance
	 *
	 */
	عائم calc_Vert_وخت_Estimate(عائم alt);

	/**
	 * @brief Get the climb rate
	 *
	 * @return Climb rate [m/s]
	 */
	عائم get_Climb_شرح();

	/**
	 * @brief Get the descend rate
	 *
	 * @return descend rate [m/s]
	 */
	عائم get_Descend_شرح();

	/**
	 * @brief Get the cruise speed
	 *
	 * @return cruise speed [m/s]
	 */
	عائم get_Cruise_چټکتیا();

	/**
	 * @brief Get the horizontal wind velocity
	 *
	 * @return horizontal wind velocity.
	 */
	ماتریکس::Vector2f get_wind();

	عائم وخت_estimate_2{0.f}; 		/**< Accumulated time estimate [s] */
	bool _is_valid{false};		/**< Checks if time estimate is valid */

	uint8_t _vehicle_type{vehicle_حالت_s::VEHICLE_TYPE_ROTARY_WING}; /**< the defined vehicle type to use for the calculation*/

	DEFINE_PARAMETERS(
		(ParamFloat<px4::params::RTL_وخت_ضریب>) param_rtl_وخت_ضریب,  /**< Safety factory for safe time estimate */
		(ParamInt<px4::params::RTL_وخت_MARGIN>)   param_rtl_وخت_margin   /**< Safety margin for safe time estimate */
	)

	param_t		param_mpc_z_v_auto_پورته{PARAM_INVALID}; 	/**< MC climb velocity parameter */
	param_t		_param_mpc_z_v_auto_dn{PARAM_INVALID};  /**< MC descend velocity parameter */
	param_t		param_fw_climb_شرح{PARAM_INVALID};    /**< FW climb speed parameter */
	param_t		param_fw_sink_شرح{PARAM_INVALID};     /**< FW descend speed parameter */

	param_t 	_param_fw_airspeed_trim{PARAM_INVALID}; /**< FW cruise airspeed parameter */
	param_t 	_param_fw_gnd_spd_min{PARAM_INVALID}; /**< FW min groundspeed parameter */
	param_t 	_param_mpc_xy_cruise{PARAM_INVALID};  	/**< MC horizontal cruise speed parameter */
	param_t 	param_rover_cruise_چټکتیا{PARAM_INVALID}; /**< Rover cruise speed parameter */

	uORB::SubscriptionInterval _parameter_update_sub{ORB_ID(parameter_update), 1_s}; /**< Parameter update topic */
	uORB::Subscription_معلومات<wind_s>		_wind_sub{ORB_ID(wind)};		/**< wind topic */
};

#د_که_پای /* RTL_TIME_ESTIMATOR_H_ */

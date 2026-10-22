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

#నిర్వచితం_కాకపోతే RTL_సమయం_ESTIMATOR_H
#నిర్వచించు RTL_సమయం_ESTIMATOR_H

#అన్తర్భావ <px4_platform_common/module_params.h>

#అన్తర్భావ <uORB/Subscription.hpp>
#అన్తర్భావ <uORB/SubscriptionInterval.hpp>
#అన్తర్భావ <uORB/topics/parameter_update.h>
#అన్తర్భావ <uORB/topics/rtl_time_estimate.h>
#అన్తర్భావ <uORB/topics/vehicle_status.h>
#అన్తర్భావ <uORB/topics/wind.h>

#అన్తర్భావ <matrix/Vector2.hpp>

using namespace సమయం_literals;

class Navigator;

class Rtl_సమయం_Estimator : బహిరంగ ModuleParams
{
బహిరంగ:
	Rtl_సమయం_Estimator();
	~Rtl_సమయం_Estimator() = అప్రమేయ;

	శూన్య update();
	శూన్య reset() { సమయం_estimate_2 = 0.f; _is_valid = false; };
	rtl_సమయం_estimate_s getEstimate() స్థిరరాశి;
	శూన్య కూడు_దూరం(దశాంశం hor_dist, స్థిరరాశి మాతృక::Vector2f &hor_direction, దశాంశం vert_dist);
	శూన్య కూడు_Vert_దూరం(దశాంశం alt);
	శూన్య కూడు_Wait(దశాంశం సమయం_s);
	శూన్య సమితి_Vehicle_Type(uint8_t vehicle_type) { _vehicle_type = vehicle_type; };

అంతరంగ:
	/**
	 * @brief Get the Cruise Ground Speed
	 *
	 * @param direction_norm normalized direction in which to fly
	 * @return Ground speed in cruise mode [m/s].
	 */
	దశాంశం get_Cruise_Ground_వేగం(స్థిరరాశి మాతృక::Vector2f &direction_norm);

	/**
	 * @brief Get time estimate of vertical distance
	 *
	 */
	దశాంశం calc_Vert_సమయం_Estimate(దశాంశం alt);

	/**
	 * @brief Get the climb rate
	 *
	 * @return Climb rate [m/s]
	 */
	దశాంశం get_Climb_రేటు();

	/**
	 * @brief Get the descend rate
	 *
	 * @return descend rate [m/s]
	 */
	దశాంశం get_Descend_రేటు();

	/**
	 * @brief Get the cruise speed
	 *
	 * @return cruise speed [m/s]
	 */
	దశాంశం get_Cruise_వేగం();

	/**
	 * @brief Get the horizontal wind velocity
	 *
	 * @return horizontal wind velocity.
	 */
	మాతృక::Vector2f get_wind();

	దశాంశం సమయం_estimate_2{0.f}; 		/**< Accumulated time estimate [s] */
	bool _is_valid{false};		/**< Checks if time estimate is valid */

	uint8_t _vehicle_type{vehicle_స్థితి_s::VEHICLE_TYPE_ROTARY_WING}; /**< the defined vehicle type to use for the calculation*/

	DEFINE_PARAMETERS(
		(ParamFloat<px4::params::RTL_సమయం_గుణకం>) param_rtl_సమయం_గుణకం,  /**< Safety factory for safe time estimate */
		(ParamInt<px4::params::RTL_సమయం_MARGIN>)   param_rtl_సమయం_margin   /**< Safety margin for safe time estimate */
	)

	param_t		param_mpc_z_v_auto_పైకి{PARAM_INVALID}; 	/**< MC climb velocity parameter */
	param_t		_param_mpc_z_v_auto_dn{PARAM_INVALID};  /**< MC descend velocity parameter */
	param_t		param_fw_climb_రేటు{PARAM_INVALID};    /**< FW climb speed parameter */
	param_t		param_fw_sink_రేటు{PARAM_INVALID};     /**< FW descend speed parameter */

	param_t 	_param_fw_airspeed_trim{PARAM_INVALID}; /**< FW cruise airspeed parameter */
	param_t 	_param_fw_gnd_spd_min{PARAM_INVALID}; /**< FW min groundspeed parameter */
	param_t 	_param_mpc_xy_cruise{PARAM_INVALID};  	/**< MC horizontal cruise speed parameter */
	param_t 	param_rover_cruise_వేగం{PARAM_INVALID}; /**< Rover cruise speed parameter */

	uORB::SubscriptionInterval _parameter_update_sub{ORB_ID(parameter_update), 1_s}; /**< Parameter update topic */
	uORB::Subscription_దత్తాంశం<wind_s>		_wind_sub{ORB_ID(wind)};		/**< wind topic */
};

#అయితే_ముగింపు /* RTL_TIME_ESTIMATOR_H_ */

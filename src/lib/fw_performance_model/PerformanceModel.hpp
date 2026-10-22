/****************************************************************************
 *
 *   Copyright (c) 2023 PX4 Development Team. All rights reserved.
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
 * @file PerformanceModel.hpp
 * Performance model.
 */

#अन्तर्भाव <float.h>
#अन्तर्भाव <math.h>
#अन्तर्भाव <px4_platform_common/module_params.h>
#अन्तर्भाव <uORB/topics/fuel_tank_status.h>

#ifndef PX4_SRC_MODULES_FW_POS_CONTROL_PERFORMANCEMODEL_H_
#define PX4_SRC_MODULES_FW_POS_CONTROL_PERFORMANCEMODEL_H_

वर्ग PerformanceModel : सार्वजनिक ModuleParams
{
सार्वजनिक:
	PerformanceModel();
	~PerformanceModel() = पूर्वनिर्धारित;

	शून्य updateParameters();

	/**
	 * Get the maximum climb rate (true airspeed) expected for current air density and weight.
	 * @param air_density in kg/m^3
	 * @return maximum climb rate in m/s
	 */
	दशांश getMaximumClimbRate(दशांश air_density) स्थिरांक;

	/**
	 * Get the minimum sink rate (true airspeed) expected for current air density and weight.
	 * @param air_density in kg/m^3
	 * @return minimum sink rate in m/s
	 */
	दशांश getMinimumSinkRate(दशांश air_density) स्थिरांक;

	/**
	 * Get the ration of actual weight to base weight
	 * @return weight ratio
	 */
	दशांश getWeightRatio() स्थिरांक;

	/**
	 * Set the fraction of fuel remaining, used to reduce the vehicle weight by the burned fuel mass
	 * if fuel-based weight compensation is enabled (WEIGHT_FUEL > 0).
	 * Non-finite input is ignored and the last known fuel fraction is kept.
	 * @param fuel_fraction_remaining fraction of fuel remaining in range [0,1]
	 */
	शून्य setFuelFractionRemaining(दशांश fuel_fraction_remaining);

	/**
	 * Check if fuel-based weight compensation is enabled.
	 * @return true if WEIGHT_FUEL is set to a valid fuel weight
	 */
	तार्किक isFuelCompensationEnabled() स्थिरांक { परत _param_weight_fuel.get() > भग्न_अंतर; }

	/**
	 * Extract the fraction of fuel remaining from a fuel tank status message, preferring the
	 * remaining percentage over the measured remaining fuel over the consumed fuel.
	 * The consumed fuel fallback assumes the tank was full at boot.
	 * @param fuel_tank_status fuel tank status message
	 * @return fraction of fuel remaining in range [0,1], NAN if not available
	 */
	स्थिर दशांश getFuelFractionRemaining(स्थिरांक fuel_tank_status_s &fuel_tank_status);

	/**
	 * Get the trim throttle for the current airspeed setpoint as well as air density and weight.
	 * @param throttle_min minimum throttle in range [0,1]
	 * @param throttle_max maximum throttle in range [0,1]
	 * @param airspeed_sp calibrated airspeed setpoint in m/s
	 * @param air_density air density in kg/m^3
	 * @return trim throttle in range [0,1]
	 */
	दशांश getTrimThrottle(दशांश throttle_min, दशांश throttle_max, दशांश airspeed_sp, दशांश air_density) स्थिरांक;

	/**
	 * Get the throttle scale factor for the current air density.
	 * @param air_density in kg/m^3
	 * @return throttle scale factor for air density
	 */
	दशांश getAirDensityThrottleScale(दशांश air_density) स्थिरांक;

	/**
	 * Get the trim airspeed compensated for weight.
	 * @return calibrated trim airspeed in m/s
	 */
	दशांश getCalibratedTrimAirspeed() स्थिरांक;

	/**
	 * Get the minimum airspeed compensated for weight, load factor due to bank angle and flaps.
	 * @param load_factor due to banking
	 * @param flaps_setpoint Flaps setpoint, normalized in range [0,1]
	 * @return calibrated minimum airspeed in m/s
	 */
	दशांश getMinimumCalibratedAirspeed(दशांश load_factor, दशांश flaps_setpoint) स्थिरांक;

	/**
	 * Get the maximum airspeed.
	 * @return calibrated maximum airspeed in m/s
	 */
	दशांश getMaximumCalibratedAirspeed() स्थिरांक;

	/**
	 * get the stall airspeed compensated for load factor due to bank angle.
	 * @param load_factor load factor due to banking
	 * @param flaps_setpoint Flaps setpoint, normalized in range [0,1]
	 * @return calibrated stall airspeed in m/s
	 */
	दशांश getCalibratedStallAirspeed(दशांश load_factor, दशांश flaps_setpoint) स्थिरांक;

	/**
	 * Run some checks on parameters and detect unfeasible combinations.
	 * @return true if all checks pass
	 */
	तार्किक runSanityChecks() स्थिरांक;

खाजगी:
	DEFINE_PARAMETERS(
		(ParamFloat<px4::params::FW_AIRSPD_MAX>) _param_fw_airspd_max,
		(ParamFloat<px4::params::FW_AIRSPD_MIN>) _param_fw_airspd_min,
		(ParamFloat<px4::params::FW_AIRSPD_TRIM>) _param_fw_airspd_trim,
		(ParamFloat<px4::params::FW_AIRSPD_STALL>) _param_fw_airspd_stall,
		(ParamFloat<px4::params::FW_T_CLMB_MAX>) _param_fw_t_clmb_max,
		(ParamFloat<px4::params::FW_T_SINK_MIN>) _param_fw_t_sink_min,
		(ParamFloat<px4::params::WEIGHT_BASE>) _param_weight_base,
		(ParamFloat<px4::params::WEIGHT_GROSS>) _param_weight_gross,
		(ParamFloat<px4::params::WEIGHT_FUEL>) _param_weight_fuel,
		(ParamFloat<px4::params::FW_SERVICE_CEIL>) _param_service_ceiling,
		(ParamFloat<px4::params::FW_THR_TRIM>) _param_fw_thr_trim,
		(ParamFloat<px4::params::FW_THR_MAX>) _param_fw_thr_max,
		(ParamFloat<px4::params::FW_THR_MIN>) _param_fw_thr_min,
		(ParamFloat<px4::params::FW_THR_ASPD_MIN>) _param_fw_thr_aspd_min,
		(ParamFloat<px4::params::FW_THR_ASPD_MAX>) _param_fw_thr_aspd_max,
		(ParamFloat<px4::params::FW_AIRSPD_FLP_SC>) _param_fw_airspd_flp_sc,
		(ParamFloat<px4::params::FW_R_LIM>) _param_fw_r_lim
	)

	/**
	 * Get the sea level trim throttle for a given calibrated airspeed setpoint.
	 * @param calibrated_airspeed_sp [m/s] calibrated
	 * @return trim throttle [0, 1] at sea level
	 */
	दशांश getTrimThrottleForCalibratedAirspeed(दशांश calibrated_airspeed_sp) स्थिरांक;


	स्थिर दशांश sanitiseAirDensity(दशांश air_density);

	दशांश _fuel_fraction_remaining{न_अंक}; ///< fraction of fuel remaining in range [0,1], NAN if unknown
};

#endif //PX4_SRC_MODULES_FW_POS_CONTROL_PERFORMANCEMODEL_H_

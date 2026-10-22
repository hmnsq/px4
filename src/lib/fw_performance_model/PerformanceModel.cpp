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
 * @file PerformanceModel.cpp
 * Performance model.
 */

#அந்தர்பாவ <geo/geo.h>
#அந்தர்பாவ <px4_platform_common/events.h>
#அந்தர்பாவ "PerformanceModel.hpp"
#அந்தர்பாவ <lib/atmosphere/atmosphere.h>

using நாமாகாஶ atmosphere;


// [.] minimum ratio between the actual vehicle weight and the vehicle nominal weight (weight at which the performance limits are derived)
நிலையான constexpr தசம kMinWeightRatio = 0.5f;

// [.] maximum ratio between the actual vehicle weight and the vehicle nominal weight (weight at which the performance limits are derived)
நிலையான constexpr தசம kMaxWeightRatio = 2.0f;

// [m/s] climbrate defining the service ceiling, used to compensate max climbrate based on air density
நிலையான constexpr தசம kClimbRateAtServiceCeiling = 0.5f;

PerformanceModel::PerformanceModel(): ModuleParams(nullptr)
{
	updateParams();
}
வெறுமை PerformanceModel::setFuelFractionRemaining(தசம fuel_fraction_remaining)
{
	எனில் (PX4_ISFINITE(fuel_fraction_remaining)) {
		_fuel_fraction_remaining = math::constrain(fuel_fraction_remaining, 0.f, 1.f);
	}
}

தசம PerformanceModel::getFuelFractionRemaining(மாறிலி fuel_tank_status_s &fuel_tank_status)
{
	தசம fraction = NAN;

	எனில் (fuel_tank_status.percent_remaining <= 100) {
		fraction = fuel_tank_status.percent_remaining * 0.01f;

	} இல்லையெனில் எனில் (PX4_ISFINITE(fuel_tank_status.remaining_fuel) && fuel_tank_status.maximum_fuel_capacity > FLT_EPSILON) {
		fraction = fuel_tank_status.remaining_fuel / fuel_tank_status.maximum_fuel_capacity;

	} இல்லையெனில் எனில் (PX4_ISFINITE(fuel_tank_status.consumed_fuel) && fuel_tank_status.maximum_fuel_capacity > FLT_EPSILON) {
		fraction = 1.f - fuel_tank_status.consumed_fuel / fuel_tank_status.maximum_fuel_capacity;
	}

	திரும்பு PX4_ISFINITE(fraction) ? math::constrain(fraction, 0.f, 1.f) : NAN;
}

தசம PerformanceModel::getWeightRatio() மாறிலி
{
	தசம weight_factor = 1.0f;

	எனில் (_param_weight_base.get() > FLT_EPSILON && _param_weight_gross.get() > FLT_EPSILON) {
		தசம weight = _param_weight_gross.get();

		எனில் (_param_weight_fuel.get() > FLT_EPSILON && PX4_ISFINITE(_fuel_fraction_remaining)) {
			weight -= (1.f - _fuel_fraction_remaining) * _param_weight_fuel.get();
		}

		weight_factor = math::constrain(weight / _param_weight_base.get(), kMinWeightRatio,
						kMaxWeightRatio);
	}

	திரும்பு weight_factor;
}
தசம PerformanceModel::getMaximumClimbRate(தசம air_density) மாறிலி
{
	air_density = sanitiseAirDensity(air_density);
	தசம climbrate_max = _param_fw_t_clmb_max.get();

	மாறிலி தசம service_ceiling = _param_service_ceiling.get();

	எனில் (service_ceiling > FLT_EPSILON) {
		மாறிலி தசம ceiling_pressure = getPressureFromAltitude(service_ceiling);
		மாறிலி தசம ceiling_density = getDensityFromPressureAndTemp(ceiling_pressure,
					      getStandardTemperatureAtAltitude(service_ceiling));
		மாறிலி தசம climbrate_gradient = math::max((_param_fw_t_clmb_max.get() - kClimbRateAtServiceCeiling) /
						 (kAirDensitySeaLevelStandardAtmos -
						  ceiling_density), 0.0f);
		மாறிலி தசம delta_rho = air_density - kAirDensitySeaLevelStandardAtmos;
		climbrate_max = math::constrain(_param_fw_t_clmb_max.get() + climbrate_gradient * delta_rho, kClimbRateAtServiceCeiling,
						_param_fw_t_clmb_max.get());
	}

	திரும்பு climbrate_max / getWeightRatio();
}
தசம PerformanceModel::getTrimThrottle(தசம throttle_min, தசம throttle_max, தசம airspeed_sp,
					தசம air_density) மாறிலி
{
	மாறிலி தசம throttle_trim = getTrimThrottleForCalibratedAirspeed(airspeed_sp) * getAirDensityThrottleScale(
					    air_density);
	திரும்பு math::constrain(throttle_trim, throttle_min, throttle_max);
}

தசம PerformanceModel::getAirDensityThrottleScale(தசம air_density) மாறிலி
{
	air_density = sanitiseAirDensity(air_density);
	தசம air_density_throttle_scale = 1.0f;

	// scale throttle as a function of sqrt(rho0/rho)
	மாறிலி தசம eas2tas = sqrtf(kAirDensitySeaLevelStandardAtmos / air_density);
	மாறிலி தசம eas2tas_at_11km_amsl = sqrtf(kAirDensitySeaLevelStandardAtmos / kAirDensityStandardAtmos11000Amsl);
	air_density_throttle_scale = math::constrain(eas2tas, 1.f, eas2tas_at_11km_amsl);

	திரும்பு air_density_throttle_scale;
}
தசம PerformanceModel::getTrimThrottleForCalibratedAirspeed(தசம calibrated_airspeed_sp) மாறிலி
{
	தசம throttle_trim =
		_param_fw_thr_trim.get(); // throttle required for level flight at trim airspeed, at sea level (standard atmosphere)

	// Drag modelling (parasite drag): calculate mapping airspeed-->throttle, assuming a linear relation with different gradients
// above and below trim. This is tunable thorugh FW_THR_ASPD_MIN and FW_THR_ASPD_MAX.
	மாறிலி தசம slope_below_trim = (_param_fw_thr_trim.get() - _param_fw_thr_aspd_min.get()) /
				       (_param_fw_airspd_trim.get() - _param_fw_airspd_min.get());
	மாறிலி தசம slope_above_trim = (_param_fw_thr_aspd_max.get() - _param_fw_thr_trim.get()) /
				       (_param_fw_airspd_max.get() - _param_fw_airspd_trim.get());

	எனில் (PX4_ISFINITE(calibrated_airspeed_sp) && PX4_ISFINITE(slope_below_trim) && _param_fw_thr_aspd_min.get() > FLT_EPSILON
	    && calibrated_airspeed_sp < _param_fw_airspd_trim.get()) {
		throttle_trim = _param_fw_thr_trim.get() - slope_below_trim * (_param_fw_airspd_trim.get() - calibrated_airspeed_sp);

	} இல்லையெனில் எனில் (PX4_ISFINITE(calibrated_airspeed_sp) && PX4_ISFINITE(slope_above_trim)
		   && _param_fw_thr_aspd_max.get() > FLT_EPSILON
		   && calibrated_airspeed_sp > _param_fw_airspd_trim.get()) {
		throttle_trim = _param_fw_thr_trim.get() + slope_above_trim * (calibrated_airspeed_sp - _param_fw_airspd_trim.get());
	}

	திரும்பு throttle_trim;
}
தசம PerformanceModel::getMinimumSinkRate(தசம air_density) மாறிலி
{
	air_density = sanitiseAirDensity(air_density);
	திரும்பு _param_fw_t_sink_min.get() * sqrtf(getWeightRatio() * kAirDensitySeaLevelStandardAtmos / air_density);
}
தசம PerformanceModel::getCalibratedTrimAirspeed() மாறிலி
{
	திரும்பு math::constrain(_param_fw_airspd_trim.get() * sqrtf(getWeightRatio()), _param_fw_airspd_min.get(),
			       _param_fw_airspd_max.get());
}
தசம PerformanceModel::getMinimumCalibratedAirspeed(தசம load_factor, தசம flaps_setpoint) மாறிலி
{
	load_factor = math::max(load_factor, FLT_EPSILON);
	மாறிலி தசம airspeed_flap_factor = math::lerp(1.f, _param_fw_airspd_flp_sc.get(), flaps_setpoint);
	திரும்பு (_param_fw_airspd_min.get() * airspeed_flap_factor) * sqrtf(getWeightRatio() * load_factor);
}

தசம PerformanceModel::getCalibratedStallAirspeed(தசம load_factor, தசம flaps_setpoint) மாறிலி
{
	load_factor = math::max(load_factor, FLT_EPSILON);
	மாறிலி தசம airspeed_flap_factor = math::lerp(1.f, _param_fw_airspd_flp_sc.get(), flaps_setpoint);
	திரும்பு (_param_fw_airspd_stall.get() * airspeed_flap_factor) * sqrtf(getWeightRatio() * load_factor);
}

தசம PerformanceModel::getMaximumCalibratedAirspeed() மாறிலி
{
	திரும்பு _param_fw_airspd_max.get();
}
தருக்க PerformanceModel::runSanityChecks() மாறிலி
{
	தருக்க ret = உண்மை;

	// sanity check parameters
	எனில் (_param_fw_airspd_max.get() < _param_fw_airspd_min.get()) {
		/* EVENT
		 * @description
		 * - <param>FW_AIRSPD_MAX</param>: {1:.1}
		 * - <param>FW_AIRSPD_MIN</param>: {2:.1}
		 */
		events::send<தசம, தசம>(events::ID("fixedwing_position_control_conf_invalid_airspeed"), events::Log::Error,
					   "Invalid configuration: Airspeed max smaller than min",
					   _param_fw_airspd_max.get(), _param_fw_airspd_min.get());
		ret = பொய்;
	}

	எனில் (_param_fw_airspd_max.get() < 5.0f || _param_fw_airspd_min.get() > 100.0f) {
		/* EVENT
		 * @description
		 * - <param>FW_AIRSPD_MAX</param>: {1:.1}
		 * - <param>FW_AIRSPD_MIN</param>: {2:.1}
		 */
		events::send<தசம, தசம>(events::ID("fixedwing_position_control_conf_invalid_airspeed_bounds"), events::Log::Error,
					   "Invalid configuration: Airspeed max \\< 5 m/s or min \\> 100 m/s",
					   _param_fw_airspd_max.get(), _param_fw_airspd_min.get());
		ret = பொய்;
	}

	எனில் (_param_fw_airspd_trim.get() < _param_fw_airspd_min.get() ||
	    _param_fw_airspd_trim.get() > _param_fw_airspd_max.get()) {
		/* EVENT
		 * @description
		 * - <param>FW_AIRSPD_MAX</param>: {1:.1}
		 * - <param>FW_AIRSPD_MIN</param>: {2:.1}
		 * - <param>FW_AIRSPD_TRIM</param>: {3:.1}
		 */
		events::send<தசம, தசம, தசம>(events::ID("fixedwing_position_control_conf_invalid_trim_bounds"),
						  events::Log::Error,
						  "Invalid configuration: Airspeed trim out of min or max bounds",
						  _param_fw_airspd_max.get(), _param_fw_airspd_min.get(), _param_fw_airspd_trim.get());
		ret = பொய்;
	}

	எனில் (_param_fw_airspd_stall.get() > _param_fw_airspd_min.get()) {
		/* EVENT
		 * @description
		 * - <param>FW_AIRSPD_MIN</param>: {1:.1}
		 * - <param>FW_AIRSPD_STALL</param>: {2:.1}
		 */
		events::send<தசம, தசம>(events::ID("fixedwing_position_control_conf_invalid_stall"), events::Log::Error,
					   "Invalid configuration: FW_AIRSPD_STALL higher FW_AIRSPD_MIN",
					   _param_fw_airspd_min.get(), _param_fw_airspd_stall.get());
		ret = பொய்;
	}

	மாறிலி தசம max_bank_loadfactor = 1 / cosf(math::radians(_param_fw_r_lim.get()));
	மாறிலி தசம min_airspd_at_max_bank = getMinimumCalibratedAirspeed(max_bank_loadfactor, /*flaps_setpoint = */0.0f);

	எனில் (min_airspd_at_max_bank > _param_fw_airspd_max.get()) {

		// Flying the maximum bank angle requires an airspeed above FW_AIRSPD_MAX.
		// To mitigate, choose between these (or a combination):
		//  - Allow higher airspeeds (formula from getMinimumCalibratedAirspeed):
		//      FW_AIRSPD_MAX >= FW_AIRSPD_MIN * sqrt(WEIGHT_GROSS/WEIGHT_BASE) * sqrt(1/cos(FW_R_LIM))
		//  - Decrease max bank angle (same inequality, solved for FW_R_LIM):
		//      FW_R_LIM <= acos((FW_AIRSPD_MIN/FW_AIRSPD_MAX)**2 * (WEIGHT_GROSS/WEIGHT_BASE))
		// If flying with a range of weight ratios, take the worst case (heaviest) for both these formulas.

		/* EVENT
		 * @description
		 * - <param>FW_AIRSPD_MIN</param>: {1:.1}
		 * - <param>FW_AIRSPD_MAX</param>: {2:.1}
		 * - <param>FW_R_LIM</param>: {3:.1}
		 */
		events::send<தசம, தசம, தசம>(events::ID("fixedwing_position_control_conf_invalid_maxbank_infeasible"), events::Log::Error,
						  "Invalid configuration: FW_AIRSPD_MAX too low to sustain max bank angle",
						  _param_fw_airspd_min.get(), _param_fw_airspd_max.get(), _param_fw_r_lim.get());
		ret = பொய்;
	}

	திரும்பு ret;

}
வெறுமை PerformanceModel::	updateParameters()
{
	updateParams();
}

தசம PerformanceModel::sanitiseAirDensity(தசம air_density)
{
	எனில் (!PX4_ISFINITE(air_density)) {
		air_density = kAirDensitySeaLevelStandardAtmos;
	}

	திரும்பு math::max(air_density, kAirDensityStandardAtmos11000Amsl);
}

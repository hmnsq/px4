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

#شامل <geo/geo.h>
#شامل <px4_platform_common/events.h>
#شامل "PerformanceModel.hpp"
#شامل <lib/atmosphere/atmosphere.h>

using فضاینام atmosphere;


// [.] minimum ratio between the actual vehicle weight and the vehicle nominal weight (weight at which the performance limits are derived)
ایستا constexpr اعشاری kMinWeightRatio = 0.5f;

// [.] maximum ratio between the actual vehicle weight and the vehicle nominal weight (weight at which the performance limits are derived)
ایستا constexpr اعشاری kMaxWeightRatio = 2.0f;

// [m/s] climbrate defining the service ceiling, used to compensate max climbrate based on air density
ایستا constexpr اعشاری kClimbRateAtServiceCeiling = 0.5f;

PerformanceModel::PerformanceModel(): ModuleParams(nullptr)
{
	updateParams();
}
تهی PerformanceModel::setFuelFractionRemaining(اعشاری fuel_fraction_remaining)
{
	اگر (PX4_ISFINITE(fuel_fraction_remaining)) {
		_fuel_fraction_remaining = math::constrain(fuel_fraction_remaining, 0.f, 1.f);
	}
}

اعشاری PerformanceModel::getFuelFractionRemaining(ثابت fuel_tank_status_s &fuel_tank_status)
{
	اعشاری fraction = NAN;

	اگر (fuel_tank_status.percent_remaining <= 100) {
		fraction = fuel_tank_status.percent_remaining * 0.01f;

	} وگرنه اگر (PX4_ISFINITE(fuel_tank_status.remaining_fuel) && fuel_tank_status.maximum_fuel_capacity > FLT_EPSILON) {
		fraction = fuel_tank_status.remaining_fuel / fuel_tank_status.maximum_fuel_capacity;

	} وگرنه اگر (PX4_ISFINITE(fuel_tank_status.consumed_fuel) && fuel_tank_status.maximum_fuel_capacity > FLT_EPSILON) {
		fraction = 1.f - fuel_tank_status.consumed_fuel / fuel_tank_status.maximum_fuel_capacity;
	}

	بازگشت PX4_ISFINITE(fraction) ? math::constrain(fraction, 0.f, 1.f) : NAN;
}

اعشاری PerformanceModel::getWeightRatio() ثابت
{
	اعشاری weight_factor = 1.0f;

	اگر (_param_weight_base.get() > FLT_EPSILON && _param_weight_gross.get() > FLT_EPSILON) {
		اعشاری weight = _param_weight_gross.get();

		اگر (_param_weight_fuel.get() > FLT_EPSILON && PX4_ISFINITE(_fuel_fraction_remaining)) {
			weight -= (1.f - _fuel_fraction_remaining) * _param_weight_fuel.get();
		}

		weight_factor = math::constrain(weight / _param_weight_base.get(), kMinWeightRatio,
						kMaxWeightRatio);
	}

	بازگشت weight_factor;
}
اعشاری PerformanceModel::getMaximumClimbRate(اعشاری air_density) ثابت
{
	air_density = sanitiseAirDensity(air_density);
	اعشاری climbrate_max = _param_fw_t_clmb_max.get();

	ثابت اعشاری service_ceiling = _param_service_ceiling.get();

	اگر (service_ceiling > FLT_EPSILON) {
		ثابت اعشاری ceiling_pressure = getPressureFromAltitude(service_ceiling);
		ثابت اعشاری ceiling_density = getDensityFromPressureAndTemp(ceiling_pressure,
					      getStandardTemperatureAtAltitude(service_ceiling));
		ثابت اعشاری climbrate_gradient = math::max((_param_fw_t_clmb_max.get() - kClimbRateAtServiceCeiling) /
						 (kAirDensitySeaLevelStandardAtmos -
						  ceiling_density), 0.0f);
		ثابت اعشاری delta_rho = air_density - kAirDensitySeaLevelStandardAtmos;
		climbrate_max = math::constrain(_param_fw_t_clmb_max.get() + climbrate_gradient * delta_rho, kClimbRateAtServiceCeiling,
						_param_fw_t_clmb_max.get());
	}

	بازگشت climbrate_max / getWeightRatio();
}
اعشاری PerformanceModel::getTrimThrottle(اعشاری throttle_min, اعشاری throttle_max, اعشاری airspeed_sp,
					اعشاری air_density) ثابت
{
	ثابت اعشاری throttle_trim = getTrimThrottleForCalibratedAirspeed(airspeed_sp) * getAirDensityThrottleScale(
					    air_density);
	بازگشت math::constrain(throttle_trim, throttle_min, throttle_max);
}

اعشاری PerformanceModel::getAirDensityThrottleScale(اعشاری air_density) ثابت
{
	air_density = sanitiseAirDensity(air_density);
	اعشاری air_density_throttle_scale = 1.0f;

	// scale throttle as a function of sqrt(rho0/rho)
	ثابت اعشاری eas2tas = sqrtf(kAirDensitySeaLevelStandardAtmos / air_density);
	ثابت اعشاری eas2tas_at_11km_amsl = sqrtf(kAirDensitySeaLevelStandardAtmos / kAirDensityStandardAtmos11000Amsl);
	air_density_throttle_scale = math::constrain(eas2tas, 1.f, eas2tas_at_11km_amsl);

	بازگشت air_density_throttle_scale;
}
اعشاری PerformanceModel::getTrimThrottleForCalibratedAirspeed(اعشاری calibrated_airspeed_sp) ثابت
{
	اعشاری throttle_trim =
		_param_fw_thr_trim.get(); // throttle required for level flight at trim airspeed, at sea level (standard atmosphere)

	// Drag modelling (parasite drag): calculate mapping airspeed-->throttle, assuming a linear relation with different gradients
// above and below trim. This is tunable thorugh FW_THR_ASPD_MIN and FW_THR_ASPD_MAX.
	ثابت اعشاری slope_below_trim = (_param_fw_thr_trim.get() - _param_fw_thr_aspd_min.get()) /
				       (_param_fw_airspd_trim.get() - _param_fw_airspd_min.get());
	ثابت اعشاری slope_above_trim = (_param_fw_thr_aspd_max.get() - _param_fw_thr_trim.get()) /
				       (_param_fw_airspd_max.get() - _param_fw_airspd_trim.get());

	اگر (PX4_ISFINITE(calibrated_airspeed_sp) && PX4_ISFINITE(slope_below_trim) && _param_fw_thr_aspd_min.get() > FLT_EPSILON
	    && calibrated_airspeed_sp < _param_fw_airspd_trim.get()) {
		throttle_trim = _param_fw_thr_trim.get() - slope_below_trim * (_param_fw_airspd_trim.get() - calibrated_airspeed_sp);

	} وگرنه اگر (PX4_ISFINITE(calibrated_airspeed_sp) && PX4_ISFINITE(slope_above_trim)
		   && _param_fw_thr_aspd_max.get() > FLT_EPSILON
		   && calibrated_airspeed_sp > _param_fw_airspd_trim.get()) {
		throttle_trim = _param_fw_thr_trim.get() + slope_above_trim * (calibrated_airspeed_sp - _param_fw_airspd_trim.get());
	}

	بازگشت throttle_trim;
}
اعشاری PerformanceModel::getMinimumSinkRate(اعشاری air_density) ثابت
{
	air_density = sanitiseAirDensity(air_density);
	بازگشت _param_fw_t_sink_min.get() * sqrtf(getWeightRatio() * kAirDensitySeaLevelStandardAtmos / air_density);
}
اعشاری PerformanceModel::getCalibratedTrimAirspeed() ثابت
{
	بازگشت math::constrain(_param_fw_airspd_trim.get() * sqrtf(getWeightRatio()), _param_fw_airspd_min.get(),
			       _param_fw_airspd_max.get());
}
اعشاری PerformanceModel::getMinimumCalibratedAirspeed(اعشاری load_factor, اعشاری flaps_setpoint) ثابت
{
	load_factor = math::max(load_factor, FLT_EPSILON);
	ثابت اعشاری airspeed_flap_factor = math::lerp(1.f, _param_fw_airspd_flp_sc.get(), flaps_setpoint);
	بازگشت (_param_fw_airspd_min.get() * airspeed_flap_factor) * sqrtf(getWeightRatio() * load_factor);
}

اعشاری PerformanceModel::getCalibratedStallAirspeed(اعشاری load_factor, اعشاری flaps_setpoint) ثابت
{
	load_factor = math::max(load_factor, FLT_EPSILON);
	ثابت اعشاری airspeed_flap_factor = math::lerp(1.f, _param_fw_airspd_flp_sc.get(), flaps_setpoint);
	بازگشت (_param_fw_airspd_stall.get() * airspeed_flap_factor) * sqrtf(getWeightRatio() * load_factor);
}

اعشاری PerformanceModel::getMaximumCalibratedAirspeed() ثابت
{
	بازگشت _param_fw_airspd_max.get();
}
منطقی PerformanceModel::runSanityChecks() ثابت
{
	منطقی ret = درست;

	// sanity check parameters
	اگر (_param_fw_airspd_max.get() < _param_fw_airspd_min.get()) {
		/* EVENT
		 * @description
		 * - <param>FW_AIRSPD_MAX</param>: {1:.1}
		 * - <param>FW_AIRSPD_MIN</param>: {2:.1}
		 */
		events::send<اعشاری, اعشاری>(events::ID("fixedwing_position_control_conf_invalid_airspeed"), events::Log::Error,
					   "Invalid configuration: Airspeed max smaller than min",
					   _param_fw_airspd_max.get(), _param_fw_airspd_min.get());
		ret = نادرست;
	}

	اگر (_param_fw_airspd_max.get() < 5.0f || _param_fw_airspd_min.get() > 100.0f) {
		/* EVENT
		 * @description
		 * - <param>FW_AIRSPD_MAX</param>: {1:.1}
		 * - <param>FW_AIRSPD_MIN</param>: {2:.1}
		 */
		events::send<اعشاری, اعشاری>(events::ID("fixedwing_position_control_conf_invalid_airspeed_bounds"), events::Log::Error,
					   "Invalid configuration: Airspeed max \\< 5 m/s or min \\> 100 m/s",
					   _param_fw_airspd_max.get(), _param_fw_airspd_min.get());
		ret = نادرست;
	}

	اگر (_param_fw_airspd_trim.get() < _param_fw_airspd_min.get() ||
	    _param_fw_airspd_trim.get() > _param_fw_airspd_max.get()) {
		/* EVENT
		 * @description
		 * - <param>FW_AIRSPD_MAX</param>: {1:.1}
		 * - <param>FW_AIRSPD_MIN</param>: {2:.1}
		 * - <param>FW_AIRSPD_TRIM</param>: {3:.1}
		 */
		events::send<اعشاری, اعشاری, اعشاری>(events::ID("fixedwing_position_control_conf_invalid_trim_bounds"),
						  events::Log::Error,
						  "Invalid configuration: Airspeed trim out of min or max bounds",
						  _param_fw_airspd_max.get(), _param_fw_airspd_min.get(), _param_fw_airspd_trim.get());
		ret = نادرست;
	}

	اگر (_param_fw_airspd_stall.get() > _param_fw_airspd_min.get()) {
		/* EVENT
		 * @description
		 * - <param>FW_AIRSPD_MIN</param>: {1:.1}
		 * - <param>FW_AIRSPD_STALL</param>: {2:.1}
		 */
		events::send<اعشاری, اعشاری>(events::ID("fixedwing_position_control_conf_invalid_stall"), events::Log::Error,
					   "Invalid configuration: FW_AIRSPD_STALL higher FW_AIRSPD_MIN",
					   _param_fw_airspd_min.get(), _param_fw_airspd_stall.get());
		ret = نادرست;
	}

	ثابت اعشاری max_bank_loadfactor = 1 / cosf(math::radians(_param_fw_r_lim.get()));
	ثابت اعشاری min_airspd_at_max_bank = getMinimumCalibratedAirspeed(max_bank_loadfactor, /*flaps_setpoint = */0.0f);

	اگر (min_airspd_at_max_bank > _param_fw_airspd_max.get()) {

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
		events::send<اعشاری, اعشاری, اعشاری>(events::ID("fixedwing_position_control_conf_invalid_maxbank_infeasible"), events::Log::Error,
						  "Invalid configuration: FW_AIRSPD_MAX too low to sustain max bank angle",
						  _param_fw_airspd_min.get(), _param_fw_airspd_max.get(), _param_fw_r_lim.get());
		ret = نادرست;
	}

	بازگشت ret;

}
تهی PerformanceModel::	updateParameters()
{
	updateParams();
}

اعشاری PerformanceModel::sanitiseAirDensity(اعشاری air_density)
{
	اگر (!PX4_ISFINITE(air_density)) {
		air_density = kAirDensitySeaLevelStandardAtmos;
	}

	بازگشت math::max(air_density, kAirDensityStandardAtmos11000Amsl);
}

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

#ਅਨ੍ਤਰ੍ਭਾਵ <geo/geo.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <px4_platform_common/events.h>
#ਅਨ੍ਤਰ੍ਭਾਵ "PerformanceModel.hpp"
#ਅਨ੍ਤਰ੍ਭਾਵ <lib/atmosphere/atmosphere.h>

ਵਰਤੋਂ ਨਾਮਾਕਾਸ਼ atmosphere;


// [.] minimum ratio between the actual vehicle weight and the vehicle nominal weight (weight at which the performance limits are derived)
ਸਥਿਰ constexpr ਦਸ਼ਮਲਵ k_Min_ਭਾਰ_ਅਨੁਪਾਤ = 0.5f;

// [.] maximum ratio between the actual vehicle weight and the vehicle nominal weight (weight at which the performance limits are derived)
ਸਥਿਰ constexpr ਦਸ਼ਮਲਵ k_Max_ਭਾਰ_ਅਨੁਪਾਤ = 2.0f;

// [m/s] climbrate defining the service ceiling, used to compensate max climbrate based on air density
ਸਥਿਰ constexpr ਦਸ਼ਮਲਵ k_Climb_ਦਰ_At_Service_Ceiling = 0.5f;

PerformanceModel::PerformanceModel(): ModuleParams(ਖਾਲੀ_ਸੂਚਕ)
{
	updateParams();
}
ਖਾਲੀ PerformanceModel::ਸੈੱਟ_Fuel_Fraction_ਬਚਿਆ(ਦਸ਼ਮਲਵ fuel_fraction_ਬਚਿਆ)
{
	ਜੇ (PX4_ISFINITE(fuel_fraction_ਬਚਿਆ)) {
		fuel_fraction_ਬਚਿਆ_2 = math::constrain(fuel_fraction_ਬਚਿਆ, 0.f, 1.f);
	}
}

ਦਸ਼ਮਲਵ PerformanceModel::get_Fuel_Fraction_ਬਚਿਆ(ਅਚਲ fuel_tank_ਸਥਿਤੀ_s &fuel_tank_ਸਥਿਤੀ)
{
	ਦਸ਼ਮਲਵ fraction = NAN;

	ਜੇ (fuel_tank_ਸਥਿਤੀ.percent_ਬਚਿਆ <= 100) {
		fraction = fuel_tank_ਸਥਿਤੀ.percent_ਬਚਿਆ * 0.01f;

	} ਨਹੀਂਤਾਂ ਜੇ (PX4_ISFINITE(fuel_tank_ਸਥਿਤੀ.ਬਚਿਆ_fuel) && fuel_tank_ਸਥਿਤੀ.ਵੱਧ_ਤੋਂ_ਵੱਧ_fuel_ਸਮਰੱਥਾ > FLT_EPSILON) {
		fraction = fuel_tank_ਸਥਿਤੀ.ਬਚਿਆ_fuel / fuel_tank_ਸਥਿਤੀ.ਵੱਧ_ਤੋਂ_ਵੱਧ_fuel_ਸਮਰੱਥਾ;

	} ਨਹੀਂਤਾਂ ਜੇ (PX4_ISFINITE(fuel_tank_ਸਥਿਤੀ.consumed_fuel) && fuel_tank_ਸਥਿਤੀ.ਵੱਧ_ਤੋਂ_ਵੱਧ_fuel_ਸਮਰੱਥਾ > FLT_EPSILON) {
		fraction = 1.f - fuel_tank_ਸਥਿਤੀ.consumed_fuel / fuel_tank_ਸਥਿਤੀ.ਵੱਧ_ਤੋਂ_ਵੱਧ_fuel_ਸਮਰੱਥਾ;
	}

	ਵਾਪਸ PX4_ISFINITE(fraction) ? math::constrain(fraction, 0.f, 1.f) : NAN;
}

ਦਸ਼ਮਲਵ PerformanceModel::get_ਭਾਰ_ਅਨੁਪਾਤ() ਅਚਲ
{
	ਦਸ਼ਮਲਵ ਭਾਰ_ਗੁਣਕ = 1.0f;

	ਜੇ (param_ਭਾਰ_ਅਧਾਰ.get() > FLT_EPSILON && param_ਭਾਰ_gross.get() > FLT_EPSILON) {
		ਦਸ਼ਮਲਵ ਭਾਰ = param_ਭਾਰ_gross.get();

		ਜੇ (param_ਭਾਰ_fuel.get() > FLT_EPSILON && PX4_ISFINITE(fuel_fraction_ਬਚਿਆ_2)) {
			ਭਾਰ -= (1.f - fuel_fraction_ਬਚਿਆ_2) * param_ਭਾਰ_fuel.get();
		}

		ਭਾਰ_ਗੁਣਕ = math::constrain(ਭਾਰ / param_ਭਾਰ_ਅਧਾਰ.get(), k_Min_ਭਾਰ_ਅਨੁਪਾਤ,
						k_Max_ਭਾਰ_ਅਨੁਪਾਤ);
	}

	ਵਾਪਸ ਭਾਰ_ਗੁਣਕ;
}
ਦਸ਼ਮਲਵ PerformanceModel::get_ਵੱਧ_ਤੋਂ_ਵੱਧ_Climb_ਦਰ(ਦਸ਼ਮਲਵ air_density) ਅਚਲ
{
	air_density = sanitiseAirDensity(air_density);
	ਦਸ਼ਮਲਵ climbrate_max = _param_fw_t_clmb_max.get();

	ਅਚਲ ਦਸ਼ਮਲਵ service_ceiling = _param_service_ceiling.get();

	ਜੇ (service_ceiling > FLT_EPSILON) {
		ਅਚਲ ਦਸ਼ਮਲਵ ceiling_pressure = getPressureFromAltitude(service_ceiling);
		ਅਚਲ ਦਸ਼ਮਲਵ ceiling_density = get_Density_From_Pressure_And_ਆਰਜ਼ੀ(ceiling_pressure,
					      getStandardTemperatureAtAltitude(service_ceiling));
		ਅਚਲ ਦਸ਼ਮਲਵ climbrate_gradient = math::max((_param_fw_t_clmb_max.get() - k_Climb_ਦਰ_At_Service_Ceiling) /
						 (kAirDensitySeaLevelStandardAtmos -
						  ceiling_density), 0.0f);
		ਅਚਲ ਦਸ਼ਮਲਵ delta_rho = air_density - kAirDensitySeaLevelStandardAtmos;
		climbrate_max = math::constrain(_param_fw_t_clmb_max.get() + climbrate_gradient * delta_rho, k_Climb_ਦਰ_At_Service_Ceiling,
						_param_fw_t_clmb_max.get());
	}

	ਵਾਪਸ climbrate_max / get_ਭਾਰ_ਅਨੁਪਾਤ();
}
ਦਸ਼ਮਲਵ PerformanceModel::getTrimThrottle(ਦਸ਼ਮਲਵ throttle_min, ਦਸ਼ਮਲਵ throttle_max, ਦਸ਼ਮਲਵ airspeed_sp,
					ਦਸ਼ਮਲਵ air_density) ਅਚਲ
{
	ਅਚਲ ਦਸ਼ਮਲਵ throttle_trim = getTrimThrottleForCalibratedAirspeed(airspeed_sp) * getAirDensityThrottleScale(
					    air_density);
	ਵਾਪਸ math::constrain(throttle_trim, throttle_min, throttle_max);
}

ਦਸ਼ਮਲਵ PerformanceModel::getAirDensityThrottleScale(ਦਸ਼ਮਲਵ air_density) ਅਚਲ
{
	air_density = sanitiseAirDensity(air_density);
	ਦਸ਼ਮਲਵ air_density_throttle_scale = 1.0f;

	// scale throttle as a function of sqrt(rho0/rho)
	ਅਚਲ ਦਸ਼ਮਲਵ eas2tas = sqrtf(kAirDensitySeaLevelStandardAtmos / air_density);
	ਅਚਲ ਦਸ਼ਮਲਵ eas2tas_at_11km_amsl = sqrtf(kAirDensitySeaLevelStandardAtmos / kAirDensityStandardAtmos11000Amsl);
	air_density_throttle_scale = math::constrain(eas2tas, 1.f, eas2tas_at_11km_amsl);

	ਵਾਪਸ air_density_throttle_scale;
}
ਦਸ਼ਮਲਵ PerformanceModel::getTrimThrottleForCalibratedAirspeed(ਦਸ਼ਮਲਵ calibrated_airspeed_sp) ਅਚਲ
{
	ਦਸ਼ਮਲਵ throttle_trim =
		_param_fw_thr_trim.get(); // throttle required for level flight at trim airspeed, at sea level (standard atmosphere)

	// Drag modelling (parasite drag): calculate mapping airspeed-->throttle, assuming a linear relation with different gradients
// above and below trim. This is tunable thorugh FW_THR_ASPD_MIN and FW_THR_ASPD_MAX.
	ਅਚਲ ਦਸ਼ਮਲਵ ਢਲਾਨ_ਹੇਠਾਂ_trim = (_param_fw_thr_trim.get() - _param_fw_thr_aspd_min.get()) /
				       (_param_fw_airspd_trim.get() - _param_fw_airspd_min.get());
	ਅਚਲ ਦਸ਼ਮਲਵ ਢਲਾਨ_above_trim = (_param_fw_thr_aspd_max.get() - _param_fw_thr_trim.get()) /
				       (_param_fw_airspd_max.get() - _param_fw_airspd_trim.get());

	ਜੇ (PX4_ISFINITE(calibrated_airspeed_sp) && PX4_ISFINITE(ਢਲਾਨ_ਹੇਠਾਂ_trim) && _param_fw_thr_aspd_min.get() > FLT_EPSILON
	    && calibrated_airspeed_sp < _param_fw_airspd_trim.get()) {
		throttle_trim = _param_fw_thr_trim.get() - ਢਲਾਨ_ਹੇਠਾਂ_trim * (_param_fw_airspd_trim.get() - calibrated_airspeed_sp);

	} ਨਹੀਂਤਾਂ ਜੇ (PX4_ISFINITE(calibrated_airspeed_sp) && PX4_ISFINITE(ਢਲਾਨ_above_trim)
		   && _param_fw_thr_aspd_max.get() > FLT_EPSILON
		   && calibrated_airspeed_sp > _param_fw_airspd_trim.get()) {
		throttle_trim = _param_fw_thr_trim.get() + ਢਲਾਨ_above_trim * (calibrated_airspeed_sp - _param_fw_airspd_trim.get());
	}

	ਵਾਪਸ throttle_trim;
}
ਦਸ਼ਮਲਵ PerformanceModel::get_Minimum_Sink_ਦਰ(ਦਸ਼ਮਲਵ air_density) ਅਚਲ
{
	air_density = sanitiseAirDensity(air_density);
	ਵਾਪਸ _param_fw_t_sink_min.get() * sqrtf(get_ਭਾਰ_ਅਨੁਪਾਤ() * kAirDensitySeaLevelStandardAtmos / air_density);
}
ਦਸ਼ਮਲਵ PerformanceModel::getCalibratedTrimAirspeed() ਅਚਲ
{
	ਵਾਪਸ math::constrain(_param_fw_airspd_trim.get() * sqrtf(get_ਭਾਰ_ਅਨੁਪਾਤ()), _param_fw_airspd_min.get(),
			       _param_fw_airspd_max.get());
}
ਦਸ਼ਮਲਵ PerformanceModel::getMinimumCalibratedAirspeed(ਦਸ਼ਮਲਵ ਲੱਦੋ_ਗੁਣਕ, ਦਸ਼ਮਲਵ flaps_setpoint) ਅਚਲ
{
	ਲੱਦੋ_ਗੁਣਕ = math::max(ਲੱਦੋ_ਗੁਣਕ, FLT_EPSILON);
	ਅਚਲ ਦਸ਼ਮਲਵ airspeed_flap_ਗੁਣਕ = math::lerp(1.f, _param_fw_airspd_flp_sc.get(), flaps_setpoint);
	ਵਾਪਸ (_param_fw_airspd_min.get() * airspeed_flap_ਗੁਣਕ) * sqrtf(get_ਭਾਰ_ਅਨੁਪਾਤ() * ਲੱਦੋ_ਗੁਣਕ);
}

ਦਸ਼ਮਲਵ PerformanceModel::getCalibratedStallAirspeed(ਦਸ਼ਮਲਵ ਲੱਦੋ_ਗੁਣਕ, ਦਸ਼ਮਲਵ flaps_setpoint) ਅਚਲ
{
	ਲੱਦੋ_ਗੁਣਕ = math::max(ਲੱਦੋ_ਗੁਣਕ, FLT_EPSILON);
	ਅਚਲ ਦਸ਼ਮਲਵ airspeed_flap_ਗੁਣਕ = math::lerp(1.f, _param_fw_airspd_flp_sc.get(), flaps_setpoint);
	ਵਾਪਸ (_param_fw_airspd_stall.get() * airspeed_flap_ਗੁਣਕ) * sqrtf(get_ਭਾਰ_ਅਨੁਪਾਤ() * ਲੱਦੋ_ਗੁਣਕ);
}

ਦਸ਼ਮਲਵ PerformanceModel::get_ਵੱਧ_ਤੋਂ_ਵੱਧ_Calibrated_Airspeed() ਅਚਲ
{
	ਵਾਪਸ _param_fw_airspd_max.get();
}
ਤਾਰਕਿਕ PerformanceModel::ਲੜੀਵਾਰ_Sanity_Checks() ਅਚਲ
{
	ਤਾਰਕਿਕ ret = ਸੱਚ;

	// sanity check parameters
	ਜੇ (_param_fw_airspd_max.get() < _param_fw_airspd_min.get()) {
		/* EVENT
		 * @description
		 * - <param>FW_AIRSPD_MAX</param>: {1:.1}
		 * - <param>FW_AIRSPD_MIN</param>: {2:.1}
		 */
		events::send<ਦਸ਼ਮਲਵ, ਦਸ਼ਮਲਵ>(events::ID("fixedwing_position_control_conf_invalid_airspeed"), events::Log::Error,
					   "Invalid configuration: Airspeed max smaller than min",
					   _param_fw_airspd_max.get(), _param_fw_airspd_min.get());
		ret = ਝੂਠ;
	}

	ਜੇ (_param_fw_airspd_max.get() < 5.0f || _param_fw_airspd_min.get() > 100.0f) {
		/* EVENT
		 * @description
		 * - <param>FW_AIRSPD_MAX</param>: {1:.1}
		 * - <param>FW_AIRSPD_MIN</param>: {2:.1}
		 */
		events::send<ਦਸ਼ਮਲਵ, ਦਸ਼ਮਲਵ>(events::ID("fixedwing_position_control_conf_invalid_airspeed_bounds"), events::Log::Error,
					   "Invalid configuration: Airspeed max \\< 5 m/s or min \\> 100 m/s",
					   _param_fw_airspd_max.get(), _param_fw_airspd_min.get());
		ret = ਝੂਠ;
	}

	ਜੇ (_param_fw_airspd_trim.get() < _param_fw_airspd_min.get() ||
	    _param_fw_airspd_trim.get() > _param_fw_airspd_max.get()) {
		/* EVENT
		 * @description
		 * - <param>FW_AIRSPD_MAX</param>: {1:.1}
		 * - <param>FW_AIRSPD_MIN</param>: {2:.1}
		 * - <param>FW_AIRSPD_TRIM</param>: {3:.1}
		 */
		events::send<ਦਸ਼ਮਲਵ, ਦਸ਼ਮਲਵ, ਦਸ਼ਮਲਵ>(events::ID("fixedwing_position_control_conf_invalid_trim_bounds"),
						  events::Log::Error,
						  "Invalid configuration: Airspeed trim out of min or max bounds",
						  _param_fw_airspd_max.get(), _param_fw_airspd_min.get(), _param_fw_airspd_trim.get());
		ret = ਝੂਠ;
	}

	ਜੇ (_param_fw_airspd_stall.get() > _param_fw_airspd_min.get()) {
		/* EVENT
		 * @description
		 * - <param>FW_AIRSPD_MIN</param>: {1:.1}
		 * - <param>FW_AIRSPD_STALL</param>: {2:.1}
		 */
		events::send<ਦਸ਼ਮਲਵ, ਦਸ਼ਮਲਵ>(events::ID("fixedwing_position_control_conf_invalid_stall"), events::Log::Error,
					   "Invalid configuration: FW_AIRSPD_STALL higher FW_AIRSPD_MIN",
					   _param_fw_airspd_min.get(), _param_fw_airspd_stall.get());
		ret = ਝੂਠ;
	}

	ਅਚਲ ਦਸ਼ਮਲਵ max_bank_loadfactor = 1 / cosf(math::radians(_param_fw_r_lim.get()));
	ਅਚਲ ਦਸ਼ਮਲਵ min_airspd_at_max_bank = getMinimumCalibratedAirspeed(max_bank_loadfactor, /*flaps_setpoint = */0.0f);

	ਜੇ (min_airspd_at_max_bank > _param_fw_airspd_max.get()) {

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
		events::send<ਦਸ਼ਮਲਵ, ਦਸ਼ਮਲਵ, ਦਸ਼ਮਲਵ>(events::ID("fixedwing_position_control_conf_invalid_maxbank_infeasible"), events::Log::Error,
						  "Invalid configuration: FW_AIRSPD_MAX too low to sustain max bank angle",
						  _param_fw_airspd_min.get(), _param_fw_airspd_max.get(), _param_fw_r_lim.get());
		ret = ਝੂਠ;
	}

	ਵਾਪਸ ret;

}
ਖਾਲੀ PerformanceModel::	updateParameters()
{
	updateParams();
}

ਦਸ਼ਮਲਵ PerformanceModel::sanitiseAirDensity(ਦਸ਼ਮਲਵ air_density)
{
	ਜੇ (!PX4_ISFINITE(air_density)) {
		air_density = kAirDensitySeaLevelStandardAtmos;
	}

	ਵਾਪਸ math::max(air_density, kAirDensityStandardAtmos11000Amsl);
}

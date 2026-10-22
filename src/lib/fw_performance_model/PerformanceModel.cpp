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

#ಅನ್ತರ್ಭಾವ <geo/geo.h>
#ಅನ್ತರ್ಭಾವ <px4_platform_common/events.h>
#ಅನ್ತರ್ಭಾವ "PerformanceModel.hpp"
#ಅನ್ತರ್ಭಾವ <lib/atmosphere/atmosphere.h>

ಬಳಸಿ ನಾಮಾಕಾಶ atmosphere;


// [.] minimum ratio between the actual vehicle weight and the vehicle nominal weight (weight at which the performance limits are derived)
ಸ್ಥಿರ constexpr ದಶಮಾಂಶ k_Min_ತೂಕ_ಅನುಪಾತ = 0.5f;

// [.] maximum ratio between the actual vehicle weight and the vehicle nominal weight (weight at which the performance limits are derived)
ಸ್ಥಿರ constexpr ದಶಮಾಂಶ k_Max_ತೂಕ_ಅನುಪಾತ = 2.0f;

// [m/s] climbrate defining the service ceiling, used to compensate max climbrate based on air density
ಸ್ಥಿರ constexpr ದಶಮಾಂಶ k_Climb_ದರ_At_Service_Ceiling = 0.5f;

PerformanceModel::PerformanceModel(): ModuleParams(ಶೂನ್ಯಸೂಚಕ)
{
	updateParams();
}
ಶೂನ್ಯ PerformanceModel::ಗಣ_Fuel_Fraction_ಉಳಿದ(ದಶಮಾಂಶ fuel_fraction_ಉಳಿದ)
{
	ಆದರೆ (PX4_ISFINITE(fuel_fraction_ಉಳಿದ)) {
		fuel_fraction_ಉಳಿದ_2 = math::constrain(fuel_fraction_ಉಳಿದ, 0.f, 1.f);
	}
}

ದಶಮಾಂಶ PerformanceModel::get_Fuel_Fraction_ಉಳಿದ(ಸ್ಥಿರಾಂಕ fuel_tank_ಸ್ಥಿತಿ_s &fuel_tank_ಸ್ಥಿತಿ)
{
	ದಶಮಾಂಶ fraction = NAN;

	ಆದರೆ (fuel_tank_ಸ್ಥಿತಿ.percent_ಉಳಿದ <= 100) {
		fraction = fuel_tank_ಸ್ಥಿತಿ.percent_ಉಳಿದ * 0.01f;

	} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (PX4_ISFINITE(fuel_tank_ಸ್ಥಿತಿ.ಉಳಿದ_fuel) && fuel_tank_ಸ್ಥಿತಿ.ಗರಿಷ್ಠ_fuel_ಸಾಮರ್ಥ್ಯ > FLT_EPSILON) {
		fraction = fuel_tank_ಸ್ಥಿತಿ.ಉಳಿದ_fuel / fuel_tank_ಸ್ಥಿತಿ.ಗರಿಷ್ಠ_fuel_ಸಾಮರ್ಥ್ಯ;

	} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (PX4_ISFINITE(fuel_tank_ಸ್ಥಿತಿ.consumed_fuel) && fuel_tank_ಸ್ಥಿತಿ.ಗರಿಷ್ಠ_fuel_ಸಾಮರ್ಥ್ಯ > FLT_EPSILON) {
		fraction = 1.f - fuel_tank_ಸ್ಥಿತಿ.consumed_fuel / fuel_tank_ಸ್ಥಿತಿ.ಗರಿಷ್ಠ_fuel_ಸಾಮರ್ಥ್ಯ;
	}

	ಹಿಂತಿರುಗಿ PX4_ISFINITE(fraction) ? math::constrain(fraction, 0.f, 1.f) : NAN;
}

ದಶಮಾಂಶ PerformanceModel::get_ತೂಕ_ಅನುಪಾತ() ಸ್ಥಿರಾಂಕ
{
	ದಶಮಾಂಶ ತೂಕ_ಗುಣಕ = 1.0f;

	ಆದರೆ (param_ತೂಕ_ತಳ.get() > FLT_EPSILON && param_ತೂಕ_gross.get() > FLT_EPSILON) {
		ದಶಮಾಂಶ ತೂಕ = param_ತೂಕ_gross.get();

		ಆದರೆ (param_ತೂಕ_fuel.get() > FLT_EPSILON && PX4_ISFINITE(fuel_fraction_ಉಳಿದ_2)) {
			ತೂಕ -= (1.f - fuel_fraction_ಉಳಿದ_2) * param_ತೂಕ_fuel.get();
		}

		ತೂಕ_ಗುಣಕ = math::constrain(ತೂಕ / param_ತೂಕ_ತಳ.get(), k_Min_ತೂಕ_ಅನುಪಾತ,
						k_Max_ತೂಕ_ಅನುಪಾತ);
	}

	ಹಿಂತಿರುಗಿ ತೂಕ_ಗುಣಕ;
}
ದಶಮಾಂಶ PerformanceModel::get_ಗರಿಷ್ಠ_Climb_ದರ(ದಶಮಾಂಶ air_density) ಸ್ಥಿರಾಂಕ
{
	air_density = sanitiseAirDensity(air_density);
	ದಶಮಾಂಶ climbrate_max = _param_fw_t_clmb_max.get();

	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ service_ceiling = _param_service_ceiling.get();

	ಆದರೆ (service_ceiling > FLT_EPSILON) {
		ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ ceiling_pressure = getPressureFromAltitude(service_ceiling);
		ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ ceiling_density = get_Density_From_Pressure_And_ತಾತ್ಕಾಲಿಕ(ceiling_pressure,
					      getStandardTemperatureAtAltitude(service_ceiling));
		ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ climbrate_gradient = math::max((_param_fw_t_clmb_max.get() - k_Climb_ದರ_At_Service_Ceiling) /
						 (kAirDensitySeaLevelStandardAtmos -
						  ceiling_density), 0.0f);
		ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ delta_rho = air_density - kAirDensitySeaLevelStandardAtmos;
		climbrate_max = math::constrain(_param_fw_t_clmb_max.get() + climbrate_gradient * delta_rho, k_Climb_ದರ_At_Service_Ceiling,
						_param_fw_t_clmb_max.get());
	}

	ಹಿಂತಿರುಗಿ climbrate_max / get_ತೂಕ_ಅನುಪಾತ();
}
ದಶಮಾಂಶ PerformanceModel::getTrimThrottle(ದಶಮಾಂಶ throttle_min, ದಶಮಾಂಶ throttle_max, ದಶಮಾಂಶ airspeed_sp,
					ದಶಮಾಂಶ air_density) ಸ್ಥಿರಾಂಕ
{
	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ throttle_trim = getTrimThrottleForCalibratedAirspeed(airspeed_sp) * getAirDensityThrottleScale(
					    air_density);
	ಹಿಂತಿರುಗಿ math::constrain(throttle_trim, throttle_min, throttle_max);
}

ದಶಮಾಂಶ PerformanceModel::getAirDensityThrottleScale(ದಶಮಾಂಶ air_density) ಸ್ಥಿರಾಂಕ
{
	air_density = sanitiseAirDensity(air_density);
	ದಶಮಾಂಶ air_density_throttle_scale = 1.0f;

	// scale throttle as a function of sqrt(rho0/rho)
	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ eas2tas = sqrtf(kAirDensitySeaLevelStandardAtmos / air_density);
	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ eas2tas_at_11km_amsl = sqrtf(kAirDensitySeaLevelStandardAtmos / kAirDensityStandardAtmos11000Amsl);
	air_density_throttle_scale = math::constrain(eas2tas, 1.f, eas2tas_at_11km_amsl);

	ಹಿಂತಿರುಗಿ air_density_throttle_scale;
}
ದಶಮಾಂಶ PerformanceModel::getTrimThrottleForCalibratedAirspeed(ದಶಮಾಂಶ calibrated_airspeed_sp) ಸ್ಥಿರಾಂಕ
{
	ದಶಮಾಂಶ throttle_trim =
		_param_fw_thr_trim.get(); // throttle required for level flight at trim airspeed, at sea level (standard atmosphere)

	// Drag modelling (parasite drag): calculate mapping airspeed-->throttle, assuming a linear relation with different gradients
// above and below trim. This is tunable thorugh FW_THR_ASPD_MIN and FW_THR_ASPD_MAX.
	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ ಇಳಿಜಾರು_ಕೆಳಗೆ_trim = (_param_fw_thr_trim.get() - _param_fw_thr_aspd_min.get()) /
				       (_param_fw_airspd_trim.get() - _param_fw_airspd_min.get());
	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ ಇಳಿಜಾರು_above_trim = (_param_fw_thr_aspd_max.get() - _param_fw_thr_trim.get()) /
				       (_param_fw_airspd_max.get() - _param_fw_airspd_trim.get());

	ಆದರೆ (PX4_ISFINITE(calibrated_airspeed_sp) && PX4_ISFINITE(ಇಳಿಜಾರು_ಕೆಳಗೆ_trim) && _param_fw_thr_aspd_min.get() > FLT_EPSILON
	    && calibrated_airspeed_sp < _param_fw_airspd_trim.get()) {
		throttle_trim = _param_fw_thr_trim.get() - ಇಳಿಜಾರು_ಕೆಳಗೆ_trim * (_param_fw_airspd_trim.get() - calibrated_airspeed_sp);

	} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (PX4_ISFINITE(calibrated_airspeed_sp) && PX4_ISFINITE(ಇಳಿಜಾರು_above_trim)
		   && _param_fw_thr_aspd_max.get() > FLT_EPSILON
		   && calibrated_airspeed_sp > _param_fw_airspd_trim.get()) {
		throttle_trim = _param_fw_thr_trim.get() + ಇಳಿಜಾರು_above_trim * (calibrated_airspeed_sp - _param_fw_airspd_trim.get());
	}

	ಹಿಂತಿರುಗಿ throttle_trim;
}
ದಶಮಾಂಶ PerformanceModel::get_Minimum_Sink_ದರ(ದಶಮಾಂಶ air_density) ಸ್ಥಿರಾಂಕ
{
	air_density = sanitiseAirDensity(air_density);
	ಹಿಂತಿರುಗಿ _param_fw_t_sink_min.get() * sqrtf(get_ತೂಕ_ಅನುಪಾತ() * kAirDensitySeaLevelStandardAtmos / air_density);
}
ದಶಮಾಂಶ PerformanceModel::getCalibratedTrimAirspeed() ಸ್ಥಿರಾಂಕ
{
	ಹಿಂತಿರುಗಿ math::constrain(_param_fw_airspd_trim.get() * sqrtf(get_ತೂಕ_ಅನುಪಾತ()), _param_fw_airspd_min.get(),
			       _param_fw_airspd_max.get());
}
ದಶಮಾಂಶ PerformanceModel::getMinimumCalibratedAirspeed(ದಶಮಾಂಶ ತುಂಬು_ಗುಣಕ, ದಶಮಾಂಶ flaps_setpoint) ಸ್ಥಿರಾಂಕ
{
	ತುಂಬು_ಗುಣಕ = math::max(ತುಂಬು_ಗುಣಕ, FLT_EPSILON);
	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ airspeed_flap_ಗುಣಕ = math::lerp(1.f, _param_fw_airspd_flp_sc.get(), flaps_setpoint);
	ಹಿಂತಿರುಗಿ (_param_fw_airspd_min.get() * airspeed_flap_ಗುಣಕ) * sqrtf(get_ತೂಕ_ಅನುಪಾತ() * ತುಂಬು_ಗುಣಕ);
}

ದಶಮಾಂಶ PerformanceModel::getCalibratedStallAirspeed(ದಶಮಾಂಶ ತುಂಬು_ಗುಣಕ, ದಶಮಾಂಶ flaps_setpoint) ಸ್ಥಿರಾಂಕ
{
	ತುಂಬು_ಗುಣಕ = math::max(ತುಂಬು_ಗುಣಕ, FLT_EPSILON);
	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ airspeed_flap_ಗುಣಕ = math::lerp(1.f, _param_fw_airspd_flp_sc.get(), flaps_setpoint);
	ಹಿಂತಿರುಗಿ (_param_fw_airspd_stall.get() * airspeed_flap_ಗುಣಕ) * sqrtf(get_ತೂಕ_ಅನುಪಾತ() * ತುಂಬು_ಗುಣಕ);
}

ದಶಮಾಂಶ PerformanceModel::get_ಗರಿಷ್ಠ_Calibrated_Airspeed() ಸ್ಥಿರಾಂಕ
{
	ಹಿಂತಿರುಗಿ _param_fw_airspd_max.get();
}
ತಾರ್ಕಿಕ PerformanceModel::ಸತತ_Sanity_Checks() ಸ್ಥಿರಾಂಕ
{
	ತಾರ್ಕಿಕ ret = ನಿಜ;

	// sanity check parameters
	ಆದರೆ (_param_fw_airspd_max.get() < _param_fw_airspd_min.get()) {
		/* EVENT
		 * @description
		 * - <param>FW_AIRSPD_MAX</param>: {1:.1}
		 * - <param>FW_AIRSPD_MIN</param>: {2:.1}
		 */
		events::send<ದಶಮಾಂಶ, ದಶಮಾಂಶ>(events::ID("fixedwing_position_control_conf_invalid_airspeed"), events::Log::Error,
					   "Invalid configuration: Airspeed max smaller than min",
					   _param_fw_airspd_max.get(), _param_fw_airspd_min.get());
		ret = ಸುಳ್ಳು;
	}

	ಆದರೆ (_param_fw_airspd_max.get() < 5.0f || _param_fw_airspd_min.get() > 100.0f) {
		/* EVENT
		 * @description
		 * - <param>FW_AIRSPD_MAX</param>: {1:.1}
		 * - <param>FW_AIRSPD_MIN</param>: {2:.1}
		 */
		events::send<ದಶಮಾಂಶ, ದಶಮಾಂಶ>(events::ID("fixedwing_position_control_conf_invalid_airspeed_bounds"), events::Log::Error,
					   "Invalid configuration: Airspeed max \\< 5 m/s or min \\> 100 m/s",
					   _param_fw_airspd_max.get(), _param_fw_airspd_min.get());
		ret = ಸುಳ್ಳು;
	}

	ಆದರೆ (_param_fw_airspd_trim.get() < _param_fw_airspd_min.get() ||
	    _param_fw_airspd_trim.get() > _param_fw_airspd_max.get()) {
		/* EVENT
		 * @description
		 * - <param>FW_AIRSPD_MAX</param>: {1:.1}
		 * - <param>FW_AIRSPD_MIN</param>: {2:.1}
		 * - <param>FW_AIRSPD_TRIM</param>: {3:.1}
		 */
		events::send<ದಶಮಾಂಶ, ದಶಮಾಂಶ, ದಶಮಾಂಶ>(events::ID("fixedwing_position_control_conf_invalid_trim_bounds"),
						  events::Log::Error,
						  "Invalid configuration: Airspeed trim out of min or max bounds",
						  _param_fw_airspd_max.get(), _param_fw_airspd_min.get(), _param_fw_airspd_trim.get());
		ret = ಸುಳ್ಳು;
	}

	ಆದರೆ (_param_fw_airspd_stall.get() > _param_fw_airspd_min.get()) {
		/* EVENT
		 * @description
		 * - <param>FW_AIRSPD_MIN</param>: {1:.1}
		 * - <param>FW_AIRSPD_STALL</param>: {2:.1}
		 */
		events::send<ದಶಮಾಂಶ, ದಶಮಾಂಶ>(events::ID("fixedwing_position_control_conf_invalid_stall"), events::Log::Error,
					   "Invalid configuration: FW_AIRSPD_STALL higher FW_AIRSPD_MIN",
					   _param_fw_airspd_min.get(), _param_fw_airspd_stall.get());
		ret = ಸುಳ್ಳು;
	}

	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ max_bank_loadfactor = 1 / cosf(math::radians(_param_fw_r_lim.get()));
	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ min_airspd_at_max_bank = getMinimumCalibratedAirspeed(max_bank_loadfactor, /*flaps_setpoint = */0.0f);

	ಆದರೆ (min_airspd_at_max_bank > _param_fw_airspd_max.get()) {

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
		events::send<ದಶಮಾಂಶ, ದಶಮಾಂಶ, ದಶಮಾಂಶ>(events::ID("fixedwing_position_control_conf_invalid_maxbank_infeasible"), events::Log::Error,
						  "Invalid configuration: FW_AIRSPD_MAX too low to sustain max bank angle",
						  _param_fw_airspd_min.get(), _param_fw_airspd_max.get(), _param_fw_r_lim.get());
		ret = ಸುಳ್ಳು;
	}

	ಹಿಂತಿರುಗಿ ret;

}
ಶೂನ್ಯ PerformanceModel::	updateParameters()
{
	updateParams();
}

ದಶಮಾಂಶ PerformanceModel::sanitiseAirDensity(ದಶಮಾಂಶ air_density)
{
	ಆದರೆ (!PX4_ISFINITE(air_density)) {
		air_density = kAirDensitySeaLevelStandardAtmos;
	}

	ಹಿಂತಿರುಗಿ math::max(air_density, kAirDensityStandardAtmos11000Amsl);
}

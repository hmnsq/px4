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

#ضمن <geo/geo.h>
#ضمن <px4_platform_common/events.h>
#ضمن "PerformanceModel.hpp"
#ضمن <lib/atmosphere/atmosphere.h>

کارول نطاق atmosphere;


// [.] minimum ratio between the actual vehicle weight and the vehicle nominal weight (weight at which the performance limits are derived)
ساكن constexpr عائم k_Min_وزن_نسبت = 0.5f;

// [.] maximum ratio between the actual vehicle weight and the vehicle nominal weight (weight at which the performance limits are derived)
ساكن constexpr عائم k_Max_وزن_نسبت = 2.0f;

// [m/s] climbrate defining the service ceiling, used to compensate max climbrate based on air density
ساكن constexpr عائم k_Climb_شرح_At_Service_Ceiling = 0.5f;

PerformanceModel::PerformanceModel(): ModuleParams(تش_اشاره)
{
	updateParams();
}
فراغ PerformanceModel::سېټ_Fuel_Fraction_پاتې_شوی(عائم fuel_fraction_پاتې_شوی)
{
	إذا (PX4_ISFINITE(fuel_fraction_پاتې_شوی)) {
		fuel_fraction_پاتې_شوی_2 = math::constrain(fuel_fraction_پاتې_شوی, 0.f, 1.f);
	}
}

عائم PerformanceModel::get_Fuel_Fraction_پاتې_شوی(ثابت fuel_tank_حالت_s &fuel_tank_حالت)
{
	عائم fraction = NAN;

	إذا (fuel_tank_حالت.percent_پاتې_شوی <= 100) {
		fraction = fuel_tank_حالت.percent_پاتې_شوی * 0.01f;

	} وإلا إذا (PX4_ISFINITE(fuel_tank_حالت.پاتې_شوی_fuel) && fuel_tank_حالت.اعظمي_fuel_ظرفیت > FLT_EPSILON) {
		fraction = fuel_tank_حالت.پاتې_شوی_fuel / fuel_tank_حالت.اعظمي_fuel_ظرفیت;

	} وإلا إذا (PX4_ISFINITE(fuel_tank_حالت.consumed_fuel) && fuel_tank_حالت.اعظمي_fuel_ظرفیت > FLT_EPSILON) {
		fraction = 1.f - fuel_tank_حالت.consumed_fuel / fuel_tank_حالت.اعظمي_fuel_ظرفیت;
	}

	أرجع PX4_ISFINITE(fraction) ? math::constrain(fraction, 0.f, 1.f) : NAN;
}

عائم PerformanceModel::get_وزن_نسبت() ثابت
{
	عائم وزن_ضریب = 1.0f;

	إذا (param_وزن_بنسټ.get() > FLT_EPSILON && param_وزن_gross.get() > FLT_EPSILON) {
		عائم وزن = param_وزن_gross.get();

		إذا (param_وزن_fuel.get() > FLT_EPSILON && PX4_ISFINITE(fuel_fraction_پاتې_شوی_2)) {
			وزن -= (1.f - fuel_fraction_پاتې_شوی_2) * param_وزن_fuel.get();
		}

		وزن_ضریب = math::constrain(وزن / param_وزن_بنسټ.get(), k_Min_وزن_نسبت,
						k_Max_وزن_نسبت);
	}

	أرجع وزن_ضریب;
}
عائم PerformanceModel::get_اعظمي_Climb_شرح(عائم air_density) ثابت
{
	air_density = sanitiseAirDensity(air_density);
	عائم climbrate_max = _param_fw_t_clmb_max.get();

	ثابت عائم service_ceiling = _param_service_ceiling.get();

	إذا (service_ceiling > FLT_EPSILON) {
		ثابت عائم ceiling_pressure = getPressureFromAltitude(service_ceiling);
		ثابت عائم ceiling_density = get_Density_From_Pressure_And_لنډمهاله(ceiling_pressure,
					      getStandardTemperatureAtAltitude(service_ceiling));
		ثابت عائم climbrate_gradient = math::max((_param_fw_t_clmb_max.get() - k_Climb_شرح_At_Service_Ceiling) /
						 (kAirDensitySeaLevelStandardAtmos -
						  ceiling_density), 0.0f);
		ثابت عائم delta_rho = air_density - kAirDensitySeaLevelStandardAtmos;
		climbrate_max = math::constrain(_param_fw_t_clmb_max.get() + climbrate_gradient * delta_rho, k_Climb_شرح_At_Service_Ceiling,
						_param_fw_t_clmb_max.get());
	}

	أرجع climbrate_max / get_وزن_نسبت();
}
عائم PerformanceModel::getTrimThrottle(عائم throttle_min, عائم throttle_max, عائم airspeed_sp,
					عائم air_density) ثابت
{
	ثابت عائم throttle_trim = getTrimThrottleForCalibratedAirspeed(airspeed_sp) * getAirDensityThrottleScale(
					    air_density);
	أرجع math::constrain(throttle_trim, throttle_min, throttle_max);
}

عائم PerformanceModel::getAirDensityThrottleScale(عائم air_density) ثابت
{
	air_density = sanitiseAirDensity(air_density);
	عائم air_density_throttle_scale = 1.0f;

	// scale throttle as a function of sqrt(rho0/rho)
	ثابت عائم eas2tas = sqrtf(kAirDensitySeaLevelStandardAtmos / air_density);
	ثابت عائم eas2tas_at_11km_amsl = sqrtf(kAirDensitySeaLevelStandardAtmos / kAirDensityStandardAtmos11000Amsl);
	air_density_throttle_scale = math::constrain(eas2tas, 1.f, eas2tas_at_11km_amsl);

	أرجع air_density_throttle_scale;
}
عائم PerformanceModel::getTrimThrottleForCalibratedAirspeed(عائم calibrated_airspeed_sp) ثابت
{
	عائم throttle_trim =
		_param_fw_thr_trim.get(); // throttle required for level flight at trim airspeed, at sea level (standard atmosphere)

	// Drag modelling (parasite drag): calculate mapping airspeed-->throttle, assuming a linear relation with different gradients
// above and below trim. This is tunable thorugh FW_THR_ASPD_MIN and FW_THR_ASPD_MAX.
	ثابت عائم میلان_لاندې_trim = (_param_fw_thr_trim.get() - _param_fw_thr_aspd_min.get()) /
				       (_param_fw_airspd_trim.get() - _param_fw_airspd_min.get());
	ثابت عائم میلان_above_trim = (_param_fw_thr_aspd_max.get() - _param_fw_thr_trim.get()) /
				       (_param_fw_airspd_max.get() - _param_fw_airspd_trim.get());

	إذا (PX4_ISFINITE(calibrated_airspeed_sp) && PX4_ISFINITE(میلان_لاندې_trim) && _param_fw_thr_aspd_min.get() > FLT_EPSILON
	    && calibrated_airspeed_sp < _param_fw_airspd_trim.get()) {
		throttle_trim = _param_fw_thr_trim.get() - میلان_لاندې_trim * (_param_fw_airspd_trim.get() - calibrated_airspeed_sp);

	} وإلا إذا (PX4_ISFINITE(calibrated_airspeed_sp) && PX4_ISFINITE(میلان_above_trim)
		   && _param_fw_thr_aspd_max.get() > FLT_EPSILON
		   && calibrated_airspeed_sp > _param_fw_airspd_trim.get()) {
		throttle_trim = _param_fw_thr_trim.get() + میلان_above_trim * (calibrated_airspeed_sp - _param_fw_airspd_trim.get());
	}

	أرجع throttle_trim;
}
عائم PerformanceModel::get_Minimum_Sink_شرح(عائم air_density) ثابت
{
	air_density = sanitiseAirDensity(air_density);
	أرجع _param_fw_t_sink_min.get() * sqrtf(get_وزن_نسبت() * kAirDensitySeaLevelStandardAtmos / air_density);
}
عائم PerformanceModel::getCalibratedTrimAirspeed() ثابت
{
	أرجع math::constrain(_param_fw_airspd_trim.get() * sqrtf(get_وزن_نسبت()), _param_fw_airspd_min.get(),
			       _param_fw_airspd_max.get());
}
عائم PerformanceModel::getMinimumCalibratedAirspeed(عائم بارول_ضریب, عائم flaps_setpoint) ثابت
{
	بارول_ضریب = math::max(بارول_ضریب, FLT_EPSILON);
	ثابت عائم airspeed_flap_ضریب = math::lerp(1.f, _param_fw_airspd_flp_sc.get(), flaps_setpoint);
	أرجع (_param_fw_airspd_min.get() * airspeed_flap_ضریب) * sqrtf(get_وزن_نسبت() * بارول_ضریب);
}

عائم PerformanceModel::getCalibratedStallAirspeed(عائم بارول_ضریب, عائم flaps_setpoint) ثابت
{
	بارول_ضریب = math::max(بارول_ضریب, FLT_EPSILON);
	ثابت عائم airspeed_flap_ضریب = math::lerp(1.f, _param_fw_airspd_flp_sc.get(), flaps_setpoint);
	أرجع (_param_fw_airspd_stall.get() * airspeed_flap_ضریب) * sqrtf(get_وزن_نسبت() * بارول_ضریب);
}

عائم PerformanceModel::get_اعظمي_Calibrated_Airspeed() ثابت
{
	أرجع _param_fw_airspd_max.get();
}
منطقي PerformanceModel::پرلپسې_Sanity_Checks() ثابت
{
	منطقي ret = ریښتیا;

	// sanity check parameters
	إذا (_param_fw_airspd_max.get() < _param_fw_airspd_min.get()) {
		/* EVENT
		 * @description
		 * - <param>FW_AIRSPD_MAX</param>: {1:.1}
		 * - <param>FW_AIRSPD_MIN</param>: {2:.1}
		 */
		events::send<عائم, عائم>(events::ID("fixedwing_position_control_conf_invalid_airspeed"), events::Log::Error,
					   "Invalid configuration: Airspeed max smaller than min",
					   _param_fw_airspd_max.get(), _param_fw_airspd_min.get());
		ret = خطأ;
	}

	إذا (_param_fw_airspd_max.get() < 5.0f || _param_fw_airspd_min.get() > 100.0f) {
		/* EVENT
		 * @description
		 * - <param>FW_AIRSPD_MAX</param>: {1:.1}
		 * - <param>FW_AIRSPD_MIN</param>: {2:.1}
		 */
		events::send<عائم, عائم>(events::ID("fixedwing_position_control_conf_invalid_airspeed_bounds"), events::Log::Error,
					   "Invalid configuration: Airspeed max \\< 5 m/s or min \\> 100 m/s",
					   _param_fw_airspd_max.get(), _param_fw_airspd_min.get());
		ret = خطأ;
	}

	إذا (_param_fw_airspd_trim.get() < _param_fw_airspd_min.get() ||
	    _param_fw_airspd_trim.get() > _param_fw_airspd_max.get()) {
		/* EVENT
		 * @description
		 * - <param>FW_AIRSPD_MAX</param>: {1:.1}
		 * - <param>FW_AIRSPD_MIN</param>: {2:.1}
		 * - <param>FW_AIRSPD_TRIM</param>: {3:.1}
		 */
		events::send<عائم, عائم, عائم>(events::ID("fixedwing_position_control_conf_invalid_trim_bounds"),
						  events::Log::Error,
						  "Invalid configuration: Airspeed trim out of min or max bounds",
						  _param_fw_airspd_max.get(), _param_fw_airspd_min.get(), _param_fw_airspd_trim.get());
		ret = خطأ;
	}

	إذا (_param_fw_airspd_stall.get() > _param_fw_airspd_min.get()) {
		/* EVENT
		 * @description
		 * - <param>FW_AIRSPD_MIN</param>: {1:.1}
		 * - <param>FW_AIRSPD_STALL</param>: {2:.1}
		 */
		events::send<عائم, عائم>(events::ID("fixedwing_position_control_conf_invalid_stall"), events::Log::Error,
					   "Invalid configuration: FW_AIRSPD_STALL higher FW_AIRSPD_MIN",
					   _param_fw_airspd_min.get(), _param_fw_airspd_stall.get());
		ret = خطأ;
	}

	ثابت عائم max_bank_loadfactor = 1 / cosf(math::radians(_param_fw_r_lim.get()));
	ثابت عائم min_airspd_at_max_bank = getMinimumCalibratedAirspeed(max_bank_loadfactor, /*flaps_setpoint = */0.0f);

	إذا (min_airspd_at_max_bank > _param_fw_airspd_max.get()) {

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
		events::send<عائم, عائم, عائم>(events::ID("fixedwing_position_control_conf_invalid_maxbank_infeasible"), events::Log::Error,
						  "Invalid configuration: FW_AIRSPD_MAX too low to sustain max bank angle",
						  _param_fw_airspd_min.get(), _param_fw_airspd_max.get(), _param_fw_r_lim.get());
		ret = خطأ;
	}

	أرجع ret;

}
فراغ PerformanceModel::	updateParameters()
{
	updateParams();
}

عائم PerformanceModel::sanitiseAirDensity(عائم air_density)
{
	إذا (!PX4_ISFINITE(air_density)) {
		air_density = kAirDensitySeaLevelStandardAtmos;
	}

	أرجع math::max(air_density, kAirDensityStandardAtmos11000Amsl);
}

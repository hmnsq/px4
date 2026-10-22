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

استعمال نامخلا atmosphere;


// [.] minimum ratio between the actual vehicle weight and the vehicle nominal weight (weight at which the performance limits are derived)
ساکن constexpr اعشاری k_Min_وزن_تناسب = 0.5f;

// [.] maximum ratio between the actual vehicle weight and the vehicle nominal weight (weight at which the performance limits are derived)
ساکن constexpr اعشاری k_Max_وزن_تناسب = 2.0f;

// [m/s] climbrate defining the service ceiling, used to compensate max climbrate based on air density
ساکن constexpr اعشاری k_Climb_شرح_At_Service_Ceiling = 0.5f;

PerformanceModel::PerformanceModel(): ModuleParams(خالی_اشاریہ)
{
	updateParams();
}
خالی PerformanceModel::سیٹ_Fuel_Fraction_بقایا(اعشاری fuel_fraction_بقایا)
{
	اگر (PX4_ISFINITE(fuel_fraction_بقایا)) {
		fuel_fraction_بقایا_2 = math::constrain(fuel_fraction_بقایا, 0.f, 1.f);
	}
}

اعشاری PerformanceModel::get_Fuel_Fraction_بقایا(ثابت fuel_tank_حالت_s &fuel_tank_حالت)
{
	اعشاری fraction = NAN;

	اگر (fuel_tank_حالت.percent_بقایا <= 100) {
		fraction = fuel_tank_حالت.percent_بقایا * 0.01f;

	} ورنہ اگر (PX4_ISFINITE(fuel_tank_حالت.بقایا_fuel) && fuel_tank_حالت.زیادہ_سے_زیادہ_fuel_استعداد > FLT_EPSILON) {
		fraction = fuel_tank_حالت.بقایا_fuel / fuel_tank_حالت.زیادہ_سے_زیادہ_fuel_استعداد;

	} ورنہ اگر (PX4_ISFINITE(fuel_tank_حالت.consumed_fuel) && fuel_tank_حالت.زیادہ_سے_زیادہ_fuel_استعداد > FLT_EPSILON) {
		fraction = 1.f - fuel_tank_حالت.consumed_fuel / fuel_tank_حالت.زیادہ_سے_زیادہ_fuel_استعداد;
	}

	واپس PX4_ISFINITE(fraction) ? math::constrain(fraction, 0.f, 1.f) : NAN;
}

اعشاری PerformanceModel::get_وزن_تناسب() ثابت
{
	اعشاری وزن_ضربی_عدد = 1.0f;

	اگر (param_وزن_بنیاد.get() > FLT_EPSILON && param_وزن_gross.get() > FLT_EPSILON) {
		اعشاری وزن = param_وزن_gross.get();

		اگر (param_وزن_fuel.get() > FLT_EPSILON && PX4_ISFINITE(fuel_fraction_بقایا_2)) {
			وزن -= (1.f - fuel_fraction_بقایا_2) * param_وزن_fuel.get();
		}

		وزن_ضربی_عدد = math::constrain(وزن / param_وزن_بنیاد.get(), k_Min_وزن_تناسب,
						k_Max_وزن_تناسب);
	}

	واپس وزن_ضربی_عدد;
}
اعشاری PerformanceModel::get_زیادہ_سے_زیادہ_Climb_شرح(اعشاری air_density) ثابت
{
	air_density = sanitiseAirDensity(air_density);
	اعشاری climbrate_max = _param_fw_t_clmb_max.get();

	ثابت اعشاری service_ceiling = _param_service_ceiling.get();

	اگر (service_ceiling > FLT_EPSILON) {
		ثابت اعشاری ceiling_pressure = getPressureFromAltitude(service_ceiling);
		ثابت اعشاری ceiling_density = get_Density_From_Pressure_And_عارضی(ceiling_pressure,
					      getStandardTemperatureAtAltitude(service_ceiling));
		ثابت اعشاری climbrate_gradient = math::max((_param_fw_t_clmb_max.get() - k_Climb_شرح_At_Service_Ceiling) /
						 (kAirDensitySeaLevelStandardAtmos -
						  ceiling_density), 0.0f);
		ثابت اعشاری delta_rho = air_density - kAirDensitySeaLevelStandardAtmos;
		climbrate_max = math::constrain(_param_fw_t_clmb_max.get() + climbrate_gradient * delta_rho, k_Climb_شرح_At_Service_Ceiling,
						_param_fw_t_clmb_max.get());
	}

	واپس climbrate_max / get_وزن_تناسب();
}
اعشاری PerformanceModel::getTrimThrottle(اعشاری throttle_min, اعشاری throttle_max, اعشاری airspeed_sp,
					اعشاری air_density) ثابت
{
	ثابت اعشاری throttle_trim = getTrimThrottleForCalibratedAirspeed(airspeed_sp) * getAirDensityThrottleScale(
					    air_density);
	واپس math::constrain(throttle_trim, throttle_min, throttle_max);
}

اعشاری PerformanceModel::getAirDensityThrottleScale(اعشاری air_density) ثابت
{
	air_density = sanitiseAirDensity(air_density);
	اعشاری air_density_throttle_scale = 1.0f;

	// scale throttle as a function of sqrt(rho0/rho)
	ثابت اعشاری eas2tas = sqrtf(kAirDensitySeaLevelStandardAtmos / air_density);
	ثابت اعشاری eas2tas_at_11km_amsl = sqrtf(kAirDensitySeaLevelStandardAtmos / kAirDensityStandardAtmos11000Amsl);
	air_density_throttle_scale = math::constrain(eas2tas, 1.f, eas2tas_at_11km_amsl);

	واپس air_density_throttle_scale;
}
اعشاری PerformanceModel::getTrimThrottleForCalibratedAirspeed(اعشاری calibrated_airspeed_sp) ثابت
{
	اعشاری throttle_trim =
		_param_fw_thr_trim.get(); // throttle required for level flight at trim airspeed, at sea level (standard atmosphere)

	// Drag modelling (parasite drag): calculate mapping airspeed-->throttle, assuming a linear relation with different gradients
// above and below trim. This is tunable thorugh FW_THR_ASPD_MIN and FW_THR_ASPD_MAX.
	ثابت اعشاری ڈھلان_نیچے_trim = (_param_fw_thr_trim.get() - _param_fw_thr_aspd_min.get()) /
				       (_param_fw_airspd_trim.get() - _param_fw_airspd_min.get());
	ثابت اعشاری ڈھلان_above_trim = (_param_fw_thr_aspd_max.get() - _param_fw_thr_trim.get()) /
				       (_param_fw_airspd_max.get() - _param_fw_airspd_trim.get());

	اگر (PX4_ISFINITE(calibrated_airspeed_sp) && PX4_ISFINITE(ڈھلان_نیچے_trim) && _param_fw_thr_aspd_min.get() > FLT_EPSILON
	    && calibrated_airspeed_sp < _param_fw_airspd_trim.get()) {
		throttle_trim = _param_fw_thr_trim.get() - ڈھلان_نیچے_trim * (_param_fw_airspd_trim.get() - calibrated_airspeed_sp);

	} ورنہ اگر (PX4_ISFINITE(calibrated_airspeed_sp) && PX4_ISFINITE(ڈھلان_above_trim)
		   && _param_fw_thr_aspd_max.get() > FLT_EPSILON
		   && calibrated_airspeed_sp > _param_fw_airspd_trim.get()) {
		throttle_trim = _param_fw_thr_trim.get() + ڈھلان_above_trim * (calibrated_airspeed_sp - _param_fw_airspd_trim.get());
	}

	واپس throttle_trim;
}
اعشاری PerformanceModel::get_Minimum_Sink_شرح(اعشاری air_density) ثابت
{
	air_density = sanitiseAirDensity(air_density);
	واپس _param_fw_t_sink_min.get() * sqrtf(get_وزن_تناسب() * kAirDensitySeaLevelStandardAtmos / air_density);
}
اعشاری PerformanceModel::getCalibratedTrimAirspeed() ثابت
{
	واپس math::constrain(_param_fw_airspd_trim.get() * sqrtf(get_وزن_تناسب()), _param_fw_airspd_min.get(),
			       _param_fw_airspd_max.get());
}
اعشاری PerformanceModel::getMinimumCalibratedAirspeed(اعشاری بھرو_ضربی_عدد, اعشاری flaps_setpoint) ثابت
{
	بھرو_ضربی_عدد = math::max(بھرو_ضربی_عدد, FLT_EPSILON);
	ثابت اعشاری airspeed_flap_ضربی_عدد = math::lerp(1.f, _param_fw_airspd_flp_sc.get(), flaps_setpoint);
	واپس (_param_fw_airspd_min.get() * airspeed_flap_ضربی_عدد) * sqrtf(get_وزن_تناسب() * بھرو_ضربی_عدد);
}

اعشاری PerformanceModel::getCalibratedStallAirspeed(اعشاری بھرو_ضربی_عدد, اعشاری flaps_setpoint) ثابت
{
	بھرو_ضربی_عدد = math::max(بھرو_ضربی_عدد, FLT_EPSILON);
	ثابت اعشاری airspeed_flap_ضربی_عدد = math::lerp(1.f, _param_fw_airspd_flp_sc.get(), flaps_setpoint);
	واپس (_param_fw_airspd_stall.get() * airspeed_flap_ضربی_عدد) * sqrtf(get_وزن_تناسب() * بھرو_ضربی_عدد);
}

اعشاری PerformanceModel::get_زیادہ_سے_زیادہ_Calibrated_Airspeed() ثابت
{
	واپس _param_fw_airspd_max.get();
}
منطقی PerformanceModel::تسلسل_Sanity_Checks() ثابت
{
	منطقی ret = سچ;

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
		ret = جھوٹ;
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
		ret = جھوٹ;
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
		ret = جھوٹ;
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
		ret = جھوٹ;
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
		ret = جھوٹ;
	}

	واپس ret;

}
خالی PerformanceModel::	updateParameters()
{
	updateParams();
}

اعشاری PerformanceModel::sanitiseAirDensity(اعشاری air_density)
{
	اگر (!PX4_ISFINITE(air_density)) {
		air_density = kAirDensitySeaLevelStandardAtmos;
	}

	واپس math::max(air_density, kAirDensityStandardAtmos11000Amsl);
}

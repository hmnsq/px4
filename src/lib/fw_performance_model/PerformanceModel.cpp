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

#అన్తర్భావ <geo/geo.h>
#అన్తర్భావ <px4_platform_common/events.h>
#అన్తర్భావ "PerformanceModel.hpp"
#అన్తర్భావ <lib/atmosphere/atmosphere.h>

ఉపయోగించి నామాకాశ atmosphere;


// [.] minimum ratio between the actual vehicle weight and the vehicle nominal weight (weight at which the performance limits are derived)
స్థిర constexpr దశాంశం k_Min_బరువు_నిష్పత్తి = 0.5f;

// [.] maximum ratio between the actual vehicle weight and the vehicle nominal weight (weight at which the performance limits are derived)
స్థిర constexpr దశాంశం k_Max_బరువు_నిష్పత్తి = 2.0f;

// [m/s] climbrate defining the service ceiling, used to compensate max climbrate based on air density
స్థిర constexpr దశాంశం k_Climb_రేటు_At_Service_Ceiling = 0.5f;

PerformanceModel::PerformanceModel(): ModuleParams(శూన్యసూచిక)
{
	updateParams();
}
శూన్య PerformanceModel::సమితి_Fuel_Fraction_మిగిలిన(దశాంశం fuel_fraction_మిగిలిన)
{
	ఒకవేళ (PX4_ISFINITE(fuel_fraction_మిగిలిన)) {
		fuel_fraction_మిగిలిన_2 = math::constrain(fuel_fraction_మిగిలిన, 0.f, 1.f);
	}
}

దశాంశం PerformanceModel::get_Fuel_Fraction_మిగిలిన(స్థిరరాశి fuel_tank_స్థితి_s &fuel_tank_స్థితి)
{
	దశాంశం fraction = NAN;

	ఒకవేళ (fuel_tank_స్థితి.percent_మిగిలిన <= 100) {
		fraction = fuel_tank_స్థితి.percent_మిగిలిన * 0.01f;

	} లేకపోతే ఒకవేళ (PX4_ISFINITE(fuel_tank_స్థితి.మిగిలిన_fuel) && fuel_tank_స్థితి.గరిష్ఠ_fuel_సామర్థ్యం > FLT_EPSILON) {
		fraction = fuel_tank_స్థితి.మిగిలిన_fuel / fuel_tank_స్థితి.గరిష్ఠ_fuel_సామర్థ్యం;

	} లేకపోతే ఒకవేళ (PX4_ISFINITE(fuel_tank_స్థితి.consumed_fuel) && fuel_tank_స్థితి.గరిష్ఠ_fuel_సామర్థ్యం > FLT_EPSILON) {
		fraction = 1.f - fuel_tank_స్థితి.consumed_fuel / fuel_tank_స్థితి.గరిష్ఠ_fuel_సామర్థ్యం;
	}

	తిరిగి PX4_ISFINITE(fraction) ? math::constrain(fraction, 0.f, 1.f) : NAN;
}

దశాంశం PerformanceModel::get_బరువు_నిష్పత్తి() స్థిరరాశి
{
	దశాంశం బరువు_గుణకం = 1.0f;

	ఒకవేళ (param_బరువు_పునాది.get() > FLT_EPSILON && param_బరువు_gross.get() > FLT_EPSILON) {
		దశాంశం బరువు = param_బరువు_gross.get();

		ఒకవేళ (param_బరువు_fuel.get() > FLT_EPSILON && PX4_ISFINITE(fuel_fraction_మిగిలిన_2)) {
			బరువు -= (1.f - fuel_fraction_మిగిలిన_2) * param_బరువు_fuel.get();
		}

		బరువు_గుణకం = math::constrain(బరువు / param_బరువు_పునాది.get(), k_Min_బరువు_నిష్పత్తి,
						k_Max_బరువు_నిష్పత్తి);
	}

	తిరిగి బరువు_గుణకం;
}
దశాంశం PerformanceModel::get_గరిష్ఠ_Climb_రేటు(దశాంశం air_density) స్థిరరాశి
{
	air_density = sanitiseAirDensity(air_density);
	దశాంశం climbrate_max = _param_fw_t_clmb_max.get();

	స్థిరరాశి దశాంశం service_ceiling = _param_service_ceiling.get();

	ఒకవేళ (service_ceiling > FLT_EPSILON) {
		స్థిరరాశి దశాంశం ceiling_pressure = getPressureFromAltitude(service_ceiling);
		స్థిరరాశి దశాంశం ceiling_density = get_Density_From_Pressure_And_తాత్కాలిక(ceiling_pressure,
					      getStandardTemperatureAtAltitude(service_ceiling));
		స్థిరరాశి దశాంశం climbrate_gradient = math::max((_param_fw_t_clmb_max.get() - k_Climb_రేటు_At_Service_Ceiling) /
						 (kAirDensitySeaLevelStandardAtmos -
						  ceiling_density), 0.0f);
		స్థిరరాశి దశాంశం delta_rho = air_density - kAirDensitySeaLevelStandardAtmos;
		climbrate_max = math::constrain(_param_fw_t_clmb_max.get() + climbrate_gradient * delta_rho, k_Climb_రేటు_At_Service_Ceiling,
						_param_fw_t_clmb_max.get());
	}

	తిరిగి climbrate_max / get_బరువు_నిష్పత్తి();
}
దశాంశం PerformanceModel::getTrimThrottle(దశాంశం throttle_min, దశాంశం throttle_max, దశాంశం airspeed_sp,
					దశాంశం air_density) స్థిరరాశి
{
	స్థిరరాశి దశాంశం throttle_trim = getTrimThrottleForCalibratedAirspeed(airspeed_sp) * getAirDensityThrottleScale(
					    air_density);
	తిరిగి math::constrain(throttle_trim, throttle_min, throttle_max);
}

దశాంశం PerformanceModel::getAirDensityThrottleScale(దశాంశం air_density) స్థిరరాశి
{
	air_density = sanitiseAirDensity(air_density);
	దశాంశం air_density_throttle_scale = 1.0f;

	// scale throttle as a function of sqrt(rho0/rho)
	స్థిరరాశి దశాంశం eas2tas = sqrtf(kAirDensitySeaLevelStandardAtmos / air_density);
	స్థిరరాశి దశాంశం eas2tas_at_11km_amsl = sqrtf(kAirDensitySeaLevelStandardAtmos / kAirDensityStandardAtmos11000Amsl);
	air_density_throttle_scale = math::constrain(eas2tas, 1.f, eas2tas_at_11km_amsl);

	తిరిగి air_density_throttle_scale;
}
దశాంశం PerformanceModel::getTrimThrottleForCalibratedAirspeed(దశాంశం calibrated_airspeed_sp) స్థిరరాశి
{
	దశాంశం throttle_trim =
		_param_fw_thr_trim.get(); // throttle required for level flight at trim airspeed, at sea level (standard atmosphere)

	// Drag modelling (parasite drag): calculate mapping airspeed-->throttle, assuming a linear relation with different gradients
// above and below trim. This is tunable thorugh FW_THR_ASPD_MIN and FW_THR_ASPD_MAX.
	స్థిరరాశి దశాంశం వాలు_క్రింద_trim = (_param_fw_thr_trim.get() - _param_fw_thr_aspd_min.get()) /
				       (_param_fw_airspd_trim.get() - _param_fw_airspd_min.get());
	స్థిరరాశి దశాంశం వాలు_above_trim = (_param_fw_thr_aspd_max.get() - _param_fw_thr_trim.get()) /
				       (_param_fw_airspd_max.get() - _param_fw_airspd_trim.get());

	ఒకవేళ (PX4_ISFINITE(calibrated_airspeed_sp) && PX4_ISFINITE(వాలు_క్రింద_trim) && _param_fw_thr_aspd_min.get() > FLT_EPSILON
	    && calibrated_airspeed_sp < _param_fw_airspd_trim.get()) {
		throttle_trim = _param_fw_thr_trim.get() - వాలు_క్రింద_trim * (_param_fw_airspd_trim.get() - calibrated_airspeed_sp);

	} లేకపోతే ఒకవేళ (PX4_ISFINITE(calibrated_airspeed_sp) && PX4_ISFINITE(వాలు_above_trim)
		   && _param_fw_thr_aspd_max.get() > FLT_EPSILON
		   && calibrated_airspeed_sp > _param_fw_airspd_trim.get()) {
		throttle_trim = _param_fw_thr_trim.get() + వాలు_above_trim * (calibrated_airspeed_sp - _param_fw_airspd_trim.get());
	}

	తిరిగి throttle_trim;
}
దశాంశం PerformanceModel::get_Minimum_Sink_రేటు(దశాంశం air_density) స్థిరరాశి
{
	air_density = sanitiseAirDensity(air_density);
	తిరిగి _param_fw_t_sink_min.get() * sqrtf(get_బరువు_నిష్పత్తి() * kAirDensitySeaLevelStandardAtmos / air_density);
}
దశాంశం PerformanceModel::getCalibratedTrimAirspeed() స్థిరరాశి
{
	తిరిగి math::constrain(_param_fw_airspd_trim.get() * sqrtf(get_బరువు_నిష్పత్తి()), _param_fw_airspd_min.get(),
			       _param_fw_airspd_max.get());
}
దశాంశం PerformanceModel::getMinimumCalibratedAirspeed(దశాంశం ఎక్కించు_గుణకం, దశాంశం flaps_setpoint) స్థిరరాశి
{
	ఎక్కించు_గుణకం = math::max(ఎక్కించు_గుణకం, FLT_EPSILON);
	స్థిరరాశి దశాంశం airspeed_flap_గుణకం = math::lerp(1.f, _param_fw_airspd_flp_sc.get(), flaps_setpoint);
	తిరిగి (_param_fw_airspd_min.get() * airspeed_flap_గుణకం) * sqrtf(get_బరువు_నిష్పత్తి() * ఎక్కించు_గుణకం);
}

దశాంశం PerformanceModel::getCalibratedStallAirspeed(దశాంశం ఎక్కించు_గుణకం, దశాంశం flaps_setpoint) స్థిరరాశి
{
	ఎక్కించు_గుణకం = math::max(ఎక్కించు_గుణకం, FLT_EPSILON);
	స్థిరరాశి దశాంశం airspeed_flap_గుణకం = math::lerp(1.f, _param_fw_airspd_flp_sc.get(), flaps_setpoint);
	తిరిగి (_param_fw_airspd_stall.get() * airspeed_flap_గుణకం) * sqrtf(get_బరువు_నిష్పత్తి() * ఎక్కించు_గుణకం);
}

దశాంశం PerformanceModel::get_గరిష్ఠ_Calibrated_Airspeed() స్థిరరాశి
{
	తిరిగి _param_fw_airspd_max.get();
}
తార్కిక PerformanceModel::నిరంతరం_Sanity_Checks() స్థిరరాశి
{
	తార్కిక ret = నిజం;

	// sanity check parameters
	ఒకవేళ (_param_fw_airspd_max.get() < _param_fw_airspd_min.get()) {
		/* EVENT
		 * @description
		 * - <param>FW_AIRSPD_MAX</param>: {1:.1}
		 * - <param>FW_AIRSPD_MIN</param>: {2:.1}
		 */
		events::send<దశాంశం, దశాంశం>(events::ID("fixedwing_position_control_conf_invalid_airspeed"), events::Log::Error,
					   "Invalid configuration: Airspeed max smaller than min",
					   _param_fw_airspd_max.get(), _param_fw_airspd_min.get());
		ret = అబద్ధం;
	}

	ఒకవేళ (_param_fw_airspd_max.get() < 5.0f || _param_fw_airspd_min.get() > 100.0f) {
		/* EVENT
		 * @description
		 * - <param>FW_AIRSPD_MAX</param>: {1:.1}
		 * - <param>FW_AIRSPD_MIN</param>: {2:.1}
		 */
		events::send<దశాంశం, దశాంశం>(events::ID("fixedwing_position_control_conf_invalid_airspeed_bounds"), events::Log::Error,
					   "Invalid configuration: Airspeed max \\< 5 m/s or min \\> 100 m/s",
					   _param_fw_airspd_max.get(), _param_fw_airspd_min.get());
		ret = అబద్ధం;
	}

	ఒకవేళ (_param_fw_airspd_trim.get() < _param_fw_airspd_min.get() ||
	    _param_fw_airspd_trim.get() > _param_fw_airspd_max.get()) {
		/* EVENT
		 * @description
		 * - <param>FW_AIRSPD_MAX</param>: {1:.1}
		 * - <param>FW_AIRSPD_MIN</param>: {2:.1}
		 * - <param>FW_AIRSPD_TRIM</param>: {3:.1}
		 */
		events::send<దశాంశం, దశాంశం, దశాంశం>(events::ID("fixedwing_position_control_conf_invalid_trim_bounds"),
						  events::Log::Error,
						  "Invalid configuration: Airspeed trim out of min or max bounds",
						  _param_fw_airspd_max.get(), _param_fw_airspd_min.get(), _param_fw_airspd_trim.get());
		ret = అబద్ధం;
	}

	ఒకవేళ (_param_fw_airspd_stall.get() > _param_fw_airspd_min.get()) {
		/* EVENT
		 * @description
		 * - <param>FW_AIRSPD_MIN</param>: {1:.1}
		 * - <param>FW_AIRSPD_STALL</param>: {2:.1}
		 */
		events::send<దశాంశం, దశాంశం>(events::ID("fixedwing_position_control_conf_invalid_stall"), events::Log::Error,
					   "Invalid configuration: FW_AIRSPD_STALL higher FW_AIRSPD_MIN",
					   _param_fw_airspd_min.get(), _param_fw_airspd_stall.get());
		ret = అబద్ధం;
	}

	స్థిరరాశి దశాంశం max_bank_loadfactor = 1 / cosf(math::radians(_param_fw_r_lim.get()));
	స్థిరరాశి దశాంశం min_airspd_at_max_bank = getMinimumCalibratedAirspeed(max_bank_loadfactor, /*flaps_setpoint = */0.0f);

	ఒకవేళ (min_airspd_at_max_bank > _param_fw_airspd_max.get()) {

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
		events::send<దశాంశం, దశాంశం, దశాంశం>(events::ID("fixedwing_position_control_conf_invalid_maxbank_infeasible"), events::Log::Error,
						  "Invalid configuration: FW_AIRSPD_MAX too low to sustain max bank angle",
						  _param_fw_airspd_min.get(), _param_fw_airspd_max.get(), _param_fw_r_lim.get());
		ret = అబద్ధం;
	}

	తిరిగి ret;

}
శూన్య PerformanceModel::	updateParameters()
{
	updateParams();
}

దశాంశం PerformanceModel::sanitiseAirDensity(దశాంశం air_density)
{
	ఒకవేళ (!PX4_ISFINITE(air_density)) {
		air_density = kAirDensitySeaLevelStandardAtmos;
	}

	తిరిగి math::max(air_density, kAirDensityStandardAtmos11000Amsl);
}

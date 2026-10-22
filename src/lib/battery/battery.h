/****************************************************************************
 *
 *   Copyright (c) 2019-2021 PX4 Development Team. All rights reserved.
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
 * @file battery.h
 *
 * Library calls for battery functionality.
 *
 * @author Julian Oes <julian@oes.ch>
 * @author Timothy Scott <timothy@auterion.com>
 */

#رهنمود once

#ضمن <math.h>
#ضمن <float.h>

#ضمن <board_config.h>
#ضمن <px4_platform_common/board_common.h>
#ضمن <px4_platform_common/module_params.h>
#ضمن <matrix/math.hpp>

#ضمن <drivers/drv_hrt.h>
#ضمن <lib/parameters/param.h>
#ضمن <lib/mathlib/math/filter/AlphaFilter.hpp>
#ضمن <uORB/PublicationMulti.hpp>
#ضمن <uORB/Subscription.hpp>
#ضمن <uORB/topics/battery_status.h>
#ضمن <uORB/topics/flight_phase_estimation.h>
#ضمن <uORB/topics/vehicle_status.h>

#ضمن <lib/failure_injection/FailureInjection.hpp>

/**
 * BatteryBase is a base class for any type of battery.
 *
 * You can use this class on its own. Or, if you need to implement a custom battery type,
 * you can inherit from this class. See, for example, src/modules/battery_status/AnalogBattery.h
 */
class Battery : عام ModuleParams
{
عام:
	Battery(صحيح شاخص, ModuleParams *والد, ثابت صحيح sample_interval_us, ثابت uint8_t منبع);
	~Battery() = افتراضي;

	/**
	 * Get the battery cell count
	 */
	صحيح cell_شمار() { أرجع _params.n_cells; }

	/**
	 * Get the empty voltage per cell
	 */
	عائم خالی_cell_voltage() { أرجع _params.v_خالی; }

	/**
	 * Get the full voltage per cell
	 */
	عائم پر_cell_voltage() { أرجع _params.v_charged; }

	فراغ مجموعه_اولویت(ثابت uint8_t اولویت) { اولویت_2 = اولویت; }
	فراغ مجموعه_Connected(ثابت bool connected) { _connected = connected; }
	فراغ مجموعه_State_از_Charge(ثابت عائم soc) { state_از_charge = math::constrain(soc, 0.f, 1.f); external_state_از_charge = true; }
	فراغ مجموعه_ظرفیت_Mah(ثابت عائم ظرفیت) { ظرفیت_mah = math::max(ظرفیت, 0.f); }
	فراغ updateVoltage(ثابت عائم voltage_v);
	فراغ updateCurrent(ثابت عائم current_a);
	فراغ updateTemperature(ثابت عائم temperature_c);

	/**
	 * Update state of charge calculations
	 *
	 * @param timestamp Time at which the battery data sample was measured
	 */
	فراغ update_Battery_وضعیت(ثابت hrt_abstime &timestamp);

	battery_وضعیت_s get_Battery_وضعیت();
	عائم get_Current_معدل() ثابت { أرجع PX4_ISFINITE(current_معدل_filter_a.getState()) ? current_معدل_filter_a.getState() : -1.f; }
	فراغ publish_Battery_وضعیت(ثابت battery_وضعیت_s &battery_وضعیت);

	/**
	 * Convenience function for combined update and publication
	 * @see updateBatteryStatus()
	 * @see publishBatteryStatus()
	 */
	فراغ update_And_Publish_Battery_وضعیت(ثابت hrt_abstime &timestamp);

	/**
	 * Calculates how much time is left before the battery is depleted,
	 * given the heavily low-pass filtered current consumption.
	 * Requires the capacity and state of charge e.g. externally set through setCapacity() and setStateOfCharge().
	 *
	 * @param current_a The current draw from the battery in amperes.
	 * @return Estimated remaining time in seconds.
	 */
	عائم compute_مانده_زمان(عائم current_a);

	/**
	 * Updates coulomb counting
	 * Requires a dt, seeupdateDt()
	 *
	 * @param current_a Positive current draw in A
	 * @return Accumulated used capacity in mAh
	 */
	عائم جمع_Discharged(عائم current_a);
	uint8_t determineWarning(عائم state_از_charge_2);
	فراغ updateDt(ثابت hrt_abstime &timestamp);

محمي:
	ساكن constexpr عائم LITHIUM_BATTERY_RECOGNITION_VOLTAGE = 2.1f;

	بنية {
		param_t v_خالی;
		param_t v_charged;
		param_t n_cells;
		param_t ظرفیت;
		param_t r_داخلی;
		param_t کم_thr;
		param_t crit_thr;
		param_t emergen_thr;
		param_t منبع;
		param_t bat_avrg_current;
		param_t i_overwrite;
	} _param_handles{};

	بنية {
		عائم v_خالی;
		عائم v_charged;
		int32_t  n_cells;
		عائم r_داخلی;
		عائم کم_thr;
		عائم crit_thr;
		عائم emergen_thr;
		int32_t منبع;
		عائم bat_avrg_current;
		عائم i_overwrite;
	} _params{};

	ثابت صحيح شاخص_2;

	bool اولی_parameter_update{true};
	فراغ updateParams() override;

خاص:
	عائم calculate_State_از_Charge_Voltage_Based(ثابت عائم voltage_v, ثابت عائم current_a);
	فراغ estimate_State_از_Charge();
	uint16_t determine_خطاها();
	فراغ computeScale();

	uORB::Subscription vehicle_وضعیت_sub{ORB_ID(vehicle_وضعیت)};
	uORB::Subscription_داده<flight_phase_estimation_s> _flight_phase_estimation_sub{ORB_ID(flight_phase_estimation)};
	uORB::PublicationMulti<battery_وضعیت_s> battery_وضعیت_pub{ORB_ID(battery_وضعیت)};

	failure_injection::Config _failure_config;

	bool external_state_از_charge{false}; ///< inticates that the soc is injected and not updated by this library

	bool _connected{false};
	ثابت uint8_t منبع_2;
	uint8_t اولویت_2{0};
	bool _battery_initialized{false};
	عائم _voltage_v{0.f};
	AlphaFilter<عائم> _ocv_filter_v;
	AlphaFilter<عائم> _cell_voltage_filter_v;
	عائم _current_a{-1};
	AlphaFilter<عائم>
	current_معدل_filter_a; ///< averaging filter for current. For FW, it is the current in level flight.
	عائم _temperature_c{NAN};
	عائم _discharged_mah{0.f};
	عائم _discharged_mah_loop{0.f};
	عائم state_از_charge_volt_based{-1.f}; // [0,1]
	عائم state_از_charge{-1.f}; // [0,1]
	عائم _scale{1.f};
	uint8_t _warning{battery_وضعیت_s::WARNING_NONE};
	عائم _dt{0.f};
	عائم _dt_discharge{0.f}; // unclamped dt, used for the discharged_mah coulomb count
	عائم ظرفیت_mah{0.f};
	hrt_abstime آخرین_timestamp{0};
	bool _armed{false};
	bool vehicle_وضعیت_is_fw{false};
	bool vehicle_وضعیت_was_fw{false};
	hrt_abstime آخرین_unconnected_timestamp{0};

	// Internal Resistance estimation
	فراغ update_داخلی_Resistance_Estimation(ثابت عائم voltage_v, ثابت عائم current_a);
	فراغ reset_داخلی_Resistance_Estimation(ثابت عائم voltage_v, ثابت عائم current_a);
	ماتریس::Vector2f _RLS_est; // [Open circuit voltage estimate [V], Total internal resistance estimate [Ohm]]^T
	ماتریس::Matrix2f _estimation_covariance;
	bool داخلی_resistance_initialized{false};
	عائم _estimation_covariance_norm{0.f};
	عائم داخلی_resistance_estimate_2{0.005f}; // [Ohm] Per cell estimate of the internal resistance
	عائم _voltage_prediction{0.f}; // [V] Predicted voltage of the estimator
	عائم _prediction_error{0.f}; // [V] Error between the predicted and measured voltage
	ساكن constexpr عائم LAMBDA = 0.95f; 	// [0, 1] Forgetting factor (Tuning parameter for the RLS algorithm)
	ساكن constexpr عائم R_DEFAULT = 0.005f; // [Ohm] Initial per cell estimate of the internal resistance
	ساكن constexpr عائم OCV_DEFAULT = 4.2f; // [V] Initial per cell estimate of the open circuit voltage
	ساكن constexpr عائم R_COVARIANCE = 0.1f; // Initial per cell covariance of the internal resistance
	ساكن constexpr عائم OCV_COVARIANCE = 1.5f; // Initial per cell covariance of the open circuit voltage

	// Temperature [degC] above which an overtemperature fault is declared,
	// leading to a failsafe warning recommending immediate landing. Note
	// that depending on the setup this may be measured in/close to the
	// battery (smart battery) or from a separate power monitor module.
	ساكن constexpr عائم BAT_موقت_MAX = 100.0f;
};

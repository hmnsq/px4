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

#ہدایت once

#شامل <math.h>
#شامل <float.h>

#شامل <board_config.h>
#شامل <px4_platform_common/board_common.h>
#شامل <px4_platform_common/module_params.h>
#شامل <matrix/math.hpp>

#شامل <drivers/drv_hrt.h>
#شامل <lib/parameters/param.h>
#شامل <lib/mathlib/math/filter/AlphaFilter.hpp>
#شامل <uORB/PublicationMulti.hpp>
#شامل <uORB/Subscription.hpp>
#شامل <uORB/topics/battery_status.h>
#شامل <uORB/topics/flight_phase_estimation.h>
#شامل <uORB/topics/vehicle_status.h>

#شامل <lib/failure_injection/FailureInjection.hpp>

/**
 * BatteryBase is a base class for any type of battery.
 *
 * You can use this class on its own. Or, if you need to implement a custom battery type,
 * you can inherit from this class. See, for example, src/modules/battery_status/AnalogBattery.h
 */
class Battery : عوامی ModuleParams
{
عوامی:
	Battery(صحیح اشاریہ, ModuleParams *والد, ثابت صحیح sample_interval_us, ثابت uint8_t ماخذ);
	~Battery() = طےشدہ;

	/**
	 * Get the battery cell count
	 */
	صحیح cell_گنتی() { واپس _params.n_cells; }

	/**
	 * Get the empty voltage per cell
	 */
	اعشاری خالی_خانے_cell_voltage() { واپس _params.v_خالی_خانے; }

	/**
	 * Get the full voltage per cell
	 */
	اعشاری بھرا_cell_voltage() { واپس _params.v_charged; }

	خالی سیٹ_ترجیح(ثابت uint8_t ترجیح) { ترجیح_2 = ترجیح; }
	خالی سیٹ_Connected(ثابت bool connected) { _connected = connected; }
	خالی سیٹ_State_کا_Charge(ثابت اعشاری soc) { state_کا_charge = math::constrain(soc, 0.f, 1.f); external_state_کا_charge = true; }
	خالی سیٹ_استعداد_Mah(ثابت اعشاری استعداد) { استعداد_mah = math::max(استعداد, 0.f); }
	خالی updateVoltage(ثابت اعشاری voltage_v);
	خالی updateCurrent(ثابت اعشاری current_a);
	خالی updateTemperature(ثابت اعشاری temperature_c);

	/**
	 * Update state of charge calculations
	 *
	 * @param timestamp Time at which the battery data sample was measured
	 */
	خالی update_Battery_حالت(ثابت hrt_abstime &timestamp);

	battery_حالت_s get_Battery_حالت();
	اعشاری get_Current_اوسط_قدر() ثابت { واپس PX4_ISFINITE(current_اوسط_قدر_filter_a.getState()) ? current_اوسط_قدر_filter_a.getState() : -1.f; }
	خالی publish_Battery_حالت(ثابت battery_حالت_s &battery_حالت);

	/**
	 * Convenience function for combined update and publication
	 * @see updateBatteryStatus()
	 * @see publishBatteryStatus()
	 */
	خالی update_And_Publish_Battery_حالت(ثابت hrt_abstime &timestamp);

	/**
	 * Calculates how much time is left before the battery is depleted,
	 * given the heavily low-pass filtered current consumption.
	 * Requires the capacity and state of charge e.g. externally set through setCapacity() and setStateOfCharge().
	 *
	 * @param current_a The current draw from the battery in amperes.
	 * @return Estimated remaining time in seconds.
	 */
	اعشاری compute_بقایا_وقت(اعشاری current_a);

	/**
	 * Updates coulomb counting
	 * Requires a dt, seeupdateDt()
	 *
	 * @param current_a Positive current draw in A
	 * @return Accumulated used capacity in mAh
	 */
	اعشاری مجموع_Discharged(اعشاری current_a);
	uint8_t determineWarning(اعشاری state_کا_charge_2);
	خالی updateDt(ثابت hrt_abstime &timestamp);

محفوظ:
	ساکن constexpr اعشاری LITHIUM_BATTERY_RECOGNITION_VOLTAGE = 2.1f;

	ساخت {
		param_t v_خالی_خانے;
		param_t v_charged;
		param_t n_cells;
		param_t استعداد;
		param_t r_اندرونی;
		param_t کم_thr;
		param_t crit_thr;
		param_t emergen_thr;
		param_t ماخذ;
		param_t bat_avrg_current;
		param_t i_overwrite;
	} _param_handles{};

	ساخت {
		اعشاری v_خالی_خانے;
		اعشاری v_charged;
		int32_t  n_cells;
		اعشاری r_اندرونی;
		اعشاری کم_thr;
		اعشاری crit_thr;
		اعشاری emergen_thr;
		int32_t ماخذ;
		اعشاری bat_avrg_current;
		اعشاری i_overwrite;
	} _params{};

	ثابت صحیح اشاریہ_2;

	bool پہلا_parameter_update{true};
	خالی updateParams() override;

نجی:
	اعشاری calculate_State_کا_Charge_Voltage_Based(ثابت اعشاری voltage_v, ثابت اعشاری current_a);
	خالی estimate_State_کا_Charge();
	uint16_t determine_خرابیاں();
	خالی computeScale();

	uORB::Subscription vehicle_حالت_sub{ORB_ID(vehicle_حالت)};
	uORB::Subscription_ڈیٹا<flight_phase_estimation_s> _flight_phase_estimation_sub{ORB_ID(flight_phase_estimation)};
	uORB::PublicationMulti<battery_حالت_s> battery_حالت_pub{ORB_ID(battery_حالت)};

	failure_injection::Config _failure_config;

	bool external_state_کا_charge{false}; ///< inticates that the soc is injected and not updated by this library

	bool _connected{false};
	ثابت uint8_t ماخذ_2;
	uint8_t ترجیح_2{0};
	bool _battery_initialized{false};
	اعشاری _voltage_v{0.f};
	AlphaFilter<اعشاری> _ocv_filter_v;
	AlphaFilter<اعشاری> _cell_voltage_filter_v;
	اعشاری _current_a{-1};
	AlphaFilter<اعشاری>
	current_اوسط_قدر_filter_a; ///< averaging filter for current. For FW, it is the current in level flight.
	اعشاری _temperature_c{NAN};
	اعشاری _discharged_mah{0.f};
	اعشاری _discharged_mah_loop{0.f};
	اعشاری state_کا_charge_volt_based{-1.f}; // [0,1]
	اعشاری state_کا_charge{-1.f}; // [0,1]
	اعشاری _scale{1.f};
	uint8_t _warning{battery_حالت_s::WARNING_NONE};
	اعشاری _dt{0.f};
	اعشاری _dt_discharge{0.f}; // unclamped dt, used for the discharged_mah coulomb count
	اعشاری استعداد_mah{0.f};
	hrt_abstime آخری_timestamp{0};
	bool _armed{false};
	bool vehicle_حالت_is_fw{false};
	bool vehicle_حالت_was_fw{false};
	hrt_abstime آخری_unconnected_timestamp{0};

	// Internal Resistance estimation
	خالی update_اندرونی_Resistance_Estimation(ثابت اعشاری voltage_v, ثابت اعشاری current_a);
	خالی reset_اندرونی_Resistance_Estimation(ثابت اعشاری voltage_v, ثابت اعشاری current_a);
	میٹرکس::Vector2f _RLS_est; // [Open circuit voltage estimate [V], Total internal resistance estimate [Ohm]]^T
	میٹرکس::Matrix2f _estimation_covariance;
	bool اندرونی_resistance_initialized{false};
	اعشاری _estimation_covariance_norm{0.f};
	اعشاری اندرونی_resistance_estimate_2{0.005f}; // [Ohm] Per cell estimate of the internal resistance
	اعشاری _voltage_prediction{0.f}; // [V] Predicted voltage of the estimator
	اعشاری _prediction_error{0.f}; // [V] Error between the predicted and measured voltage
	ساکن constexpr اعشاری LAMBDA = 0.95f; 	// [0, 1] Forgetting factor (Tuning parameter for the RLS algorithm)
	ساکن constexpr اعشاری R_DEFAULT = 0.005f; // [Ohm] Initial per cell estimate of the internal resistance
	ساکن constexpr اعشاری OCV_DEFAULT = 4.2f; // [V] Initial per cell estimate of the open circuit voltage
	ساکن constexpr اعشاری R_COVARIANCE = 0.1f; // Initial per cell covariance of the internal resistance
	ساکن constexpr اعشاری OCV_COVARIANCE = 1.5f; // Initial per cell covariance of the open circuit voltage

	// Temperature [degC] above which an overtemperature fault is declared,
	// leading to a failsafe warning recommending immediate landing. Note
	// that depending on the setup this may be measured in/close to the
	// battery (smart battery) or from a separate power monitor module.
	ساکن constexpr اعشاری BAT_عارضی_MAX = 100.0f;
};

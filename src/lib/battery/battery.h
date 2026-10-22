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

#pragma once

#অন্তর্ভাব <math.h>
#অন্তর্ভাব <float.h>

#অন্তর্ভাব <board_config.h>
#অন্তর্ভাব <px4_platform_common/board_common.h>
#অন্তর্ভাব <px4_platform_common/module_params.h>
#অন্তর্ভাব <matrix/math.hpp>

#অন্তর্ভাব <drivers/drv_hrt.h>
#অন্তর্ভাব <lib/parameters/param.h>
#অন্তর্ভাব <lib/mathlib/math/filter/AlphaFilter.hpp>
#অন্তর্ভাব <uORB/PublicationMulti.hpp>
#অন্তর্ভাব <uORB/Subscription.hpp>
#অন্তর্ভাব <uORB/topics/battery_status.h>
#অন্তর্ভাব <uORB/topics/flight_phase_estimation.h>
#অন্তর্ভাব <uORB/topics/vehicle_status.h>

#অন্তর্ভাব <lib/failure_injection/FailureInjection.hpp>

/**
 * BatteryBase is a base class for any type of battery.
 *
 * You can use this class on its own. Or, if you need to implement a custom battery type,
 * you can inherit from this class. See, for example, src/modules/battery_status/AnalogBattery.h
 */
class Battery : সর্বজনীন ModuleParams
{
সর্বজনীন:
	Battery(পূর্ণসংখ্যা সূচক, ModuleParams *parent, ধ্রুবক পূর্ণসংখ্যা sample_interval_us, ধ্রুবক uint8_t source);
	~Battery() = পূর্বনির্ধারিত;

	/**
	 * Get the battery cell count
	 */
	পূর্ণসংখ্যা cell_গুনতি() { ফেরত _params.n_cells; }

	/**
	 * Get the empty voltage per cell
	 */
	দশমিক empty_cell_voltage() { ফেরত _params.v_empty; }

	/**
	 * Get the full voltage per cell
	 */
	দশমিক full_cell_voltage() { ফেরত _params.v_charged; }

	শূন্য setPriority(ধ্রুবক uint8_t priority) { _priority = priority; }
	শূন্য setConnected(ধ্রুবক bool connected) { _connected = connected; }
	শূন্য setStateOfCharge(ধ্রুবক দশমিক soc) { _state_of_charge = math::constrain(soc, 0.f, 1.f); _external_state_of_charge = true; }
	শূন্য setCapacityMah(ধ্রুবক দশমিক capacity) { _capacity_mah = math::max(capacity, 0.f); }
	শূন্য updateVoltage(ধ্রুবক দশমিক voltage_v);
	শূন্য updateCurrent(ধ্রুবক দশমিক current_a);
	শূন্য updateTemperature(ধ্রুবক দশমিক temperature_c);

	/**
	 * Update state of charge calculations
	 *
	 * @param timestamp Time at which the battery data sample was measured
	 */
	শূন্য updateBatteryStatus(ধ্রুবক hrt_abstime &timestamp);

	battery_status_s getBatteryStatus();
	দশমিক getCurrentAverage() ধ্রুবক { ফেরত PX4_ISFINITE(_current_average_filter_a.getState()) ? _current_average_filter_a.getState() : -1.f; }
	শূন্য publishBatteryStatus(ধ্রুবক battery_status_s &battery_status);

	/**
	 * Convenience function for combined update and publication
	 * @see updateBatteryStatus()
	 * @see publishBatteryStatus()
	 */
	শূন্য updateAndPublishBatteryStatus(ধ্রুবক hrt_abstime &timestamp);

	/**
	 * Calculates how much time is left before the battery is depleted,
	 * given the heavily low-pass filtered current consumption.
	 * Requires the capacity and state of charge e.g. externally set through setCapacity() and setStateOfCharge().
	 *
	 * @param current_a The current draw from the battery in amperes.
	 * @return Estimated remaining time in seconds.
	 */
	দশমিক computeRemainingTime(দশমিক current_a);

	/**
	 * Updates coulomb counting
	 * Requires a dt, seeupdateDt()
	 *
	 * @param current_a Positive current draw in A
	 * @return Accumulated used capacity in mAh
	 */
	দশমিক যোগ_Discharged(দশমিক current_a);
	uint8_t determineWarning(দশমিক state_of_charge);
	শূন্য updateDt(ধ্রুবক hrt_abstime &timestamp);

সুরক্ষিত:
	স্থির constexpr দশমিক LITHIUM_BATTERY_RECOGNITION_VOLTAGE = 2.1f;

	কাঠামো {
		param_t v_empty;
		param_t v_charged;
		param_t n_cells;
		param_t capacity;
		param_t r_internal;
		param_t নিচের_thr;
		param_t crit_thr;
		param_t emergen_thr;
		param_t source;
		param_t bat_avrg_current;
		param_t i_overwrite;
	} _param_handles{};

	কাঠামো {
		দশমিক v_empty;
		দশমিক v_charged;
		int32_t  n_cells;
		দশমিক r_internal;
		দশমিক নিচের_thr;
		দশমিক crit_thr;
		দশমিক emergen_thr;
		int32_t source;
		দশমিক bat_avrg_current;
		দশমিক i_overwrite;
	} _params{};

	ধ্রুবক পূর্ণসংখ্যা সূচক_2;

	bool _first_parameter_update{true};
	শূন্য updateParams() override;

ব্যক্তিগত:
	দশমিক calculateStateOfChargeVoltageBased(ধ্রুবক দশমিক voltage_v, ধ্রুবক দশমিক current_a);
	শূন্য estimateStateOfCharge();
	uint16_t determineFaults();
	শূন্য computeScale();

	uORB::Subscription _vehicle_status_sub{ORB_ID(vehicle_status)};
	uORB::Subscription_তথ্য<flight_phase_estimation_s> _flight_phase_estimation_sub{ORB_ID(flight_phase_estimation)};
	uORB::PublicationMulti<battery_status_s> _battery_status_pub{ORB_ID(battery_status)};

	failure_injection::Config _failure_config;

	bool _external_state_of_charge{false}; ///< inticates that the soc is injected and not updated by this library

	bool _connected{false};
	ধ্রুবক uint8_t _source;
	uint8_t _priority{0};
	bool _battery_initialized{false};
	দশমিক _voltage_v{0.f};
	AlphaFilter<দশমিক> _ocv_filter_v;
	AlphaFilter<দশমিক> _cell_voltage_filter_v;
	দশমিক _current_a{-1};
	AlphaFilter<দশমিক>
	_current_average_filter_a; ///< averaging filter for current. For FW, it is the current in level flight.
	দশমিক _temperature_c{NAN};
	দশমিক _discharged_mah{0.f};
	দশমিক _discharged_mah_loop{0.f};
	দশমিক _state_of_charge_volt_based{-1.f}; // [0,1]
	দশমিক _state_of_charge{-1.f}; // [0,1]
	দশমিক _scale{1.f};
	uint8_t _warning{battery_status_s::WARNING_NONE};
	দশমিক _dt{0.f};
	দশমিক _dt_discharge{0.f}; // unclamped dt, used for the discharged_mah coulomb count
	দশমিক _capacity_mah{0.f};
	hrt_abstime _last_timestamp{0};
	bool _armed{false};
	bool _vehicle_status_is_fw{false};
	bool _vehicle_status_was_fw{false};
	hrt_abstime _last_unconnected_timestamp{0};

	// Internal Resistance estimation
	শূন্য updateInternalResistanceEstimation(ধ্রুবক দশমিক voltage_v, ধ্রুবক দশমিক current_a);
	শূন্য resetInternalResistanceEstimation(ধ্রুবক দশমিক voltage_v, ধ্রুবক দশমিক current_a);
	matrix::Vector2f _RLS_est; // [Open circuit voltage estimate [V], Total internal resistance estimate [Ohm]]^T
	matrix::Matrix2f _estimation_covariance;
	bool _internal_resistance_initialized{false};
	দশমিক _estimation_covariance_norm{0.f};
	দশমিক _internal_resistance_estimate{0.005f}; // [Ohm] Per cell estimate of the internal resistance
	দশমিক _voltage_prediction{0.f}; // [V] Predicted voltage of the estimator
	দশমিক _prediction_error{0.f}; // [V] Error between the predicted and measured voltage
	স্থির constexpr দশমিক LAMBDA = 0.95f; 	// [0, 1] Forgetting factor (Tuning parameter for the RLS algorithm)
	স্থির constexpr দশমিক R_DEFAULT = 0.005f; // [Ohm] Initial per cell estimate of the internal resistance
	স্থির constexpr দশমিক OCV_DEFAULT = 4.2f; // [V] Initial per cell estimate of the open circuit voltage
	স্থির constexpr দশমিক R_COVARIANCE = 0.1f; // Initial per cell covariance of the internal resistance
	স্থির constexpr দশমিক OCV_COVARIANCE = 1.5f; // Initial per cell covariance of the open circuit voltage

	// Temperature [degC] above which an overtemperature fault is declared,
	// leading to a failsafe warning recommending immediate landing. Note
	// that depending on the setup this may be measured in/close to the
	// battery (smart battery) or from a separate power monitor module.
	স্থির constexpr দশমিক BAT_অস্থায়ী_MAX = 100.0f;
};

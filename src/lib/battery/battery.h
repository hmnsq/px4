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

#അന്തര്ഭാവ <math.h>
#അന്തര്ഭാവ <float.h>

#അന്തര്ഭാവ <board_config.h>
#അന്തര്ഭാവ <px4_platform_common/board_common.h>
#അന്തര്ഭാവ <px4_platform_common/module_params.h>
#അന്തര്ഭാവ <matrix/math.hpp>

#അന്തര്ഭാവ <drivers/drv_hrt.h>
#അന്തര്ഭാവ <lib/parameters/param.h>
#അന്തര്ഭാവ <lib/mathlib/math/filter/AlphaFilter.hpp>
#അന്തര്ഭാവ <uORB/PublicationMulti.hpp>
#അന്തര്ഭാവ <uORB/Subscription.hpp>
#അന്തര്ഭാവ <uORB/topics/battery_status.h>
#അന്തര്ഭാവ <uORB/topics/flight_phase_estimation.h>
#അന്തര്ഭാവ <uORB/topics/vehicle_status.h>

#അന്തര്ഭാവ <lib/failure_injection/FailureInjection.hpp>

/**
 * BatteryBase is a base class for any type of battery.
 *
 * You can use this class on its own. Or, if you need to implement a custom battery type,
 * you can inherit from this class. See, for example, src/modules/battery_status/AnalogBattery.h
 */
class Battery : പൊതു ModuleParams
{
പൊതു:
	Battery(പൂർണ്ണസംഖ്യ index, ModuleParams *parent, സ്ഥിരം പൂർണ്ണസംഖ്യ sample_interval_us, സ്ഥിരം uint8_t source);
	~Battery() = സ്ഥിരസ്ഥിതി;

	/**
	 * Get the battery cell count
	 */
	പൂർണ്ണസംഖ്യ cell_എണ്ണം() { തിരികെ _params.n_cells; }

	/**
	 * Get the empty voltage per cell
	 */
	ദശാംശം empty_cell_voltage() { തിരികെ _params.v_empty; }

	/**
	 * Get the full voltage per cell
	 */
	ദശാംശം full_cell_voltage() { തിരികെ _params.v_charged; }

	ശൂന്യം setPriority(സ്ഥിരം uint8_t priority) { _priority = priority; }
	ശൂന്യം setConnected(സ്ഥിരം bool connected) { _connected = connected; }
	ശൂന്യം setStateOfCharge(സ്ഥിരം ദശാംശം soc) { _state_of_charge = math::constrain(soc, 0.f, 1.f); _external_state_of_charge = true; }
	ശൂന്യം setCapacityMah(സ്ഥിരം ദശാംശം capacity) { _capacity_mah = math::max(capacity, 0.f); }
	ശൂന്യം updateVoltage(സ്ഥിരം ദശാംശം voltage_v);
	ശൂന്യം updateCurrent(സ്ഥിരം ദശാംശം current_a);
	ശൂന്യം updateTemperature(സ്ഥിരം ദശാംശം temperature_c);

	/**
	 * Update state of charge calculations
	 *
	 * @param timestamp Time at which the battery data sample was measured
	 */
	ശൂന്യം updateBatteryStatus(സ്ഥിരം hrt_abstime &timestamp);

	battery_status_s getBatteryStatus();
	ദശാംശം getCurrentAverage() സ്ഥിരം { തിരികെ PX4_ISFINITE(_current_average_filter_a.getState()) ? _current_average_filter_a.getState() : -1.f; }
	ശൂന്യം publishBatteryStatus(സ്ഥിരം battery_status_s &battery_status);

	/**
	 * Convenience function for combined update and publication
	 * @see updateBatteryStatus()
	 * @see publishBatteryStatus()
	 */
	ശൂന്യം updateAndPublishBatteryStatus(സ്ഥിരം hrt_abstime &timestamp);

	/**
	 * Calculates how much time is left before the battery is depleted,
	 * given the heavily low-pass filtered current consumption.
	 * Requires the capacity and state of charge e.g. externally set through setCapacity() and setStateOfCharge().
	 *
	 * @param current_a The current draw from the battery in amperes.
	 * @return Estimated remaining time in seconds.
	 */
	ദശാംശം computeRemainingTime(ദശാംശം current_a);

	/**
	 * Updates coulomb counting
	 * Requires a dt, seeupdateDt()
	 *
	 * @param current_a Positive current draw in A
	 * @return Accumulated used capacity in mAh
	 */
	ദശാംശം sumDischarged(ദശാംശം current_a);
	uint8_t determineWarning(ദശാംശം state_of_charge);
	ശൂന്യം updateDt(സ്ഥിരം hrt_abstime &timestamp);

സംരക്ഷിത:
	സ്ഥിതം constexpr ദശാംശം LITHIUM_BATTERY_RECOGNITION_VOLTAGE = 2.1f;

	ഘടന {
		param_t v_empty;
		param_t v_charged;
		param_t n_cells;
		param_t capacity;
		param_t r_internal;
		param_t താഴെയുള്ള_thr;
		param_t crit_thr;
		param_t emergen_thr;
		param_t source;
		param_t bat_avrg_current;
		param_t i_overwrite;
	} _param_handles{};

	ഘടന {
		ദശാംശം v_empty;
		ദശാംശം v_charged;
		int32_t  n_cells;
		ദശാംശം r_internal;
		ദശാംശം താഴെയുള്ള_thr;
		ദശാംശം crit_thr;
		ദശാംശം emergen_thr;
		int32_t source;
		ദശാംശം bat_avrg_current;
		ദശാംശം i_overwrite;
	} _params{};

	സ്ഥിരം പൂർണ്ണസംഖ്യ _index;

	bool _first_parameter_update{true};
	ശൂന്യം updateParams() override;

സ്വകാര്യ:
	ദശാംശം calculateStateOfChargeVoltageBased(സ്ഥിരം ദശാംശം voltage_v, സ്ഥിരം ദശാംശം current_a);
	ശൂന്യം estimateStateOfCharge();
	uint16_t determineFaults();
	ശൂന്യം computeScale();

	uORB::Subscription _vehicle_status_sub{ORB_ID(vehicle_status)};
	uORB::SubscriptionData<flight_phase_estimation_s> _flight_phase_estimation_sub{ORB_ID(flight_phase_estimation)};
	uORB::PublicationMulti<battery_status_s> _battery_status_pub{ORB_ID(battery_status)};

	failure_injection::Config _failure_config;

	bool _external_state_of_charge{false}; ///< inticates that the soc is injected and not updated by this library

	bool _connected{false};
	സ്ഥിരം uint8_t _source;
	uint8_t _priority{0};
	bool _battery_initialized{false};
	ദശാംശം _voltage_v{0.f};
	AlphaFilter<ദശാംശം> _ocv_filter_v;
	AlphaFilter<ദശാംശം> _cell_voltage_filter_v;
	ദശാംശം _current_a{-1};
	AlphaFilter<ദശാംശം>
	_current_average_filter_a; ///< averaging filter for current. For FW, it is the current in level flight.
	ദശാംശം _temperature_c{NAN};
	ദശാംശം _discharged_mah{0.f};
	ദശാംശം _discharged_mah_loop{0.f};
	ദശാംശം _state_of_charge_volt_based{-1.f}; // [0,1]
	ദശാംശം _state_of_charge{-1.f}; // [0,1]
	ദശാംശം _scale{1.f};
	uint8_t _warning{battery_status_s::WARNING_NONE};
	ദശാംശം _dt{0.f};
	ദശാംശം _dt_discharge{0.f}; // unclamped dt, used for the discharged_mah coulomb count
	ദശാംശം _capacity_mah{0.f};
	hrt_abstime _last_timestamp{0};
	bool _armed{false};
	bool _vehicle_status_is_fw{false};
	bool _vehicle_status_was_fw{false};
	hrt_abstime _last_unconnected_timestamp{0};

	// Internal Resistance estimation
	ശൂന്യം updateInternalResistanceEstimation(സ്ഥിരം ദശാംശം voltage_v, സ്ഥിരം ദശാംശം current_a);
	ശൂന്യം resetInternalResistanceEstimation(സ്ഥിരം ദശാംശം voltage_v, സ്ഥിരം ദശാംശം current_a);
	matrix::Vector2f _RLS_est; // [Open circuit voltage estimate [V], Total internal resistance estimate [Ohm]]^T
	matrix::Matrix2f _estimation_covariance;
	bool _internal_resistance_initialized{false};
	ദശാംശം _estimation_covariance_norm{0.f};
	ദശാംശം _internal_resistance_estimate{0.005f}; // [Ohm] Per cell estimate of the internal resistance
	ദശാംശം _voltage_prediction{0.f}; // [V] Predicted voltage of the estimator
	ദശാംശം _prediction_error{0.f}; // [V] Error between the predicted and measured voltage
	സ്ഥിതം constexpr ദശാംശം LAMBDA = 0.95f; 	// [0, 1] Forgetting factor (Tuning parameter for the RLS algorithm)
	സ്ഥിതം constexpr ദശാംശം R_DEFAULT = 0.005f; // [Ohm] Initial per cell estimate of the internal resistance
	സ്ഥിതം constexpr ദശാംശം OCV_DEFAULT = 4.2f; // [V] Initial per cell estimate of the open circuit voltage
	സ്ഥിതം constexpr ദശാംശം R_COVARIANCE = 0.1f; // Initial per cell covariance of the internal resistance
	സ്ഥിതം constexpr ദശാംശം OCV_COVARIANCE = 1.5f; // Initial per cell covariance of the open circuit voltage

	// Temperature [degC] above which an overtemperature fault is declared,
	// leading to a failsafe warning recommending immediate landing. Note
	// that depending on the setup this may be measured in/close to the
	// battery (smart battery) or from a separate power monitor module.
	സ്ഥിതം constexpr ദശാംശം BAT_TEMP_MAX = 100.0f;
};

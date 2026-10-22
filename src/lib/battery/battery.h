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

#அந்தர்பாவ <math.h>
#அந்தர்பாவ <float.h>

#அந்தர்பாவ <board_config.h>
#அந்தர்பாவ <px4_platform_common/board_common.h>
#அந்தர்பாவ <px4_platform_common/module_params.h>
#அந்தர்பாவ <matrix/math.hpp>

#அந்தர்பாவ <drivers/drv_hrt.h>
#அந்தர்பாவ <lib/parameters/param.h>
#அந்தர்பாவ <lib/mathlib/math/filter/AlphaFilter.hpp>
#அந்தர்பாவ <uORB/PublicationMulti.hpp>
#அந்தர்பாவ <uORB/Subscription.hpp>
#அந்தர்பாவ <uORB/topics/battery_status.h>
#அந்தர்பாவ <uORB/topics/flight_phase_estimation.h>
#அந்தர்பாவ <uORB/topics/vehicle_status.h>

#அந்தர்பாவ <lib/failure_injection/FailureInjection.hpp>

/**
 * BatteryBase is a base class for any type of battery.
 *
 * You can use this class on its own. Or, if you need to implement a custom battery type,
 * you can inherit from this class. See, for example, src/modules/battery_status/AnalogBattery.h
 */
class Battery : பொது ModuleParams
{
பொது:
	Battery(முழு index, ModuleParams *parent, மாறிலி முழு sample_interval_us, மாறிலி uint8_t source);
	~Battery() = இயல்பு;

	/**
	 * Get the battery cell count
	 */
	முழு cell_மொத்தம்() { திரும்பு _params.n_cells; }

	/**
	 * Get the empty voltage per cell
	 */
	தசம empty_cell_voltage() { திரும்பு _params.v_empty; }

	/**
	 * Get the full voltage per cell
	 */
	தசம full_cell_voltage() { திரும்பு _params.v_charged; }

	வெறுமை setPriority(மாறிலி uint8_t priority) { _priority = priority; }
	வெறுமை setConnected(மாறிலி bool connected) { _connected = connected; }
	வெறுமை setStateOfCharge(மாறிலி தசம soc) { _state_of_charge = math::constrain(soc, 0.f, 1.f); _external_state_of_charge = true; }
	வெறுமை setCapacityMah(மாறிலி தசம capacity) { _capacity_mah = math::max(capacity, 0.f); }
	வெறுமை updateVoltage(மாறிலி தசம voltage_v);
	வெறுமை updateCurrent(மாறிலி தசம current_a);
	வெறுமை updateTemperature(மாறிலி தசம temperature_c);

	/**
	 * Update state of charge calculations
	 *
	 * @param timestamp Time at which the battery data sample was measured
	 */
	வெறுமை updateBatteryStatus(மாறிலி hrt_abstime &timestamp);

	battery_status_s getBatteryStatus();
	தசம getCurrentAverage() மாறிலி { திரும்பு PX4_ISFINITE(_current_average_filter_a.getState()) ? _current_average_filter_a.getState() : -1.f; }
	வெறுமை publishBatteryStatus(மாறிலி battery_status_s &battery_status);

	/**
	 * Convenience function for combined update and publication
	 * @see updateBatteryStatus()
	 * @see publishBatteryStatus()
	 */
	வெறுமை updateAndPublishBatteryStatus(மாறிலி hrt_abstime &timestamp);

	/**
	 * Calculates how much time is left before the battery is depleted,
	 * given the heavily low-pass filtered current consumption.
	 * Requires the capacity and state of charge e.g. externally set through setCapacity() and setStateOfCharge().
	 *
	 * @param current_a The current draw from the battery in amperes.
	 * @return Estimated remaining time in seconds.
	 */
	தசம computeRemainingTime(தசம current_a);

	/**
	 * Updates coulomb counting
	 * Requires a dt, seeupdateDt()
	 *
	 * @param current_a Positive current draw in A
	 * @return Accumulated used capacity in mAh
	 */
	தசம sumDischarged(தசம current_a);
	uint8_t determineWarning(தசம state_of_charge);
	வெறுமை updateDt(மாறிலி hrt_abstime &timestamp);

பாதுகாக்கப்பட்ட:
	நிலையான constexpr தசம LITHIUM_BATTERY_RECOGNITION_VOLTAGE = 2.1f;

	கட்டமைப்பு {
		param_t v_empty;
		param_t v_charged;
		param_t n_cells;
		param_t capacity;
		param_t r_internal;
		param_t கீழ்_thr;
		param_t crit_thr;
		param_t emergen_thr;
		param_t source;
		param_t bat_avrg_current;
		param_t i_overwrite;
	} _param_handles{};

	கட்டமைப்பு {
		தசம v_empty;
		தசம v_charged;
		int32_t  n_cells;
		தசம r_internal;
		தசம கீழ்_thr;
		தசம crit_thr;
		தசம emergen_thr;
		int32_t source;
		தசம bat_avrg_current;
		தசம i_overwrite;
	} _params{};

	மாறிலி முழு _index;

	bool _first_parameter_update{true};
	வெறுமை updateParams() override;

தனிப்பட்ட:
	தசம calculateStateOfChargeVoltageBased(மாறிலி தசம voltage_v, மாறிலி தசம current_a);
	வெறுமை estimateStateOfCharge();
	uint16_t determineFaults();
	வெறுமை computeScale();

	uORB::Subscription _vehicle_status_sub{ORB_ID(vehicle_status)};
	uORB::SubscriptionData<flight_phase_estimation_s> _flight_phase_estimation_sub{ORB_ID(flight_phase_estimation)};
	uORB::PublicationMulti<battery_status_s> _battery_status_pub{ORB_ID(battery_status)};

	failure_injection::Config _failure_config;

	bool _external_state_of_charge{false}; ///< inticates that the soc is injected and not updated by this library

	bool _connected{false};
	மாறிலி uint8_t _source;
	uint8_t _priority{0};
	bool _battery_initialized{false};
	தசம _voltage_v{0.f};
	AlphaFilter<தசம> _ocv_filter_v;
	AlphaFilter<தசம> _cell_voltage_filter_v;
	தசம _current_a{-1};
	AlphaFilter<தசம>
	_current_average_filter_a; ///< averaging filter for current. For FW, it is the current in level flight.
	தசம _temperature_c{NAN};
	தசம _discharged_mah{0.f};
	தசம _discharged_mah_loop{0.f};
	தசம _state_of_charge_volt_based{-1.f}; // [0,1]
	தசம _state_of_charge{-1.f}; // [0,1]
	தசம _scale{1.f};
	uint8_t _warning{battery_status_s::WARNING_NONE};
	தசம _dt{0.f};
	தசம _dt_discharge{0.f}; // unclamped dt, used for the discharged_mah coulomb count
	தசம _capacity_mah{0.f};
	hrt_abstime _last_timestamp{0};
	bool _armed{false};
	bool _vehicle_status_is_fw{false};
	bool _vehicle_status_was_fw{false};
	hrt_abstime _last_unconnected_timestamp{0};

	// Internal Resistance estimation
	வெறுமை updateInternalResistanceEstimation(மாறிலி தசம voltage_v, மாறிலி தசம current_a);
	வெறுமை resetInternalResistanceEstimation(மாறிலி தசம voltage_v, மாறிலி தசம current_a);
	matrix::Vector2f _RLS_est; // [Open circuit voltage estimate [V], Total internal resistance estimate [Ohm]]^T
	matrix::Matrix2f _estimation_covariance;
	bool _internal_resistance_initialized{false};
	தசம _estimation_covariance_norm{0.f};
	தசம _internal_resistance_estimate{0.005f}; // [Ohm] Per cell estimate of the internal resistance
	தசம _voltage_prediction{0.f}; // [V] Predicted voltage of the estimator
	தசம _prediction_error{0.f}; // [V] Error between the predicted and measured voltage
	நிலையான constexpr தசம LAMBDA = 0.95f; 	// [0, 1] Forgetting factor (Tuning parameter for the RLS algorithm)
	நிலையான constexpr தசம R_DEFAULT = 0.005f; // [Ohm] Initial per cell estimate of the internal resistance
	நிலையான constexpr தசம OCV_DEFAULT = 4.2f; // [V] Initial per cell estimate of the open circuit voltage
	நிலையான constexpr தசம R_COVARIANCE = 0.1f; // Initial per cell covariance of the internal resistance
	நிலையான constexpr தசம OCV_COVARIANCE = 1.5f; // Initial per cell covariance of the open circuit voltage

	// Temperature [degC] above which an overtemperature fault is declared,
	// leading to a failsafe warning recommending immediate landing. Note
	// that depending on the setup this may be measured in/close to the
	// battery (smart battery) or from a separate power monitor module.
	நிலையான constexpr தசம BAT_TEMP_MAX = 100.0f;
};

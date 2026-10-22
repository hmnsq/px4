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

#ಅನ್ತರ್ಭಾವ <math.h>
#ಅನ್ತರ್ಭಾವ <float.h>

#ಅನ್ತರ್ಭಾವ <board_config.h>
#ಅನ್ತರ್ಭಾವ <px4_platform_common/board_common.h>
#ಅನ್ತರ್ಭಾವ <px4_platform_common/module_params.h>
#ಅನ್ತರ್ಭಾವ <matrix/math.hpp>

#ಅನ್ತರ್ಭಾವ <drivers/drv_hrt.h>
#ಅನ್ತರ್ಭಾವ <lib/parameters/param.h>
#ಅನ್ತರ್ಭಾವ <lib/mathlib/math/filter/AlphaFilter.hpp>
#ಅನ್ತರ್ಭಾವ <uORB/PublicationMulti.hpp>
#ಅನ್ತರ್ಭಾವ <uORB/Subscription.hpp>
#ಅನ್ತರ್ಭಾವ <uORB/topics/battery_status.h>
#ಅನ್ತರ್ಭಾವ <uORB/topics/flight_phase_estimation.h>
#ಅನ್ತರ್ಭಾವ <uORB/topics/vehicle_status.h>

#ಅನ್ತರ್ಭಾವ <lib/failure_injection/FailureInjection.hpp>

/**
 * BatteryBase is a base class for any type of battery.
 *
 * You can use this class on its own. Or, if you need to implement a custom battery type,
 * you can inherit from this class. See, for example, src/modules/battery_status/AnalogBattery.h
 */
class Battery : ಸಾರ್ವಜನಿಕ ModuleParams
{
ಸಾರ್ವಜನಿಕ:
	Battery(ಪೂರ್ಣಾಙ್ಕ index, ModuleParams *parent, ಧ್ರುವ ಪೂರ್ಣಾಙ್ಕ sample_interval_us, ಧ್ರುವ uint8_t source);
	~Battery() = ಸ್ವಭಾವ;

	/**
	 * Get the battery cell count
	 */
	ಪೂರ್ಣಾಙ್ಕ cell_ಎಣಿಕೆ() { ಪ್ರತ್ಯಾಗಮ _params.n_cells; }

	/**
	 * Get the empty voltage per cell
	 */
	ಭಿನ್ನ empty_cell_voltage() { ಪ್ರತ್ಯಾಗಮ _params.v_empty; }

	/**
	 * Get the full voltage per cell
	 */
	ಭಿನ್ನ full_cell_voltage() { ಪ್ರತ್ಯಾಗಮ _params.v_charged; }

	ಶೂನ್ಯ setPriority(ಧ್ರುವ uint8_t priority) { _priority = priority; }
	ಶೂನ್ಯ setConnected(ಧ್ರುವ bool connected) { _connected = connected; }
	ಶೂನ್ಯ setStateOfCharge(ಧ್ರುವ ಭಿನ್ನ soc) { _state_of_charge = math::constrain(soc, 0.f, 1.f); _external_state_of_charge = true; }
	ಶೂನ್ಯ setCapacityMah(ಧ್ರುವ ಭಿನ್ನ capacity) { _capacity_mah = math::max(capacity, 0.f); }
	ಶೂನ್ಯ updateVoltage(ಧ್ರುವ ಭಿನ್ನ voltage_v);
	ಶೂನ್ಯ updateCurrent(ಧ್ರುವ ಭಿನ್ನ current_a);
	ಶೂನ್ಯ updateTemperature(ಧ್ರುವ ಭಿನ್ನ temperature_c);

	/**
	 * Update state of charge calculations
	 *
	 * @param timestamp Time at which the battery data sample was measured
	 */
	ಶೂನ್ಯ updateBatteryStatus(ಧ್ರುವ hrt_abstime &timestamp);

	battery_status_s getBatteryStatus();
	ಭಿನ್ನ getCurrentAverage() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ PX4_ISFINITE(_current_average_filter_a.getState()) ? _current_average_filter_a.getState() : -1.f; }
	ಶೂನ್ಯ publishBatteryStatus(ಧ್ರುವ battery_status_s &battery_status);

	/**
	 * Convenience function for combined update and publication
	 * @see updateBatteryStatus()
	 * @see publishBatteryStatus()
	 */
	ಶೂನ್ಯ updateAndPublishBatteryStatus(ಧ್ರುವ hrt_abstime &timestamp);

	/**
	 * Calculates how much time is left before the battery is depleted,
	 * given the heavily low-pass filtered current consumption.
	 * Requires the capacity and state of charge e.g. externally set through setCapacity() and setStateOfCharge().
	 *
	 * @param current_a The current draw from the battery in amperes.
	 * @return Estimated remaining time in seconds.
	 */
	ಭಿನ್ನ computeRemainingTime(ಭಿನ್ನ current_a);

	/**
	 * Updates coulomb counting
	 * Requires a dt, seeupdateDt()
	 *
	 * @param current_a Positive current draw in A
	 * @return Accumulated used capacity in mAh
	 */
	ಭಿನ್ನ sumDischarged(ಭಿನ್ನ current_a);
	uint8_t determineWarning(ಭಿನ್ನ state_of_charge);
	ಶೂನ್ಯ updateDt(ಧ್ರುವ hrt_abstime &timestamp);

ರಕ್ಷಿತ:
	ಸ್ಥಿರ constexpr ಭಿನ್ನ LITHIUM_BATTERY_RECOGNITION_VOLTAGE = 2.1f;

	ಸಂರಚನಾ {
		param_t v_empty;
		param_t v_charged;
		param_t n_cells;
		param_t capacity;
		param_t r_internal;
		param_t ಕೆಳಗಿನ_thr;
		param_t crit_thr;
		param_t emergen_thr;
		param_t source;
		param_t bat_avrg_current;
		param_t i_overwrite;
	} _param_handles{};

	ಸಂರಚನಾ {
		ಭಿನ್ನ v_empty;
		ಭಿನ್ನ v_charged;
		int32_t  n_cells;
		ಭಿನ್ನ r_internal;
		ಭಿನ್ನ ಕೆಳಗಿನ_thr;
		ಭಿನ್ನ crit_thr;
		ಭಿನ್ನ emergen_thr;
		int32_t source;
		ಭಿನ್ನ bat_avrg_current;
		ಭಿನ್ನ i_overwrite;
	} _params{};

	ಧ್ರುವ ಪೂರ್ಣಾಙ್ಕ _index;

	bool _first_parameter_update{true};
	ಶೂನ್ಯ updateParams() override;

ಗುಪ್ತ:
	ಭಿನ್ನ calculateStateOfChargeVoltageBased(ಧ್ರುವ ಭಿನ್ನ voltage_v, ಧ್ರುವ ಭಿನ್ನ current_a);
	ಶೂನ್ಯ estimateStateOfCharge();
	uint16_t determineFaults();
	ಶೂನ್ಯ computeScale();

	uORB::Subscription _vehicle_status_sub{ORB_ID(vehicle_status)};
	uORB::SubscriptionData<flight_phase_estimation_s> _flight_phase_estimation_sub{ORB_ID(flight_phase_estimation)};
	uORB::PublicationMulti<battery_status_s> _battery_status_pub{ORB_ID(battery_status)};

	failure_injection::Config _failure_config;

	bool _external_state_of_charge{false}; ///< inticates that the soc is injected and not updated by this library

	bool _connected{false};
	ಧ್ರುವ uint8_t _source;
	uint8_t _priority{0};
	bool _battery_initialized{false};
	ಭಿನ್ನ _voltage_v{0.f};
	AlphaFilter<ಭಿನ್ನ> _ocv_filter_v;
	AlphaFilter<ಭಿನ್ನ> _cell_voltage_filter_v;
	ಭಿನ್ನ _current_a{-1};
	AlphaFilter<ಭಿನ್ನ>
	_current_average_filter_a; ///< averaging filter for current. For FW, it is the current in level flight.
	ಭಿನ್ನ _temperature_c{NAN};
	ಭಿನ್ನ _discharged_mah{0.f};
	ಭಿನ್ನ _discharged_mah_loop{0.f};
	ಭಿನ್ನ _state_of_charge_volt_based{-1.f}; // [0,1]
	ಭಿನ್ನ _state_of_charge{-1.f}; // [0,1]
	ಭಿನ್ನ _scale{1.f};
	uint8_t _warning{battery_status_s::WARNING_NONE};
	ಭಿನ್ನ _dt{0.f};
	ಭಿನ್ನ _dt_discharge{0.f}; // unclamped dt, used for the discharged_mah coulomb count
	ಭಿನ್ನ _capacity_mah{0.f};
	hrt_abstime _last_timestamp{0};
	bool _armed{false};
	bool _vehicle_status_is_fw{false};
	bool _vehicle_status_was_fw{false};
	hrt_abstime _last_unconnected_timestamp{0};

	// Internal Resistance estimation
	ಶೂನ್ಯ updateInternalResistanceEstimation(ಧ್ರುವ ಭಿನ್ನ voltage_v, ಧ್ರುವ ಭಿನ್ನ current_a);
	ಶೂನ್ಯ resetInternalResistanceEstimation(ಧ್ರುವ ಭಿನ್ನ voltage_v, ಧ್ರುವ ಭಿನ್ನ current_a);
	matrix::Vector2f _RLS_est; // [Open circuit voltage estimate [V], Total internal resistance estimate [Ohm]]^T
	matrix::Matrix2f _estimation_covariance;
	bool _internal_resistance_initialized{false};
	ಭಿನ್ನ _estimation_covariance_norm{0.f};
	ಭಿನ್ನ _internal_resistance_estimate{0.005f}; // [Ohm] Per cell estimate of the internal resistance
	ಭಿನ್ನ _voltage_prediction{0.f}; // [V] Predicted voltage of the estimator
	ಭಿನ್ನ _prediction_error{0.f}; // [V] Error between the predicted and measured voltage
	ಸ್ಥಿರ constexpr ಭಿನ್ನ LAMBDA = 0.95f; 	// [0, 1] Forgetting factor (Tuning parameter for the RLS algorithm)
	ಸ್ಥಿರ constexpr ಭಿನ್ನ R_DEFAULT = 0.005f; // [Ohm] Initial per cell estimate of the internal resistance
	ಸ್ಥಿರ constexpr ಭಿನ್ನ OCV_DEFAULT = 4.2f; // [V] Initial per cell estimate of the open circuit voltage
	ಸ್ಥಿರ constexpr ಭಿನ್ನ R_COVARIANCE = 0.1f; // Initial per cell covariance of the internal resistance
	ಸ್ಥಿರ constexpr ಭಿನ್ನ OCV_COVARIANCE = 1.5f; // Initial per cell covariance of the open circuit voltage

	// Temperature [degC] above which an overtemperature fault is declared,
	// leading to a failsafe warning recommending immediate landing. Note
	// that depending on the setup this may be measured in/close to the
	// battery (smart battery) or from a separate power monitor module.
	ಸ್ಥಿರ constexpr ಭಿನ್ನ BAT_TEMP_MAX = 100.0f;
};

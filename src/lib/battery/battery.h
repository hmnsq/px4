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

#సూచన once

#అన్తర్భావ <math.h>
#అన్తర్భావ <float.h>

#అన్తర్భావ <board_config.h>
#అన్తర్భావ <px4_platform_common/board_common.h>
#అన్తర్భావ <px4_platform_common/module_params.h>
#అన్తర్భావ <matrix/math.hpp>

#అన్తర్భావ <drivers/drv_hrt.h>
#అన్తర్భావ <lib/parameters/param.h>
#అన్తర్భావ <lib/mathlib/math/filter/AlphaFilter.hpp>
#అన్తర్భావ <uORB/PublicationMulti.hpp>
#అన్తర్భావ <uORB/Subscription.hpp>
#అన్తర్భావ <uORB/topics/battery_status.h>
#అన్తర్భావ <uORB/topics/flight_phase_estimation.h>
#అన్తర్భావ <uORB/topics/vehicle_status.h>

#అన్తర్భావ <lib/failure_injection/FailureInjection.hpp>

/**
 * BatteryBase is a base class for any type of battery.
 *
 * You can use this class on its own. Or, if you need to implement a custom battery type,
 * you can inherit from this class. See, for example, src/modules/battery_status/AnalogBattery.h
 */
class Battery : బహిరంగ ModuleParams
{
బహిరంగ:
	Battery(పూర్ణాంకం సూచీ, ModuleParams *మాతృ, స్థిరరాశి పూర్ణాంకం sample_interval_us, స్థిరరాశి uint8_t ఆధారం);
	~Battery() = అప్రమేయ;

	/**
	 * Get the battery cell count
	 */
	పూర్ణాంకం cell_లెక్క() { తిరిగి _params.n_cells; }

	/**
	 * Get the empty voltage per cell
	 */
	దశాంశం ఖాళీలు_cell_voltage() { తిరిగి _params.v_ఖాళీలు; }

	/**
	 * Get the full voltage per cell
	 */
	దశాంశం నిండిన_cell_voltage() { తిరిగి _params.v_charged; }

	శూన్య సమితి_ప్రాధాన్యత(స్థిరరాశి uint8_t ప్రాధాన్యత) { ప్రాధాన్యత_2 = ప్రాధాన్యత; }
	శూన్య సమితి_Connected(స్థిరరాశి bool connected) { _connected = connected; }
	శూన్య సమితి_State_యొక్క_Charge(స్థిరరాశి దశాంశం soc) { state_యొక్క_charge = math::constrain(soc, 0.f, 1.f); external_state_యొక్క_charge = true; }
	శూన్య సమితి_సామర్థ్యం_Mah(స్థిరరాశి దశాంశం సామర్థ్యం) { సామర్థ్యం_mah = math::max(సామర్థ్యం, 0.f); }
	శూన్య updateVoltage(స్థిరరాశి దశాంశం voltage_v);
	శూన్య updateCurrent(స్థిరరాశి దశాంశం current_a);
	శూన్య updateTemperature(స్థిరరాశి దశాంశం temperature_c);

	/**
	 * Update state of charge calculations
	 *
	 * @param timestamp Time at which the battery data sample was measured
	 */
	శూన్య update_Battery_స్థితి(స్థిరరాశి hrt_abstime &timestamp);

	battery_స్థితి_s get_Battery_స్థితి();
	దశాంశం get_Current_సరాసరి() స్థిరరాశి { తిరిగి PX4_ISFINITE(current_సరాసరి_filter_a.getState()) ? current_సరాసరి_filter_a.getState() : -1.f; }
	శూన్య publish_Battery_స్థితి(స్థిరరాశి battery_స్థితి_s &battery_స్థితి);

	/**
	 * Convenience function for combined update and publication
	 * @see updateBatteryStatus()
	 * @see publishBatteryStatus()
	 */
	శూన్య update_And_Publish_Battery_స్థితి(స్థిరరాశి hrt_abstime &timestamp);

	/**
	 * Calculates how much time is left before the battery is depleted,
	 * given the heavily low-pass filtered current consumption.
	 * Requires the capacity and state of charge e.g. externally set through setCapacity() and setStateOfCharge().
	 *
	 * @param current_a The current draw from the battery in amperes.
	 * @return Estimated remaining time in seconds.
	 */
	దశాంశం compute_మిగిలిన_సమయం(దశాంశం current_a);

	/**
	 * Updates coulomb counting
	 * Requires a dt, seeupdateDt()
	 *
	 * @param current_a Positive current draw in A
	 * @return Accumulated used capacity in mAh
	 */
	దశాంశం కూడిక_Discharged(దశాంశం current_a);
	uint8_t determineWarning(దశాంశం state_యొక్క_charge_2);
	శూన్య updateDt(స్థిరరాశి hrt_abstime &timestamp);

రక్షిత:
	స్థిర constexpr దశాంశం LITHIUM_BATTERY_RECOGNITION_VOLTAGE = 2.1f;

	నిర్మాణం {
		param_t v_ఖాళీలు;
		param_t v_charged;
		param_t n_cells;
		param_t సామర్థ్యం;
		param_t r_అంతర్గత;
		param_t దిగువ_thr;
		param_t crit_thr;
		param_t emergen_thr;
		param_t ఆధారం;
		param_t bat_avrg_current;
		param_t i_overwrite;
	} _param_handles{};

	నిర్మాణం {
		దశాంశం v_ఖాళీలు;
		దశాంశం v_charged;
		int32_t  n_cells;
		దశాంశం r_అంతర్గత;
		దశాంశం దిగువ_thr;
		దశాంశం crit_thr;
		దశాంశం emergen_thr;
		int32_t ఆధారం;
		దశాంశం bat_avrg_current;
		దశాంశం i_overwrite;
	} _params{};

	స్థిరరాశి పూర్ణాంకం సూచీ_2;

	bool మొదటి_parameter_update{true};
	శూన్య updateParams() override;

అంతరంగ:
	దశాంశం calculate_State_యొక్క_Charge_Voltage_Based(స్థిరరాశి దశాంశం voltage_v, స్థిరరాశి దశాంశం current_a);
	శూన్య estimate_State_యొక్క_Charge();
	uint16_t determine_దోషాలు();
	శూన్య computeScale();

	uORB::Subscription vehicle_స్థితి_sub{ORB_ID(vehicle_స్థితి)};
	uORB::Subscription_దత్తాంశం<flight_phase_estimation_s> _flight_phase_estimation_sub{ORB_ID(flight_phase_estimation)};
	uORB::PublicationMulti<battery_స్థితి_s> battery_స్థితి_pub{ORB_ID(battery_స్థితి)};

	failure_injection::Config _failure_config;

	bool external_state_యొక్క_charge{false}; ///< inticates that the soc is injected and not updated by this library

	bool _connected{false};
	స్థిరరాశి uint8_t ఆధారం_2;
	uint8_t ప్రాధాన్యత_2{0};
	bool _battery_initialized{false};
	దశాంశం _voltage_v{0.f};
	AlphaFilter<దశాంశం> _ocv_filter_v;
	AlphaFilter<దశాంశం> _cell_voltage_filter_v;
	దశాంశం _current_a{-1};
	AlphaFilter<దశాంశం>
	current_సరాసరి_filter_a; ///< averaging filter for current. For FW, it is the current in level flight.
	దశాంశం _temperature_c{NAN};
	దశాంశం _discharged_mah{0.f};
	దశాంశం _discharged_mah_loop{0.f};
	దశాంశం state_యొక్క_charge_volt_based{-1.f}; // [0,1]
	దశాంశం state_యొక్క_charge{-1.f}; // [0,1]
	దశాంశం _scale{1.f};
	uint8_t _warning{battery_స్థితి_s::WARNING_NONE};
	దశాంశం _dt{0.f};
	దశాంశం _dt_discharge{0.f}; // unclamped dt, used for the discharged_mah coulomb count
	దశాంశం సామర్థ్యం_mah{0.f};
	hrt_abstime చివరి_timestamp{0};
	bool _armed{false};
	bool vehicle_స్థితి_is_fw{false};
	bool vehicle_స్థితి_was_fw{false};
	hrt_abstime చివరి_unconnected_timestamp{0};

	// Internal Resistance estimation
	శూన్య update_అంతర్గత_Resistance_Estimation(స్థిరరాశి దశాంశం voltage_v, స్థిరరాశి దశాంశం current_a);
	శూన్య reset_అంతర్గత_Resistance_Estimation(స్థిరరాశి దశాంశం voltage_v, స్థిరరాశి దశాంశం current_a);
	మాతృక::Vector2f _RLS_est; // [Open circuit voltage estimate [V], Total internal resistance estimate [Ohm]]^T
	మాతృక::Matrix2f _estimation_covariance;
	bool అంతర్గత_resistance_initialized{false};
	దశాంశం _estimation_covariance_norm{0.f};
	దశాంశం అంతర్గత_resistance_estimate_2{0.005f}; // [Ohm] Per cell estimate of the internal resistance
	దశాంశం _voltage_prediction{0.f}; // [V] Predicted voltage of the estimator
	దశాంశం _prediction_error{0.f}; // [V] Error between the predicted and measured voltage
	స్థిర constexpr దశాంశం LAMBDA = 0.95f; 	// [0, 1] Forgetting factor (Tuning parameter for the RLS algorithm)
	స్థిర constexpr దశాంశం R_DEFAULT = 0.005f; // [Ohm] Initial per cell estimate of the internal resistance
	స్థిర constexpr దశాంశం OCV_DEFAULT = 4.2f; // [V] Initial per cell estimate of the open circuit voltage
	స్థిర constexpr దశాంశం R_COVARIANCE = 0.1f; // Initial per cell covariance of the internal resistance
	స్థిర constexpr దశాంశం OCV_COVARIANCE = 1.5f; // Initial per cell covariance of the open circuit voltage

	// Temperature [degC] above which an overtemperature fault is declared,
	// leading to a failsafe warning recommending immediate landing. Note
	// that depending on the setup this may be measured in/close to the
	// battery (smart battery) or from a separate power monitor module.
	స్థిర constexpr దశాంశం BAT_తాత్కాలిక_MAX = 100.0f;
};

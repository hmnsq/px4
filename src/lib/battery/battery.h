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

#ਹਦਾਇਤ once

#ਅਨ੍ਤਰ੍ਭਾਵ <math.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <float.h>

#ਅਨ੍ਤਰ੍ਭਾਵ <board_config.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <px4_platform_common/board_common.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <px4_platform_common/module_params.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <matrix/math.hpp>

#ਅਨ੍ਤਰ੍ਭਾਵ <drivers/drv_hrt.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <lib/parameters/param.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <lib/mathlib/math/filter/AlphaFilter.hpp>
#ਅਨ੍ਤਰ੍ਭਾਵ <uORB/PublicationMulti.hpp>
#ਅਨ੍ਤਰ੍ਭਾਵ <uORB/Subscription.hpp>
#ਅਨ੍ਤਰ੍ਭਾਵ <uORB/topics/battery_status.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <uORB/topics/flight_phase_estimation.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <uORB/topics/vehicle_status.h>

#ਅਨ੍ਤਰ੍ਭਾਵ <lib/failure_injection/FailureInjection.hpp>

/**
 * BatteryBase is a base class for any type of battery.
 *
 * You can use this class on its own. Or, if you need to implement a custom battery type,
 * you can inherit from this class. See, for example, src/modules/battery_status/AnalogBattery.h
 */
class Battery : ਜਨਤਕ ModuleParams
{
ਜਨਤਕ:
	Battery(ਪੂਰਨ ਸੂਚਕਾਂਕ, ModuleParams *ਮਾਪਾ, ਅਚਲ ਪੂਰਨ sample_interval_us, ਅਚਲ uint8_t ਸਰੋਤ);
	~Battery() = ਮੂਲ;

	/**
	 * Get the battery cell count
	 */
	ਪੂਰਨ cell_ਗਿਣਤੀ() { ਵਾਪਸ _params.n_cells; }

	/**
	 * Get the empty voltage per cell
	 */
	ਦਸ਼ਮਲਵ ਖਾਲੀ_ਥਾਵਾਂ_cell_voltage() { ਵਾਪਸ _params.v_ਖਾਲੀ_ਥਾਵਾਂ; }

	/**
	 * Get the full voltage per cell
	 */
	ਦਸ਼ਮਲਵ ਭਰਿਆ_cell_voltage() { ਵਾਪਸ _params.v_charged; }

	ਖਾਲੀ ਸੈੱਟ_ਤਰਜੀਹ(ਅਚਲ uint8_t ਤਰਜੀਹ) { ਤਰਜੀਹ_2 = ਤਰਜੀਹ; }
	ਖਾਲੀ ਸੈੱਟ_Connected(ਅਚਲ bool connected) { _connected = connected; }
	ਖਾਲੀ ਸੈੱਟ_State_ਦਾ_Charge(ਅਚਲ ਦਸ਼ਮਲਵ soc) { state_ਦਾ_charge = math::constrain(soc, 0.f, 1.f); external_state_ਦਾ_charge = true; }
	ਖਾਲੀ ਸੈੱਟ_ਸਮਰੱਥਾ_Mah(ਅਚਲ ਦਸ਼ਮਲਵ ਸਮਰੱਥਾ) { ਸਮਰੱਥਾ_mah = math::max(ਸਮਰੱਥਾ, 0.f); }
	ਖਾਲੀ updateVoltage(ਅਚਲ ਦਸ਼ਮਲਵ voltage_v);
	ਖਾਲੀ updateCurrent(ਅਚਲ ਦਸ਼ਮਲਵ current_a);
	ਖਾਲੀ updateTemperature(ਅਚਲ ਦਸ਼ਮਲਵ temperature_c);

	/**
	 * Update state of charge calculations
	 *
	 * @param timestamp Time at which the battery data sample was measured
	 */
	ਖਾਲੀ update_Battery_ਸਥਿਤੀ(ਅਚਲ hrt_abstime &timestamp);

	battery_ਸਥਿਤੀ_s get_Battery_ਸਥਿਤੀ();
	ਦਸ਼ਮਲਵ get_Current_ਔਸਤਨ() ਅਚਲ { ਵਾਪਸ PX4_ISFINITE(current_ਔਸਤਨ_filter_a.getState()) ? current_ਔਸਤਨ_filter_a.getState() : -1.f; }
	ਖਾਲੀ publish_Battery_ਸਥਿਤੀ(ਅਚਲ battery_ਸਥਿਤੀ_s &battery_ਸਥਿਤੀ);

	/**
	 * Convenience function for combined update and publication
	 * @see updateBatteryStatus()
	 * @see publishBatteryStatus()
	 */
	ਖਾਲੀ update_And_Publish_Battery_ਸਥਿਤੀ(ਅਚਲ hrt_abstime &timestamp);

	/**
	 * Calculates how much time is left before the battery is depleted,
	 * given the heavily low-pass filtered current consumption.
	 * Requires the capacity and state of charge e.g. externally set through setCapacity() and setStateOfCharge().
	 *
	 * @param current_a The current draw from the battery in amperes.
	 * @return Estimated remaining time in seconds.
	 */
	ਦਸ਼ਮਲਵ compute_ਬਚਿਆ_ਸਮਾਂ(ਦਸ਼ਮਲਵ current_a);

	/**
	 * Updates coulomb counting
	 * Requires a dt, seeupdateDt()
	 *
	 * @param current_a Positive current draw in A
	 * @return Accumulated used capacity in mAh
	 */
	ਦਸ਼ਮਲਵ ਜੋੜ_Discharged(ਦਸ਼ਮਲਵ current_a);
	uint8_t determineWarning(ਦਸ਼ਮਲਵ state_ਦਾ_charge_2);
	ਖਾਲੀ updateDt(ਅਚਲ hrt_abstime &timestamp);

ਸੁਰੱਖਿਅਤ:
	ਸਥਿਰ constexpr ਦਸ਼ਮਲਵ LITHIUM_BATTERY_RECOGNITION_VOLTAGE = 2.1f;

	ਬਣਤਰ {
		param_t v_ਖਾਲੀ_ਥਾਵਾਂ;
		param_t v_charged;
		param_t n_cells;
		param_t ਸਮਰੱਥਾ;
		param_t r_ਅੰਦਰੂਨੀ;
		param_t ਨੀਵਾਂ_thr;
		param_t crit_thr;
		param_t emergen_thr;
		param_t ਸਰੋਤ;
		param_t bat_avrg_current;
		param_t i_overwrite;
	} _param_handles{};

	ਬਣਤਰ {
		ਦਸ਼ਮਲਵ v_ਖਾਲੀ_ਥਾਵਾਂ;
		ਦਸ਼ਮਲਵ v_charged;
		int32_t  n_cells;
		ਦਸ਼ਮਲਵ r_ਅੰਦਰੂਨੀ;
		ਦਸ਼ਮਲਵ ਨੀਵਾਂ_thr;
		ਦਸ਼ਮਲਵ crit_thr;
		ਦਸ਼ਮਲਵ emergen_thr;
		int32_t ਸਰੋਤ;
		ਦਸ਼ਮਲਵ bat_avrg_current;
		ਦਸ਼ਮਲਵ i_overwrite;
	} _params{};

	ਅਚਲ ਪੂਰਨ ਸੂਚਕਾਂਕ_2;

	bool ਪਹਿਲਾ_parameter_update{true};
	ਖਾਲੀ updateParams() override;

ਨਿਜੀ:
	ਦਸ਼ਮਲਵ calculate_State_ਦਾ_Charge_Voltage_Based(ਅਚਲ ਦਸ਼ਮਲਵ voltage_v, ਅਚਲ ਦਸ਼ਮਲਵ current_a);
	ਖਾਲੀ estimate_State_ਦਾ_Charge();
	uint16_t determine_ਨੁਕਸ();
	ਖਾਲੀ computeScale();

	uORB::Subscription vehicle_ਸਥਿਤੀ_sub{ORB_ID(vehicle_ਸਥਿਤੀ)};
	uORB::Subscription_ਅੰਕੜੇ<flight_phase_estimation_s> _flight_phase_estimation_sub{ORB_ID(flight_phase_estimation)};
	uORB::PublicationMulti<battery_ਸਥਿਤੀ_s> battery_ਸਥਿਤੀ_pub{ORB_ID(battery_ਸਥਿਤੀ)};

	failure_injection::Config _failure_config;

	bool external_state_ਦਾ_charge{false}; ///< inticates that the soc is injected and not updated by this library

	bool _connected{false};
	ਅਚਲ uint8_t ਸਰੋਤ_2;
	uint8_t ਤਰਜੀਹ_2{0};
	bool _battery_initialized{false};
	ਦਸ਼ਮਲਵ _voltage_v{0.f};
	AlphaFilter<ਦਸ਼ਮਲਵ> _ocv_filter_v;
	AlphaFilter<ਦਸ਼ਮਲਵ> _cell_voltage_filter_v;
	ਦਸ਼ਮਲਵ _current_a{-1};
	AlphaFilter<ਦਸ਼ਮਲਵ>
	current_ਔਸਤਨ_filter_a; ///< averaging filter for current. For FW, it is the current in level flight.
	ਦਸ਼ਮਲਵ _temperature_c{NAN};
	ਦਸ਼ਮਲਵ _discharged_mah{0.f};
	ਦਸ਼ਮਲਵ _discharged_mah_loop{0.f};
	ਦਸ਼ਮਲਵ state_ਦਾ_charge_volt_based{-1.f}; // [0,1]
	ਦਸ਼ਮਲਵ state_ਦਾ_charge{-1.f}; // [0,1]
	ਦਸ਼ਮਲਵ _scale{1.f};
	uint8_t _warning{battery_ਸਥਿਤੀ_s::WARNING_NONE};
	ਦਸ਼ਮਲਵ _dt{0.f};
	ਦਸ਼ਮਲਵ _dt_discharge{0.f}; // unclamped dt, used for the discharged_mah coulomb count
	ਦਸ਼ਮਲਵ ਸਮਰੱਥਾ_mah{0.f};
	hrt_abstime ਆਖਰੀ_timestamp{0};
	bool _armed{false};
	bool vehicle_ਸਥਿਤੀ_is_fw{false};
	bool vehicle_ਸਥਿਤੀ_was_fw{false};
	hrt_abstime ਆਖਰੀ_unconnected_timestamp{0};

	// Internal Resistance estimation
	ਖਾਲੀ update_ਅੰਦਰੂਨੀ_Resistance_Estimation(ਅਚਲ ਦਸ਼ਮਲਵ voltage_v, ਅਚਲ ਦਸ਼ਮਲਵ current_a);
	ਖਾਲੀ reset_ਅੰਦਰੂਨੀ_Resistance_Estimation(ਅਚਲ ਦਸ਼ਮਲਵ voltage_v, ਅਚਲ ਦਸ਼ਮਲਵ current_a);
	ਮੈਟ੍ਰਿਕਸ::Vector2f _RLS_est; // [Open circuit voltage estimate [V], Total internal resistance estimate [Ohm]]^T
	ਮੈਟ੍ਰਿਕਸ::Matrix2f _estimation_covariance;
	bool ਅੰਦਰੂਨੀ_resistance_initialized{false};
	ਦਸ਼ਮਲਵ _estimation_covariance_norm{0.f};
	ਦਸ਼ਮਲਵ ਅੰਦਰੂਨੀ_resistance_estimate_2{0.005f}; // [Ohm] Per cell estimate of the internal resistance
	ਦਸ਼ਮਲਵ _voltage_prediction{0.f}; // [V] Predicted voltage of the estimator
	ਦਸ਼ਮਲਵ _prediction_error{0.f}; // [V] Error between the predicted and measured voltage
	ਸਥਿਰ constexpr ਦਸ਼ਮਲਵ LAMBDA = 0.95f; 	// [0, 1] Forgetting factor (Tuning parameter for the RLS algorithm)
	ਸਥਿਰ constexpr ਦਸ਼ਮਲਵ R_DEFAULT = 0.005f; // [Ohm] Initial per cell estimate of the internal resistance
	ਸਥਿਰ constexpr ਦਸ਼ਮਲਵ OCV_DEFAULT = 4.2f; // [V] Initial per cell estimate of the open circuit voltage
	ਸਥਿਰ constexpr ਦਸ਼ਮਲਵ R_COVARIANCE = 0.1f; // Initial per cell covariance of the internal resistance
	ਸਥਿਰ constexpr ਦਸ਼ਮਲਵ OCV_COVARIANCE = 1.5f; // Initial per cell covariance of the open circuit voltage

	// Temperature [degC] above which an overtemperature fault is declared,
	// leading to a failsafe warning recommending immediate landing. Note
	// that depending on the setup this may be measured in/close to the
	// battery (smart battery) or from a separate power monitor module.
	ਸਥਿਰ constexpr ਦਸ਼ਮਲਵ BAT_ਆਰਜ਼ੀ_MAX = 100.0f;
};

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

#निर्देशः once

#अन्तर्भाव <math.h>
#अन्तर्भाव <float.h>

#अन्तर्भाव <board_config.h>
#अन्तर्भाव <px4_platform_common/board_common.h>
#अन्तर्भाव <px4_platform_common/module_params.h>
#अन्तर्भाव <matrix/math.hpp>

#अन्तर्भाव <drivers/drv_hrt.h>
#अन्तर्भाव <lib/parameters/param.h>
#अन्तर्भाव <lib/mathlib/math/filter/AlphaFilter.hpp>
#अन्तर्भाव <uORB/PublicationMulti.hpp>
#अन्तर्भाव <uORB/Subscription.hpp>
#अन्तर्भाव <uORB/topics/battery_status.h>
#अन्तर्भाव <uORB/topics/flight_phase_estimation.h>
#अन्तर्भाव <uORB/topics/vehicle_status.h>

#अन्तर्भाव <lib/failure_injection/FailureInjection.hpp>

/**
 * BatteryBase is a base class for any type of battery.
 *
 * You can use this class on its own. Or, if you need to implement a custom battery type,
 * you can inherit from this class. See, for example, src/modules/battery_status/AnalogBattery.h
 */
class Battery : सार्वजनिक ModuleParams
{
सार्वजनिक:
	Battery(पूर्णाङ्क सूचकांक, ModuleParams *parent, ध्रुव पूर्णाङ्क sample_interval_us, ध्रुव uint8_t source);
	~Battery() = स्वभाव;

	/**
	 * Get the battery cell count
	 */
	पूर्णाङ्क cell_गणनम्() { प्रत्यागम _params.n_cells; }

	/**
	 * Get the empty voltage per cell
	 */
	भिन्न रिक्तानि_cell_voltage() { प्रत्यागम _params.v_रिक्तानि; }

	/**
	 * Get the full voltage per cell
	 */
	भिन्न पूर्णम्_cell_voltage() { प्रत्यागम _params.v_charged; }

	शून्य समुच्चयः_प्राथमिकता(ध्रुव uint8_t प्राथमिकता) { प्राथमिकता_2 = प्राथमिकता; }
	शून्य समुच्चयः_Connected(ध्रुव bool connected) { _connected = connected; }
	शून्य समुच्चयः_State_सम्बन्धि_Charge(ध्रुव भिन्न soc) { state_सम्बन्धि_charge = math::constrain(soc, 0.f, 1.f); external_state_सम्बन्धि_charge = true; }
	शून्य समुच्चयः_क्षमता_Mah(ध्रुव भिन्न क्षमता) { क्षमता_mah = math::max(क्षमता, 0.f); }
	शून्य updateVoltage(ध्रुव भिन्न voltage_v);
	शून्य updateCurrent(ध्रुव भिन्न current_a);
	शून्य updateTemperature(ध्रुव भिन्न temperature_c);

	/**
	 * Update state of charge calculations
	 *
	 * @param timestamp Time at which the battery data sample was measured
	 */
	शून्य update_Battery_स्थितिः(ध्रुव hrt_abstime &timestamp);

	battery_स्थितिः_s get_Battery_स्थितिः();
	भिन्न get_Current_मध्यमानम्() ध्रुव { प्रत्यागम PX4_ISFINITE(current_मध्यमानम्_filter_a.getState()) ? current_मध्यमानम्_filter_a.getState() : -1.f; }
	शून्य publish_Battery_स्थितिः(ध्रुव battery_स्थितिः_s &battery_स्थितिः);

	/**
	 * Convenience function for combined update and publication
	 * @see updateBatteryStatus()
	 * @see publishBatteryStatus()
	 */
	शून्य update_And_Publish_Battery_स्थितिः(ध्रुव hrt_abstime &timestamp);

	/**
	 * Calculates how much time is left before the battery is depleted,
	 * given the heavily low-pass filtered current consumption.
	 * Requires the capacity and state of charge e.g. externally set through setCapacity() and setStateOfCharge().
	 *
	 * @param current_a The current draw from the battery in amperes.
	 * @return Estimated remaining time in seconds.
	 */
	भिन्न computeRemainingTime(भिन्न current_a);

	/**
	 * Updates coulomb counting
	 * Requires a dt, seeupdateDt()
	 *
	 * @param current_a Positive current draw in A
	 * @return Accumulated used capacity in mAh
	 */
	भिन्न योग_Discharged(भिन्न current_a);
	uint8_t determineWarning(भिन्न state_सम्बन्धि_charge_2);
	शून्य updateDt(ध्रुव hrt_abstime &timestamp);

रक्षित:
	स्थिर constexpr भिन्न LITHIUM_BATTERY_RECOGNITION_VOLTAGE = 2.1f;

	संरचना {
		param_t v_रिक्तानि;
		param_t v_charged;
		param_t n_cells;
		param_t क्षमता;
		param_t r_आन्तरिकम्;
		param_t नीचः_thr;
		param_t crit_thr;
		param_t emergen_thr;
		param_t source;
		param_t bat_avrg_current;
		param_t i_overwrite;
	} _param_handles{};

	संरचना {
		भिन्न v_रिक्तानि;
		भिन्न v_charged;
		int32_t  n_cells;
		भिन्न r_आन्तरिकम्;
		भिन्न नीचः_thr;
		भिन्न crit_thr;
		भिन्न emergen_thr;
		int32_t source;
		भिन्न bat_avrg_current;
		भिन्न i_overwrite;
	} _params{};

	ध्रुव पूर्णाङ्क सूचकांक_2;

	bool _first_parameter_update{true};
	शून्य updateParams() override;

गुप्त:
	भिन्न calculate_State_सम्बन्धि_Charge_Voltage_Based(ध्रुव भिन्न voltage_v, ध्रुव भिन्न current_a);
	शून्य estimate_State_सम्बन्धि_Charge();
	uint16_t determine_दोषाः();
	शून्य computeScale();

	uORB::Subscription vehicle_स्थितिः_sub{ORB_ID(vehicle_स्थितिः)};
	uORB::Subscription_दत्त<flight_phase_estimation_s> _flight_phase_estimation_sub{ORB_ID(flight_phase_estimation)};
	uORB::PublicationMulti<battery_स्थितिः_s> battery_स्थितिः_pub{ORB_ID(battery_स्थितिः)};

	failure_injection::Config _failure_config;

	bool external_state_सम्बन्धि_charge{false}; ///< inticates that the soc is injected and not updated by this library

	bool _connected{false};
	ध्रुव uint8_t _source;
	uint8_t प्राथमिकता_2{0};
	bool _battery_initialized{false};
	भिन्न _voltage_v{0.f};
	AlphaFilter<भिन्न> _ocv_filter_v;
	AlphaFilter<भिन्न> _cell_voltage_filter_v;
	भिन्न _current_a{-1};
	AlphaFilter<भिन्न>
	current_मध्यमानम्_filter_a; ///< averaging filter for current. For FW, it is the current in level flight.
	भिन्न _temperature_c{NAN};
	भिन्न _discharged_mah{0.f};
	भिन्न _discharged_mah_loop{0.f};
	भिन्न state_सम्बन्धि_charge_volt_based{-1.f}; // [0,1]
	भिन्न state_सम्बन्धि_charge{-1.f}; // [0,1]
	भिन्न _scale{1.f};
	uint8_t _warning{battery_स्थितिः_s::WARNING_NONE};
	भिन्न _dt{0.f};
	भिन्न _dt_discharge{0.f}; // unclamped dt, used for the discharged_mah coulomb count
	भिन्न क्षमता_mah{0.f};
	hrt_abstime चरमः_timestamp{0};
	bool _armed{false};
	bool vehicle_स्थितिः_is_fw{false};
	bool vehicle_स्थितिः_was_fw{false};
	hrt_abstime चरमः_unconnected_timestamp{0};

	// Internal Resistance estimation
	शून्य update_आन्तरिकम्_Resistance_Estimation(ध्रुव भिन्न voltage_v, ध्रुव भिन्न current_a);
	शून्य reset_आन्तरिकम्_Resistance_Estimation(ध्रुव भिन्न voltage_v, ध्रुव भिन्न current_a);
	matrix::Vector2f _RLS_est; // [Open circuit voltage estimate [V], Total internal resistance estimate [Ohm]]^T
	matrix::Matrix2f _estimation_covariance;
	bool आन्तरिकम्_resistance_initialized{false};
	भिन्न _estimation_covariance_norm{0.f};
	भिन्न आन्तरिकम्_resistance_estimate_2{0.005f}; // [Ohm] Per cell estimate of the internal resistance
	भिन्न _voltage_prediction{0.f}; // [V] Predicted voltage of the estimator
	भिन्न _prediction_error{0.f}; // [V] Error between the predicted and measured voltage
	स्थिर constexpr भिन्न LAMBDA = 0.95f; 	// [0, 1] Forgetting factor (Tuning parameter for the RLS algorithm)
	स्थिर constexpr भिन्न R_DEFAULT = 0.005f; // [Ohm] Initial per cell estimate of the internal resistance
	स्थिर constexpr भिन्न OCV_DEFAULT = 4.2f; // [V] Initial per cell estimate of the open circuit voltage
	स्थिर constexpr भिन्न R_COVARIANCE = 0.1f; // Initial per cell covariance of the internal resistance
	स्थिर constexpr भिन्न OCV_COVARIANCE = 1.5f; // Initial per cell covariance of the open circuit voltage

	// Temperature [degC] above which an overtemperature fault is declared,
	// leading to a failsafe warning recommending immediate landing. Note
	// that depending on the setup this may be measured in/close to the
	// battery (smart battery) or from a separate power monitor module.
	स्थिर constexpr भिन्न BAT_अस्थायी_MAX = 100.0f;
};

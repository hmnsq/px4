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
 * @file battery.cpp
 *
 * Library calls for battery functionality.
 *
 * @author Julian Oes <julian@oes.ch>
 * @author Timothy Scott <timothy@auterion.com>
 */

#अन्तर्भाव "battery.h"
#अन्तर्भाव <mathlib/mathlib.h>
#अन्तर्भाव <cstring>
#अन्तर्भाव <px4_platform_common/defines.h>

उपयुज्य नामाकाश time_literals;
उपयुज्य नामाकाश matrix;

Battery::Battery(पूर्णाङ्क सूचकांक, ModuleParams *parent, ध्रुव पूर्णाङ्क sample_interval_us, ध्रुव uint8_t source) :
	ModuleParams(parent),
	सूचकांक_2(सूचकांक < 1 || सूचकांक > 9 ? 1 : सूचकांक),
	_source(source)
{
	ध्रुव भिन्न expected_filter_dt = static_cast<भिन्न>(sample_interval_us) * 1e-6f;
	current_मध्यमानम्_filter_a.समुच्चयः_Parameters(expected_filter_dt, 50.f);
	_ocv_filter_v.समुच्चयः_Parameters(expected_filter_dt, 1.f);
	_cell_voltage_filter_v.समुच्चयः_Parameters(expected_filter_dt, 1.f);

	यदि (सूचकांक > 9 || सूचकांक < 1) {
		PX4_ERR("Battery index must be between 1 and 9 (inclusive). Received %d. Defaulting to 1.", सूचकांक);
	}

	// 16 chars for parameter name + null terminator
	वर्ण param_name[17];

	snprintf(param_name, परिमाण(param_name), "BAT%d_V_EMPTY", सूचकांक_2);
	_param_handles.v_रिक्तानि = param_find(param_name);

	यदि (_param_handles.v_रिक्तानि == PARAM_INVALID) {
		PX4_ERR("Could not find parameter with name %s", param_name);
	}

	snprintf(param_name, परिमाण(param_name), "BAT%d_V_CHARGED", सूचकांक_2);
	_param_handles.v_charged = param_find(param_name);

	snprintf(param_name, परिमाण(param_name), "BAT%d_N_CELLS", सूचकांक_2);
	_param_handles.n_cells = param_find(param_name);

	snprintf(param_name, परिमाण(param_name), "BAT%d_CAPACITY", सूचकांक_2);
	_param_handles.क्षमता = param_find(param_name);

	snprintf(param_name, परिमाण(param_name), "BAT%d_R_INTERNAL", सूचकांक_2);
	_param_handles.r_आन्तरिकम् = param_find(param_name);

	snprintf(param_name, परिमाण(param_name), "BAT%d_SOURCE", सूचकांक_2);
	_param_handles.source = param_find(param_name);

	snprintf(param_name, परिमाण(param_name), "BAT%d_I_OVERWRITE", सूचकांक_2);
	_param_handles.i_overwrite = param_find(param_name);

	_param_handles.नीचः_thr = param_find("BAT_LOW_THR");
	_param_handles.crit_thr = param_find("BAT_CRIT_THR");
	_param_handles.emergen_thr = param_find("BAT_EMERGEN_THR");

	_param_handles.bat_avrg_current = param_find("BAT_AVRG_CURRENT");

	updateParams();
}

शून्य Battery::updateVoltage(ध्रुव भिन्न voltage_v)
{
	_voltage_v = voltage_v;
}

शून्य Battery::updateCurrent(ध्रुव भिन्न current_a)
{
	// Overwrite the measured current if current overwrite is defined and vehicle is unarmed
	यदि (!_armed && _params.i_overwrite > FLT_EPSILON) {
		_current_a = _params.i_overwrite;

	} अन्यथा {
		_current_a = current_a;
	}
}

शून्य Battery::updateTemperature(ध्रुव भिन्न temperature_c)
{
	_temperature_c = temperature_c;
}

शून्य Battery::update_Battery_स्थितिः(ध्रुव hrt_abstime &timestamp)
{
	updateDt(timestamp);

	// Require minimum voltage otherwise override connected status
	यदि (_voltage_v < LITHIUM_BATTERY_RECOGNITION_VOLTAGE) {
		_connected = असत्य;
	}

	यदि (!_connected || (चरमः_unconnected_timestamp == 0)) {
		चरमः_unconnected_timestamp = timestamp;
	}

	// Wait with initializing filters to avoid relying on a voltage sample from the rising edge
	_battery_initialized = _connected && (timestamp > चरमः_unconnected_timestamp + 2_s);

	यदि (_connected && !_battery_initialized && आन्तरिकम्_resistance_initialized && _params.n_cells > 0) {
		reset_आन्तरिकम्_Resistance_Estimation(_voltage_v, _current_a);
	}

	योग_Discharged(_current_a);
	state_सम्बन्धि_charge_volt_based =
		calculate_State_सम्बन्धि_Charge_Voltage_Based(_voltage_v, _current_a);

	यदि (!external_state_सम्बन्धि_charge) {
		estimate_State_सम्बन्धि_Charge();
	}

	computeScale();

	यदि (_connected && _battery_initialized) {
		_warning = determineWarning(state_सम्बन्धि_charge);
	}

	यदि (vehicle_स्थितिः_sub.updated()) {
		vehicle_स्थितिः_s vehicle_स्थितिः;

		यदि (vehicle_स्थितिः_sub.copy(&vehicle_स्थितिः)) {
			_armed = (vehicle_स्थितिः.arming_state == vehicle_स्थितिः_s::ARMING_STATE_ARMED);
			vehicle_स्थितिः_is_fw = (vehicle_स्थितिः.vehicle_type == vehicle_स्थितिः_s::VEHICLE_TYPE_FIXED_WING);
		}
	}
}

battery_स्थितिः_s Battery::get_Battery_स्थितिः()
{
	battery_स्थितिः_s battery_स्थितिः{};
	battery_स्थितिः.voltage_v = _voltage_v;
	battery_स्थितिः.current_a = _current_a;
	battery_स्थितिः.current_मध्यमानम्_a = current_मध्यमानम्_filter_a.getState();
	battery_स्थितिः.discharged_mah = _discharged_mah;
	battery_स्थितिः.remaining = state_सम्बन्धि_charge;
	battery_स्थितिः.scale = _scale;
	battery_स्थितिः.time_remaining_s = computeRemainingTime(_current_a);
	battery_स्थितिः.temperature = _temperature_c;
	battery_स्थितिः.cell_गणनम् = _params.n_cells;
	battery_स्थितिः.connected = _connected;
	battery_स्थितिः.source = _source;
	battery_स्थितिः.प्राथमिकता = प्राथमिकता_2;
	battery_स्थितिः.क्षमता = static_cast<uint16_t>(क्षमता_mah);
	battery_स्थितिः.id = static_cast<uint8_t>(सूचकांक_2);
	battery_स्थितिः.warning = _warning;
	battery_स्थितिः.timestamp = hrt_absolute_time();
	battery_स्थितिः.दोषाः = determine_दोषाः();
	battery_स्थितिः.remaining_क्षमता_wh = NAN; // not measured by dumb power module; smart-battery drivers overwrite
	battery_स्थितिः.nominal_voltage = NAN;        // not measured by dumb power module; smart-battery drivers overwrite
	battery_स्थितिः.आन्तरिकम्_resistance_estimate = आन्तरिकम्_resistance_estimate_2;
	battery_स्थितिः.ocv_estimate = _voltage_v + आन्तरिकम्_resistance_estimate_2 * _params.n_cells * _current_a;
	battery_स्थितिः.ocv_estimate_filtered = _ocv_filter_v.getState();
	battery_स्थितिः.volt_based_soc_estimate = _params.n_cells > 0 ? math::interpolate(_ocv_filter_v.getState() /
			_params.n_cells,
			_params.v_रिक्तानि, _params.v_charged, 0.f, 1.f) : -1.f;
	battery_स्थितिः.voltage_prediction = _voltage_prediction;
	battery_स्थितिः.prediction_error = _prediction_error;
	battery_स्थितिः.estimation_covariance_norm = _estimation_covariance_norm;
	प्रत्यागम battery_स्थितिः;
}

शून्य Battery::publish_Battery_स्थितिः(ध्रुव battery_स्थितिः_s &battery_स्थितिः)
{
	यदि (_source == _params.source) {
		battery_स्थितिः_pub.publish(battery_स्थितिः);
	}
}

शून्य Battery::update_And_Publish_Battery_स्थितिः(ध्रुव hrt_abstime &timestamp)
{
	update_Battery_स्थितिः(timestamp);

	battery_स्थितिः_s battery_स्थितिः = get_Battery_स्थितिः();

	_failure_config.update();

	यदि (!failure_injection::process_battery(_failure_config, battery_स्थितिः.id, battery_स्थितिः)) {
		प्रत्यागम;
	}

	publish_Battery_स्थितिः(battery_स्थितिः);
}
शून्य Battery::updateDt(ध्रुव hrt_abstime &timestamp)
{
	यदि (चरमः_timestamp != 0) {
		// _dt_discharge is the true, unclamped time delta: for the coulomb
		// count in sumDischarged() below, using the real elapsed time is
		// always more accurate than clamping it, even across an unusually
		// long gap between updates (e.g. an update source that reports
		// less often than every 2 seconds).
		_dt_discharge = (timestamp - चरमः_timestamp) / 1e6f;

		// _dt is clamped to guard the numerical stability of the current
		// average filter below, which assumes a roughly steady sample
		// interval and should not see a single very large dt.
		_dt = math::min(_dt_discharge, 2.f);
	}

	चरमः_timestamp = timestamp;
}

भिन्न Battery::योग_Discharged(भिन्न current_a)
{
	यदि (_dt_discharge > FLT_EPSILON && fabsf(current_a + 1.f) > FLT_EPSILON) {
		// mAh since last loop: (current[A] * 1000 = [mA]) * (dt[s] / 3600 = [h])
		// current = -1 means invalid current measurement
		_discharged_mah_loop = (current_a * 1e3f) * (_dt_discharge / 3600.f);
		_discharged_mah += _discharged_mah_loop;
	}

	प्रत्यागम _discharged_mah;
}

भिन्न Battery::calculate_State_सम्बन्धि_Charge_Voltage_Based(ध्रुव भिन्न voltage_v, ध्रुव भिन्न current_a)
{
	यदि (_params.n_cells == 0) {
		प्रत्यागम -1.0f;
	}

	// remaining battery capacity based on voltage
	भिन्न cell_voltage = voltage_v / _params.n_cells;

	// correct battery voltage locally for load drop according to internal resistance and current
	यदि (current_a > FLT_EPSILON) {
		update_आन्तरिकम्_Resistance_Estimation(voltage_v, current_a);

		यदि (_params.r_आन्तरिकम् >= 0.f) { // Use user specified internal resistance value
			cell_voltage += _params.r_आन्तरिकम् * current_a;

		} अन्यथा { // Use estimated internal resistance value
			cell_voltage += आन्तरिकम्_resistance_estimate_2 * current_a;
		}

	}

	_cell_voltage_filter_v.update(cell_voltage);
	प्रत्यागम math::interpolate(_cell_voltage_filter_v.getState(), _params.v_रिक्तानि, _params.v_charged, 0.f, 1.f);
}

शून्य Battery::update_आन्तरिकम्_Resistance_Estimation(ध्रुव भिन्न voltage_v, ध्रुव भिन्न current_a)
{
	Vector2f x{1, -current_a};
	_voltage_prediction = (x.transpose() * _RLS_est)(0, 0);
	_prediction_error = voltage_v - _voltage_prediction;
	ध्रुव Vector2f gamma = _estimation_covariance * x / (LAMBDA + (x.transpose() * _estimation_covariance * x)(0, 0));
	ध्रुव Vector2f RSL_est_अस्थायी = _RLS_est + gamma * _prediction_error;
	ध्रुव Matrix2f estimation_covariance_अस्थायी = (_estimation_covariance
			- Matrix<भिन्न, 2, 1>(gamma) * (x.transpose() * _estimation_covariance)) / LAMBDA;
	ध्रुव भिन्न estimation_covariance_अस्थायी_norm =
		sqrtf(powf(estimation_covariance_अस्थायी(0, 0), 2.f)
		      + 2.f * powf(estimation_covariance_अस्थायी(1, 0), 2.f)
		      + powf(estimation_covariance_अस्थायी(1, 1), 2.f));

	यदि (estimation_covariance_अस्थायी_norm < _estimation_covariance_norm) { // Only update if estimation improves
		_RLS_est = RSL_est_अस्थायी;
		_estimation_covariance = estimation_covariance_अस्थायी;
		_estimation_covariance_norm = estimation_covariance_अस्थायी_norm;
		आन्तरिकम्_resistance_estimate_2 =
			math::max(_RLS_est(1) / _params.n_cells, 0.f); // Only use positive values

	} अन्यथा { // Update OCV estimate with IR estimate
		_RLS_est(0) = voltage_v + _RLS_est(1) * current_a;
	}

	_ocv_filter_v.update(voltage_v + आन्तरिकम्_resistance_estimate_2 * _params.n_cells * current_a);
}

शून्य Battery::reset_आन्तरिकम्_Resistance_Estimation(ध्रुव भिन्न voltage_v, ध्रुव भिन्न current_a)
{
	_RLS_est(0) = voltage_v;
	_RLS_est(1) = R_DEFAULT * _params.n_cells;
	_estimation_covariance.समुच्चयः_Zero();
	_estimation_covariance(0, 0) = OCV_COVARIANCE * _params.n_cells;
	_estimation_covariance(1, 1) = R_COVARIANCE * _params.n_cells;
	_estimation_covariance_norm = sqrtf(powf(_estimation_covariance(0, 0), 2.f) + 2.f * powf(_estimation_covariance(1, 0),
					    2.f) + powf(_estimation_covariance(1, 1), 2.f));
	आन्तरिकम्_resistance_estimate_2 = R_DEFAULT;
	_ocv_filter_v.reset(voltage_v + आन्तरिकम्_resistance_estimate_2 * _params.n_cells * current_a);

	यदि (_params.r_आन्तरिकम् >= 0.f) { // Use user specified internal resistance value
		_cell_voltage_filter_v.reset(voltage_v / _params.n_cells + _params.r_आन्तरिकम् * current_a);

	} अन्यथा { // Use estimated internal resistance value
		_cell_voltage_filter_v.reset(voltage_v / _params.n_cells + आन्तरिकम्_resistance_estimate_2 * current_a);
	}
}

शून्य Battery::estimate_State_सम्बन्धि_Charge()
{
	// choose which quantity we're using for final reporting
	यदि ((क्षमता_mah > 0.f) && _battery_initialized) {
		// if battery capacity is known, fuse voltage measurement with used capacity
		// The lower the voltage the more adjust the estimate with it to avoid deep discharge
		ध्रुव भिन्न weight_v = 3e-2f * (1 - state_सम्बन्धि_charge_volt_based);
		state_सम्बन्धि_charge = (1 - weight_v) * state_सम्बन्धि_charge + weight_v * state_सम्बन्धि_charge_volt_based;
		// directly apply current capacity slope calculated using current
		state_सम्बन्धि_charge -= _discharged_mah_loop / क्षमता_mah;
		state_सम्बन्धि_charge = math::max(state_सम्बन्धि_charge, 0.f);

		ध्रुव भिन्न state_सम्बन्धि_charge_current_based = math::max(1.f - _discharged_mah / क्षमता_mah, 0.f);
		state_सम्बन्धि_charge = math::min(state_सम्बन्धि_charge_current_based, state_सम्बन्धि_charge);

	} अन्यथा {
		state_सम्बन्धि_charge = state_सम्बन्धि_charge_volt_based;
	}
}

uint8_t Battery::determineWarning(भिन्न state_सम्बन्धि_charge_2)
{
	यदि (state_सम्बन्धि_charge_2 < _params.emergen_thr) {
		प्रत्यागम battery_स्थितिः_s::WARNING_EMERGENCY;

	} अन्यथा यदि (state_सम्बन्धि_charge_2 < _params.crit_thr) {
		प्रत्यागम battery_स्थितिः_s::WARNING_CRITICAL;

	} अन्यथा यदि (state_सम्बन्धि_charge_2 < _params.नीचः_thr) {
		प्रत्यागम battery_स्थितिः_s::WARNING_नीचः;

	} अन्यथा {
		प्रत्यागम battery_स्थितिः_s::WARNING_NONE;
	}
}

uint16_t Battery::determine_दोषाः()
{
	uint16_t दोषाः{0};

	यदि ((_params.n_cells > 0)
	    && (_voltage_v > (_params.n_cells * _params.v_charged * 1.05f))) {
		// Reported as a "spike" since "over-voltage" does not exist in MAV_BATTERY_FAULT
		दोषाः |= (1 << battery_स्थितिः_s::FAULT_SPIKES);
	}

	यदि (PX4_ISFINITE(_temperature_c) && _temperature_c > BAT_अस्थायी_MAX) {
		दोषाः |= (1 << battery_स्थितिः_s::FAULT_OVER_TEMPERATURE);
	}

	प्रत्यागम दोषाः;
}

शून्य Battery::computeScale()
{
	ध्रुव भिन्न cell_voltage = _cell_voltage_filter_v.getState();

	यदि (cell_voltage > FLT_EPSILON) {
		_scale = math::constrain(_params.v_charged / cell_voltage, 1.f, 1.3f); // Allow at most 30% compensation

	} अन्यथा {
		_scale = 1.f;
	}
}

भिन्न Battery::computeRemainingTime(भिन्न current_a)
{
	भिन्न time_remaining_s = NAN;

	_flight_phase_estimation_sub.update();

	तार्किक reset_filter_transition_now = !vehicle_स्थितिः_was_fw && vehicle_स्थितिः_is_fw;

	// reset filter if not feasible, negative or we did a VTOL transition to FW mode
	यदि (!PX4_ISFINITE(current_मध्यमानम्_filter_a.getState()) || current_मध्यमानम्_filter_a.getState() < FLT_EPSILON
	    || reset_filter_transition_now) {
		current_मध्यमानम्_filter_a.reset(_params.bat_avrg_current);
	}

	vehicle_स्थितिः_was_fw = vehicle_स्थितिः_is_fw;

	यदि (_armed && PX4_ISFINITE(current_a)) {
		// For FW only update when we are in level flight
		यदि (!vehicle_स्थितिः_is_fw || ((hrt_absolute_time() - _flight_phase_estimation_sub.get().timestamp) < 2_s
					       && _flight_phase_estimation_sub.get().flight_phase == flight_phase_estimation_s::FLIGHT_PHASE_LEVEL)) {
			यदि (_dt > FLT_EPSILON) {
				current_मध्यमानम्_filter_a.update(fmaxf(current_a, 0.f), _dt);

			} अन्यथा {
				current_मध्यमानम्_filter_a.update(fmaxf(current_a, 0.f));
			}
		}
	}

	// Remaining time estimation only possible with capacity
	यदि (क्षमता_mah > 0.f) {
		ध्रुव भिन्न remaining_क्षमता_mah = state_सम्बन्धि_charge * क्षमता_mah;
		ध्रुव भिन्न current_ma = fmaxf(current_मध्यमानम्_filter_a.getState() * 1e3f, FLT_EPSILON);
		time_remaining_s = remaining_क्षमता_mah / current_ma * 3600.f;
	}

	प्रत्यागम time_remaining_s;
}

शून्य Battery::updateParams()
{
	ध्रुव पूर्णाङ्क n_cells = _first_parameter_update ? 0 : _params.n_cells;
	param_get(_param_handles.v_रिक्तानि, &_params.v_रिक्तानि);
	param_get(_param_handles.v_charged, &_params.v_charged);
	param_get(_param_handles.n_cells, &_params.n_cells);
	param_get(_param_handles.r_आन्तरिकम्, &_params.r_आन्तरिकम्);
	param_get(_param_handles.source, &_params.source);
	param_get(_param_handles.नीचः_thr, &_params.नीचः_thr);
	param_get(_param_handles.crit_thr, &_params.crit_thr);
	param_get(_param_handles.emergen_thr, &_params.emergen_thr);
	param_get(_param_handles.bat_avrg_current, &_params.bat_avrg_current);
	param_get(_param_handles.i_overwrite, &_params.i_overwrite);

	भिन्न क्षमता{0.f};
	param_get(_param_handles.क्षमता, &क्षमता);
	समुच्चयः_क्षमता_Mah(क्षमता);

	यदि (n_cells != _params.n_cells) {
		आन्तरिकम्_resistance_initialized = असत्य;
	}

	यदि (!आन्तरिकम्_resistance_initialized && _params.n_cells > 0) {
		_RLS_est(0) = OCV_DEFAULT * _params.n_cells;
		_RLS_est(1) = R_DEFAULT * _params.n_cells;
		_estimation_covariance(0, 0) = OCV_COVARIANCE * _params.n_cells;
		_estimation_covariance(0, 1) = 0.f;
		_estimation_covariance(1, 0) = 0.f;
		_estimation_covariance(1, 1) = R_COVARIANCE * _params.n_cells;
		_estimation_covariance_norm = sqrtf(powf(_estimation_covariance(0, 0), 2.f) + 2.f * powf(_estimation_covariance(1, 0),
						    2.f) + powf(_estimation_covariance(1, 1), 2.f));
		आन्तरिकम्_resistance_initialized = सत्य;
	}

	ModuleParams::updateParams();

	_first_parameter_update = असत्य;
}

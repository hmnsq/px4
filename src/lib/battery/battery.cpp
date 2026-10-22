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

using नामाकाश time_literals;
using नामाकाश matrix;

Battery::Battery(पूर्णाङ्क index, ModuleParams *parent, ध्रुव पूर्णाङ्क sample_interval_us, ध्रुव uint8_t source) :
	ModuleParams(parent),
	_index(index < 1 || index > 9 ? 1 : index),
	_source(source)
{
	ध्रुव भिन्न expected_filter_dt = static_cast<भिन्न>(sample_interval_us) * 1e-6f;
	_current_average_filter_a.setParameters(expected_filter_dt, 50.f);
	_ocv_filter_v.setParameters(expected_filter_dt, 1.f);
	_cell_voltage_filter_v.setParameters(expected_filter_dt, 1.f);

	यदि (index > 9 || index < 1) {
		PX4_ERR("Battery index must be between 1 and 9 (inclusive). Received %d. Defaulting to 1.", index);
	}

	// 16 chars for parameter name + null terminator
	वर्ण param_name[17];

	snprintf(param_name, परिमाण(param_name), "BAT%d_V_EMPTY", _index);
	_param_handles.v_empty = param_find(param_name);

	यदि (_param_handles.v_empty == PARAM_INVALID) {
		PX4_ERR("Could not find parameter with name %s", param_name);
	}

	snprintf(param_name, परिमाण(param_name), "BAT%d_V_CHARGED", _index);
	_param_handles.v_charged = param_find(param_name);

	snprintf(param_name, परिमाण(param_name), "BAT%d_N_CELLS", _index);
	_param_handles.n_cells = param_find(param_name);

	snprintf(param_name, परिमाण(param_name), "BAT%d_CAPACITY", _index);
	_param_handles.capacity = param_find(param_name);

	snprintf(param_name, परिमाण(param_name), "BAT%d_R_INTERNAL", _index);
	_param_handles.r_internal = param_find(param_name);

	snprintf(param_name, परिमाण(param_name), "BAT%d_SOURCE", _index);
	_param_handles.source = param_find(param_name);

	snprintf(param_name, परिमाण(param_name), "BAT%d_I_OVERWRITE", _index);
	_param_handles.i_overwrite = param_find(param_name);

	_param_handles.निचला_thr = param_find("BAT_LOW_THR");
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

शून्य Battery::updateBatteryStatus(ध्रुव hrt_abstime &timestamp)
{
	updateDt(timestamp);

	// Require minimum voltage otherwise override connected status
	यदि (_voltage_v < LITHIUM_BATTERY_RECOGNITION_VOLTAGE) {
		_connected = असत्य;
	}

	यदि (!_connected || (_last_unconnected_timestamp == 0)) {
		_last_unconnected_timestamp = timestamp;
	}

	// Wait with initializing filters to avoid relying on a voltage sample from the rising edge
	_battery_initialized = _connected && (timestamp > _last_unconnected_timestamp + 2_s);

	यदि (_connected && !_battery_initialized && _internal_resistance_initialized && _params.n_cells > 0) {
		resetInternalResistanceEstimation(_voltage_v, _current_a);
	}

	sumDischarged(_current_a);
	_state_of_charge_volt_based =
		calculateStateOfChargeVoltageBased(_voltage_v, _current_a);

	यदि (!_external_state_of_charge) {
		estimateStateOfCharge();
	}

	computeScale();

	यदि (_connected && _battery_initialized) {
		_warning = determineWarning(_state_of_charge);
	}

	यदि (_vehicle_status_sub.updated()) {
		vehicle_status_s vehicle_status;

		यदि (_vehicle_status_sub.copy(&vehicle_status)) {
			_armed = (vehicle_status.arming_state == vehicle_status_s::ARMING_STATE_ARMED);
			_vehicle_status_is_fw = (vehicle_status.vehicle_type == vehicle_status_s::VEHICLE_TYPE_FIXED_WING);
		}
	}
}

battery_status_s Battery::getBatteryStatus()
{
	battery_status_s battery_status{};
	battery_status.voltage_v = _voltage_v;
	battery_status.current_a = _current_a;
	battery_status.current_average_a = _current_average_filter_a.getState();
	battery_status.discharged_mah = _discharged_mah;
	battery_status.remaining = _state_of_charge;
	battery_status.scale = _scale;
	battery_status.time_remaining_s = computeRemainingTime(_current_a);
	battery_status.temperature = _temperature_c;
	battery_status.cell_गिनती = _params.n_cells;
	battery_status.connected = _connected;
	battery_status.source = _source;
	battery_status.priority = _priority;
	battery_status.capacity = static_cast<uint16_t>(_capacity_mah);
	battery_status.id = static_cast<uint8_t>(_index);
	battery_status.warning = _warning;
	battery_status.timestamp = hrt_absolute_time();
	battery_status.faults = determineFaults();
	battery_status.remaining_capacity_wh = NAN; // not measured by dumb power module; smart-battery drivers overwrite
	battery_status.nominal_voltage = NAN;        // not measured by dumb power module; smart-battery drivers overwrite
	battery_status.internal_resistance_estimate = _internal_resistance_estimate;
	battery_status.ocv_estimate = _voltage_v + _internal_resistance_estimate * _params.n_cells * _current_a;
	battery_status.ocv_estimate_filtered = _ocv_filter_v.getState();
	battery_status.volt_based_soc_estimate = _params.n_cells > 0 ? math::interpolate(_ocv_filter_v.getState() /
			_params.n_cells,
			_params.v_empty, _params.v_charged, 0.f, 1.f) : -1.f;
	battery_status.voltage_prediction = _voltage_prediction;
	battery_status.prediction_error = _prediction_error;
	battery_status.estimation_covariance_norm = _estimation_covariance_norm;
	प्रत्यागम battery_status;
}

शून्य Battery::publishBatteryStatus(ध्रुव battery_status_s &battery_status)
{
	यदि (_source == _params.source) {
		_battery_status_pub.publish(battery_status);
	}
}

शून्य Battery::updateAndPublishBatteryStatus(ध्रुव hrt_abstime &timestamp)
{
	updateBatteryStatus(timestamp);

	battery_status_s battery_status = getBatteryStatus();

	_failure_config.update();

	यदि (!failure_injection::process_battery(_failure_config, battery_status.id, battery_status)) {
		प्रत्यागम;
	}

	publishBatteryStatus(battery_status);
}
शून्य Battery::updateDt(ध्रुव hrt_abstime &timestamp)
{
	यदि (_last_timestamp != 0) {
		// _dt_discharge is the true, unclamped time delta: for the coulomb
		// count in sumDischarged() below, using the real elapsed time is
		// always more accurate than clamping it, even across an unusually
		// long gap between updates (e.g. an update source that reports
		// less often than every 2 seconds).
		_dt_discharge = (timestamp - _last_timestamp) / 1e6f;

		// _dt is clamped to guard the numerical stability of the current
		// average filter below, which assumes a roughly steady sample
		// interval and should not see a single very large dt.
		_dt = math::min(_dt_discharge, 2.f);
	}

	_last_timestamp = timestamp;
}

भिन्न Battery::sumDischarged(भिन्न current_a)
{
	यदि (_dt_discharge > FLT_EPSILON && fabsf(current_a + 1.f) > FLT_EPSILON) {
		// mAh since last loop: (current[A] * 1000 = [mA]) * (dt[s] / 3600 = [h])
		// current = -1 means invalid current measurement
		_discharged_mah_loop = (current_a * 1e3f) * (_dt_discharge / 3600.f);
		_discharged_mah += _discharged_mah_loop;
	}

	प्रत्यागम _discharged_mah;
}

भिन्न Battery::calculateStateOfChargeVoltageBased(ध्रुव भिन्न voltage_v, ध्रुव भिन्न current_a)
{
	यदि (_params.n_cells == 0) {
		प्रत्यागम -1.0f;
	}

	// remaining battery capacity based on voltage
	भिन्न cell_voltage = voltage_v / _params.n_cells;

	// correct battery voltage locally for load drop according to internal resistance and current
	यदि (current_a > FLT_EPSILON) {
		updateInternalResistanceEstimation(voltage_v, current_a);

		यदि (_params.r_internal >= 0.f) { // Use user specified internal resistance value
			cell_voltage += _params.r_internal * current_a;

		} अन्यथा { // Use estimated internal resistance value
			cell_voltage += _internal_resistance_estimate * current_a;
		}

	}

	_cell_voltage_filter_v.update(cell_voltage);
	प्रत्यागम math::interpolate(_cell_voltage_filter_v.getState(), _params.v_empty, _params.v_charged, 0.f, 1.f);
}

शून्य Battery::updateInternalResistanceEstimation(ध्रुव भिन्न voltage_v, ध्रुव भिन्न current_a)
{
	Vector2f x{1, -current_a};
	_voltage_prediction = (x.transpose() * _RLS_est)(0, 0);
	_prediction_error = voltage_v - _voltage_prediction;
	ध्रुव Vector2f gamma = _estimation_covariance * x / (LAMBDA + (x.transpose() * _estimation_covariance * x)(0, 0));
	ध्रुव Vector2f RSL_est_temp = _RLS_est + gamma * _prediction_error;
	ध्रुव Matrix2f estimation_covariance_temp = (_estimation_covariance
			- Matrix<भिन्न, 2, 1>(gamma) * (x.transpose() * _estimation_covariance)) / LAMBDA;
	ध्रुव भिन्न estimation_covariance_temp_norm =
		sqrtf(powf(estimation_covariance_temp(0, 0), 2.f)
		      + 2.f * powf(estimation_covariance_temp(1, 0), 2.f)
		      + powf(estimation_covariance_temp(1, 1), 2.f));

	यदि (estimation_covariance_temp_norm < _estimation_covariance_norm) { // Only update if estimation improves
		_RLS_est = RSL_est_temp;
		_estimation_covariance = estimation_covariance_temp;
		_estimation_covariance_norm = estimation_covariance_temp_norm;
		_internal_resistance_estimate =
			math::max(_RLS_est(1) / _params.n_cells, 0.f); // Only use positive values

	} अन्यथा { // Update OCV estimate with IR estimate
		_RLS_est(0) = voltage_v + _RLS_est(1) * current_a;
	}

	_ocv_filter_v.update(voltage_v + _internal_resistance_estimate * _params.n_cells * current_a);
}

शून्य Battery::resetInternalResistanceEstimation(ध्रुव भिन्न voltage_v, ध्रुव भिन्न current_a)
{
	_RLS_est(0) = voltage_v;
	_RLS_est(1) = R_DEFAULT * _params.n_cells;
	_estimation_covariance.setZero();
	_estimation_covariance(0, 0) = OCV_COVARIANCE * _params.n_cells;
	_estimation_covariance(1, 1) = R_COVARIANCE * _params.n_cells;
	_estimation_covariance_norm = sqrtf(powf(_estimation_covariance(0, 0), 2.f) + 2.f * powf(_estimation_covariance(1, 0),
					    2.f) + powf(_estimation_covariance(1, 1), 2.f));
	_internal_resistance_estimate = R_DEFAULT;
	_ocv_filter_v.reset(voltage_v + _internal_resistance_estimate * _params.n_cells * current_a);

	यदि (_params.r_internal >= 0.f) { // Use user specified internal resistance value
		_cell_voltage_filter_v.reset(voltage_v / _params.n_cells + _params.r_internal * current_a);

	} अन्यथा { // Use estimated internal resistance value
		_cell_voltage_filter_v.reset(voltage_v / _params.n_cells + _internal_resistance_estimate * current_a);
	}
}

शून्य Battery::estimateStateOfCharge()
{
	// choose which quantity we're using for final reporting
	यदि ((_capacity_mah > 0.f) && _battery_initialized) {
		// if battery capacity is known, fuse voltage measurement with used capacity
		// The lower the voltage the more adjust the estimate with it to avoid deep discharge
		ध्रुव भिन्न weight_v = 3e-2f * (1 - _state_of_charge_volt_based);
		_state_of_charge = (1 - weight_v) * _state_of_charge + weight_v * _state_of_charge_volt_based;
		// directly apply current capacity slope calculated using current
		_state_of_charge -= _discharged_mah_loop / _capacity_mah;
		_state_of_charge = math::max(_state_of_charge, 0.f);

		ध्रुव भिन्न state_of_charge_current_based = math::max(1.f - _discharged_mah / _capacity_mah, 0.f);
		_state_of_charge = math::min(state_of_charge_current_based, _state_of_charge);

	} अन्यथा {
		_state_of_charge = _state_of_charge_volt_based;
	}
}

uint8_t Battery::determineWarning(भिन्न state_of_charge)
{
	यदि (state_of_charge < _params.emergen_thr) {
		प्रत्यागम battery_status_s::WARNING_EMERGENCY;

	} अन्यथा यदि (state_of_charge < _params.crit_thr) {
		प्रत्यागम battery_status_s::WARNING_CRITICAL;

	} अन्यथा यदि (state_of_charge < _params.निचला_thr) {
		प्रत्यागम battery_status_s::WARNING_निचला;

	} अन्यथा {
		प्रत्यागम battery_status_s::WARNING_NONE;
	}
}

uint16_t Battery::determineFaults()
{
	uint16_t faults{0};

	यदि ((_params.n_cells > 0)
	    && (_voltage_v > (_params.n_cells * _params.v_charged * 1.05f))) {
		// Reported as a "spike" since "over-voltage" does not exist in MAV_BATTERY_FAULT
		faults |= (1 << battery_status_s::FAULT_SPIKES);
	}

	यदि (PX4_ISFINITE(_temperature_c) && _temperature_c > BAT_TEMP_MAX) {
		faults |= (1 << battery_status_s::FAULT_OVER_TEMPERATURE);
	}

	प्रत्यागम faults;
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

	तार्किक reset_filter_transition_now = !_vehicle_status_was_fw && _vehicle_status_is_fw;

	// reset filter if not feasible, negative or we did a VTOL transition to FW mode
	यदि (!PX4_ISFINITE(_current_average_filter_a.getState()) || _current_average_filter_a.getState() < FLT_EPSILON
	    || reset_filter_transition_now) {
		_current_average_filter_a.reset(_params.bat_avrg_current);
	}

	_vehicle_status_was_fw = _vehicle_status_is_fw;

	यदि (_armed && PX4_ISFINITE(current_a)) {
		// For FW only update when we are in level flight
		यदि (!_vehicle_status_is_fw || ((hrt_absolute_time() - _flight_phase_estimation_sub.get().timestamp) < 2_s
					       && _flight_phase_estimation_sub.get().flight_phase == flight_phase_estimation_s::FLIGHT_PHASE_LEVEL)) {
			यदि (_dt > FLT_EPSILON) {
				_current_average_filter_a.update(fmaxf(current_a, 0.f), _dt);

			} अन्यथा {
				_current_average_filter_a.update(fmaxf(current_a, 0.f));
			}
		}
	}

	// Remaining time estimation only possible with capacity
	यदि (_capacity_mah > 0.f) {
		ध्रुव भिन्न remaining_capacity_mah = _state_of_charge * _capacity_mah;
		ध्रुव भिन्न current_ma = fmaxf(_current_average_filter_a.getState() * 1e3f, FLT_EPSILON);
		time_remaining_s = remaining_capacity_mah / current_ma * 3600.f;
	}

	प्रत्यागम time_remaining_s;
}

शून्य Battery::updateParams()
{
	ध्रुव पूर्णाङ्क n_cells = _first_parameter_update ? 0 : _params.n_cells;
	param_get(_param_handles.v_empty, &_params.v_empty);
	param_get(_param_handles.v_charged, &_params.v_charged);
	param_get(_param_handles.n_cells, &_params.n_cells);
	param_get(_param_handles.r_internal, &_params.r_internal);
	param_get(_param_handles.source, &_params.source);
	param_get(_param_handles.निचला_thr, &_params.निचला_thr);
	param_get(_param_handles.crit_thr, &_params.crit_thr);
	param_get(_param_handles.emergen_thr, &_params.emergen_thr);
	param_get(_param_handles.bat_avrg_current, &_params.bat_avrg_current);
	param_get(_param_handles.i_overwrite, &_params.i_overwrite);

	भिन्न capacity{0.f};
	param_get(_param_handles.capacity, &capacity);
	setCapacityMah(capacity);

	यदि (n_cells != _params.n_cells) {
		_internal_resistance_initialized = असत्य;
	}

	यदि (!_internal_resistance_initialized && _params.n_cells > 0) {
		_RLS_est(0) = OCV_DEFAULT * _params.n_cells;
		_RLS_est(1) = R_DEFAULT * _params.n_cells;
		_estimation_covariance(0, 0) = OCV_COVARIANCE * _params.n_cells;
		_estimation_covariance(0, 1) = 0.f;
		_estimation_covariance(1, 0) = 0.f;
		_estimation_covariance(1, 1) = R_COVARIANCE * _params.n_cells;
		_estimation_covariance_norm = sqrtf(powf(_estimation_covariance(0, 0), 2.f) + 2.f * powf(_estimation_covariance(1, 0),
						    2.f) + powf(_estimation_covariance(1, 1), 2.f));
		_internal_resistance_initialized = सत्य;
	}

	ModuleParams::updateParams();

	_first_parameter_update = असत्य;
}

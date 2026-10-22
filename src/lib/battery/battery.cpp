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

#அந்தர்பாவ "battery.h"
#அந்தர்பாவ <mathlib/mathlib.h>
#அந்தர்பாவ <cstring>
#அந்தர்பாவ <px4_platform_common/defines.h>

using நாமாகாஶ time_literals;
using நாமாகாஶ matrix;

Battery::Battery(முழு index, ModuleParams *parent, மாறிலி முழு sample_interval_us, மாறிலி uint8_t source) :
	ModuleParams(parent),
	_index(index < 1 || index > 9 ? 1 : index),
	_source(source)
{
	மாறிலி தசம expected_filter_dt = static_cast<தசம>(sample_interval_us) * 1e-6f;
	_current_average_filter_a.setParameters(expected_filter_dt, 50.f);
	_ocv_filter_v.setParameters(expected_filter_dt, 1.f);
	_cell_voltage_filter_v.setParameters(expected_filter_dt, 1.f);

	எனில் (index > 9 || index < 1) {
		PX4_ERR("Battery index must be between 1 and 9 (inclusive). Received %d. Defaulting to 1.", index);
	}

	// 16 chars for parameter name + null terminator
	எழுத்து param_name[17];

	snprintf(param_name, அளவு(param_name), "BAT%d_V_EMPTY", _index);
	_param_handles.v_empty = param_find(param_name);

	எனில் (_param_handles.v_empty == PARAM_INVALID) {
		PX4_ERR("Could not find parameter with name %s", param_name);
	}

	snprintf(param_name, அளவு(param_name), "BAT%d_V_CHARGED", _index);
	_param_handles.v_charged = param_find(param_name);

	snprintf(param_name, அளவு(param_name), "BAT%d_N_CELLS", _index);
	_param_handles.n_cells = param_find(param_name);

	snprintf(param_name, அளவு(param_name), "BAT%d_CAPACITY", _index);
	_param_handles.capacity = param_find(param_name);

	snprintf(param_name, அளவு(param_name), "BAT%d_R_INTERNAL", _index);
	_param_handles.r_internal = param_find(param_name);

	snprintf(param_name, அளவு(param_name), "BAT%d_SOURCE", _index);
	_param_handles.source = param_find(param_name);

	snprintf(param_name, அளவு(param_name), "BAT%d_I_OVERWRITE", _index);
	_param_handles.i_overwrite = param_find(param_name);

	_param_handles.கீழ்_thr = param_find("BAT_LOW_THR");
	_param_handles.crit_thr = param_find("BAT_CRIT_THR");
	_param_handles.emergen_thr = param_find("BAT_EMERGEN_THR");

	_param_handles.bat_avrg_current = param_find("BAT_AVRG_CURRENT");

	updateParams();
}

வெறுமை Battery::updateVoltage(மாறிலி தசம voltage_v)
{
	_voltage_v = voltage_v;
}

வெறுமை Battery::updateCurrent(மாறிலி தசம current_a)
{
	// Overwrite the measured current if current overwrite is defined and vehicle is unarmed
	எனில் (!_armed && _params.i_overwrite > FLT_EPSILON) {
		_current_a = _params.i_overwrite;

	} இல்லையெனில் {
		_current_a = current_a;
	}
}

வெறுமை Battery::updateTemperature(மாறிலி தசம temperature_c)
{
	_temperature_c = temperature_c;
}

வெறுமை Battery::updateBatteryStatus(மாறிலி hrt_abstime &timestamp)
{
	updateDt(timestamp);

	// Require minimum voltage otherwise override connected status
	எனில் (_voltage_v < LITHIUM_BATTERY_RECOGNITION_VOLTAGE) {
		_connected = பொய்;
	}

	எனில் (!_connected || (_last_unconnected_timestamp == 0)) {
		_last_unconnected_timestamp = timestamp;
	}

	// Wait with initializing filters to avoid relying on a voltage sample from the rising edge
	_battery_initialized = _connected && (timestamp > _last_unconnected_timestamp + 2_s);

	எனில் (_connected && !_battery_initialized && _internal_resistance_initialized && _params.n_cells > 0) {
		resetInternalResistanceEstimation(_voltage_v, _current_a);
	}

	sumDischarged(_current_a);
	_state_of_charge_volt_based =
		calculateStateOfChargeVoltageBased(_voltage_v, _current_a);

	எனில் (!_external_state_of_charge) {
		estimateStateOfCharge();
	}

	computeScale();

	எனில் (_connected && _battery_initialized) {
		_warning = determineWarning(_state_of_charge);
	}

	எனில் (_vehicle_status_sub.updated()) {
		vehicle_status_s vehicle_status;

		எனில் (_vehicle_status_sub.copy(&vehicle_status)) {
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
	battery_status.cell_மொத்தம் = _params.n_cells;
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
	திரும்பு battery_status;
}

வெறுமை Battery::publishBatteryStatus(மாறிலி battery_status_s &battery_status)
{
	எனில் (_source == _params.source) {
		_battery_status_pub.publish(battery_status);
	}
}

வெறுமை Battery::updateAndPublishBatteryStatus(மாறிலி hrt_abstime &timestamp)
{
	updateBatteryStatus(timestamp);

	battery_status_s battery_status = getBatteryStatus();

	_failure_config.update();

	எனில் (!failure_injection::process_battery(_failure_config, battery_status.id, battery_status)) {
		திரும்பு;
	}

	publishBatteryStatus(battery_status);
}
வெறுமை Battery::updateDt(மாறிலி hrt_abstime &timestamp)
{
	எனில் (_last_timestamp != 0) {
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

தசம Battery::sumDischarged(தசம current_a)
{
	எனில் (_dt_discharge > FLT_EPSILON && fabsf(current_a + 1.f) > FLT_EPSILON) {
		// mAh since last loop: (current[A] * 1000 = [mA]) * (dt[s] / 3600 = [h])
		// current = -1 means invalid current measurement
		_discharged_mah_loop = (current_a * 1e3f) * (_dt_discharge / 3600.f);
		_discharged_mah += _discharged_mah_loop;
	}

	திரும்பு _discharged_mah;
}

தசம Battery::calculateStateOfChargeVoltageBased(மாறிலி தசம voltage_v, மாறிலி தசம current_a)
{
	எனில் (_params.n_cells == 0) {
		திரும்பு -1.0f;
	}

	// remaining battery capacity based on voltage
	தசம cell_voltage = voltage_v / _params.n_cells;

	// correct battery voltage locally for load drop according to internal resistance and current
	எனில் (current_a > FLT_EPSILON) {
		updateInternalResistanceEstimation(voltage_v, current_a);

		எனில் (_params.r_internal >= 0.f) { // Use user specified internal resistance value
			cell_voltage += _params.r_internal * current_a;

		} இல்லையெனில் { // Use estimated internal resistance value
			cell_voltage += _internal_resistance_estimate * current_a;
		}

	}

	_cell_voltage_filter_v.update(cell_voltage);
	திரும்பு math::interpolate(_cell_voltage_filter_v.getState(), _params.v_empty, _params.v_charged, 0.f, 1.f);
}

வெறுமை Battery::updateInternalResistanceEstimation(மாறிலி தசம voltage_v, மாறிலி தசம current_a)
{
	Vector2f x{1, -current_a};
	_voltage_prediction = (x.transpose() * _RLS_est)(0, 0);
	_prediction_error = voltage_v - _voltage_prediction;
	மாறிலி Vector2f gamma = _estimation_covariance * x / (LAMBDA + (x.transpose() * _estimation_covariance * x)(0, 0));
	மாறிலி Vector2f RSL_est_temp = _RLS_est + gamma * _prediction_error;
	மாறிலி Matrix2f estimation_covariance_temp = (_estimation_covariance
			- Matrix<தசம, 2, 1>(gamma) * (x.transpose() * _estimation_covariance)) / LAMBDA;
	மாறிலி தசம estimation_covariance_temp_norm =
		sqrtf(powf(estimation_covariance_temp(0, 0), 2.f)
		      + 2.f * powf(estimation_covariance_temp(1, 0), 2.f)
		      + powf(estimation_covariance_temp(1, 1), 2.f));

	எனில் (estimation_covariance_temp_norm < _estimation_covariance_norm) { // Only update if estimation improves
		_RLS_est = RSL_est_temp;
		_estimation_covariance = estimation_covariance_temp;
		_estimation_covariance_norm = estimation_covariance_temp_norm;
		_internal_resistance_estimate =
			math::max(_RLS_est(1) / _params.n_cells, 0.f); // Only use positive values

	} இல்லையெனில் { // Update OCV estimate with IR estimate
		_RLS_est(0) = voltage_v + _RLS_est(1) * current_a;
	}

	_ocv_filter_v.update(voltage_v + _internal_resistance_estimate * _params.n_cells * current_a);
}

வெறுமை Battery::resetInternalResistanceEstimation(மாறிலி தசம voltage_v, மாறிலி தசம current_a)
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

	எனில் (_params.r_internal >= 0.f) { // Use user specified internal resistance value
		_cell_voltage_filter_v.reset(voltage_v / _params.n_cells + _params.r_internal * current_a);

	} இல்லையெனில் { // Use estimated internal resistance value
		_cell_voltage_filter_v.reset(voltage_v / _params.n_cells + _internal_resistance_estimate * current_a);
	}
}

வெறுமை Battery::estimateStateOfCharge()
{
	// choose which quantity we're using for final reporting
	எனில் ((_capacity_mah > 0.f) && _battery_initialized) {
		// if battery capacity is known, fuse voltage measurement with used capacity
		// The lower the voltage the more adjust the estimate with it to avoid deep discharge
		மாறிலி தசம weight_v = 3e-2f * (1 - _state_of_charge_volt_based);
		_state_of_charge = (1 - weight_v) * _state_of_charge + weight_v * _state_of_charge_volt_based;
		// directly apply current capacity slope calculated using current
		_state_of_charge -= _discharged_mah_loop / _capacity_mah;
		_state_of_charge = math::max(_state_of_charge, 0.f);

		மாறிலி தசம state_of_charge_current_based = math::max(1.f - _discharged_mah / _capacity_mah, 0.f);
		_state_of_charge = math::min(state_of_charge_current_based, _state_of_charge);

	} இல்லையெனில் {
		_state_of_charge = _state_of_charge_volt_based;
	}
}

uint8_t Battery::determineWarning(தசம state_of_charge)
{
	எனில் (state_of_charge < _params.emergen_thr) {
		திரும்பு battery_status_s::WARNING_EMERGENCY;

	} இல்லையெனில் எனில் (state_of_charge < _params.crit_thr) {
		திரும்பு battery_status_s::WARNING_CRITICAL;

	} இல்லையெனில் எனில் (state_of_charge < _params.கீழ்_thr) {
		திரும்பு battery_status_s::WARNING_கீழ்;

	} இல்லையெனில் {
		திரும்பு battery_status_s::WARNING_NONE;
	}
}

uint16_t Battery::determineFaults()
{
	uint16_t faults{0};

	எனில் ((_params.n_cells > 0)
	    && (_voltage_v > (_params.n_cells * _params.v_charged * 1.05f))) {
		// Reported as a "spike" since "over-voltage" does not exist in MAV_BATTERY_FAULT
		faults |= (1 << battery_status_s::FAULT_SPIKES);
	}

	எனில் (PX4_ISFINITE(_temperature_c) && _temperature_c > BAT_TEMP_MAX) {
		faults |= (1 << battery_status_s::FAULT_OVER_TEMPERATURE);
	}

	திரும்பு faults;
}

வெறுமை Battery::computeScale()
{
	மாறிலி தசம cell_voltage = _cell_voltage_filter_v.getState();

	எனில் (cell_voltage > FLT_EPSILON) {
		_scale = math::constrain(_params.v_charged / cell_voltage, 1.f, 1.3f); // Allow at most 30% compensation

	} இல்லையெனில் {
		_scale = 1.f;
	}
}

தசம Battery::computeRemainingTime(தசம current_a)
{
	தசம time_remaining_s = NAN;

	_flight_phase_estimation_sub.update();

	தருக்க reset_filter_transition_now = !_vehicle_status_was_fw && _vehicle_status_is_fw;

	// reset filter if not feasible, negative or we did a VTOL transition to FW mode
	எனில் (!PX4_ISFINITE(_current_average_filter_a.getState()) || _current_average_filter_a.getState() < FLT_EPSILON
	    || reset_filter_transition_now) {
		_current_average_filter_a.reset(_params.bat_avrg_current);
	}

	_vehicle_status_was_fw = _vehicle_status_is_fw;

	எனில் (_armed && PX4_ISFINITE(current_a)) {
		// For FW only update when we are in level flight
		எனில் (!_vehicle_status_is_fw || ((hrt_absolute_time() - _flight_phase_estimation_sub.get().timestamp) < 2_s
					       && _flight_phase_estimation_sub.get().flight_phase == flight_phase_estimation_s::FLIGHT_PHASE_LEVEL)) {
			எனில் (_dt > FLT_EPSILON) {
				_current_average_filter_a.update(fmaxf(current_a, 0.f), _dt);

			} இல்லையெனில் {
				_current_average_filter_a.update(fmaxf(current_a, 0.f));
			}
		}
	}

	// Remaining time estimation only possible with capacity
	எனில் (_capacity_mah > 0.f) {
		மாறிலி தசம remaining_capacity_mah = _state_of_charge * _capacity_mah;
		மாறிலி தசம current_ma = fmaxf(_current_average_filter_a.getState() * 1e3f, FLT_EPSILON);
		time_remaining_s = remaining_capacity_mah / current_ma * 3600.f;
	}

	திரும்பு time_remaining_s;
}

வெறுமை Battery::updateParams()
{
	மாறிலி முழு n_cells = _first_parameter_update ? 0 : _params.n_cells;
	param_get(_param_handles.v_empty, &_params.v_empty);
	param_get(_param_handles.v_charged, &_params.v_charged);
	param_get(_param_handles.n_cells, &_params.n_cells);
	param_get(_param_handles.r_internal, &_params.r_internal);
	param_get(_param_handles.source, &_params.source);
	param_get(_param_handles.கீழ்_thr, &_params.கீழ்_thr);
	param_get(_param_handles.crit_thr, &_params.crit_thr);
	param_get(_param_handles.emergen_thr, &_params.emergen_thr);
	param_get(_param_handles.bat_avrg_current, &_params.bat_avrg_current);
	param_get(_param_handles.i_overwrite, &_params.i_overwrite);

	தசம capacity{0.f};
	param_get(_param_handles.capacity, &capacity);
	setCapacityMah(capacity);

	எனில் (n_cells != _params.n_cells) {
		_internal_resistance_initialized = பொய்;
	}

	எனில் (!_internal_resistance_initialized && _params.n_cells > 0) {
		_RLS_est(0) = OCV_DEFAULT * _params.n_cells;
		_RLS_est(1) = R_DEFAULT * _params.n_cells;
		_estimation_covariance(0, 0) = OCV_COVARIANCE * _params.n_cells;
		_estimation_covariance(0, 1) = 0.f;
		_estimation_covariance(1, 0) = 0.f;
		_estimation_covariance(1, 1) = R_COVARIANCE * _params.n_cells;
		_estimation_covariance_norm = sqrtf(powf(_estimation_covariance(0, 0), 2.f) + 2.f * powf(_estimation_covariance(1, 0),
						    2.f) + powf(_estimation_covariance(1, 1), 2.f));
		_internal_resistance_initialized = உண்மை;
	}

	ModuleParams::updateParams();

	_first_parameter_update = பொய்;
}

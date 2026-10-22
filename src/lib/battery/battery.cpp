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

#ਅਨ੍ਤਰ੍ਭਾਵ "battery.h"
#ਅਨ੍ਤਰ੍ਭਾਵ <mathlib/mathlib.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <cstring>
#ਅਨ੍ਤਰ੍ਭਾਵ <px4_platform_common/defines.h>

ਵਰਤੋਂ ਨਾਮਾਕਾਸ਼ ਸਮਾਂ_literals;
ਵਰਤੋਂ ਨਾਮਾਕਾਸ਼ ਮੈਟ੍ਰਿਕਸ;

Battery::Battery(ਪੂਰਨ ਸੂਚਕਾਂਕ, ModuleParams *ਮਾਪਾ, ਅਚਲ ਪੂਰਨ sample_interval_us, ਅਚਲ uint8_t ਸਰੋਤ) :
	ModuleParams(ਮਾਪਾ),
	ਸੂਚਕਾਂਕ_2(ਸੂਚਕਾਂਕ < 1 || ਸੂਚਕਾਂਕ > 9 ? 1 : ਸੂਚਕਾਂਕ),
	ਸਰੋਤ_2(ਸਰੋਤ)
{
	ਅਚਲ ਦਸ਼ਮਲਵ expected_filter_dt = static_cast<ਦਸ਼ਮਲਵ>(sample_interval_us) * 1e-6f;
	current_ਔਸਤਨ_filter_a.ਸੈੱਟ_Parameters(expected_filter_dt, 50.f);
	_ocv_filter_v.ਸੈੱਟ_Parameters(expected_filter_dt, 1.f);
	_cell_voltage_filter_v.ਸੈੱਟ_Parameters(expected_filter_dt, 1.f);

	ਜੇ (ਸੂਚਕਾਂਕ > 9 || ਸੂਚਕਾਂਕ < 1) {
		PX4_ERR("Battery index must be between 1 and 9 (inclusive). Received %d. Defaulting to 1.", ਸੂਚਕਾਂਕ);
	}

	// 16 chars for parameter name + null terminator
	ਅੱਖਰ param_name[17];

	snprintf(param_name, ਆਕਾਰ(param_name), "BAT%d_V_EMPTY", ਸੂਚਕਾਂਕ_2);
	_param_handles.v_ਖਾਲੀ_ਥਾਵਾਂ = param_ਲੱਭੋ(param_name);

	ਜੇ (_param_handles.v_ਖਾਲੀ_ਥਾਵਾਂ == PARAM_INVALID) {
		PX4_ERR("Could not find parameter with name %s", param_name);
	}

	snprintf(param_name, ਆਕਾਰ(param_name), "BAT%d_V_CHARGED", ਸੂਚਕਾਂਕ_2);
	_param_handles.v_charged = param_ਲੱਭੋ(param_name);

	snprintf(param_name, ਆਕਾਰ(param_name), "BAT%d_N_CELLS", ਸੂਚਕਾਂਕ_2);
	_param_handles.n_cells = param_ਲੱਭੋ(param_name);

	snprintf(param_name, ਆਕਾਰ(param_name), "BAT%d_CAPACITY", ਸੂਚਕਾਂਕ_2);
	_param_handles.ਸਮਰੱਥਾ = param_ਲੱਭੋ(param_name);

	snprintf(param_name, ਆਕਾਰ(param_name), "BAT%d_R_INTERNAL", ਸੂਚਕਾਂਕ_2);
	_param_handles.r_ਅੰਦਰੂਨੀ = param_ਲੱਭੋ(param_name);

	snprintf(param_name, ਆਕਾਰ(param_name), "BAT%d_SOURCE", ਸੂਚਕਾਂਕ_2);
	_param_handles.ਸਰੋਤ = param_ਲੱਭੋ(param_name);

	snprintf(param_name, ਆਕਾਰ(param_name), "BAT%d_I_OVERWRITE", ਸੂਚਕਾਂਕ_2);
	_param_handles.i_overwrite = param_ਲੱਭੋ(param_name);

	_param_handles.ਨੀਵਾਂ_thr = param_ਲੱਭੋ("BAT_LOW_THR");
	_param_handles.crit_thr = param_ਲੱਭੋ("BAT_CRIT_THR");
	_param_handles.emergen_thr = param_ਲੱਭੋ("BAT_EMERGEN_THR");

	_param_handles.bat_avrg_current = param_ਲੱਭੋ("BAT_AVRG_CURRENT");

	updateParams();
}

ਖਾਲੀ Battery::updateVoltage(ਅਚਲ ਦਸ਼ਮਲਵ voltage_v)
{
	_voltage_v = voltage_v;
}

ਖਾਲੀ Battery::updateCurrent(ਅਚਲ ਦਸ਼ਮਲਵ current_a)
{
	// Overwrite the measured current if current overwrite is defined and vehicle is unarmed
	ਜੇ (!_armed && _params.i_overwrite > FLT_EPSILON) {
		_current_a = _params.i_overwrite;

	} ਨਹੀਂਤਾਂ {
		_current_a = current_a;
	}
}

ਖਾਲੀ Battery::updateTemperature(ਅਚਲ ਦਸ਼ਮਲਵ temperature_c)
{
	_temperature_c = temperature_c;
}

ਖਾਲੀ Battery::update_Battery_ਸਥਿਤੀ(ਅਚਲ hrt_abstime &timestamp)
{
	updateDt(timestamp);

	// Require minimum voltage otherwise override connected status
	ਜੇ (_voltage_v < LITHIUM_BATTERY_RECOGNITION_VOLTAGE) {
		_connected = ਝੂਠ;
	}

	ਜੇ (!_connected || (ਆਖਰੀ_unconnected_timestamp == 0)) {
		ਆਖਰੀ_unconnected_timestamp = timestamp;
	}

	// Wait with initializing filters to avoid relying on a voltage sample from the rising edge
	_battery_initialized = _connected && (timestamp > ਆਖਰੀ_unconnected_timestamp + 2_s);

	ਜੇ (_connected && !_battery_initialized && ਅੰਦਰੂਨੀ_resistance_initialized && _params.n_cells > 0) {
		reset_ਅੰਦਰੂਨੀ_Resistance_Estimation(_voltage_v, _current_a);
	}

	ਜੋੜ_Discharged(_current_a);
	state_ਦਾ_charge_volt_based =
		calculate_State_ਦਾ_Charge_Voltage_Based(_voltage_v, _current_a);

	ਜੇ (!external_state_ਦਾ_charge) {
		estimate_State_ਦਾ_Charge();
	}

	computeScale();

	ਜੇ (_connected && _battery_initialized) {
		_warning = determineWarning(state_ਦਾ_charge);
	}

	ਜੇ (vehicle_ਸਥਿਤੀ_sub.updated()) {
		vehicle_ਸਥਿਤੀ_s vehicle_ਸਥਿਤੀ;

		ਜੇ (vehicle_ਸਥਿਤੀ_sub.copy(&vehicle_ਸਥਿਤੀ)) {
			_armed = (vehicle_ਸਥਿਤੀ.arming_state == vehicle_ਸਥਿਤੀ_s::ARMING_STATE_ARMED);
			vehicle_ਸਥਿਤੀ_is_fw = (vehicle_ਸਥਿਤੀ.vehicle_type == vehicle_ਸਥਿਤੀ_s::VEHICLE_TYPE_FIXED_WING);
		}
	}
}

battery_ਸਥਿਤੀ_s Battery::get_Battery_ਸਥਿਤੀ()
{
	battery_ਸਥਿਤੀ_s battery_ਸਥਿਤੀ{};
	battery_ਸਥਿਤੀ.voltage_v = _voltage_v;
	battery_ਸਥਿਤੀ.current_a = _current_a;
	battery_ਸਥਿਤੀ.current_ਔਸਤਨ_a = current_ਔਸਤਨ_filter_a.getState();
	battery_ਸਥਿਤੀ.discharged_mah = _discharged_mah;
	battery_ਸਥਿਤੀ.ਬਚਿਆ = state_ਦਾ_charge;
	battery_ਸਥਿਤੀ.scale = _scale;
	battery_ਸਥਿਤੀ.ਸਮਾਂ_ਬਚਿਆ_s = compute_ਬਚਿਆ_ਸਮਾਂ(_current_a);
	battery_ਸਥਿਤੀ.temperature = _temperature_c;
	battery_ਸਥਿਤੀ.cell_ਗਿਣਤੀ = _params.n_cells;
	battery_ਸਥਿਤੀ.connected = _connected;
	battery_ਸਥਿਤੀ.ਸਰੋਤ = ਸਰੋਤ_2;
	battery_ਸਥਿਤੀ.ਤਰਜੀਹ = ਤਰਜੀਹ_2;
	battery_ਸਥਿਤੀ.ਸਮਰੱਥਾ = static_cast<uint16_t>(ਸਮਰੱਥਾ_mah);
	battery_ਸਥਿਤੀ.id = static_cast<uint8_t>(ਸੂਚਕਾਂਕ_2);
	battery_ਸਥਿਤੀ.warning = _warning;
	battery_ਸਥਿਤੀ.timestamp = hrt_absolute_ਸਮਾਂ();
	battery_ਸਥਿਤੀ.ਨੁਕਸ = determine_ਨੁਕਸ();
	battery_ਸਥਿਤੀ.ਬਚਿਆ_ਸਮਰੱਥਾ_wh = NAN; // not measured by dumb power module; smart-battery drivers overwrite
	battery_ਸਥਿਤੀ.nominal_voltage = NAN;        // not measured by dumb power module; smart-battery drivers overwrite
	battery_ਸਥਿਤੀ.ਅੰਦਰੂਨੀ_resistance_estimate = ਅੰਦਰੂਨੀ_resistance_estimate_2;
	battery_ਸਥਿਤੀ.ocv_estimate = _voltage_v + ਅੰਦਰੂਨੀ_resistance_estimate_2 * _params.n_cells * _current_a;
	battery_ਸਥਿਤੀ.ocv_estimate_filtered = _ocv_filter_v.getState();
	battery_ਸਥਿਤੀ.volt_based_soc_estimate = _params.n_cells > 0 ? math::interpolate(_ocv_filter_v.getState() /
			_params.n_cells,
			_params.v_ਖਾਲੀ_ਥਾਵਾਂ, _params.v_charged, 0.f, 1.f) : -1.f;
	battery_ਸਥਿਤੀ.voltage_prediction = _voltage_prediction;
	battery_ਸਥਿਤੀ.prediction_error = _prediction_error;
	battery_ਸਥਿਤੀ.estimation_covariance_norm = _estimation_covariance_norm;
	ਵਾਪਸ battery_ਸਥਿਤੀ;
}

ਖਾਲੀ Battery::publish_Battery_ਸਥਿਤੀ(ਅਚਲ battery_ਸਥਿਤੀ_s &battery_ਸਥਿਤੀ)
{
	ਜੇ (ਸਰੋਤ_2 == _params.ਸਰੋਤ) {
		battery_ਸਥਿਤੀ_pub.publish(battery_ਸਥਿਤੀ);
	}
}

ਖਾਲੀ Battery::update_And_Publish_Battery_ਸਥਿਤੀ(ਅਚਲ hrt_abstime &timestamp)
{
	update_Battery_ਸਥਿਤੀ(timestamp);

	battery_ਸਥਿਤੀ_s battery_ਸਥਿਤੀ = get_Battery_ਸਥਿਤੀ();

	_failure_config.update();

	ਜੇ (!failure_injection::process_battery(_failure_config, battery_ਸਥਿਤੀ.id, battery_ਸਥਿਤੀ)) {
		ਵਾਪਸ;
	}

	publish_Battery_ਸਥਿਤੀ(battery_ਸਥਿਤੀ);
}
ਖਾਲੀ Battery::updateDt(ਅਚਲ hrt_abstime &timestamp)
{
	ਜੇ (ਆਖਰੀ_timestamp != 0) {
		// _dt_discharge is the true, unclamped time delta: for the coulomb
		// count in sumDischarged() below, using the real elapsed time is
		// always more accurate than clamping it, even across an unusually
		// long gap between updates (e.g. an update source that reports
		// less often than every 2 seconds).
		_dt_discharge = (timestamp - ਆਖਰੀ_timestamp) / 1e6f;

		// _dt is clamped to guard the numerical stability of the current
		// average filter below, which assumes a roughly steady sample
		// interval and should not see a single very large dt.
		_dt = math::min(_dt_discharge, 2.f);
	}

	ਆਖਰੀ_timestamp = timestamp;
}

ਦਸ਼ਮਲਵ Battery::ਜੋੜ_Discharged(ਦਸ਼ਮਲਵ current_a)
{
	ਜੇ (_dt_discharge > FLT_EPSILON && fabsf(current_a + 1.f) > FLT_EPSILON) {
		// mAh since last loop: (current[A] * 1000 = [mA]) * (dt[s] / 3600 = [h])
		// current = -1 means invalid current measurement
		_discharged_mah_loop = (current_a * 1e3f) * (_dt_discharge / 3600.f);
		_discharged_mah += _discharged_mah_loop;
	}

	ਵਾਪਸ _discharged_mah;
}

ਦਸ਼ਮਲਵ Battery::calculate_State_ਦਾ_Charge_Voltage_Based(ਅਚਲ ਦਸ਼ਮਲਵ voltage_v, ਅਚਲ ਦਸ਼ਮਲਵ current_a)
{
	ਜੇ (_params.n_cells == 0) {
		ਵਾਪਸ -1.0f;
	}

	// remaining battery capacity based on voltage
	ਦਸ਼ਮਲਵ cell_voltage = voltage_v / _params.n_cells;

	// correct battery voltage locally for load drop according to internal resistance and current
	ਜੇ (current_a > FLT_EPSILON) {
		update_ਅੰਦਰੂਨੀ_Resistance_Estimation(voltage_v, current_a);

		ਜੇ (_params.r_ਅੰਦਰੂਨੀ >= 0.f) { // Use user specified internal resistance value
			cell_voltage += _params.r_ਅੰਦਰੂਨੀ * current_a;

		} ਨਹੀਂਤਾਂ { // Use estimated internal resistance value
			cell_voltage += ਅੰਦਰੂਨੀ_resistance_estimate_2 * current_a;
		}

	}

	_cell_voltage_filter_v.update(cell_voltage);
	ਵਾਪਸ math::interpolate(_cell_voltage_filter_v.getState(), _params.v_ਖਾਲੀ_ਥਾਵਾਂ, _params.v_charged, 0.f, 1.f);
}

ਖਾਲੀ Battery::update_ਅੰਦਰੂਨੀ_Resistance_Estimation(ਅਚਲ ਦਸ਼ਮਲਵ voltage_v, ਅਚਲ ਦਸ਼ਮਲਵ current_a)
{
	Vector2f x{1, -current_a};
	_voltage_prediction = (x.transpose() * _RLS_est)(0, 0);
	_prediction_error = voltage_v - _voltage_prediction;
	ਅਚਲ Vector2f gamma = _estimation_covariance * x / (LAMBDA + (x.transpose() * _estimation_covariance * x)(0, 0));
	ਅਚਲ Vector2f RSL_est_ਆਰਜ਼ੀ = _RLS_est + gamma * _prediction_error;
	ਅਚਲ Matrix2f estimation_covariance_ਆਰਜ਼ੀ = (_estimation_covariance
			- ਮੈਟ੍ਰਿਕਸ_2<ਦਸ਼ਮਲਵ, 2, 1>(gamma) * (x.transpose() * _estimation_covariance)) / LAMBDA;
	ਅਚਲ ਦਸ਼ਮਲਵ estimation_covariance_ਆਰਜ਼ੀ_norm =
		sqrtf(powf(estimation_covariance_ਆਰਜ਼ੀ(0, 0), 2.f)
		      + 2.f * powf(estimation_covariance_ਆਰਜ਼ੀ(1, 0), 2.f)
		      + powf(estimation_covariance_ਆਰਜ਼ੀ(1, 1), 2.f));

	ਜੇ (estimation_covariance_ਆਰਜ਼ੀ_norm < _estimation_covariance_norm) { // Only update if estimation improves
		_RLS_est = RSL_est_ਆਰਜ਼ੀ;
		_estimation_covariance = estimation_covariance_ਆਰਜ਼ੀ;
		_estimation_covariance_norm = estimation_covariance_ਆਰਜ਼ੀ_norm;
		ਅੰਦਰੂਨੀ_resistance_estimate_2 =
			math::max(_RLS_est(1) / _params.n_cells, 0.f); // Only use positive values

	} ਨਹੀਂਤਾਂ { // Update OCV estimate with IR estimate
		_RLS_est(0) = voltage_v + _RLS_est(1) * current_a;
	}

	_ocv_filter_v.update(voltage_v + ਅੰਦਰੂਨੀ_resistance_estimate_2 * _params.n_cells * current_a);
}

ਖਾਲੀ Battery::reset_ਅੰਦਰੂਨੀ_Resistance_Estimation(ਅਚਲ ਦਸ਼ਮਲਵ voltage_v, ਅਚਲ ਦਸ਼ਮਲਵ current_a)
{
	_RLS_est(0) = voltage_v;
	_RLS_est(1) = R_DEFAULT * _params.n_cells;
	_estimation_covariance.ਸੈੱਟ_Zero();
	_estimation_covariance(0, 0) = OCV_COVARIANCE * _params.n_cells;
	_estimation_covariance(1, 1) = R_COVARIANCE * _params.n_cells;
	_estimation_covariance_norm = sqrtf(powf(_estimation_covariance(0, 0), 2.f) + 2.f * powf(_estimation_covariance(1, 0),
					    2.f) + powf(_estimation_covariance(1, 1), 2.f));
	ਅੰਦਰੂਨੀ_resistance_estimate_2 = R_DEFAULT;
	_ocv_filter_v.reset(voltage_v + ਅੰਦਰੂਨੀ_resistance_estimate_2 * _params.n_cells * current_a);

	ਜੇ (_params.r_ਅੰਦਰੂਨੀ >= 0.f) { // Use user specified internal resistance value
		_cell_voltage_filter_v.reset(voltage_v / _params.n_cells + _params.r_ਅੰਦਰੂਨੀ * current_a);

	} ਨਹੀਂਤਾਂ { // Use estimated internal resistance value
		_cell_voltage_filter_v.reset(voltage_v / _params.n_cells + ਅੰਦਰੂਨੀ_resistance_estimate_2 * current_a);
	}
}

ਖਾਲੀ Battery::estimate_State_ਦਾ_Charge()
{
	// choose which quantity we're using for final reporting
	ਜੇ ((ਸਮਰੱਥਾ_mah > 0.f) && _battery_initialized) {
		// if battery capacity is known, fuse voltage measurement with used capacity
		// The lower the voltage the more adjust the estimate with it to avoid deep discharge
		ਅਚਲ ਦਸ਼ਮਲਵ ਭਾਰ_v = 3e-2f * (1 - state_ਦਾ_charge_volt_based);
		state_ਦਾ_charge = (1 - ਭਾਰ_v) * state_ਦਾ_charge + ਭਾਰ_v * state_ਦਾ_charge_volt_based;
		// directly apply current capacity slope calculated using current
		state_ਦਾ_charge -= _discharged_mah_loop / ਸਮਰੱਥਾ_mah;
		state_ਦਾ_charge = math::max(state_ਦਾ_charge, 0.f);

		ਅਚਲ ਦਸ਼ਮਲਵ state_ਦਾ_charge_current_based = math::max(1.f - _discharged_mah / ਸਮਰੱਥਾ_mah, 0.f);
		state_ਦਾ_charge = math::min(state_ਦਾ_charge_current_based, state_ਦਾ_charge);

	} ਨਹੀਂਤਾਂ {
		state_ਦਾ_charge = state_ਦਾ_charge_volt_based;
	}
}

uint8_t Battery::determineWarning(ਦਸ਼ਮਲਵ state_ਦਾ_charge_2)
{
	ਜੇ (state_ਦਾ_charge_2 < _params.emergen_thr) {
		ਵਾਪਸ battery_ਸਥਿਤੀ_s::WARNING_EMERGENCY;

	} ਨਹੀਂਤਾਂ ਜੇ (state_ਦਾ_charge_2 < _params.crit_thr) {
		ਵਾਪਸ battery_ਸਥਿਤੀ_s::WARNING_CRITICAL;

	} ਨਹੀਂਤਾਂ ਜੇ (state_ਦਾ_charge_2 < _params.ਨੀਵਾਂ_thr) {
		ਵਾਪਸ battery_ਸਥਿਤੀ_s::WARNING_ਨੀਵਾਂ;

	} ਨਹੀਂਤਾਂ {
		ਵਾਪਸ battery_ਸਥਿਤੀ_s::WARNING_NONE;
	}
}

uint16_t Battery::determine_ਨੁਕਸ()
{
	uint16_t ਨੁਕਸ{0};

	ਜੇ ((_params.n_cells > 0)
	    && (_voltage_v > (_params.n_cells * _params.v_charged * 1.05f))) {
		// Reported as a "spike" since "over-voltage" does not exist in MAV_BATTERY_FAULT
		ਨੁਕਸ |= (1 << battery_ਸਥਿਤੀ_s::FAULT_SPIKES);
	}

	ਜੇ (PX4_ISFINITE(_temperature_c) && _temperature_c > BAT_ਆਰਜ਼ੀ_MAX) {
		ਨੁਕਸ |= (1 << battery_ਸਥਿਤੀ_s::FAULT_OVER_TEMPERATURE);
	}

	ਵਾਪਸ ਨੁਕਸ;
}

ਖਾਲੀ Battery::computeScale()
{
	ਅਚਲ ਦਸ਼ਮਲਵ cell_voltage = _cell_voltage_filter_v.getState();

	ਜੇ (cell_voltage > FLT_EPSILON) {
		_scale = math::constrain(_params.v_charged / cell_voltage, 1.f, 1.3f); // Allow at most 30% compensation

	} ਨਹੀਂਤਾਂ {
		_scale = 1.f;
	}
}

ਦਸ਼ਮਲਵ Battery::compute_ਬਚਿਆ_ਸਮਾਂ(ਦਸ਼ਮਲਵ current_a)
{
	ਦਸ਼ਮਲਵ ਸਮਾਂ_ਬਚਿਆ_s = NAN;

	_flight_phase_estimation_sub.update();

	ਤਾਰਕਿਕ reset_filter_transition_now = !vehicle_ਸਥਿਤੀ_was_fw && vehicle_ਸਥਿਤੀ_is_fw;

	// reset filter if not feasible, negative or we did a VTOL transition to FW mode
	ਜੇ (!PX4_ISFINITE(current_ਔਸਤਨ_filter_a.getState()) || current_ਔਸਤਨ_filter_a.getState() < FLT_EPSILON
	    || reset_filter_transition_now) {
		current_ਔਸਤਨ_filter_a.reset(_params.bat_avrg_current);
	}

	vehicle_ਸਥਿਤੀ_was_fw = vehicle_ਸਥਿਤੀ_is_fw;

	ਜੇ (_armed && PX4_ISFINITE(current_a)) {
		// For FW only update when we are in level flight
		ਜੇ (!vehicle_ਸਥਿਤੀ_is_fw || ((hrt_absolute_ਸਮਾਂ() - _flight_phase_estimation_sub.get().timestamp) < 2_s
					       && _flight_phase_estimation_sub.get().flight_phase == flight_phase_estimation_s::FLIGHT_PHASE_LEVEL)) {
			ਜੇ (_dt > FLT_EPSILON) {
				current_ਔਸਤਨ_filter_a.update(fmaxf(current_a, 0.f), _dt);

			} ਨਹੀਂਤਾਂ {
				current_ਔਸਤਨ_filter_a.update(fmaxf(current_a, 0.f));
			}
		}
	}

	// Remaining time estimation only possible with capacity
	ਜੇ (ਸਮਰੱਥਾ_mah > 0.f) {
		ਅਚਲ ਦਸ਼ਮਲਵ ਬਚਿਆ_ਸਮਰੱਥਾ_mah = state_ਦਾ_charge * ਸਮਰੱਥਾ_mah;
		ਅਚਲ ਦਸ਼ਮਲਵ current_ma = fmaxf(current_ਔਸਤਨ_filter_a.getState() * 1e3f, FLT_EPSILON);
		ਸਮਾਂ_ਬਚਿਆ_s = ਬਚਿਆ_ਸਮਰੱਥਾ_mah / current_ma * 3600.f;
	}

	ਵਾਪਸ ਸਮਾਂ_ਬਚਿਆ_s;
}

ਖਾਲੀ Battery::updateParams()
{
	ਅਚਲ ਪੂਰਨ n_cells = ਪਹਿਲਾ_parameter_update ? 0 : _params.n_cells;
	param_get(_param_handles.v_ਖਾਲੀ_ਥਾਵਾਂ, &_params.v_ਖਾਲੀ_ਥਾਵਾਂ);
	param_get(_param_handles.v_charged, &_params.v_charged);
	param_get(_param_handles.n_cells, &_params.n_cells);
	param_get(_param_handles.r_ਅੰਦਰੂਨੀ, &_params.r_ਅੰਦਰੂਨੀ);
	param_get(_param_handles.ਸਰੋਤ, &_params.ਸਰੋਤ);
	param_get(_param_handles.ਨੀਵਾਂ_thr, &_params.ਨੀਵਾਂ_thr);
	param_get(_param_handles.crit_thr, &_params.crit_thr);
	param_get(_param_handles.emergen_thr, &_params.emergen_thr);
	param_get(_param_handles.bat_avrg_current, &_params.bat_avrg_current);
	param_get(_param_handles.i_overwrite, &_params.i_overwrite);

	ਦਸ਼ਮਲਵ ਸਮਰੱਥਾ{0.f};
	param_get(_param_handles.ਸਮਰੱਥਾ, &ਸਮਰੱਥਾ);
	ਸੈੱਟ_ਸਮਰੱਥਾ_Mah(ਸਮਰੱਥਾ);

	ਜੇ (n_cells != _params.n_cells) {
		ਅੰਦਰੂਨੀ_resistance_initialized = ਝੂਠ;
	}

	ਜੇ (!ਅੰਦਰੂਨੀ_resistance_initialized && _params.n_cells > 0) {
		_RLS_est(0) = OCV_DEFAULT * _params.n_cells;
		_RLS_est(1) = R_DEFAULT * _params.n_cells;
		_estimation_covariance(0, 0) = OCV_COVARIANCE * _params.n_cells;
		_estimation_covariance(0, 1) = 0.f;
		_estimation_covariance(1, 0) = 0.f;
		_estimation_covariance(1, 1) = R_COVARIANCE * _params.n_cells;
		_estimation_covariance_norm = sqrtf(powf(_estimation_covariance(0, 0), 2.f) + 2.f * powf(_estimation_covariance(1, 0),
						    2.f) + powf(_estimation_covariance(1, 1), 2.f));
		ਅੰਦਰੂਨੀ_resistance_initialized = ਸੱਚ;
	}

	ModuleParams::updateParams();

	ਪਹਿਲਾ_parameter_update = ਝੂਠ;
}

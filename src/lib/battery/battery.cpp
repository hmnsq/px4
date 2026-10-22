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

#ಅನ್ತರ್ಭಾವ "battery.h"
#ಅನ್ತರ್ಭಾವ <mathlib/mathlib.h>
#ಅನ್ತರ್ಭಾವ <cstring>
#ಅನ್ತರ್ಭಾವ <px4_platform_common/defines.h>

ಬಳಸಿ ನಾಮಾಕಾಶ ಸಮಯ_literals;
ಬಳಸಿ ನಾಮಾಕಾಶ ಮಾತೃಕೆ;

Battery::Battery(ಪೂರ್ಣಾಂಕ ಸೂಚ್ಯಂಕ, ModuleParams *ಪೋಷಕ, ಸ್ಥಿರಾಂಕ ಪೂರ್ಣಾಂಕ sample_interval_us, ಸ್ಥಿರಾಂಕ uint8_t ಮೂಲ) :
	ModuleParams(ಪೋಷಕ),
	ಸೂಚ್ಯಂಕ_2(ಸೂಚ್ಯಂಕ < 1 || ಸೂಚ್ಯಂಕ > 9 ? 1 : ಸೂಚ್ಯಂಕ),
	ಮೂಲ_2(ಮೂಲ)
{
	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ expected_filter_dt = static_cast<ದಶಮಾಂಶ>(sample_interval_us) * 1e-6f;
	current_ಸರಾಸರಿ_ಮೌಲ್ಯ_filter_a.ಗಣ_Parameters(expected_filter_dt, 50.f);
	_ocv_filter_v.ಗಣ_Parameters(expected_filter_dt, 1.f);
	_cell_voltage_filter_v.ಗಣ_Parameters(expected_filter_dt, 1.f);

	ಆದರೆ (ಸೂಚ್ಯಂಕ > 9 || ಸೂಚ್ಯಂಕ < 1) {
		PX4_ERR("Battery index must be between 1 and 9 (inclusive). Received %d. Defaulting to 1.", ಸೂಚ್ಯಂಕ);
	}

	// 16 chars for parameter name + null terminator
	ಅಕ್ಷರ param_name[17];

	snprintf(param_name, ಗಾತ್ರ(param_name), "BAT%d_V_EMPTY", ಸೂಚ್ಯಂಕ_2);
	_param_handles.v_ಖಾಲಿಗಳು = param_ಕಂಡುಹಿಡಿ(param_name);

	ಆದರೆ (_param_handles.v_ಖಾಲಿಗಳು == PARAM_INVALID) {
		PX4_ERR("Could not find parameter with name %s", param_name);
	}

	snprintf(param_name, ಗಾತ್ರ(param_name), "BAT%d_V_CHARGED", ಸೂಚ್ಯಂಕ_2);
	_param_handles.v_charged = param_ಕಂಡುಹಿಡಿ(param_name);

	snprintf(param_name, ಗಾತ್ರ(param_name), "BAT%d_N_CELLS", ಸೂಚ್ಯಂಕ_2);
	_param_handles.n_cells = param_ಕಂಡುಹಿಡಿ(param_name);

	snprintf(param_name, ಗಾತ್ರ(param_name), "BAT%d_CAPACITY", ಸೂಚ್ಯಂಕ_2);
	_param_handles.ಸಾಮರ್ಥ್ಯ = param_ಕಂಡುಹಿಡಿ(param_name);

	snprintf(param_name, ಗಾತ್ರ(param_name), "BAT%d_R_INTERNAL", ಸೂಚ್ಯಂಕ_2);
	_param_handles.r_ಆಂತರಿಕ = param_ಕಂಡುಹಿಡಿ(param_name);

	snprintf(param_name, ಗಾತ್ರ(param_name), "BAT%d_SOURCE", ಸೂಚ್ಯಂಕ_2);
	_param_handles.ಮೂಲ = param_ಕಂಡುಹಿಡಿ(param_name);

	snprintf(param_name, ಗಾತ್ರ(param_name), "BAT%d_I_OVERWRITE", ಸೂಚ್ಯಂಕ_2);
	_param_handles.i_overwrite = param_ಕಂಡುಹಿಡಿ(param_name);

	_param_handles.ಕೆಳಗಿನ_thr = param_ಕಂಡುಹಿಡಿ("BAT_LOW_THR");
	_param_handles.crit_thr = param_ಕಂಡುಹಿಡಿ("BAT_CRIT_THR");
	_param_handles.emergen_thr = param_ಕಂಡುಹಿಡಿ("BAT_EMERGEN_THR");

	_param_handles.bat_avrg_current = param_ಕಂಡುಹಿಡಿ("BAT_AVRG_CURRENT");

	updateParams();
}

ಶೂನ್ಯ Battery::updateVoltage(ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ voltage_v)
{
	_voltage_v = voltage_v;
}

ಶೂನ್ಯ Battery::updateCurrent(ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ current_a)
{
	// Overwrite the measured current if current overwrite is defined and vehicle is unarmed
	ಆದರೆ (!_armed && _params.i_overwrite > FLT_EPSILON) {
		_current_a = _params.i_overwrite;

	} ಇಲ್ಲದಿದ್ದರೆ {
		_current_a = current_a;
	}
}

ಶೂನ್ಯ Battery::updateTemperature(ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ temperature_c)
{
	_temperature_c = temperature_c;
}

ಶೂನ್ಯ Battery::update_Battery_ಸ್ಥಿತಿ(ಸ್ಥಿರಾಂಕ hrt_abstime &timestamp)
{
	updateDt(timestamp);

	// Require minimum voltage otherwise override connected status
	ಆದರೆ (_voltage_v < LITHIUM_BATTERY_RECOGNITION_VOLTAGE) {
		_connected = ಸುಳ್ಳು;
	}

	ಆದರೆ (!_connected || (ಕೊನೆಯ_unconnected_timestamp == 0)) {
		ಕೊನೆಯ_unconnected_timestamp = timestamp;
	}

	// Wait with initializing filters to avoid relying on a voltage sample from the rising edge
	_battery_initialized = _connected && (timestamp > ಕೊನೆಯ_unconnected_timestamp + 2_s);

	ಆದರೆ (_connected && !_battery_initialized && ಆಂತರಿಕ_resistance_initialized && _params.n_cells > 0) {
		reset_ಆಂತರಿಕ_Resistance_Estimation(_voltage_v, _current_a);
	}

	ಮೊತ್ತ_Discharged(_current_a);
	state_ದ_charge_volt_based =
		calculate_State_ದ_Charge_Voltage_Based(_voltage_v, _current_a);

	ಆದರೆ (!external_state_ದ_charge) {
		estimate_State_ದ_Charge();
	}

	computeScale();

	ಆದರೆ (_connected && _battery_initialized) {
		_warning = determineWarning(state_ದ_charge);
	}

	ಆದರೆ (vehicle_ಸ್ಥಿತಿ_sub.updated()) {
		vehicle_ಸ್ಥಿತಿ_s vehicle_ಸ್ಥಿತಿ;

		ಆದರೆ (vehicle_ಸ್ಥಿತಿ_sub.copy(&vehicle_ಸ್ಥಿತಿ)) {
			_armed = (vehicle_ಸ್ಥಿತಿ.arming_state == vehicle_ಸ್ಥಿತಿ_s::ARMING_STATE_ARMED);
			vehicle_ಸ್ಥಿತಿ_is_fw = (vehicle_ಸ್ಥಿತಿ.vehicle_type == vehicle_ಸ್ಥಿತಿ_s::VEHICLE_TYPE_FIXED_WING);
		}
	}
}

battery_ಸ್ಥಿತಿ_s Battery::get_Battery_ಸ್ಥಿತಿ()
{
	battery_ಸ್ಥಿತಿ_s battery_ಸ್ಥಿತಿ{};
	battery_ಸ್ಥಿತಿ.voltage_v = _voltage_v;
	battery_ಸ್ಥಿತಿ.current_a = _current_a;
	battery_ಸ್ಥಿತಿ.current_ಸರಾಸರಿ_ಮೌಲ್ಯ_a = current_ಸರಾಸರಿ_ಮೌಲ್ಯ_filter_a.getState();
	battery_ಸ್ಥಿತಿ.discharged_mah = _discharged_mah;
	battery_ಸ್ಥಿತಿ.ಉಳಿದ = state_ದ_charge;
	battery_ಸ್ಥಿತಿ.scale = _scale;
	battery_ಸ್ಥಿತಿ.ಸಮಯ_ಉಳಿದ_s = compute_ಉಳಿದ_ಸಮಯ(_current_a);
	battery_ಸ್ಥಿತಿ.temperature = _temperature_c;
	battery_ಸ್ಥಿತಿ.cell_ಎಣಿಕೆ = _params.n_cells;
	battery_ಸ್ಥಿತಿ.connected = _connected;
	battery_ಸ್ಥಿತಿ.ಮೂಲ = ಮೂಲ_2;
	battery_ಸ್ಥಿತಿ.ಆದ್ಯತೆ = ಆದ್ಯತೆ_2;
	battery_ಸ್ಥಿತಿ.ಸಾಮರ್ಥ್ಯ = static_cast<uint16_t>(ಸಾಮರ್ಥ್ಯ_mah);
	battery_ಸ್ಥಿತಿ.id = static_cast<uint8_t>(ಸೂಚ್ಯಂಕ_2);
	battery_ಸ್ಥಿತಿ.warning = _warning;
	battery_ಸ್ಥಿತಿ.timestamp = hrt_absolute_ಸಮಯ();
	battery_ಸ್ಥಿತಿ.ದೋಷಗಳು = determine_ದೋಷಗಳು();
	battery_ಸ್ಥಿತಿ.ಉಳಿದ_ಸಾಮರ್ಥ್ಯ_wh = NAN; // not measured by dumb power module; smart-battery drivers overwrite
	battery_ಸ್ಥಿತಿ.nominal_voltage = NAN;        // not measured by dumb power module; smart-battery drivers overwrite
	battery_ಸ್ಥಿತಿ.ಆಂತರಿಕ_resistance_estimate = ಆಂತರಿಕ_resistance_estimate_2;
	battery_ಸ್ಥಿತಿ.ocv_estimate = _voltage_v + ಆಂತರಿಕ_resistance_estimate_2 * _params.n_cells * _current_a;
	battery_ಸ್ಥಿತಿ.ocv_estimate_filtered = _ocv_filter_v.getState();
	battery_ಸ್ಥಿತಿ.volt_based_soc_estimate = _params.n_cells > 0 ? math::interpolate(_ocv_filter_v.getState() /
			_params.n_cells,
			_params.v_ಖಾಲಿಗಳು, _params.v_charged, 0.f, 1.f) : -1.f;
	battery_ಸ್ಥಿತಿ.voltage_prediction = _voltage_prediction;
	battery_ಸ್ಥಿತಿ.prediction_error = _prediction_error;
	battery_ಸ್ಥಿತಿ.estimation_covariance_norm = _estimation_covariance_norm;
	ಹಿಂತಿರುಗಿ battery_ಸ್ಥಿತಿ;
}

ಶೂನ್ಯ Battery::publish_Battery_ಸ್ಥಿತಿ(ಸ್ಥಿರಾಂಕ battery_ಸ್ಥಿತಿ_s &battery_ಸ್ಥಿತಿ)
{
	ಆದರೆ (ಮೂಲ_2 == _params.ಮೂಲ) {
		battery_ಸ್ಥಿತಿ_pub.publish(battery_ಸ್ಥಿತಿ);
	}
}

ಶೂನ್ಯ Battery::update_And_Publish_Battery_ಸ್ಥಿತಿ(ಸ್ಥಿರಾಂಕ hrt_abstime &timestamp)
{
	update_Battery_ಸ್ಥಿತಿ(timestamp);

	battery_ಸ್ಥಿತಿ_s battery_ಸ್ಥಿತಿ = get_Battery_ಸ್ಥಿತಿ();

	_failure_config.update();

	ಆದರೆ (!failure_injection::process_battery(_failure_config, battery_ಸ್ಥಿತಿ.id, battery_ಸ್ಥಿತಿ)) {
		ಹಿಂತಿರುಗಿ;
	}

	publish_Battery_ಸ್ಥಿತಿ(battery_ಸ್ಥಿತಿ);
}
ಶೂನ್ಯ Battery::updateDt(ಸ್ಥಿರಾಂಕ hrt_abstime &timestamp)
{
	ಆದರೆ (ಕೊನೆಯ_timestamp != 0) {
		// _dt_discharge is the true, unclamped time delta: for the coulomb
		// count in sumDischarged() below, using the real elapsed time is
		// always more accurate than clamping it, even across an unusually
		// long gap between updates (e.g. an update source that reports
		// less often than every 2 seconds).
		_dt_discharge = (timestamp - ಕೊನೆಯ_timestamp) / 1e6f;

		// _dt is clamped to guard the numerical stability of the current
		// average filter below, which assumes a roughly steady sample
		// interval and should not see a single very large dt.
		_dt = math::min(_dt_discharge, 2.f);
	}

	ಕೊನೆಯ_timestamp = timestamp;
}

ದಶಮಾಂಶ Battery::ಮೊತ್ತ_Discharged(ದಶಮಾಂಶ current_a)
{
	ಆದರೆ (_dt_discharge > FLT_EPSILON && fabsf(current_a + 1.f) > FLT_EPSILON) {
		// mAh since last loop: (current[A] * 1000 = [mA]) * (dt[s] / 3600 = [h])
		// current = -1 means invalid current measurement
		_discharged_mah_loop = (current_a * 1e3f) * (_dt_discharge / 3600.f);
		_discharged_mah += _discharged_mah_loop;
	}

	ಹಿಂತಿರುಗಿ _discharged_mah;
}

ದಶಮಾಂಶ Battery::calculate_State_ದ_Charge_Voltage_Based(ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ voltage_v, ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ current_a)
{
	ಆದರೆ (_params.n_cells == 0) {
		ಹಿಂತಿರುಗಿ -1.0f;
	}

	// remaining battery capacity based on voltage
	ದಶಮಾಂಶ cell_voltage = voltage_v / _params.n_cells;

	// correct battery voltage locally for load drop according to internal resistance and current
	ಆದರೆ (current_a > FLT_EPSILON) {
		update_ಆಂತರಿಕ_Resistance_Estimation(voltage_v, current_a);

		ಆದರೆ (_params.r_ಆಂತರಿಕ >= 0.f) { // Use user specified internal resistance value
			cell_voltage += _params.r_ಆಂತರಿಕ * current_a;

		} ಇಲ್ಲದಿದ್ದರೆ { // Use estimated internal resistance value
			cell_voltage += ಆಂತರಿಕ_resistance_estimate_2 * current_a;
		}

	}

	_cell_voltage_filter_v.update(cell_voltage);
	ಹಿಂತಿರುಗಿ math::interpolate(_cell_voltage_filter_v.getState(), _params.v_ಖಾಲಿಗಳು, _params.v_charged, 0.f, 1.f);
}

ಶೂನ್ಯ Battery::update_ಆಂತರಿಕ_Resistance_Estimation(ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ voltage_v, ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ current_a)
{
	Vector2f x{1, -current_a};
	_voltage_prediction = (x.transpose() * _RLS_est)(0, 0);
	_prediction_error = voltage_v - _voltage_prediction;
	ಸ್ಥಿರಾಂಕ Vector2f gamma = _estimation_covariance * x / (LAMBDA + (x.transpose() * _estimation_covariance * x)(0, 0));
	ಸ್ಥಿರಾಂಕ Vector2f RSL_est_ತಾತ್ಕಾಲಿಕ = _RLS_est + gamma * _prediction_error;
	ಸ್ಥಿರಾಂಕ Matrix2f estimation_covariance_ತಾತ್ಕಾಲಿಕ = (_estimation_covariance
			- ಮಾತೃಕೆ_2<ದಶಮಾಂಶ, 2, 1>(gamma) * (x.transpose() * _estimation_covariance)) / LAMBDA;
	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ estimation_covariance_ತಾತ್ಕಾಲಿಕ_norm =
		sqrtf(powf(estimation_covariance_ತಾತ್ಕಾಲಿಕ(0, 0), 2.f)
		      + 2.f * powf(estimation_covariance_ತಾತ್ಕಾಲಿಕ(1, 0), 2.f)
		      + powf(estimation_covariance_ತಾತ್ಕಾಲಿಕ(1, 1), 2.f));

	ಆದರೆ (estimation_covariance_ತಾತ್ಕಾಲಿಕ_norm < _estimation_covariance_norm) { // Only update if estimation improves
		_RLS_est = RSL_est_ತಾತ್ಕಾಲಿಕ;
		_estimation_covariance = estimation_covariance_ತಾತ್ಕಾಲಿಕ;
		_estimation_covariance_norm = estimation_covariance_ತಾತ್ಕಾಲಿಕ_norm;
		ಆಂತರಿಕ_resistance_estimate_2 =
			math::max(_RLS_est(1) / _params.n_cells, 0.f); // Only use positive values

	} ಇಲ್ಲದಿದ್ದರೆ { // Update OCV estimate with IR estimate
		_RLS_est(0) = voltage_v + _RLS_est(1) * current_a;
	}

	_ocv_filter_v.update(voltage_v + ಆಂತರಿಕ_resistance_estimate_2 * _params.n_cells * current_a);
}

ಶೂನ್ಯ Battery::reset_ಆಂತರಿಕ_Resistance_Estimation(ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ voltage_v, ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ current_a)
{
	_RLS_est(0) = voltage_v;
	_RLS_est(1) = R_DEFAULT * _params.n_cells;
	_estimation_covariance.ಗಣ_Zero();
	_estimation_covariance(0, 0) = OCV_COVARIANCE * _params.n_cells;
	_estimation_covariance(1, 1) = R_COVARIANCE * _params.n_cells;
	_estimation_covariance_norm = sqrtf(powf(_estimation_covariance(0, 0), 2.f) + 2.f * powf(_estimation_covariance(1, 0),
					    2.f) + powf(_estimation_covariance(1, 1), 2.f));
	ಆಂತರಿಕ_resistance_estimate_2 = R_DEFAULT;
	_ocv_filter_v.reset(voltage_v + ಆಂತರಿಕ_resistance_estimate_2 * _params.n_cells * current_a);

	ಆದರೆ (_params.r_ಆಂತರಿಕ >= 0.f) { // Use user specified internal resistance value
		_cell_voltage_filter_v.reset(voltage_v / _params.n_cells + _params.r_ಆಂತರಿಕ * current_a);

	} ಇಲ್ಲದಿದ್ದರೆ { // Use estimated internal resistance value
		_cell_voltage_filter_v.reset(voltage_v / _params.n_cells + ಆಂತರಿಕ_resistance_estimate_2 * current_a);
	}
}

ಶೂನ್ಯ Battery::estimate_State_ದ_Charge()
{
	// choose which quantity we're using for final reporting
	ಆದರೆ ((ಸಾಮರ್ಥ್ಯ_mah > 0.f) && _battery_initialized) {
		// if battery capacity is known, fuse voltage measurement with used capacity
		// The lower the voltage the more adjust the estimate with it to avoid deep discharge
		ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ ತೂಕ_v = 3e-2f * (1 - state_ದ_charge_volt_based);
		state_ದ_charge = (1 - ತೂಕ_v) * state_ದ_charge + ತೂಕ_v * state_ದ_charge_volt_based;
		// directly apply current capacity slope calculated using current
		state_ದ_charge -= _discharged_mah_loop / ಸಾಮರ್ಥ್ಯ_mah;
		state_ದ_charge = math::max(state_ದ_charge, 0.f);

		ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ state_ದ_charge_current_based = math::max(1.f - _discharged_mah / ಸಾಮರ್ಥ್ಯ_mah, 0.f);
		state_ದ_charge = math::min(state_ದ_charge_current_based, state_ದ_charge);

	} ಇಲ್ಲದಿದ್ದರೆ {
		state_ದ_charge = state_ದ_charge_volt_based;
	}
}

uint8_t Battery::determineWarning(ದಶಮಾಂಶ state_ದ_charge_2)
{
	ಆದರೆ (state_ದ_charge_2 < _params.emergen_thr) {
		ಹಿಂತಿರುಗಿ battery_ಸ್ಥಿತಿ_s::WARNING_EMERGENCY;

	} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (state_ದ_charge_2 < _params.crit_thr) {
		ಹಿಂತಿರುಗಿ battery_ಸ್ಥಿತಿ_s::WARNING_CRITICAL;

	} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (state_ದ_charge_2 < _params.ಕೆಳಗಿನ_thr) {
		ಹಿಂತಿರುಗಿ battery_ಸ್ಥಿತಿ_s::WARNING_ಕೆಳಗಿನ;

	} ಇಲ್ಲದಿದ್ದರೆ {
		ಹಿಂತಿರುಗಿ battery_ಸ್ಥಿತಿ_s::WARNING_NONE;
	}
}

uint16_t Battery::determine_ದೋಷಗಳು()
{
	uint16_t ದೋಷಗಳು{0};

	ಆದರೆ ((_params.n_cells > 0)
	    && (_voltage_v > (_params.n_cells * _params.v_charged * 1.05f))) {
		// Reported as a "spike" since "over-voltage" does not exist in MAV_BATTERY_FAULT
		ದೋಷಗಳು |= (1 << battery_ಸ್ಥಿತಿ_s::FAULT_SPIKES);
	}

	ಆದರೆ (PX4_ISFINITE(_temperature_c) && _temperature_c > BAT_ತಾತ್ಕಾಲಿಕ_MAX) {
		ದೋಷಗಳು |= (1 << battery_ಸ್ಥಿತಿ_s::FAULT_OVER_TEMPERATURE);
	}

	ಹಿಂತಿರುಗಿ ದೋಷಗಳು;
}

ಶೂನ್ಯ Battery::computeScale()
{
	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ cell_voltage = _cell_voltage_filter_v.getState();

	ಆದರೆ (cell_voltage > FLT_EPSILON) {
		_scale = math::constrain(_params.v_charged / cell_voltage, 1.f, 1.3f); // Allow at most 30% compensation

	} ಇಲ್ಲದಿದ್ದರೆ {
		_scale = 1.f;
	}
}

ದಶಮಾಂಶ Battery::compute_ಉಳಿದ_ಸಮಯ(ದಶಮಾಂಶ current_a)
{
	ದಶಮಾಂಶ ಸಮಯ_ಉಳಿದ_s = NAN;

	_flight_phase_estimation_sub.update();

	ತಾರ್ಕಿಕ reset_filter_transition_now = !vehicle_ಸ್ಥಿತಿ_was_fw && vehicle_ಸ್ಥಿತಿ_is_fw;

	// reset filter if not feasible, negative or we did a VTOL transition to FW mode
	ಆದರೆ (!PX4_ISFINITE(current_ಸರಾಸರಿ_ಮೌಲ್ಯ_filter_a.getState()) || current_ಸರಾಸರಿ_ಮೌಲ್ಯ_filter_a.getState() < FLT_EPSILON
	    || reset_filter_transition_now) {
		current_ಸರಾಸರಿ_ಮೌಲ್ಯ_filter_a.reset(_params.bat_avrg_current);
	}

	vehicle_ಸ್ಥಿತಿ_was_fw = vehicle_ಸ್ಥಿತಿ_is_fw;

	ಆದರೆ (_armed && PX4_ISFINITE(current_a)) {
		// For FW only update when we are in level flight
		ಆದರೆ (!vehicle_ಸ್ಥಿತಿ_is_fw || ((hrt_absolute_ಸಮಯ() - _flight_phase_estimation_sub.get().timestamp) < 2_s
					       && _flight_phase_estimation_sub.get().flight_phase == flight_phase_estimation_s::FLIGHT_PHASE_LEVEL)) {
			ಆದರೆ (_dt > FLT_EPSILON) {
				current_ಸರಾಸರಿ_ಮೌಲ್ಯ_filter_a.update(fmaxf(current_a, 0.f), _dt);

			} ಇಲ್ಲದಿದ್ದರೆ {
				current_ಸರಾಸರಿ_ಮೌಲ್ಯ_filter_a.update(fmaxf(current_a, 0.f));
			}
		}
	}

	// Remaining time estimation only possible with capacity
	ಆದರೆ (ಸಾಮರ್ಥ್ಯ_mah > 0.f) {
		ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ ಉಳಿದ_ಸಾಮರ್ಥ್ಯ_mah = state_ದ_charge * ಸಾಮರ್ಥ್ಯ_mah;
		ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ current_ma = fmaxf(current_ಸರಾಸರಿ_ಮೌಲ್ಯ_filter_a.getState() * 1e3f, FLT_EPSILON);
		ಸಮಯ_ಉಳಿದ_s = ಉಳಿದ_ಸಾಮರ್ಥ್ಯ_mah / current_ma * 3600.f;
	}

	ಹಿಂತಿರುಗಿ ಸಮಯ_ಉಳಿದ_s;
}

ಶೂನ್ಯ Battery::updateParams()
{
	ಸ್ಥಿರಾಂಕ ಪೂರ್ಣಾಂಕ n_cells = ಮೊದಲ_parameter_update ? 0 : _params.n_cells;
	param_get(_param_handles.v_ಖಾಲಿಗಳು, &_params.v_ಖಾಲಿಗಳು);
	param_get(_param_handles.v_charged, &_params.v_charged);
	param_get(_param_handles.n_cells, &_params.n_cells);
	param_get(_param_handles.r_ಆಂತರಿಕ, &_params.r_ಆಂತರಿಕ);
	param_get(_param_handles.ಮೂಲ, &_params.ಮೂಲ);
	param_get(_param_handles.ಕೆಳಗಿನ_thr, &_params.ಕೆಳಗಿನ_thr);
	param_get(_param_handles.crit_thr, &_params.crit_thr);
	param_get(_param_handles.emergen_thr, &_params.emergen_thr);
	param_get(_param_handles.bat_avrg_current, &_params.bat_avrg_current);
	param_get(_param_handles.i_overwrite, &_params.i_overwrite);

	ದಶಮಾಂಶ ಸಾಮರ್ಥ್ಯ{0.f};
	param_get(_param_handles.ಸಾಮರ್ಥ್ಯ, &ಸಾಮರ್ಥ್ಯ);
	ಗಣ_ಸಾಮರ್ಥ್ಯ_Mah(ಸಾಮರ್ಥ್ಯ);

	ಆದರೆ (n_cells != _params.n_cells) {
		ಆಂತರಿಕ_resistance_initialized = ಸುಳ್ಳು;
	}

	ಆದರೆ (!ಆಂತರಿಕ_resistance_initialized && _params.n_cells > 0) {
		_RLS_est(0) = OCV_DEFAULT * _params.n_cells;
		_RLS_est(1) = R_DEFAULT * _params.n_cells;
		_estimation_covariance(0, 0) = OCV_COVARIANCE * _params.n_cells;
		_estimation_covariance(0, 1) = 0.f;
		_estimation_covariance(1, 0) = 0.f;
		_estimation_covariance(1, 1) = R_COVARIANCE * _params.n_cells;
		_estimation_covariance_norm = sqrtf(powf(_estimation_covariance(0, 0), 2.f) + 2.f * powf(_estimation_covariance(1, 0),
						    2.f) + powf(_estimation_covariance(1, 1), 2.f));
		ಆಂತರಿಕ_resistance_initialized = ನಿಜ;
	}

	ModuleParams::updateParams();

	ಮೊದಲ_parameter_update = ಸುಳ್ಳು;
}

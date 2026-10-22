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

#شامل "battery.h"
#شامل <mathlib/mathlib.h>
#شامل <cstring>
#شامل <px4_platform_common/defines.h>

استعمال نامخلا وقت_literals;
استعمال نامخلا میٹرکس;

Battery::Battery(صحیح اشاریہ, ModuleParams *والد, ثابت صحیح sample_interval_us, ثابت uint8_t ماخذ) :
	ModuleParams(والد),
	اشاریہ_2(اشاریہ < 1 || اشاریہ > 9 ? 1 : اشاریہ),
	ماخذ_2(ماخذ)
{
	ثابت اعشاری expected_filter_dt = static_cast<اعشاری>(sample_interval_us) * 1e-6f;
	current_اوسط_قدر_filter_a.سیٹ_Parameters(expected_filter_dt, 50.f);
	_ocv_filter_v.سیٹ_Parameters(expected_filter_dt, 1.f);
	_cell_voltage_filter_v.سیٹ_Parameters(expected_filter_dt, 1.f);

	اگر (اشاریہ > 9 || اشاریہ < 1) {
		PX4_ERR("Battery index must be between 1 and 9 (inclusive). Received %d. Defaulting to 1.", اشاریہ);
	}

	// 16 chars for parameter name + null terminator
	حرف param_name[17];

	snprintf(param_name, حجم(param_name), "BAT%d_V_EMPTY", اشاریہ_2);
	_param_handles.v_خالی_خانے = param_ڈھونڈو(param_name);

	اگر (_param_handles.v_خالی_خانے == PARAM_INVALID) {
		PX4_ERR("Could not find parameter with name %s", param_name);
	}

	snprintf(param_name, حجم(param_name), "BAT%d_V_CHARGED", اشاریہ_2);
	_param_handles.v_charged = param_ڈھونڈو(param_name);

	snprintf(param_name, حجم(param_name), "BAT%d_N_CELLS", اشاریہ_2);
	_param_handles.n_cells = param_ڈھونڈو(param_name);

	snprintf(param_name, حجم(param_name), "BAT%d_CAPACITY", اشاریہ_2);
	_param_handles.استعداد = param_ڈھونڈو(param_name);

	snprintf(param_name, حجم(param_name), "BAT%d_R_INTERNAL", اشاریہ_2);
	_param_handles.r_اندرونی = param_ڈھونڈو(param_name);

	snprintf(param_name, حجم(param_name), "BAT%d_SOURCE", اشاریہ_2);
	_param_handles.ماخذ = param_ڈھونڈو(param_name);

	snprintf(param_name, حجم(param_name), "BAT%d_I_OVERWRITE", اشاریہ_2);
	_param_handles.i_overwrite = param_ڈھونڈو(param_name);

	_param_handles.کم_thr = param_ڈھونڈو("BAT_LOW_THR");
	_param_handles.crit_thr = param_ڈھونڈو("BAT_CRIT_THR");
	_param_handles.emergen_thr = param_ڈھونڈو("BAT_EMERGEN_THR");

	_param_handles.bat_avrg_current = param_ڈھونڈو("BAT_AVRG_CURRENT");

	updateParams();
}

خالی Battery::updateVoltage(ثابت اعشاری voltage_v)
{
	_voltage_v = voltage_v;
}

خالی Battery::updateCurrent(ثابت اعشاری current_a)
{
	// Overwrite the measured current if current overwrite is defined and vehicle is unarmed
	اگر (!_armed && _params.i_overwrite > FLT_EPSILON) {
		_current_a = _params.i_overwrite;

	} ورنہ {
		_current_a = current_a;
	}
}

خالی Battery::updateTemperature(ثابت اعشاری temperature_c)
{
	_temperature_c = temperature_c;
}

خالی Battery::update_Battery_حالت(ثابت hrt_abstime &timestamp)
{
	updateDt(timestamp);

	// Require minimum voltage otherwise override connected status
	اگر (_voltage_v < LITHIUM_BATTERY_RECOGNITION_VOLTAGE) {
		_connected = جھوٹ;
	}

	اگر (!_connected || (آخری_unconnected_timestamp == 0)) {
		آخری_unconnected_timestamp = timestamp;
	}

	// Wait with initializing filters to avoid relying on a voltage sample from the rising edge
	_battery_initialized = _connected && (timestamp > آخری_unconnected_timestamp + 2_s);

	اگر (_connected && !_battery_initialized && اندرونی_resistance_initialized && _params.n_cells > 0) {
		reset_اندرونی_Resistance_Estimation(_voltage_v, _current_a);
	}

	مجموع_Discharged(_current_a);
	state_کا_charge_volt_based =
		calculate_State_کا_Charge_Voltage_Based(_voltage_v, _current_a);

	اگر (!external_state_کا_charge) {
		estimate_State_کا_Charge();
	}

	computeScale();

	اگر (_connected && _battery_initialized) {
		_warning = determineWarning(state_کا_charge);
	}

	اگر (vehicle_حالت_sub.updated()) {
		vehicle_حالت_s vehicle_حالت;

		اگر (vehicle_حالت_sub.copy(&vehicle_حالت)) {
			_armed = (vehicle_حالت.arming_state == vehicle_حالت_s::ARMING_STATE_ARMED);
			vehicle_حالت_is_fw = (vehicle_حالت.vehicle_type == vehicle_حالت_s::VEHICLE_TYPE_FIXED_WING);
		}
	}
}

battery_حالت_s Battery::get_Battery_حالت()
{
	battery_حالت_s battery_حالت{};
	battery_حالت.voltage_v = _voltage_v;
	battery_حالت.current_a = _current_a;
	battery_حالت.current_اوسط_قدر_a = current_اوسط_قدر_filter_a.getState();
	battery_حالت.discharged_mah = _discharged_mah;
	battery_حالت.بقایا = state_کا_charge;
	battery_حالت.scale = _scale;
	battery_حالت.وقت_بقایا_s = compute_بقایا_وقت(_current_a);
	battery_حالت.temperature = _temperature_c;
	battery_حالت.cell_گنتی = _params.n_cells;
	battery_حالت.connected = _connected;
	battery_حالت.ماخذ = ماخذ_2;
	battery_حالت.ترجیح = ترجیح_2;
	battery_حالت.استعداد = static_cast<uint16_t>(استعداد_mah);
	battery_حالت.id = static_cast<uint8_t>(اشاریہ_2);
	battery_حالت.warning = _warning;
	battery_حالت.timestamp = hrt_absolute_وقت();
	battery_حالت.خرابیاں = determine_خرابیاں();
	battery_حالت.بقایا_استعداد_wh = NAN; // not measured by dumb power module; smart-battery drivers overwrite
	battery_حالت.nominal_voltage = NAN;        // not measured by dumb power module; smart-battery drivers overwrite
	battery_حالت.اندرونی_resistance_estimate = اندرونی_resistance_estimate_2;
	battery_حالت.ocv_estimate = _voltage_v + اندرونی_resistance_estimate_2 * _params.n_cells * _current_a;
	battery_حالت.ocv_estimate_filtered = _ocv_filter_v.getState();
	battery_حالت.volt_based_soc_estimate = _params.n_cells > 0 ? math::interpolate(_ocv_filter_v.getState() /
			_params.n_cells,
			_params.v_خالی_خانے, _params.v_charged, 0.f, 1.f) : -1.f;
	battery_حالت.voltage_prediction = _voltage_prediction;
	battery_حالت.prediction_error = _prediction_error;
	battery_حالت.estimation_covariance_norm = _estimation_covariance_norm;
	واپس battery_حالت;
}

خالی Battery::publish_Battery_حالت(ثابت battery_حالت_s &battery_حالت)
{
	اگر (ماخذ_2 == _params.ماخذ) {
		battery_حالت_pub.publish(battery_حالت);
	}
}

خالی Battery::update_And_Publish_Battery_حالت(ثابت hrt_abstime &timestamp)
{
	update_Battery_حالت(timestamp);

	battery_حالت_s battery_حالت = get_Battery_حالت();

	_failure_config.update();

	اگر (!failure_injection::process_battery(_failure_config, battery_حالت.id, battery_حالت)) {
		واپس;
	}

	publish_Battery_حالت(battery_حالت);
}
خالی Battery::updateDt(ثابت hrt_abstime &timestamp)
{
	اگر (آخری_timestamp != 0) {
		// _dt_discharge is the true, unclamped time delta: for the coulomb
		// count in sumDischarged() below, using the real elapsed time is
		// always more accurate than clamping it, even across an unusually
		// long gap between updates (e.g. an update source that reports
		// less often than every 2 seconds).
		_dt_discharge = (timestamp - آخری_timestamp) / 1e6f;

		// _dt is clamped to guard the numerical stability of the current
		// average filter below, which assumes a roughly steady sample
		// interval and should not see a single very large dt.
		_dt = math::min(_dt_discharge, 2.f);
	}

	آخری_timestamp = timestamp;
}

اعشاری Battery::مجموع_Discharged(اعشاری current_a)
{
	اگر (_dt_discharge > FLT_EPSILON && fabsf(current_a + 1.f) > FLT_EPSILON) {
		// mAh since last loop: (current[A] * 1000 = [mA]) * (dt[s] / 3600 = [h])
		// current = -1 means invalid current measurement
		_discharged_mah_loop = (current_a * 1e3f) * (_dt_discharge / 3600.f);
		_discharged_mah += _discharged_mah_loop;
	}

	واپس _discharged_mah;
}

اعشاری Battery::calculate_State_کا_Charge_Voltage_Based(ثابت اعشاری voltage_v, ثابت اعشاری current_a)
{
	اگر (_params.n_cells == 0) {
		واپس -1.0f;
	}

	// remaining battery capacity based on voltage
	اعشاری cell_voltage = voltage_v / _params.n_cells;

	// correct battery voltage locally for load drop according to internal resistance and current
	اگر (current_a > FLT_EPSILON) {
		update_اندرونی_Resistance_Estimation(voltage_v, current_a);

		اگر (_params.r_اندرونی >= 0.f) { // Use user specified internal resistance value
			cell_voltage += _params.r_اندرونی * current_a;

		} ورنہ { // Use estimated internal resistance value
			cell_voltage += اندرونی_resistance_estimate_2 * current_a;
		}

	}

	_cell_voltage_filter_v.update(cell_voltage);
	واپس math::interpolate(_cell_voltage_filter_v.getState(), _params.v_خالی_خانے, _params.v_charged, 0.f, 1.f);
}

خالی Battery::update_اندرونی_Resistance_Estimation(ثابت اعشاری voltage_v, ثابت اعشاری current_a)
{
	Vector2f x{1, -current_a};
	_voltage_prediction = (x.transpose() * _RLS_est)(0, 0);
	_prediction_error = voltage_v - _voltage_prediction;
	ثابت Vector2f gamma = _estimation_covariance * x / (LAMBDA + (x.transpose() * _estimation_covariance * x)(0, 0));
	ثابت Vector2f RSL_est_عارضی = _RLS_est + gamma * _prediction_error;
	ثابت Matrix2f estimation_covariance_عارضی = (_estimation_covariance
			- میٹرکس_2<اعشاری, 2, 1>(gamma) * (x.transpose() * _estimation_covariance)) / LAMBDA;
	ثابت اعشاری estimation_covariance_عارضی_norm =
		sqrtf(powf(estimation_covariance_عارضی(0, 0), 2.f)
		      + 2.f * powf(estimation_covariance_عارضی(1, 0), 2.f)
		      + powf(estimation_covariance_عارضی(1, 1), 2.f));

	اگر (estimation_covariance_عارضی_norm < _estimation_covariance_norm) { // Only update if estimation improves
		_RLS_est = RSL_est_عارضی;
		_estimation_covariance = estimation_covariance_عارضی;
		_estimation_covariance_norm = estimation_covariance_عارضی_norm;
		اندرونی_resistance_estimate_2 =
			math::max(_RLS_est(1) / _params.n_cells, 0.f); // Only use positive values

	} ورنہ { // Update OCV estimate with IR estimate
		_RLS_est(0) = voltage_v + _RLS_est(1) * current_a;
	}

	_ocv_filter_v.update(voltage_v + اندرونی_resistance_estimate_2 * _params.n_cells * current_a);
}

خالی Battery::reset_اندرونی_Resistance_Estimation(ثابت اعشاری voltage_v, ثابت اعشاری current_a)
{
	_RLS_est(0) = voltage_v;
	_RLS_est(1) = R_DEFAULT * _params.n_cells;
	_estimation_covariance.سیٹ_Zero();
	_estimation_covariance(0, 0) = OCV_COVARIANCE * _params.n_cells;
	_estimation_covariance(1, 1) = R_COVARIANCE * _params.n_cells;
	_estimation_covariance_norm = sqrtf(powf(_estimation_covariance(0, 0), 2.f) + 2.f * powf(_estimation_covariance(1, 0),
					    2.f) + powf(_estimation_covariance(1, 1), 2.f));
	اندرونی_resistance_estimate_2 = R_DEFAULT;
	_ocv_filter_v.reset(voltage_v + اندرونی_resistance_estimate_2 * _params.n_cells * current_a);

	اگر (_params.r_اندرونی >= 0.f) { // Use user specified internal resistance value
		_cell_voltage_filter_v.reset(voltage_v / _params.n_cells + _params.r_اندرونی * current_a);

	} ورنہ { // Use estimated internal resistance value
		_cell_voltage_filter_v.reset(voltage_v / _params.n_cells + اندرونی_resistance_estimate_2 * current_a);
	}
}

خالی Battery::estimate_State_کا_Charge()
{
	// choose which quantity we're using for final reporting
	اگر ((استعداد_mah > 0.f) && _battery_initialized) {
		// if battery capacity is known, fuse voltage measurement with used capacity
		// The lower the voltage the more adjust the estimate with it to avoid deep discharge
		ثابت اعشاری وزن_v = 3e-2f * (1 - state_کا_charge_volt_based);
		state_کا_charge = (1 - وزن_v) * state_کا_charge + وزن_v * state_کا_charge_volt_based;
		// directly apply current capacity slope calculated using current
		state_کا_charge -= _discharged_mah_loop / استعداد_mah;
		state_کا_charge = math::max(state_کا_charge, 0.f);

		ثابت اعشاری state_کا_charge_current_based = math::max(1.f - _discharged_mah / استعداد_mah, 0.f);
		state_کا_charge = math::min(state_کا_charge_current_based, state_کا_charge);

	} ورنہ {
		state_کا_charge = state_کا_charge_volt_based;
	}
}

uint8_t Battery::determineWarning(اعشاری state_کا_charge_2)
{
	اگر (state_کا_charge_2 < _params.emergen_thr) {
		واپس battery_حالت_s::WARNING_EMERGENCY;

	} ورنہ اگر (state_کا_charge_2 < _params.crit_thr) {
		واپس battery_حالت_s::WARNING_CRITICAL;

	} ورنہ اگر (state_کا_charge_2 < _params.کم_thr) {
		واپس battery_حالت_s::WARNING_کم;

	} ورنہ {
		واپس battery_حالت_s::WARNING_NONE;
	}
}

uint16_t Battery::determine_خرابیاں()
{
	uint16_t خرابیاں{0};

	اگر ((_params.n_cells > 0)
	    && (_voltage_v > (_params.n_cells * _params.v_charged * 1.05f))) {
		// Reported as a "spike" since "over-voltage" does not exist in MAV_BATTERY_FAULT
		خرابیاں |= (1 << battery_حالت_s::FAULT_SPIKES);
	}

	اگر (PX4_ISFINITE(_temperature_c) && _temperature_c > BAT_عارضی_MAX) {
		خرابیاں |= (1 << battery_حالت_s::FAULT_OVER_TEMPERATURE);
	}

	واپس خرابیاں;
}

خالی Battery::computeScale()
{
	ثابت اعشاری cell_voltage = _cell_voltage_filter_v.getState();

	اگر (cell_voltage > FLT_EPSILON) {
		_scale = math::constrain(_params.v_charged / cell_voltage, 1.f, 1.3f); // Allow at most 30% compensation

	} ورنہ {
		_scale = 1.f;
	}
}

اعشاری Battery::compute_بقایا_وقت(اعشاری current_a)
{
	اعشاری وقت_بقایا_s = NAN;

	_flight_phase_estimation_sub.update();

	منطقی reset_filter_transition_now = !vehicle_حالت_was_fw && vehicle_حالت_is_fw;

	// reset filter if not feasible, negative or we did a VTOL transition to FW mode
	اگر (!PX4_ISFINITE(current_اوسط_قدر_filter_a.getState()) || current_اوسط_قدر_filter_a.getState() < FLT_EPSILON
	    || reset_filter_transition_now) {
		current_اوسط_قدر_filter_a.reset(_params.bat_avrg_current);
	}

	vehicle_حالت_was_fw = vehicle_حالت_is_fw;

	اگر (_armed && PX4_ISFINITE(current_a)) {
		// For FW only update when we are in level flight
		اگر (!vehicle_حالت_is_fw || ((hrt_absolute_وقت() - _flight_phase_estimation_sub.get().timestamp) < 2_s
					       && _flight_phase_estimation_sub.get().flight_phase == flight_phase_estimation_s::FLIGHT_PHASE_LEVEL)) {
			اگر (_dt > FLT_EPSILON) {
				current_اوسط_قدر_filter_a.update(fmaxf(current_a, 0.f), _dt);

			} ورنہ {
				current_اوسط_قدر_filter_a.update(fmaxf(current_a, 0.f));
			}
		}
	}

	// Remaining time estimation only possible with capacity
	اگر (استعداد_mah > 0.f) {
		ثابت اعشاری بقایا_استعداد_mah = state_کا_charge * استعداد_mah;
		ثابت اعشاری current_ma = fmaxf(current_اوسط_قدر_filter_a.getState() * 1e3f, FLT_EPSILON);
		وقت_بقایا_s = بقایا_استعداد_mah / current_ma * 3600.f;
	}

	واپس وقت_بقایا_s;
}

خالی Battery::updateParams()
{
	ثابت صحیح n_cells = پہلا_parameter_update ? 0 : _params.n_cells;
	param_get(_param_handles.v_خالی_خانے, &_params.v_خالی_خانے);
	param_get(_param_handles.v_charged, &_params.v_charged);
	param_get(_param_handles.n_cells, &_params.n_cells);
	param_get(_param_handles.r_اندرونی, &_params.r_اندرونی);
	param_get(_param_handles.ماخذ, &_params.ماخذ);
	param_get(_param_handles.کم_thr, &_params.کم_thr);
	param_get(_param_handles.crit_thr, &_params.crit_thr);
	param_get(_param_handles.emergen_thr, &_params.emergen_thr);
	param_get(_param_handles.bat_avrg_current, &_params.bat_avrg_current);
	param_get(_param_handles.i_overwrite, &_params.i_overwrite);

	اعشاری استعداد{0.f};
	param_get(_param_handles.استعداد, &استعداد);
	سیٹ_استعداد_Mah(استعداد);

	اگر (n_cells != _params.n_cells) {
		اندرونی_resistance_initialized = جھوٹ;
	}

	اگر (!اندرونی_resistance_initialized && _params.n_cells > 0) {
		_RLS_est(0) = OCV_DEFAULT * _params.n_cells;
		_RLS_est(1) = R_DEFAULT * _params.n_cells;
		_estimation_covariance(0, 0) = OCV_COVARIANCE * _params.n_cells;
		_estimation_covariance(0, 1) = 0.f;
		_estimation_covariance(1, 0) = 0.f;
		_estimation_covariance(1, 1) = R_COVARIANCE * _params.n_cells;
		_estimation_covariance_norm = sqrtf(powf(_estimation_covariance(0, 0), 2.f) + 2.f * powf(_estimation_covariance(1, 0),
						    2.f) + powf(_estimation_covariance(1, 1), 2.f));
		اندرونی_resistance_initialized = سچ;
	}

	ModuleParams::updateParams();

	پہلا_parameter_update = جھوٹ;
}

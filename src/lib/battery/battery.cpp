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

#ضمن "battery.h"
#ضمن <mathlib/mathlib.h>
#ضمن <cstring>
#ضمن <px4_platform_common/defines.h>

استفاده نطاق زمان_literals;
استفاده نطاق ماتریس;

Battery::Battery(صحيح شاخص, ModuleParams *والد, ثابت صحيح sample_interval_us, ثابت uint8_t منبع) :
	ModuleParams(والد),
	شاخص_2(شاخص < 1 || شاخص > 9 ? 1 : شاخص),
	منبع_2(منبع)
{
	ثابت عائم expected_filter_dt = static_cast<عائم>(sample_interval_us) * 1e-6f;
	current_معدل_filter_a.مجموعه_Parameters(expected_filter_dt, 50.f);
	_ocv_filter_v.مجموعه_Parameters(expected_filter_dt, 1.f);
	_cell_voltage_filter_v.مجموعه_Parameters(expected_filter_dt, 1.f);

	إذا (شاخص > 9 || شاخص < 1) {
		PX4_ERR("Battery index must be between 1 and 9 (inclusive). Received %d. Defaulting to 1.", شاخص);
	}

	// 16 chars for parameter name + null terminator
	حرف param_name[17];

	snprintf(param_name, حجم(param_name), "BAT%d_V_EMPTY", شاخص_2);
	_param_handles.v_خالی = param_پیدا_کن(param_name);

	إذا (_param_handles.v_خالی == PARAM_INVALID) {
		PX4_ERR("Could not find parameter with name %s", param_name);
	}

	snprintf(param_name, حجم(param_name), "BAT%d_V_CHARGED", شاخص_2);
	_param_handles.v_charged = param_پیدا_کن(param_name);

	snprintf(param_name, حجم(param_name), "BAT%d_N_CELLS", شاخص_2);
	_param_handles.n_cells = param_پیدا_کن(param_name);

	snprintf(param_name, حجم(param_name), "BAT%d_CAPACITY", شاخص_2);
	_param_handles.ظرفیت = param_پیدا_کن(param_name);

	snprintf(param_name, حجم(param_name), "BAT%d_R_INTERNAL", شاخص_2);
	_param_handles.r_داخلی = param_پیدا_کن(param_name);

	snprintf(param_name, حجم(param_name), "BAT%d_SOURCE", شاخص_2);
	_param_handles.منبع = param_پیدا_کن(param_name);

	snprintf(param_name, حجم(param_name), "BAT%d_I_OVERWRITE", شاخص_2);
	_param_handles.i_overwrite = param_پیدا_کن(param_name);

	_param_handles.کم_thr = param_پیدا_کن("BAT_LOW_THR");
	_param_handles.crit_thr = param_پیدا_کن("BAT_CRIT_THR");
	_param_handles.emergen_thr = param_پیدا_کن("BAT_EMERGEN_THR");

	_param_handles.bat_avrg_current = param_پیدا_کن("BAT_AVRG_CURRENT");

	updateParams();
}

فراغ Battery::updateVoltage(ثابت عائم voltage_v)
{
	_voltage_v = voltage_v;
}

فراغ Battery::updateCurrent(ثابت عائم current_a)
{
	// Overwrite the measured current if current overwrite is defined and vehicle is unarmed
	إذا (!_armed && _params.i_overwrite > FLT_EPSILON) {
		_current_a = _params.i_overwrite;

	} وإلا {
		_current_a = current_a;
	}
}

فراغ Battery::updateTemperature(ثابت عائم temperature_c)
{
	_temperature_c = temperature_c;
}

فراغ Battery::update_Battery_وضعیت(ثابت hrt_abstime &timestamp)
{
	updateDt(timestamp);

	// Require minimum voltage otherwise override connected status
	إذا (_voltage_v < LITHIUM_BATTERY_RECOGNITION_VOLTAGE) {
		_connected = خطأ;
	}

	إذا (!_connected || (آخرین_unconnected_timestamp == 0)) {
		آخرین_unconnected_timestamp = timestamp;
	}

	// Wait with initializing filters to avoid relying on a voltage sample from the rising edge
	_battery_initialized = _connected && (timestamp > آخرین_unconnected_timestamp + 2_s);

	إذا (_connected && !_battery_initialized && داخلی_resistance_initialized && _params.n_cells > 0) {
		reset_داخلی_Resistance_Estimation(_voltage_v, _current_a);
	}

	جمع_Discharged(_current_a);
	state_از_charge_volt_based =
		calculate_State_از_Charge_Voltage_Based(_voltage_v, _current_a);

	إذا (!external_state_از_charge) {
		estimate_State_از_Charge();
	}

	computeScale();

	إذا (_connected && _battery_initialized) {
		_warning = determineWarning(state_از_charge);
	}

	إذا (vehicle_وضعیت_sub.updated()) {
		vehicle_وضعیت_s vehicle_وضعیت;

		إذا (vehicle_وضعیت_sub.copy(&vehicle_وضعیت)) {
			_armed = (vehicle_وضعیت.arming_state == vehicle_وضعیت_s::ARMING_STATE_ARMED);
			vehicle_وضعیت_is_fw = (vehicle_وضعیت.vehicle_type == vehicle_وضعیت_s::VEHICLE_TYPE_FIXED_WING);
		}
	}
}

battery_وضعیت_s Battery::get_Battery_وضعیت()
{
	battery_وضعیت_s battery_وضعیت{};
	battery_وضعیت.voltage_v = _voltage_v;
	battery_وضعیت.current_a = _current_a;
	battery_وضعیت.current_معدل_a = current_معدل_filter_a.getState();
	battery_وضعیت.discharged_mah = _discharged_mah;
	battery_وضعیت.مانده = state_از_charge;
	battery_وضعیت.scale = _scale;
	battery_وضعیت.زمان_مانده_s = compute_مانده_زمان(_current_a);
	battery_وضعیت.temperature = _temperature_c;
	battery_وضعیت.cell_شمار = _params.n_cells;
	battery_وضعیت.connected = _connected;
	battery_وضعیت.منبع = منبع_2;
	battery_وضعیت.اولویت = اولویت_2;
	battery_وضعیت.ظرفیت = static_cast<uint16_t>(ظرفیت_mah);
	battery_وضعیت.id = static_cast<uint8_t>(شاخص_2);
	battery_وضعیت.warning = _warning;
	battery_وضعیت.timestamp = hrt_absolute_زمان();
	battery_وضعیت.خطاها = determine_خطاها();
	battery_وضعیت.مانده_ظرفیت_wh = NAN; // not measured by dumb power module; smart-battery drivers overwrite
	battery_وضعیت.nominal_voltage = NAN;        // not measured by dumb power module; smart-battery drivers overwrite
	battery_وضعیت.داخلی_resistance_estimate = داخلی_resistance_estimate_2;
	battery_وضعیت.ocv_estimate = _voltage_v + داخلی_resistance_estimate_2 * _params.n_cells * _current_a;
	battery_وضعیت.ocv_estimate_filtered = _ocv_filter_v.getState();
	battery_وضعیت.volt_based_soc_estimate = _params.n_cells > 0 ? math::interpolate(_ocv_filter_v.getState() /
			_params.n_cells,
			_params.v_خالی, _params.v_charged, 0.f, 1.f) : -1.f;
	battery_وضعیت.voltage_prediction = _voltage_prediction;
	battery_وضعیت.prediction_error = _prediction_error;
	battery_وضعیت.estimation_covariance_norm = _estimation_covariance_norm;
	أرجع battery_وضعیت;
}

فراغ Battery::publish_Battery_وضعیت(ثابت battery_وضعیت_s &battery_وضعیت)
{
	إذا (منبع_2 == _params.منبع) {
		battery_وضعیت_pub.publish(battery_وضعیت);
	}
}

فراغ Battery::update_And_Publish_Battery_وضعیت(ثابت hrt_abstime &timestamp)
{
	update_Battery_وضعیت(timestamp);

	battery_وضعیت_s battery_وضعیت = get_Battery_وضعیت();

	_failure_config.update();

	إذا (!failure_injection::process_battery(_failure_config, battery_وضعیت.id, battery_وضعیت)) {
		أرجع;
	}

	publish_Battery_وضعیت(battery_وضعیت);
}
فراغ Battery::updateDt(ثابت hrt_abstime &timestamp)
{
	إذا (آخرین_timestamp != 0) {
		// _dt_discharge is the true, unclamped time delta: for the coulomb
		// count in sumDischarged() below, using the real elapsed time is
		// always more accurate than clamping it, even across an unusually
		// long gap between updates (e.g. an update source that reports
		// less often than every 2 seconds).
		_dt_discharge = (timestamp - آخرین_timestamp) / 1e6f;

		// _dt is clamped to guard the numerical stability of the current
		// average filter below, which assumes a roughly steady sample
		// interval and should not see a single very large dt.
		_dt = math::min(_dt_discharge, 2.f);
	}

	آخرین_timestamp = timestamp;
}

عائم Battery::جمع_Discharged(عائم current_a)
{
	إذا (_dt_discharge > FLT_EPSILON && fabsf(current_a + 1.f) > FLT_EPSILON) {
		// mAh since last loop: (current[A] * 1000 = [mA]) * (dt[s] / 3600 = [h])
		// current = -1 means invalid current measurement
		_discharged_mah_loop = (current_a * 1e3f) * (_dt_discharge / 3600.f);
		_discharged_mah += _discharged_mah_loop;
	}

	أرجع _discharged_mah;
}

عائم Battery::calculate_State_از_Charge_Voltage_Based(ثابت عائم voltage_v, ثابت عائم current_a)
{
	إذا (_params.n_cells == 0) {
		أرجع -1.0f;
	}

	// remaining battery capacity based on voltage
	عائم cell_voltage = voltage_v / _params.n_cells;

	// correct battery voltage locally for load drop according to internal resistance and current
	إذا (current_a > FLT_EPSILON) {
		update_داخلی_Resistance_Estimation(voltage_v, current_a);

		إذا (_params.r_داخلی >= 0.f) { // Use user specified internal resistance value
			cell_voltage += _params.r_داخلی * current_a;

		} وإلا { // Use estimated internal resistance value
			cell_voltage += داخلی_resistance_estimate_2 * current_a;
		}

	}

	_cell_voltage_filter_v.update(cell_voltage);
	أرجع math::interpolate(_cell_voltage_filter_v.getState(), _params.v_خالی, _params.v_charged, 0.f, 1.f);
}

فراغ Battery::update_داخلی_Resistance_Estimation(ثابت عائم voltage_v, ثابت عائم current_a)
{
	Vector2f x{1, -current_a};
	_voltage_prediction = (x.transpose() * _RLS_est)(0, 0);
	_prediction_error = voltage_v - _voltage_prediction;
	ثابت Vector2f gamma = _estimation_covariance * x / (LAMBDA + (x.transpose() * _estimation_covariance * x)(0, 0));
	ثابت Vector2f RSL_est_موقت = _RLS_est + gamma * _prediction_error;
	ثابت Matrix2f estimation_covariance_موقت = (_estimation_covariance
			- ماتریس_2<عائم, 2, 1>(gamma) * (x.transpose() * _estimation_covariance)) / LAMBDA;
	ثابت عائم estimation_covariance_موقت_norm =
		sqrtf(powf(estimation_covariance_موقت(0, 0), 2.f)
		      + 2.f * powf(estimation_covariance_موقت(1, 0), 2.f)
		      + powf(estimation_covariance_موقت(1, 1), 2.f));

	إذا (estimation_covariance_موقت_norm < _estimation_covariance_norm) { // Only update if estimation improves
		_RLS_est = RSL_est_موقت;
		_estimation_covariance = estimation_covariance_موقت;
		_estimation_covariance_norm = estimation_covariance_موقت_norm;
		داخلی_resistance_estimate_2 =
			math::max(_RLS_est(1) / _params.n_cells, 0.f); // Only use positive values

	} وإلا { // Update OCV estimate with IR estimate
		_RLS_est(0) = voltage_v + _RLS_est(1) * current_a;
	}

	_ocv_filter_v.update(voltage_v + داخلی_resistance_estimate_2 * _params.n_cells * current_a);
}

فراغ Battery::reset_داخلی_Resistance_Estimation(ثابت عائم voltage_v, ثابت عائم current_a)
{
	_RLS_est(0) = voltage_v;
	_RLS_est(1) = R_DEFAULT * _params.n_cells;
	_estimation_covariance.مجموعه_Zero();
	_estimation_covariance(0, 0) = OCV_COVARIANCE * _params.n_cells;
	_estimation_covariance(1, 1) = R_COVARIANCE * _params.n_cells;
	_estimation_covariance_norm = sqrtf(powf(_estimation_covariance(0, 0), 2.f) + 2.f * powf(_estimation_covariance(1, 0),
					    2.f) + powf(_estimation_covariance(1, 1), 2.f));
	داخلی_resistance_estimate_2 = R_DEFAULT;
	_ocv_filter_v.reset(voltage_v + داخلی_resistance_estimate_2 * _params.n_cells * current_a);

	إذا (_params.r_داخلی >= 0.f) { // Use user specified internal resistance value
		_cell_voltage_filter_v.reset(voltage_v / _params.n_cells + _params.r_داخلی * current_a);

	} وإلا { // Use estimated internal resistance value
		_cell_voltage_filter_v.reset(voltage_v / _params.n_cells + داخلی_resistance_estimate_2 * current_a);
	}
}

فراغ Battery::estimate_State_از_Charge()
{
	// choose which quantity we're using for final reporting
	إذا ((ظرفیت_mah > 0.f) && _battery_initialized) {
		// if battery capacity is known, fuse voltage measurement with used capacity
		// The lower the voltage the more adjust the estimate with it to avoid deep discharge
		ثابت عائم وزن_v = 3e-2f * (1 - state_از_charge_volt_based);
		state_از_charge = (1 - وزن_v) * state_از_charge + وزن_v * state_از_charge_volt_based;
		// directly apply current capacity slope calculated using current
		state_از_charge -= _discharged_mah_loop / ظرفیت_mah;
		state_از_charge = math::max(state_از_charge, 0.f);

		ثابت عائم state_از_charge_current_based = math::max(1.f - _discharged_mah / ظرفیت_mah, 0.f);
		state_از_charge = math::min(state_از_charge_current_based, state_از_charge);

	} وإلا {
		state_از_charge = state_از_charge_volt_based;
	}
}

uint8_t Battery::determineWarning(عائم state_از_charge_2)
{
	إذا (state_از_charge_2 < _params.emergen_thr) {
		أرجع battery_وضعیت_s::WARNING_EMERGENCY;

	} وإلا إذا (state_از_charge_2 < _params.crit_thr) {
		أرجع battery_وضعیت_s::WARNING_CRITICAL;

	} وإلا إذا (state_از_charge_2 < _params.کم_thr) {
		أرجع battery_وضعیت_s::WARNING_کم;

	} وإلا {
		أرجع battery_وضعیت_s::WARNING_NONE;
	}
}

uint16_t Battery::determine_خطاها()
{
	uint16_t خطاها{0};

	إذا ((_params.n_cells > 0)
	    && (_voltage_v > (_params.n_cells * _params.v_charged * 1.05f))) {
		// Reported as a "spike" since "over-voltage" does not exist in MAV_BATTERY_FAULT
		خطاها |= (1 << battery_وضعیت_s::FAULT_SPIKES);
	}

	إذا (PX4_ISFINITE(_temperature_c) && _temperature_c > BAT_موقت_MAX) {
		خطاها |= (1 << battery_وضعیت_s::FAULT_OVER_TEMPERATURE);
	}

	أرجع خطاها;
}

فراغ Battery::computeScale()
{
	ثابت عائم cell_voltage = _cell_voltage_filter_v.getState();

	إذا (cell_voltage > FLT_EPSILON) {
		_scale = math::constrain(_params.v_charged / cell_voltage, 1.f, 1.3f); // Allow at most 30% compensation

	} وإلا {
		_scale = 1.f;
	}
}

عائم Battery::compute_مانده_زمان(عائم current_a)
{
	عائم زمان_مانده_s = NAN;

	_flight_phase_estimation_sub.update();

	منطقي reset_filter_transition_now = !vehicle_وضعیت_was_fw && vehicle_وضعیت_is_fw;

	// reset filter if not feasible, negative or we did a VTOL transition to FW mode
	إذا (!PX4_ISFINITE(current_معدل_filter_a.getState()) || current_معدل_filter_a.getState() < FLT_EPSILON
	    || reset_filter_transition_now) {
		current_معدل_filter_a.reset(_params.bat_avrg_current);
	}

	vehicle_وضعیت_was_fw = vehicle_وضعیت_is_fw;

	إذا (_armed && PX4_ISFINITE(current_a)) {
		// For FW only update when we are in level flight
		إذا (!vehicle_وضعیت_is_fw || ((hrt_absolute_زمان() - _flight_phase_estimation_sub.get().timestamp) < 2_s
					       && _flight_phase_estimation_sub.get().flight_phase == flight_phase_estimation_s::FLIGHT_PHASE_LEVEL)) {
			إذا (_dt > FLT_EPSILON) {
				current_معدل_filter_a.update(fmaxf(current_a, 0.f), _dt);

			} وإلا {
				current_معدل_filter_a.update(fmaxf(current_a, 0.f));
			}
		}
	}

	// Remaining time estimation only possible with capacity
	إذا (ظرفیت_mah > 0.f) {
		ثابت عائم مانده_ظرفیت_mah = state_از_charge * ظرفیت_mah;
		ثابت عائم current_ma = fmaxf(current_معدل_filter_a.getState() * 1e3f, FLT_EPSILON);
		زمان_مانده_s = مانده_ظرفیت_mah / current_ma * 3600.f;
	}

	أرجع زمان_مانده_s;
}

فراغ Battery::updateParams()
{
	ثابت صحيح n_cells = اولی_parameter_update ? 0 : _params.n_cells;
	param_get(_param_handles.v_خالی, &_params.v_خالی);
	param_get(_param_handles.v_charged, &_params.v_charged);
	param_get(_param_handles.n_cells, &_params.n_cells);
	param_get(_param_handles.r_داخلی, &_params.r_داخلی);
	param_get(_param_handles.منبع, &_params.منبع);
	param_get(_param_handles.کم_thr, &_params.کم_thr);
	param_get(_param_handles.crit_thr, &_params.crit_thr);
	param_get(_param_handles.emergen_thr, &_params.emergen_thr);
	param_get(_param_handles.bat_avrg_current, &_params.bat_avrg_current);
	param_get(_param_handles.i_overwrite, &_params.i_overwrite);

	عائم ظرفیت{0.f};
	param_get(_param_handles.ظرفیت, &ظرفیت);
	مجموعه_ظرفیت_Mah(ظرفیت);

	إذا (n_cells != _params.n_cells) {
		داخلی_resistance_initialized = خطأ;
	}

	إذا (!داخلی_resistance_initialized && _params.n_cells > 0) {
		_RLS_est(0) = OCV_DEFAULT * _params.n_cells;
		_RLS_est(1) = R_DEFAULT * _params.n_cells;
		_estimation_covariance(0, 0) = OCV_COVARIANCE * _params.n_cells;
		_estimation_covariance(0, 1) = 0.f;
		_estimation_covariance(1, 0) = 0.f;
		_estimation_covariance(1, 1) = R_COVARIANCE * _params.n_cells;
		_estimation_covariance_norm = sqrtf(powf(_estimation_covariance(0, 0), 2.f) + 2.f * powf(_estimation_covariance(1, 0),
						    2.f) + powf(_estimation_covariance(1, 1), 2.f));
		داخلی_resistance_initialized = درست;
	}

	ModuleParams::updateParams();

	اولی_parameter_update = خطأ;
}

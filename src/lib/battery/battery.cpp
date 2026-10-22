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

#అన్తర్భావ "battery.h"
#అన్తర్భావ <mathlib/mathlib.h>
#అన్తర్భావ <cstring>
#అన్తర్భావ <px4_platform_common/defines.h>

ఉపయోగించి నామాకాశ సమయం_literals;
ఉపయోగించి నామాకాశ మాతృక;

Battery::Battery(పూర్ణాంకం సూచీ, ModuleParams *మాతృ, స్థిరరాశి పూర్ణాంకం sample_interval_us, స్థిరరాశి uint8_t ఆధారం) :
	ModuleParams(మాతృ),
	సూచీ_2(సూచీ < 1 || సూచీ > 9 ? 1 : సూచీ),
	ఆధారం_2(ఆధారం)
{
	స్థిరరాశి దశాంశం expected_filter_dt = static_cast<దశాంశం>(sample_interval_us) * 1e-6f;
	current_సరాసరి_filter_a.సమితి_Parameters(expected_filter_dt, 50.f);
	_ocv_filter_v.సమితి_Parameters(expected_filter_dt, 1.f);
	_cell_voltage_filter_v.సమితి_Parameters(expected_filter_dt, 1.f);

	ఒకవేళ (సూచీ > 9 || సూచీ < 1) {
		PX4_ERR("Battery index must be between 1 and 9 (inclusive). Received %d. Defaulting to 1.", సూచీ);
	}

	// 16 chars for parameter name + null terminator
	అక్షరం param_name[17];

	snprintf(param_name, పరిమాణం(param_name), "BAT%d_V_EMPTY", సూచీ_2);
	_param_handles.v_ఖాళీలు = param_కనుగొను(param_name);

	ఒకవేళ (_param_handles.v_ఖాళీలు == PARAM_INVALID) {
		PX4_ERR("Could not find parameter with name %s", param_name);
	}

	snprintf(param_name, పరిమాణం(param_name), "BAT%d_V_CHARGED", సూచీ_2);
	_param_handles.v_charged = param_కనుగొను(param_name);

	snprintf(param_name, పరిమాణం(param_name), "BAT%d_N_CELLS", సూచీ_2);
	_param_handles.n_cells = param_కనుగొను(param_name);

	snprintf(param_name, పరిమాణం(param_name), "BAT%d_CAPACITY", సూచీ_2);
	_param_handles.సామర్థ్యం = param_కనుగొను(param_name);

	snprintf(param_name, పరిమాణం(param_name), "BAT%d_R_INTERNAL", సూచీ_2);
	_param_handles.r_అంతర్గత = param_కనుగొను(param_name);

	snprintf(param_name, పరిమాణం(param_name), "BAT%d_SOURCE", సూచీ_2);
	_param_handles.ఆధారం = param_కనుగొను(param_name);

	snprintf(param_name, పరిమాణం(param_name), "BAT%d_I_OVERWRITE", సూచీ_2);
	_param_handles.i_overwrite = param_కనుగొను(param_name);

	_param_handles.దిగువ_thr = param_కనుగొను("BAT_LOW_THR");
	_param_handles.crit_thr = param_కనుగొను("BAT_CRIT_THR");
	_param_handles.emergen_thr = param_కనుగొను("BAT_EMERGEN_THR");

	_param_handles.bat_avrg_current = param_కనుగొను("BAT_AVRG_CURRENT");

	updateParams();
}

శూన్య Battery::updateVoltage(స్థిరరాశి దశాంశం voltage_v)
{
	_voltage_v = voltage_v;
}

శూన్య Battery::updateCurrent(స్థిరరాశి దశాంశం current_a)
{
	// Overwrite the measured current if current overwrite is defined and vehicle is unarmed
	ఒకవేళ (!_armed && _params.i_overwrite > FLT_EPSILON) {
		_current_a = _params.i_overwrite;

	} లేకపోతే {
		_current_a = current_a;
	}
}

శూన్య Battery::updateTemperature(స్థిరరాశి దశాంశం temperature_c)
{
	_temperature_c = temperature_c;
}

శూన్య Battery::update_Battery_స్థితి(స్థిరరాశి hrt_abstime &timestamp)
{
	updateDt(timestamp);

	// Require minimum voltage otherwise override connected status
	ఒకవేళ (_voltage_v < LITHIUM_BATTERY_RECOGNITION_VOLTAGE) {
		_connected = అబద్ధం;
	}

	ఒకవేళ (!_connected || (చివరి_unconnected_timestamp == 0)) {
		చివరి_unconnected_timestamp = timestamp;
	}

	// Wait with initializing filters to avoid relying on a voltage sample from the rising edge
	_battery_initialized = _connected && (timestamp > చివరి_unconnected_timestamp + 2_s);

	ఒకవేళ (_connected && !_battery_initialized && అంతర్గత_resistance_initialized && _params.n_cells > 0) {
		reset_అంతర్గత_Resistance_Estimation(_voltage_v, _current_a);
	}

	కూడిక_Discharged(_current_a);
	state_యొక్క_charge_volt_based =
		calculate_State_యొక్క_Charge_Voltage_Based(_voltage_v, _current_a);

	ఒకవేళ (!external_state_యొక్క_charge) {
		estimate_State_యొక్క_Charge();
	}

	computeScale();

	ఒకవేళ (_connected && _battery_initialized) {
		_warning = determineWarning(state_యొక్క_charge);
	}

	ఒకవేళ (vehicle_స్థితి_sub.updated()) {
		vehicle_స్థితి_s vehicle_స్థితి;

		ఒకవేళ (vehicle_స్థితి_sub.copy(&vehicle_స్థితి)) {
			_armed = (vehicle_స్థితి.arming_state == vehicle_స్థితి_s::ARMING_STATE_ARMED);
			vehicle_స్థితి_is_fw = (vehicle_స్థితి.vehicle_type == vehicle_స్థితి_s::VEHICLE_TYPE_FIXED_WING);
		}
	}
}

battery_స్థితి_s Battery::get_Battery_స్థితి()
{
	battery_స్థితి_s battery_స్థితి{};
	battery_స్థితి.voltage_v = _voltage_v;
	battery_స్థితి.current_a = _current_a;
	battery_స్థితి.current_సరాసరి_a = current_సరాసరి_filter_a.getState();
	battery_స్థితి.discharged_mah = _discharged_mah;
	battery_స్థితి.మిగిలిన = state_యొక్క_charge;
	battery_స్థితి.scale = _scale;
	battery_స్థితి.సమయం_మిగిలిన_s = compute_మిగిలిన_సమయం(_current_a);
	battery_స్థితి.temperature = _temperature_c;
	battery_స్థితి.cell_లెక్క = _params.n_cells;
	battery_స్థితి.connected = _connected;
	battery_స్థితి.ఆధారం = ఆధారం_2;
	battery_స్థితి.ప్రాధాన్యత = ప్రాధాన్యత_2;
	battery_స్థితి.సామర్థ్యం = static_cast<uint16_t>(సామర్థ్యం_mah);
	battery_స్థితి.id = static_cast<uint8_t>(సూచీ_2);
	battery_స్థితి.warning = _warning;
	battery_స్థితి.timestamp = hrt_absolute_సమయం();
	battery_స్థితి.దోషాలు = determine_దోషాలు();
	battery_స్థితి.మిగిలిన_సామర్థ్యం_wh = NAN; // not measured by dumb power module; smart-battery drivers overwrite
	battery_స్థితి.nominal_voltage = NAN;        // not measured by dumb power module; smart-battery drivers overwrite
	battery_స్థితి.అంతర్గత_resistance_estimate = అంతర్గత_resistance_estimate_2;
	battery_స్థితి.ocv_estimate = _voltage_v + అంతర్గత_resistance_estimate_2 * _params.n_cells * _current_a;
	battery_స్థితి.ocv_estimate_filtered = _ocv_filter_v.getState();
	battery_స్థితి.volt_based_soc_estimate = _params.n_cells > 0 ? math::interpolate(_ocv_filter_v.getState() /
			_params.n_cells,
			_params.v_ఖాళీలు, _params.v_charged, 0.f, 1.f) : -1.f;
	battery_స్థితి.voltage_prediction = _voltage_prediction;
	battery_స్థితి.prediction_error = _prediction_error;
	battery_స్థితి.estimation_covariance_norm = _estimation_covariance_norm;
	తిరిగి battery_స్థితి;
}

శూన్య Battery::publish_Battery_స్థితి(స్థిరరాశి battery_స్థితి_s &battery_స్థితి)
{
	ఒకవేళ (ఆధారం_2 == _params.ఆధారం) {
		battery_స్థితి_pub.publish(battery_స్థితి);
	}
}

శూన్య Battery::update_And_Publish_Battery_స్థితి(స్థిరరాశి hrt_abstime &timestamp)
{
	update_Battery_స్థితి(timestamp);

	battery_స్థితి_s battery_స్థితి = get_Battery_స్థితి();

	_failure_config.update();

	ఒకవేళ (!failure_injection::process_battery(_failure_config, battery_స్థితి.id, battery_స్థితి)) {
		తిరిగి;
	}

	publish_Battery_స్థితి(battery_స్థితి);
}
శూన్య Battery::updateDt(స్థిరరాశి hrt_abstime &timestamp)
{
	ఒకవేళ (చివరి_timestamp != 0) {
		// _dt_discharge is the true, unclamped time delta: for the coulomb
		// count in sumDischarged() below, using the real elapsed time is
		// always more accurate than clamping it, even across an unusually
		// long gap between updates (e.g. an update source that reports
		// less often than every 2 seconds).
		_dt_discharge = (timestamp - చివరి_timestamp) / 1e6f;

		// _dt is clamped to guard the numerical stability of the current
		// average filter below, which assumes a roughly steady sample
		// interval and should not see a single very large dt.
		_dt = math::min(_dt_discharge, 2.f);
	}

	చివరి_timestamp = timestamp;
}

దశాంశం Battery::కూడిక_Discharged(దశాంశం current_a)
{
	ఒకవేళ (_dt_discharge > FLT_EPSILON && fabsf(current_a + 1.f) > FLT_EPSILON) {
		// mAh since last loop: (current[A] * 1000 = [mA]) * (dt[s] / 3600 = [h])
		// current = -1 means invalid current measurement
		_discharged_mah_loop = (current_a * 1e3f) * (_dt_discharge / 3600.f);
		_discharged_mah += _discharged_mah_loop;
	}

	తిరిగి _discharged_mah;
}

దశాంశం Battery::calculate_State_యొక్క_Charge_Voltage_Based(స్థిరరాశి దశాంశం voltage_v, స్థిరరాశి దశాంశం current_a)
{
	ఒకవేళ (_params.n_cells == 0) {
		తిరిగి -1.0f;
	}

	// remaining battery capacity based on voltage
	దశాంశం cell_voltage = voltage_v / _params.n_cells;

	// correct battery voltage locally for load drop according to internal resistance and current
	ఒకవేళ (current_a > FLT_EPSILON) {
		update_అంతర్గత_Resistance_Estimation(voltage_v, current_a);

		ఒకవేళ (_params.r_అంతర్గత >= 0.f) { // Use user specified internal resistance value
			cell_voltage += _params.r_అంతర్గత * current_a;

		} లేకపోతే { // Use estimated internal resistance value
			cell_voltage += అంతర్గత_resistance_estimate_2 * current_a;
		}

	}

	_cell_voltage_filter_v.update(cell_voltage);
	తిరిగి math::interpolate(_cell_voltage_filter_v.getState(), _params.v_ఖాళీలు, _params.v_charged, 0.f, 1.f);
}

శూన్య Battery::update_అంతర్గత_Resistance_Estimation(స్థిరరాశి దశాంశం voltage_v, స్థిరరాశి దశాంశం current_a)
{
	Vector2f x{1, -current_a};
	_voltage_prediction = (x.transpose() * _RLS_est)(0, 0);
	_prediction_error = voltage_v - _voltage_prediction;
	స్థిరరాశి Vector2f gamma = _estimation_covariance * x / (LAMBDA + (x.transpose() * _estimation_covariance * x)(0, 0));
	స్థిరరాశి Vector2f RSL_est_తాత్కాలిక = _RLS_est + gamma * _prediction_error;
	స్థిరరాశి Matrix2f estimation_covariance_తాత్కాలిక = (_estimation_covariance
			- మాతృక_2<దశాంశం, 2, 1>(gamma) * (x.transpose() * _estimation_covariance)) / LAMBDA;
	స్థిరరాశి దశాంశం estimation_covariance_తాత్కాలిక_norm =
		sqrtf(powf(estimation_covariance_తాత్కాలిక(0, 0), 2.f)
		      + 2.f * powf(estimation_covariance_తాత్కాలిక(1, 0), 2.f)
		      + powf(estimation_covariance_తాత్కాలిక(1, 1), 2.f));

	ఒకవేళ (estimation_covariance_తాత్కాలిక_norm < _estimation_covariance_norm) { // Only update if estimation improves
		_RLS_est = RSL_est_తాత్కాలిక;
		_estimation_covariance = estimation_covariance_తాత్కాలిక;
		_estimation_covariance_norm = estimation_covariance_తాత్కాలిక_norm;
		అంతర్గత_resistance_estimate_2 =
			math::max(_RLS_est(1) / _params.n_cells, 0.f); // Only use positive values

	} లేకపోతే { // Update OCV estimate with IR estimate
		_RLS_est(0) = voltage_v + _RLS_est(1) * current_a;
	}

	_ocv_filter_v.update(voltage_v + అంతర్గత_resistance_estimate_2 * _params.n_cells * current_a);
}

శూన్య Battery::reset_అంతర్గత_Resistance_Estimation(స్థిరరాశి దశాంశం voltage_v, స్థిరరాశి దశాంశం current_a)
{
	_RLS_est(0) = voltage_v;
	_RLS_est(1) = R_DEFAULT * _params.n_cells;
	_estimation_covariance.సమితి_Zero();
	_estimation_covariance(0, 0) = OCV_COVARIANCE * _params.n_cells;
	_estimation_covariance(1, 1) = R_COVARIANCE * _params.n_cells;
	_estimation_covariance_norm = sqrtf(powf(_estimation_covariance(0, 0), 2.f) + 2.f * powf(_estimation_covariance(1, 0),
					    2.f) + powf(_estimation_covariance(1, 1), 2.f));
	అంతర్గత_resistance_estimate_2 = R_DEFAULT;
	_ocv_filter_v.reset(voltage_v + అంతర్గత_resistance_estimate_2 * _params.n_cells * current_a);

	ఒకవేళ (_params.r_అంతర్గత >= 0.f) { // Use user specified internal resistance value
		_cell_voltage_filter_v.reset(voltage_v / _params.n_cells + _params.r_అంతర్గత * current_a);

	} లేకపోతే { // Use estimated internal resistance value
		_cell_voltage_filter_v.reset(voltage_v / _params.n_cells + అంతర్గత_resistance_estimate_2 * current_a);
	}
}

శూన్య Battery::estimate_State_యొక్క_Charge()
{
	// choose which quantity we're using for final reporting
	ఒకవేళ ((సామర్థ్యం_mah > 0.f) && _battery_initialized) {
		// if battery capacity is known, fuse voltage measurement with used capacity
		// The lower the voltage the more adjust the estimate with it to avoid deep discharge
		స్థిరరాశి దశాంశం బరువు_v = 3e-2f * (1 - state_యొక్క_charge_volt_based);
		state_యొక్క_charge = (1 - బరువు_v) * state_యొక్క_charge + బరువు_v * state_యొక్క_charge_volt_based;
		// directly apply current capacity slope calculated using current
		state_యొక్క_charge -= _discharged_mah_loop / సామర్థ్యం_mah;
		state_యొక్క_charge = math::max(state_యొక్క_charge, 0.f);

		స్థిరరాశి దశాంశం state_యొక్క_charge_current_based = math::max(1.f - _discharged_mah / సామర్థ్యం_mah, 0.f);
		state_యొక్క_charge = math::min(state_యొక్క_charge_current_based, state_యొక్క_charge);

	} లేకపోతే {
		state_యొక్క_charge = state_యొక్క_charge_volt_based;
	}
}

uint8_t Battery::determineWarning(దశాంశం state_యొక్క_charge_2)
{
	ఒకవేళ (state_యొక్క_charge_2 < _params.emergen_thr) {
		తిరిగి battery_స్థితి_s::WARNING_EMERGENCY;

	} లేకపోతే ఒకవేళ (state_యొక్క_charge_2 < _params.crit_thr) {
		తిరిగి battery_స్థితి_s::WARNING_CRITICAL;

	} లేకపోతే ఒకవేళ (state_యొక్క_charge_2 < _params.దిగువ_thr) {
		తిరిగి battery_స్థితి_s::WARNING_దిగువ;

	} లేకపోతే {
		తిరిగి battery_స్థితి_s::WARNING_NONE;
	}
}

uint16_t Battery::determine_దోషాలు()
{
	uint16_t దోషాలు{0};

	ఒకవేళ ((_params.n_cells > 0)
	    && (_voltage_v > (_params.n_cells * _params.v_charged * 1.05f))) {
		// Reported as a "spike" since "over-voltage" does not exist in MAV_BATTERY_FAULT
		దోషాలు |= (1 << battery_స్థితి_s::FAULT_SPIKES);
	}

	ఒకవేళ (PX4_ISFINITE(_temperature_c) && _temperature_c > BAT_తాత్కాలిక_MAX) {
		దోషాలు |= (1 << battery_స్థితి_s::FAULT_OVER_TEMPERATURE);
	}

	తిరిగి దోషాలు;
}

శూన్య Battery::computeScale()
{
	స్థిరరాశి దశాంశం cell_voltage = _cell_voltage_filter_v.getState();

	ఒకవేళ (cell_voltage > FLT_EPSILON) {
		_scale = math::constrain(_params.v_charged / cell_voltage, 1.f, 1.3f); // Allow at most 30% compensation

	} లేకపోతే {
		_scale = 1.f;
	}
}

దశాంశం Battery::compute_మిగిలిన_సమయం(దశాంశం current_a)
{
	దశాంశం సమయం_మిగిలిన_s = NAN;

	_flight_phase_estimation_sub.update();

	తార్కిక reset_filter_transition_now = !vehicle_స్థితి_was_fw && vehicle_స్థితి_is_fw;

	// reset filter if not feasible, negative or we did a VTOL transition to FW mode
	ఒకవేళ (!PX4_ISFINITE(current_సరాసరి_filter_a.getState()) || current_సరాసరి_filter_a.getState() < FLT_EPSILON
	    || reset_filter_transition_now) {
		current_సరాసరి_filter_a.reset(_params.bat_avrg_current);
	}

	vehicle_స్థితి_was_fw = vehicle_స్థితి_is_fw;

	ఒకవేళ (_armed && PX4_ISFINITE(current_a)) {
		// For FW only update when we are in level flight
		ఒకవేళ (!vehicle_స్థితి_is_fw || ((hrt_absolute_సమయం() - _flight_phase_estimation_sub.get().timestamp) < 2_s
					       && _flight_phase_estimation_sub.get().flight_phase == flight_phase_estimation_s::FLIGHT_PHASE_LEVEL)) {
			ఒకవేళ (_dt > FLT_EPSILON) {
				current_సరాసరి_filter_a.update(fmaxf(current_a, 0.f), _dt);

			} లేకపోతే {
				current_సరాసరి_filter_a.update(fmaxf(current_a, 0.f));
			}
		}
	}

	// Remaining time estimation only possible with capacity
	ఒకవేళ (సామర్థ్యం_mah > 0.f) {
		స్థిరరాశి దశాంశం మిగిలిన_సామర్థ్యం_mah = state_యొక్క_charge * సామర్థ్యం_mah;
		స్థిరరాశి దశాంశం current_ma = fmaxf(current_సరాసరి_filter_a.getState() * 1e3f, FLT_EPSILON);
		సమయం_మిగిలిన_s = మిగిలిన_సామర్థ్యం_mah / current_ma * 3600.f;
	}

	తిరిగి సమయం_మిగిలిన_s;
}

శూన్య Battery::updateParams()
{
	స్థిరరాశి పూర్ణాంకం n_cells = మొదటి_parameter_update ? 0 : _params.n_cells;
	param_get(_param_handles.v_ఖాళీలు, &_params.v_ఖాళీలు);
	param_get(_param_handles.v_charged, &_params.v_charged);
	param_get(_param_handles.n_cells, &_params.n_cells);
	param_get(_param_handles.r_అంతర్గత, &_params.r_అంతర్గత);
	param_get(_param_handles.ఆధారం, &_params.ఆధారం);
	param_get(_param_handles.దిగువ_thr, &_params.దిగువ_thr);
	param_get(_param_handles.crit_thr, &_params.crit_thr);
	param_get(_param_handles.emergen_thr, &_params.emergen_thr);
	param_get(_param_handles.bat_avrg_current, &_params.bat_avrg_current);
	param_get(_param_handles.i_overwrite, &_params.i_overwrite);

	దశాంశం సామర్థ్యం{0.f};
	param_get(_param_handles.సామర్థ్యం, &సామర్థ్యం);
	సమితి_సామర్థ్యం_Mah(సామర్థ్యం);

	ఒకవేళ (n_cells != _params.n_cells) {
		అంతర్గత_resistance_initialized = అబద్ధం;
	}

	ఒకవేళ (!అంతర్గత_resistance_initialized && _params.n_cells > 0) {
		_RLS_est(0) = OCV_DEFAULT * _params.n_cells;
		_RLS_est(1) = R_DEFAULT * _params.n_cells;
		_estimation_covariance(0, 0) = OCV_COVARIANCE * _params.n_cells;
		_estimation_covariance(0, 1) = 0.f;
		_estimation_covariance(1, 0) = 0.f;
		_estimation_covariance(1, 1) = R_COVARIANCE * _params.n_cells;
		_estimation_covariance_norm = sqrtf(powf(_estimation_covariance(0, 0), 2.f) + 2.f * powf(_estimation_covariance(1, 0),
						    2.f) + powf(_estimation_covariance(1, 1), 2.f));
		అంతర్గత_resistance_initialized = నిజం;
	}

	ModuleParams::updateParams();

	మొదటి_parameter_update = అబద్ధం;
}

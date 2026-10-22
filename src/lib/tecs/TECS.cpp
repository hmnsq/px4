/****************************************************************************
 *
 *   Copyright (c) 2017-2023 PX4 Development Team. All rights reserved.
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
 * @file TECS.cpp
 *
 * @author Paul Riseborough
 */

#అన్తర్భావ "TECS.hpp"

#అన్తర్భావ <lib/geo/geo.h>

#అన్తర్భావ <px4_platform_common/defines.h>

ఉపయోగించి math::constrain;
ఉపయోగించి math::max;
ఉపయోగించి math::min;

స్థిర constexpr తార్కిక is_Valid_సమయం_అడుగు(దశాంశం dt) { తిరిగి (PX4_ISFINITE(dt) && dt > FLT_EPSILON);}
స్థిర constexpr తార్కిక fastDescendFullyEngaged(దశాంశం fast_descend) { తిరిగి 1.f - fast_descend < FLT_EPSILON; }

శూన్య TECSAirspeedFilter::initialize(స్థిరరాశి దశాంశం equivalent_airspeed, స్థిరరాశి దశాంశం equivalent_airspeed_trim,
				    స్థిరరాశి తార్కిక airspeed_sensor_అందుబాటు)
{
	ఒకవేళ (airspeed_sensor_అందుబాటు && PX4_ISFINITE(equivalent_airspeed)) {
		_airspeed_state.వేగం = equivalent_airspeed;

	} లేకపోతే {
		_airspeed_state.వేగం = equivalent_airspeed_trim;
	}

	_airspeed_state.వేగం_రేటు = 0.f;
}

శూన్య TECSAirspeedFilter::update(స్థిరరాశి దశాంశం dt, స్థిరరాశి ఆదానం_2 &ఆదానం, స్థిరరాశి Param &param,
				స్థిరరాశి తార్కిక airspeed_sensor_అందుబాటు)
{
	// Input checking
	ఒకవేళ (!is_Valid_సమయం_అడుగు(dt)) {
		// Do not update the states.
		తిరిగి;
	}

	దశాంశం airspeed;

	ఒకవేళ (PX4_ISFINITE(ఆదానం.equivalent_airspeed) && airspeed_sensor_అందుబాటు) {
		airspeed = ఆదానం.equivalent_airspeed;

	} లేకపోతే {
		airspeed = param.equivalent_airspeed_trim;
	}

	దశాంశం airspeed_derivative;

	ఒకవేళ (PX4_ISFINITE(ఆదానం.equivalent_airspeed_రేటు) && airspeed_sensor_అందుబాటు) {
		airspeed_derivative = ఆదానం.equivalent_airspeed_రేటు;

	} లేకపోతే {
		airspeed_derivative = 0.0f;
	}

	/* Filter airspeed and rate using a constant airspeed rate model in a steady state Kalman Filter.
	 We use the gains of the continuous Kalman filter Kc and approximate the discrete version Kalman gain Kd =dt*Kc,
	 since the continuous algebraic Riccatti equation is easier to solve.
	*/

	మాతృక::Vector2f new_state_predicted;

	new_state_predicted(0) = _airspeed_state.వేగం + dt * _airspeed_state.వేగం_రేటు;
	new_state_predicted(1) = _airspeed_state.వేగం_రేటు;

	స్థిరరాశి మాతృక::Vector2f innovation{(airspeed - new_state_predicted(0)), (airspeed_derivative - new_state_predicted(1))};

	మాతృక::Vector2f new_state;
	new_state(0) = new_state_predicted(0) + dt * (kKg00 * innovation(0) + kKg01 * innovation(1));
	new_state(1) = new_state_predicted(1) + dt * (kKg10 * innovation(0) + kKg11 * innovation(1));

	// Clip airspeed at zero
	ఒకవేళ (new_state(0) < FLT_EPSILON) {
		new_state(0) = 0.0f;
		// calculate input that would result in zero speed.
		స్థిరరాశి దశాంశం desired_airspeed_innovation = (-new_state_predicted(0) / dt - kKg01 * innovation(1)) / kKg00;
		new_state(1) = new_state_predicted(1) + dt * (kKg10 * desired_airspeed_innovation + kKg11 * innovation(1));
	}

	// Update states
	_airspeed_state.వేగం = new_state(0);
	_airspeed_state.వేగం_రేటు = new_state(1);
}

TECSAirspeedFilter::AirspeedFilterState TECSAirspeedFilter::getState() స్థిరరాశి
{
	తిరిగి _airspeed_state;
}

శూన్య TECSAltitudeReferenceModel::update(స్థిరరాశి దశాంశం dt, స్థిరరాశి AltitudeReferenceState &setpoint, దశాంశం altitude,
					దశాంశం ఎత్తు_రేటు, స్థిరరాశి Param &param)
{
	// Input checks
	ఒకవేళ (!is_Valid_సమయం_అడుగు(dt)) {
		// Do not update the states.
		తిరిగి;
	}

	స్థిరరాశి దశాంశం current_alt = PX4_ISFINITE(altitude) ? altitude : 0.f;

	_velocity_control_traj_generator.సమితి_Max_Jerk(JERK_MAX);
	_velocity_control_traj_generator.సమితి_Max_Accel_పైకి(param.vert_accel_హద్దు);
	_velocity_control_traj_generator.సమితి_Max_Accel_కిందికి(param.vert_accel_హద్దు);
	_velocity_control_traj_generator.సమితి_Max_Vel_పైకి(param.max_sink_రేటు); // different convention for FW than for MC
	_velocity_control_traj_generator.సమితి_Max_Vel_కిందికి(param.max_climb_రేటు); // different convention for FW than for MC

	// Altitude setpoint reference
	_alt_control_traj_generator.సమితి_Max_Jerk(JERK_MAX);
	_alt_control_traj_generator.సమితి_Max_Accel(param.vert_accel_హద్దు);
	_alt_control_traj_generator.సమితి_Max_Vel(fmaxf(param.max_climb_రేటు, param.max_sink_రేటు));

	// XXX: this is a bit risky.. .alt_rate here could be NAN (by interface design) - and is only ok to input to the
	// setVelSpFeedback() method because it calls the reset in the logic below when it is NAN.
	// TODO: stop it with the NAN interfaces, make sure to take care of this when refactoring and separating altitude
	// and height rate control loops.
	_velocity_control_traj_generator.సమితి_Vel_Sp_Feedback(setpoint.alt_రేటు);

	తార్కిక control_altitude = నిజం;
	దశాంశం altitude_setpoint = setpoint.alt;

	ఒకవేళ (!PX4_ISFINITE(setpoint.alt) && !PX4_ISFINITE(setpoint.alt_రేటు)) {
		// neither altitude nor altitude rate is set - reset to current altitude
		_velocity_control_traj_generator.reset(0.f, 0, current_alt);
		altitude_setpoint = current_alt;

	} లేకపోతే ఒకవేళ (PX4_ISFINITE(setpoint.alt_రేటు)) {
		// input is height rate (not altitude)
		_velocity_control_traj_generator.సమితి_Current_Position_Estimate(current_alt);
		_velocity_control_traj_generator.update(dt, setpoint.alt_రేటు);
		altitude_setpoint = _velocity_control_traj_generator.getCurrentPosition();
		control_altitude = PX4_ISFINITE(altitude_setpoint); // returns true if altitude is locked

	} లేకపోతే {
		// input is altitude
		_velocity_control_traj_generator.reset(0, ఎత్తు_రేటు, altitude_setpoint);
	}

	ఒకవేళ (control_altitude) {
		స్థిరరాశి దశాంశం లక్ష్యం_climbrate_m_s = math::min(param.లక్ష్యం_climbrate, param.max_climb_రేటు);
		స్థిరరాశి దశాంశం లక్ష్యం_sinkrate_m_s = math::min(param.లక్ష్యం_sinkrate, param.max_sink_రేటు);

		స్థిరరాశి దశాంశం delta_trajectory_గమ్యం_లక్ష్యం_m = altitude_setpoint - _alt_control_traj_generator.getCurrentPosition();

		దశాంశం ఎత్తు_రేటు_లక్ష్యం = math::signNoZero<దశాంశం>(delta_trajectory_గమ్యం_లక్ష్యం_m) *
					   math::trajectory::compute_Max_వేగం_From_దూరం(
						   JERK_MAX, param.vert_accel_హద్దు, fabsf(delta_trajectory_గమ్యం_లక్ష్యం_m), 0.f);

		ఎత్తు_రేటు_లక్ష్యం = math::constrain(ఎత్తు_రేటు_లక్ష్యం, -లక్ష్యం_sinkrate_m_s, లక్ష్యం_climbrate_m_s);

		_alt_control_traj_generator.updateDurations(ఎత్తు_రేటు_లక్ష్యం);
		_alt_control_traj_generator.updateTraj(dt);
		ఎత్తు_రేటు_setpoint_ప్రత్యక్ష = NAN;

	} లేకపోతే {
		_alt_control_traj_generator.సమితి_Current_Velocity(_velocity_control_traj_generator.getCurrentVelocity());
		_alt_control_traj_generator.సమితి_Current_Position(current_alt);
		ఎత్తు_రేటు_setpoint_ప్రత్యక్ష = _velocity_control_traj_generator.getCurrentVelocity();
	}
}

TECSAltitudeReferenceModel::AltitudeReferenceState TECSAltitudeReferenceModel::getAltitudeReference() స్థిరరాశి
{
	TECSAltitudeReferenceModel::AltitudeReferenceState ref{
		.alt = _alt_control_traj_generator.getCurrentPosition(),
		.alt_రేటు = _alt_control_traj_generator.getCurrentVelocity(),
	};

	తిరిగి ref;
}

శూన్య TECSAltitudeReferenceModel::initialize(స్థిరరాశి AltitudeReferenceState &state)
{
	స్థిరరాశి దశాంశం init_state_alt = PX4_ISFINITE(state.alt) ? state.alt : 0.f;
	స్థిరరాశి దశాంశం init_state_alt_రేటు = PX4_ISFINITE(state.alt_రేటు) ? state.alt_రేటు : 0.f;

	_alt_control_traj_generator.reset(0.0f, init_state_alt_రేటు, init_state_alt);
	_velocity_control_traj_generator.reset(0.f, init_state_alt_రేటు, init_state_alt);
}

శూన్య TECSControl::initialize(స్థిరరాశి Setpoint &setpoint, స్థిరరాశి ఆదానం_2 &ఆదానం, స్థిరరాశి Param &param, స్థిరరాశి Flag &flag)
{
	resetIntegrals();

	// Initialization always goes through the altitude loop, even if a direct height rate setpoint is given
	Setpoint altitude_loop_setpoint{setpoint};
	altitude_loop_setpoint.altitude_రేటు_setpoint_ప్రత్యక్ష = NAN;

	స్థిరరాశి STERate_హద్దు హద్దు_2{calculate_మొత్తం_Energy_రేటు_హద్దు(param)};
	స్థిరరాశి SpecificEnergyRates specific_energy_రేటు{_updateOuterLoops(altitude_loop_setpoint, ఆదానం, హద్దు_2, param, flag)};

	స్థిరరాశి SpecificEnergyWeighting బరువు{update_వేగం_Altitude_Weights(param, flag)};
	స్థిరరాశి SpecificEnergyRates pitch_specific_energy_రేటు{_calcPitchControlEnergyRates(setpoint, ఆదానం, హద్దు_2,
			specific_energy_రేటు, param, flag, బరువు)};
	Control_విలువలు seb_రేటు{calc_Pitch_Control_Seb_రేటు(బరువు, pitch_specific_energy_రేటు)};

	_pitch_setpoint = _calcPitchControlOutput(ఆదానం, seb_రేటు, param);

	ste_రేటు_estimate_filter.reset(specific_energy_రేటు.spe_రేటు.estimate + specific_energy_రేటు.ske_రేటు.estimate);

	Control_విలువలు ste_రేటు{calc_Throttle_Control_Ste_రేటు(హద్దు_2, specific_energy_రేటు, param)};

	_throttle_setpoint = _calcThrottleControlOutput(హద్దు_2, ste_రేటు, param, flag);

	// Debug output
	_debug_output.మొత్తం_energy_రేటు_estimate = ste_రేటు.estimate;
	_debug_output.మొత్తం_energy_రేటు_sp = ste_రేటు.setpoint;
	_debug_output.throttle_integrator = _throttle_integ_state;
	_debug_output.energy_balance_రేటు_estimate = seb_రేటు.estimate;
	_debug_output.energy_balance_రేటు_sp = seb_రేటు.setpoint;
	_debug_output.pitch_integrator = _pitch_integ_state;
}

శూన్య TECSControl::update(స్థిరరాశి దశాంశం dt, స్థిరరాశి Setpoint &setpoint, స్థిరరాశి ఆదానం_2 &ఆదానం, స్థిరరాశి Param &param,
			 స్థిరరాశి Flag &flag)
{
	// Input checking
	ఒకవేళ (!is_Valid_సమయం_అడుగు(dt)) {
		// Do not update the states and output.
		తిరిగి;
	}

	స్థిరరాశి STERate_హద్దు హద్దు_2{calculate_మొత్తం_Energy_రేటు_హద్దు(param)};
	స్థిరరాశి SpecificEnergyRates specific_energy_రేటు{_updateOuterLoops(setpoint, ఆదానం, హద్దు_2, param, flag)};

	_calcPitchControl(dt, setpoint, ఆదానం, హద్దు_2, specific_energy_రేటు, param, flag);

	_calcThrottleControl(dt, హద్దు_2, specific_energy_రేటు, param, flag);
}

TECSControl::SpecificEnergyRates TECSControl::_updateOuterLoops(స్థిరరాశి Setpoint &setpoint, స్థిరరాశి ఆదానం_2 &ఆదానం,
		స్థిరరాశి STERate_హద్దు &హద్దు_2, స్థిరరాశి Param &param, స్థిరరాశి Flag &flag)
{
	OuterLoopSetpoints control_setpoint;

	control_setpoint.tas_రేటు_setpoint = _calcAirspeedControlOutput(setpoint, ఆదానం, హద్దు_2, param, flag);

	ఒకవేళ (PX4_ISFINITE(setpoint.altitude_రేటు_setpoint_ప్రత్యక్ష)) {
		// direct height rate control
		control_setpoint.altitude_రేటు_setpoint = setpoint.altitude_రేటు_setpoint_ప్రత్యక్ష;

	} లేకపోతే {
		// altitude is locked, go through altitude outer loop
		control_setpoint.altitude_రేటు_setpoint = _calcAltitudeControlOutput(setpoint, ఆదానం, param);
	}

	స్థిరరాశి SpecificEnergyRates specific_energy_రేటు{_calcSpecificEnergyRates(control_setpoint, ఆదానం)};

	_detectUnderspeed(ఆదానం, param, flag);

	_debug_output.altitude_రేటు_control = control_setpoint.altitude_రేటు_setpoint;
	_debug_output.true_airspeed_derivative_control = control_setpoint.tas_రేటు_setpoint;

	తిరిగి specific_energy_రేటు;
}

TECSControl::STERate_హద్దు TECSControl::calculate_మొత్తం_Energy_రేటు_హద్దు(స్థిరరాశి Param &param) స్థిరరాశి
{
	TECSControl::STERate_హద్దు హద్దు_2;
	// Calculate the specific total energy rate limits from the max throttle limits
	హద్దు_2.STE_రేటు_max = math::max(param.max_climb_రేటు, FLT_EPSILON) * CONSTANTS_ONE_G;
	హద్దు_2.STE_రేటు_min = - math::max(param.min_sink_రేటు, FLT_EPSILON) * CONSTANTS_ONE_G;

	తిరిగి హద్దు_2;
}

దశాంశం TECSControl::_calcAirspeedControlOutput(స్థిరరాశి Setpoint &setpoint, స్థిరరాశి ఆదానం_2 &ఆదానం, స్థిరరాశి STERate_హద్దు &హద్దు_2,
		స్థిరరాశి Param &param, స్థిరరాశి Flag &flag) స్థిరరాశి
{
	దశాంశం airspeed_రేటు_output{0.0f};

	// calculate the demanded true airspeed rate of change based on first order response of true airspeed error
	// if airspeed measurement is not enabled then always set the rate setpoint to zero in order to avoid constant rate setpoints
	ఒకవేళ (flag.airspeed_enabled) {
		// Calculate limits for the demanded rate of change of speed based on physical performance limits
		// with a 50% margin to allow the total energy controller to correct for errors. Increase it in case of fast descend
		స్థిరరాశి దశాంశం max_tas_రేటు_sp = (param.fast_descend * 0.5f + 0.5f) * హద్దు_2.STE_రేటు_max / math::max(ఆదానం.tas,
					      FLT_EPSILON);
		స్థిరరాశి దశాంశం min_tas_రేటు_sp = (param.fast_descend * 0.5f + 0.5f) * హద్దు_2.STE_రేటు_min / math::max(ఆదానం.tas,
					      FLT_EPSILON);
		airspeed_రేటు_output = constrain((setpoint.tas_setpoint - ఆదానం.tas) * param.airspeed_error_gain, min_tas_రేటు_sp,
						 max_tas_రేటు_sp);
	}

	తిరిగి airspeed_రేటు_output;
}

దశాంశం TECSControl::_calcAltitudeControlOutput(స్థిరరాశి Setpoint &setpoint, స్థిరరాశి ఆదానం_2 &ఆదానం, స్థిరరాశి Param &param) స్థిరరాశి
{
	దశాంశం altitude_రేటు_output;
	altitude_రేటు_output = (setpoint.altitude_reference.alt - ఆదానం.altitude) * param.altitude_error_gain
			       + param.altitude_setpoint_gain_ff * setpoint.altitude_reference.alt_రేటు;

	altitude_రేటు_output = math::constrain(altitude_రేటు_output, -param.max_sink_రేటు, param.max_climb_రేటు);

	తిరిగి altitude_రేటు_output;
}

TECSControl::SpecificEnergyRates TECSControl::_calcSpecificEnergyRates(స్థిరరాశి OuterLoopSetpoints &control_setpoint,
		స్థిరరాశి ఆదానం_2 &ఆదానం) స్థిరరాశి
{
	SpecificEnergyRates specific_energy_rates;
	// Calculate specific energy rate demands in units of (m**2/sec**3)
	specific_energy_rates.spe_రేటు.setpoint = control_setpoint.altitude_రేటు_setpoint *
			CONSTANTS_ONE_G; // potential energy rate of change

	// d/dt(0.5 V^2) = V * dV/dt: use measured TAS, not the TAS setpoint, to keep the airspeed loop linear
	specific_energy_rates.ske_రేటు.setpoint = ఆదానం.tas *
			control_setpoint.tas_రేటు_setpoint; // kinetic energy rate of change

	// Calculate specific energy rates in units of (m**2/sec**3)
	specific_energy_rates.spe_రేటు.estimate = ఆదానం.altitude_రేటు * CONSTANTS_ONE_G; // potential energy rate of change
	specific_energy_rates.ske_రేటు.estimate = ఆదానం.tas * ఆదానం.tas_రేటు;// kinetic energy rate of change

	తిరిగి specific_energy_rates;
}

శూన్య TECSControl::_detectUnderspeed(స్థిరరాశి ఆదానం_2 &ఆదానం, స్థిరరాశి Param &param, స్థిరరాశి Flag &flag)
{
	ఒకవేళ (!flag.detect_underspeed_enabled || !flag.airspeed_enabled) {
		నిష్పత్తి_undersped = 0.0f;
		తిరిగి;
	}

	// Ramp underspeed ratio from 0 at tas_min to 1 at 90% of tas_min, or halfway to tas_stall if that is higher
	స్థిరరాశి దశాంశం tas_fully_undersped = math::min(param.tas_min,
					  math::max(0.9f * param.tas_min, 0.5f * (param.tas_min + param.tas_stall)));
	స్థిరరాశి దశాంశం underspeed_ramp_వెడల్పు = param.tas_min - tas_fully_undersped;

	నిష్పత్తి_undersped = math::constrain((param.tas_min - ఆదానం.tas) / math::max(underspeed_ramp_వెడల్పు, FLT_EPSILON),
					   0.0f, 1.0f);
}

TECSControl::SpecificEnergyWeighting TECSControl::update_వేగం_Altitude_Weights(స్థిరరాశి Param &param, స్థిరరాశి Flag &flag)
{

	SpecificEnergyWeighting బరువు;
	// Calculate the weight applied to control of specific kinetic energy error
	దశాంశం pitch_వేగం_బరువు = constrain(param.pitch_వేగం_బరువు, 0.0f, 2.0f);

	// Underspeed or fast descend: interpolate speed weight from nominal
	// towards max. We take the maximum of the two - this keeps the speed
	// weight continuous, even when underspeed and fast descend are combined
	స్థిరరాశి దశాంశం max_నిష్పత్తి = fmaxf(నిష్పత్తి_undersped, param.fast_descend);
	pitch_వేగం_బరువు = max_నిష్పత్తి * 2.0f + (1.0f - max_నిష్పత్తి) * pitch_వేగం_బరువు;

	ఒకవేళ (!flag.airspeed_enabled) {
		pitch_వేగం_బరువు = 0.0f;
	}

	బరువు.spe_weighting = constrain(2.0f - pitch_వేగం_బరువు, 0.f, 2.f);
	బరువు.ske_weighting = constrain(pitch_వేగం_బరువు, 0.f, 2.f);

	తిరిగి బరువు;
}

దశాంశం TECSControl::calc_Pitch_Control_Ske_రేటు_Setpoint(స్థిరరాశి Setpoint &setpoint, స్థిరరాశి ఆదానం_2 &ఆదానం,
		స్థిరరాశి STERate_హద్దు &హద్దు_2, స్థిరరాశి Param &param, స్థిరరాశి Flag &flag, స్థిరరాశి SpecificEnergyWeighting &బరువు) స్థిరరాశి
{
	// Trading altitude for speed with pitch is only wanted up to trim airspeed. Above trim, accelerating is left to
	// the throttle loop: if it saturates, the vehicle settles at a lower airspeed instead of below the altitude setpoint.
	// The airspeed setpoint seen by the pitch loop is therefore capped at max(trim, current airspeed). Between trim
	// and the setpoint the pitch loop holds altitude, while an overspeed above the setpoint is still corrected.
	// With a speed weight of 2 (speed priority, e.g. gliders, fast descend or underspeed) pitch is the only way to
	// track the setpoint, so the cap is faded out as the weight goes from 1 to 2.
	స్థిరరాశి దశాంశం tas_setpoint_capped = min(setpoint.tas_setpoint, max(param.tas_trim, ఆదానం.tas));

	Setpoint pitch_control_setpoint{setpoint};
	pitch_control_setpoint.tas_setpoint = lerp(tas_setpoint_capped, setpoint.tas_setpoint,
					      constrain(బరువు.ske_weighting - 1.f, 0.f, 1.f));

	// d/dt(0.5 V^2) = V * dV/dt: the measured airspeed, not the setpoint, keeps the airspeed loop linear
	తిరిగి ఆదానం.tas * _calcAirspeedControlOutput(pitch_control_setpoint, ఆదానం, హద్దు_2, param, flag);
}

TECSControl::SpecificEnergyRates TECSControl::_calcPitchControlEnergyRates(స్థిరరాశి Setpoint &setpoint,
		స్థిరరాశి ఆదానం_2 &ఆదానం, స్థిరరాశి STERate_హద్దు &హద్దు_2, స్థిరరాశి SpecificEnergyRates &specific_energy_rates,
		స్థిరరాశి Param &param, స్థిరరాశి Flag &flag, స్థిరరాశి SpecificEnergyWeighting &బరువు) స్థిరరాశి
{
	SpecificEnergyRates pitch_specific_energy_rates{specific_energy_rates};
	pitch_specific_energy_rates.ske_రేటు.setpoint = calc_Pitch_Control_Ske_రేటు_Setpoint(setpoint, ఆదానం, హద్దు_2, param, flag,
			బరువు);

	// Between trim airspeed and the airspeed setpoint the throttle alone accelerates the vehicle. The capped setpoint
	// above already removes the kinetic setpoint of the pitch loop there, faded in with speed priority. But the
	// throttle's acceleration would still show up in the kinetic energy rate estimate, and the pitch loop would pitch
	// up to turn it into height. Remove the estimate from the pitch loop in this band as well, with the same fade, so
	// that pitch controls height only. The setpoint must not be scaled again, or it would be faded twice.
	స్థిరరాశి తార్కిక throttle_accelerates = ఆదానం.tas > param.tas_trim && ఆదానం.tas < setpoint.tas_setpoint;
	స్థిరరాశి దశాంశం kinetic_estimate_gain = throttle_accelerates ? constrain(బరువు.ske_weighting - 1.f, 0.f, 1.f) : 1.f;
	pitch_specific_energy_rates.ske_రేటు.estimate *= kinetic_estimate_gain;

	తిరిగి pitch_specific_energy_rates;
}

శూన్య TECSControl::_calcPitchControl(దశాంశం dt, స్థిరరాశి Setpoint &setpoint, స్థిరరాశి ఆదానం_2 &ఆదానం, స్థిరరాశి STERate_హద్దు &హద్దు_2,
				    స్థిరరాశి SpecificEnergyRates &specific_energy_rates, స్థిరరాశి Param &param, స్థిరరాశి Flag &flag)
{
	స్థిరరాశి SpecificEnergyWeighting బరువు{update_వేగం_Altitude_Weights(param, flag)};

	స్థిరరాశి SpecificEnergyRates pitch_specific_energy_rates{_calcPitchControlEnergyRates(setpoint, ఆదానం, హద్దు_2,
			specific_energy_rates, param, flag, బరువు)};

	Control_విలువలు seb_రేటు{calc_Pitch_Control_Seb_రేటు(బరువు, pitch_specific_energy_rates)};

	_calcPitchControlUpdate(dt, ఆదానం, seb_రేటు, param);
	స్థిరరాశి దశాంశం pitch_setpoint{_calcPitchControlOutput(ఆదానం, seb_రేటు, param)};

	// Comply with the specified vertical acceleration limit by applying a pitch rate limit
	// NOTE: at zero airspeed, the pitch increment is unbounded
	స్థిరరాశి దశాంశం pitch_increment = dt * param.vert_accel_హద్దు / math::max(ఆదానం.tas, FLT_EPSILON);
	_pitch_setpoint = constrain(pitch_setpoint, _pitch_setpoint - pitch_increment,
				    _pitch_setpoint + pitch_increment);
	_pitch_setpoint = constrain(_pitch_setpoint, param.pitch_min, param.pitch_max);

	//Debug Output
	_debug_output.energy_balance_రేటు_estimate = seb_రేటు.estimate;
	_debug_output.energy_balance_రేటు_sp = seb_రేటు.setpoint;
	_debug_output.pitch_integrator = _pitch_integ_state;
}

TECSControl::Control_విలువలు TECSControl::calc_Pitch_Control_Seb_రేటు(స్థిరరాశి SpecificEnergyWeighting &బరువు,
		స్థిరరాశి SpecificEnergyRates &specific_energy_rates) స్థిరరాశి
{
	Control_విలువలు seb_రేటు;
	/*
	 * The SKE_weighting variable controls how speed and altitude control are prioritized by the pitch demand calculation.
	 * A weighting of 1 gives equal speed and altitude priority
	 * A weighting of 0 gives 100% priority to altitude control and must be used when no airspeed measurement is available.
	 * A weighting of 2 provides 100% priority to speed control and is used when:
	 * a) an underspeed condition is detected.
	 * b) during climbout where a minimum pitch angle has been set to ensure altitude is gained. If the airspeed
	 * rises above the demanded value, the pitch angle demand is increased by the TECS controller to prevent the vehicle overspeeding.
	 * The weighting can be adjusted between 0 and 2 depending on speed and altitude accuracy requirements.
	*/
	seb_రేటు.setpoint = specific_energy_rates.spe_రేటు.setpoint * బరువు.spe_weighting -
			    specific_energy_rates.ske_రేటు.setpoint *
			    బరువు.ske_weighting;

	seb_రేటు.estimate = (specific_energy_rates.spe_రేటు.estimate * బరువు.spe_weighting) -
			    (specific_energy_rates.ske_రేటు.estimate * బరువు.ske_weighting);

	తిరిగి seb_రేటు;
}

శూన్య TECSControl::_calcPitchControlUpdate(దశాంశం dt, స్థిరరాశి ఆదానం_2 &ఆదానం, స్థిరరాశి Control_విలువలు &seb_రేటు,
		స్థిరరాశి Param &param)
{

	// In fast descend, the pitch integrator converges to the (substantially
	// negative) pitch. Ramp it down with decreasing fast descend ratio,
	// such that it is zero when the ratio becomes 0 again.
	ఒకవేళ (param.fast_descend < _fast_descend_prev && _fast_descend_prev > FLT_EPSILON) {
		_pitch_integ_state *= param.fast_descend / _fast_descend_prev;
	}

	_fast_descend_prev = param.fast_descend;

	ఒకవేళ (param.integrator_gain_pitch > FLT_EPSILON) {

		స్థిరరాశి దశాంశం climb_angle_గమ్యం_SEB_రేటు = climb_Angle_గమ్యం_Seb_రేటు(ఆదానం, param);

		// Calculate pitch integrator input term
		దశాంశం pitch_integ_ఆదానం = _getControlError(seb_రేటు) * param.integrator_gain_pitch / climb_angle_గమ్యం_SEB_రేటు;

		// Guard against NaN integrator input - discard bad input sample (e.g., exiting offboard velocity mode)
		ఒకవేళ (!PX4_ISFINITE(pitch_integ_ఆదానం)) {
			pitch_integ_ఆదానం = 0.f;
		}

		// Prevent the integrator changing in a direction that will increase pitch demand saturation
		ఒకవేళ (_pitch_setpoint >= param.pitch_max) {
			pitch_integ_ఆదానం = min(pitch_integ_ఆదానం, 0.f);

		} లేకపోతే ఒకవేళ (_pitch_setpoint <= param.pitch_min) {
			pitch_integ_ఆదానం = max(pitch_integ_ఆదానం, 0.f);
		}

		// Update the pitch integrator state
		_pitch_integ_state = _pitch_integ_state + pitch_integ_ఆదానం * dt;

		// Safety check: reset integrator if state became corrupted
		ఒకవేళ (!PX4_ISFINITE(_pitch_integ_state)) {
			_pitch_integ_state = 0.f;
		}

	} లేకపోతే {
		_pitch_integ_state = 0.0f;
	}
}

దశాంశం TECSControl::climb_Angle_గమ్యం_Seb_రేటు(స్థిరరాశి ఆదానం_2 &ఆదానం, స్థిరరాశి Param &param)
{
	// max() returns its second argument for a NaN first one, so a NaN airspeed falls back to the minimum airspeed
	తిరిగి max(ఆదానం.tas, param.tas_min, FLT_EPSILON) * CONSTANTS_ONE_G;
}

దశాంశం TECSControl::_calcPitchControlOutput(స్థిరరాశి ఆదానం_2 &ఆదానం, స్థిరరాశి Control_విలువలు &seb_రేటు, స్థిరరాశి Param &param) స్థిరరాశి
{
	// Calculate a specific energy correction that doesn't include the integrator contribution
	దశాంశం SEB_రేటు_correction = _getControlError(seb_రేటు) * param.pitch_damping_gain +
				    param.seb_రేటు_ff *
				    seb_రేటు.setpoint;

	// Convert the specific energy balance rate correction to a climb angle and add the level-flight pitch, the angle
	// of attack at zero climb angle, to get the absolute pitch setpoint. This calculation assumes:
	// a) The climb angle follows pitch angle with a lag that is small enough not to destabilise the control loop.
	// b) The offset between climb angle and pitch angle (angle of attack) is constant and equal to the level-flight
	// pitch, excluding the effect of pitch transients due to control action or turbulence.
	దశాంశం pitch_setpoint_unc = SEB_రేటు_correction / climb_Angle_గమ్యం_Seb_రేటు(ఆదానం, param) + _pitch_integ_state
				   + param.level_flight_pitch;

	// Guard against a non-finite feedforward/damping contribution (e.g. seb_rate going NaN on the energy/speed
	// side when exiting offboard velocity mode). constrain() does not reject NaN, so fall back to the integrator
	// state, which is kept finite by _calcPitchControlUpdate, to avoid latching a NaN pitch demand.
	ఒకవేళ (!PX4_ISFINITE(pitch_setpoint_unc)) {
		pitch_setpoint_unc = _pitch_integ_state + param.level_flight_pitch;
	}

	తిరిగి constrain(pitch_setpoint_unc, param.pitch_min, param.pitch_max);
}

శూన్య TECSControl::_calcThrottleControl(దశాంశం dt, స్థిరరాశి STERate_హద్దు &హద్దు_2,
				       స్థిరరాశి SpecificEnergyRates &specific_energy_rates, స్థిరరాశి Param &param, స్థిరరాశి Flag &flag)
{
	// Update STE rate estimate LP filter
	స్థిరరాశి దశాంశం STE_రేటు_estimate_raw = specific_energy_rates.spe_రేటు.estimate + specific_energy_rates.ske_రేటు.estimate;
	ste_రేటు_estimate_filter.సమితి_Parameters(dt, param.ste_రేటు_సమయం_const);
	ste_రేటు_estimate_filter.update(STE_రేటు_estimate_raw);
	Control_విలువలు ste_రేటు{calc_Throttle_Control_Ste_రేటు(హద్దు_2, specific_energy_rates, param)};
	// During fast descend, we control airspeed over the pitch control loop. Give minimal thrust as soon as we are descending
	దశాంశం throttle_setpoint{param.throttle_min};

	ఒకవేళ (!fastDescendFullyEngaged(param.fast_descend)) {
		_calcThrottleControlUpdate(dt, హద్దు_2, ste_రేటు, param, flag);
		throttle_setpoint = (1.f - param.fast_descend) * _calcThrottleControlOutput(హద్దు_2, ste_రేటు, param,
				    flag) + param.fast_descend * param.throttle_min;
	}

	// Rate limit the throttle demand
	ఒకవేళ (fabsf(param.throttle_slewrate) > FLT_EPSILON) {
		స్థిరరాశి దశాంశం throttle_increment_హద్దు = dt * (param.throttle_max - param.throttle_min) * param.throttle_slewrate;
		throttle_setpoint = constrain(throttle_setpoint, _throttle_setpoint - throttle_increment_హద్దు,
					      _throttle_setpoint + throttle_increment_హద్దు);
	}

	_throttle_setpoint = constrain(throttle_setpoint, param.throttle_min, param.throttle_max);

	// Debug output
	_debug_output.మొత్తం_energy_రేటు_estimate = ste_రేటు.estimate;
	_debug_output.మొత్తం_energy_రేటు_sp = ste_రేటు.setpoint;
	_debug_output.throttle_integrator = _throttle_integ_state;
}

TECSControl::Control_విలువలు TECSControl::calc_Throttle_Control_Ste_రేటు(స్థిరరాశి STERate_హద్దు &హద్దు_2,
		స్థిరరాశి SpecificEnergyRates &specific_energy_rates,
		స్థిరరాశి Param &param) స్థిరరాశి
{
	// Output ste rate values
	Control_విలువలు ste_రేటు;
	ste_రేటు.setpoint = specific_energy_rates.spe_రేటు.setpoint + specific_energy_rates.ske_రేటు.setpoint;

	// Adjust the demanded total energy rate to compensate for induced drag rise in turns.
	// Assume induced drag scales linearly with normal load factor.
	// The additional normal load factor is given by (1/cos(bank angle) - 1)
	ste_రేటు.setpoint += param.ఎక్కించు_గుణకం_correction * (param.ఎక్కించు_గుణకం - 1.f);

	ste_రేటు.setpoint = constrain(ste_రేటు.setpoint, హద్దు_2.STE_రేటు_min, హద్దు_2.STE_రేటు_max);
	ste_రేటు.estimate = ste_రేటు_estimate_filter.getState();

	తిరిగి ste_రేటు;
}

శూన్య TECSControl::_calcThrottleControlUpdate(దశాంశం dt, స్థిరరాశి STERate_హద్దు &హద్దు_2, స్థిరరాశి Control_విలువలు &ste_రేటు,
		స్థిరరాశి Param &param, స్థిరరాశి Flag &flag)
{
	స్థిరరాశి దశాంశం STE_రేటు_గమ్యం_throttle = ste_రేటు_గమ్యం_Throttle(హద్దు_2);

	// Integral handling
	ఒకవేళ (flag.airspeed_enabled) {
		ఒకవేళ (param.integrator_gain_throttle > FLT_EPSILON) {
			// underspeed conditions zero out integration
			దశాంశం throttle_integ_ఆదానం = (_getControlError(ste_రేటు) * param.integrator_gain_throttle) * dt *
						     STE_రేటు_గమ్యం_throttle * (1.0f - నిష్పత్తి_undersped);

			// only allow integrator propagation into direction which unsaturates throttle
			ఒకవేళ (_throttle_setpoint >= param.throttle_max) {
				throttle_integ_ఆదానం = math::min(0.f, throttle_integ_ఆదానం);

			}

			ఒకవేళ (_throttle_setpoint <= param.throttle_min) {
				throttle_integ_ఆదానం = math::max(0.f, throttle_integ_ఆదానం);
			}

			// Calculate a throttle demand from the integrated total energy rate error
			// This will be added to the total throttle demand to compensate for steady state errors
			_throttle_integ_state = PX4_ISFINITE(throttle_integ_ఆదానం) ? _throttle_integ_state + throttle_integ_ఆదానం : 0.f;

		} లేకపోతే {
			_throttle_integ_state = 0.0f;
		}

	}
}

దశాంశం TECSControl::_calcThrottleControlOutput(స్థిరరాశి STERate_హద్దు &హద్దు_2, స్థిరరాశి Control_విలువలు &ste_రేటు,
		స్థిరరాశి Param &param,
		స్థిరరాశి Flag &flag) స్థిరరాశి
{
	స్థిరరాశి దశాంశం STE_రేటు_గమ్యం_throttle = ste_రేటు_గమ్యం_Throttle(హద్దు_2);

	// Calculate a predicted throttle from the demanded rate of change of energy, using the cruise throttle
	// as the starting point. Assume:
	// Specific total energy rate = _STE_rate_max is achieved when throttle is set to _throttle_setpoint_max
	// Specific total energy rate = 0 at cruise throttle
	// Specific total energy rate = _STE_rate_min is achieved when throttle is set to _throttle_setpoint_min

	// assume airspeed and density-independent delta_throttle to sink/climb rate mapping
	// TODO: include air density for thrust mappings
	స్థిరరాశి దశాంశం throttle_above_trim_చొప్పున_ste_రేటు = (param.throttle_max - param.throttle_trim) / హద్దు_2.STE_రేటు_max;
	స్థిరరాశి దశాంశం throttle_క్రింద_trim_చొప్పున_ste_రేటు = (param.throttle_trim - param.throttle_min) / హద్దు_2.STE_రేటు_min;

	దశాంశం throttle_predicted = 0.0f;

	ఒకవేళ (ste_రేటు.setpoint >= FLT_EPSILON) {
		// throttle is between trim and maximum
		throttle_predicted = param.throttle_trim + ste_రేటు.setpoint * throttle_above_trim_చొప్పున_ste_రేటు;

	} లేకపోతే {
		// throttle is between trim and minimum
		throttle_predicted = param.throttle_trim - ste_రేటు.setpoint * throttle_క్రింద_trim_చొప్పున_ste_రేటు;

	}

	// Add proportional and derivative control feedback to the predicted throttle and constrain to throttle limits
	దశాంశం throttle_setpoint = (_getControlError(ste_రేటు) * param.throttle_damping_gain) * STE_రేటు_గమ్యం_throttle +
				  throttle_predicted;

	ఒకవేళ (flag.airspeed_enabled) {
		// Add the integrator feedback during closed loop operation with an airspeed sensor
		throttle_setpoint += _throttle_integ_state;

	} లేకపోతే {
		/* We want to avoid reducing the throttle output when switching from airspeed enabled mode into airspeedless mode.
		  Thus, if the throttle integrator has a positive value, add it still to the throttle setpoint. */
		throttle_setpoint += math::max(0.0f, _throttle_integ_state);
	}


	// ramp in max throttle setting with underspeediness value
	throttle_setpoint = నిష్పత్తి_undersped * param.throttle_max + (1.0f - నిష్పత్తి_undersped) * throttle_setpoint;

	తిరిగి constrain(throttle_setpoint, param.throttle_min, param.throttle_max);
}

శూన్య TECSControl::resetIntegrals()
{
	_pitch_integ_state = 0.0f;
	_throttle_integ_state = 0.0f;
}

శూన్య TECS::initControlParams(స్థిరరాశి ఆదానం_2 &ఆదానం)
{
	// Update parameters from input
	// Reference model
	_reference_param.లక్ష్యం_climbrate = ఆదానం.లక్ష్యం_climbrate;
	_reference_param.లక్ష్యం_sinkrate = ఆదానం.లక్ష్యం_sinkrate;
	// Control
	_control_param.tas_min = ఆదానం.eas_గమ్యం_tas * _equivalent_airspeed_min;
	_control_param.tas_stall = ఆదానం.eas_గమ్యం_tas * _equivalent_airspeed_stall;
	_control_param.tas_max = ఆదానం.eas_గమ్యం_tas * _equivalent_airspeed_max;
	_control_param.tas_trim = ఆదానం.eas_గమ్యం_tas * _control_param.equivalent_airspeed_trim;
	_control_param.pitch_max = ఆదానం.pitch_max;
	_control_param.pitch_min = ఆదానం.pitch_min;
	_control_param.throttle_trim = ఆదానం.throttle_trim;
	_control_param.throttle_max = ఆదానం.throttle_max;
	_control_param.throttle_min = ఆదానం.throttle_min;
}

దశాంశం TECS::calcTrueAirspeedSetpoint(దశాంశం eas_గమ్యం_tas, దశాంశం eas_setpoint)
{
	తిరిగి lerp(eas_గమ్యం_tas * eas_setpoint, _control_param.tas_max, _fast_descend);
}

శూన్య TECS::initialize(స్థిరరాశి ఆదానం_2 &ఆదానం)
{
	// Init subclasses
	TECSAltitudeReferenceModel::AltitudeReferenceState current_state{.alt = ఆదానం.altitude,
			.alt_రేటు = ఆదానం.altitude_రేటు};
	_altitude_reference_model.initialize(current_state);
	_airspeed_filter.initialize(ఆదానం.equivalent_airspeed, _airspeed_filter_param.equivalent_airspeed_trim,
				    _control_flag.airspeed_enabled);

	// Seed the controller from the filter, which falls back to trim airspeed without a valid measurement,
	// so the controller sees the same airspeed on reset as on every following update
	స్థిరరాశి దశాంశం true_airspeed = ఆదానం.eas_గమ్యం_tas * _airspeed_filter.getState().వేగం;

	TECSControl::Setpoint control_setpoint;
	control_setpoint.altitude_reference = _altitude_reference_model.getAltitudeReference();
	control_setpoint.altitude_రేటు_setpoint_ప్రత్యక్ష = NAN; // initialization always goes through the altitude loop
	control_setpoint.tas_setpoint = true_airspeed;

	స్థిరరాశి TECSControl::ఆదానం_2 control_ఆదానం{ .altitude = ఆదానం.altitude,
						.altitude_రేటు = ఆదానం.altitude_రేటు,
						.tas = true_airspeed,
						.tas_రేటు = 0.0f};

	_control.initialize(control_setpoint, control_ఆదానం, _control_param, _control_flag);

	_fast_descend = 0.f;
	_enabled_fast_descend_timestamp = 0U;
}

శూన్య TECS::update(స్థిరరాశి ఆదానం_2 &ఆదానం)
{
	// Calculate the time since last update (seconds)
	స్థిరరాశి hrt_abstime now = hrt_absolute_సమయం();
	స్థిరరాశి దశాంశం dt = static_cast<దశాంశం>(now - _update_timestamp) * 1e-6f;

	initControlParams(ఆదానం);

	ఒకవేళ (dt < DT_MIN) {
		// Update interval too small, do not update. Assume constant states/output in this case.
		తిరిగి;
	}

	దశాంశం true_airspeed_setpoint;

	ఒకవేళ (dt > DT_MAX || _update_timestamp == 0UL) {
		// Update time interval too large, can't guarantee sanity of state updates anymore. reset the control loop.
		initialize(ఆదానం);
		true_airspeed_setpoint = calcTrueAirspeedSetpoint(ఆదానం.eas_గమ్యం_tas, ఆదానం.equivalent_airspeed_setpoint);

	} లేకపోతే {
		/* Check if we want to fast descend. On fast descend, we set the throttle to min, and use the altitude control
		loop to control the speed to the maximum airspeed. */
		సమితి_Fast_Descend(ఆదానం.altitude_setpoint, ఆదానం.altitude);
		_control_param.fast_descend = _fast_descend;

		// Update airspeedfilter submodule
		స్థిరరాశి TECSAirspeedFilter::ఆదానం_2 airspeed_ఆదానం{ .equivalent_airspeed = ఆదానం.equivalent_airspeed,
				.equivalent_airspeed_రేటు = ఆదానం.వేగం_deriv_forward / ఆదానం.eas_గమ్యం_tas};

		_airspeed_filter.update(dt, airspeed_ఆదానం, _airspeed_filter_param, _control_flag.airspeed_enabled);

		// Update Reference model submodule
		ఒకవేళ (fastDescendFullyEngaged(_fast_descend)) {
			// Reset the altitude reference model, while we are in fast descend.
			స్థిరరాశి TECSAltitudeReferenceModel::AltitudeReferenceState init_state{
				.alt = ఆదానం.altitude,
				.alt_రేటు = ఆదానం.altitude_రేటు};
			_altitude_reference_model.initialize(init_state);

		} లేకపోతే ఒకవేళ (_control.get_Underspeed_నిష్పత్తి() > 0.5f) {
			// Reset altitude reference during underspeed to prevent altitude error accumulation
			// that would cause aggressive pitch-up (and potential secondary stall) on recovery.
			స్థిరరాశి TECSAltitudeReferenceModel::AltitudeReferenceState init_state{
				.alt = ఆదానం.altitude,
				.alt_రేటు = ఆదానం.altitude_రేటు};
			_altitude_reference_model.initialize(init_state);

		} లేకపోతే {
			స్థిరరాశి TECSAltitudeReferenceModel::AltitudeReferenceState setpoint{ .alt = ఆదానం.altitude_setpoint,
					.alt_రేటు = ఆదానం.altitude_రేటు_setpoint};

			_altitude_reference_model.update(dt, setpoint, ఆదానం.altitude, ఆదానం.altitude_రేటు, _reference_param);
		}

		TECSControl::Setpoint control_setpoint;
		control_setpoint.altitude_reference = _altitude_reference_model.getAltitudeReference();
		control_setpoint.altitude_రేటు_setpoint_ప్రత్యక్ష = _altitude_reference_model.get_ఎత్తు_రేటు_Setpoint_ప్రత్యక్ష();
		true_airspeed_setpoint = calcTrueAirspeedSetpoint(ఆదానం.eas_గమ్యం_tas, ఆదానం.equivalent_airspeed_setpoint);
		control_setpoint.tas_setpoint = true_airspeed_setpoint;

		స్థిరరాశి TECSAirspeedFilter::AirspeedFilterState airspeed_state{_airspeed_filter.getState()};
		స్థిరరాశి TECSControl::ఆదానం_2 control_ఆదానం{ .altitude = ఆదానం.altitude,
							.altitude_రేటు = ఆదానం.altitude_రేటు,
							.tas = ఆదానం.eas_గమ్యం_tas * airspeed_state.వేగం,
							.tas_రేటు = ఆదానం.eas_గమ్యం_tas * airspeed_state.వేగం_రేటు};

		_control.update(dt, control_setpoint, control_ఆదానం, _control_param, _control_flag);
	}

	debug_స్థితి.control = _control.getDebugOutput();
	debug_స్థితి.true_airspeed_sp = true_airspeed_setpoint;
	స్థిరరాశి TECSAirspeedFilter::AirspeedFilterState airspeed_state{_airspeed_filter.getState()};
	స్థిరరాశి TECSAltitudeReferenceModel::AltitudeReferenceState altitude_reference{_altitude_reference_model.getAltitudeReference()};
	debug_స్థితి.true_airspeed_filtered = ఆదానం.eas_గమ్యం_tas * airspeed_state.వేగం;
	debug_స్థితి.true_airspeed_derivative = ఆదానం.eas_గమ్యం_tas * airspeed_state.వేగం_రేటు;
	debug_స్థితి.altitude_reference = altitude_reference.alt;
	debug_స్థితి.ఎత్తు_రేటు_reference = altitude_reference.alt_రేటు;
	debug_స్థితి.ఎత్తు_రేటు_ప్రత్యక్ష = _altitude_reference_model.get_ఎత్తు_రేటు_Setpoint_ప్రత్యక్ష();
	debug_స్థితి.fast_descend = _fast_descend;

	_update_timestamp = now;
}

శూన్య TECS::సమితి_Fast_Descend(స్థిరరాశి దశాంశం alt_setpoint, స్థిరరాశి దశాంశం alt)
{
	// disable fast descend if we are close to the target altitude or the altitude setpoint is not finite

	ఒకవేళ (PX4_ISFINITE(alt_setpoint) && _control_flag.airspeed_enabled && (_fast_descend_alt_err > FLT_EPSILON)
	    && ((alt_setpoint + _fast_descend_alt_err) < alt)) {
		స్వయంచాలక now = hrt_absolute_సమయం();

		ఒకవేళ (_enabled_fast_descend_timestamp == 0U) {
			_enabled_fast_descend_timestamp = now;
		}

		_fast_descend = constrain(max(_fast_descend, static_cast<దశాంశం>(now - _enabled_fast_descend_timestamp) /
					      static_cast<దశాంశం>(FAST_DESCEND_RAMP_పైకి_సమయం)), 0.f, 1.f);

	} లేకపోతే ఒకవేళ (PX4_ISFINITE(alt_setpoint) && (_fast_descend > FLT_EPSILON) && (_fast_descend_alt_err > FLT_EPSILON)) {
		// We're in fast descend, scale it down until FAST_DESCEND_HEIGHT_MARGIN above target altitude
		_fast_descend = constrain((alt - alt_setpoint - FAST_DESCEND_ఎత్తు_MARGIN) / _fast_descend_alt_err, 0.f, 1.f);
		_enabled_fast_descend_timestamp = 0U;

	} లేకపోతే {
		_fast_descend = 0.f;
		_enabled_fast_descend_timestamp = 0U;
	}
}

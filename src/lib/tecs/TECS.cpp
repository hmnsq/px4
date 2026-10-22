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

#ਅਨ੍ਤਰ੍ਭਾਵ "TECS.hpp"

#ਅਨ੍ਤਰ੍ਭਾਵ <lib/geo/geo.h>

#ਅਨ੍ਤਰ੍ਭਾਵ <px4_platform_common/defines.h>

ਵਰਤੋਂ math::constrain;
ਵਰਤੋਂ math::max;
ਵਰਤੋਂ math::min;

ਸਥਿਰ constexpr ਤਾਰਕਿਕ is_Valid_ਸਮਾਂ_ਕਦਮ(ਦਸ਼ਮਲਵ dt) { ਵਾਪਸ (PX4_ISFINITE(dt) && dt > FLT_EPSILON);}
ਸਥਿਰ constexpr ਤਾਰਕਿਕ fastDescendFullyEngaged(ਦਸ਼ਮਲਵ fast_descend) { ਵਾਪਸ 1.f - fast_descend < FLT_EPSILON; }

ਖਾਲੀ TECSAirspeedFilter::initialize(ਅਚਲ ਦਸ਼ਮਲਵ equivalent_airspeed, ਅਚਲ ਦਸ਼ਮਲਵ equivalent_airspeed_trim,
				    ਅਚਲ ਤਾਰਕਿਕ airspeed_sensor_ਉਪਲਬਧ)
{
	ਜੇ (airspeed_sensor_ਉਪਲਬਧ && PX4_ISFINITE(equivalent_airspeed)) {
		_airspeed_state.ਰਫਤਾਰ = equivalent_airspeed;

	} ਨਹੀਂਤਾਂ {
		_airspeed_state.ਰਫਤਾਰ = equivalent_airspeed_trim;
	}

	_airspeed_state.ਰਫਤਾਰ_ਦਰ = 0.f;
}

ਖਾਲੀ TECSAirspeedFilter::update(ਅਚਲ ਦਸ਼ਮਲਵ dt, ਅਚਲ ਨਿਵੇਸ਼_2 &ਨਿਵੇਸ਼, ਅਚਲ Param &param,
				ਅਚਲ ਤਾਰਕਿਕ airspeed_sensor_ਉਪਲਬਧ)
{
	// Input checking
	ਜੇ (!is_Valid_ਸਮਾਂ_ਕਦਮ(dt)) {
		// Do not update the states.
		ਵਾਪਸ;
	}

	ਦਸ਼ਮਲਵ airspeed;

	ਜੇ (PX4_ISFINITE(ਨਿਵੇਸ਼.equivalent_airspeed) && airspeed_sensor_ਉਪਲਬਧ) {
		airspeed = ਨਿਵੇਸ਼.equivalent_airspeed;

	} ਨਹੀਂਤਾਂ {
		airspeed = param.equivalent_airspeed_trim;
	}

	ਦਸ਼ਮਲਵ airspeed_derivative;

	ਜੇ (PX4_ISFINITE(ਨਿਵੇਸ਼.equivalent_airspeed_ਦਰ) && airspeed_sensor_ਉਪਲਬਧ) {
		airspeed_derivative = ਨਿਵੇਸ਼.equivalent_airspeed_ਦਰ;

	} ਨਹੀਂਤਾਂ {
		airspeed_derivative = 0.0f;
	}

	/* Filter airspeed and rate using a constant airspeed rate model in a steady state Kalman Filter.
	 We use the gains of the continuous Kalman filter Kc and approximate the discrete version Kalman gain Kd =dt*Kc,
	 since the continuous algebraic Riccatti equation is easier to solve.
	*/

	ਮੈਟ੍ਰਿਕਸ::Vector2f new_state_predicted;

	new_state_predicted(0) = _airspeed_state.ਰਫਤਾਰ + dt * _airspeed_state.ਰਫਤਾਰ_ਦਰ;
	new_state_predicted(1) = _airspeed_state.ਰਫਤਾਰ_ਦਰ;

	ਅਚਲ ਮੈਟ੍ਰਿਕਸ::Vector2f innovation{(airspeed - new_state_predicted(0)), (airspeed_derivative - new_state_predicted(1))};

	ਮੈਟ੍ਰਿਕਸ::Vector2f new_state;
	new_state(0) = new_state_predicted(0) + dt * (kKg00 * innovation(0) + kKg01 * innovation(1));
	new_state(1) = new_state_predicted(1) + dt * (kKg10 * innovation(0) + kKg11 * innovation(1));

	// Clip airspeed at zero
	ਜੇ (new_state(0) < FLT_EPSILON) {
		new_state(0) = 0.0f;
		// calculate input that would result in zero speed.
		ਅਚਲ ਦਸ਼ਮਲਵ desired_airspeed_innovation = (-new_state_predicted(0) / dt - kKg01 * innovation(1)) / kKg00;
		new_state(1) = new_state_predicted(1) + dt * (kKg10 * desired_airspeed_innovation + kKg11 * innovation(1));
	}

	// Update states
	_airspeed_state.ਰਫਤਾਰ = new_state(0);
	_airspeed_state.ਰਫਤਾਰ_ਦਰ = new_state(1);
}

TECSAirspeedFilter::AirspeedFilterState TECSAirspeedFilter::getState() ਅਚਲ
{
	ਵਾਪਸ _airspeed_state;
}

ਖਾਲੀ TECSAltitudeReferenceModel::update(ਅਚਲ ਦਸ਼ਮਲਵ dt, ਅਚਲ AltitudeReferenceState &setpoint, ਦਸ਼ਮਲਵ altitude,
					ਦਸ਼ਮਲਵ ਉਚਾਈ_ਦਰ, ਅਚਲ Param &param)
{
	// Input checks
	ਜੇ (!is_Valid_ਸਮਾਂ_ਕਦਮ(dt)) {
		// Do not update the states.
		ਵਾਪਸ;
	}

	ਅਚਲ ਦਸ਼ਮਲਵ current_alt = PX4_ISFINITE(altitude) ? altitude : 0.f;

	_velocity_control_traj_generator.ਸੈੱਟ_Max_Jerk(JERK_MAX);
	_velocity_control_traj_generator.ਸੈੱਟ_Max_Accel_ਉੱਪਰ(param.vert_accel_ਹੱਦਬੰਦੀ);
	_velocity_control_traj_generator.ਸੈੱਟ_Max_Accel_ਥੱਲੇ(param.vert_accel_ਹੱਦਬੰਦੀ);
	_velocity_control_traj_generator.ਸੈੱਟ_Max_Vel_ਉੱਪਰ(param.max_sink_ਦਰ); // different convention for FW than for MC
	_velocity_control_traj_generator.ਸੈੱਟ_Max_Vel_ਥੱਲੇ(param.max_climb_ਦਰ); // different convention for FW than for MC

	// Altitude setpoint reference
	_alt_control_traj_generator.ਸੈੱਟ_Max_Jerk(JERK_MAX);
	_alt_control_traj_generator.ਸੈੱਟ_Max_Accel(param.vert_accel_ਹੱਦਬੰਦੀ);
	_alt_control_traj_generator.ਸੈੱਟ_Max_Vel(fmaxf(param.max_climb_ਦਰ, param.max_sink_ਦਰ));

	// XXX: this is a bit risky.. .alt_rate here could be NAN (by interface design) - and is only ok to input to the
	// setVelSpFeedback() method because it calls the reset in the logic below when it is NAN.
	// TODO: stop it with the NAN interfaces, make sure to take care of this when refactoring and separating altitude
	// and height rate control loops.
	_velocity_control_traj_generator.ਸੈੱਟ_Vel_Sp_Feedback(setpoint.alt_ਦਰ);

	ਤਾਰਕਿਕ control_altitude = ਸੱਚ;
	ਦਸ਼ਮਲਵ altitude_setpoint = setpoint.alt;

	ਜੇ (!PX4_ISFINITE(setpoint.alt) && !PX4_ISFINITE(setpoint.alt_ਦਰ)) {
		// neither altitude nor altitude rate is set - reset to current altitude
		_velocity_control_traj_generator.reset(0.f, 0, current_alt);
		altitude_setpoint = current_alt;

	} ਨਹੀਂਤਾਂ ਜੇ (PX4_ISFINITE(setpoint.alt_ਦਰ)) {
		// input is height rate (not altitude)
		_velocity_control_traj_generator.ਸੈੱਟ_Current_Position_Estimate(current_alt);
		_velocity_control_traj_generator.update(dt, setpoint.alt_ਦਰ);
		altitude_setpoint = _velocity_control_traj_generator.getCurrentPosition();
		control_altitude = PX4_ISFINITE(altitude_setpoint); // returns true if altitude is locked

	} ਨਹੀਂਤਾਂ {
		// input is altitude
		_velocity_control_traj_generator.reset(0, ਉਚਾਈ_ਦਰ, altitude_setpoint);
	}

	ਜੇ (control_altitude) {
		ਅਚਲ ਦਸ਼ਮਲਵ ਨਿਸ਼ਾਨਾ_climbrate_m_s = math::min(param.ਨਿਸ਼ਾਨਾ_climbrate, param.max_climb_ਦਰ);
		ਅਚਲ ਦਸ਼ਮਲਵ ਨਿਸ਼ਾਨਾ_sinkrate_m_s = math::min(param.ਨਿਸ਼ਾਨਾ_sinkrate, param.max_sink_ਦਰ);

		ਅਚਲ ਦਸ਼ਮਲਵ delta_trajectory_ਮੰਜ਼ਿਲ_ਨਿਸ਼ਾਨਾ_m = altitude_setpoint - _alt_control_traj_generator.getCurrentPosition();

		ਦਸ਼ਮਲਵ ਉਚਾਈ_ਦਰ_ਨਿਸ਼ਾਨਾ = math::signNoZero<ਦਸ਼ਮਲਵ>(delta_trajectory_ਮੰਜ਼ਿਲ_ਨਿਸ਼ਾਨਾ_m) *
					   math::trajectory::compute_Max_ਰਫਤਾਰ_From_ਦੂਰੀ(
						   JERK_MAX, param.vert_accel_ਹੱਦਬੰਦੀ, fabsf(delta_trajectory_ਮੰਜ਼ਿਲ_ਨਿਸ਼ਾਨਾ_m), 0.f);

		ਉਚਾਈ_ਦਰ_ਨਿਸ਼ਾਨਾ = math::constrain(ਉਚਾਈ_ਦਰ_ਨਿਸ਼ਾਨਾ, -ਨਿਸ਼ਾਨਾ_sinkrate_m_s, ਨਿਸ਼ਾਨਾ_climbrate_m_s);

		_alt_control_traj_generator.updateDurations(ਉਚਾਈ_ਦਰ_ਨਿਸ਼ਾਨਾ);
		_alt_control_traj_generator.updateTraj(dt);
		ਉਚਾਈ_ਦਰ_setpoint_ਸਿੱਧਾ = NAN;

	} ਨਹੀਂਤਾਂ {
		_alt_control_traj_generator.ਸੈੱਟ_Current_Velocity(_velocity_control_traj_generator.getCurrentVelocity());
		_alt_control_traj_generator.ਸੈੱਟ_Current_Position(current_alt);
		ਉਚਾਈ_ਦਰ_setpoint_ਸਿੱਧਾ = _velocity_control_traj_generator.getCurrentVelocity();
	}
}

TECSAltitudeReferenceModel::AltitudeReferenceState TECSAltitudeReferenceModel::getAltitudeReference() ਅਚਲ
{
	TECSAltitudeReferenceModel::AltitudeReferenceState ref{
		.alt = _alt_control_traj_generator.getCurrentPosition(),
		.alt_ਦਰ = _alt_control_traj_generator.getCurrentVelocity(),
	};

	ਵਾਪਸ ref;
}

ਖਾਲੀ TECSAltitudeReferenceModel::initialize(ਅਚਲ AltitudeReferenceState &state)
{
	ਅਚਲ ਦਸ਼ਮਲਵ init_state_alt = PX4_ISFINITE(state.alt) ? state.alt : 0.f;
	ਅਚਲ ਦਸ਼ਮਲਵ init_state_alt_ਦਰ = PX4_ISFINITE(state.alt_ਦਰ) ? state.alt_ਦਰ : 0.f;

	_alt_control_traj_generator.reset(0.0f, init_state_alt_ਦਰ, init_state_alt);
	_velocity_control_traj_generator.reset(0.f, init_state_alt_ਦਰ, init_state_alt);
}

ਖਾਲੀ TECSControl::initialize(ਅਚਲ Setpoint &setpoint, ਅਚਲ ਨਿਵੇਸ਼_2 &ਨਿਵੇਸ਼, ਅਚਲ Param &param, ਅਚਲ Flag &flag)
{
	resetIntegrals();

	// Initialization always goes through the altitude loop, even if a direct height rate setpoint is given
	Setpoint altitude_loop_setpoint{setpoint};
	altitude_loop_setpoint.altitude_ਦਰ_setpoint_ਸਿੱਧਾ = NAN;

	ਅਚਲ STERate_ਹੱਦਬੰਦੀ ਹੱਦਬੰਦੀ_2{calculate_ਕੁੱਲ_Energy_ਦਰ_ਹੱਦਬੰਦੀ(param)};
	ਅਚਲ SpecificEnergyRates specific_energy_ਦਰ{_updateOuterLoops(altitude_loop_setpoint, ਨਿਵੇਸ਼, ਹੱਦਬੰਦੀ_2, param, flag)};

	ਅਚਲ SpecificEnergyWeighting ਭਾਰ{update_ਰਫਤਾਰ_Altitude_Weights(param, flag)};
	ਅਚਲ SpecificEnergyRates pitch_specific_energy_ਦਰ{_calcPitchControlEnergyRates(setpoint, ਨਿਵੇਸ਼, ਹੱਦਬੰਦੀ_2,
			specific_energy_ਦਰ, param, flag, ਭਾਰ)};
	Control_ਮੁੱਲਾਂ seb_ਦਰ{calc_Pitch_Control_Seb_ਦਰ(ਭਾਰ, pitch_specific_energy_ਦਰ)};

	_pitch_setpoint = _calcPitchControlOutput(ਨਿਵੇਸ਼, seb_ਦਰ, param);

	ste_ਦਰ_estimate_filter.reset(specific_energy_ਦਰ.spe_ਦਰ.estimate + specific_energy_ਦਰ.ske_ਦਰ.estimate);

	Control_ਮੁੱਲਾਂ ste_ਦਰ{calc_Throttle_Control_Ste_ਦਰ(ਹੱਦਬੰਦੀ_2, specific_energy_ਦਰ, param)};

	_throttle_setpoint = _calcThrottleControlOutput(ਹੱਦਬੰਦੀ_2, ste_ਦਰ, param, flag);

	// Debug output
	_debug_output.ਕੁੱਲ_energy_ਦਰ_estimate = ste_ਦਰ.estimate;
	_debug_output.ਕੁੱਲ_energy_ਦਰ_sp = ste_ਦਰ.setpoint;
	_debug_output.throttle_integrator = _throttle_integ_state;
	_debug_output.energy_balance_ਦਰ_estimate = seb_ਦਰ.estimate;
	_debug_output.energy_balance_ਦਰ_sp = seb_ਦਰ.setpoint;
	_debug_output.pitch_integrator = _pitch_integ_state;
}

ਖਾਲੀ TECSControl::update(ਅਚਲ ਦਸ਼ਮਲਵ dt, ਅਚਲ Setpoint &setpoint, ਅਚਲ ਨਿਵੇਸ਼_2 &ਨਿਵੇਸ਼, ਅਚਲ Param &param,
			 ਅਚਲ Flag &flag)
{
	// Input checking
	ਜੇ (!is_Valid_ਸਮਾਂ_ਕਦਮ(dt)) {
		// Do not update the states and output.
		ਵਾਪਸ;
	}

	ਅਚਲ STERate_ਹੱਦਬੰਦੀ ਹੱਦਬੰਦੀ_2{calculate_ਕੁੱਲ_Energy_ਦਰ_ਹੱਦਬੰਦੀ(param)};
	ਅਚਲ SpecificEnergyRates specific_energy_ਦਰ{_updateOuterLoops(setpoint, ਨਿਵੇਸ਼, ਹੱਦਬੰਦੀ_2, param, flag)};

	_calcPitchControl(dt, setpoint, ਨਿਵੇਸ਼, ਹੱਦਬੰਦੀ_2, specific_energy_ਦਰ, param, flag);

	_calcThrottleControl(dt, ਹੱਦਬੰਦੀ_2, specific_energy_ਦਰ, param, flag);
}

TECSControl::SpecificEnergyRates TECSControl::_updateOuterLoops(ਅਚਲ Setpoint &setpoint, ਅਚਲ ਨਿਵੇਸ਼_2 &ਨਿਵੇਸ਼,
		ਅਚਲ STERate_ਹੱਦਬੰਦੀ &ਹੱਦਬੰਦੀ_2, ਅਚਲ Param &param, ਅਚਲ Flag &flag)
{
	OuterLoopSetpoints control_setpoint;

	control_setpoint.tas_ਦਰ_setpoint = _calcAirspeedControlOutput(setpoint, ਨਿਵੇਸ਼, ਹੱਦਬੰਦੀ_2, param, flag);

	ਜੇ (PX4_ISFINITE(setpoint.altitude_ਦਰ_setpoint_ਸਿੱਧਾ)) {
		// direct height rate control
		control_setpoint.altitude_ਦਰ_setpoint = setpoint.altitude_ਦਰ_setpoint_ਸਿੱਧਾ;

	} ਨਹੀਂਤਾਂ {
		// altitude is locked, go through altitude outer loop
		control_setpoint.altitude_ਦਰ_setpoint = _calcAltitudeControlOutput(setpoint, ਨਿਵੇਸ਼, param);
	}

	ਅਚਲ SpecificEnergyRates specific_energy_ਦਰ{_calcSpecificEnergyRates(control_setpoint, ਨਿਵੇਸ਼)};

	_detectUnderspeed(ਨਿਵੇਸ਼, param, flag);

	_debug_output.altitude_ਦਰ_control = control_setpoint.altitude_ਦਰ_setpoint;
	_debug_output.true_airspeed_derivative_control = control_setpoint.tas_ਦਰ_setpoint;

	ਵਾਪਸ specific_energy_ਦਰ;
}

TECSControl::STERate_ਹੱਦਬੰਦੀ TECSControl::calculate_ਕੁੱਲ_Energy_ਦਰ_ਹੱਦਬੰਦੀ(ਅਚਲ Param &param) ਅਚਲ
{
	TECSControl::STERate_ਹੱਦਬੰਦੀ ਹੱਦਬੰਦੀ_2;
	// Calculate the specific total energy rate limits from the max throttle limits
	ਹੱਦਬੰਦੀ_2.STE_ਦਰ_max = math::max(param.max_climb_ਦਰ, FLT_EPSILON) * CONSTANTS_ONE_G;
	ਹੱਦਬੰਦੀ_2.STE_ਦਰ_min = - math::max(param.min_sink_ਦਰ, FLT_EPSILON) * CONSTANTS_ONE_G;

	ਵਾਪਸ ਹੱਦਬੰਦੀ_2;
}

ਦਸ਼ਮਲਵ TECSControl::_calcAirspeedControlOutput(ਅਚਲ Setpoint &setpoint, ਅਚਲ ਨਿਵੇਸ਼_2 &ਨਿਵੇਸ਼, ਅਚਲ STERate_ਹੱਦਬੰਦੀ &ਹੱਦਬੰਦੀ_2,
		ਅਚਲ Param &param, ਅਚਲ Flag &flag) ਅਚਲ
{
	ਦਸ਼ਮਲਵ airspeed_ਦਰ_output{0.0f};

	// calculate the demanded true airspeed rate of change based on first order response of true airspeed error
	// if airspeed measurement is not enabled then always set the rate setpoint to zero in order to avoid constant rate setpoints
	ਜੇ (flag.airspeed_enabled) {
		// Calculate limits for the demanded rate of change of speed based on physical performance limits
		// with a 50% margin to allow the total energy controller to correct for errors. Increase it in case of fast descend
		ਅਚਲ ਦਸ਼ਮਲਵ max_tas_ਦਰ_sp = (param.fast_descend * 0.5f + 0.5f) * ਹੱਦਬੰਦੀ_2.STE_ਦਰ_max / math::max(ਨਿਵੇਸ਼.tas,
					      FLT_EPSILON);
		ਅਚਲ ਦਸ਼ਮਲਵ min_tas_ਦਰ_sp = (param.fast_descend * 0.5f + 0.5f) * ਹੱਦਬੰਦੀ_2.STE_ਦਰ_min / math::max(ਨਿਵੇਸ਼.tas,
					      FLT_EPSILON);
		airspeed_ਦਰ_output = constrain((setpoint.tas_setpoint - ਨਿਵੇਸ਼.tas) * param.airspeed_error_gain, min_tas_ਦਰ_sp,
						 max_tas_ਦਰ_sp);
	}

	ਵਾਪਸ airspeed_ਦਰ_output;
}

ਦਸ਼ਮਲਵ TECSControl::_calcAltitudeControlOutput(ਅਚਲ Setpoint &setpoint, ਅਚਲ ਨਿਵੇਸ਼_2 &ਨਿਵੇਸ਼, ਅਚਲ Param &param) ਅਚਲ
{
	ਦਸ਼ਮਲਵ altitude_ਦਰ_output;
	altitude_ਦਰ_output = (setpoint.altitude_reference.alt - ਨਿਵੇਸ਼.altitude) * param.altitude_error_gain
			       + param.altitude_setpoint_gain_ff * setpoint.altitude_reference.alt_ਦਰ;

	altitude_ਦਰ_output = math::constrain(altitude_ਦਰ_output, -param.max_sink_ਦਰ, param.max_climb_ਦਰ);

	ਵਾਪਸ altitude_ਦਰ_output;
}

TECSControl::SpecificEnergyRates TECSControl::_calcSpecificEnergyRates(ਅਚਲ OuterLoopSetpoints &control_setpoint,
		ਅਚਲ ਨਿਵੇਸ਼_2 &ਨਿਵੇਸ਼) ਅਚਲ
{
	SpecificEnergyRates specific_energy_rates;
	// Calculate specific energy rate demands in units of (m**2/sec**3)
	specific_energy_rates.spe_ਦਰ.setpoint = control_setpoint.altitude_ਦਰ_setpoint *
			CONSTANTS_ONE_G; // potential energy rate of change

	// d/dt(0.5 V^2) = V * dV/dt: use measured TAS, not the TAS setpoint, to keep the airspeed loop linear
	specific_energy_rates.ske_ਦਰ.setpoint = ਨਿਵੇਸ਼.tas *
			control_setpoint.tas_ਦਰ_setpoint; // kinetic energy rate of change

	// Calculate specific energy rates in units of (m**2/sec**3)
	specific_energy_rates.spe_ਦਰ.estimate = ਨਿਵੇਸ਼.altitude_ਦਰ * CONSTANTS_ONE_G; // potential energy rate of change
	specific_energy_rates.ske_ਦਰ.estimate = ਨਿਵੇਸ਼.tas * ਨਿਵੇਸ਼.tas_ਦਰ;// kinetic energy rate of change

	ਵਾਪਸ specific_energy_rates;
}

ਖਾਲੀ TECSControl::_detectUnderspeed(ਅਚਲ ਨਿਵੇਸ਼_2 &ਨਿਵੇਸ਼, ਅਚਲ Param &param, ਅਚਲ Flag &flag)
{
	ਜੇ (!flag.detect_underspeed_enabled || !flag.airspeed_enabled) {
		ਅਨੁਪਾਤ_undersped = 0.0f;
		ਵਾਪਸ;
	}

	// Ramp underspeed ratio from 0 at tas_min to 1 at 90% of tas_min, or halfway to tas_stall if that is higher
	ਅਚਲ ਦਸ਼ਮਲਵ tas_fully_undersped = math::min(param.tas_min,
					  math::max(0.9f * param.tas_min, 0.5f * (param.tas_min + param.tas_stall)));
	ਅਚਲ ਦਸ਼ਮਲਵ underspeed_ramp_ਚੌੜਾਈ = param.tas_min - tas_fully_undersped;

	ਅਨੁਪਾਤ_undersped = math::constrain((param.tas_min - ਨਿਵੇਸ਼.tas) / math::max(underspeed_ramp_ਚੌੜਾਈ, FLT_EPSILON),
					   0.0f, 1.0f);
}

TECSControl::SpecificEnergyWeighting TECSControl::update_ਰਫਤਾਰ_Altitude_Weights(ਅਚਲ Param &param, ਅਚਲ Flag &flag)
{

	SpecificEnergyWeighting ਭਾਰ;
	// Calculate the weight applied to control of specific kinetic energy error
	ਦਸ਼ਮਲਵ pitch_ਰਫਤਾਰ_ਭਾਰ = constrain(param.pitch_ਰਫਤਾਰ_ਭਾਰ, 0.0f, 2.0f);

	// Underspeed or fast descend: interpolate speed weight from nominal
	// towards max. We take the maximum of the two - this keeps the speed
	// weight continuous, even when underspeed and fast descend are combined
	ਅਚਲ ਦਸ਼ਮਲਵ max_ਅਨੁਪਾਤ = fmaxf(ਅਨੁਪਾਤ_undersped, param.fast_descend);
	pitch_ਰਫਤਾਰ_ਭਾਰ = max_ਅਨੁਪਾਤ * 2.0f + (1.0f - max_ਅਨੁਪਾਤ) * pitch_ਰਫਤਾਰ_ਭਾਰ;

	ਜੇ (!flag.airspeed_enabled) {
		pitch_ਰਫਤਾਰ_ਭਾਰ = 0.0f;
	}

	ਭਾਰ.spe_weighting = constrain(2.0f - pitch_ਰਫਤਾਰ_ਭਾਰ, 0.f, 2.f);
	ਭਾਰ.ske_weighting = constrain(pitch_ਰਫਤਾਰ_ਭਾਰ, 0.f, 2.f);

	ਵਾਪਸ ਭਾਰ;
}

ਦਸ਼ਮਲਵ TECSControl::calc_Pitch_Control_Ske_ਦਰ_Setpoint(ਅਚਲ Setpoint &setpoint, ਅਚਲ ਨਿਵੇਸ਼_2 &ਨਿਵੇਸ਼,
		ਅਚਲ STERate_ਹੱਦਬੰਦੀ &ਹੱਦਬੰਦੀ_2, ਅਚਲ Param &param, ਅਚਲ Flag &flag, ਅਚਲ SpecificEnergyWeighting &ਭਾਰ) ਅਚਲ
{
	// Trading altitude for speed with pitch is only wanted up to trim airspeed. Above trim, accelerating is left to
	// the throttle loop: if it saturates, the vehicle settles at a lower airspeed instead of below the altitude setpoint.
	// The airspeed setpoint seen by the pitch loop is therefore capped at max(trim, current airspeed). Between trim
	// and the setpoint the pitch loop holds altitude, while an overspeed above the setpoint is still corrected.
	// With a speed weight of 2 (speed priority, e.g. gliders, fast descend or underspeed) pitch is the only way to
	// track the setpoint, so the cap is faded out as the weight goes from 1 to 2.
	ਅਚਲ ਦਸ਼ਮਲਵ tas_setpoint_capped = min(setpoint.tas_setpoint, max(param.tas_trim, ਨਿਵੇਸ਼.tas));

	Setpoint pitch_control_setpoint{setpoint};
	pitch_control_setpoint.tas_setpoint = lerp(tas_setpoint_capped, setpoint.tas_setpoint,
					      constrain(ਭਾਰ.ske_weighting - 1.f, 0.f, 1.f));

	// d/dt(0.5 V^2) = V * dV/dt: the measured airspeed, not the setpoint, keeps the airspeed loop linear
	ਵਾਪਸ ਨਿਵੇਸ਼.tas * _calcAirspeedControlOutput(pitch_control_setpoint, ਨਿਵੇਸ਼, ਹੱਦਬੰਦੀ_2, param, flag);
}

TECSControl::SpecificEnergyRates TECSControl::_calcPitchControlEnergyRates(ਅਚਲ Setpoint &setpoint,
		ਅਚਲ ਨਿਵੇਸ਼_2 &ਨਿਵੇਸ਼, ਅਚਲ STERate_ਹੱਦਬੰਦੀ &ਹੱਦਬੰਦੀ_2, ਅਚਲ SpecificEnergyRates &specific_energy_rates,
		ਅਚਲ Param &param, ਅਚਲ Flag &flag, ਅਚਲ SpecificEnergyWeighting &ਭਾਰ) ਅਚਲ
{
	SpecificEnergyRates pitch_specific_energy_rates{specific_energy_rates};
	pitch_specific_energy_rates.ske_ਦਰ.setpoint = calc_Pitch_Control_Ske_ਦਰ_Setpoint(setpoint, ਨਿਵੇਸ਼, ਹੱਦਬੰਦੀ_2, param, flag,
			ਭਾਰ);

	// Between trim airspeed and the airspeed setpoint the throttle alone accelerates the vehicle. The capped setpoint
	// above already removes the kinetic setpoint of the pitch loop there, faded in with speed priority. But the
	// throttle's acceleration would still show up in the kinetic energy rate estimate, and the pitch loop would pitch
	// up to turn it into height. Remove the estimate from the pitch loop in this band as well, with the same fade, so
	// that pitch controls height only. The setpoint must not be scaled again, or it would be faded twice.
	ਅਚਲ ਤਾਰਕਿਕ throttle_accelerates = ਨਿਵੇਸ਼.tas > param.tas_trim && ਨਿਵੇਸ਼.tas < setpoint.tas_setpoint;
	ਅਚਲ ਦਸ਼ਮਲਵ kinetic_estimate_gain = throttle_accelerates ? constrain(ਭਾਰ.ske_weighting - 1.f, 0.f, 1.f) : 1.f;
	pitch_specific_energy_rates.ske_ਦਰ.estimate *= kinetic_estimate_gain;

	ਵਾਪਸ pitch_specific_energy_rates;
}

ਖਾਲੀ TECSControl::_calcPitchControl(ਦਸ਼ਮਲਵ dt, ਅਚਲ Setpoint &setpoint, ਅਚਲ ਨਿਵੇਸ਼_2 &ਨਿਵੇਸ਼, ਅਚਲ STERate_ਹੱਦਬੰਦੀ &ਹੱਦਬੰਦੀ_2,
				    ਅਚਲ SpecificEnergyRates &specific_energy_rates, ਅਚਲ Param &param, ਅਚਲ Flag &flag)
{
	ਅਚਲ SpecificEnergyWeighting ਭਾਰ{update_ਰਫਤਾਰ_Altitude_Weights(param, flag)};

	ਅਚਲ SpecificEnergyRates pitch_specific_energy_rates{_calcPitchControlEnergyRates(setpoint, ਨਿਵੇਸ਼, ਹੱਦਬੰਦੀ_2,
			specific_energy_rates, param, flag, ਭਾਰ)};

	Control_ਮੁੱਲਾਂ seb_ਦਰ{calc_Pitch_Control_Seb_ਦਰ(ਭਾਰ, pitch_specific_energy_rates)};

	_calcPitchControlUpdate(dt, ਨਿਵੇਸ਼, seb_ਦਰ, param);
	ਅਚਲ ਦਸ਼ਮਲਵ pitch_setpoint{_calcPitchControlOutput(ਨਿਵੇਸ਼, seb_ਦਰ, param)};

	// Comply with the specified vertical acceleration limit by applying a pitch rate limit
	// NOTE: at zero airspeed, the pitch increment is unbounded
	ਅਚਲ ਦਸ਼ਮਲਵ pitch_increment = dt * param.vert_accel_ਹੱਦਬੰਦੀ / math::max(ਨਿਵੇਸ਼.tas, FLT_EPSILON);
	_pitch_setpoint = constrain(pitch_setpoint, _pitch_setpoint - pitch_increment,
				    _pitch_setpoint + pitch_increment);
	_pitch_setpoint = constrain(_pitch_setpoint, param.pitch_min, param.pitch_max);

	//Debug Output
	_debug_output.energy_balance_ਦਰ_estimate = seb_ਦਰ.estimate;
	_debug_output.energy_balance_ਦਰ_sp = seb_ਦਰ.setpoint;
	_debug_output.pitch_integrator = _pitch_integ_state;
}

TECSControl::Control_ਮੁੱਲਾਂ TECSControl::calc_Pitch_Control_Seb_ਦਰ(ਅਚਲ SpecificEnergyWeighting &ਭਾਰ,
		ਅਚਲ SpecificEnergyRates &specific_energy_rates) ਅਚਲ
{
	Control_ਮੁੱਲਾਂ seb_ਦਰ;
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
	seb_ਦਰ.setpoint = specific_energy_rates.spe_ਦਰ.setpoint * ਭਾਰ.spe_weighting -
			    specific_energy_rates.ske_ਦਰ.setpoint *
			    ਭਾਰ.ske_weighting;

	seb_ਦਰ.estimate = (specific_energy_rates.spe_ਦਰ.estimate * ਭਾਰ.spe_weighting) -
			    (specific_energy_rates.ske_ਦਰ.estimate * ਭਾਰ.ske_weighting);

	ਵਾਪਸ seb_ਦਰ;
}

ਖਾਲੀ TECSControl::_calcPitchControlUpdate(ਦਸ਼ਮਲਵ dt, ਅਚਲ ਨਿਵੇਸ਼_2 &ਨਿਵੇਸ਼, ਅਚਲ Control_ਮੁੱਲਾਂ &seb_ਦਰ,
		ਅਚਲ Param &param)
{

	// In fast descend, the pitch integrator converges to the (substantially
	// negative) pitch. Ramp it down with decreasing fast descend ratio,
	// such that it is zero when the ratio becomes 0 again.
	ਜੇ (param.fast_descend < _fast_descend_prev && _fast_descend_prev > FLT_EPSILON) {
		_pitch_integ_state *= param.fast_descend / _fast_descend_prev;
	}

	_fast_descend_prev = param.fast_descend;

	ਜੇ (param.integrator_gain_pitch > FLT_EPSILON) {

		ਅਚਲ ਦਸ਼ਮਲਵ climb_angle_ਮੰਜ਼ਿਲ_SEB_ਦਰ = climb_Angle_ਮੰਜ਼ਿਲ_Seb_ਦਰ(ਨਿਵੇਸ਼, param);

		// Calculate pitch integrator input term
		ਦਸ਼ਮਲਵ pitch_integ_ਨਿਵੇਸ਼ = _getControlError(seb_ਦਰ) * param.integrator_gain_pitch / climb_angle_ਮੰਜ਼ਿਲ_SEB_ਦਰ;

		// Guard against NaN integrator input - discard bad input sample (e.g., exiting offboard velocity mode)
		ਜੇ (!PX4_ISFINITE(pitch_integ_ਨਿਵੇਸ਼)) {
			pitch_integ_ਨਿਵੇਸ਼ = 0.f;
		}

		// Prevent the integrator changing in a direction that will increase pitch demand saturation
		ਜੇ (_pitch_setpoint >= param.pitch_max) {
			pitch_integ_ਨਿਵੇਸ਼ = min(pitch_integ_ਨਿਵੇਸ਼, 0.f);

		} ਨਹੀਂਤਾਂ ਜੇ (_pitch_setpoint <= param.pitch_min) {
			pitch_integ_ਨਿਵੇਸ਼ = max(pitch_integ_ਨਿਵੇਸ਼, 0.f);
		}

		// Update the pitch integrator state
		_pitch_integ_state = _pitch_integ_state + pitch_integ_ਨਿਵੇਸ਼ * dt;

		// Safety check: reset integrator if state became corrupted
		ਜੇ (!PX4_ISFINITE(_pitch_integ_state)) {
			_pitch_integ_state = 0.f;
		}

	} ਨਹੀਂਤਾਂ {
		_pitch_integ_state = 0.0f;
	}
}

ਦਸ਼ਮਲਵ TECSControl::climb_Angle_ਮੰਜ਼ਿਲ_Seb_ਦਰ(ਅਚਲ ਨਿਵੇਸ਼_2 &ਨਿਵੇਸ਼, ਅਚਲ Param &param)
{
	// max() returns its second argument for a NaN first one, so a NaN airspeed falls back to the minimum airspeed
	ਵਾਪਸ max(ਨਿਵੇਸ਼.tas, param.tas_min, FLT_EPSILON) * CONSTANTS_ONE_G;
}

ਦਸ਼ਮਲਵ TECSControl::_calcPitchControlOutput(ਅਚਲ ਨਿਵੇਸ਼_2 &ਨਿਵੇਸ਼, ਅਚਲ Control_ਮੁੱਲਾਂ &seb_ਦਰ, ਅਚਲ Param &param) ਅਚਲ
{
	// Calculate a specific energy correction that doesn't include the integrator contribution
	ਦਸ਼ਮਲਵ SEB_ਦਰ_correction = _getControlError(seb_ਦਰ) * param.pitch_damping_gain +
				    param.seb_ਦਰ_ff *
				    seb_ਦਰ.setpoint;

	// Convert the specific energy balance rate correction to a climb angle and add the level-flight pitch, the angle
	// of attack at zero climb angle, to get the absolute pitch setpoint. This calculation assumes:
	// a) The climb angle follows pitch angle with a lag that is small enough not to destabilise the control loop.
	// b) The offset between climb angle and pitch angle (angle of attack) is constant and equal to the level-flight
	// pitch, excluding the effect of pitch transients due to control action or turbulence.
	ਦਸ਼ਮਲਵ pitch_setpoint_unc = SEB_ਦਰ_correction / climb_Angle_ਮੰਜ਼ਿਲ_Seb_ਦਰ(ਨਿਵੇਸ਼, param) + _pitch_integ_state
				   + param.level_flight_pitch;

	// Guard against a non-finite feedforward/damping contribution (e.g. seb_rate going NaN on the energy/speed
	// side when exiting offboard velocity mode). constrain() does not reject NaN, so fall back to the integrator
	// state, which is kept finite by _calcPitchControlUpdate, to avoid latching a NaN pitch demand.
	ਜੇ (!PX4_ISFINITE(pitch_setpoint_unc)) {
		pitch_setpoint_unc = _pitch_integ_state + param.level_flight_pitch;
	}

	ਵਾਪਸ constrain(pitch_setpoint_unc, param.pitch_min, param.pitch_max);
}

ਖਾਲੀ TECSControl::_calcThrottleControl(ਦਸ਼ਮਲਵ dt, ਅਚਲ STERate_ਹੱਦਬੰਦੀ &ਹੱਦਬੰਦੀ_2,
				       ਅਚਲ SpecificEnergyRates &specific_energy_rates, ਅਚਲ Param &param, ਅਚਲ Flag &flag)
{
	// Update STE rate estimate LP filter
	ਅਚਲ ਦਸ਼ਮਲਵ STE_ਦਰ_estimate_raw = specific_energy_rates.spe_ਦਰ.estimate + specific_energy_rates.ske_ਦਰ.estimate;
	ste_ਦਰ_estimate_filter.ਸੈੱਟ_Parameters(dt, param.ste_ਦਰ_ਸਮਾਂ_const);
	ste_ਦਰ_estimate_filter.update(STE_ਦਰ_estimate_raw);
	Control_ਮੁੱਲਾਂ ste_ਦਰ{calc_Throttle_Control_Ste_ਦਰ(ਹੱਦਬੰਦੀ_2, specific_energy_rates, param)};
	// During fast descend, we control airspeed over the pitch control loop. Give minimal thrust as soon as we are descending
	ਦਸ਼ਮਲਵ throttle_setpoint{param.throttle_min};

	ਜੇ (!fastDescendFullyEngaged(param.fast_descend)) {
		_calcThrottleControlUpdate(dt, ਹੱਦਬੰਦੀ_2, ste_ਦਰ, param, flag);
		throttle_setpoint = (1.f - param.fast_descend) * _calcThrottleControlOutput(ਹੱਦਬੰਦੀ_2, ste_ਦਰ, param,
				    flag) + param.fast_descend * param.throttle_min;
	}

	// Rate limit the throttle demand
	ਜੇ (fabsf(param.throttle_slewrate) > FLT_EPSILON) {
		ਅਚਲ ਦਸ਼ਮਲਵ throttle_increment_ਹੱਦਬੰਦੀ = dt * (param.throttle_max - param.throttle_min) * param.throttle_slewrate;
		throttle_setpoint = constrain(throttle_setpoint, _throttle_setpoint - throttle_increment_ਹੱਦਬੰਦੀ,
					      _throttle_setpoint + throttle_increment_ਹੱਦਬੰਦੀ);
	}

	_throttle_setpoint = constrain(throttle_setpoint, param.throttle_min, param.throttle_max);

	// Debug output
	_debug_output.ਕੁੱਲ_energy_ਦਰ_estimate = ste_ਦਰ.estimate;
	_debug_output.ਕੁੱਲ_energy_ਦਰ_sp = ste_ਦਰ.setpoint;
	_debug_output.throttle_integrator = _throttle_integ_state;
}

TECSControl::Control_ਮੁੱਲਾਂ TECSControl::calc_Throttle_Control_Ste_ਦਰ(ਅਚਲ STERate_ਹੱਦਬੰਦੀ &ਹੱਦਬੰਦੀ_2,
		ਅਚਲ SpecificEnergyRates &specific_energy_rates,
		ਅਚਲ Param &param) ਅਚਲ
{
	// Output ste rate values
	Control_ਮੁੱਲਾਂ ste_ਦਰ;
	ste_ਦਰ.setpoint = specific_energy_rates.spe_ਦਰ.setpoint + specific_energy_rates.ske_ਦਰ.setpoint;

	// Adjust the demanded total energy rate to compensate for induced drag rise in turns.
	// Assume induced drag scales linearly with normal load factor.
	// The additional normal load factor is given by (1/cos(bank angle) - 1)
	ste_ਦਰ.setpoint += param.ਲੱਦੋ_ਗੁਣਕ_correction * (param.ਲੱਦੋ_ਗੁਣਕ - 1.f);

	ste_ਦਰ.setpoint = constrain(ste_ਦਰ.setpoint, ਹੱਦਬੰਦੀ_2.STE_ਦਰ_min, ਹੱਦਬੰਦੀ_2.STE_ਦਰ_max);
	ste_ਦਰ.estimate = ste_ਦਰ_estimate_filter.getState();

	ਵਾਪਸ ste_ਦਰ;
}

ਖਾਲੀ TECSControl::_calcThrottleControlUpdate(ਦਸ਼ਮਲਵ dt, ਅਚਲ STERate_ਹੱਦਬੰਦੀ &ਹੱਦਬੰਦੀ_2, ਅਚਲ Control_ਮੁੱਲਾਂ &ste_ਦਰ,
		ਅਚਲ Param &param, ਅਚਲ Flag &flag)
{
	ਅਚਲ ਦਸ਼ਮਲਵ STE_ਦਰ_ਮੰਜ਼ਿਲ_throttle = ste_ਦਰ_ਮੰਜ਼ਿਲ_Throttle(ਹੱਦਬੰਦੀ_2);

	// Integral handling
	ਜੇ (flag.airspeed_enabled) {
		ਜੇ (param.integrator_gain_throttle > FLT_EPSILON) {
			// underspeed conditions zero out integration
			ਦਸ਼ਮਲਵ throttle_integ_ਨਿਵੇਸ਼ = (_getControlError(ste_ਦਰ) * param.integrator_gain_throttle) * dt *
						     STE_ਦਰ_ਮੰਜ਼ਿਲ_throttle * (1.0f - ਅਨੁਪਾਤ_undersped);

			// only allow integrator propagation into direction which unsaturates throttle
			ਜੇ (_throttle_setpoint >= param.throttle_max) {
				throttle_integ_ਨਿਵੇਸ਼ = math::min(0.f, throttle_integ_ਨਿਵੇਸ਼);

			}

			ਜੇ (_throttle_setpoint <= param.throttle_min) {
				throttle_integ_ਨਿਵੇਸ਼ = math::max(0.f, throttle_integ_ਨਿਵੇਸ਼);
			}

			// Calculate a throttle demand from the integrated total energy rate error
			// This will be added to the total throttle demand to compensate for steady state errors
			_throttle_integ_state = PX4_ISFINITE(throttle_integ_ਨਿਵੇਸ਼) ? _throttle_integ_state + throttle_integ_ਨਿਵੇਸ਼ : 0.f;

		} ਨਹੀਂਤਾਂ {
			_throttle_integ_state = 0.0f;
		}

	}
}

ਦਸ਼ਮਲਵ TECSControl::_calcThrottleControlOutput(ਅਚਲ STERate_ਹੱਦਬੰਦੀ &ਹੱਦਬੰਦੀ_2, ਅਚਲ Control_ਮੁੱਲਾਂ &ste_ਦਰ,
		ਅਚਲ Param &param,
		ਅਚਲ Flag &flag) ਅਚਲ
{
	ਅਚਲ ਦਸ਼ਮਲਵ STE_ਦਰ_ਮੰਜ਼ਿਲ_throttle = ste_ਦਰ_ਮੰਜ਼ਿਲ_Throttle(ਹੱਦਬੰਦੀ_2);

	// Calculate a predicted throttle from the demanded rate of change of energy, using the cruise throttle
	// as the starting point. Assume:
	// Specific total energy rate = _STE_rate_max is achieved when throttle is set to _throttle_setpoint_max
	// Specific total energy rate = 0 at cruise throttle
	// Specific total energy rate = _STE_rate_min is achieved when throttle is set to _throttle_setpoint_min

	// assume airspeed and density-independent delta_throttle to sink/climb rate mapping
	// TODO: include air density for thrust mappings
	ਅਚਲ ਦਸ਼ਮਲਵ throttle_above_trim_ਪ੍ਰਤੀ_ste_ਦਰ = (param.throttle_max - param.throttle_trim) / ਹੱਦਬੰਦੀ_2.STE_ਦਰ_max;
	ਅਚਲ ਦਸ਼ਮਲਵ throttle_ਹੇਠਾਂ_trim_ਪ੍ਰਤੀ_ste_ਦਰ = (param.throttle_trim - param.throttle_min) / ਹੱਦਬੰਦੀ_2.STE_ਦਰ_min;

	ਦਸ਼ਮਲਵ throttle_predicted = 0.0f;

	ਜੇ (ste_ਦਰ.setpoint >= FLT_EPSILON) {
		// throttle is between trim and maximum
		throttle_predicted = param.throttle_trim + ste_ਦਰ.setpoint * throttle_above_trim_ਪ੍ਰਤੀ_ste_ਦਰ;

	} ਨਹੀਂਤਾਂ {
		// throttle is between trim and minimum
		throttle_predicted = param.throttle_trim - ste_ਦਰ.setpoint * throttle_ਹੇਠਾਂ_trim_ਪ੍ਰਤੀ_ste_ਦਰ;

	}

	// Add proportional and derivative control feedback to the predicted throttle and constrain to throttle limits
	ਦਸ਼ਮਲਵ throttle_setpoint = (_getControlError(ste_ਦਰ) * param.throttle_damping_gain) * STE_ਦਰ_ਮੰਜ਼ਿਲ_throttle +
				  throttle_predicted;

	ਜੇ (flag.airspeed_enabled) {
		// Add the integrator feedback during closed loop operation with an airspeed sensor
		throttle_setpoint += _throttle_integ_state;

	} ਨਹੀਂਤਾਂ {
		/* We want to avoid reducing the throttle output when switching from airspeed enabled mode into airspeedless mode.
		  Thus, if the throttle integrator has a positive value, add it still to the throttle setpoint. */
		throttle_setpoint += math::max(0.0f, _throttle_integ_state);
	}


	// ramp in max throttle setting with underspeediness value
	throttle_setpoint = ਅਨੁਪਾਤ_undersped * param.throttle_max + (1.0f - ਅਨੁਪਾਤ_undersped) * throttle_setpoint;

	ਵਾਪਸ constrain(throttle_setpoint, param.throttle_min, param.throttle_max);
}

ਖਾਲੀ TECSControl::resetIntegrals()
{
	_pitch_integ_state = 0.0f;
	_throttle_integ_state = 0.0f;
}

ਖਾਲੀ TECS::initControlParams(ਅਚਲ ਨਿਵੇਸ਼_2 &ਨਿਵੇਸ਼)
{
	// Update parameters from input
	// Reference model
	_reference_param.ਨਿਸ਼ਾਨਾ_climbrate = ਨਿਵੇਸ਼.ਨਿਸ਼ਾਨਾ_climbrate;
	_reference_param.ਨਿਸ਼ਾਨਾ_sinkrate = ਨਿਵੇਸ਼.ਨਿਸ਼ਾਨਾ_sinkrate;
	// Control
	_control_param.tas_min = ਨਿਵੇਸ਼.eas_ਮੰਜ਼ਿਲ_tas * _equivalent_airspeed_min;
	_control_param.tas_stall = ਨਿਵੇਸ਼.eas_ਮੰਜ਼ਿਲ_tas * _equivalent_airspeed_stall;
	_control_param.tas_max = ਨਿਵੇਸ਼.eas_ਮੰਜ਼ਿਲ_tas * _equivalent_airspeed_max;
	_control_param.tas_trim = ਨਿਵੇਸ਼.eas_ਮੰਜ਼ਿਲ_tas * _control_param.equivalent_airspeed_trim;
	_control_param.pitch_max = ਨਿਵੇਸ਼.pitch_max;
	_control_param.pitch_min = ਨਿਵੇਸ਼.pitch_min;
	_control_param.throttle_trim = ਨਿਵੇਸ਼.throttle_trim;
	_control_param.throttle_max = ਨਿਵੇਸ਼.throttle_max;
	_control_param.throttle_min = ਨਿਵੇਸ਼.throttle_min;
}

ਦਸ਼ਮਲਵ TECS::calcTrueAirspeedSetpoint(ਦਸ਼ਮਲਵ eas_ਮੰਜ਼ਿਲ_tas, ਦਸ਼ਮਲਵ eas_setpoint)
{
	ਵਾਪਸ lerp(eas_ਮੰਜ਼ਿਲ_tas * eas_setpoint, _control_param.tas_max, _fast_descend);
}

ਖਾਲੀ TECS::initialize(ਅਚਲ ਨਿਵੇਸ਼_2 &ਨਿਵੇਸ਼)
{
	// Init subclasses
	TECSAltitudeReferenceModel::AltitudeReferenceState current_state{.alt = ਨਿਵੇਸ਼.altitude,
			.alt_ਦਰ = ਨਿਵੇਸ਼.altitude_ਦਰ};
	_altitude_reference_model.initialize(current_state);
	_airspeed_filter.initialize(ਨਿਵੇਸ਼.equivalent_airspeed, _airspeed_filter_param.equivalent_airspeed_trim,
				    _control_flag.airspeed_enabled);

	// Seed the controller from the filter, which falls back to trim airspeed without a valid measurement,
	// so the controller sees the same airspeed on reset as on every following update
	ਅਚਲ ਦਸ਼ਮਲਵ true_airspeed = ਨਿਵੇਸ਼.eas_ਮੰਜ਼ਿਲ_tas * _airspeed_filter.getState().ਰਫਤਾਰ;

	TECSControl::Setpoint control_setpoint;
	control_setpoint.altitude_reference = _altitude_reference_model.getAltitudeReference();
	control_setpoint.altitude_ਦਰ_setpoint_ਸਿੱਧਾ = NAN; // initialization always goes through the altitude loop
	control_setpoint.tas_setpoint = true_airspeed;

	ਅਚਲ TECSControl::ਨਿਵੇਸ਼_2 control_ਨਿਵੇਸ਼{ .altitude = ਨਿਵੇਸ਼.altitude,
						.altitude_ਦਰ = ਨਿਵੇਸ਼.altitude_ਦਰ,
						.tas = true_airspeed,
						.tas_ਦਰ = 0.0f};

	_control.initialize(control_setpoint, control_ਨਿਵੇਸ਼, _control_param, _control_flag);

	_fast_descend = 0.f;
	_enabled_fast_descend_timestamp = 0U;
}

ਖਾਲੀ TECS::update(ਅਚਲ ਨਿਵੇਸ਼_2 &ਨਿਵੇਸ਼)
{
	// Calculate the time since last update (seconds)
	ਅਚਲ hrt_abstime now = hrt_absolute_ਸਮਾਂ();
	ਅਚਲ ਦਸ਼ਮਲਵ dt = static_cast<ਦਸ਼ਮਲਵ>(now - _update_timestamp) * 1e-6f;

	initControlParams(ਨਿਵੇਸ਼);

	ਜੇ (dt < DT_MIN) {
		// Update interval too small, do not update. Assume constant states/output in this case.
		ਵਾਪਸ;
	}

	ਦਸ਼ਮਲਵ true_airspeed_setpoint;

	ਜੇ (dt > DT_MAX || _update_timestamp == 0UL) {
		// Update time interval too large, can't guarantee sanity of state updates anymore. reset the control loop.
		initialize(ਨਿਵੇਸ਼);
		true_airspeed_setpoint = calcTrueAirspeedSetpoint(ਨਿਵੇਸ਼.eas_ਮੰਜ਼ਿਲ_tas, ਨਿਵੇਸ਼.equivalent_airspeed_setpoint);

	} ਨਹੀਂਤਾਂ {
		/* Check if we want to fast descend. On fast descend, we set the throttle to min, and use the altitude control
		loop to control the speed to the maximum airspeed. */
		ਸੈੱਟ_Fast_Descend(ਨਿਵੇਸ਼.altitude_setpoint, ਨਿਵੇਸ਼.altitude);
		_control_param.fast_descend = _fast_descend;

		// Update airspeedfilter submodule
		ਅਚਲ TECSAirspeedFilter::ਨਿਵੇਸ਼_2 airspeed_ਨਿਵੇਸ਼{ .equivalent_airspeed = ਨਿਵੇਸ਼.equivalent_airspeed,
				.equivalent_airspeed_ਦਰ = ਨਿਵੇਸ਼.ਰਫਤਾਰ_deriv_forward / ਨਿਵੇਸ਼.eas_ਮੰਜ਼ਿਲ_tas};

		_airspeed_filter.update(dt, airspeed_ਨਿਵੇਸ਼, _airspeed_filter_param, _control_flag.airspeed_enabled);

		// Update Reference model submodule
		ਜੇ (fastDescendFullyEngaged(_fast_descend)) {
			// Reset the altitude reference model, while we are in fast descend.
			ਅਚਲ TECSAltitudeReferenceModel::AltitudeReferenceState init_state{
				.alt = ਨਿਵੇਸ਼.altitude,
				.alt_ਦਰ = ਨਿਵੇਸ਼.altitude_ਦਰ};
			_altitude_reference_model.initialize(init_state);

		} ਨਹੀਂਤਾਂ ਜੇ (_control.get_Underspeed_ਅਨੁਪਾਤ() > 0.5f) {
			// Reset altitude reference during underspeed to prevent altitude error accumulation
			// that would cause aggressive pitch-up (and potential secondary stall) on recovery.
			ਅਚਲ TECSAltitudeReferenceModel::AltitudeReferenceState init_state{
				.alt = ਨਿਵੇਸ਼.altitude,
				.alt_ਦਰ = ਨਿਵੇਸ਼.altitude_ਦਰ};
			_altitude_reference_model.initialize(init_state);

		} ਨਹੀਂਤਾਂ {
			ਅਚਲ TECSAltitudeReferenceModel::AltitudeReferenceState setpoint{ .alt = ਨਿਵੇਸ਼.altitude_setpoint,
					.alt_ਦਰ = ਨਿਵੇਸ਼.altitude_ਦਰ_setpoint};

			_altitude_reference_model.update(dt, setpoint, ਨਿਵੇਸ਼.altitude, ਨਿਵੇਸ਼.altitude_ਦਰ, _reference_param);
		}

		TECSControl::Setpoint control_setpoint;
		control_setpoint.altitude_reference = _altitude_reference_model.getAltitudeReference();
		control_setpoint.altitude_ਦਰ_setpoint_ਸਿੱਧਾ = _altitude_reference_model.get_ਉਚਾਈ_ਦਰ_Setpoint_ਸਿੱਧਾ();
		true_airspeed_setpoint = calcTrueAirspeedSetpoint(ਨਿਵੇਸ਼.eas_ਮੰਜ਼ਿਲ_tas, ਨਿਵੇਸ਼.equivalent_airspeed_setpoint);
		control_setpoint.tas_setpoint = true_airspeed_setpoint;

		ਅਚਲ TECSAirspeedFilter::AirspeedFilterState airspeed_state{_airspeed_filter.getState()};
		ਅਚਲ TECSControl::ਨਿਵੇਸ਼_2 control_ਨਿਵੇਸ਼{ .altitude = ਨਿਵੇਸ਼.altitude,
							.altitude_ਦਰ = ਨਿਵੇਸ਼.altitude_ਦਰ,
							.tas = ਨਿਵੇਸ਼.eas_ਮੰਜ਼ਿਲ_tas * airspeed_state.ਰਫਤਾਰ,
							.tas_ਦਰ = ਨਿਵੇਸ਼.eas_ਮੰਜ਼ਿਲ_tas * airspeed_state.ਰਫਤਾਰ_ਦਰ};

		_control.update(dt, control_setpoint, control_ਨਿਵੇਸ਼, _control_param, _control_flag);
	}

	debug_ਸਥਿਤੀ.control = _control.getDebugOutput();
	debug_ਸਥਿਤੀ.true_airspeed_sp = true_airspeed_setpoint;
	ਅਚਲ TECSAirspeedFilter::AirspeedFilterState airspeed_state{_airspeed_filter.getState()};
	ਅਚਲ TECSAltitudeReferenceModel::AltitudeReferenceState altitude_reference{_altitude_reference_model.getAltitudeReference()};
	debug_ਸਥਿਤੀ.true_airspeed_filtered = ਨਿਵੇਸ਼.eas_ਮੰਜ਼ਿਲ_tas * airspeed_state.ਰਫਤਾਰ;
	debug_ਸਥਿਤੀ.true_airspeed_derivative = ਨਿਵੇਸ਼.eas_ਮੰਜ਼ਿਲ_tas * airspeed_state.ਰਫਤਾਰ_ਦਰ;
	debug_ਸਥਿਤੀ.altitude_reference = altitude_reference.alt;
	debug_ਸਥਿਤੀ.ਉਚਾਈ_ਦਰ_reference = altitude_reference.alt_ਦਰ;
	debug_ਸਥਿਤੀ.ਉਚਾਈ_ਦਰ_ਸਿੱਧਾ = _altitude_reference_model.get_ਉਚਾਈ_ਦਰ_Setpoint_ਸਿੱਧਾ();
	debug_ਸਥਿਤੀ.fast_descend = _fast_descend;

	_update_timestamp = now;
}

ਖਾਲੀ TECS::ਸੈੱਟ_Fast_Descend(ਅਚਲ ਦਸ਼ਮਲਵ alt_setpoint, ਅਚਲ ਦਸ਼ਮਲਵ alt)
{
	// disable fast descend if we are close to the target altitude or the altitude setpoint is not finite

	ਜੇ (PX4_ISFINITE(alt_setpoint) && _control_flag.airspeed_enabled && (_fast_descend_alt_err > FLT_EPSILON)
	    && ((alt_setpoint + _fast_descend_alt_err) < alt)) {
		ਸਵੈਚਾਲਿਤ now = hrt_absolute_ਸਮਾਂ();

		ਜੇ (_enabled_fast_descend_timestamp == 0U) {
			_enabled_fast_descend_timestamp = now;
		}

		_fast_descend = constrain(max(_fast_descend, static_cast<ਦਸ਼ਮਲਵ>(now - _enabled_fast_descend_timestamp) /
					      static_cast<ਦਸ਼ਮਲਵ>(FAST_DESCEND_RAMP_ਉੱਪਰ_ਸਮਾਂ)), 0.f, 1.f);

	} ਨਹੀਂਤਾਂ ਜੇ (PX4_ISFINITE(alt_setpoint) && (_fast_descend > FLT_EPSILON) && (_fast_descend_alt_err > FLT_EPSILON)) {
		// We're in fast descend, scale it down until FAST_DESCEND_HEIGHT_MARGIN above target altitude
		_fast_descend = constrain((alt - alt_setpoint - FAST_DESCEND_ਉਚਾਈ_MARGIN) / _fast_descend_alt_err, 0.f, 1.f);
		_enabled_fast_descend_timestamp = 0U;

	} ਨਹੀਂਤਾਂ {
		_fast_descend = 0.f;
		_enabled_fast_descend_timestamp = 0U;
	}
}

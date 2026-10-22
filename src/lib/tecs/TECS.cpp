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

#ಅನ್ತರ್ಭಾವ "TECS.hpp"

#ಅನ್ತರ್ಭಾವ <lib/geo/geo.h>

#ಅನ್ತರ್ಭಾವ <px4_platform_common/defines.h>

ಬಳಸಿ math::constrain;
ಬಳಸಿ math::max;
ಬಳಸಿ math::min;

ಸ್ಥಿರ constexpr ತಾರ್ಕಿಕ is_Valid_ಸಮಯ_ಹೆಜ್ಜೆ(ದಶಮಾಂಶ dt) { ಹಿಂತಿರುಗಿ (PX4_ISFINITE(dt) && dt > FLT_EPSILON);}
ಸ್ಥಿರ constexpr ತಾರ್ಕಿಕ fastDescendFullyEngaged(ದಶಮಾಂಶ fast_descend) { ಹಿಂತಿರುಗಿ 1.f - fast_descend < FLT_EPSILON; }

ಶೂನ್ಯ TECSAirspeedFilter::initialize(ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ equivalent_airspeed, ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ equivalent_airspeed_trim,
				    ಸ್ಥಿರಾಂಕ ತಾರ್ಕಿಕ airspeed_sensor_ಲಭ್ಯ)
{
	ಆದರೆ (airspeed_sensor_ಲಭ್ಯ && PX4_ISFINITE(equivalent_airspeed)) {
		_airspeed_state.ವೇಗ = equivalent_airspeed;

	} ಇಲ್ಲದಿದ್ದರೆ {
		_airspeed_state.ವೇಗ = equivalent_airspeed_trim;
	}

	_airspeed_state.ವೇಗ_ದರ = 0.f;
}

ಶೂನ್ಯ TECSAirspeedFilter::update(ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ dt, ಸ್ಥಿರಾಂಕ ಒಳಹರಿವು_2 &ಒಳಹರಿವು, ಸ್ಥಿರಾಂಕ Param &param,
				ಸ್ಥಿರಾಂಕ ತಾರ್ಕಿಕ airspeed_sensor_ಲಭ್ಯ)
{
	// Input checking
	ಆದರೆ (!is_Valid_ಸಮಯ_ಹೆಜ್ಜೆ(dt)) {
		// Do not update the states.
		ಹಿಂತಿರುಗಿ;
	}

	ದಶಮಾಂಶ airspeed;

	ಆದರೆ (PX4_ISFINITE(ಒಳಹರಿವು.equivalent_airspeed) && airspeed_sensor_ಲಭ್ಯ) {
		airspeed = ಒಳಹರಿವು.equivalent_airspeed;

	} ಇಲ್ಲದಿದ್ದರೆ {
		airspeed = param.equivalent_airspeed_trim;
	}

	ದಶಮಾಂಶ airspeed_derivative;

	ಆದರೆ (PX4_ISFINITE(ಒಳಹರಿವು.equivalent_airspeed_ದರ) && airspeed_sensor_ಲಭ್ಯ) {
		airspeed_derivative = ಒಳಹರಿವು.equivalent_airspeed_ದರ;

	} ಇಲ್ಲದಿದ್ದರೆ {
		airspeed_derivative = 0.0f;
	}

	/* Filter airspeed and rate using a constant airspeed rate model in a steady state Kalman Filter.
	 We use the gains of the continuous Kalman filter Kc and approximate the discrete version Kalman gain Kd =dt*Kc,
	 since the continuous algebraic Riccatti equation is easier to solve.
	*/

	ಮಾತೃಕೆ::Vector2f new_state_predicted;

	new_state_predicted(0) = _airspeed_state.ವೇಗ + dt * _airspeed_state.ವೇಗ_ದರ;
	new_state_predicted(1) = _airspeed_state.ವೇಗ_ದರ;

	ಸ್ಥಿರಾಂಕ ಮಾತೃಕೆ::Vector2f innovation{(airspeed - new_state_predicted(0)), (airspeed_derivative - new_state_predicted(1))};

	ಮಾತೃಕೆ::Vector2f new_state;
	new_state(0) = new_state_predicted(0) + dt * (kKg00 * innovation(0) + kKg01 * innovation(1));
	new_state(1) = new_state_predicted(1) + dt * (kKg10 * innovation(0) + kKg11 * innovation(1));

	// Clip airspeed at zero
	ಆದರೆ (new_state(0) < FLT_EPSILON) {
		new_state(0) = 0.0f;
		// calculate input that would result in zero speed.
		ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ desired_airspeed_innovation = (-new_state_predicted(0) / dt - kKg01 * innovation(1)) / kKg00;
		new_state(1) = new_state_predicted(1) + dt * (kKg10 * desired_airspeed_innovation + kKg11 * innovation(1));
	}

	// Update states
	_airspeed_state.ವೇಗ = new_state(0);
	_airspeed_state.ವೇಗ_ದರ = new_state(1);
}

TECSAirspeedFilter::AirspeedFilterState TECSAirspeedFilter::getState() ಸ್ಥಿರಾಂಕ
{
	ಹಿಂತಿರುಗಿ _airspeed_state;
}

ಶೂನ್ಯ TECSAltitudeReferenceModel::update(ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ dt, ಸ್ಥಿರಾಂಕ AltitudeReferenceState &setpoint, ದಶಮಾಂಶ altitude,
					ದಶಮಾಂಶ ಎತ್ತರ_ದರ, ಸ್ಥಿರಾಂಕ Param &param)
{
	// Input checks
	ಆದರೆ (!is_Valid_ಸಮಯ_ಹೆಜ್ಜೆ(dt)) {
		// Do not update the states.
		ಹಿಂತಿರುಗಿ;
	}

	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ current_alt = PX4_ISFINITE(altitude) ? altitude : 0.f;

	_velocity_control_traj_generator.ಗಣ_Max_Jerk(JERK_MAX);
	_velocity_control_traj_generator.ಗಣ_Max_Accel_ಮೇಲೆ(param.vert_accel_ಎಲ್ಲೆ);
	_velocity_control_traj_generator.ಗಣ_Max_Accel_ಕೆಳಕ್ಕೆ(param.vert_accel_ಎಲ್ಲೆ);
	_velocity_control_traj_generator.ಗಣ_Max_Vel_ಮೇಲೆ(param.max_sink_ದರ); // different convention for FW than for MC
	_velocity_control_traj_generator.ಗಣ_Max_Vel_ಕೆಳಕ್ಕೆ(param.max_climb_ದರ); // different convention for FW than for MC

	// Altitude setpoint reference
	_alt_control_traj_generator.ಗಣ_Max_Jerk(JERK_MAX);
	_alt_control_traj_generator.ಗಣ_Max_Accel(param.vert_accel_ಎಲ್ಲೆ);
	_alt_control_traj_generator.ಗಣ_Max_Vel(fmaxf(param.max_climb_ದರ, param.max_sink_ದರ));

	// XXX: this is a bit risky.. .alt_rate here could be NAN (by interface design) - and is only ok to input to the
	// setVelSpFeedback() method because it calls the reset in the logic below when it is NAN.
	// TODO: stop it with the NAN interfaces, make sure to take care of this when refactoring and separating altitude
	// and height rate control loops.
	_velocity_control_traj_generator.ಗಣ_Vel_Sp_Feedback(setpoint.alt_ದರ);

	ತಾರ್ಕಿಕ control_altitude = ನಿಜ;
	ದಶಮಾಂಶ altitude_setpoint = setpoint.alt;

	ಆದರೆ (!PX4_ISFINITE(setpoint.alt) && !PX4_ISFINITE(setpoint.alt_ದರ)) {
		// neither altitude nor altitude rate is set - reset to current altitude
		_velocity_control_traj_generator.reset(0.f, 0, current_alt);
		altitude_setpoint = current_alt;

	} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (PX4_ISFINITE(setpoint.alt_ದರ)) {
		// input is height rate (not altitude)
		_velocity_control_traj_generator.ಗಣ_Current_Position_Estimate(current_alt);
		_velocity_control_traj_generator.update(dt, setpoint.alt_ದರ);
		altitude_setpoint = _velocity_control_traj_generator.getCurrentPosition();
		control_altitude = PX4_ISFINITE(altitude_setpoint); // returns true if altitude is locked

	} ಇಲ್ಲದಿದ್ದರೆ {
		// input is altitude
		_velocity_control_traj_generator.reset(0, ಎತ್ತರ_ದರ, altitude_setpoint);
	}

	ಆದರೆ (control_altitude) {
		ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ ಗುರಿ_climbrate_m_s = math::min(param.ಗುರಿ_climbrate, param.max_climb_ದರ);
		ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ ಗುರಿ_sinkrate_m_s = math::min(param.ಗುರಿ_sinkrate, param.max_sink_ದರ);

		ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ delta_trajectory_ಗಮ್ಯ_ಗುರಿ_m = altitude_setpoint - _alt_control_traj_generator.getCurrentPosition();

		ದಶಮಾಂಶ ಎತ್ತರ_ದರ_ಗುರಿ = math::signNoZero<ದಶಮಾಂಶ>(delta_trajectory_ಗಮ್ಯ_ಗುರಿ_m) *
					   math::trajectory::compute_Max_ವೇಗ_From_ದೂರ(
						   JERK_MAX, param.vert_accel_ಎಲ್ಲೆ, fabsf(delta_trajectory_ಗಮ್ಯ_ಗುರಿ_m), 0.f);

		ಎತ್ತರ_ದರ_ಗುರಿ = math::constrain(ಎತ್ತರ_ದರ_ಗುರಿ, -ಗುರಿ_sinkrate_m_s, ಗುರಿ_climbrate_m_s);

		_alt_control_traj_generator.updateDurations(ಎತ್ತರ_ದರ_ಗುರಿ);
		_alt_control_traj_generator.updateTraj(dt);
		ಎತ್ತರ_ದರ_setpoint_ನೇರ = NAN;

	} ಇಲ್ಲದಿದ್ದರೆ {
		_alt_control_traj_generator.ಗಣ_Current_Velocity(_velocity_control_traj_generator.getCurrentVelocity());
		_alt_control_traj_generator.ಗಣ_Current_Position(current_alt);
		ಎತ್ತರ_ದರ_setpoint_ನೇರ = _velocity_control_traj_generator.getCurrentVelocity();
	}
}

TECSAltitudeReferenceModel::AltitudeReferenceState TECSAltitudeReferenceModel::getAltitudeReference() ಸ್ಥಿರಾಂಕ
{
	TECSAltitudeReferenceModel::AltitudeReferenceState ref{
		.alt = _alt_control_traj_generator.getCurrentPosition(),
		.alt_ದರ = _alt_control_traj_generator.getCurrentVelocity(),
	};

	ಹಿಂತಿರುಗಿ ref;
}

ಶೂನ್ಯ TECSAltitudeReferenceModel::initialize(ಸ್ಥಿರಾಂಕ AltitudeReferenceState &state)
{
	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ init_state_alt = PX4_ISFINITE(state.alt) ? state.alt : 0.f;
	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ init_state_alt_ದರ = PX4_ISFINITE(state.alt_ದರ) ? state.alt_ದರ : 0.f;

	_alt_control_traj_generator.reset(0.0f, init_state_alt_ದರ, init_state_alt);
	_velocity_control_traj_generator.reset(0.f, init_state_alt_ದರ, init_state_alt);
}

ಶೂನ್ಯ TECSControl::initialize(ಸ್ಥಿರಾಂಕ Setpoint &setpoint, ಸ್ಥಿರಾಂಕ ಒಳಹರಿವು_2 &ಒಳಹರಿವು, ಸ್ಥಿರಾಂಕ Param &param, ಸ್ಥಿರಾಂಕ Flag &flag)
{
	resetIntegrals();

	// Initialization always goes through the altitude loop, even if a direct height rate setpoint is given
	Setpoint altitude_loop_setpoint{setpoint};
	altitude_loop_setpoint.altitude_ದರ_setpoint_ನೇರ = NAN;

	ಸ್ಥಿರಾಂಕ STERate_ಎಲ್ಲೆ ಎಲ್ಲೆ_2{calculate_ಒಟ್ಟು_Energy_ದರ_ಎಲ್ಲೆ(param)};
	ಸ್ಥಿರಾಂಕ SpecificEnergyRates specific_energy_ದರ{_updateOuterLoops(altitude_loop_setpoint, ಒಳಹರಿವು, ಎಲ್ಲೆ_2, param, flag)};

	ಸ್ಥಿರಾಂಕ SpecificEnergyWeighting ತೂಕ{update_ವೇಗ_Altitude_Weights(param, flag)};
	ಸ್ಥಿರಾಂಕ SpecificEnergyRates pitch_specific_energy_ದರ{_calcPitchControlEnergyRates(setpoint, ಒಳಹರಿವು, ಎಲ್ಲೆ_2,
			specific_energy_ದರ, param, flag, ತೂಕ)};
	Control_ಮೌಲ್ಯಗಳು seb_ದರ{calc_Pitch_Control_Seb_ದರ(ತೂಕ, pitch_specific_energy_ದರ)};

	_pitch_setpoint = _calcPitchControlOutput(ಒಳಹರಿವು, seb_ದರ, param);

	ste_ದರ_estimate_filter.reset(specific_energy_ದರ.spe_ದರ.estimate + specific_energy_ದರ.ske_ದರ.estimate);

	Control_ಮೌಲ್ಯಗಳು ste_ದರ{calc_Throttle_Control_Ste_ದರ(ಎಲ್ಲೆ_2, specific_energy_ದರ, param)};

	_throttle_setpoint = _calcThrottleControlOutput(ಎಲ್ಲೆ_2, ste_ದರ, param, flag);

	// Debug output
	_debug_output.ಒಟ್ಟು_energy_ದರ_estimate = ste_ದರ.estimate;
	_debug_output.ಒಟ್ಟು_energy_ದರ_sp = ste_ದರ.setpoint;
	_debug_output.throttle_integrator = _throttle_integ_state;
	_debug_output.energy_balance_ದರ_estimate = seb_ದರ.estimate;
	_debug_output.energy_balance_ದರ_sp = seb_ದರ.setpoint;
	_debug_output.pitch_integrator = _pitch_integ_state;
}

ಶೂನ್ಯ TECSControl::update(ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ dt, ಸ್ಥಿರಾಂಕ Setpoint &setpoint, ಸ್ಥಿರಾಂಕ ಒಳಹರಿವು_2 &ಒಳಹರಿವು, ಸ್ಥಿರಾಂಕ Param &param,
			 ಸ್ಥಿರಾಂಕ Flag &flag)
{
	// Input checking
	ಆದರೆ (!is_Valid_ಸಮಯ_ಹೆಜ್ಜೆ(dt)) {
		// Do not update the states and output.
		ಹಿಂತಿರುಗಿ;
	}

	ಸ್ಥಿರಾಂಕ STERate_ಎಲ್ಲೆ ಎಲ್ಲೆ_2{calculate_ಒಟ್ಟು_Energy_ದರ_ಎಲ್ಲೆ(param)};
	ಸ್ಥಿರಾಂಕ SpecificEnergyRates specific_energy_ದರ{_updateOuterLoops(setpoint, ಒಳಹರಿವು, ಎಲ್ಲೆ_2, param, flag)};

	_calcPitchControl(dt, setpoint, ಒಳಹರಿವು, ಎಲ್ಲೆ_2, specific_energy_ದರ, param, flag);

	_calcThrottleControl(dt, ಎಲ್ಲೆ_2, specific_energy_ದರ, param, flag);
}

TECSControl::SpecificEnergyRates TECSControl::_updateOuterLoops(ಸ್ಥಿರಾಂಕ Setpoint &setpoint, ಸ್ಥಿರಾಂಕ ಒಳಹರಿವು_2 &ಒಳಹರಿವು,
		ಸ್ಥಿರಾಂಕ STERate_ಎಲ್ಲೆ &ಎಲ್ಲೆ_2, ಸ್ಥಿರಾಂಕ Param &param, ಸ್ಥಿರಾಂಕ Flag &flag)
{
	OuterLoopSetpoints control_setpoint;

	control_setpoint.tas_ದರ_setpoint = _calcAirspeedControlOutput(setpoint, ಒಳಹರಿವು, ಎಲ್ಲೆ_2, param, flag);

	ಆದರೆ (PX4_ISFINITE(setpoint.altitude_ದರ_setpoint_ನೇರ)) {
		// direct height rate control
		control_setpoint.altitude_ದರ_setpoint = setpoint.altitude_ದರ_setpoint_ನೇರ;

	} ಇಲ್ಲದಿದ್ದರೆ {
		// altitude is locked, go through altitude outer loop
		control_setpoint.altitude_ದರ_setpoint = _calcAltitudeControlOutput(setpoint, ಒಳಹರಿವು, param);
	}

	ಸ್ಥಿರಾಂಕ SpecificEnergyRates specific_energy_ದರ{_calcSpecificEnergyRates(control_setpoint, ಒಳಹರಿವು)};

	_detectUnderspeed(ಒಳಹರಿವು, param, flag);

	_debug_output.altitude_ದರ_control = control_setpoint.altitude_ದರ_setpoint;
	_debug_output.true_airspeed_derivative_control = control_setpoint.tas_ದರ_setpoint;

	ಹಿಂತಿರುಗಿ specific_energy_ದರ;
}

TECSControl::STERate_ಎಲ್ಲೆ TECSControl::calculate_ಒಟ್ಟು_Energy_ದರ_ಎಲ್ಲೆ(ಸ್ಥಿರಾಂಕ Param &param) ಸ್ಥಿರಾಂಕ
{
	TECSControl::STERate_ಎಲ್ಲೆ ಎಲ್ಲೆ_2;
	// Calculate the specific total energy rate limits from the max throttle limits
	ಎಲ್ಲೆ_2.STE_ದರ_max = math::max(param.max_climb_ದರ, FLT_EPSILON) * CONSTANTS_ONE_G;
	ಎಲ್ಲೆ_2.STE_ದರ_min = - math::max(param.min_sink_ದರ, FLT_EPSILON) * CONSTANTS_ONE_G;

	ಹಿಂತಿರುಗಿ ಎಲ್ಲೆ_2;
}

ದಶಮಾಂಶ TECSControl::_calcAirspeedControlOutput(ಸ್ಥಿರಾಂಕ Setpoint &setpoint, ಸ್ಥಿರಾಂಕ ಒಳಹರಿವು_2 &ಒಳಹರಿವು, ಸ್ಥಿರಾಂಕ STERate_ಎಲ್ಲೆ &ಎಲ್ಲೆ_2,
		ಸ್ಥಿರಾಂಕ Param &param, ಸ್ಥಿರಾಂಕ Flag &flag) ಸ್ಥಿರಾಂಕ
{
	ದಶಮಾಂಶ airspeed_ದರ_output{0.0f};

	// calculate the demanded true airspeed rate of change based on first order response of true airspeed error
	// if airspeed measurement is not enabled then always set the rate setpoint to zero in order to avoid constant rate setpoints
	ಆದರೆ (flag.airspeed_enabled) {
		// Calculate limits for the demanded rate of change of speed based on physical performance limits
		// with a 50% margin to allow the total energy controller to correct for errors. Increase it in case of fast descend
		ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ max_tas_ದರ_sp = (param.fast_descend * 0.5f + 0.5f) * ಎಲ್ಲೆ_2.STE_ದರ_max / math::max(ಒಳಹರಿವು.tas,
					      FLT_EPSILON);
		ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ min_tas_ದರ_sp = (param.fast_descend * 0.5f + 0.5f) * ಎಲ್ಲೆ_2.STE_ದರ_min / math::max(ಒಳಹರಿವು.tas,
					      FLT_EPSILON);
		airspeed_ದರ_output = constrain((setpoint.tas_setpoint - ಒಳಹರಿವು.tas) * param.airspeed_error_gain, min_tas_ದರ_sp,
						 max_tas_ದರ_sp);
	}

	ಹಿಂತಿರುಗಿ airspeed_ದರ_output;
}

ದಶಮಾಂಶ TECSControl::_calcAltitudeControlOutput(ಸ್ಥಿರಾಂಕ Setpoint &setpoint, ಸ್ಥಿರಾಂಕ ಒಳಹರಿವು_2 &ಒಳಹರಿವು, ಸ್ಥಿರಾಂಕ Param &param) ಸ್ಥಿರಾಂಕ
{
	ದಶಮಾಂಶ altitude_ದರ_output;
	altitude_ದರ_output = (setpoint.altitude_reference.alt - ಒಳಹರಿವು.altitude) * param.altitude_error_gain
			       + param.altitude_setpoint_gain_ff * setpoint.altitude_reference.alt_ದರ;

	altitude_ದರ_output = math::constrain(altitude_ದರ_output, -param.max_sink_ದರ, param.max_climb_ದರ);

	ಹಿಂತಿರುಗಿ altitude_ದರ_output;
}

TECSControl::SpecificEnergyRates TECSControl::_calcSpecificEnergyRates(ಸ್ಥಿರಾಂಕ OuterLoopSetpoints &control_setpoint,
		ಸ್ಥಿರಾಂಕ ಒಳಹರಿವು_2 &ಒಳಹರಿವು) ಸ್ಥಿರಾಂಕ
{
	SpecificEnergyRates specific_energy_rates;
	// Calculate specific energy rate demands in units of (m**2/sec**3)
	specific_energy_rates.spe_ದರ.setpoint = control_setpoint.altitude_ದರ_setpoint *
			CONSTANTS_ONE_G; // potential energy rate of change

	// d/dt(0.5 V^2) = V * dV/dt: use measured TAS, not the TAS setpoint, to keep the airspeed loop linear
	specific_energy_rates.ske_ದರ.setpoint = ಒಳಹರಿವು.tas *
			control_setpoint.tas_ದರ_setpoint; // kinetic energy rate of change

	// Calculate specific energy rates in units of (m**2/sec**3)
	specific_energy_rates.spe_ದರ.estimate = ಒಳಹರಿವು.altitude_ದರ * CONSTANTS_ONE_G; // potential energy rate of change
	specific_energy_rates.ske_ದರ.estimate = ಒಳಹರಿವು.tas * ಒಳಹರಿವು.tas_ದರ;// kinetic energy rate of change

	ಹಿಂತಿರುಗಿ specific_energy_rates;
}

ಶೂನ್ಯ TECSControl::_detectUnderspeed(ಸ್ಥಿರಾಂಕ ಒಳಹರಿವು_2 &ಒಳಹರಿವು, ಸ್ಥಿರಾಂಕ Param &param, ಸ್ಥಿರಾಂಕ Flag &flag)
{
	ಆದರೆ (!flag.detect_underspeed_enabled || !flag.airspeed_enabled) {
		ಅನುಪಾತ_undersped = 0.0f;
		ಹಿಂತಿರುಗಿ;
	}

	// Ramp underspeed ratio from 0 at tas_min to 1 at 90% of tas_min, or halfway to tas_stall if that is higher
	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ tas_fully_undersped = math::min(param.tas_min,
					  math::max(0.9f * param.tas_min, 0.5f * (param.tas_min + param.tas_stall)));
	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ underspeed_ramp_ಅಗಲ = param.tas_min - tas_fully_undersped;

	ಅನುಪಾತ_undersped = math::constrain((param.tas_min - ಒಳಹರಿವು.tas) / math::max(underspeed_ramp_ಅಗಲ, FLT_EPSILON),
					   0.0f, 1.0f);
}

TECSControl::SpecificEnergyWeighting TECSControl::update_ವೇಗ_Altitude_Weights(ಸ್ಥಿರಾಂಕ Param &param, ಸ್ಥಿರಾಂಕ Flag &flag)
{

	SpecificEnergyWeighting ತೂಕ;
	// Calculate the weight applied to control of specific kinetic energy error
	ದಶಮಾಂಶ pitch_ವೇಗ_ತೂಕ = constrain(param.pitch_ವೇಗ_ತೂಕ, 0.0f, 2.0f);

	// Underspeed or fast descend: interpolate speed weight from nominal
	// towards max. We take the maximum of the two - this keeps the speed
	// weight continuous, even when underspeed and fast descend are combined
	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ max_ಅನುಪಾತ = fmaxf(ಅನುಪಾತ_undersped, param.fast_descend);
	pitch_ವೇಗ_ತೂಕ = max_ಅನುಪಾತ * 2.0f + (1.0f - max_ಅನುಪಾತ) * pitch_ವೇಗ_ತೂಕ;

	ಆದರೆ (!flag.airspeed_enabled) {
		pitch_ವೇಗ_ತೂಕ = 0.0f;
	}

	ತೂಕ.spe_weighting = constrain(2.0f - pitch_ವೇಗ_ತೂಕ, 0.f, 2.f);
	ತೂಕ.ske_weighting = constrain(pitch_ವೇಗ_ತೂಕ, 0.f, 2.f);

	ಹಿಂತಿರುಗಿ ತೂಕ;
}

ದಶಮಾಂಶ TECSControl::calc_Pitch_Control_Ske_ದರ_Setpoint(ಸ್ಥಿರಾಂಕ Setpoint &setpoint, ಸ್ಥಿರಾಂಕ ಒಳಹರಿವು_2 &ಒಳಹರಿವು,
		ಸ್ಥಿರಾಂಕ STERate_ಎಲ್ಲೆ &ಎಲ್ಲೆ_2, ಸ್ಥಿರಾಂಕ Param &param, ಸ್ಥಿರಾಂಕ Flag &flag, ಸ್ಥಿರಾಂಕ SpecificEnergyWeighting &ತೂಕ) ಸ್ಥಿರಾಂಕ
{
	// Trading altitude for speed with pitch is only wanted up to trim airspeed. Above trim, accelerating is left to
	// the throttle loop: if it saturates, the vehicle settles at a lower airspeed instead of below the altitude setpoint.
	// The airspeed setpoint seen by the pitch loop is therefore capped at max(trim, current airspeed). Between trim
	// and the setpoint the pitch loop holds altitude, while an overspeed above the setpoint is still corrected.
	// With a speed weight of 2 (speed priority, e.g. gliders, fast descend or underspeed) pitch is the only way to
	// track the setpoint, so the cap is faded out as the weight goes from 1 to 2.
	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ tas_setpoint_capped = min(setpoint.tas_setpoint, max(param.tas_trim, ಒಳಹರಿವು.tas));

	Setpoint pitch_control_setpoint{setpoint};
	pitch_control_setpoint.tas_setpoint = lerp(tas_setpoint_capped, setpoint.tas_setpoint,
					      constrain(ತೂಕ.ske_weighting - 1.f, 0.f, 1.f));

	// d/dt(0.5 V^2) = V * dV/dt: the measured airspeed, not the setpoint, keeps the airspeed loop linear
	ಹಿಂತಿರುಗಿ ಒಳಹರಿವು.tas * _calcAirspeedControlOutput(pitch_control_setpoint, ಒಳಹರಿವು, ಎಲ್ಲೆ_2, param, flag);
}

TECSControl::SpecificEnergyRates TECSControl::_calcPitchControlEnergyRates(ಸ್ಥಿರಾಂಕ Setpoint &setpoint,
		ಸ್ಥಿರಾಂಕ ಒಳಹರಿವು_2 &ಒಳಹರಿವು, ಸ್ಥಿರಾಂಕ STERate_ಎಲ್ಲೆ &ಎಲ್ಲೆ_2, ಸ್ಥಿರಾಂಕ SpecificEnergyRates &specific_energy_rates,
		ಸ್ಥಿರಾಂಕ Param &param, ಸ್ಥಿರಾಂಕ Flag &flag, ಸ್ಥಿರಾಂಕ SpecificEnergyWeighting &ತೂಕ) ಸ್ಥಿರಾಂಕ
{
	SpecificEnergyRates pitch_specific_energy_rates{specific_energy_rates};
	pitch_specific_energy_rates.ske_ದರ.setpoint = calc_Pitch_Control_Ske_ದರ_Setpoint(setpoint, ಒಳಹರಿವು, ಎಲ್ಲೆ_2, param, flag,
			ತೂಕ);

	// Between trim airspeed and the airspeed setpoint the throttle alone accelerates the vehicle. The capped setpoint
	// above already removes the kinetic setpoint of the pitch loop there, faded in with speed priority. But the
	// throttle's acceleration would still show up in the kinetic energy rate estimate, and the pitch loop would pitch
	// up to turn it into height. Remove the estimate from the pitch loop in this band as well, with the same fade, so
	// that pitch controls height only. The setpoint must not be scaled again, or it would be faded twice.
	ಸ್ಥಿರಾಂಕ ತಾರ್ಕಿಕ throttle_accelerates = ಒಳಹರಿವು.tas > param.tas_trim && ಒಳಹರಿವು.tas < setpoint.tas_setpoint;
	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ kinetic_estimate_gain = throttle_accelerates ? constrain(ತೂಕ.ske_weighting - 1.f, 0.f, 1.f) : 1.f;
	pitch_specific_energy_rates.ske_ದರ.estimate *= kinetic_estimate_gain;

	ಹಿಂತಿರುಗಿ pitch_specific_energy_rates;
}

ಶೂನ್ಯ TECSControl::_calcPitchControl(ದಶಮಾಂಶ dt, ಸ್ಥಿರಾಂಕ Setpoint &setpoint, ಸ್ಥಿರಾಂಕ ಒಳಹರಿವು_2 &ಒಳಹರಿವು, ಸ್ಥಿರಾಂಕ STERate_ಎಲ್ಲೆ &ಎಲ್ಲೆ_2,
				    ಸ್ಥಿರಾಂಕ SpecificEnergyRates &specific_energy_rates, ಸ್ಥಿರಾಂಕ Param &param, ಸ್ಥಿರಾಂಕ Flag &flag)
{
	ಸ್ಥಿರಾಂಕ SpecificEnergyWeighting ತೂಕ{update_ವೇಗ_Altitude_Weights(param, flag)};

	ಸ್ಥಿರಾಂಕ SpecificEnergyRates pitch_specific_energy_rates{_calcPitchControlEnergyRates(setpoint, ಒಳಹರಿವು, ಎಲ್ಲೆ_2,
			specific_energy_rates, param, flag, ತೂಕ)};

	Control_ಮೌಲ್ಯಗಳು seb_ದರ{calc_Pitch_Control_Seb_ದರ(ತೂಕ, pitch_specific_energy_rates)};

	_calcPitchControlUpdate(dt, ಒಳಹರಿವು, seb_ದರ, param);
	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ pitch_setpoint{_calcPitchControlOutput(ಒಳಹರಿವು, seb_ದರ, param)};

	// Comply with the specified vertical acceleration limit by applying a pitch rate limit
	// NOTE: at zero airspeed, the pitch increment is unbounded
	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ pitch_increment = dt * param.vert_accel_ಎಲ್ಲೆ / math::max(ಒಳಹರಿವು.tas, FLT_EPSILON);
	_pitch_setpoint = constrain(pitch_setpoint, _pitch_setpoint - pitch_increment,
				    _pitch_setpoint + pitch_increment);
	_pitch_setpoint = constrain(_pitch_setpoint, param.pitch_min, param.pitch_max);

	//Debug Output
	_debug_output.energy_balance_ದರ_estimate = seb_ದರ.estimate;
	_debug_output.energy_balance_ದರ_sp = seb_ದರ.setpoint;
	_debug_output.pitch_integrator = _pitch_integ_state;
}

TECSControl::Control_ಮೌಲ್ಯಗಳು TECSControl::calc_Pitch_Control_Seb_ದರ(ಸ್ಥಿರಾಂಕ SpecificEnergyWeighting &ತೂಕ,
		ಸ್ಥಿರಾಂಕ SpecificEnergyRates &specific_energy_rates) ಸ್ಥಿರಾಂಕ
{
	Control_ಮೌಲ್ಯಗಳು seb_ದರ;
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
	seb_ದರ.setpoint = specific_energy_rates.spe_ದರ.setpoint * ತೂಕ.spe_weighting -
			    specific_energy_rates.ske_ದರ.setpoint *
			    ತೂಕ.ske_weighting;

	seb_ದರ.estimate = (specific_energy_rates.spe_ದರ.estimate * ತೂಕ.spe_weighting) -
			    (specific_energy_rates.ske_ದರ.estimate * ತೂಕ.ske_weighting);

	ಹಿಂತಿರುಗಿ seb_ದರ;
}

ಶೂನ್ಯ TECSControl::_calcPitchControlUpdate(ದಶಮಾಂಶ dt, ಸ್ಥಿರಾಂಕ ಒಳಹರಿವು_2 &ಒಳಹರಿವು, ಸ್ಥಿರಾಂಕ Control_ಮೌಲ್ಯಗಳು &seb_ದರ,
		ಸ್ಥಿರಾಂಕ Param &param)
{

	// In fast descend, the pitch integrator converges to the (substantially
	// negative) pitch. Ramp it down with decreasing fast descend ratio,
	// such that it is zero when the ratio becomes 0 again.
	ಆದರೆ (param.fast_descend < _fast_descend_prev && _fast_descend_prev > FLT_EPSILON) {
		_pitch_integ_state *= param.fast_descend / _fast_descend_prev;
	}

	_fast_descend_prev = param.fast_descend;

	ಆದರೆ (param.integrator_gain_pitch > FLT_EPSILON) {

		ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ climb_angle_ಗಮ್ಯ_SEB_ದರ = climb_Angle_ಗಮ್ಯ_Seb_ದರ(ಒಳಹರಿವು, param);

		// Calculate pitch integrator input term
		ದಶಮಾಂಶ pitch_integ_ಒಳಹರಿವು = _getControlError(seb_ದರ) * param.integrator_gain_pitch / climb_angle_ಗಮ್ಯ_SEB_ದರ;

		// Guard against NaN integrator input - discard bad input sample (e.g., exiting offboard velocity mode)
		ಆದರೆ (!PX4_ISFINITE(pitch_integ_ಒಳಹರಿವು)) {
			pitch_integ_ಒಳಹರಿವು = 0.f;
		}

		// Prevent the integrator changing in a direction that will increase pitch demand saturation
		ಆದರೆ (_pitch_setpoint >= param.pitch_max) {
			pitch_integ_ಒಳಹರಿವು = min(pitch_integ_ಒಳಹರಿವು, 0.f);

		} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (_pitch_setpoint <= param.pitch_min) {
			pitch_integ_ಒಳಹರಿವು = max(pitch_integ_ಒಳಹರಿವು, 0.f);
		}

		// Update the pitch integrator state
		_pitch_integ_state = _pitch_integ_state + pitch_integ_ಒಳಹರಿವು * dt;

		// Safety check: reset integrator if state became corrupted
		ಆದರೆ (!PX4_ISFINITE(_pitch_integ_state)) {
			_pitch_integ_state = 0.f;
		}

	} ಇಲ್ಲದಿದ್ದರೆ {
		_pitch_integ_state = 0.0f;
	}
}

ದಶಮಾಂಶ TECSControl::climb_Angle_ಗಮ್ಯ_Seb_ದರ(ಸ್ಥಿರಾಂಕ ಒಳಹರಿವು_2 &ಒಳಹರಿವು, ಸ್ಥಿರಾಂಕ Param &param)
{
	// max() returns its second argument for a NaN first one, so a NaN airspeed falls back to the minimum airspeed
	ಹಿಂತಿರುಗಿ max(ಒಳಹರಿವು.tas, param.tas_min, FLT_EPSILON) * CONSTANTS_ONE_G;
}

ದಶಮಾಂಶ TECSControl::_calcPitchControlOutput(ಸ್ಥಿರಾಂಕ ಒಳಹರಿವು_2 &ಒಳಹರಿವು, ಸ್ಥಿರಾಂಕ Control_ಮೌಲ್ಯಗಳು &seb_ದರ, ಸ್ಥಿರಾಂಕ Param &param) ಸ್ಥಿರಾಂಕ
{
	// Calculate a specific energy correction that doesn't include the integrator contribution
	ದಶಮಾಂಶ SEB_ದರ_correction = _getControlError(seb_ದರ) * param.pitch_damping_gain +
				    param.seb_ದರ_ff *
				    seb_ದರ.setpoint;

	// Convert the specific energy balance rate correction to a climb angle and add the level-flight pitch, the angle
	// of attack at zero climb angle, to get the absolute pitch setpoint. This calculation assumes:
	// a) The climb angle follows pitch angle with a lag that is small enough not to destabilise the control loop.
	// b) The offset between climb angle and pitch angle (angle of attack) is constant and equal to the level-flight
	// pitch, excluding the effect of pitch transients due to control action or turbulence.
	ದಶಮಾಂಶ pitch_setpoint_unc = SEB_ದರ_correction / climb_Angle_ಗಮ್ಯ_Seb_ದರ(ಒಳಹರಿವು, param) + _pitch_integ_state
				   + param.level_flight_pitch;

	// Guard against a non-finite feedforward/damping contribution (e.g. seb_rate going NaN on the energy/speed
	// side when exiting offboard velocity mode). constrain() does not reject NaN, so fall back to the integrator
	// state, which is kept finite by _calcPitchControlUpdate, to avoid latching a NaN pitch demand.
	ಆದರೆ (!PX4_ISFINITE(pitch_setpoint_unc)) {
		pitch_setpoint_unc = _pitch_integ_state + param.level_flight_pitch;
	}

	ಹಿಂತಿರುಗಿ constrain(pitch_setpoint_unc, param.pitch_min, param.pitch_max);
}

ಶೂನ್ಯ TECSControl::_calcThrottleControl(ದಶಮಾಂಶ dt, ಸ್ಥಿರಾಂಕ STERate_ಎಲ್ಲೆ &ಎಲ್ಲೆ_2,
				       ಸ್ಥಿರಾಂಕ SpecificEnergyRates &specific_energy_rates, ಸ್ಥಿರಾಂಕ Param &param, ಸ್ಥಿರಾಂಕ Flag &flag)
{
	// Update STE rate estimate LP filter
	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ STE_ದರ_estimate_raw = specific_energy_rates.spe_ದರ.estimate + specific_energy_rates.ske_ದರ.estimate;
	ste_ದರ_estimate_filter.ಗಣ_Parameters(dt, param.ste_ದರ_ಸಮಯ_const);
	ste_ದರ_estimate_filter.update(STE_ದರ_estimate_raw);
	Control_ಮೌಲ್ಯಗಳು ste_ದರ{calc_Throttle_Control_Ste_ದರ(ಎಲ್ಲೆ_2, specific_energy_rates, param)};
	// During fast descend, we control airspeed over the pitch control loop. Give minimal thrust as soon as we are descending
	ದಶಮಾಂಶ throttle_setpoint{param.throttle_min};

	ಆದರೆ (!fastDescendFullyEngaged(param.fast_descend)) {
		_calcThrottleControlUpdate(dt, ಎಲ್ಲೆ_2, ste_ದರ, param, flag);
		throttle_setpoint = (1.f - param.fast_descend) * _calcThrottleControlOutput(ಎಲ್ಲೆ_2, ste_ದರ, param,
				    flag) + param.fast_descend * param.throttle_min;
	}

	// Rate limit the throttle demand
	ಆದರೆ (fabsf(param.throttle_slewrate) > FLT_EPSILON) {
		ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ throttle_increment_ಎಲ್ಲೆ = dt * (param.throttle_max - param.throttle_min) * param.throttle_slewrate;
		throttle_setpoint = constrain(throttle_setpoint, _throttle_setpoint - throttle_increment_ಎಲ್ಲೆ,
					      _throttle_setpoint + throttle_increment_ಎಲ್ಲೆ);
	}

	_throttle_setpoint = constrain(throttle_setpoint, param.throttle_min, param.throttle_max);

	// Debug output
	_debug_output.ಒಟ್ಟು_energy_ದರ_estimate = ste_ದರ.estimate;
	_debug_output.ಒಟ್ಟು_energy_ದರ_sp = ste_ದರ.setpoint;
	_debug_output.throttle_integrator = _throttle_integ_state;
}

TECSControl::Control_ಮೌಲ್ಯಗಳು TECSControl::calc_Throttle_Control_Ste_ದರ(ಸ್ಥಿರಾಂಕ STERate_ಎಲ್ಲೆ &ಎಲ್ಲೆ_2,
		ಸ್ಥಿರಾಂಕ SpecificEnergyRates &specific_energy_rates,
		ಸ್ಥಿರಾಂಕ Param &param) ಸ್ಥಿರಾಂಕ
{
	// Output ste rate values
	Control_ಮೌಲ್ಯಗಳು ste_ದರ;
	ste_ದರ.setpoint = specific_energy_rates.spe_ದರ.setpoint + specific_energy_rates.ske_ದರ.setpoint;

	// Adjust the demanded total energy rate to compensate for induced drag rise in turns.
	// Assume induced drag scales linearly with normal load factor.
	// The additional normal load factor is given by (1/cos(bank angle) - 1)
	ste_ದರ.setpoint += param.ತುಂಬು_ಗುಣಕ_correction * (param.ತುಂಬು_ಗುಣಕ - 1.f);

	ste_ದರ.setpoint = constrain(ste_ದರ.setpoint, ಎಲ್ಲೆ_2.STE_ದರ_min, ಎಲ್ಲೆ_2.STE_ದರ_max);
	ste_ದರ.estimate = ste_ದರ_estimate_filter.getState();

	ಹಿಂತಿರುಗಿ ste_ದರ;
}

ಶೂನ್ಯ TECSControl::_calcThrottleControlUpdate(ದಶಮಾಂಶ dt, ಸ್ಥಿರಾಂಕ STERate_ಎಲ್ಲೆ &ಎಲ್ಲೆ_2, ಸ್ಥಿರಾಂಕ Control_ಮೌಲ್ಯಗಳು &ste_ದರ,
		ಸ್ಥಿರಾಂಕ Param &param, ಸ್ಥಿರಾಂಕ Flag &flag)
{
	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ STE_ದರ_ಗಮ್ಯ_throttle = ste_ದರ_ಗಮ್ಯ_Throttle(ಎಲ್ಲೆ_2);

	// Integral handling
	ಆದರೆ (flag.airspeed_enabled) {
		ಆದರೆ (param.integrator_gain_throttle > FLT_EPSILON) {
			// underspeed conditions zero out integration
			ದಶಮಾಂಶ throttle_integ_ಒಳಹರಿವು = (_getControlError(ste_ದರ) * param.integrator_gain_throttle) * dt *
						     STE_ದರ_ಗಮ್ಯ_throttle * (1.0f - ಅನುಪಾತ_undersped);

			// only allow integrator propagation into direction which unsaturates throttle
			ಆದರೆ (_throttle_setpoint >= param.throttle_max) {
				throttle_integ_ಒಳಹರಿವು = math::min(0.f, throttle_integ_ಒಳಹರಿವು);

			}

			ಆದರೆ (_throttle_setpoint <= param.throttle_min) {
				throttle_integ_ಒಳಹರಿವು = math::max(0.f, throttle_integ_ಒಳಹರಿವು);
			}

			// Calculate a throttle demand from the integrated total energy rate error
			// This will be added to the total throttle demand to compensate for steady state errors
			_throttle_integ_state = PX4_ISFINITE(throttle_integ_ಒಳಹರಿವು) ? _throttle_integ_state + throttle_integ_ಒಳಹರಿವು : 0.f;

		} ಇಲ್ಲದಿದ್ದರೆ {
			_throttle_integ_state = 0.0f;
		}

	}
}

ದಶಮಾಂಶ TECSControl::_calcThrottleControlOutput(ಸ್ಥಿರಾಂಕ STERate_ಎಲ್ಲೆ &ಎಲ್ಲೆ_2, ಸ್ಥಿರಾಂಕ Control_ಮೌಲ್ಯಗಳು &ste_ದರ,
		ಸ್ಥಿರಾಂಕ Param &param,
		ಸ್ಥಿರಾಂಕ Flag &flag) ಸ್ಥಿರಾಂಕ
{
	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ STE_ದರ_ಗಮ್ಯ_throttle = ste_ದರ_ಗಮ್ಯ_Throttle(ಎಲ್ಲೆ_2);

	// Calculate a predicted throttle from the demanded rate of change of energy, using the cruise throttle
	// as the starting point. Assume:
	// Specific total energy rate = _STE_rate_max is achieved when throttle is set to _throttle_setpoint_max
	// Specific total energy rate = 0 at cruise throttle
	// Specific total energy rate = _STE_rate_min is achieved when throttle is set to _throttle_setpoint_min

	// assume airspeed and density-independent delta_throttle to sink/climb rate mapping
	// TODO: include air density for thrust mappings
	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ throttle_above_trim_ಪ್ರತಿ_ste_ದರ = (param.throttle_max - param.throttle_trim) / ಎಲ್ಲೆ_2.STE_ದರ_max;
	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ throttle_ಕೆಳಗೆ_trim_ಪ್ರತಿ_ste_ದರ = (param.throttle_trim - param.throttle_min) / ಎಲ್ಲೆ_2.STE_ದರ_min;

	ದಶಮಾಂಶ throttle_predicted = 0.0f;

	ಆದರೆ (ste_ದರ.setpoint >= FLT_EPSILON) {
		// throttle is between trim and maximum
		throttle_predicted = param.throttle_trim + ste_ದರ.setpoint * throttle_above_trim_ಪ್ರತಿ_ste_ದರ;

	} ಇಲ್ಲದಿದ್ದರೆ {
		// throttle is between trim and minimum
		throttle_predicted = param.throttle_trim - ste_ದರ.setpoint * throttle_ಕೆಳಗೆ_trim_ಪ್ರತಿ_ste_ದರ;

	}

	// Add proportional and derivative control feedback to the predicted throttle and constrain to throttle limits
	ದಶಮಾಂಶ throttle_setpoint = (_getControlError(ste_ದರ) * param.throttle_damping_gain) * STE_ದರ_ಗಮ್ಯ_throttle +
				  throttle_predicted;

	ಆದರೆ (flag.airspeed_enabled) {
		// Add the integrator feedback during closed loop operation with an airspeed sensor
		throttle_setpoint += _throttle_integ_state;

	} ಇಲ್ಲದಿದ್ದರೆ {
		/* We want to avoid reducing the throttle output when switching from airspeed enabled mode into airspeedless mode.
		  Thus, if the throttle integrator has a positive value, add it still to the throttle setpoint. */
		throttle_setpoint += math::max(0.0f, _throttle_integ_state);
	}


	// ramp in max throttle setting with underspeediness value
	throttle_setpoint = ಅನುಪಾತ_undersped * param.throttle_max + (1.0f - ಅನುಪಾತ_undersped) * throttle_setpoint;

	ಹಿಂತಿರುಗಿ constrain(throttle_setpoint, param.throttle_min, param.throttle_max);
}

ಶೂನ್ಯ TECSControl::resetIntegrals()
{
	_pitch_integ_state = 0.0f;
	_throttle_integ_state = 0.0f;
}

ಶೂನ್ಯ TECS::initControlParams(ಸ್ಥಿರಾಂಕ ಒಳಹರಿವು_2 &ಒಳಹರಿವು)
{
	// Update parameters from input
	// Reference model
	_reference_param.ಗುರಿ_climbrate = ಒಳಹರಿವು.ಗುರಿ_climbrate;
	_reference_param.ಗುರಿ_sinkrate = ಒಳಹರಿವು.ಗುರಿ_sinkrate;
	// Control
	_control_param.tas_min = ಒಳಹರಿವು.eas_ಗಮ್ಯ_tas * _equivalent_airspeed_min;
	_control_param.tas_stall = ಒಳಹರಿವು.eas_ಗಮ್ಯ_tas * _equivalent_airspeed_stall;
	_control_param.tas_max = ಒಳಹರಿವು.eas_ಗಮ್ಯ_tas * _equivalent_airspeed_max;
	_control_param.tas_trim = ಒಳಹರಿವು.eas_ಗಮ್ಯ_tas * _control_param.equivalent_airspeed_trim;
	_control_param.pitch_max = ಒಳಹರಿವು.pitch_max;
	_control_param.pitch_min = ಒಳಹರಿವು.pitch_min;
	_control_param.throttle_trim = ಒಳಹರಿವು.throttle_trim;
	_control_param.throttle_max = ಒಳಹರಿವು.throttle_max;
	_control_param.throttle_min = ಒಳಹರಿವು.throttle_min;
}

ದಶಮಾಂಶ TECS::calcTrueAirspeedSetpoint(ದಶಮಾಂಶ eas_ಗಮ್ಯ_tas, ದಶಮಾಂಶ eas_setpoint)
{
	ಹಿಂತಿರುಗಿ lerp(eas_ಗಮ್ಯ_tas * eas_setpoint, _control_param.tas_max, _fast_descend);
}

ಶೂನ್ಯ TECS::initialize(ಸ್ಥಿರಾಂಕ ಒಳಹರಿವು_2 &ಒಳಹರಿವು)
{
	// Init subclasses
	TECSAltitudeReferenceModel::AltitudeReferenceState current_state{.alt = ಒಳಹರಿವು.altitude,
			.alt_ದರ = ಒಳಹರಿವು.altitude_ದರ};
	_altitude_reference_model.initialize(current_state);
	_airspeed_filter.initialize(ಒಳಹರಿವು.equivalent_airspeed, _airspeed_filter_param.equivalent_airspeed_trim,
				    _control_flag.airspeed_enabled);

	// Seed the controller from the filter, which falls back to trim airspeed without a valid measurement,
	// so the controller sees the same airspeed on reset as on every following update
	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ true_airspeed = ಒಳಹರಿವು.eas_ಗಮ್ಯ_tas * _airspeed_filter.getState().ವೇಗ;

	TECSControl::Setpoint control_setpoint;
	control_setpoint.altitude_reference = _altitude_reference_model.getAltitudeReference();
	control_setpoint.altitude_ದರ_setpoint_ನೇರ = NAN; // initialization always goes through the altitude loop
	control_setpoint.tas_setpoint = true_airspeed;

	ಸ್ಥಿರಾಂಕ TECSControl::ಒಳಹರಿವು_2 control_ಒಳಹರಿವು{ .altitude = ಒಳಹರಿವು.altitude,
						.altitude_ದರ = ಒಳಹರಿವು.altitude_ದರ,
						.tas = true_airspeed,
						.tas_ದರ = 0.0f};

	_control.initialize(control_setpoint, control_ಒಳಹರಿವು, _control_param, _control_flag);

	_fast_descend = 0.f;
	_enabled_fast_descend_timestamp = 0U;
}

ಶೂನ್ಯ TECS::update(ಸ್ಥಿರಾಂಕ ಒಳಹರಿವು_2 &ಒಳಹರಿವು)
{
	// Calculate the time since last update (seconds)
	ಸ್ಥಿರಾಂಕ hrt_abstime now = hrt_absolute_ಸಮಯ();
	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ dt = static_cast<ದಶಮಾಂಶ>(now - _update_timestamp) * 1e-6f;

	initControlParams(ಒಳಹರಿವು);

	ಆದರೆ (dt < DT_MIN) {
		// Update interval too small, do not update. Assume constant states/output in this case.
		ಹಿಂತಿರುಗಿ;
	}

	ದಶಮಾಂಶ true_airspeed_setpoint;

	ಆದರೆ (dt > DT_MAX || _update_timestamp == 0UL) {
		// Update time interval too large, can't guarantee sanity of state updates anymore. reset the control loop.
		initialize(ಒಳಹರಿವು);
		true_airspeed_setpoint = calcTrueAirspeedSetpoint(ಒಳಹರಿವು.eas_ಗಮ್ಯ_tas, ಒಳಹರಿವು.equivalent_airspeed_setpoint);

	} ಇಲ್ಲದಿದ್ದರೆ {
		/* Check if we want to fast descend. On fast descend, we set the throttle to min, and use the altitude control
		loop to control the speed to the maximum airspeed. */
		ಗಣ_Fast_Descend(ಒಳಹರಿವು.altitude_setpoint, ಒಳಹರಿವು.altitude);
		_control_param.fast_descend = _fast_descend;

		// Update airspeedfilter submodule
		ಸ್ಥಿರಾಂಕ TECSAirspeedFilter::ಒಳಹರಿವು_2 airspeed_ಒಳಹರಿವು{ .equivalent_airspeed = ಒಳಹರಿವು.equivalent_airspeed,
				.equivalent_airspeed_ದರ = ಒಳಹರಿವು.ವೇಗ_deriv_forward / ಒಳಹರಿವು.eas_ಗಮ್ಯ_tas};

		_airspeed_filter.update(dt, airspeed_ಒಳಹರಿವು, _airspeed_filter_param, _control_flag.airspeed_enabled);

		// Update Reference model submodule
		ಆದರೆ (fastDescendFullyEngaged(_fast_descend)) {
			// Reset the altitude reference model, while we are in fast descend.
			ಸ್ಥಿರಾಂಕ TECSAltitudeReferenceModel::AltitudeReferenceState init_state{
				.alt = ಒಳಹರಿವು.altitude,
				.alt_ದರ = ಒಳಹರಿವು.altitude_ದರ};
			_altitude_reference_model.initialize(init_state);

		} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (_control.get_Underspeed_ಅನುಪಾತ() > 0.5f) {
			// Reset altitude reference during underspeed to prevent altitude error accumulation
			// that would cause aggressive pitch-up (and potential secondary stall) on recovery.
			ಸ್ಥಿರಾಂಕ TECSAltitudeReferenceModel::AltitudeReferenceState init_state{
				.alt = ಒಳಹರಿವು.altitude,
				.alt_ದರ = ಒಳಹರಿವು.altitude_ದರ};
			_altitude_reference_model.initialize(init_state);

		} ಇಲ್ಲದಿದ್ದರೆ {
			ಸ್ಥಿರಾಂಕ TECSAltitudeReferenceModel::AltitudeReferenceState setpoint{ .alt = ಒಳಹರಿವು.altitude_setpoint,
					.alt_ದರ = ಒಳಹರಿವು.altitude_ದರ_setpoint};

			_altitude_reference_model.update(dt, setpoint, ಒಳಹರಿವು.altitude, ಒಳಹರಿವು.altitude_ದರ, _reference_param);
		}

		TECSControl::Setpoint control_setpoint;
		control_setpoint.altitude_reference = _altitude_reference_model.getAltitudeReference();
		control_setpoint.altitude_ದರ_setpoint_ನೇರ = _altitude_reference_model.get_ಎತ್ತರ_ದರ_Setpoint_ನೇರ();
		true_airspeed_setpoint = calcTrueAirspeedSetpoint(ಒಳಹರಿವು.eas_ಗಮ್ಯ_tas, ಒಳಹರಿವು.equivalent_airspeed_setpoint);
		control_setpoint.tas_setpoint = true_airspeed_setpoint;

		ಸ್ಥಿರಾಂಕ TECSAirspeedFilter::AirspeedFilterState airspeed_state{_airspeed_filter.getState()};
		ಸ್ಥಿರಾಂಕ TECSControl::ಒಳಹರಿವು_2 control_ಒಳಹರಿವು{ .altitude = ಒಳಹರಿವು.altitude,
							.altitude_ದರ = ಒಳಹರಿವು.altitude_ದರ,
							.tas = ಒಳಹರಿವು.eas_ಗಮ್ಯ_tas * airspeed_state.ವೇಗ,
							.tas_ದರ = ಒಳಹರಿವು.eas_ಗಮ್ಯ_tas * airspeed_state.ವೇಗ_ದರ};

		_control.update(dt, control_setpoint, control_ಒಳಹರಿವು, _control_param, _control_flag);
	}

	debug_ಸ್ಥಿತಿ.control = _control.getDebugOutput();
	debug_ಸ್ಥಿತಿ.true_airspeed_sp = true_airspeed_setpoint;
	ಸ್ಥಿರಾಂಕ TECSAirspeedFilter::AirspeedFilterState airspeed_state{_airspeed_filter.getState()};
	ಸ್ಥಿರಾಂಕ TECSAltitudeReferenceModel::AltitudeReferenceState altitude_reference{_altitude_reference_model.getAltitudeReference()};
	debug_ಸ್ಥಿತಿ.true_airspeed_filtered = ಒಳಹರಿವು.eas_ಗಮ್ಯ_tas * airspeed_state.ವೇಗ;
	debug_ಸ್ಥಿತಿ.true_airspeed_derivative = ಒಳಹರಿವು.eas_ಗಮ್ಯ_tas * airspeed_state.ವೇಗ_ದರ;
	debug_ಸ್ಥಿತಿ.altitude_reference = altitude_reference.alt;
	debug_ಸ್ಥಿತಿ.ಎತ್ತರ_ದರ_reference = altitude_reference.alt_ದರ;
	debug_ಸ್ಥಿತಿ.ಎತ್ತರ_ದರ_ನೇರ = _altitude_reference_model.get_ಎತ್ತರ_ದರ_Setpoint_ನೇರ();
	debug_ಸ್ಥಿತಿ.fast_descend = _fast_descend;

	_update_timestamp = now;
}

ಶೂನ್ಯ TECS::ಗಣ_Fast_Descend(ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ alt_setpoint, ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ alt)
{
	// disable fast descend if we are close to the target altitude or the altitude setpoint is not finite

	ಆದರೆ (PX4_ISFINITE(alt_setpoint) && _control_flag.airspeed_enabled && (_fast_descend_alt_err > FLT_EPSILON)
	    && ((alt_setpoint + _fast_descend_alt_err) < alt)) {
		ಸ್ವಯಂಚಾಲಿತ now = hrt_absolute_ಸಮಯ();

		ಆದರೆ (_enabled_fast_descend_timestamp == 0U) {
			_enabled_fast_descend_timestamp = now;
		}

		_fast_descend = constrain(max(_fast_descend, static_cast<ದಶಮಾಂಶ>(now - _enabled_fast_descend_timestamp) /
					      static_cast<ದಶಮಾಂಶ>(FAST_DESCEND_RAMP_ಮೇಲೆ_ಸಮಯ)), 0.f, 1.f);

	} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (PX4_ISFINITE(alt_setpoint) && (_fast_descend > FLT_EPSILON) && (_fast_descend_alt_err > FLT_EPSILON)) {
		// We're in fast descend, scale it down until FAST_DESCEND_HEIGHT_MARGIN above target altitude
		_fast_descend = constrain((alt - alt_setpoint - FAST_DESCEND_ಎತ್ತರ_MARGIN) / _fast_descend_alt_err, 0.f, 1.f);
		_enabled_fast_descend_timestamp = 0U;

	} ಇಲ್ಲದಿದ್ದರೆ {
		_fast_descend = 0.f;
		_enabled_fast_descend_timestamp = 0U;
	}
}

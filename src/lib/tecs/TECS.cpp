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

#ضمن "TECS.hpp"

#ضمن <lib/geo/geo.h>

#ضمن <px4_platform_common/defines.h>

استعمال math::constrain;
استعمال math::max;
استعمال math::min;

ساكن constexpr منطقي is_Valid_وقت_قدم(عائم dt) { أرجع (PX4_ISFINITE(dt) && dt > FLT_EPSILON);}
ساكن constexpr منطقي fastDescendFullyEngaged(عائم fast_descend) { أرجع 1.f - fast_descend < FLT_EPSILON; }

فراغ TECSAirspeedFilter::initialize(ثابت عائم equivalent_airspeed, ثابت عائم equivalent_airspeed_trim,
				    ثابت منطقي airspeed_sensor_موجود)
{
	إذا (airspeed_sensor_موجود && PX4_ISFINITE(equivalent_airspeed)) {
		_airspeed_state.رفتار = equivalent_airspeed;

	} وإلا {
		_airspeed_state.رفتار = equivalent_airspeed_trim;
	}

	_airspeed_state.رفتار_شرح = 0.f;
}

فراغ TECSAirspeedFilter::update(ثابت عائم dt, ثابت داخلا_2 &داخلا, ثابت Param &param,
				ثابت منطقي airspeed_sensor_موجود)
{
	// Input checking
	إذا (!is_Valid_وقت_قدم(dt)) {
		// Do not update the states.
		أرجع;
	}

	عائم airspeed;

	إذا (PX4_ISFINITE(داخلا.equivalent_airspeed) && airspeed_sensor_موجود) {
		airspeed = داخلا.equivalent_airspeed;

	} وإلا {
		airspeed = param.equivalent_airspeed_trim;
	}

	عائم airspeed_derivative;

	إذا (PX4_ISFINITE(داخلا.equivalent_airspeed_شرح) && airspeed_sensor_موجود) {
		airspeed_derivative = داخلا.equivalent_airspeed_شرح;

	} وإلا {
		airspeed_derivative = 0.0f;
	}

	/* Filter airspeed and rate using a constant airspeed rate model in a steady state Kalman Filter.
	 We use the gains of the continuous Kalman filter Kc and approximate the discrete version Kalman gain Kd =dt*Kc,
	 since the continuous algebraic Riccatti equation is easier to solve.
	*/

	ميٽرڪس::Vector2f new_state_predicted;

	new_state_predicted(0) = _airspeed_state.رفتار + dt * _airspeed_state.رفتار_شرح;
	new_state_predicted(1) = _airspeed_state.رفتار_شرح;

	ثابت ميٽرڪس::Vector2f innovation{(airspeed - new_state_predicted(0)), (airspeed_derivative - new_state_predicted(1))};

	ميٽرڪس::Vector2f new_state;
	new_state(0) = new_state_predicted(0) + dt * (kKg00 * innovation(0) + kKg01 * innovation(1));
	new_state(1) = new_state_predicted(1) + dt * (kKg10 * innovation(0) + kKg11 * innovation(1));

	// Clip airspeed at zero
	إذا (new_state(0) < FLT_EPSILON) {
		new_state(0) = 0.0f;
		// calculate input that would result in zero speed.
		ثابت عائم desired_airspeed_innovation = (-new_state_predicted(0) / dt - kKg01 * innovation(1)) / kKg00;
		new_state(1) = new_state_predicted(1) + dt * (kKg10 * desired_airspeed_innovation + kKg11 * innovation(1));
	}

	// Update states
	_airspeed_state.رفتار = new_state(0);
	_airspeed_state.رفتار_شرح = new_state(1);
}

TECSAirspeedFilter::AirspeedFilterState TECSAirspeedFilter::getState() ثابت
{
	أرجع _airspeed_state;
}

فراغ TECSAltitudeReferenceModel::update(ثابت عائم dt, ثابت AltitudeReferenceState &setpoint, عائم altitude,
					عائم اوچائي_شرح, ثابت Param &param)
{
	// Input checks
	إذا (!is_Valid_وقت_قدم(dt)) {
		// Do not update the states.
		أرجع;
	}

	ثابت عائم current_alt = PX4_ISFINITE(altitude) ? altitude : 0.f;

	_velocity_control_traj_generator.سيٽ_Max_Jerk(JERK_MAX);
	_velocity_control_traj_generator.سيٽ_Max_Accel_مٿي(param.vert_accel_حد_بندي);
	_velocity_control_traj_generator.سيٽ_Max_Accel_هيٺ_ڏانهن(param.vert_accel_حد_بندي);
	_velocity_control_traj_generator.سيٽ_Max_Vel_مٿي(param.max_sink_شرح); // different convention for FW than for MC
	_velocity_control_traj_generator.سيٽ_Max_Vel_هيٺ_ڏانهن(param.max_climb_شرح); // different convention for FW than for MC

	// Altitude setpoint reference
	_alt_control_traj_generator.سيٽ_Max_Jerk(JERK_MAX);
	_alt_control_traj_generator.سيٽ_Max_Accel(param.vert_accel_حد_بندي);
	_alt_control_traj_generator.سيٽ_Max_Vel(fmaxf(param.max_climb_شرح, param.max_sink_شرح));

	// XXX: this is a bit risky.. .alt_rate here could be NAN (by interface design) - and is only ok to input to the
	// setVelSpFeedback() method because it calls the reset in the logic below when it is NAN.
	// TODO: stop it with the NAN interfaces, make sure to take care of this when refactoring and separating altitude
	// and height rate control loops.
	_velocity_control_traj_generator.سيٽ_Vel_Sp_Feedback(setpoint.alt_شرح);

	منطقي control_altitude = سچ;
	عائم altitude_setpoint = setpoint.alt;

	إذا (!PX4_ISFINITE(setpoint.alt) && !PX4_ISFINITE(setpoint.alt_شرح)) {
		// neither altitude nor altitude rate is set - reset to current altitude
		_velocity_control_traj_generator.reset(0.f, 0, current_alt);
		altitude_setpoint = current_alt;

	} وإلا إذا (PX4_ISFINITE(setpoint.alt_شرح)) {
		// input is height rate (not altitude)
		_velocity_control_traj_generator.سيٽ_Current_Position_Estimate(current_alt);
		_velocity_control_traj_generator.update(dt, setpoint.alt_شرح);
		altitude_setpoint = _velocity_control_traj_generator.getCurrentPosition();
		control_altitude = PX4_ISFINITE(altitude_setpoint); // returns true if altitude is locked

	} وإلا {
		// input is altitude
		_velocity_control_traj_generator.reset(0, اوچائي_شرح, altitude_setpoint);
	}

	إذا (control_altitude) {
		ثابت عائم هدف_climbrate_m_s = math::min(param.هدف_climbrate, param.max_climb_شرح);
		ثابت عائم هدف_sinkrate_m_s = math::min(param.هدف_sinkrate, param.max_sink_شرح);

		ثابت عائم delta_trajectory_منزل_هدف_m = altitude_setpoint - _alt_control_traj_generator.getCurrentPosition();

		عائم اوچائي_شرح_هدف = math::signNoZero<عائم>(delta_trajectory_منزل_هدف_m) *
					   math::trajectory::compute_Max_رفتار_From_مفاصلو(
						   JERK_MAX, param.vert_accel_حد_بندي, fabsf(delta_trajectory_منزل_هدف_m), 0.f);

		اوچائي_شرح_هدف = math::constrain(اوچائي_شرح_هدف, -هدف_sinkrate_m_s, هدف_climbrate_m_s);

		_alt_control_traj_generator.updateDurations(اوچائي_شرح_هدف);
		_alt_control_traj_generator.updateTraj(dt);
		اوچائي_شرح_setpoint_سڌو = NAN;

	} وإلا {
		_alt_control_traj_generator.سيٽ_Current_Velocity(_velocity_control_traj_generator.getCurrentVelocity());
		_alt_control_traj_generator.سيٽ_Current_Position(current_alt);
		اوچائي_شرح_setpoint_سڌو = _velocity_control_traj_generator.getCurrentVelocity();
	}
}

TECSAltitudeReferenceModel::AltitudeReferenceState TECSAltitudeReferenceModel::getAltitudeReference() ثابت
{
	TECSAltitudeReferenceModel::AltitudeReferenceState ref{
		.alt = _alt_control_traj_generator.getCurrentPosition(),
		.alt_شرح = _alt_control_traj_generator.getCurrentVelocity(),
	};

	أرجع ref;
}

فراغ TECSAltitudeReferenceModel::initialize(ثابت AltitudeReferenceState &state)
{
	ثابت عائم init_state_alt = PX4_ISFINITE(state.alt) ? state.alt : 0.f;
	ثابت عائم init_state_alt_شرح = PX4_ISFINITE(state.alt_شرح) ? state.alt_شرح : 0.f;

	_alt_control_traj_generator.reset(0.0f, init_state_alt_شرح, init_state_alt);
	_velocity_control_traj_generator.reset(0.f, init_state_alt_شرح, init_state_alt);
}

فراغ TECSControl::initialize(ثابت Setpoint &setpoint, ثابت داخلا_2 &داخلا, ثابت Param &param, ثابت Flag &flag)
{
	resetIntegrals();

	// Initialization always goes through the altitude loop, even if a direct height rate setpoint is given
	Setpoint altitude_loop_setpoint{setpoint};
	altitude_loop_setpoint.altitude_شرح_setpoint_سڌو = NAN;

	ثابت STERate_حد_بندي حد_بندي_2{calculate_ڪل_Energy_شرح_حد_بندي(param)};
	ثابت SpecificEnergyRates specific_energy_شرح{_updateOuterLoops(altitude_loop_setpoint, داخلا, حد_بندي_2, param, flag)};

	ثابت SpecificEnergyWeighting وزن{update_رفتار_Altitude_Weights(param, flag)};
	ثابت SpecificEnergyRates pitch_specific_energy_شرح{_calcPitchControlEnergyRates(setpoint, داخلا, حد_بندي_2,
			specific_energy_شرح, param, flag, وزن)};
	Control_قدرون seb_شرح{calc_Pitch_Control_Seb_شرح(وزن, pitch_specific_energy_شرح)};

	_pitch_setpoint = _calcPitchControlOutput(داخلا, seb_شرح, param);

	ste_شرح_estimate_filter.reset(specific_energy_شرح.spe_شرح.estimate + specific_energy_شرح.ske_شرح.estimate);

	Control_قدرون ste_شرح{calc_Throttle_Control_Ste_شرح(حد_بندي_2, specific_energy_شرح, param)};

	_throttle_setpoint = _calcThrottleControlOutput(حد_بندي_2, ste_شرح, param, flag);

	// Debug output
	_debug_output.ڪل_energy_شرح_estimate = ste_شرح.estimate;
	_debug_output.ڪل_energy_شرح_sp = ste_شرح.setpoint;
	_debug_output.throttle_integrator = _throttle_integ_state;
	_debug_output.energy_balance_شرح_estimate = seb_شرح.estimate;
	_debug_output.energy_balance_شرح_sp = seb_شرح.setpoint;
	_debug_output.pitch_integrator = _pitch_integ_state;
}

فراغ TECSControl::update(ثابت عائم dt, ثابت Setpoint &setpoint, ثابت داخلا_2 &داخلا, ثابت Param &param,
			 ثابت Flag &flag)
{
	// Input checking
	إذا (!is_Valid_وقت_قدم(dt)) {
		// Do not update the states and output.
		أرجع;
	}

	ثابت STERate_حد_بندي حد_بندي_2{calculate_ڪل_Energy_شرح_حد_بندي(param)};
	ثابت SpecificEnergyRates specific_energy_شرح{_updateOuterLoops(setpoint, داخلا, حد_بندي_2, param, flag)};

	_calcPitchControl(dt, setpoint, داخلا, حد_بندي_2, specific_energy_شرح, param, flag);

	_calcThrottleControl(dt, حد_بندي_2, specific_energy_شرح, param, flag);
}

TECSControl::SpecificEnergyRates TECSControl::_updateOuterLoops(ثابت Setpoint &setpoint, ثابت داخلا_2 &داخلا,
		ثابت STERate_حد_بندي &حد_بندي_2, ثابت Param &param, ثابت Flag &flag)
{
	OuterLoopSetpoints control_setpoint;

	control_setpoint.tas_شرح_setpoint = _calcAirspeedControlOutput(setpoint, داخلا, حد_بندي_2, param, flag);

	إذا (PX4_ISFINITE(setpoint.altitude_شرح_setpoint_سڌو)) {
		// direct height rate control
		control_setpoint.altitude_شرح_setpoint = setpoint.altitude_شرح_setpoint_سڌو;

	} وإلا {
		// altitude is locked, go through altitude outer loop
		control_setpoint.altitude_شرح_setpoint = _calcAltitudeControlOutput(setpoint, داخلا, param);
	}

	ثابت SpecificEnergyRates specific_energy_شرح{_calcSpecificEnergyRates(control_setpoint, داخلا)};

	_detectUnderspeed(داخلا, param, flag);

	_debug_output.altitude_شرح_control = control_setpoint.altitude_شرح_setpoint;
	_debug_output.true_airspeed_derivative_control = control_setpoint.tas_شرح_setpoint;

	أرجع specific_energy_شرح;
}

TECSControl::STERate_حد_بندي TECSControl::calculate_ڪل_Energy_شرح_حد_بندي(ثابت Param &param) ثابت
{
	TECSControl::STERate_حد_بندي حد_بندي_2;
	// Calculate the specific total energy rate limits from the max throttle limits
	حد_بندي_2.STE_شرح_max = math::max(param.max_climb_شرح, FLT_EPSILON) * CONSTANTS_ONE_G;
	حد_بندي_2.STE_شرح_min = - math::max(param.min_sink_شرح, FLT_EPSILON) * CONSTANTS_ONE_G;

	أرجع حد_بندي_2;
}

عائم TECSControl::_calcAirspeedControlOutput(ثابت Setpoint &setpoint, ثابت داخلا_2 &داخلا, ثابت STERate_حد_بندي &حد_بندي_2,
		ثابت Param &param, ثابت Flag &flag) ثابت
{
	عائم airspeed_شرح_output{0.0f};

	// calculate the demanded true airspeed rate of change based on first order response of true airspeed error
	// if airspeed measurement is not enabled then always set the rate setpoint to zero in order to avoid constant rate setpoints
	إذا (flag.airspeed_enabled) {
		// Calculate limits for the demanded rate of change of speed based on physical performance limits
		// with a 50% margin to allow the total energy controller to correct for errors. Increase it in case of fast descend
		ثابت عائم max_tas_شرح_sp = (param.fast_descend * 0.5f + 0.5f) * حد_بندي_2.STE_شرح_max / math::max(داخلا.tas,
					      FLT_EPSILON);
		ثابت عائم min_tas_شرح_sp = (param.fast_descend * 0.5f + 0.5f) * حد_بندي_2.STE_شرح_min / math::max(داخلا.tas,
					      FLT_EPSILON);
		airspeed_شرح_output = constrain((setpoint.tas_setpoint - داخلا.tas) * param.airspeed_error_gain, min_tas_شرح_sp,
						 max_tas_شرح_sp);
	}

	أرجع airspeed_شرح_output;
}

عائم TECSControl::_calcAltitudeControlOutput(ثابت Setpoint &setpoint, ثابت داخلا_2 &داخلا, ثابت Param &param) ثابت
{
	عائم altitude_شرح_output;
	altitude_شرح_output = (setpoint.altitude_reference.alt - داخلا.altitude) * param.altitude_error_gain
			       + param.altitude_setpoint_gain_ff * setpoint.altitude_reference.alt_شرح;

	altitude_شرح_output = math::constrain(altitude_شرح_output, -param.max_sink_شرح, param.max_climb_شرح);

	أرجع altitude_شرح_output;
}

TECSControl::SpecificEnergyRates TECSControl::_calcSpecificEnergyRates(ثابت OuterLoopSetpoints &control_setpoint,
		ثابت داخلا_2 &داخلا) ثابت
{
	SpecificEnergyRates specific_energy_rates;
	// Calculate specific energy rate demands in units of (m**2/sec**3)
	specific_energy_rates.spe_شرح.setpoint = control_setpoint.altitude_شرح_setpoint *
			CONSTANTS_ONE_G; // potential energy rate of change

	// d/dt(0.5 V^2) = V * dV/dt: use measured TAS, not the TAS setpoint, to keep the airspeed loop linear
	specific_energy_rates.ske_شرح.setpoint = داخلا.tas *
			control_setpoint.tas_شرح_setpoint; // kinetic energy rate of change

	// Calculate specific energy rates in units of (m**2/sec**3)
	specific_energy_rates.spe_شرح.estimate = داخلا.altitude_شرح * CONSTANTS_ONE_G; // potential energy rate of change
	specific_energy_rates.ske_شرح.estimate = داخلا.tas * داخلا.tas_شرح;// kinetic energy rate of change

	أرجع specific_energy_rates;
}

فراغ TECSControl::_detectUnderspeed(ثابت داخلا_2 &داخلا, ثابت Param &param, ثابت Flag &flag)
{
	إذا (!flag.detect_underspeed_enabled || !flag.airspeed_enabled) {
		تناسب_undersped = 0.0f;
		أرجع;
	}

	// Ramp underspeed ratio from 0 at tas_min to 1 at 90% of tas_min, or halfway to tas_stall if that is higher
	ثابت عائم tas_fully_undersped = math::min(param.tas_min,
					  math::max(0.9f * param.tas_min, 0.5f * (param.tas_min + param.tas_stall)));
	ثابت عائم underspeed_ramp_ويڪر = param.tas_min - tas_fully_undersped;

	تناسب_undersped = math::constrain((param.tas_min - داخلا.tas) / math::max(underspeed_ramp_ويڪر, FLT_EPSILON),
					   0.0f, 1.0f);
}

TECSControl::SpecificEnergyWeighting TECSControl::update_رفتار_Altitude_Weights(ثابت Param &param, ثابت Flag &flag)
{

	SpecificEnergyWeighting وزن;
	// Calculate the weight applied to control of specific kinetic energy error
	عائم pitch_رفتار_وزن = constrain(param.pitch_رفتار_وزن, 0.0f, 2.0f);

	// Underspeed or fast descend: interpolate speed weight from nominal
	// towards max. We take the maximum of the two - this keeps the speed
	// weight continuous, even when underspeed and fast descend are combined
	ثابت عائم max_تناسب = fmaxf(تناسب_undersped, param.fast_descend);
	pitch_رفتار_وزن = max_تناسب * 2.0f + (1.0f - max_تناسب) * pitch_رفتار_وزن;

	إذا (!flag.airspeed_enabled) {
		pitch_رفتار_وزن = 0.0f;
	}

	وزن.spe_weighting = constrain(2.0f - pitch_رفتار_وزن, 0.f, 2.f);
	وزن.ske_weighting = constrain(pitch_رفتار_وزن, 0.f, 2.f);

	أرجع وزن;
}

عائم TECSControl::calc_Pitch_Control_Ske_شرح_Setpoint(ثابت Setpoint &setpoint, ثابت داخلا_2 &داخلا,
		ثابت STERate_حد_بندي &حد_بندي_2, ثابت Param &param, ثابت Flag &flag, ثابت SpecificEnergyWeighting &وزن) ثابت
{
	// Trading altitude for speed with pitch is only wanted up to trim airspeed. Above trim, accelerating is left to
	// the throttle loop: if it saturates, the vehicle settles at a lower airspeed instead of below the altitude setpoint.
	// The airspeed setpoint seen by the pitch loop is therefore capped at max(trim, current airspeed). Between trim
	// and the setpoint the pitch loop holds altitude, while an overspeed above the setpoint is still corrected.
	// With a speed weight of 2 (speed priority, e.g. gliders, fast descend or underspeed) pitch is the only way to
	// track the setpoint, so the cap is faded out as the weight goes from 1 to 2.
	ثابت عائم tas_setpoint_capped = min(setpoint.tas_setpoint, max(param.tas_trim, داخلا.tas));

	Setpoint pitch_control_setpoint{setpoint};
	pitch_control_setpoint.tas_setpoint = lerp(tas_setpoint_capped, setpoint.tas_setpoint,
					      constrain(وزن.ske_weighting - 1.f, 0.f, 1.f));

	// d/dt(0.5 V^2) = V * dV/dt: the measured airspeed, not the setpoint, keeps the airspeed loop linear
	أرجع داخلا.tas * _calcAirspeedControlOutput(pitch_control_setpoint, داخلا, حد_بندي_2, param, flag);
}

TECSControl::SpecificEnergyRates TECSControl::_calcPitchControlEnergyRates(ثابت Setpoint &setpoint,
		ثابت داخلا_2 &داخلا, ثابت STERate_حد_بندي &حد_بندي_2, ثابت SpecificEnergyRates &specific_energy_rates,
		ثابت Param &param, ثابت Flag &flag, ثابت SpecificEnergyWeighting &وزن) ثابت
{
	SpecificEnergyRates pitch_specific_energy_rates{specific_energy_rates};
	pitch_specific_energy_rates.ske_شرح.setpoint = calc_Pitch_Control_Ske_شرح_Setpoint(setpoint, داخلا, حد_بندي_2, param, flag,
			وزن);

	// Between trim airspeed and the airspeed setpoint the throttle alone accelerates the vehicle. The capped setpoint
	// above already removes the kinetic setpoint of the pitch loop there, faded in with speed priority. But the
	// throttle's acceleration would still show up in the kinetic energy rate estimate, and the pitch loop would pitch
	// up to turn it into height. Remove the estimate from the pitch loop in this band as well, with the same fade, so
	// that pitch controls height only. The setpoint must not be scaled again, or it would be faded twice.
	ثابت منطقي throttle_accelerates = داخلا.tas > param.tas_trim && داخلا.tas < setpoint.tas_setpoint;
	ثابت عائم kinetic_estimate_gain = throttle_accelerates ? constrain(وزن.ske_weighting - 1.f, 0.f, 1.f) : 1.f;
	pitch_specific_energy_rates.ske_شرح.estimate *= kinetic_estimate_gain;

	أرجع pitch_specific_energy_rates;
}

فراغ TECSControl::_calcPitchControl(عائم dt, ثابت Setpoint &setpoint, ثابت داخلا_2 &داخلا, ثابت STERate_حد_بندي &حد_بندي_2,
				    ثابت SpecificEnergyRates &specific_energy_rates, ثابت Param &param, ثابت Flag &flag)
{
	ثابت SpecificEnergyWeighting وزن{update_رفتار_Altitude_Weights(param, flag)};

	ثابت SpecificEnergyRates pitch_specific_energy_rates{_calcPitchControlEnergyRates(setpoint, داخلا, حد_بندي_2,
			specific_energy_rates, param, flag, وزن)};

	Control_قدرون seb_شرح{calc_Pitch_Control_Seb_شرح(وزن, pitch_specific_energy_rates)};

	_calcPitchControlUpdate(dt, داخلا, seb_شرح, param);
	ثابت عائم pitch_setpoint{_calcPitchControlOutput(داخلا, seb_شرح, param)};

	// Comply with the specified vertical acceleration limit by applying a pitch rate limit
	// NOTE: at zero airspeed, the pitch increment is unbounded
	ثابت عائم pitch_increment = dt * param.vert_accel_حد_بندي / math::max(داخلا.tas, FLT_EPSILON);
	_pitch_setpoint = constrain(pitch_setpoint, _pitch_setpoint - pitch_increment,
				    _pitch_setpoint + pitch_increment);
	_pitch_setpoint = constrain(_pitch_setpoint, param.pitch_min, param.pitch_max);

	//Debug Output
	_debug_output.energy_balance_شرح_estimate = seb_شرح.estimate;
	_debug_output.energy_balance_شرح_sp = seb_شرح.setpoint;
	_debug_output.pitch_integrator = _pitch_integ_state;
}

TECSControl::Control_قدرون TECSControl::calc_Pitch_Control_Seb_شرح(ثابت SpecificEnergyWeighting &وزن,
		ثابت SpecificEnergyRates &specific_energy_rates) ثابت
{
	Control_قدرون seb_شرح;
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
	seb_شرح.setpoint = specific_energy_rates.spe_شرح.setpoint * وزن.spe_weighting -
			    specific_energy_rates.ske_شرح.setpoint *
			    وزن.ske_weighting;

	seb_شرح.estimate = (specific_energy_rates.spe_شرح.estimate * وزن.spe_weighting) -
			    (specific_energy_rates.ske_شرح.estimate * وزن.ske_weighting);

	أرجع seb_شرح;
}

فراغ TECSControl::_calcPitchControlUpdate(عائم dt, ثابت داخلا_2 &داخلا, ثابت Control_قدرون &seb_شرح,
		ثابت Param &param)
{

	// In fast descend, the pitch integrator converges to the (substantially
	// negative) pitch. Ramp it down with decreasing fast descend ratio,
	// such that it is zero when the ratio becomes 0 again.
	إذا (param.fast_descend < _fast_descend_prev && _fast_descend_prev > FLT_EPSILON) {
		_pitch_integ_state *= param.fast_descend / _fast_descend_prev;
	}

	_fast_descend_prev = param.fast_descend;

	إذا (param.integrator_gain_pitch > FLT_EPSILON) {

		ثابت عائم climb_angle_منزل_SEB_شرح = climb_Angle_منزل_Seb_شرح(داخلا, param);

		// Calculate pitch integrator input term
		عائم pitch_integ_داخلا = _getControlError(seb_شرح) * param.integrator_gain_pitch / climb_angle_منزل_SEB_شرح;

		// Guard against NaN integrator input - discard bad input sample (e.g., exiting offboard velocity mode)
		إذا (!PX4_ISFINITE(pitch_integ_داخلا)) {
			pitch_integ_داخلا = 0.f;
		}

		// Prevent the integrator changing in a direction that will increase pitch demand saturation
		إذا (_pitch_setpoint >= param.pitch_max) {
			pitch_integ_داخلا = min(pitch_integ_داخلا, 0.f);

		} وإلا إذا (_pitch_setpoint <= param.pitch_min) {
			pitch_integ_داخلا = max(pitch_integ_داخلا, 0.f);
		}

		// Update the pitch integrator state
		_pitch_integ_state = _pitch_integ_state + pitch_integ_داخلا * dt;

		// Safety check: reset integrator if state became corrupted
		إذا (!PX4_ISFINITE(_pitch_integ_state)) {
			_pitch_integ_state = 0.f;
		}

	} وإلا {
		_pitch_integ_state = 0.0f;
	}
}

عائم TECSControl::climb_Angle_منزل_Seb_شرح(ثابت داخلا_2 &داخلا, ثابت Param &param)
{
	// max() returns its second argument for a NaN first one, so a NaN airspeed falls back to the minimum airspeed
	أرجع max(داخلا.tas, param.tas_min, FLT_EPSILON) * CONSTANTS_ONE_G;
}

عائم TECSControl::_calcPitchControlOutput(ثابت داخلا_2 &داخلا, ثابت Control_قدرون &seb_شرح, ثابت Param &param) ثابت
{
	// Calculate a specific energy correction that doesn't include the integrator contribution
	عائم SEB_شرح_correction = _getControlError(seb_شرح) * param.pitch_damping_gain +
				    param.seb_شرح_ff *
				    seb_شرح.setpoint;

	// Convert the specific energy balance rate correction to a climb angle and add the level-flight pitch, the angle
	// of attack at zero climb angle, to get the absolute pitch setpoint. This calculation assumes:
	// a) The climb angle follows pitch angle with a lag that is small enough not to destabilise the control loop.
	// b) The offset between climb angle and pitch angle (angle of attack) is constant and equal to the level-flight
	// pitch, excluding the effect of pitch transients due to control action or turbulence.
	عائم pitch_setpoint_unc = SEB_شرح_correction / climb_Angle_منزل_Seb_شرح(داخلا, param) + _pitch_integ_state
				   + param.level_flight_pitch;

	// Guard against a non-finite feedforward/damping contribution (e.g. seb_rate going NaN on the energy/speed
	// side when exiting offboard velocity mode). constrain() does not reject NaN, so fall back to the integrator
	// state, which is kept finite by _calcPitchControlUpdate, to avoid latching a NaN pitch demand.
	إذا (!PX4_ISFINITE(pitch_setpoint_unc)) {
		pitch_setpoint_unc = _pitch_integ_state + param.level_flight_pitch;
	}

	أرجع constrain(pitch_setpoint_unc, param.pitch_min, param.pitch_max);
}

فراغ TECSControl::_calcThrottleControl(عائم dt, ثابت STERate_حد_بندي &حد_بندي_2,
				       ثابت SpecificEnergyRates &specific_energy_rates, ثابت Param &param, ثابت Flag &flag)
{
	// Update STE rate estimate LP filter
	ثابت عائم STE_شرح_estimate_raw = specific_energy_rates.spe_شرح.estimate + specific_energy_rates.ske_شرح.estimate;
	ste_شرح_estimate_filter.سيٽ_Parameters(dt, param.ste_شرح_وقت_const);
	ste_شرح_estimate_filter.update(STE_شرح_estimate_raw);
	Control_قدرون ste_شرح{calc_Throttle_Control_Ste_شرح(حد_بندي_2, specific_energy_rates, param)};
	// During fast descend, we control airspeed over the pitch control loop. Give minimal thrust as soon as we are descending
	عائم throttle_setpoint{param.throttle_min};

	إذا (!fastDescendFullyEngaged(param.fast_descend)) {
		_calcThrottleControlUpdate(dt, حد_بندي_2, ste_شرح, param, flag);
		throttle_setpoint = (1.f - param.fast_descend) * _calcThrottleControlOutput(حد_بندي_2, ste_شرح, param,
				    flag) + param.fast_descend * param.throttle_min;
	}

	// Rate limit the throttle demand
	إذا (fabsf(param.throttle_slewrate) > FLT_EPSILON) {
		ثابت عائم throttle_increment_حد_بندي = dt * (param.throttle_max - param.throttle_min) * param.throttle_slewrate;
		throttle_setpoint = constrain(throttle_setpoint, _throttle_setpoint - throttle_increment_حد_بندي,
					      _throttle_setpoint + throttle_increment_حد_بندي);
	}

	_throttle_setpoint = constrain(throttle_setpoint, param.throttle_min, param.throttle_max);

	// Debug output
	_debug_output.ڪل_energy_شرح_estimate = ste_شرح.estimate;
	_debug_output.ڪل_energy_شرح_sp = ste_شرح.setpoint;
	_debug_output.throttle_integrator = _throttle_integ_state;
}

TECSControl::Control_قدرون TECSControl::calc_Throttle_Control_Ste_شرح(ثابت STERate_حد_بندي &حد_بندي_2,
		ثابت SpecificEnergyRates &specific_energy_rates,
		ثابت Param &param) ثابت
{
	// Output ste rate values
	Control_قدرون ste_شرح;
	ste_شرح.setpoint = specific_energy_rates.spe_شرح.setpoint + specific_energy_rates.ske_شرح.setpoint;

	// Adjust the demanded total energy rate to compensate for induced drag rise in turns.
	// Assume induced drag scales linearly with normal load factor.
	// The additional normal load factor is given by (1/cos(bank angle) - 1)
	ste_شرح.setpoint += param.ڀريو_ضرب_ڪندڙ_correction * (param.ڀريو_ضرب_ڪندڙ - 1.f);

	ste_شرح.setpoint = constrain(ste_شرح.setpoint, حد_بندي_2.STE_شرح_min, حد_بندي_2.STE_شرح_max);
	ste_شرح.estimate = ste_شرح_estimate_filter.getState();

	أرجع ste_شرح;
}

فراغ TECSControl::_calcThrottleControlUpdate(عائم dt, ثابت STERate_حد_بندي &حد_بندي_2, ثابت Control_قدرون &ste_شرح,
		ثابت Param &param, ثابت Flag &flag)
{
	ثابت عائم STE_شرح_منزل_throttle = ste_شرح_منزل_Throttle(حد_بندي_2);

	// Integral handling
	إذا (flag.airspeed_enabled) {
		إذا (param.integrator_gain_throttle > FLT_EPSILON) {
			// underspeed conditions zero out integration
			عائم throttle_integ_داخلا = (_getControlError(ste_شرح) * param.integrator_gain_throttle) * dt *
						     STE_شرح_منزل_throttle * (1.0f - تناسب_undersped);

			// only allow integrator propagation into direction which unsaturates throttle
			إذا (_throttle_setpoint >= param.throttle_max) {
				throttle_integ_داخلا = math::min(0.f, throttle_integ_داخلا);

			}

			إذا (_throttle_setpoint <= param.throttle_min) {
				throttle_integ_داخلا = math::max(0.f, throttle_integ_داخلا);
			}

			// Calculate a throttle demand from the integrated total energy rate error
			// This will be added to the total throttle demand to compensate for steady state errors
			_throttle_integ_state = PX4_ISFINITE(throttle_integ_داخلا) ? _throttle_integ_state + throttle_integ_داخلا : 0.f;

		} وإلا {
			_throttle_integ_state = 0.0f;
		}

	}
}

عائم TECSControl::_calcThrottleControlOutput(ثابت STERate_حد_بندي &حد_بندي_2, ثابت Control_قدرون &ste_شرح,
		ثابت Param &param,
		ثابت Flag &flag) ثابت
{
	ثابت عائم STE_شرح_منزل_throttle = ste_شرح_منزل_Throttle(حد_بندي_2);

	// Calculate a predicted throttle from the demanded rate of change of energy, using the cruise throttle
	// as the starting point. Assume:
	// Specific total energy rate = _STE_rate_max is achieved when throttle is set to _throttle_setpoint_max
	// Specific total energy rate = 0 at cruise throttle
	// Specific total energy rate = _STE_rate_min is achieved when throttle is set to _throttle_setpoint_min

	// assume airspeed and density-independent delta_throttle to sink/climb rate mapping
	// TODO: include air density for thrust mappings
	ثابت عائم throttle_above_trim_في_ste_شرح = (param.throttle_max - param.throttle_trim) / حد_بندي_2.STE_شرح_max;
	ثابت عائم throttle_هيٺ_trim_في_ste_شرح = (param.throttle_trim - param.throttle_min) / حد_بندي_2.STE_شرح_min;

	عائم throttle_predicted = 0.0f;

	إذا (ste_شرح.setpoint >= FLT_EPSILON) {
		// throttle is between trim and maximum
		throttle_predicted = param.throttle_trim + ste_شرح.setpoint * throttle_above_trim_في_ste_شرح;

	} وإلا {
		// throttle is between trim and minimum
		throttle_predicted = param.throttle_trim - ste_شرح.setpoint * throttle_هيٺ_trim_في_ste_شرح;

	}

	// Add proportional and derivative control feedback to the predicted throttle and constrain to throttle limits
	عائم throttle_setpoint = (_getControlError(ste_شرح) * param.throttle_damping_gain) * STE_شرح_منزل_throttle +
				  throttle_predicted;

	إذا (flag.airspeed_enabled) {
		// Add the integrator feedback during closed loop operation with an airspeed sensor
		throttle_setpoint += _throttle_integ_state;

	} وإلا {
		/* We want to avoid reducing the throttle output when switching from airspeed enabled mode into airspeedless mode.
		  Thus, if the throttle integrator has a positive value, add it still to the throttle setpoint. */
		throttle_setpoint += math::max(0.0f, _throttle_integ_state);
	}


	// ramp in max throttle setting with underspeediness value
	throttle_setpoint = تناسب_undersped * param.throttle_max + (1.0f - تناسب_undersped) * throttle_setpoint;

	أرجع constrain(throttle_setpoint, param.throttle_min, param.throttle_max);
}

فراغ TECSControl::resetIntegrals()
{
	_pitch_integ_state = 0.0f;
	_throttle_integ_state = 0.0f;
}

فراغ TECS::initControlParams(ثابت داخلا_2 &داخلا)
{
	// Update parameters from input
	// Reference model
	_reference_param.هدف_climbrate = داخلا.هدف_climbrate;
	_reference_param.هدف_sinkrate = داخلا.هدف_sinkrate;
	// Control
	_control_param.tas_min = داخلا.eas_منزل_tas * _equivalent_airspeed_min;
	_control_param.tas_stall = داخلا.eas_منزل_tas * _equivalent_airspeed_stall;
	_control_param.tas_max = داخلا.eas_منزل_tas * _equivalent_airspeed_max;
	_control_param.tas_trim = داخلا.eas_منزل_tas * _control_param.equivalent_airspeed_trim;
	_control_param.pitch_max = داخلا.pitch_max;
	_control_param.pitch_min = داخلا.pitch_min;
	_control_param.throttle_trim = داخلا.throttle_trim;
	_control_param.throttle_max = داخلا.throttle_max;
	_control_param.throttle_min = داخلا.throttle_min;
}

عائم TECS::calcTrueAirspeedSetpoint(عائم eas_منزل_tas, عائم eas_setpoint)
{
	أرجع lerp(eas_منزل_tas * eas_setpoint, _control_param.tas_max, _fast_descend);
}

فراغ TECS::initialize(ثابت داخلا_2 &داخلا)
{
	// Init subclasses
	TECSAltitudeReferenceModel::AltitudeReferenceState current_state{.alt = داخلا.altitude,
			.alt_شرح = داخلا.altitude_شرح};
	_altitude_reference_model.initialize(current_state);
	_airspeed_filter.initialize(داخلا.equivalent_airspeed, _airspeed_filter_param.equivalent_airspeed_trim,
				    _control_flag.airspeed_enabled);

	// Seed the controller from the filter, which falls back to trim airspeed without a valid measurement,
	// so the controller sees the same airspeed on reset as on every following update
	ثابت عائم true_airspeed = داخلا.eas_منزل_tas * _airspeed_filter.getState().رفتار;

	TECSControl::Setpoint control_setpoint;
	control_setpoint.altitude_reference = _altitude_reference_model.getAltitudeReference();
	control_setpoint.altitude_شرح_setpoint_سڌو = NAN; // initialization always goes through the altitude loop
	control_setpoint.tas_setpoint = true_airspeed;

	ثابت TECSControl::داخلا_2 control_داخلا{ .altitude = داخلا.altitude,
						.altitude_شرح = داخلا.altitude_شرح,
						.tas = true_airspeed,
						.tas_شرح = 0.0f};

	_control.initialize(control_setpoint, control_داخلا, _control_param, _control_flag);

	_fast_descend = 0.f;
	_enabled_fast_descend_timestamp = 0U;
}

فراغ TECS::update(ثابت داخلا_2 &داخلا)
{
	// Calculate the time since last update (seconds)
	ثابت hrt_abstime now = hrt_absolute_وقت();
	ثابت عائم dt = static_cast<عائم>(now - _update_timestamp) * 1e-6f;

	initControlParams(داخلا);

	إذا (dt < DT_MIN) {
		// Update interval too small, do not update. Assume constant states/output in this case.
		أرجع;
	}

	عائم true_airspeed_setpoint;

	إذا (dt > DT_MAX || _update_timestamp == 0UL) {
		// Update time interval too large, can't guarantee sanity of state updates anymore. reset the control loop.
		initialize(داخلا);
		true_airspeed_setpoint = calcTrueAirspeedSetpoint(داخلا.eas_منزل_tas, داخلا.equivalent_airspeed_setpoint);

	} وإلا {
		/* Check if we want to fast descend. On fast descend, we set the throttle to min, and use the altitude control
		loop to control the speed to the maximum airspeed. */
		سيٽ_Fast_Descend(داخلا.altitude_setpoint, داخلا.altitude);
		_control_param.fast_descend = _fast_descend;

		// Update airspeedfilter submodule
		ثابت TECSAirspeedFilter::داخلا_2 airspeed_داخلا{ .equivalent_airspeed = داخلا.equivalent_airspeed,
				.equivalent_airspeed_شرح = داخلا.رفتار_deriv_forward / داخلا.eas_منزل_tas};

		_airspeed_filter.update(dt, airspeed_داخلا, _airspeed_filter_param, _control_flag.airspeed_enabled);

		// Update Reference model submodule
		إذا (fastDescendFullyEngaged(_fast_descend)) {
			// Reset the altitude reference model, while we are in fast descend.
			ثابت TECSAltitudeReferenceModel::AltitudeReferenceState init_state{
				.alt = داخلا.altitude,
				.alt_شرح = داخلا.altitude_شرح};
			_altitude_reference_model.initialize(init_state);

		} وإلا إذا (_control.get_Underspeed_تناسب() > 0.5f) {
			// Reset altitude reference during underspeed to prevent altitude error accumulation
			// that would cause aggressive pitch-up (and potential secondary stall) on recovery.
			ثابت TECSAltitudeReferenceModel::AltitudeReferenceState init_state{
				.alt = داخلا.altitude,
				.alt_شرح = داخلا.altitude_شرح};
			_altitude_reference_model.initialize(init_state);

		} وإلا {
			ثابت TECSAltitudeReferenceModel::AltitudeReferenceState setpoint{ .alt = داخلا.altitude_setpoint,
					.alt_شرح = داخلا.altitude_شرح_setpoint};

			_altitude_reference_model.update(dt, setpoint, داخلا.altitude, داخلا.altitude_شرح, _reference_param);
		}

		TECSControl::Setpoint control_setpoint;
		control_setpoint.altitude_reference = _altitude_reference_model.getAltitudeReference();
		control_setpoint.altitude_شرح_setpoint_سڌو = _altitude_reference_model.get_اوچائي_شرح_Setpoint_سڌو();
		true_airspeed_setpoint = calcTrueAirspeedSetpoint(داخلا.eas_منزل_tas, داخلا.equivalent_airspeed_setpoint);
		control_setpoint.tas_setpoint = true_airspeed_setpoint;

		ثابت TECSAirspeedFilter::AirspeedFilterState airspeed_state{_airspeed_filter.getState()};
		ثابت TECSControl::داخلا_2 control_داخلا{ .altitude = داخلا.altitude,
							.altitude_شرح = داخلا.altitude_شرح,
							.tas = داخلا.eas_منزل_tas * airspeed_state.رفتار,
							.tas_شرح = داخلا.eas_منزل_tas * airspeed_state.رفتار_شرح};

		_control.update(dt, control_setpoint, control_داخلا, _control_param, _control_flag);
	}

	debug_حالت.control = _control.getDebugOutput();
	debug_حالت.true_airspeed_sp = true_airspeed_setpoint;
	ثابت TECSAirspeedFilter::AirspeedFilterState airspeed_state{_airspeed_filter.getState()};
	ثابت TECSAltitudeReferenceModel::AltitudeReferenceState altitude_reference{_altitude_reference_model.getAltitudeReference()};
	debug_حالت.true_airspeed_filtered = داخلا.eas_منزل_tas * airspeed_state.رفتار;
	debug_حالت.true_airspeed_derivative = داخلا.eas_منزل_tas * airspeed_state.رفتار_شرح;
	debug_حالت.altitude_reference = altitude_reference.alt;
	debug_حالت.اوچائي_شرح_reference = altitude_reference.alt_شرح;
	debug_حالت.اوچائي_شرح_سڌو = _altitude_reference_model.get_اوچائي_شرح_Setpoint_سڌو();
	debug_حالت.fast_descend = _fast_descend;

	_update_timestamp = now;
}

فراغ TECS::سيٽ_Fast_Descend(ثابت عائم alt_setpoint, ثابت عائم alt)
{
	// disable fast descend if we are close to the target altitude or the altitude setpoint is not finite

	إذا (PX4_ISFINITE(alt_setpoint) && _control_flag.airspeed_enabled && (_fast_descend_alt_err > FLT_EPSILON)
	    && ((alt_setpoint + _fast_descend_alt_err) < alt)) {
		خودڪار now = hrt_absolute_وقت();

		إذا (_enabled_fast_descend_timestamp == 0U) {
			_enabled_fast_descend_timestamp = now;
		}

		_fast_descend = constrain(max(_fast_descend, static_cast<عائم>(now - _enabled_fast_descend_timestamp) /
					      static_cast<عائم>(FAST_DESCEND_RAMP_مٿي_وقت)), 0.f, 1.f);

	} وإلا إذا (PX4_ISFINITE(alt_setpoint) && (_fast_descend > FLT_EPSILON) && (_fast_descend_alt_err > FLT_EPSILON)) {
		// We're in fast descend, scale it down until FAST_DESCEND_HEIGHT_MARGIN above target altitude
		_fast_descend = constrain((alt - alt_setpoint - FAST_DESCEND_اوچائي_MARGIN) / _fast_descend_alt_err, 0.f, 1.f);
		_enabled_fast_descend_timestamp = 0U;

	} وإلا {
		_fast_descend = 0.f;
		_enabled_fast_descend_timestamp = 0U;
	}
}

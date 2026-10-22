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

#અન્તર્ભાવ "TECS.hpp"

#અન્તર્ભાવ <lib/geo/geo.h>

#અન્તર્ભાવ <px4_platform_common/defines.h>

using math::constrain;
using math::max;
using math::min;

સ્થિર constexpr તાર્કિક isValidTimeStep(દશાંશ dt) { પાછા (PX4_ISFINITE(dt) && dt > FLT_EPSILON);}
સ્થિર constexpr તાર્કિક fastDescendFullyEngaged(દશાંશ fast_descend) { પાછા 1.f - fast_descend < FLT_EPSILON; }

શૂન્ય TECSAirspeedFilter::initialize(અચલ દશાંશ equivalent_airspeed, અચલ દશાંશ equivalent_airspeed_trim,
				    અચલ તાર્કિક airspeed_sensor_available)
{
	જો (airspeed_sensor_available && PX4_ISFINITE(equivalent_airspeed)) {
		_airspeed_state.speed = equivalent_airspeed;

	} નહીંતર {
		_airspeed_state.speed = equivalent_airspeed_trim;
	}

	_airspeed_state.speed_rate = 0.f;
}

શૂન્ય TECSAirspeedFilter::update(અચલ દશાંશ dt, અચલ આગત_2 &આગત, અચલ Param &param,
				અચલ તાર્કિક airspeed_sensor_available)
{
	// Input checking
	જો (!isValidTimeStep(dt)) {
		// Do not update the states.
		પાછા;
	}

	દશાંશ airspeed;

	જો (PX4_ISFINITE(આગત.equivalent_airspeed) && airspeed_sensor_available) {
		airspeed = આગત.equivalent_airspeed;

	} નહીંતર {
		airspeed = param.equivalent_airspeed_trim;
	}

	દશાંશ airspeed_derivative;

	જો (PX4_ISFINITE(આગત.equivalent_airspeed_rate) && airspeed_sensor_available) {
		airspeed_derivative = આગત.equivalent_airspeed_rate;

	} નહીંતર {
		airspeed_derivative = 0.0f;
	}

	/* Filter airspeed and rate using a constant airspeed rate model in a steady state Kalman Filter.
	 We use the gains of the continuous Kalman filter Kc and approximate the discrete version Kalman gain Kd =dt*Kc,
	 since the continuous algebraic Riccatti equation is easier to solve.
	*/

	matrix::Vector2f new_state_predicted;

	new_state_predicted(0) = _airspeed_state.speed + dt * _airspeed_state.speed_rate;
	new_state_predicted(1) = _airspeed_state.speed_rate;

	અચલ matrix::Vector2f innovation{(airspeed - new_state_predicted(0)), (airspeed_derivative - new_state_predicted(1))};

	matrix::Vector2f new_state;
	new_state(0) = new_state_predicted(0) + dt * (kKg00 * innovation(0) + kKg01 * innovation(1));
	new_state(1) = new_state_predicted(1) + dt * (kKg10 * innovation(0) + kKg11 * innovation(1));

	// Clip airspeed at zero
	જો (new_state(0) < FLT_EPSILON) {
		new_state(0) = 0.0f;
		// calculate input that would result in zero speed.
		અચલ દશાંશ desired_airspeed_innovation = (-new_state_predicted(0) / dt - kKg01 * innovation(1)) / kKg00;
		new_state(1) = new_state_predicted(1) + dt * (kKg10 * desired_airspeed_innovation + kKg11 * innovation(1));
	}

	// Update states
	_airspeed_state.speed = new_state(0);
	_airspeed_state.speed_rate = new_state(1);
}

TECSAirspeedFilter::AirspeedFilterState TECSAirspeedFilter::getState() અચલ
{
	પાછા _airspeed_state;
}

શૂન્ય TECSAltitudeReferenceModel::update(અચલ દશાંશ dt, અચલ AltitudeReferenceState &setpoint, દશાંશ altitude,
					દશાંશ height_rate, અચલ Param &param)
{
	// Input checks
	જો (!isValidTimeStep(dt)) {
		// Do not update the states.
		પાછા;
	}

	અચલ દશાંશ current_alt = PX4_ISFINITE(altitude) ? altitude : 0.f;

	_velocity_control_traj_generator.setMaxJerk(JERK_MAX);
	_velocity_control_traj_generator.setMaxAccelUp(param.vert_accel_limit);
	_velocity_control_traj_generator.setMaxAccelDown(param.vert_accel_limit);
	_velocity_control_traj_generator.setMaxVelUp(param.max_sink_rate); // different convention for FW than for MC
	_velocity_control_traj_generator.setMaxVelDown(param.max_climb_rate); // different convention for FW than for MC

	// Altitude setpoint reference
	_alt_control_traj_generator.setMaxJerk(JERK_MAX);
	_alt_control_traj_generator.setMaxAccel(param.vert_accel_limit);
	_alt_control_traj_generator.setMaxVel(fmaxf(param.max_climb_rate, param.max_sink_rate));

	// XXX: this is a bit risky.. .alt_rate here could be NAN (by interface design) - and is only ok to input to the
	// setVelSpFeedback() method because it calls the reset in the logic below when it is NAN.
	// TODO: stop it with the NAN interfaces, make sure to take care of this when refactoring and separating altitude
	// and height rate control loops.
	_velocity_control_traj_generator.setVelSpFeedback(setpoint.alt_rate);

	તાર્કિક control_altitude = સાચું;
	દશાંશ altitude_setpoint = setpoint.alt;

	જો (!PX4_ISFINITE(setpoint.alt) && !PX4_ISFINITE(setpoint.alt_rate)) {
		// neither altitude nor altitude rate is set - reset to current altitude
		_velocity_control_traj_generator.reset(0.f, 0, current_alt);
		altitude_setpoint = current_alt;

	} નહીંતર જો (PX4_ISFINITE(setpoint.alt_rate)) {
		// input is height rate (not altitude)
		_velocity_control_traj_generator.setCurrentPositionEstimate(current_alt);
		_velocity_control_traj_generator.update(dt, setpoint.alt_rate);
		altitude_setpoint = _velocity_control_traj_generator.getCurrentPosition();
		control_altitude = PX4_ISFINITE(altitude_setpoint); // returns true if altitude is locked

	} નહીંતર {
		// input is altitude
		_velocity_control_traj_generator.reset(0, height_rate, altitude_setpoint);
	}

	જો (control_altitude) {
		અચલ દશાંશ target_climbrate_m_s = math::min(param.target_climbrate, param.max_climb_rate);
		અચલ દશાંશ target_sinkrate_m_s = math::min(param.target_sinkrate, param.max_sink_rate);

		અચલ દશાંશ delta_trajectory_to_target_m = altitude_setpoint - _alt_control_traj_generator.getCurrentPosition();

		દશાંશ height_rate_target = math::signNoZero<દશાંશ>(delta_trajectory_to_target_m) *
					   math::trajectory::computeMaxSpeedFromDistance(
						   JERK_MAX, param.vert_accel_limit, fabsf(delta_trajectory_to_target_m), 0.f);

		height_rate_target = math::constrain(height_rate_target, -target_sinkrate_m_s, target_climbrate_m_s);

		_alt_control_traj_generator.updateDurations(height_rate_target);
		_alt_control_traj_generator.updateTraj(dt);
		_height_rate_setpoint_direct = NAN;

	} નહીંતર {
		_alt_control_traj_generator.setCurrentVelocity(_velocity_control_traj_generator.getCurrentVelocity());
		_alt_control_traj_generator.setCurrentPosition(current_alt);
		_height_rate_setpoint_direct = _velocity_control_traj_generator.getCurrentVelocity();
	}
}

TECSAltitudeReferenceModel::AltitudeReferenceState TECSAltitudeReferenceModel::getAltitudeReference() અચલ
{
	TECSAltitudeReferenceModel::AltitudeReferenceState ref{
		.alt = _alt_control_traj_generator.getCurrentPosition(),
		.alt_rate = _alt_control_traj_generator.getCurrentVelocity(),
	};

	પાછા ref;
}

શૂન્ય TECSAltitudeReferenceModel::initialize(અચલ AltitudeReferenceState &state)
{
	અચલ દશાંશ init_state_alt = PX4_ISFINITE(state.alt) ? state.alt : 0.f;
	અચલ દશાંશ init_state_alt_rate = PX4_ISFINITE(state.alt_rate) ? state.alt_rate : 0.f;

	_alt_control_traj_generator.reset(0.0f, init_state_alt_rate, init_state_alt);
	_velocity_control_traj_generator.reset(0.f, init_state_alt_rate, init_state_alt);
}

શૂન્ય TECSControl::initialize(અચલ Setpoint &setpoint, અચલ આગત_2 &આગત, અચલ Param &param, અચલ Flag &flag)
{
	resetIntegrals();

	// Initialization always goes through the altitude loop, even if a direct height rate setpoint is given
	Setpoint altitude_loop_setpoint{setpoint};
	altitude_loop_setpoint.altitude_rate_setpoint_direct = NAN;

	અચલ STERateLimit limit{_calculateTotalEnergyRateLimit(param)};
	અચલ SpecificEnergyRates specific_energy_rate{_updateOuterLoops(altitude_loop_setpoint, આગત, limit, param, flag)};

	અચલ SpecificEnergyWeighting weight{_updateSpeedAltitudeWeights(param, flag)};
	અચલ SpecificEnergyRates pitch_specific_energy_rate{_calcPitchControlEnergyRates(setpoint, આગત, limit,
			specific_energy_rate, param, flag, weight)};
	Control_કિંમતો seb_rate{_calcPitchControlSebRate(weight, pitch_specific_energy_rate)};

	_pitch_setpoint = _calcPitchControlOutput(આગત, seb_rate, param);

	_ste_rate_estimate_filter.reset(specific_energy_rate.spe_rate.estimate + specific_energy_rate.ske_rate.estimate);

	Control_કિંમતો ste_rate{_calcThrottleControlSteRate(limit, specific_energy_rate, param)};

	_throttle_setpoint = _calcThrottleControlOutput(limit, ste_rate, param, flag);

	// Debug output
	_debug_output.total_energy_rate_estimate = ste_rate.estimate;
	_debug_output.total_energy_rate_sp = ste_rate.setpoint;
	_debug_output.throttle_integrator = _throttle_integ_state;
	_debug_output.energy_balance_rate_estimate = seb_rate.estimate;
	_debug_output.energy_balance_rate_sp = seb_rate.setpoint;
	_debug_output.pitch_integrator = _pitch_integ_state;
}

શૂન્ય TECSControl::update(અચલ દશાંશ dt, અચલ Setpoint &setpoint, અચલ આગત_2 &આગત, અચલ Param &param,
			 અચલ Flag &flag)
{
	// Input checking
	જો (!isValidTimeStep(dt)) {
		// Do not update the states and output.
		પાછા;
	}

	અચલ STERateLimit limit{_calculateTotalEnergyRateLimit(param)};
	અચલ SpecificEnergyRates specific_energy_rate{_updateOuterLoops(setpoint, આગત, limit, param, flag)};

	_calcPitchControl(dt, setpoint, આગત, limit, specific_energy_rate, param, flag);

	_calcThrottleControl(dt, limit, specific_energy_rate, param, flag);
}

TECSControl::SpecificEnergyRates TECSControl::_updateOuterLoops(અચલ Setpoint &setpoint, અચલ આગત_2 &આગત,
		અચલ STERateLimit &limit, અચલ Param &param, અચલ Flag &flag)
{
	OuterLoopSetpoints control_setpoint;

	control_setpoint.tas_rate_setpoint = _calcAirspeedControlOutput(setpoint, આગત, limit, param, flag);

	જો (PX4_ISFINITE(setpoint.altitude_rate_setpoint_direct)) {
		// direct height rate control
		control_setpoint.altitude_rate_setpoint = setpoint.altitude_rate_setpoint_direct;

	} નહીંતર {
		// altitude is locked, go through altitude outer loop
		control_setpoint.altitude_rate_setpoint = _calcAltitudeControlOutput(setpoint, આગત, param);
	}

	અચલ SpecificEnergyRates specific_energy_rate{_calcSpecificEnergyRates(control_setpoint, આગત)};

	_detectUnderspeed(આગત, param, flag);

	_debug_output.altitude_rate_control = control_setpoint.altitude_rate_setpoint;
	_debug_output.true_airspeed_derivative_control = control_setpoint.tas_rate_setpoint;

	પાછા specific_energy_rate;
}

TECSControl::STERateLimit TECSControl::_calculateTotalEnergyRateLimit(અચલ Param &param) અચલ
{
	TECSControl::STERateLimit limit;
	// Calculate the specific total energy rate limits from the max throttle limits
	limit.STE_rate_max = math::max(param.max_climb_rate, FLT_EPSILON) * CONSTANTS_ONE_G;
	limit.STE_rate_min = - math::max(param.min_sink_rate, FLT_EPSILON) * CONSTANTS_ONE_G;

	પાછા limit;
}

દશાંશ TECSControl::_calcAirspeedControlOutput(અચલ Setpoint &setpoint, અચલ આગત_2 &આગત, અચલ STERateLimit &limit,
		અચલ Param &param, અચલ Flag &flag) અચલ
{
	દશાંશ airspeed_rate_output{0.0f};

	// calculate the demanded true airspeed rate of change based on first order response of true airspeed error
	// if airspeed measurement is not enabled then always set the rate setpoint to zero in order to avoid constant rate setpoints
	જો (flag.airspeed_enabled) {
		// Calculate limits for the demanded rate of change of speed based on physical performance limits
		// with a 50% margin to allow the total energy controller to correct for errors. Increase it in case of fast descend
		અચલ દશાંશ max_tas_rate_sp = (param.fast_descend * 0.5f + 0.5f) * limit.STE_rate_max / math::max(આગત.tas,
					      FLT_EPSILON);
		અચલ દશાંશ min_tas_rate_sp = (param.fast_descend * 0.5f + 0.5f) * limit.STE_rate_min / math::max(આગત.tas,
					      FLT_EPSILON);
		airspeed_rate_output = constrain((setpoint.tas_setpoint - આગત.tas) * param.airspeed_error_gain, min_tas_rate_sp,
						 max_tas_rate_sp);
	}

	પાછા airspeed_rate_output;
}

દશાંશ TECSControl::_calcAltitudeControlOutput(અચલ Setpoint &setpoint, અચલ આગત_2 &આગત, અચલ Param &param) અચલ
{
	દશાંશ altitude_rate_output;
	altitude_rate_output = (setpoint.altitude_reference.alt - આગત.altitude) * param.altitude_error_gain
			       + param.altitude_setpoint_gain_ff * setpoint.altitude_reference.alt_rate;

	altitude_rate_output = math::constrain(altitude_rate_output, -param.max_sink_rate, param.max_climb_rate);

	પાછા altitude_rate_output;
}

TECSControl::SpecificEnergyRates TECSControl::_calcSpecificEnergyRates(અચલ OuterLoopSetpoints &control_setpoint,
		અચલ આગત_2 &આગત) અચલ
{
	SpecificEnergyRates specific_energy_rates;
	// Calculate specific energy rate demands in units of (m**2/sec**3)
	specific_energy_rates.spe_rate.setpoint = control_setpoint.altitude_rate_setpoint *
			CONSTANTS_ONE_G; // potential energy rate of change

	// d/dt(0.5 V^2) = V * dV/dt: use measured TAS, not the TAS setpoint, to keep the airspeed loop linear
	specific_energy_rates.ske_rate.setpoint = આગત.tas *
			control_setpoint.tas_rate_setpoint; // kinetic energy rate of change

	// Calculate specific energy rates in units of (m**2/sec**3)
	specific_energy_rates.spe_rate.estimate = આગત.altitude_rate * CONSTANTS_ONE_G; // potential energy rate of change
	specific_energy_rates.ske_rate.estimate = આગત.tas * આગત.tas_rate;// kinetic energy rate of change

	પાછા specific_energy_rates;
}

શૂન્ય TECSControl::_detectUnderspeed(અચલ આગત_2 &આગત, અચલ Param &param, અચલ Flag &flag)
{
	જો (!flag.detect_underspeed_enabled || !flag.airspeed_enabled) {
		_ratio_undersped = 0.0f;
		પાછા;
	}

	// Ramp underspeed ratio from 0 at tas_min to 1 at 90% of tas_min, or halfway to tas_stall if that is higher
	અચલ દશાંશ tas_fully_undersped = math::min(param.tas_min,
					  math::max(0.9f * param.tas_min, 0.5f * (param.tas_min + param.tas_stall)));
	અચલ દશાંશ underspeed_ramp_width = param.tas_min - tas_fully_undersped;

	_ratio_undersped = math::constrain((param.tas_min - આગત.tas) / math::max(underspeed_ramp_width, FLT_EPSILON),
					   0.0f, 1.0f);
}

TECSControl::SpecificEnergyWeighting TECSControl::_updateSpeedAltitudeWeights(અચલ Param &param, અચલ Flag &flag)
{

	SpecificEnergyWeighting weight;
	// Calculate the weight applied to control of specific kinetic energy error
	દશાંશ pitch_speed_weight = constrain(param.pitch_speed_weight, 0.0f, 2.0f);

	// Underspeed or fast descend: interpolate speed weight from nominal
	// towards max. We take the maximum of the two - this keeps the speed
	// weight continuous, even when underspeed and fast descend are combined
	અચલ દશાંશ max_ratio = fmaxf(_ratio_undersped, param.fast_descend);
	pitch_speed_weight = max_ratio * 2.0f + (1.0f - max_ratio) * pitch_speed_weight;

	જો (!flag.airspeed_enabled) {
		pitch_speed_weight = 0.0f;
	}

	weight.spe_weighting = constrain(2.0f - pitch_speed_weight, 0.f, 2.f);
	weight.ske_weighting = constrain(pitch_speed_weight, 0.f, 2.f);

	પાછા weight;
}

દશાંશ TECSControl::_calcPitchControlSkeRateSetpoint(અચલ Setpoint &setpoint, અચલ આગત_2 &આગત,
		અચલ STERateLimit &limit, અચલ Param &param, અચલ Flag &flag, અચલ SpecificEnergyWeighting &weight) અચલ
{
	// Trading altitude for speed with pitch is only wanted up to trim airspeed. Above trim, accelerating is left to
	// the throttle loop: if it saturates, the vehicle settles at a lower airspeed instead of below the altitude setpoint.
	// The airspeed setpoint seen by the pitch loop is therefore capped at max(trim, current airspeed). Between trim
	// and the setpoint the pitch loop holds altitude, while an overspeed above the setpoint is still corrected.
	// With a speed weight of 2 (speed priority, e.g. gliders, fast descend or underspeed) pitch is the only way to
	// track the setpoint, so the cap is faded out as the weight goes from 1 to 2.
	અચલ દશાંશ tas_setpoint_capped = min(setpoint.tas_setpoint, max(param.tas_trim, આગત.tas));

	Setpoint pitch_control_setpoint{setpoint};
	pitch_control_setpoint.tas_setpoint = lerp(tas_setpoint_capped, setpoint.tas_setpoint,
					      constrain(weight.ske_weighting - 1.f, 0.f, 1.f));

	// d/dt(0.5 V^2) = V * dV/dt: the measured airspeed, not the setpoint, keeps the airspeed loop linear
	પાછા આગત.tas * _calcAirspeedControlOutput(pitch_control_setpoint, આગત, limit, param, flag);
}

TECSControl::SpecificEnergyRates TECSControl::_calcPitchControlEnergyRates(અચલ Setpoint &setpoint,
		અચલ આગત_2 &આગત, અચલ STERateLimit &limit, અચલ SpecificEnergyRates &specific_energy_rates,
		અચલ Param &param, અચલ Flag &flag, અચલ SpecificEnergyWeighting &weight) અચલ
{
	SpecificEnergyRates pitch_specific_energy_rates{specific_energy_rates};
	pitch_specific_energy_rates.ske_rate.setpoint = _calcPitchControlSkeRateSetpoint(setpoint, આગત, limit, param, flag,
			weight);

	// Between trim airspeed and the airspeed setpoint the throttle alone accelerates the vehicle. The capped setpoint
	// above already removes the kinetic setpoint of the pitch loop there, faded in with speed priority. But the
	// throttle's acceleration would still show up in the kinetic energy rate estimate, and the pitch loop would pitch
	// up to turn it into height. Remove the estimate from the pitch loop in this band as well, with the same fade, so
	// that pitch controls height only. The setpoint must not be scaled again, or it would be faded twice.
	અચલ તાર્કિક throttle_accelerates = આગત.tas > param.tas_trim && આગત.tas < setpoint.tas_setpoint;
	અચલ દશાંશ kinetic_estimate_gain = throttle_accelerates ? constrain(weight.ske_weighting - 1.f, 0.f, 1.f) : 1.f;
	pitch_specific_energy_rates.ske_rate.estimate *= kinetic_estimate_gain;

	પાછા pitch_specific_energy_rates;
}

શૂન્ય TECSControl::_calcPitchControl(દશાંશ dt, અચલ Setpoint &setpoint, અચલ આગત_2 &આગત, અચલ STERateLimit &limit,
				    અચલ SpecificEnergyRates &specific_energy_rates, અચલ Param &param, અચલ Flag &flag)
{
	અચલ SpecificEnergyWeighting weight{_updateSpeedAltitudeWeights(param, flag)};

	અચલ SpecificEnergyRates pitch_specific_energy_rates{_calcPitchControlEnergyRates(setpoint, આગત, limit,
			specific_energy_rates, param, flag, weight)};

	Control_કિંમતો seb_rate{_calcPitchControlSebRate(weight, pitch_specific_energy_rates)};

	_calcPitchControlUpdate(dt, આગત, seb_rate, param);
	અચલ દશાંશ pitch_setpoint{_calcPitchControlOutput(આગત, seb_rate, param)};

	// Comply with the specified vertical acceleration limit by applying a pitch rate limit
	// NOTE: at zero airspeed, the pitch increment is unbounded
	અચલ દશાંશ pitch_increment = dt * param.vert_accel_limit / math::max(આગત.tas, FLT_EPSILON);
	_pitch_setpoint = constrain(pitch_setpoint, _pitch_setpoint - pitch_increment,
				    _pitch_setpoint + pitch_increment);
	_pitch_setpoint = constrain(_pitch_setpoint, param.pitch_min, param.pitch_max);

	//Debug Output
	_debug_output.energy_balance_rate_estimate = seb_rate.estimate;
	_debug_output.energy_balance_rate_sp = seb_rate.setpoint;
	_debug_output.pitch_integrator = _pitch_integ_state;
}

TECSControl::Control_કિંમતો TECSControl::_calcPitchControlSebRate(અચલ SpecificEnergyWeighting &weight,
		અચલ SpecificEnergyRates &specific_energy_rates) અચલ
{
	Control_કિંમતો seb_rate;
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
	seb_rate.setpoint = specific_energy_rates.spe_rate.setpoint * weight.spe_weighting -
			    specific_energy_rates.ske_rate.setpoint *
			    weight.ske_weighting;

	seb_rate.estimate = (specific_energy_rates.spe_rate.estimate * weight.spe_weighting) -
			    (specific_energy_rates.ske_rate.estimate * weight.ske_weighting);

	પાછા seb_rate;
}

શૂન્ય TECSControl::_calcPitchControlUpdate(દશાંશ dt, અચલ આગત_2 &આગત, અચલ Control_કિંમતો &seb_rate,
		અચલ Param &param)
{

	// In fast descend, the pitch integrator converges to the (substantially
	// negative) pitch. Ramp it down with decreasing fast descend ratio,
	// such that it is zero when the ratio becomes 0 again.
	જો (param.fast_descend < _fast_descend_prev && _fast_descend_prev > FLT_EPSILON) {
		_pitch_integ_state *= param.fast_descend / _fast_descend_prev;
	}

	_fast_descend_prev = param.fast_descend;

	જો (param.integrator_gain_pitch > FLT_EPSILON) {

		અચલ દશાંશ climb_angle_to_SEB_rate = _climbAngleToSebRate(આગત, param);

		// Calculate pitch integrator input term
		દશાંશ pitch_integ_આગત = _getControlError(seb_rate) * param.integrator_gain_pitch / climb_angle_to_SEB_rate;

		// Guard against NaN integrator input - discard bad input sample (e.g., exiting offboard velocity mode)
		જો (!PX4_ISFINITE(pitch_integ_આગત)) {
			pitch_integ_આગત = 0.f;
		}

		// Prevent the integrator changing in a direction that will increase pitch demand saturation
		જો (_pitch_setpoint >= param.pitch_max) {
			pitch_integ_આગત = min(pitch_integ_આગત, 0.f);

		} નહીંતર જો (_pitch_setpoint <= param.pitch_min) {
			pitch_integ_આગત = max(pitch_integ_આગત, 0.f);
		}

		// Update the pitch integrator state
		_pitch_integ_state = _pitch_integ_state + pitch_integ_આગત * dt;

		// Safety check: reset integrator if state became corrupted
		જો (!PX4_ISFINITE(_pitch_integ_state)) {
			_pitch_integ_state = 0.f;
		}

	} નહીંતર {
		_pitch_integ_state = 0.0f;
	}
}

દશાંશ TECSControl::_climbAngleToSebRate(અચલ આગત_2 &આગત, અચલ Param &param)
{
	// max() returns its second argument for a NaN first one, so a NaN airspeed falls back to the minimum airspeed
	પાછા max(આગત.tas, param.tas_min, FLT_EPSILON) * CONSTANTS_ONE_G;
}

દશાંશ TECSControl::_calcPitchControlOutput(અચલ આગત_2 &આગત, અચલ Control_કિંમતો &seb_rate, અચલ Param &param) અચલ
{
	// Calculate a specific energy correction that doesn't include the integrator contribution
	દશાંશ SEB_rate_correction = _getControlError(seb_rate) * param.pitch_damping_gain +
				    param.seb_rate_ff *
				    seb_rate.setpoint;

	// Convert the specific energy balance rate correction to a climb angle and add the level-flight pitch, the angle
	// of attack at zero climb angle, to get the absolute pitch setpoint. This calculation assumes:
	// a) The climb angle follows pitch angle with a lag that is small enough not to destabilise the control loop.
	// b) The offset between climb angle and pitch angle (angle of attack) is constant and equal to the level-flight
	// pitch, excluding the effect of pitch transients due to control action or turbulence.
	દશાંશ pitch_setpoint_unc = SEB_rate_correction / _climbAngleToSebRate(આગત, param) + _pitch_integ_state
				   + param.level_flight_pitch;

	// Guard against a non-finite feedforward/damping contribution (e.g. seb_rate going NaN on the energy/speed
	// side when exiting offboard velocity mode). constrain() does not reject NaN, so fall back to the integrator
	// state, which is kept finite by _calcPitchControlUpdate, to avoid latching a NaN pitch demand.
	જો (!PX4_ISFINITE(pitch_setpoint_unc)) {
		pitch_setpoint_unc = _pitch_integ_state + param.level_flight_pitch;
	}

	પાછા constrain(pitch_setpoint_unc, param.pitch_min, param.pitch_max);
}

શૂન્ય TECSControl::_calcThrottleControl(દશાંશ dt, અચલ STERateLimit &limit,
				       અચલ SpecificEnergyRates &specific_energy_rates, અચલ Param &param, અચલ Flag &flag)
{
	// Update STE rate estimate LP filter
	અચલ દશાંશ STE_rate_estimate_raw = specific_energy_rates.spe_rate.estimate + specific_energy_rates.ske_rate.estimate;
	_ste_rate_estimate_filter.setParameters(dt, param.ste_rate_time_const);
	_ste_rate_estimate_filter.update(STE_rate_estimate_raw);
	Control_કિંમતો ste_rate{_calcThrottleControlSteRate(limit, specific_energy_rates, param)};
	// During fast descend, we control airspeed over the pitch control loop. Give minimal thrust as soon as we are descending
	દશાંશ throttle_setpoint{param.throttle_min};

	જો (!fastDescendFullyEngaged(param.fast_descend)) {
		_calcThrottleControlUpdate(dt, limit, ste_rate, param, flag);
		throttle_setpoint = (1.f - param.fast_descend) * _calcThrottleControlOutput(limit, ste_rate, param,
				    flag) + param.fast_descend * param.throttle_min;
	}

	// Rate limit the throttle demand
	જો (fabsf(param.throttle_slewrate) > FLT_EPSILON) {
		અચલ દશાંશ throttle_increment_limit = dt * (param.throttle_max - param.throttle_min) * param.throttle_slewrate;
		throttle_setpoint = constrain(throttle_setpoint, _throttle_setpoint - throttle_increment_limit,
					      _throttle_setpoint + throttle_increment_limit);
	}

	_throttle_setpoint = constrain(throttle_setpoint, param.throttle_min, param.throttle_max);

	// Debug output
	_debug_output.total_energy_rate_estimate = ste_rate.estimate;
	_debug_output.total_energy_rate_sp = ste_rate.setpoint;
	_debug_output.throttle_integrator = _throttle_integ_state;
}

TECSControl::Control_કિંમતો TECSControl::_calcThrottleControlSteRate(અચલ STERateLimit &limit,
		અચલ SpecificEnergyRates &specific_energy_rates,
		અચલ Param &param) અચલ
{
	// Output ste rate values
	Control_કિંમતો ste_rate;
	ste_rate.setpoint = specific_energy_rates.spe_rate.setpoint + specific_energy_rates.ske_rate.setpoint;

	// Adjust the demanded total energy rate to compensate for induced drag rise in turns.
	// Assume induced drag scales linearly with normal load factor.
	// The additional normal load factor is given by (1/cos(bank angle) - 1)
	ste_rate.setpoint += param.load_factor_correction * (param.load_factor - 1.f);

	ste_rate.setpoint = constrain(ste_rate.setpoint, limit.STE_rate_min, limit.STE_rate_max);
	ste_rate.estimate = _ste_rate_estimate_filter.getState();

	પાછા ste_rate;
}

શૂન્ય TECSControl::_calcThrottleControlUpdate(દશાંશ dt, અચલ STERateLimit &limit, અચલ Control_કિંમતો &ste_rate,
		અચલ Param &param, અચલ Flag &flag)
{
	અચલ દશાંશ STE_rate_to_throttle = _steRateToThrottle(limit);

	// Integral handling
	જો (flag.airspeed_enabled) {
		જો (param.integrator_gain_throttle > FLT_EPSILON) {
			// underspeed conditions zero out integration
			દશાંશ throttle_integ_આગત = (_getControlError(ste_rate) * param.integrator_gain_throttle) * dt *
						     STE_rate_to_throttle * (1.0f - _ratio_undersped);

			// only allow integrator propagation into direction which unsaturates throttle
			જો (_throttle_setpoint >= param.throttle_max) {
				throttle_integ_આગત = math::min(0.f, throttle_integ_આગત);

			}

			જો (_throttle_setpoint <= param.throttle_min) {
				throttle_integ_આગત = math::max(0.f, throttle_integ_આગત);
			}

			// Calculate a throttle demand from the integrated total energy rate error
			// This will be added to the total throttle demand to compensate for steady state errors
			_throttle_integ_state = PX4_ISFINITE(throttle_integ_આગત) ? _throttle_integ_state + throttle_integ_આગત : 0.f;

		} નહીંતર {
			_throttle_integ_state = 0.0f;
		}

	}
}

દશાંશ TECSControl::_calcThrottleControlOutput(અચલ STERateLimit &limit, અચલ Control_કિંમતો &ste_rate,
		અચલ Param &param,
		અચલ Flag &flag) અચલ
{
	અચલ દશાંશ STE_rate_to_throttle = _steRateToThrottle(limit);

	// Calculate a predicted throttle from the demanded rate of change of energy, using the cruise throttle
	// as the starting point. Assume:
	// Specific total energy rate = _STE_rate_max is achieved when throttle is set to _throttle_setpoint_max
	// Specific total energy rate = 0 at cruise throttle
	// Specific total energy rate = _STE_rate_min is achieved when throttle is set to _throttle_setpoint_min

	// assume airspeed and density-independent delta_throttle to sink/climb rate mapping
	// TODO: include air density for thrust mappings
	અચલ દશાંશ throttle_above_trim_per_ste_rate = (param.throttle_max - param.throttle_trim) / limit.STE_rate_max;
	અચલ દશાંશ throttle_below_trim_per_ste_rate = (param.throttle_trim - param.throttle_min) / limit.STE_rate_min;

	દશાંશ throttle_predicted = 0.0f;

	જો (ste_rate.setpoint >= FLT_EPSILON) {
		// throttle is between trim and maximum
		throttle_predicted = param.throttle_trim + ste_rate.setpoint * throttle_above_trim_per_ste_rate;

	} નહીંતર {
		// throttle is between trim and minimum
		throttle_predicted = param.throttle_trim - ste_rate.setpoint * throttle_below_trim_per_ste_rate;

	}

	// Add proportional and derivative control feedback to the predicted throttle and constrain to throttle limits
	દશાંશ throttle_setpoint = (_getControlError(ste_rate) * param.throttle_damping_gain) * STE_rate_to_throttle +
				  throttle_predicted;

	જો (flag.airspeed_enabled) {
		// Add the integrator feedback during closed loop operation with an airspeed sensor
		throttle_setpoint += _throttle_integ_state;

	} નહીંતર {
		/* We want to avoid reducing the throttle output when switching from airspeed enabled mode into airspeedless mode.
		  Thus, if the throttle integrator has a positive value, add it still to the throttle setpoint. */
		throttle_setpoint += math::max(0.0f, _throttle_integ_state);
	}


	// ramp in max throttle setting with underspeediness value
	throttle_setpoint = _ratio_undersped * param.throttle_max + (1.0f - _ratio_undersped) * throttle_setpoint;

	પાછા constrain(throttle_setpoint, param.throttle_min, param.throttle_max);
}

શૂન્ય TECSControl::resetIntegrals()
{
	_pitch_integ_state = 0.0f;
	_throttle_integ_state = 0.0f;
}

શૂન્ય TECS::initControlParams(અચલ આગત_2 &આગત)
{
	// Update parameters from input
	// Reference model
	_reference_param.target_climbrate = આગત.target_climbrate;
	_reference_param.target_sinkrate = આગત.target_sinkrate;
	// Control
	_control_param.tas_min = આગત.eas_to_tas * _equivalent_airspeed_min;
	_control_param.tas_stall = આગત.eas_to_tas * _equivalent_airspeed_stall;
	_control_param.tas_max = આગત.eas_to_tas * _equivalent_airspeed_max;
	_control_param.tas_trim = આગત.eas_to_tas * _control_param.equivalent_airspeed_trim;
	_control_param.pitch_max = આગત.pitch_max;
	_control_param.pitch_min = આગત.pitch_min;
	_control_param.throttle_trim = આગત.throttle_trim;
	_control_param.throttle_max = આગત.throttle_max;
	_control_param.throttle_min = આગત.throttle_min;
}

દશાંશ TECS::calcTrueAirspeedSetpoint(દશાંશ eas_to_tas, દશાંશ eas_setpoint)
{
	પાછા lerp(eas_to_tas * eas_setpoint, _control_param.tas_max, _fast_descend);
}

શૂન્ય TECS::initialize(અચલ આગત_2 &આગત)
{
	// Init subclasses
	TECSAltitudeReferenceModel::AltitudeReferenceState current_state{.alt = આગત.altitude,
			.alt_rate = આગત.altitude_rate};
	_altitude_reference_model.initialize(current_state);
	_airspeed_filter.initialize(આગત.equivalent_airspeed, _airspeed_filter_param.equivalent_airspeed_trim,
				    _control_flag.airspeed_enabled);

	// Seed the controller from the filter, which falls back to trim airspeed without a valid measurement,
	// so the controller sees the same airspeed on reset as on every following update
	અચલ દશાંશ true_airspeed = આગત.eas_to_tas * _airspeed_filter.getState().speed;

	TECSControl::Setpoint control_setpoint;
	control_setpoint.altitude_reference = _altitude_reference_model.getAltitudeReference();
	control_setpoint.altitude_rate_setpoint_direct = NAN; // initialization always goes through the altitude loop
	control_setpoint.tas_setpoint = true_airspeed;

	અચલ TECSControl::આગત_2 control_આગત{ .altitude = આગત.altitude,
						.altitude_rate = આગત.altitude_rate,
						.tas = true_airspeed,
						.tas_rate = 0.0f};

	_control.initialize(control_setpoint, control_આગત, _control_param, _control_flag);

	_fast_descend = 0.f;
	_enabled_fast_descend_timestamp = 0U;
}

શૂન્ય TECS::update(અચલ આગત_2 &આગત)
{
	// Calculate the time since last update (seconds)
	અચલ hrt_abstime now = hrt_absolute_time();
	અચલ દશાંશ dt = static_cast<દશાંશ>(now - _update_timestamp) * 1e-6f;

	initControlParams(આગત);

	જો (dt < DT_MIN) {
		// Update interval too small, do not update. Assume constant states/output in this case.
		પાછા;
	}

	દશાંશ true_airspeed_setpoint;

	જો (dt > DT_MAX || _update_timestamp == 0UL) {
		// Update time interval too large, can't guarantee sanity of state updates anymore. reset the control loop.
		initialize(આગત);
		true_airspeed_setpoint = calcTrueAirspeedSetpoint(આગત.eas_to_tas, આગત.equivalent_airspeed_setpoint);

	} નહીંતર {
		/* Check if we want to fast descend. On fast descend, we set the throttle to min, and use the altitude control
		loop to control the speed to the maximum airspeed. */
		_setFastDescend(આગત.altitude_setpoint, આગત.altitude);
		_control_param.fast_descend = _fast_descend;

		// Update airspeedfilter submodule
		અચલ TECSAirspeedFilter::આગત_2 airspeed_આગત{ .equivalent_airspeed = આગત.equivalent_airspeed,
				.equivalent_airspeed_rate = આગત.speed_deriv_forward / આગત.eas_to_tas};

		_airspeed_filter.update(dt, airspeed_આગત, _airspeed_filter_param, _control_flag.airspeed_enabled);

		// Update Reference model submodule
		જો (fastDescendFullyEngaged(_fast_descend)) {
			// Reset the altitude reference model, while we are in fast descend.
			અચલ TECSAltitudeReferenceModel::AltitudeReferenceState init_state{
				.alt = આગત.altitude,
				.alt_rate = આગત.altitude_rate};
			_altitude_reference_model.initialize(init_state);

		} નહીંતર જો (_control.getUnderspeedRatio() > 0.5f) {
			// Reset altitude reference during underspeed to prevent altitude error accumulation
			// that would cause aggressive pitch-up (and potential secondary stall) on recovery.
			અચલ TECSAltitudeReferenceModel::AltitudeReferenceState init_state{
				.alt = આગત.altitude,
				.alt_rate = આગત.altitude_rate};
			_altitude_reference_model.initialize(init_state);

		} નહીંતર {
			અચલ TECSAltitudeReferenceModel::AltitudeReferenceState setpoint{ .alt = આગત.altitude_setpoint,
					.alt_rate = આગત.altitude_rate_setpoint};

			_altitude_reference_model.update(dt, setpoint, આગત.altitude, આગત.altitude_rate, _reference_param);
		}

		TECSControl::Setpoint control_setpoint;
		control_setpoint.altitude_reference = _altitude_reference_model.getAltitudeReference();
		control_setpoint.altitude_rate_setpoint_direct = _altitude_reference_model.getHeightRateSetpointDirect();
		true_airspeed_setpoint = calcTrueAirspeedSetpoint(આગત.eas_to_tas, આગત.equivalent_airspeed_setpoint);
		control_setpoint.tas_setpoint = true_airspeed_setpoint;

		અચલ TECSAirspeedFilter::AirspeedFilterState airspeed_state{_airspeed_filter.getState()};
		અચલ TECSControl::આગત_2 control_આગત{ .altitude = આગત.altitude,
							.altitude_rate = આગત.altitude_rate,
							.tas = આગત.eas_to_tas * airspeed_state.speed,
							.tas_rate = આગત.eas_to_tas * airspeed_state.speed_rate};

		_control.update(dt, control_setpoint, control_આગત, _control_param, _control_flag);
	}

	_debug_status.control = _control.getDebugOutput();
	_debug_status.true_airspeed_sp = true_airspeed_setpoint;
	અચલ TECSAirspeedFilter::AirspeedFilterState airspeed_state{_airspeed_filter.getState()};
	અચલ TECSAltitudeReferenceModel::AltitudeReferenceState altitude_reference{_altitude_reference_model.getAltitudeReference()};
	_debug_status.true_airspeed_filtered = આગત.eas_to_tas * airspeed_state.speed;
	_debug_status.true_airspeed_derivative = આગત.eas_to_tas * airspeed_state.speed_rate;
	_debug_status.altitude_reference = altitude_reference.alt;
	_debug_status.height_rate_reference = altitude_reference.alt_rate;
	_debug_status.height_rate_direct = _altitude_reference_model.getHeightRateSetpointDirect();
	_debug_status.fast_descend = _fast_descend;

	_update_timestamp = now;
}

શૂન્ય TECS::_setFastDescend(અચલ દશાંશ alt_setpoint, અચલ દશાંશ alt)
{
	// disable fast descend if we are close to the target altitude or the altitude setpoint is not finite

	જો (PX4_ISFINITE(alt_setpoint) && _control_flag.airspeed_enabled && (_fast_descend_alt_err > FLT_EPSILON)
	    && ((alt_setpoint + _fast_descend_alt_err) < alt)) {
		auto now = hrt_absolute_time();

		જો (_enabled_fast_descend_timestamp == 0U) {
			_enabled_fast_descend_timestamp = now;
		}

		_fast_descend = constrain(max(_fast_descend, static_cast<દશાંશ>(now - _enabled_fast_descend_timestamp) /
					      static_cast<દશાંશ>(FAST_DESCEND_RAMP_UP_TIME)), 0.f, 1.f);

	} નહીંતર જો (PX4_ISFINITE(alt_setpoint) && (_fast_descend > FLT_EPSILON) && (_fast_descend_alt_err > FLT_EPSILON)) {
		// We're in fast descend, scale it down until FAST_DESCEND_HEIGHT_MARGIN above target altitude
		_fast_descend = constrain((alt - alt_setpoint - FAST_DESCEND_HEIGHT_MARGIN) / _fast_descend_alt_err, 0.f, 1.f);
		_enabled_fast_descend_timestamp = 0U;

	} નહીંતર {
		_fast_descend = 0.f;
		_enabled_fast_descend_timestamp = 0U;
	}
}

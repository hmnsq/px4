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

استفاده math::constrain;
استفاده math::max;
استفاده math::min;

ساكن constexpr منطقي is_Valid_زمان_گام(عائم dt) { أرجع (PX4_ISFINITE(dt) && dt > FLT_EPSILON);}
ساكن constexpr منطقي fastDescendFullyEngaged(عائم fast_descend) { أرجع 1.f - fast_descend < FLT_EPSILON; }

فراغ TECSAirspeedFilter::initialize(ثابت عائم equivalent_airspeed, ثابت عائم equivalent_airspeed_trim,
				    ثابت منطقي airspeed_sensor_موجود)
{
	إذا (airspeed_sensor_موجود && PX4_ISFINITE(equivalent_airspeed)) {
		_airspeed_state.سرعت = equivalent_airspeed;

	} وإلا {
		_airspeed_state.سرعت = equivalent_airspeed_trim;
	}

	_airspeed_state.سرعت_نرخ = 0.f;
}

فراغ TECSAirspeedFilter::update(ثابت عائم dt, ثابت درون_داد_2 &درون_داد, ثابت Param &param,
				ثابت منطقي airspeed_sensor_موجود)
{
	// Input checking
	إذا (!is_Valid_زمان_گام(dt)) {
		// Do not update the states.
		أرجع;
	}

	عائم airspeed;

	إذا (PX4_ISFINITE(درون_داد.equivalent_airspeed) && airspeed_sensor_موجود) {
		airspeed = درون_داد.equivalent_airspeed;

	} وإلا {
		airspeed = param.equivalent_airspeed_trim;
	}

	عائم airspeed_derivative;

	إذا (PX4_ISFINITE(درون_داد.equivalent_airspeed_نرخ) && airspeed_sensor_موجود) {
		airspeed_derivative = درون_داد.equivalent_airspeed_نرخ;

	} وإلا {
		airspeed_derivative = 0.0f;
	}

	/* Filter airspeed and rate using a constant airspeed rate model in a steady state Kalman Filter.
	 We use the gains of the continuous Kalman filter Kc and approximate the discrete version Kalman gain Kd =dt*Kc,
	 since the continuous algebraic Riccatti equation is easier to solve.
	*/

	ماتریس::Vector2f new_state_predicted;

	new_state_predicted(0) = _airspeed_state.سرعت + dt * _airspeed_state.سرعت_نرخ;
	new_state_predicted(1) = _airspeed_state.سرعت_نرخ;

	ثابت ماتریس::Vector2f innovation{(airspeed - new_state_predicted(0)), (airspeed_derivative - new_state_predicted(1))};

	ماتریس::Vector2f new_state;
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
	_airspeed_state.سرعت = new_state(0);
	_airspeed_state.سرعت_نرخ = new_state(1);
}

TECSAirspeedFilter::AirspeedFilterState TECSAirspeedFilter::getState() ثابت
{
	أرجع _airspeed_state;
}

فراغ TECSAltitudeReferenceModel::update(ثابت عائم dt, ثابت AltitudeReferenceState &setpoint, عائم altitude,
					عائم ارتفاع_نرخ, ثابت Param &param)
{
	// Input checks
	إذا (!is_Valid_زمان_گام(dt)) {
		// Do not update the states.
		أرجع;
	}

	ثابت عائم current_alt = PX4_ISFINITE(altitude) ? altitude : 0.f;

	_velocity_control_traj_generator.مجموعه_Max_Jerk(JERK_MAX);
	_velocity_control_traj_generator.مجموعه_Max_Accel_به_بالا(param.vert_accel_حد);
	_velocity_control_traj_generator.مجموعه_Max_Accel_پایین(param.vert_accel_حد);
	_velocity_control_traj_generator.مجموعه_Max_Vel_به_بالا(param.max_sink_نرخ); // different convention for FW than for MC
	_velocity_control_traj_generator.مجموعه_Max_Vel_پایین(param.max_climb_نرخ); // different convention for FW than for MC

	// Altitude setpoint reference
	_alt_control_traj_generator.مجموعه_Max_Jerk(JERK_MAX);
	_alt_control_traj_generator.مجموعه_Max_Accel(param.vert_accel_حد);
	_alt_control_traj_generator.مجموعه_Max_Vel(fmaxf(param.max_climb_نرخ, param.max_sink_نرخ));

	// XXX: this is a bit risky.. .alt_rate here could be NAN (by interface design) - and is only ok to input to the
	// setVelSpFeedback() method because it calls the reset in the logic below when it is NAN.
	// TODO: stop it with the NAN interfaces, make sure to take care of this when refactoring and separating altitude
	// and height rate control loops.
	_velocity_control_traj_generator.مجموعه_Vel_Sp_Feedback(setpoint.alt_نرخ);

	منطقي control_altitude = درست;
	عائم altitude_setpoint = setpoint.alt;

	إذا (!PX4_ISFINITE(setpoint.alt) && !PX4_ISFINITE(setpoint.alt_نرخ)) {
		// neither altitude nor altitude rate is set - reset to current altitude
		_velocity_control_traj_generator.reset(0.f, 0, current_alt);
		altitude_setpoint = current_alt;

	} وإلا إذا (PX4_ISFINITE(setpoint.alt_نرخ)) {
		// input is height rate (not altitude)
		_velocity_control_traj_generator.مجموعه_Current_Position_Estimate(current_alt);
		_velocity_control_traj_generator.update(dt, setpoint.alt_نرخ);
		altitude_setpoint = _velocity_control_traj_generator.getCurrentPosition();
		control_altitude = PX4_ISFINITE(altitude_setpoint); // returns true if altitude is locked

	} وإلا {
		// input is altitude
		_velocity_control_traj_generator.reset(0, ارتفاع_نرخ, altitude_setpoint);
	}

	إذا (control_altitude) {
		ثابت عائم هدف_climbrate_m_s = math::min(param.هدف_climbrate, param.max_climb_نرخ);
		ثابت عائم هدف_sinkrate_m_s = math::min(param.هدف_sinkrate, param.max_sink_نرخ);

		ثابت عائم delta_trajectory_مقصد_هدف_m = altitude_setpoint - _alt_control_traj_generator.getCurrentPosition();

		عائم ارتفاع_نرخ_هدف = math::signNoZero<عائم>(delta_trajectory_مقصد_هدف_m) *
					   math::trajectory::compute_Max_سرعت_From_فاصله(
						   JERK_MAX, param.vert_accel_حد, fabsf(delta_trajectory_مقصد_هدف_m), 0.f);

		ارتفاع_نرخ_هدف = math::constrain(ارتفاع_نرخ_هدف, -هدف_sinkrate_m_s, هدف_climbrate_m_s);

		_alt_control_traj_generator.updateDurations(ارتفاع_نرخ_هدف);
		_alt_control_traj_generator.updateTraj(dt);
		ارتفاع_نرخ_setpoint_مستقیم = NAN;

	} وإلا {
		_alt_control_traj_generator.مجموعه_Current_Velocity(_velocity_control_traj_generator.getCurrentVelocity());
		_alt_control_traj_generator.مجموعه_Current_Position(current_alt);
		ارتفاع_نرخ_setpoint_مستقیم = _velocity_control_traj_generator.getCurrentVelocity();
	}
}

TECSAltitudeReferenceModel::AltitudeReferenceState TECSAltitudeReferenceModel::getAltitudeReference() ثابت
{
	TECSAltitudeReferenceModel::AltitudeReferenceState ref{
		.alt = _alt_control_traj_generator.getCurrentPosition(),
		.alt_نرخ = _alt_control_traj_generator.getCurrentVelocity(),
	};

	أرجع ref;
}

فراغ TECSAltitudeReferenceModel::initialize(ثابت AltitudeReferenceState &state)
{
	ثابت عائم init_state_alt = PX4_ISFINITE(state.alt) ? state.alt : 0.f;
	ثابت عائم init_state_alt_نرخ = PX4_ISFINITE(state.alt_نرخ) ? state.alt_نرخ : 0.f;

	_alt_control_traj_generator.reset(0.0f, init_state_alt_نرخ, init_state_alt);
	_velocity_control_traj_generator.reset(0.f, init_state_alt_نرخ, init_state_alt);
}

فراغ TECSControl::initialize(ثابت Setpoint &setpoint, ثابت درون_داد_2 &درون_داد, ثابت Param &param, ثابت Flag &flag)
{
	resetIntegrals();

	// Initialization always goes through the altitude loop, even if a direct height rate setpoint is given
	Setpoint altitude_loop_setpoint{setpoint};
	altitude_loop_setpoint.altitude_نرخ_setpoint_مستقیم = NAN;

	ثابت STERate_حد حد_2{calculate_مجموع_Energy_نرخ_حد(param)};
	ثابت SpecificEnergyRates specific_energy_نرخ{_updateOuterLoops(altitude_loop_setpoint, درون_داد, حد_2, param, flag)};

	ثابت SpecificEnergyWeighting وزن{update_سرعت_Altitude_Weights(param, flag)};
	ثابت SpecificEnergyRates pitch_specific_energy_نرخ{_calcPitchControlEnergyRates(setpoint, درون_داد, حد_2,
			specific_energy_نرخ, param, flag, وزن)};
	Control_مقادیر seb_نرخ{calc_Pitch_Control_Seb_نرخ(وزن, pitch_specific_energy_نرخ)};

	_pitch_setpoint = _calcPitchControlOutput(درون_داد, seb_نرخ, param);

	ste_نرخ_estimate_filter.reset(specific_energy_نرخ.spe_نرخ.estimate + specific_energy_نرخ.ske_نرخ.estimate);

	Control_مقادیر ste_نرخ{calc_Throttle_Control_Ste_نرخ(حد_2, specific_energy_نرخ, param)};

	_throttle_setpoint = _calcThrottleControlOutput(حد_2, ste_نرخ, param, flag);

	// Debug output
	_debug_output.مجموع_energy_نرخ_estimate = ste_نرخ.estimate;
	_debug_output.مجموع_energy_نرخ_sp = ste_نرخ.setpoint;
	_debug_output.throttle_integrator = _throttle_integ_state;
	_debug_output.energy_balance_نرخ_estimate = seb_نرخ.estimate;
	_debug_output.energy_balance_نرخ_sp = seb_نرخ.setpoint;
	_debug_output.pitch_integrator = _pitch_integ_state;
}

فراغ TECSControl::update(ثابت عائم dt, ثابت Setpoint &setpoint, ثابت درون_داد_2 &درون_داد, ثابت Param &param,
			 ثابت Flag &flag)
{
	// Input checking
	إذا (!is_Valid_زمان_گام(dt)) {
		// Do not update the states and output.
		أرجع;
	}

	ثابت STERate_حد حد_2{calculate_مجموع_Energy_نرخ_حد(param)};
	ثابت SpecificEnergyRates specific_energy_نرخ{_updateOuterLoops(setpoint, درون_داد, حد_2, param, flag)};

	_calcPitchControl(dt, setpoint, درون_داد, حد_2, specific_energy_نرخ, param, flag);

	_calcThrottleControl(dt, حد_2, specific_energy_نرخ, param, flag);
}

TECSControl::SpecificEnergyRates TECSControl::_updateOuterLoops(ثابت Setpoint &setpoint, ثابت درون_داد_2 &درون_داد,
		ثابت STERate_حد &حد_2, ثابت Param &param, ثابت Flag &flag)
{
	OuterLoopSetpoints control_setpoint;

	control_setpoint.tas_نرخ_setpoint = _calcAirspeedControlOutput(setpoint, درون_داد, حد_2, param, flag);

	إذا (PX4_ISFINITE(setpoint.altitude_نرخ_setpoint_مستقیم)) {
		// direct height rate control
		control_setpoint.altitude_نرخ_setpoint = setpoint.altitude_نرخ_setpoint_مستقیم;

	} وإلا {
		// altitude is locked, go through altitude outer loop
		control_setpoint.altitude_نرخ_setpoint = _calcAltitudeControlOutput(setpoint, درون_داد, param);
	}

	ثابت SpecificEnergyRates specific_energy_نرخ{_calcSpecificEnergyRates(control_setpoint, درون_داد)};

	_detectUnderspeed(درون_داد, param, flag);

	_debug_output.altitude_نرخ_control = control_setpoint.altitude_نرخ_setpoint;
	_debug_output.true_airspeed_derivative_control = control_setpoint.tas_نرخ_setpoint;

	أرجع specific_energy_نرخ;
}

TECSControl::STERate_حد TECSControl::calculate_مجموع_Energy_نرخ_حد(ثابت Param &param) ثابت
{
	TECSControl::STERate_حد حد_2;
	// Calculate the specific total energy rate limits from the max throttle limits
	حد_2.STE_نرخ_max = math::max(param.max_climb_نرخ, FLT_EPSILON) * CONSTANTS_ONE_G;
	حد_2.STE_نرخ_min = - math::max(param.min_sink_نرخ, FLT_EPSILON) * CONSTANTS_ONE_G;

	أرجع حد_2;
}

عائم TECSControl::_calcAirspeedControlOutput(ثابت Setpoint &setpoint, ثابت درون_داد_2 &درون_داد, ثابت STERate_حد &حد_2,
		ثابت Param &param, ثابت Flag &flag) ثابت
{
	عائم airspeed_نرخ_output{0.0f};

	// calculate the demanded true airspeed rate of change based on first order response of true airspeed error
	// if airspeed measurement is not enabled then always set the rate setpoint to zero in order to avoid constant rate setpoints
	إذا (flag.airspeed_enabled) {
		// Calculate limits for the demanded rate of change of speed based on physical performance limits
		// with a 50% margin to allow the total energy controller to correct for errors. Increase it in case of fast descend
		ثابت عائم max_tas_نرخ_sp = (param.fast_descend * 0.5f + 0.5f) * حد_2.STE_نرخ_max / math::max(درون_داد.tas,
					      FLT_EPSILON);
		ثابت عائم min_tas_نرخ_sp = (param.fast_descend * 0.5f + 0.5f) * حد_2.STE_نرخ_min / math::max(درون_داد.tas,
					      FLT_EPSILON);
		airspeed_نرخ_output = constrain((setpoint.tas_setpoint - درون_داد.tas) * param.airspeed_error_gain, min_tas_نرخ_sp,
						 max_tas_نرخ_sp);
	}

	أرجع airspeed_نرخ_output;
}

عائم TECSControl::_calcAltitudeControlOutput(ثابت Setpoint &setpoint, ثابت درون_داد_2 &درون_داد, ثابت Param &param) ثابت
{
	عائم altitude_نرخ_output;
	altitude_نرخ_output = (setpoint.altitude_reference.alt - درون_داد.altitude) * param.altitude_error_gain
			       + param.altitude_setpoint_gain_ff * setpoint.altitude_reference.alt_نرخ;

	altitude_نرخ_output = math::constrain(altitude_نرخ_output, -param.max_sink_نرخ, param.max_climb_نرخ);

	أرجع altitude_نرخ_output;
}

TECSControl::SpecificEnergyRates TECSControl::_calcSpecificEnergyRates(ثابت OuterLoopSetpoints &control_setpoint,
		ثابت درون_داد_2 &درون_داد) ثابت
{
	SpecificEnergyRates specific_energy_rates;
	// Calculate specific energy rate demands in units of (m**2/sec**3)
	specific_energy_rates.spe_نرخ.setpoint = control_setpoint.altitude_نرخ_setpoint *
			CONSTANTS_ONE_G; // potential energy rate of change

	// d/dt(0.5 V^2) = V * dV/dt: use measured TAS, not the TAS setpoint, to keep the airspeed loop linear
	specific_energy_rates.ske_نرخ.setpoint = درون_داد.tas *
			control_setpoint.tas_نرخ_setpoint; // kinetic energy rate of change

	// Calculate specific energy rates in units of (m**2/sec**3)
	specific_energy_rates.spe_نرخ.estimate = درون_داد.altitude_نرخ * CONSTANTS_ONE_G; // potential energy rate of change
	specific_energy_rates.ske_نرخ.estimate = درون_داد.tas * درون_داد.tas_نرخ;// kinetic energy rate of change

	أرجع specific_energy_rates;
}

فراغ TECSControl::_detectUnderspeed(ثابت درون_داد_2 &درون_داد, ثابت Param &param, ثابت Flag &flag)
{
	إذا (!flag.detect_underspeed_enabled || !flag.airspeed_enabled) {
		نسبت_undersped = 0.0f;
		أرجع;
	}

	// Ramp underspeed ratio from 0 at tas_min to 1 at 90% of tas_min, or halfway to tas_stall if that is higher
	ثابت عائم tas_fully_undersped = math::min(param.tas_min,
					  math::max(0.9f * param.tas_min, 0.5f * (param.tas_min + param.tas_stall)));
	ثابت عائم underspeed_ramp_عرض = param.tas_min - tas_fully_undersped;

	نسبت_undersped = math::constrain((param.tas_min - درون_داد.tas) / math::max(underspeed_ramp_عرض, FLT_EPSILON),
					   0.0f, 1.0f);
}

TECSControl::SpecificEnergyWeighting TECSControl::update_سرعت_Altitude_Weights(ثابت Param &param, ثابت Flag &flag)
{

	SpecificEnergyWeighting وزن;
	// Calculate the weight applied to control of specific kinetic energy error
	عائم pitch_سرعت_وزن = constrain(param.pitch_سرعت_وزن, 0.0f, 2.0f);

	// Underspeed or fast descend: interpolate speed weight from nominal
	// towards max. We take the maximum of the two - this keeps the speed
	// weight continuous, even when underspeed and fast descend are combined
	ثابت عائم max_نسبت = fmaxf(نسبت_undersped, param.fast_descend);
	pitch_سرعت_وزن = max_نسبت * 2.0f + (1.0f - max_نسبت) * pitch_سرعت_وزن;

	إذا (!flag.airspeed_enabled) {
		pitch_سرعت_وزن = 0.0f;
	}

	وزن.spe_weighting = constrain(2.0f - pitch_سرعت_وزن, 0.f, 2.f);
	وزن.ske_weighting = constrain(pitch_سرعت_وزن, 0.f, 2.f);

	أرجع وزن;
}

عائم TECSControl::calc_Pitch_Control_Ske_نرخ_Setpoint(ثابت Setpoint &setpoint, ثابت درون_داد_2 &درون_داد,
		ثابت STERate_حد &حد_2, ثابت Param &param, ثابت Flag &flag, ثابت SpecificEnergyWeighting &وزن) ثابت
{
	// Trading altitude for speed with pitch is only wanted up to trim airspeed. Above trim, accelerating is left to
	// the throttle loop: if it saturates, the vehicle settles at a lower airspeed instead of below the altitude setpoint.
	// The airspeed setpoint seen by the pitch loop is therefore capped at max(trim, current airspeed). Between trim
	// and the setpoint the pitch loop holds altitude, while an overspeed above the setpoint is still corrected.
	// With a speed weight of 2 (speed priority, e.g. gliders, fast descend or underspeed) pitch is the only way to
	// track the setpoint, so the cap is faded out as the weight goes from 1 to 2.
	ثابت عائم tas_setpoint_capped = min(setpoint.tas_setpoint, max(param.tas_trim, درون_داد.tas));

	Setpoint pitch_control_setpoint{setpoint};
	pitch_control_setpoint.tas_setpoint = lerp(tas_setpoint_capped, setpoint.tas_setpoint,
					      constrain(وزن.ske_weighting - 1.f, 0.f, 1.f));

	// d/dt(0.5 V^2) = V * dV/dt: the measured airspeed, not the setpoint, keeps the airspeed loop linear
	أرجع درون_داد.tas * _calcAirspeedControlOutput(pitch_control_setpoint, درون_داد, حد_2, param, flag);
}

TECSControl::SpecificEnergyRates TECSControl::_calcPitchControlEnergyRates(ثابت Setpoint &setpoint,
		ثابت درون_داد_2 &درون_داد, ثابت STERate_حد &حد_2, ثابت SpecificEnergyRates &specific_energy_rates,
		ثابت Param &param, ثابت Flag &flag, ثابت SpecificEnergyWeighting &وزن) ثابت
{
	SpecificEnergyRates pitch_specific_energy_rates{specific_energy_rates};
	pitch_specific_energy_rates.ske_نرخ.setpoint = calc_Pitch_Control_Ske_نرخ_Setpoint(setpoint, درون_داد, حد_2, param, flag,
			وزن);

	// Between trim airspeed and the airspeed setpoint the throttle alone accelerates the vehicle. The capped setpoint
	// above already removes the kinetic setpoint of the pitch loop there, faded in with speed priority. But the
	// throttle's acceleration would still show up in the kinetic energy rate estimate, and the pitch loop would pitch
	// up to turn it into height. Remove the estimate from the pitch loop in this band as well, with the same fade, so
	// that pitch controls height only. The setpoint must not be scaled again, or it would be faded twice.
	ثابت منطقي throttle_accelerates = درون_داد.tas > param.tas_trim && درون_داد.tas < setpoint.tas_setpoint;
	ثابت عائم kinetic_estimate_gain = throttle_accelerates ? constrain(وزن.ske_weighting - 1.f, 0.f, 1.f) : 1.f;
	pitch_specific_energy_rates.ske_نرخ.estimate *= kinetic_estimate_gain;

	أرجع pitch_specific_energy_rates;
}

فراغ TECSControl::_calcPitchControl(عائم dt, ثابت Setpoint &setpoint, ثابت درون_داد_2 &درون_داد, ثابت STERate_حد &حد_2,
				    ثابت SpecificEnergyRates &specific_energy_rates, ثابت Param &param, ثابت Flag &flag)
{
	ثابت SpecificEnergyWeighting وزن{update_سرعت_Altitude_Weights(param, flag)};

	ثابت SpecificEnergyRates pitch_specific_energy_rates{_calcPitchControlEnergyRates(setpoint, درون_داد, حد_2,
			specific_energy_rates, param, flag, وزن)};

	Control_مقادیر seb_نرخ{calc_Pitch_Control_Seb_نرخ(وزن, pitch_specific_energy_rates)};

	_calcPitchControlUpdate(dt, درون_داد, seb_نرخ, param);
	ثابت عائم pitch_setpoint{_calcPitchControlOutput(درون_داد, seb_نرخ, param)};

	// Comply with the specified vertical acceleration limit by applying a pitch rate limit
	// NOTE: at zero airspeed, the pitch increment is unbounded
	ثابت عائم pitch_increment = dt * param.vert_accel_حد / math::max(درون_داد.tas, FLT_EPSILON);
	_pitch_setpoint = constrain(pitch_setpoint, _pitch_setpoint - pitch_increment,
				    _pitch_setpoint + pitch_increment);
	_pitch_setpoint = constrain(_pitch_setpoint, param.pitch_min, param.pitch_max);

	//Debug Output
	_debug_output.energy_balance_نرخ_estimate = seb_نرخ.estimate;
	_debug_output.energy_balance_نرخ_sp = seb_نرخ.setpoint;
	_debug_output.pitch_integrator = _pitch_integ_state;
}

TECSControl::Control_مقادیر TECSControl::calc_Pitch_Control_Seb_نرخ(ثابت SpecificEnergyWeighting &وزن,
		ثابت SpecificEnergyRates &specific_energy_rates) ثابت
{
	Control_مقادیر seb_نرخ;
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
	seb_نرخ.setpoint = specific_energy_rates.spe_نرخ.setpoint * وزن.spe_weighting -
			    specific_energy_rates.ske_نرخ.setpoint *
			    وزن.ske_weighting;

	seb_نرخ.estimate = (specific_energy_rates.spe_نرخ.estimate * وزن.spe_weighting) -
			    (specific_energy_rates.ske_نرخ.estimate * وزن.ske_weighting);

	أرجع seb_نرخ;
}

فراغ TECSControl::_calcPitchControlUpdate(عائم dt, ثابت درون_داد_2 &درون_داد, ثابت Control_مقادیر &seb_نرخ,
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

		ثابت عائم climb_angle_مقصد_SEB_نرخ = climb_Angle_مقصد_Seb_نرخ(درون_داد, param);

		// Calculate pitch integrator input term
		عائم pitch_integ_درون_داد = _getControlError(seb_نرخ) * param.integrator_gain_pitch / climb_angle_مقصد_SEB_نرخ;

		// Guard against NaN integrator input - discard bad input sample (e.g., exiting offboard velocity mode)
		إذا (!PX4_ISFINITE(pitch_integ_درون_داد)) {
			pitch_integ_درون_داد = 0.f;
		}

		// Prevent the integrator changing in a direction that will increase pitch demand saturation
		إذا (_pitch_setpoint >= param.pitch_max) {
			pitch_integ_درون_داد = min(pitch_integ_درون_داد, 0.f);

		} وإلا إذا (_pitch_setpoint <= param.pitch_min) {
			pitch_integ_درون_داد = max(pitch_integ_درون_داد, 0.f);
		}

		// Update the pitch integrator state
		_pitch_integ_state = _pitch_integ_state + pitch_integ_درون_داد * dt;

		// Safety check: reset integrator if state became corrupted
		إذا (!PX4_ISFINITE(_pitch_integ_state)) {
			_pitch_integ_state = 0.f;
		}

	} وإلا {
		_pitch_integ_state = 0.0f;
	}
}

عائم TECSControl::climb_Angle_مقصد_Seb_نرخ(ثابت درون_داد_2 &درون_داد, ثابت Param &param)
{
	// max() returns its second argument for a NaN first one, so a NaN airspeed falls back to the minimum airspeed
	أرجع max(درون_داد.tas, param.tas_min, FLT_EPSILON) * CONSTANTS_ONE_G;
}

عائم TECSControl::_calcPitchControlOutput(ثابت درون_داد_2 &درون_داد, ثابت Control_مقادیر &seb_نرخ, ثابت Param &param) ثابت
{
	// Calculate a specific energy correction that doesn't include the integrator contribution
	عائم SEB_نرخ_correction = _getControlError(seb_نرخ) * param.pitch_damping_gain +
				    param.seb_نرخ_ff *
				    seb_نرخ.setpoint;

	// Convert the specific energy balance rate correction to a climb angle and add the level-flight pitch, the angle
	// of attack at zero climb angle, to get the absolute pitch setpoint. This calculation assumes:
	// a) The climb angle follows pitch angle with a lag that is small enough not to destabilise the control loop.
	// b) The offset between climb angle and pitch angle (angle of attack) is constant and equal to the level-flight
	// pitch, excluding the effect of pitch transients due to control action or turbulence.
	عائم pitch_setpoint_unc = SEB_نرخ_correction / climb_Angle_مقصد_Seb_نرخ(درون_داد, param) + _pitch_integ_state
				   + param.level_flight_pitch;

	// Guard against a non-finite feedforward/damping contribution (e.g. seb_rate going NaN on the energy/speed
	// side when exiting offboard velocity mode). constrain() does not reject NaN, so fall back to the integrator
	// state, which is kept finite by _calcPitchControlUpdate, to avoid latching a NaN pitch demand.
	إذا (!PX4_ISFINITE(pitch_setpoint_unc)) {
		pitch_setpoint_unc = _pitch_integ_state + param.level_flight_pitch;
	}

	أرجع constrain(pitch_setpoint_unc, param.pitch_min, param.pitch_max);
}

فراغ TECSControl::_calcThrottleControl(عائم dt, ثابت STERate_حد &حد_2,
				       ثابت SpecificEnergyRates &specific_energy_rates, ثابت Param &param, ثابت Flag &flag)
{
	// Update STE rate estimate LP filter
	ثابت عائم STE_نرخ_estimate_raw = specific_energy_rates.spe_نرخ.estimate + specific_energy_rates.ske_نرخ.estimate;
	ste_نرخ_estimate_filter.مجموعه_Parameters(dt, param.ste_نرخ_زمان_const);
	ste_نرخ_estimate_filter.update(STE_نرخ_estimate_raw);
	Control_مقادیر ste_نرخ{calc_Throttle_Control_Ste_نرخ(حد_2, specific_energy_rates, param)};
	// During fast descend, we control airspeed over the pitch control loop. Give minimal thrust as soon as we are descending
	عائم throttle_setpoint{param.throttle_min};

	إذا (!fastDescendFullyEngaged(param.fast_descend)) {
		_calcThrottleControlUpdate(dt, حد_2, ste_نرخ, param, flag);
		throttle_setpoint = (1.f - param.fast_descend) * _calcThrottleControlOutput(حد_2, ste_نرخ, param,
				    flag) + param.fast_descend * param.throttle_min;
	}

	// Rate limit the throttle demand
	إذا (fabsf(param.throttle_slewrate) > FLT_EPSILON) {
		ثابت عائم throttle_increment_حد = dt * (param.throttle_max - param.throttle_min) * param.throttle_slewrate;
		throttle_setpoint = constrain(throttle_setpoint, _throttle_setpoint - throttle_increment_حد,
					      _throttle_setpoint + throttle_increment_حد);
	}

	_throttle_setpoint = constrain(throttle_setpoint, param.throttle_min, param.throttle_max);

	// Debug output
	_debug_output.مجموع_energy_نرخ_estimate = ste_نرخ.estimate;
	_debug_output.مجموع_energy_نرخ_sp = ste_نرخ.setpoint;
	_debug_output.throttle_integrator = _throttle_integ_state;
}

TECSControl::Control_مقادیر TECSControl::calc_Throttle_Control_Ste_نرخ(ثابت STERate_حد &حد_2,
		ثابت SpecificEnergyRates &specific_energy_rates,
		ثابت Param &param) ثابت
{
	// Output ste rate values
	Control_مقادیر ste_نرخ;
	ste_نرخ.setpoint = specific_energy_rates.spe_نرخ.setpoint + specific_energy_rates.ske_نرخ.setpoint;

	// Adjust the demanded total energy rate to compensate for induced drag rise in turns.
	// Assume induced drag scales linearly with normal load factor.
	// The additional normal load factor is given by (1/cos(bank angle) - 1)
	ste_نرخ.setpoint += param.بارگیری_ضریب_correction * (param.بارگیری_ضریب - 1.f);

	ste_نرخ.setpoint = constrain(ste_نرخ.setpoint, حد_2.STE_نرخ_min, حد_2.STE_نرخ_max);
	ste_نرخ.estimate = ste_نرخ_estimate_filter.getState();

	أرجع ste_نرخ;
}

فراغ TECSControl::_calcThrottleControlUpdate(عائم dt, ثابت STERate_حد &حد_2, ثابت Control_مقادیر &ste_نرخ,
		ثابت Param &param, ثابت Flag &flag)
{
	ثابت عائم STE_نرخ_مقصد_throttle = ste_نرخ_مقصد_Throttle(حد_2);

	// Integral handling
	إذا (flag.airspeed_enabled) {
		إذا (param.integrator_gain_throttle > FLT_EPSILON) {
			// underspeed conditions zero out integration
			عائم throttle_integ_درون_داد = (_getControlError(ste_نرخ) * param.integrator_gain_throttle) * dt *
						     STE_نرخ_مقصد_throttle * (1.0f - نسبت_undersped);

			// only allow integrator propagation into direction which unsaturates throttle
			إذا (_throttle_setpoint >= param.throttle_max) {
				throttle_integ_درون_داد = math::min(0.f, throttle_integ_درون_داد);

			}

			إذا (_throttle_setpoint <= param.throttle_min) {
				throttle_integ_درون_داد = math::max(0.f, throttle_integ_درون_داد);
			}

			// Calculate a throttle demand from the integrated total energy rate error
			// This will be added to the total throttle demand to compensate for steady state errors
			_throttle_integ_state = PX4_ISFINITE(throttle_integ_درون_داد) ? _throttle_integ_state + throttle_integ_درون_داد : 0.f;

		} وإلا {
			_throttle_integ_state = 0.0f;
		}

	}
}

عائم TECSControl::_calcThrottleControlOutput(ثابت STERate_حد &حد_2, ثابت Control_مقادیر &ste_نرخ,
		ثابت Param &param,
		ثابت Flag &flag) ثابت
{
	ثابت عائم STE_نرخ_مقصد_throttle = ste_نرخ_مقصد_Throttle(حد_2);

	// Calculate a predicted throttle from the demanded rate of change of energy, using the cruise throttle
	// as the starting point. Assume:
	// Specific total energy rate = _STE_rate_max is achieved when throttle is set to _throttle_setpoint_max
	// Specific total energy rate = 0 at cruise throttle
	// Specific total energy rate = _STE_rate_min is achieved when throttle is set to _throttle_setpoint_min

	// assume airspeed and density-independent delta_throttle to sink/climb rate mapping
	// TODO: include air density for thrust mappings
	ثابت عائم throttle_above_trim_در_هر_ste_نرخ = (param.throttle_max - param.throttle_trim) / حد_2.STE_نرخ_max;
	ثابت عائم throttle_زیر_trim_در_هر_ste_نرخ = (param.throttle_trim - param.throttle_min) / حد_2.STE_نرخ_min;

	عائم throttle_predicted = 0.0f;

	إذا (ste_نرخ.setpoint >= FLT_EPSILON) {
		// throttle is between trim and maximum
		throttle_predicted = param.throttle_trim + ste_نرخ.setpoint * throttle_above_trim_در_هر_ste_نرخ;

	} وإلا {
		// throttle is between trim and minimum
		throttle_predicted = param.throttle_trim - ste_نرخ.setpoint * throttle_زیر_trim_در_هر_ste_نرخ;

	}

	// Add proportional and derivative control feedback to the predicted throttle and constrain to throttle limits
	عائم throttle_setpoint = (_getControlError(ste_نرخ) * param.throttle_damping_gain) * STE_نرخ_مقصد_throttle +
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
	throttle_setpoint = نسبت_undersped * param.throttle_max + (1.0f - نسبت_undersped) * throttle_setpoint;

	أرجع constrain(throttle_setpoint, param.throttle_min, param.throttle_max);
}

فراغ TECSControl::resetIntegrals()
{
	_pitch_integ_state = 0.0f;
	_throttle_integ_state = 0.0f;
}

فراغ TECS::initControlParams(ثابت درون_داد_2 &درون_داد)
{
	// Update parameters from input
	// Reference model
	_reference_param.هدف_climbrate = درون_داد.هدف_climbrate;
	_reference_param.هدف_sinkrate = درون_داد.هدف_sinkrate;
	// Control
	_control_param.tas_min = درون_داد.eas_مقصد_tas * _equivalent_airspeed_min;
	_control_param.tas_stall = درون_داد.eas_مقصد_tas * _equivalent_airspeed_stall;
	_control_param.tas_max = درون_داد.eas_مقصد_tas * _equivalent_airspeed_max;
	_control_param.tas_trim = درون_داد.eas_مقصد_tas * _control_param.equivalent_airspeed_trim;
	_control_param.pitch_max = درون_داد.pitch_max;
	_control_param.pitch_min = درون_داد.pitch_min;
	_control_param.throttle_trim = درون_داد.throttle_trim;
	_control_param.throttle_max = درون_داد.throttle_max;
	_control_param.throttle_min = درون_داد.throttle_min;
}

عائم TECS::calcTrueAirspeedSetpoint(عائم eas_مقصد_tas, عائم eas_setpoint)
{
	أرجع lerp(eas_مقصد_tas * eas_setpoint, _control_param.tas_max, _fast_descend);
}

فراغ TECS::initialize(ثابت درون_داد_2 &درون_داد)
{
	// Init subclasses
	TECSAltitudeReferenceModel::AltitudeReferenceState current_state{.alt = درون_داد.altitude,
			.alt_نرخ = درون_داد.altitude_نرخ};
	_altitude_reference_model.initialize(current_state);
	_airspeed_filter.initialize(درون_داد.equivalent_airspeed, _airspeed_filter_param.equivalent_airspeed_trim,
				    _control_flag.airspeed_enabled);

	// Seed the controller from the filter, which falls back to trim airspeed without a valid measurement,
	// so the controller sees the same airspeed on reset as on every following update
	ثابت عائم true_airspeed = درون_داد.eas_مقصد_tas * _airspeed_filter.getState().سرعت;

	TECSControl::Setpoint control_setpoint;
	control_setpoint.altitude_reference = _altitude_reference_model.getAltitudeReference();
	control_setpoint.altitude_نرخ_setpoint_مستقیم = NAN; // initialization always goes through the altitude loop
	control_setpoint.tas_setpoint = true_airspeed;

	ثابت TECSControl::درون_داد_2 control_درون_داد{ .altitude = درون_داد.altitude,
						.altitude_نرخ = درون_داد.altitude_نرخ,
						.tas = true_airspeed,
						.tas_نرخ = 0.0f};

	_control.initialize(control_setpoint, control_درون_داد, _control_param, _control_flag);

	_fast_descend = 0.f;
	_enabled_fast_descend_timestamp = 0U;
}

فراغ TECS::update(ثابت درون_داد_2 &درون_داد)
{
	// Calculate the time since last update (seconds)
	ثابت hrt_abstime now = hrt_absolute_زمان();
	ثابت عائم dt = static_cast<عائم>(now - _update_timestamp) * 1e-6f;

	initControlParams(درون_داد);

	إذا (dt < DT_MIN) {
		// Update interval too small, do not update. Assume constant states/output in this case.
		أرجع;
	}

	عائم true_airspeed_setpoint;

	إذا (dt > DT_MAX || _update_timestamp == 0UL) {
		// Update time interval too large, can't guarantee sanity of state updates anymore. reset the control loop.
		initialize(درون_داد);
		true_airspeed_setpoint = calcTrueAirspeedSetpoint(درون_داد.eas_مقصد_tas, درون_داد.equivalent_airspeed_setpoint);

	} وإلا {
		/* Check if we want to fast descend. On fast descend, we set the throttle to min, and use the altitude control
		loop to control the speed to the maximum airspeed. */
		مجموعه_Fast_Descend(درون_داد.altitude_setpoint, درون_داد.altitude);
		_control_param.fast_descend = _fast_descend;

		// Update airspeedfilter submodule
		ثابت TECSAirspeedFilter::درون_داد_2 airspeed_درون_داد{ .equivalent_airspeed = درون_داد.equivalent_airspeed,
				.equivalent_airspeed_نرخ = درون_داد.سرعت_deriv_forward / درون_داد.eas_مقصد_tas};

		_airspeed_filter.update(dt, airspeed_درون_داد, _airspeed_filter_param, _control_flag.airspeed_enabled);

		// Update Reference model submodule
		إذا (fastDescendFullyEngaged(_fast_descend)) {
			// Reset the altitude reference model, while we are in fast descend.
			ثابت TECSAltitudeReferenceModel::AltitudeReferenceState init_state{
				.alt = درون_داد.altitude,
				.alt_نرخ = درون_داد.altitude_نرخ};
			_altitude_reference_model.initialize(init_state);

		} وإلا إذا (_control.get_Underspeed_نسبت() > 0.5f) {
			// Reset altitude reference during underspeed to prevent altitude error accumulation
			// that would cause aggressive pitch-up (and potential secondary stall) on recovery.
			ثابت TECSAltitudeReferenceModel::AltitudeReferenceState init_state{
				.alt = درون_داد.altitude,
				.alt_نرخ = درون_داد.altitude_نرخ};
			_altitude_reference_model.initialize(init_state);

		} وإلا {
			ثابت TECSAltitudeReferenceModel::AltitudeReferenceState setpoint{ .alt = درون_داد.altitude_setpoint,
					.alt_نرخ = درون_داد.altitude_نرخ_setpoint};

			_altitude_reference_model.update(dt, setpoint, درون_داد.altitude, درون_داد.altitude_نرخ, _reference_param);
		}

		TECSControl::Setpoint control_setpoint;
		control_setpoint.altitude_reference = _altitude_reference_model.getAltitudeReference();
		control_setpoint.altitude_نرخ_setpoint_مستقیم = _altitude_reference_model.get_ارتفاع_نرخ_Setpoint_مستقیم();
		true_airspeed_setpoint = calcTrueAirspeedSetpoint(درون_داد.eas_مقصد_tas, درون_داد.equivalent_airspeed_setpoint);
		control_setpoint.tas_setpoint = true_airspeed_setpoint;

		ثابت TECSAirspeedFilter::AirspeedFilterState airspeed_state{_airspeed_filter.getState()};
		ثابت TECSControl::درون_داد_2 control_درون_داد{ .altitude = درون_داد.altitude,
							.altitude_نرخ = درون_داد.altitude_نرخ,
							.tas = درون_داد.eas_مقصد_tas * airspeed_state.سرعت,
							.tas_نرخ = درون_داد.eas_مقصد_tas * airspeed_state.سرعت_نرخ};

		_control.update(dt, control_setpoint, control_درون_داد, _control_param, _control_flag);
	}

	debug_وضعیت.control = _control.getDebugOutput();
	debug_وضعیت.true_airspeed_sp = true_airspeed_setpoint;
	ثابت TECSAirspeedFilter::AirspeedFilterState airspeed_state{_airspeed_filter.getState()};
	ثابت TECSAltitudeReferenceModel::AltitudeReferenceState altitude_reference{_altitude_reference_model.getAltitudeReference()};
	debug_وضعیت.true_airspeed_filtered = درون_داد.eas_مقصد_tas * airspeed_state.سرعت;
	debug_وضعیت.true_airspeed_derivative = درون_داد.eas_مقصد_tas * airspeed_state.سرعت_نرخ;
	debug_وضعیت.altitude_reference = altitude_reference.alt;
	debug_وضعیت.ارتفاع_نرخ_reference = altitude_reference.alt_نرخ;
	debug_وضعیت.ارتفاع_نرخ_مستقیم = _altitude_reference_model.get_ارتفاع_نرخ_Setpoint_مستقیم();
	debug_وضعیت.fast_descend = _fast_descend;

	_update_timestamp = now;
}

فراغ TECS::مجموعه_Fast_Descend(ثابت عائم alt_setpoint, ثابت عائم alt)
{
	// disable fast descend if we are close to the target altitude or the altitude setpoint is not finite

	إذا (PX4_ISFINITE(alt_setpoint) && _control_flag.airspeed_enabled && (_fast_descend_alt_err > FLT_EPSILON)
	    && ((alt_setpoint + _fast_descend_alt_err) < alt)) {
		خودکار now = hrt_absolute_زمان();

		إذا (_enabled_fast_descend_timestamp == 0U) {
			_enabled_fast_descend_timestamp = now;
		}

		_fast_descend = constrain(max(_fast_descend, static_cast<عائم>(now - _enabled_fast_descend_timestamp) /
					      static_cast<عائم>(FAST_DESCEND_RAMP_به_بالا_زمان)), 0.f, 1.f);

	} وإلا إذا (PX4_ISFINITE(alt_setpoint) && (_fast_descend > FLT_EPSILON) && (_fast_descend_alt_err > FLT_EPSILON)) {
		// We're in fast descend, scale it down until FAST_DESCEND_HEIGHT_MARGIN above target altitude
		_fast_descend = constrain((alt - alt_setpoint - FAST_DESCEND_ارتفاع_MARGIN) / _fast_descend_alt_err, 0.f, 1.f);
		_enabled_fast_descend_timestamp = 0U;

	} وإلا {
		_fast_descend = 0.f;
		_enabled_fast_descend_timestamp = 0U;
	}
}

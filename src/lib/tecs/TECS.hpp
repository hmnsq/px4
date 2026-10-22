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
 * @file TECS.hpp
 *
 * @author Paul Riseborough
 */

#pragma once

#અન્તર્ભાવ <drivers/drv_hrt.h>
#અન્તર્ભાવ <mathlib/mathlib.h>
#અન્તર્ભાવ <matrix/math.hpp>
#અન્તર્ભાવ <lib/mathlib/math/filter/AlphaFilter.hpp>
#અન્તર્ભાવ <motion_planning/VelocitySmoothing.hpp>
#અન્તર્ભાવ <motion_planning/ManualVelocitySmoothingZ.hpp>

using નામાકાશ time_literals;

#ifndef FRIEND_TEST // for gtest
#define FRIEND_TEST(a, b)
#endif

વર્ગ TECSAirspeedFilter
{
જાહેર:
	/**
	 * @brief State of the equivalent airspeed filter.
	 *
	 */
	સંરચના AirspeedFilterState {
		દશાંશ speed;		///< speed of the air in EAS [m/s]
		દશાંશ speed_rate;	///< rate of speed of the air [m/s²]
	};

	/**
	 * @brief Parameters of the airspeed filter.
	 *
	 */
	સંરચના Param {
		દશાંશ equivalent_airspeed_trim; 	///< the trim value of the equivalent airspeed [m/s].
	};

	/**
	 * @brief Input, which will be filtered.
	 *
	 */
	સંરચના આગત_2 {
		દશાંશ equivalent_airspeed; 	///< the measured equivalent airspeed in [m/s].
		દશાંશ equivalent_airspeed_rate;	///< the measured rate of equivalent airspeed in [m/s²].
	};
જાહેર:
	TECSAirspeedFilter() = મૂળભૂત;
	~TECSAirspeedFilter() = મૂળભૂત;
	/**
	 * @brief Initialize filter
	 *
	 * @param[in] equivalent_airspeed is the equivalent airspeed in [m/s].
	 * @param[in] equivalent_airspeed_trim is the equivalent airspeed trim (vehicle setting) in [m/s].
	 * @param[in] airspeed_sensor_available boolean if the airspeed sensor is available.
	 */
	શૂન્ય initialize(દશાંશ equivalent_airspeed, અચલ દશાંશ equivalent_airspeed_trim,
			અચલ તાર્કિક airspeed_sensor_available);

	/**
	 * @brief Update filter
	 *
	 * @param[in] dt is the timestep in [s].
	 * @param[in] input are the raw measured values.
	 * @param[in] param are the filter parameters.
	 * @param[in] airspeed_sensor_available boolean if the airspeed sensor is available.
	 */
	શૂન્ય update(દશાંશ dt, અચલ આગત_2 &આગત, અચલ Param &param, અચલ તાર્કિક airspeed_sensor_available);

	/**
	 * @brief Get the filtered airspeed states.
	 *
	 * @return Current state of the airspeed filter.
	 */
	AirspeedFilterState getState() અચલ;

ખાનગી:
	// States
	AirspeedFilterState _airspeed_state{.speed = 0.0f, .speed_rate = 0.0f};	///< Complementary filter state

	// Steady-state Kalman gains for the 2-state (airspeed, airspeed_rate) complementary filter.
	//
	// Noise model: R = diag(σ_meas², σ_rate_meas²),  Q = diag(0, σ_proc²)
	//
	// Defining:  a = 1/σ_meas,  b = σ_proc/σ_rate_meas²,  d = a + b,  n = sqrt(σ_proc·(2a + b))
	//
	//   K = | a·n/d        b/d       |
	//       | a²·σ_proc/d  b·n/d     |
	//
	// With σ_meas=0.07 m/s, σ_rate_meas=0.2 m/s², σ_proc=0.2 m/s²:
	//   a=14.2857, b=5, d=19.2857, n=2.5912
	//
	// To recompute if any noise value changes:
	//   python3 -c "import math; sm=0.07; sr=0.2; sp=0.2; a=1/sm; b=sp/sr**2; d=a+b; n=math.sqrt(sp*(2*a+b)); print(a*n/d, b/d, a*a*sp/d, b*n/d)"
	સ્થિર constexpr દશાંશ kKg00{1.91940287f};
	સ્થિર constexpr દશાંશ kKg01{0.25925926f};
	સ્થિર constexpr દશાંશ kKg10{2.11640212f};
	સ્થિર constexpr દશાંશ kKg11{0.67179101f};
};

વર્ગ TECSAltitudeReferenceModel
{
જાહેર:
	/**
	 * @brief Altitude reference state.
	 *
	 */
	સંરચના AltitudeReferenceState {
		દશાંશ alt; 	///< Reference altitude amsl in [m].
		દશાંશ alt_rate;	///< Reference altitude rate in [m/s].
	};

	/**
	 * @brief Parameters for the reference model.
	 *
	 */
	સંરચના Param {
		દશાંશ target_climbrate;	///< The target climbrate in [m/s].
		દશાંશ target_sinkrate;	///< The target sinkrate in [m/s].
		દશાંશ vert_accel_limit;	///< Magnitude of the maximum vertical acceleration allowed [m/s²].
		દશાંશ max_climb_rate;	///< Climb rate produced by max allowed throttle [m/s].
		દશાંશ max_sink_rate;	///< Maximum sink rate (with min throttle, max speed) [m/s].
	};

જાહેર:
	TECSAltitudeReferenceModel() = મૂળભૂત;
	~TECSAltitudeReferenceModel() = મૂળભૂત;

	/**
	 * @brief Initialize reference models.
	 *
	 * @param[in] state is the current altitude state of the vehicle.
	 */
	શૂન્ય initialize(અચલ AltitudeReferenceState &state);

	/**
	 * @brief Update reference models.
	 *
	 * @param[in] dt is the update interval in [s].
	 * @param[in] setpoint are the desired setpoints.
	 * @param[in] altitude is the altitude amsl in [m].
	 * @param[in] height_rate is the current height rate in [m/s].
	 * @param[in] param are the reference model parameters.
	 */
	શૂન્ય update(દશાંશ dt, અચલ AltitudeReferenceState &setpoint, દશાંશ altitude, દશાંશ height_rate, અચલ Param &param);

	/**
	 * @brief Get the current altitude reference of altitude reference model.
	 *
	 * @return Altitude reference state.
	 */
	AltitudeReferenceState getAltitudeReference() અચલ;

	/**
	 * @brief Get the Height Rate Setpoint directly from the velocity trajectory generator
	 *
	 * @return float direct height rate setpoint [m/s]
	 */
	દશાંશ getHeightRateSetpointDirect() અચલ {પાછા _height_rate_setpoint_direct; }


ખાનગી:
	સ્થિર constexpr દશાંશ JERK_MAX = 1000.f;	///< Magnitude of the maximum jerk allowed [m/s³].

	// State
	VelocitySmoothing
	_alt_control_traj_generator;		///< Generates altitude rate and altitude setpoint trajectory when altitude is commanded.
	ManualVelocitySmoothingZ
	_velocity_control_traj_generator;	///< generates height rate trajectory when height rate is commanded
	દશાંશ _height_rate_setpoint_direct{NAN}; ///< generated direct height rate setpoint
};

વર્ગ TECSControl
{
જાહેર:
	/**
	 * @brief The control parameters.
	 *
	 */
	સંરચના Param {
		// Vehicle specific params
		દશાંશ max_sink_rate;			///< Maximum sink rate (with min throttle and max speed) [m/s].
		દશાંશ min_sink_rate;			///< Minimum sink rate (with min throttle and trim speed) [m/s].
		દશાંશ max_climb_rate;			///< Climb rate produced by max allowed throttle [m/s].
		દશાંશ vert_accel_limit;			///< Magnitude of the maximum vertical acceleration allowed [m/s²].
		દશાંશ equivalent_airspeed_trim;		///< Equivalent cruise airspeed for airspeed less mode [m/s].
		દશાંશ tas_min;				///< True airspeed demand lower limit [m/s].
		દશાંશ tas_stall;			///< True stall airspeed [m/s].
		દશાંશ tas_max;				///< True airspeed demand upper limit [m/s].
		દશાંશ tas_trim;				///< True airspeed trim [m/s].
		દશાંશ pitch_max;			///< Maximum absolute pitch angle setpoint [rad].
		દશાંશ pitch_min;			///< Minimum absolute pitch angle setpoint [rad].
		દશાંશ level_flight_pitch;		///< Pitch angle in level flight at trim airspeed, fed forward at zero climb angle [rad].
		દશાંશ throttle_trim;		///< Normalized throttle required to fly level at calibrated airspeed setpoint [0,1]
		દશાંશ throttle_max;			///< Normalized throttle upper limit.
		દશાંશ throttle_min;			///< Normalized throttle lower limit.

		// Altitude control param
		દશાંશ altitude_error_gain;		///< Altitude error inverse time constant [1/s].
		દશાંશ altitude_setpoint_gain_ff;	///< Gain from altitude demand derivative to demanded climb rate.

		// Airspeed control param
		દશાંશ airspeed_error_gain;				///< Airspeed error inverse time constant [1/s].

		// Energy control param
		દશાંશ ste_rate_time_const;				///< Filter time constant for specific total energy rate (damping path) [s].
		દશાંશ seb_rate_ff;					///< Specific energy balance rate feedforward gain.

		// Pitch control param
		દશાંશ pitch_speed_weight;				///< Speed control weighting used by pitch demand calculation.
		દશાંશ integrator_gain_pitch;				///< Integrator gain used by the pitch demand calculation.
		દશાંશ pitch_damping_gain;				///< Damping gain of the pitch demand calculation [s].

		// Throttle control param
		દશાંશ integrator_gain_throttle;				///< Integrator gain used by the throttle demand calculation.
		દશાંશ throttle_damping_gain;				///< Damping gain of the throttle demand calculation [s].
		દશાંશ throttle_slewrate;				///< Throttle demand slew rate limit [1/s].

		દશાંશ load_factor_correction;				///< Gain from normal load factor increase to total energy rate demand [m²/s³].
		દશાંશ load_factor;					///< Additional normal load factor.

		દશાંશ fast_descend;
	};

	/**
	 * @brief The debug output
	 *
	 */
	સંરચના DebugOutput {
		દશાંશ altitude_rate_control;			///< Altitude rate setpoint from altitude control loop [m/s].
		દશાંશ true_airspeed_derivative_control;		///< Airspeed rate setpoint from airspeed control loop [m/s²].
		દશાંશ total_energy_rate_estimate;		///< Total energy rate estimate [m²/s³].
		દશાંશ total_energy_rate_sp;			///< Total energy rate setpoint [m²/s³].
		દશાંશ energy_balance_rate_estimate;		///< Energy balance rate estimate [m²/s³].
		દશાંશ energy_balance_rate_sp;			///< Energy balance rate setpoint [m²/s³].
		દશાંશ pitch_integrator;				///< Pitch control integrator state, a correction added to the pitch setpoint [rad].
		દશાંશ throttle_integrator;			///< Throttle control integrator state [-].
	};

	/**
	 * @brief Given setpoint to control.
	 *
	 */
	સંરચના Setpoint {
		TECSAltitudeReferenceModel::AltitudeReferenceState altitude_reference;	///< Altitude/height rate reference.
		દશાંશ altitude_rate_setpoint_direct;					///< Direct height rate setpoint.
		દશાંશ tas_setpoint;						///< True airspeed setpoint.
	};

	/**
	 * @brief Given current measurement from the UAS.
	 *
	 */
	સંરચના આગત_2 {
		દશાંશ altitude;		///< Current altitude amsl of the UAS [m].
		દશાંશ altitude_rate;	///< Current altitude rate of the UAS [m/s].
		દશાંશ tas;		///< Current true airspeed of the UAS [m/s].
		દશાંશ tas_rate;		///< Current true airspeed rate of the UAS [m/s²].
	};

	/**
	 * @brief Control flags.
	 *
	 */
	સંરચના Flag {
		તાર્કિક airspeed_enabled;			///< Flag if the airspeed sensor is enabled.
		તાર્કિક detect_underspeed_enabled;		///< Flag if underspeed detection is enabled.
	};
જાહેર:
	TECSControl() = મૂળભૂત;
	~TECSControl() = મૂળભૂત;
	/**
	 * @brief Initialization of the state.
	 *
	 * @param[in] setpoint is the current setpoint struct.
	 * @param[in] input is the current input measurements.
	 * @param[in] param is the current parameter set.
	 * @param[in] flag is the current activated flags.
	 */
	શૂન્ય initialize(અચલ Setpoint &setpoint, અચલ આગત_2 &આગત, અચલ Param &param, અચલ Flag &flag);
	/**
	 * @brief Update state and output.
	 *
	 * @param[in] dt is the update time interval in [s].
	 * @param[in] setpoint is the current setpoint struct.
	 * @param[in] input is the current input measurements.
	 * @param[in] param is the current parameter set.
	 * @param[in] flag is the current activated flags.
	 */
	શૂન્ય update(દશાંશ dt, અચલ Setpoint &setpoint, અચલ આગત_2 &આગત, અચલ Param &param, અચલ Flag &flag);
	/**
	 * @brief Reset the control loop integrals.
	 *
	 */
	શૂન્ય resetIntegrals();
	/**
	 * @brief Get the underspeed ratio.
	 *
	 * @return Ratio of detected underspeed [0,1].
	 */
	દશાંશ getUnderspeedRatio() અચલ {પાછા _ratio_undersped;}
	/**
	 * @brief Get the throttle setpoint.
	 *
	 * @return throttle setpoint.
	 */
	દશાંશ getThrottleSetpoint() અચલ {પાછા _throttle_setpoint;}
	/**
	 * @brief Get the pitch setpoint.
	 *
	 * @return The commanded absolute pitch angle [rad].
	 */
	દશાંશ getPitchSetpoint() અચલ {પાછા _pitch_setpoint;}
	/**
	 * @brief Get the Debug Output
	 *
	 * @return the debug output struct.
	 */
	અચલ DebugOutput &getDebugOutput() અચલ { પાછા _debug_output; }

ખાનગી:
	// Allows the regression test to pre-corrupt _pitch_integ_state and verify the
	// safety reset in _calcPitchControlUpdate, which the public update() path cannot
	// reach on its own because the integrator-input guard rejects bad input first.
	FRIEND_TEST(TECSControlTest, PitchIntegratorResetsCorruptedState);

	/**
	 * @brief Specific total energy rate limit.
	 *
	 */
	સંરચના STERateLimit {
		દશાંશ STE_rate_max;	///< Maximum specific total energy rate limit [m²/s³].
		દશાંશ STE_rate_min;	///< Minimal specific total energy rate limit [m²/s³].
	};

	/**
	 * @brief Control values.
	 * setpoint and current state estimate value as input to a controller.
	 *
	 */
	સંરચના Control_કિંમતો {
		દશાંશ setpoint;		///< Control setpoint.
		દશાંશ estimate;		///< Control estimate of current state value.
	};

	/**
	 * @brief Calculated specific energy rates.
	 *
	 */
	સંરચના SpecificEnergyRates {
		Control_કિંમતો ske_rate;	///< Specific kinetic energy rate [m²/s³].
		Control_કિંમતો spe_rate;	///< Specific potential energy rate [m²/s³].
	};

	/**
	 * @brief Altitude rate and airspeed setpoints from the outer loops, from which the energy rate setpoints follow.
	 *
	 */
	સંરચના OuterLoopSetpoints {
		દશાંશ altitude_rate_setpoint;	///< Controlled altitude rate setpoint [m/s].
		દશાંશ tas_rate_setpoint;	///< Controlled true airspeed rate setpoint [m/s²].
	};

	/**
	 * @brief Weight factors for specific energy.
	 *
	 */
	સંરચના SpecificEnergyWeighting {
		દશાંશ spe_weighting;	///< Specific potential energy weight.
		દશાંશ ske_weighting;	///< Specific kinetic energy weight.
	};

ખાનગી:
	/**
	 * @brief Get control error from setpoint and estimate
	 *
	 * @param val is the current control setpoint and estimate.
	 * @return error value
	 */
	સ્થિર constexpr દશાંશ _getControlError(TECSControl::Control_કિંમતો val) {પાછા (val.setpoint - val.estimate);}
	/**
	 * @brief Calculate specific total energy rate limits.
	 *
	 * @param[in] param are the control parameters.
	 * @return Specific total energy rate limits in [m²/s³].
	 */
	STERateLimit _calculateTotalEnergyRateLimit(અચલ Param &param) અચલ;
	/**
	 * @brief Gain from specific total energy rate error to throttle.
	 *
	 * @param[in] limit is the specific total energy rate limits in [m²/s³].
	 * @return throttle per specific total energy rate [s³/m²].
	 */
	સ્થિર દશાંશ _steRateToThrottle(અચલ STERateLimit &limit) { પાછા 1.0f / (limit.STE_rate_max - limit.STE_rate_min); }
	/**
	 * @brief calculate airspeed control proportional output.
	 *
	 * @param setpoint is the control setpoints.
	 * @param input	is the current input measurement of the UAS.
	 * @param limit is the specific total energy rate limits in [m²/s³].
	 * @param param	is the control parameters.
	 * @param flag	is the control flags.
	 * @return controlled airspeed rate setpoint in [m/s²].
	 */
	દશાંશ _calcAirspeedControlOutput(અચલ Setpoint &setpoint, અચલ આગત_2 &આગત, અચલ STERateLimit &limit,
					 અચલ Param &param, અચલ Flag &flag) અચલ;
	/**
	 * @brief calculate altitude control proportional output.
	 *
	 * @param setpoint is the control setpoints.
	 * @param input is the current input measurement of the UAS.
	 * @param param is the control parameters.
	 * @return controlled altitude rate setpoint in [m/s].
	 */
	દશાંશ _calcAltitudeControlOutput(અચલ Setpoint &setpoint, અચલ આગત_2 &આગત, અચલ Param &param) અચલ;
	/**
	 * @brief Run the airspeed and altitude outer loops and the underspeed detection.
	 *
	 * Uses the direct height rate setpoint if it is finite, the altitude loop otherwise.
	 *
	 * @param setpoint is the control setpoints.
	 * @param input is the current input measurement of the UAS.
	 * @param limit is the specific total energy rate limits in [m²/s³].
	 * @param param is the control parameters.
	 * @param flag is the control flags.
	 * @return Specific energy rates in [m²/s³].
	 */
	SpecificEnergyRates _updateOuterLoops(અચલ Setpoint &setpoint, અચલ આગત_2 &આગત, અચલ STERateLimit &limit,
					      અચલ Param &param, અચલ Flag &flag);
	/**
	 * @brief Calculate specific energy rates.
	 *
	 * @param control_setpoint is the controlled altitude and airspeed rate setpoints.
	 * @param input is the current input measurement of the UAS.
	 * @return Specific energy rates in [m²/s³].
	 */
	SpecificEnergyRates _calcSpecificEnergyRates(અચલ OuterLoopSetpoints &control_setpoint, અચલ આગત_2 &આગત) અચલ;
	/**
	 * @brief Detect underspeed.
	 *
	 * @param input is the current input measurement of the UAS.
	 * @param param is the control parameters.
	 * @param flag is the control flags.
	 */
	શૂન્ય _detectUnderspeed(અચલ આગત_2 &આગત, અચલ Param &param, અચલ Flag &flag);
	/**
	 * @brief Update specific energy balance weights.
	 *
	 * @param param is the control parameters.
	 * @param flag is the control flags.
	 * @return Weights used for the specific energy balance.
	 */
	SpecificEnergyWeighting _updateSpeedAltitudeWeights(અચલ Param &param, અચલ Flag &flag);
	/**
	 * @brief Calculate the specific kinetic energy rate setpoint used by the pitch loop.
	 *
	 * @param setpoint is the control setpoints.
	 * @param input is the current input measurement of the UAS.
	 * @param limit is the specific total energy rate limits in [m²/s³].
	 * @param param is the control parameters.
	 * @param flag is the control flags.
	 * @param weight is the weighting use of the potential and kinetic energy.
	 * @return specific kinetic energy rate setpoint in [m²/s³].
	 */
	દશાંશ _calcPitchControlSkeRateSetpoint(અચલ Setpoint &setpoint, અચલ આગત_2 &આગત, અચલ STERateLimit &limit,
					       અચલ Param &param, અચલ Flag &flag, અચલ SpecificEnergyWeighting &weight) અચલ;
	/**
	 * @brief Specific energy rates as seen by the pitch loop.
	 *
	 * The airspeed setpoint is capped as in _calcPitchControlSkeRateSetpoint, and between trim airspeed and the
	 * airspeed setpoint the kinetic energy rate estimate is removed as well, so that pitch does not counter the
	 * throttle's acceleration.
	 *
	 * @param setpoint is the control setpoints.
	 * @param input is the current input measurement of the UAS.
	 * @param limit is the specific total energy rate limits in [m²/s³].
	 * @param specific_energy_rates is the specific energy rates of the outer loops in [m²/s³].
	 * @param param is the control parameters.
	 * @param flag is the control flags.
	 * @param weight is the weighting use of the potential and kinetic energy.
	 * @return specific energy rates for the pitch loop in [m²/s³].
	 */
	SpecificEnergyRates _calcPitchControlEnergyRates(અચલ Setpoint &setpoint, અચલ આગત_2 &આગત,
			અચલ STERateLimit &limit, અચલ SpecificEnergyRates &specific_energy_rates, અચલ Param &param,
			અચલ Flag &flag, અચલ SpecificEnergyWeighting &weight) અચલ;
	/**
	 * @brief Calculate pitch control.
	 *
	 * @param dt is the update time interval in [s].
	 * @param setpoint is the control setpoints.
	 * @param input is the current input measurement of the UAS.
	 * @param limit is the specific total energy rate limits in [m²/s³].
	 * @param specific_energy_rate is the calculated specific energy.
	 * @param param is the control parameters.
	 * @param flag is the control flags.
	 */
	શૂન્ય _calcPitchControl(દશાંશ dt, અચલ Setpoint &setpoint, અચલ આગત_2 &આગત, અચલ STERateLimit &limit,
			       અચલ SpecificEnergyRates &specific_energy_rate, અચલ Param &param, અચલ Flag &flag);

	/**
	 * @brief Calculate pitch control specific energy balance rates.
	 *
	 * @param weight is the weighting use of the potential and kinetic energy.
	 * @param specific_energy_rate is the specific energy rates in [m²/s³].
	 * @return specific energy balance rate values in [m²/s³].
	 */
	Control_કિંમતો _calcPitchControlSebRate(અચલ SpecificEnergyWeighting &weight,
					       અચલ SpecificEnergyRates &specific_energy_rate) અચલ;

	/**
	 * @brief Calculate the pitch control update function.
	 * Update the states of the pitch control (pitch integrator).
	 *
	 * @param dt is the update time interval in [s].
	 * @param input is the current input measurement of the UAS.
	 * @param seb_rate is the specific energy balance rate in [m²/s³].
	 * @param param is the control parameters.
	 */
	શૂન્ય _calcPitchControlUpdate(દશાંશ dt, અચલ આગત_2 &આગત, અચલ Control_કિંમતો &seb_rate, અચલ Param &param);

	/**
	 * @brief Derivative of the specific energy balance rate with respect to the climb angle.
	 *
	 * Uses the filtered true airspeed, which is trim airspeed without a valid measurement, floored at the minimum
	 * airspeed so the conversion stays well conditioned at low or invalid airspeed.
	 *
	 * @param input is the current input measurement of the UAS.
	 * @param param is the control parameters.
	 * @return climb angle to specific energy balance rate gain in [m²/s³/rad].
	 */
	સ્થિર દશાંશ _climbAngleToSebRate(અચલ આગત_2 &આગત, અચલ Param &param);

	/**
	 * @brief Calculate the pitch control output function.
	 *
	 * @param input is the current input measurement of the UAS.
	 * @param seb_rate is the specific energy balance rate in [m²/s³].
	 * @param param is the control parameters.
	 * @return absolute pitch setpoint [rad].
	 */
	દશાંશ _calcPitchControlOutput(અચલ આગત_2 &આગત, અચલ Control_કિંમતો &seb_rate, અચલ Param &param) અચલ;

	/**
	 * @brief Update controlled throttle setpoint.
	 *
	 * @param dt is the update time interval in [s].
	 * @param limit is the specific total energy rate limits in [m²/s³].
	 * @param specific_energy_rate is the calculated specific energy.
	 * @param param is the control parameters.
	 * @param flag is the control flags.
	 */
	શૂન્ય _calcThrottleControl(દશાંશ dt, અચલ STERateLimit &limit, અચલ SpecificEnergyRates &specific_energy_rate,
				  અચલ Param &param, અચલ Flag &flag);

	/**
	 * @brief Calculate throttle control specific total energy
	 *
	 * @param limit is the specific total energy rate limits in [m²/s³].
	 * @param specific_energy_rate is the specific energy rates in [m²/s³].
	 * @param param is the control parameters.
	 * @return specific total energy rate values in [m²/s³]
	 */
	Control_કિંમતો _calcThrottleControlSteRate(અચલ STERateLimit &limit, અચલ SpecificEnergyRates &specific_energy_rate,
			અચલ Param &param) અચલ;

	/**
	 * @brief Calculate the throttle control update function.
	 * Update the throttle control states (throttle integrator).
	 *
	 * @param dt is the update time interval in [s].
	 * @param limit is the specific total energy rate limits in [m²/s³].
	 * @param ste_rate is the specific total energy rates in [m²/s³].
	 * @param param is the control parameters.
	 * @param flag is the control flags.
	 */
	શૂન્ય _calcThrottleControlUpdate(દશાંશ dt, અચલ STERateLimit &limit, અચલ Control_કિંમતો &ste_rate, અચલ Param &param,
					અચલ Flag &flag);

	/**
	 * @brief Calculate the throttle control output function.
	 *
	 * @param limit is the specific total energy rate limits in [m²/s³].
	 * @param ste_rate is the specific total energy rates in [m²/s³].
	 * @param param is the control parameters.
	 * @param flag is the control flags.
	 * @return throttle setpoint in [0,1].
	 */
	દશાંશ _calcThrottleControlOutput(અચલ STERateLimit &limit, અચલ Control_કિંમતો &ste_rate, અચલ Param &param,
					 અચલ Flag &flag) અચલ;

ખાનગી:
	// State
	AlphaFilter<દશાંશ> _ste_rate_estimate_filter;		///< Low pass filter for the specific total energy rate.
	દશાંશ _pitch_integ_state{0.0f};				///< Pitch integrator state [rad].
	દશાંશ _fast_descend_prev{0.0f};				///< Fast descend ratio of the last update [0,1].
	દશાંશ _throttle_integ_state{0.0f};			///< Throttle integrator state [-].

	// Output
	DebugOutput _debug_output;				///< Debug output.
	દશાંશ _pitch_setpoint{0.0f};				///< Controlled absolute pitch setpoint [rad].
	દશાંશ _throttle_setpoint{0.0f};				///< Controlled throttle setpoint [0,1].
	દશાંશ _ratio_undersped{0.0f};				///< A continuous representation of how "undersped" the TAS is [0,1]
};

વર્ગ TECS
{
જાહેર:
	સંરચના DebugOutput {
		TECSControl::DebugOutput control;
		દશાંશ true_airspeed_sp;
		દશાંશ true_airspeed_filtered;
		દશાંશ true_airspeed_derivative;
		દશાંશ altitude_reference;
		દશાંશ height_rate_reference;
		દશાંશ height_rate_direct;
		દશાંશ fast_descend;
	};

	/**
	 * @brief Vehicle state, setpoints and limits for one control update.
	 *
	 */
	સંરચના આગત_2 {
		દશાંશ altitude;				///< Current altitude amsl [m].
		દશાંશ altitude_rate;			///< Current altitude rate [m/s].
		દશાંશ altitude_setpoint;		///< Altitude setpoint amsl [m], NAN if not set.
		દશાંશ altitude_rate_setpoint{NAN};	///< Altitude rate setpoint [m/s], NAN if not set.
		દશાંશ equivalent_airspeed;		///< Current equivalent airspeed [m/s].
		દશાંશ speed_deriv_forward;		///< Current forward acceleration, used as true airspeed rate [m/s²].
		દશાંશ equivalent_airspeed_setpoint;	///< Equivalent airspeed setpoint [m/s].
		દશાંશ eas_to_tas;			///< Conversion factor from equivalent to true airspeed [-].
		દશાંશ throttle_min;			///< Normalized throttle lower limit.
		દશાંશ throttle_max;			///< Normalized throttle upper limit.
		દશાંશ throttle_trim;			///< Normalized throttle required to fly level at the airspeed setpoint.
		દશાંશ pitch_min;			///< Minimum absolute pitch angle setpoint [rad].
		દશાંશ pitch_max;			///< Maximum absolute pitch angle setpoint [rad].
		દશાંશ target_climbrate;			///< Climb rate the reference model targets [m/s].
		દશાંશ target_sinkrate;			///< Sink rate the reference model targets [m/s].
	};

જાહેર:
	TECS() = મૂળભૂત;
	~TECS() = મૂળભૂત;

	// no copy, assignment, move, move assignment
	TECS(અચલ TECS &) = delete;
	TECS &operator=(અચલ TECS &) = delete;
	TECS(TECS &&) = delete;
	TECS &operator=(TECS &&) = delete;

	અચલ DebugOutput &getStatus() અચલ { પાછા _debug_status; }

	/**
	 * Set the airspeed enable state
	 *
	 * @param[in] enabled is true if the airspeed measurement is used for control.
	 */
	શૂન્ય enable_airspeed(તાર્કિક enabled) { _control_flag.airspeed_enabled = enabled; }

	/**
	 * @brief Update the control loop calculations
	 *
	 * All pitch angles, including the pitch limits and the resulting pitch setpoint, are absolute: the angle of the
	 * body x-axis above the horizon.
	 *
	 * @param[in] input is the current vehicle state, setpoints and limits.
	 */
	શૂન્ય update(અચલ આગત_2 &આગત);

	શૂન્ય resetIntegrals()
	{
		_control.resetIntegrals();
	}

	શૂન્ય set_detect_underspeed_enabled(તાર્કિક enabled) { _control_flag.detect_underspeed_enabled = enabled; }

	// setters for parameters

	શૂન્ય set_integrator_gain_throttle(દશાંશ gain) { _control_param.integrator_gain_throttle = gain;}
	શૂન્ય set_integrator_gain_pitch(દશાંશ gain) { _control_param.integrator_gain_pitch = gain; }

	શૂન્ય set_max_sink_rate(દશાંશ max_sink_rate) { _control_param.max_sink_rate = max_sink_rate; _reference_param.max_sink_rate = max_sink_rate; }
	શૂન્ય set_min_sink_rate(દશાંશ min_sink_rate) { _control_param.min_sink_rate = min_sink_rate; }
	શૂન્ય set_max_climb_rate(દશાંશ climb_rate) { _control_param.max_climb_rate = climb_rate; _reference_param.max_climb_rate = climb_rate; }

	શૂન્ય set_altitude_rate_ff(દશાંશ altitude_rate_ff) { _control_param.altitude_setpoint_gain_ff = altitude_rate_ff; }
	શૂન્ય set_altitude_error_time_constant(દશાંશ time_const) { _control_param.altitude_error_gain = 1.0f / math::max(time_const, 0.1f); }
	શૂન્ય set_fast_descend_altitude_error(દશાંશ altitude_error) { _fast_descend_alt_err = altitude_error; }

	શૂન્ય set_equivalent_airspeed_min(દશાંશ airspeed) { _equivalent_airspeed_min = airspeed; }
	શૂન્ય set_equivalent_airspeed_stall(દશાંશ airspeed) { _equivalent_airspeed_stall = airspeed; }
	શૂન્ય set_equivalent_airspeed_max(દશાંશ airspeed) { _equivalent_airspeed_max = airspeed; }
	શૂન્ય set_equivalent_airspeed_trim(દશાંશ airspeed) { _control_param.equivalent_airspeed_trim = airspeed; _airspeed_filter_param.equivalent_airspeed_trim = airspeed; }

	શૂન્ય set_pitch_damping(દશાંશ damping) { _control_param.pitch_damping_gain = damping; }
	શૂન્ય set_vertical_accel_limit(દશાંશ limit) { _reference_param.vert_accel_limit = limit; _control_param.vert_accel_limit = limit; }

	શૂન્ય set_speed_weight(દશાંશ weight) { _control_param.pitch_speed_weight = weight; }
	શૂન્ય set_airspeed_error_time_constant(દશાંશ time_const) { _control_param.airspeed_error_gain = 1.0f / math::max(time_const, 0.1f); }

	શૂન્ય set_throttle_damp(દશાંશ throttle_damp) { _control_param.throttle_damping_gain = throttle_damp; }
	શૂન્ય set_throttle_slewrate(દશાંશ slewrate) { _control_param.throttle_slewrate = slewrate; }

	શૂન્ય set_roll_throttle_compensation(દશાંશ compensation) { _control_param.load_factor_correction = compensation; }
	શૂન્ય set_load_factor(દશાંશ load_factor) { _control_param.load_factor = load_factor; }

	શૂન્ય set_ste_rate_time_const(દશાંશ time_const) { _control_param.ste_rate_time_const = time_const; }

	શૂન્ય set_seb_rate_ff_gain(દશાંશ ff_gain) { _control_param.seb_rate_ff = ff_gain; }

	/**
	 * Set the pitch angle in level flight at trim airspeed [rad], fed forward as the pitch at zero climb angle
	 */
	શૂન્ય set_level_flight_pitch(દશાંશ pitch) { _control_param.level_flight_pitch = pitch; }

	/**
	 * Handle the altitude reset
	 *
	 * If the estimation system resets the height in one discrete step this
	 * will gracefully even out the reset over time.
	 *
	 * @param[in] altitude is the altitude amsl after the reset [m].
	 * @param[in] altitude_rate is the current altitude rate [m/s].
	 */
	શૂન્ય handle_alt_step(દશાંશ altitude, દશાંશ altitude_rate)
	{
		TECSAltitudeReferenceModel::AltitudeReferenceState init_state{ .alt = altitude,
				.alt_rate = altitude_rate};

		// reset altitude reference model.
		_altitude_reference_model.initialize(init_state);
	}

	/**
	 * Returns the absolute pitch setpoint [rad]
	 */
	દશાંશ get_pitch_setpoint() અચલ {પાછા _control.getPitchSetpoint();}
	દશાંશ get_throttle_setpoint() અચલ {પાછા _control.getThrottleSetpoint();}

	/**
	 * Returns the altitude tracking time constant
	 */
	દશાંશ get_altitude_error_time_constant() અચલ { પાછા 1.0f / _control_param.altitude_error_gain; }

	દશાંશ get_underspeed_ratio() અચલ { પાછા _control.getUnderspeedRatio(); }

ખાનગી:
	/**
	 * @brief Initialize the control parameters
	 *
	 * @param[in] input is the current vehicle state, setpoints and limits.
	 */
	શૂન્ય initControlParams(અચલ આગત_2 &આગત);

	/**
	 * @brief calculate true airspeed setpoint
	 *
	 * Calculate true airspeed setpoint based on input and fast descend ratio
	 *
	 * @param eas_to_tas is the eas to tas conversion factor
	 * @param eas_setpoint is the desired equivalent airspeed setpoint [m/s]
	 * @return true airspeed setpoint[m/s]
	 */
	દશાંશ calcTrueAirspeedSetpoint(દશાંશ eas_to_tas, દશાંશ eas_setpoint);

	/**
	 * @brief Initialize the control loop
	 *
	 * @param[in] input is the current vehicle state, setpoints and limits.
	 */
	શૂન્ય initialize(અચલ આગત_2 &આગત);

	TECSControl 			_control;			///< Control submodule.
	TECSAirspeedFilter 		_airspeed_filter;		///< Airspeed filter submodule.
	TECSAltitudeReferenceModel 	_altitude_reference_model;	///< Setpoint reference model submodule.

	hrt_abstime _update_timestamp{0};				///< last timestamp of the update function call.

	દશાંશ _equivalent_airspeed_min{10.0f};				///< equivalent airspeed demand lower limit (m/sec)
	દશાંશ _equivalent_airspeed_stall{7.0f};				///< equivalent stall airspeed (m/sec)
	દશાંશ _equivalent_airspeed_max{20.0f};				///< equivalent airspeed demand upper limit (m/sec)
	દશાંશ _fast_descend_alt_err{-1.f};	 				///< Altitude difference between current altitude to altitude setpoint needed to descend with higher airspeed [m].
	દશાંશ _fast_descend{0.f};					///< Value for fast descend in [0,1]. continuous value used to flatten the high speed value out when close to target altitude.
	hrt_abstime _enabled_fast_descend_timestamp{0U};		///< timestamp at activation of fast descend mode

	સ્થિર constexpr દશાંશ DT_MIN = 0.001f;				///< minimum allowed value of _dt (sec)
	સ્થિર constexpr દશાંશ DT_MAX = 1.0f;				///< max value of _dt allowed before a filter state reset is performed (sec)
	સ્થિર constexpr hrt_abstime FAST_DESCEND_RAMP_UP_TIME = 2_s; 	///< Ramp up time until fast descend is fully engaged
	સ્થિર constexpr દશાંશ FAST_DESCEND_HEIGHT_MARGIN = 5.f;	///< Height above setpoint where fast descend ends [m]

	DebugOutput _debug_status{};

	// Params
	/// Airspeed filter parameters.
	TECSAirspeedFilter::Param _airspeed_filter_param{
		.equivalent_airspeed_trim = 15.0f,
	};
	/// Reference model parameters.
	TECSAltitudeReferenceModel::Param _reference_param{
		.target_climbrate = 2.0f,
		.target_sinkrate = 2.0f,
		.vert_accel_limit = 0.0f,
		.max_climb_rate = 2.0f,
		.max_sink_rate = 2.0f,
	};
	/// Control parameters.
	TECSControl::Param _control_param{
		.max_sink_rate = 5.0f,
		.min_sink_rate = 2.0f,
		.max_climb_rate = 5.0f,
		.vert_accel_limit = 0.0f,
		.equivalent_airspeed_trim = 15.0f,
		.tas_min = 10.0f,
		.tas_stall = 7.0f,
		.tas_max = 20.0f,
		.tas_trim = 15.0f,
		.pitch_max = 0.5f,
		.pitch_min = -0.5f,
		.level_flight_pitch = 0.f,
		.throttle_trim = 0.0f,
		.throttle_max = 1.0f,
		.throttle_min = 0.1f,
		.altitude_error_gain = 0.2f,
		.altitude_setpoint_gain_ff = 0.0f,
		.airspeed_error_gain = 0.1f,
		.ste_rate_time_const = 0.1f,
		.seb_rate_ff = 1.0f,
		.pitch_speed_weight = 1.0f,
		.integrator_gain_pitch = 0.0f,
		.pitch_damping_gain = 0.0f,
		.integrator_gain_throttle = 0.0f,
		.throttle_damping_gain = 0.0f,
		.throttle_slewrate = 0.0f,
		.load_factor_correction = 0.0f,
		.load_factor = 1.0f,
		.fast_descend = 0.f
	};

	TECSControl::Flag _control_flag{
		.airspeed_enabled = ખોટું,
		.detect_underspeed_enabled = ખોટું,
	};

	/**
	 * @brief Set fast descend value
	 *
	 * @param alt_setpoint is the altitude setpoint
	 * @param alt is the current altitude
	 */
	શૂન્ય _setFastDescend(દશાંશ alt_setpoint, દશાંશ alt);
};

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

#ଅନ୍ତର୍ଭାଵ <drivers/drv_hrt.h>
#ଅନ୍ତର୍ଭାଵ <mathlib/mathlib.h>
#ଅନ୍ତର୍ଭାଵ <matrix/math.hpp>
#ଅନ୍ତର୍ଭାଵ <lib/mathlib/math/filter/AlphaFilter.hpp>
#ଅନ୍ତର୍ଭାଵ <motion_planning/VelocitySmoothing.hpp>
#ଅନ୍ତର୍ଭାଵ <motion_planning/ManualVelocitySmoothingZ.hpp>

using ନାମାକାଶ time_literals;

#ifndef FRIEND_TEST // for gtest
#define FRIEND_TEST(a, b)
#endif

ଶ୍ରେଣୀ TECSAirspeedFilter
{
ସର୍ବସାଧାରଣ:
	/**
	 * @brief State of the equivalent airspeed filter.
	 *
	 */
	ସଂରଚନା AirspeedFilterState {
		ଦଶମିକ speed;		///< speed of the air in EAS [m/s]
		ଦଶମିକ speed_rate;	///< rate of speed of the air [m/s²]
	};

	/**
	 * @brief Parameters of the airspeed filter.
	 *
	 */
	ସଂରଚନା Param {
		ଦଶମିକ equivalent_airspeed_trim; 	///< the trim value of the equivalent airspeed [m/s].
	};

	/**
	 * @brief Input, which will be filtered.
	 *
	 */
	ସଂରଚନା ନିବେଶ_2 {
		ଦଶମିକ equivalent_airspeed; 	///< the measured equivalent airspeed in [m/s].
		ଦଶମିକ equivalent_airspeed_rate;	///< the measured rate of equivalent airspeed in [m/s²].
	};
ସର୍ବସାଧାରଣ:
	TECSAirspeedFilter() = ପୂର୍ବନିର୍ଦ୍ଧାରିତ;
	~TECSAirspeedFilter() = ପୂର୍ବନିର୍ଦ୍ଧାରିତ;
	/**
	 * @brief Initialize filter
	 *
	 * @param[in] equivalent_airspeed is the equivalent airspeed in [m/s].
	 * @param[in] equivalent_airspeed_trim is the equivalent airspeed trim (vehicle setting) in [m/s].
	 * @param[in] airspeed_sensor_available boolean if the airspeed sensor is available.
	 */
	ଶୂନ୍ୟ initialize(ଦଶମିକ equivalent_airspeed, ଅଚଳ ଦଶମିକ equivalent_airspeed_trim,
			ଅଚଳ ତାର୍କିକ airspeed_sensor_available);

	/**
	 * @brief Update filter
	 *
	 * @param[in] dt is the timestep in [s].
	 * @param[in] input are the raw measured values.
	 * @param[in] param are the filter parameters.
	 * @param[in] airspeed_sensor_available boolean if the airspeed sensor is available.
	 */
	ଶୂନ୍ୟ update(ଦଶମିକ dt, ଅଚଳ ନିବେଶ_2 &ନିବେଶ, ଅଚଳ Param &param, ଅଚଳ ତାର୍କିକ airspeed_sensor_available);

	/**
	 * @brief Get the filtered airspeed states.
	 *
	 * @return Current state of the airspeed filter.
	 */
	AirspeedFilterState getState() ଅଚଳ;

ବ୍ୟକ୍ତିଗତ:
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
	ସ୍ଥିର constexpr ଦଶମିକ kKg00{1.91940287f};
	ସ୍ଥିର constexpr ଦଶମିକ kKg01{0.25925926f};
	ସ୍ଥିର constexpr ଦଶମିକ kKg10{2.11640212f};
	ସ୍ଥିର constexpr ଦଶମିକ kKg11{0.67179101f};
};

ଶ୍ରେଣୀ TECSAltitudeReferenceModel
{
ସର୍ବସାଧାରଣ:
	/**
	 * @brief Altitude reference state.
	 *
	 */
	ସଂରଚନା AltitudeReferenceState {
		ଦଶମିକ alt; 	///< Reference altitude amsl in [m].
		ଦଶମିକ alt_rate;	///< Reference altitude rate in [m/s].
	};

	/**
	 * @brief Parameters for the reference model.
	 *
	 */
	ସଂରଚନା Param {
		ଦଶମିକ target_climbrate;	///< The target climbrate in [m/s].
		ଦଶମିକ target_sinkrate;	///< The target sinkrate in [m/s].
		ଦଶମିକ vert_accel_limit;	///< Magnitude of the maximum vertical acceleration allowed [m/s²].
		ଦଶମିକ max_climb_rate;	///< Climb rate produced by max allowed throttle [m/s].
		ଦଶମିକ max_sink_rate;	///< Maximum sink rate (with min throttle, max speed) [m/s].
	};

ସର୍ବସାଧାରଣ:
	TECSAltitudeReferenceModel() = ପୂର୍ବନିର୍ଦ୍ଧାରିତ;
	~TECSAltitudeReferenceModel() = ପୂର୍ବନିର୍ଦ୍ଧାରିତ;

	/**
	 * @brief Initialize reference models.
	 *
	 * @param[in] state is the current altitude state of the vehicle.
	 */
	ଶୂନ୍ୟ initialize(ଅଚଳ AltitudeReferenceState &state);

	/**
	 * @brief Update reference models.
	 *
	 * @param[in] dt is the update interval in [s].
	 * @param[in] setpoint are the desired setpoints.
	 * @param[in] altitude is the altitude amsl in [m].
	 * @param[in] height_rate is the current height rate in [m/s].
	 * @param[in] param are the reference model parameters.
	 */
	ଶୂନ୍ୟ update(ଦଶମିକ dt, ଅଚଳ AltitudeReferenceState &setpoint, ଦଶମିକ altitude, ଦଶମିକ height_rate, ଅଚଳ Param &param);

	/**
	 * @brief Get the current altitude reference of altitude reference model.
	 *
	 * @return Altitude reference state.
	 */
	AltitudeReferenceState getAltitudeReference() ଅଚଳ;

	/**
	 * @brief Get the Height Rate Setpoint directly from the velocity trajectory generator
	 *
	 * @return float direct height rate setpoint [m/s]
	 */
	ଦଶମିକ getHeightRateSetpointDirect() ଅଚଳ {ଫେରନ୍ତୁ _height_rate_setpoint_direct; }


ବ୍ୟକ୍ତିଗତ:
	ସ୍ଥିର constexpr ଦଶମିକ JERK_MAX = 1000.f;	///< Magnitude of the maximum jerk allowed [m/s³].

	// State
	VelocitySmoothing
	_alt_control_traj_generator;		///< Generates altitude rate and altitude setpoint trajectory when altitude is commanded.
	ManualVelocitySmoothingZ
	_velocity_control_traj_generator;	///< generates height rate trajectory when height rate is commanded
	ଦଶମିକ _height_rate_setpoint_direct{NAN}; ///< generated direct height rate setpoint
};

ଶ୍ରେଣୀ TECSControl
{
ସର୍ବସାଧାରଣ:
	/**
	 * @brief The control parameters.
	 *
	 */
	ସଂରଚନା Param {
		// Vehicle specific params
		ଦଶମିକ max_sink_rate;			///< Maximum sink rate (with min throttle and max speed) [m/s].
		ଦଶମିକ min_sink_rate;			///< Minimum sink rate (with min throttle and trim speed) [m/s].
		ଦଶମିକ max_climb_rate;			///< Climb rate produced by max allowed throttle [m/s].
		ଦଶମିକ vert_accel_limit;			///< Magnitude of the maximum vertical acceleration allowed [m/s²].
		ଦଶମିକ equivalent_airspeed_trim;		///< Equivalent cruise airspeed for airspeed less mode [m/s].
		ଦଶମିକ tas_min;				///< True airspeed demand lower limit [m/s].
		ଦଶମିକ tas_stall;			///< True stall airspeed [m/s].
		ଦଶମିକ tas_max;				///< True airspeed demand upper limit [m/s].
		ଦଶମିକ tas_trim;				///< True airspeed trim [m/s].
		ଦଶମିକ pitch_max;			///< Maximum absolute pitch angle setpoint [rad].
		ଦଶମିକ pitch_min;			///< Minimum absolute pitch angle setpoint [rad].
		ଦଶମିକ level_flight_pitch;		///< Pitch angle in level flight at trim airspeed, fed forward at zero climb angle [rad].
		ଦଶମିକ throttle_trim;		///< Normalized throttle required to fly level at calibrated airspeed setpoint [0,1]
		ଦଶମିକ throttle_max;			///< Normalized throttle upper limit.
		ଦଶମିକ throttle_min;			///< Normalized throttle lower limit.

		// Altitude control param
		ଦଶମିକ altitude_error_gain;		///< Altitude error inverse time constant [1/s].
		ଦଶମିକ altitude_setpoint_gain_ff;	///< Gain from altitude demand derivative to demanded climb rate.

		// Airspeed control param
		ଦଶମିକ airspeed_error_gain;				///< Airspeed error inverse time constant [1/s].

		// Energy control param
		ଦଶମିକ ste_rate_time_const;				///< Filter time constant for specific total energy rate (damping path) [s].
		ଦଶମିକ seb_rate_ff;					///< Specific energy balance rate feedforward gain.

		// Pitch control param
		ଦଶମିକ pitch_speed_weight;				///< Speed control weighting used by pitch demand calculation.
		ଦଶମିକ integrator_gain_pitch;				///< Integrator gain used by the pitch demand calculation.
		ଦଶମିକ pitch_damping_gain;				///< Damping gain of the pitch demand calculation [s].

		// Throttle control param
		ଦଶମିକ integrator_gain_throttle;				///< Integrator gain used by the throttle demand calculation.
		ଦଶମିକ throttle_damping_gain;				///< Damping gain of the throttle demand calculation [s].
		ଦଶମିକ throttle_slewrate;				///< Throttle demand slew rate limit [1/s].

		ଦଶମିକ load_factor_correction;				///< Gain from normal load factor increase to total energy rate demand [m²/s³].
		ଦଶମିକ load_factor;					///< Additional normal load factor.

		ଦଶମିକ fast_descend;
	};

	/**
	 * @brief The debug output
	 *
	 */
	ସଂରଚନା DebugOutput {
		ଦଶମିକ altitude_rate_control;			///< Altitude rate setpoint from altitude control loop [m/s].
		ଦଶମିକ true_airspeed_derivative_control;		///< Airspeed rate setpoint from airspeed control loop [m/s²].
		ଦଶମିକ total_energy_rate_estimate;		///< Total energy rate estimate [m²/s³].
		ଦଶମିକ total_energy_rate_sp;			///< Total energy rate setpoint [m²/s³].
		ଦଶମିକ energy_balance_rate_estimate;		///< Energy balance rate estimate [m²/s³].
		ଦଶମିକ energy_balance_rate_sp;			///< Energy balance rate setpoint [m²/s³].
		ଦଶମିକ pitch_integrator;				///< Pitch control integrator state, a correction added to the pitch setpoint [rad].
		ଦଶମିକ throttle_integrator;			///< Throttle control integrator state [-].
	};

	/**
	 * @brief Given setpoint to control.
	 *
	 */
	ସଂରଚନା Setpoint {
		TECSAltitudeReferenceModel::AltitudeReferenceState altitude_reference;	///< Altitude/height rate reference.
		ଦଶମିକ altitude_rate_setpoint_direct;					///< Direct height rate setpoint.
		ଦଶମିକ tas_setpoint;						///< True airspeed setpoint.
	};

	/**
	 * @brief Given current measurement from the UAS.
	 *
	 */
	ସଂରଚନା ନିବେଶ_2 {
		ଦଶମିକ altitude;		///< Current altitude amsl of the UAS [m].
		ଦଶମିକ altitude_rate;	///< Current altitude rate of the UAS [m/s].
		ଦଶମିକ tas;		///< Current true airspeed of the UAS [m/s].
		ଦଶମିକ tas_rate;		///< Current true airspeed rate of the UAS [m/s²].
	};

	/**
	 * @brief Control flags.
	 *
	 */
	ସଂରଚନା Flag {
		ତାର୍କିକ airspeed_enabled;			///< Flag if the airspeed sensor is enabled.
		ତାର୍କିକ detect_underspeed_enabled;		///< Flag if underspeed detection is enabled.
	};
ସର୍ବସାଧାରଣ:
	TECSControl() = ପୂର୍ବନିର୍ଦ୍ଧାରିତ;
	~TECSControl() = ପୂର୍ବନିର୍ଦ୍ଧାରିତ;
	/**
	 * @brief Initialization of the state.
	 *
	 * @param[in] setpoint is the current setpoint struct.
	 * @param[in] input is the current input measurements.
	 * @param[in] param is the current parameter set.
	 * @param[in] flag is the current activated flags.
	 */
	ଶୂନ୍ୟ initialize(ଅଚଳ Setpoint &setpoint, ଅଚଳ ନିବେଶ_2 &ନିବେଶ, ଅଚଳ Param &param, ଅଚଳ Flag &flag);
	/**
	 * @brief Update state and output.
	 *
	 * @param[in] dt is the update time interval in [s].
	 * @param[in] setpoint is the current setpoint struct.
	 * @param[in] input is the current input measurements.
	 * @param[in] param is the current parameter set.
	 * @param[in] flag is the current activated flags.
	 */
	ଶୂନ୍ୟ update(ଦଶମିକ dt, ଅଚଳ Setpoint &setpoint, ଅଚଳ ନିବେଶ_2 &ନିବେଶ, ଅଚଳ Param &param, ଅଚଳ Flag &flag);
	/**
	 * @brief Reset the control loop integrals.
	 *
	 */
	ଶୂନ୍ୟ resetIntegrals();
	/**
	 * @brief Get the underspeed ratio.
	 *
	 * @return Ratio of detected underspeed [0,1].
	 */
	ଦଶମିକ getUnderspeedRatio() ଅଚଳ {ଫେରନ୍ତୁ _ratio_undersped;}
	/**
	 * @brief Get the throttle setpoint.
	 *
	 * @return throttle setpoint.
	 */
	ଦଶମିକ getThrottleSetpoint() ଅଚଳ {ଫେରନ୍ତୁ _throttle_setpoint;}
	/**
	 * @brief Get the pitch setpoint.
	 *
	 * @return The commanded absolute pitch angle [rad].
	 */
	ଦଶମିକ getPitchSetpoint() ଅଚଳ {ଫେରନ୍ତୁ _pitch_setpoint;}
	/**
	 * @brief Get the Debug Output
	 *
	 * @return the debug output struct.
	 */
	ଅଚଳ DebugOutput &getDebugOutput() ଅଚଳ { ଫେରନ୍ତୁ _debug_output; }

ବ୍ୟକ୍ତିଗତ:
	// Allows the regression test to pre-corrupt _pitch_integ_state and verify the
	// safety reset in _calcPitchControlUpdate, which the public update() path cannot
	// reach on its own because the integrator-input guard rejects bad input first.
	FRIEND_TEST(TECSControlTest, PitchIntegratorResetsCorruptedState);

	/**
	 * @brief Specific total energy rate limit.
	 *
	 */
	ସଂରଚନା STERateLimit {
		ଦଶମିକ STE_rate_max;	///< Maximum specific total energy rate limit [m²/s³].
		ଦଶମିକ STE_rate_min;	///< Minimal specific total energy rate limit [m²/s³].
	};

	/**
	 * @brief Control values.
	 * setpoint and current state estimate value as input to a controller.
	 *
	 */
	ସଂରଚନା Control_ମୂଲ୍ୟଗୁଡ଼ିକ {
		ଦଶମିକ setpoint;		///< Control setpoint.
		ଦଶମିକ estimate;		///< Control estimate of current state value.
	};

	/**
	 * @brief Calculated specific energy rates.
	 *
	 */
	ସଂରଚନା SpecificEnergyRates {
		Control_ମୂଲ୍ୟଗୁଡ଼ିକ ske_rate;	///< Specific kinetic energy rate [m²/s³].
		Control_ମୂଲ୍ୟଗୁଡ଼ିକ spe_rate;	///< Specific potential energy rate [m²/s³].
	};

	/**
	 * @brief Altitude rate and airspeed setpoints from the outer loops, from which the energy rate setpoints follow.
	 *
	 */
	ସଂରଚନା OuterLoopSetpoints {
		ଦଶମିକ altitude_rate_setpoint;	///< Controlled altitude rate setpoint [m/s].
		ଦଶମିକ tas_rate_setpoint;	///< Controlled true airspeed rate setpoint [m/s²].
	};

	/**
	 * @brief Weight factors for specific energy.
	 *
	 */
	ସଂରଚନା SpecificEnergyWeighting {
		ଦଶମିକ spe_weighting;	///< Specific potential energy weight.
		ଦଶମିକ ske_weighting;	///< Specific kinetic energy weight.
	};

ବ୍ୟକ୍ତିଗତ:
	/**
	 * @brief Get control error from setpoint and estimate
	 *
	 * @param val is the current control setpoint and estimate.
	 * @return error value
	 */
	ସ୍ଥିର constexpr ଦଶମିକ _getControlError(TECSControl::Control_ମୂଲ୍ୟଗୁଡ଼ିକ val) {ଫେରନ୍ତୁ (val.setpoint - val.estimate);}
	/**
	 * @brief Calculate specific total energy rate limits.
	 *
	 * @param[in] param are the control parameters.
	 * @return Specific total energy rate limits in [m²/s³].
	 */
	STERateLimit _calculateTotalEnergyRateLimit(ଅଚଳ Param &param) ଅଚଳ;
	/**
	 * @brief Gain from specific total energy rate error to throttle.
	 *
	 * @param[in] limit is the specific total energy rate limits in [m²/s³].
	 * @return throttle per specific total energy rate [s³/m²].
	 */
	ସ୍ଥିର ଦଶମିକ _steRateToThrottle(ଅଚଳ STERateLimit &limit) { ଫେରନ୍ତୁ 1.0f / (limit.STE_rate_max - limit.STE_rate_min); }
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
	ଦଶମିକ _calcAirspeedControlOutput(ଅଚଳ Setpoint &setpoint, ଅଚଳ ନିବେଶ_2 &ନିବେଶ, ଅଚଳ STERateLimit &limit,
					 ଅଚଳ Param &param, ଅଚଳ Flag &flag) ଅଚଳ;
	/**
	 * @brief calculate altitude control proportional output.
	 *
	 * @param setpoint is the control setpoints.
	 * @param input is the current input measurement of the UAS.
	 * @param param is the control parameters.
	 * @return controlled altitude rate setpoint in [m/s].
	 */
	ଦଶମିକ _calcAltitudeControlOutput(ଅଚଳ Setpoint &setpoint, ଅଚଳ ନିବେଶ_2 &ନିବେଶ, ଅଚଳ Param &param) ଅଚଳ;
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
	SpecificEnergyRates _updateOuterLoops(ଅଚଳ Setpoint &setpoint, ଅଚଳ ନିବେଶ_2 &ନିବେଶ, ଅଚଳ STERateLimit &limit,
					      ଅଚଳ Param &param, ଅଚଳ Flag &flag);
	/**
	 * @brief Calculate specific energy rates.
	 *
	 * @param control_setpoint is the controlled altitude and airspeed rate setpoints.
	 * @param input is the current input measurement of the UAS.
	 * @return Specific energy rates in [m²/s³].
	 */
	SpecificEnergyRates _calcSpecificEnergyRates(ଅଚଳ OuterLoopSetpoints &control_setpoint, ଅଚଳ ନିବେଶ_2 &ନିବେଶ) ଅଚଳ;
	/**
	 * @brief Detect underspeed.
	 *
	 * @param input is the current input measurement of the UAS.
	 * @param param is the control parameters.
	 * @param flag is the control flags.
	 */
	ଶୂନ୍ୟ _detectUnderspeed(ଅଚଳ ନିବେଶ_2 &ନିବେଶ, ଅଚଳ Param &param, ଅଚଳ Flag &flag);
	/**
	 * @brief Update specific energy balance weights.
	 *
	 * @param param is the control parameters.
	 * @param flag is the control flags.
	 * @return Weights used for the specific energy balance.
	 */
	SpecificEnergyWeighting _updateSpeedAltitudeWeights(ଅଚଳ Param &param, ଅଚଳ Flag &flag);
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
	ଦଶମିକ _calcPitchControlSkeRateSetpoint(ଅଚଳ Setpoint &setpoint, ଅଚଳ ନିବେଶ_2 &ନିବେଶ, ଅଚଳ STERateLimit &limit,
					       ଅଚଳ Param &param, ଅଚଳ Flag &flag, ଅଚଳ SpecificEnergyWeighting &weight) ଅଚଳ;
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
	SpecificEnergyRates _calcPitchControlEnergyRates(ଅଚଳ Setpoint &setpoint, ଅଚଳ ନିବେଶ_2 &ନିବେଶ,
			ଅଚଳ STERateLimit &limit, ଅଚଳ SpecificEnergyRates &specific_energy_rates, ଅଚଳ Param &param,
			ଅଚଳ Flag &flag, ଅଚଳ SpecificEnergyWeighting &weight) ଅଚଳ;
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
	ଶୂନ୍ୟ _calcPitchControl(ଦଶମିକ dt, ଅଚଳ Setpoint &setpoint, ଅଚଳ ନିବେଶ_2 &ନିବେଶ, ଅଚଳ STERateLimit &limit,
			       ଅଚଳ SpecificEnergyRates &specific_energy_rate, ଅଚଳ Param &param, ଅଚଳ Flag &flag);

	/**
	 * @brief Calculate pitch control specific energy balance rates.
	 *
	 * @param weight is the weighting use of the potential and kinetic energy.
	 * @param specific_energy_rate is the specific energy rates in [m²/s³].
	 * @return specific energy balance rate values in [m²/s³].
	 */
	Control_ମୂଲ୍ୟଗୁଡ଼ିକ _calcPitchControlSebRate(ଅଚଳ SpecificEnergyWeighting &weight,
					       ଅଚଳ SpecificEnergyRates &specific_energy_rate) ଅଚଳ;

	/**
	 * @brief Calculate the pitch control update function.
	 * Update the states of the pitch control (pitch integrator).
	 *
	 * @param dt is the update time interval in [s].
	 * @param input is the current input measurement of the UAS.
	 * @param seb_rate is the specific energy balance rate in [m²/s³].
	 * @param param is the control parameters.
	 */
	ଶୂନ୍ୟ _calcPitchControlUpdate(ଦଶମିକ dt, ଅଚଳ ନିବେଶ_2 &ନିବେଶ, ଅଚଳ Control_ମୂଲ୍ୟଗୁଡ଼ିକ &seb_rate, ଅଚଳ Param &param);

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
	ସ୍ଥିର ଦଶମିକ _climbAngleToSebRate(ଅଚଳ ନିବେଶ_2 &ନିବେଶ, ଅଚଳ Param &param);

	/**
	 * @brief Calculate the pitch control output function.
	 *
	 * @param input is the current input measurement of the UAS.
	 * @param seb_rate is the specific energy balance rate in [m²/s³].
	 * @param param is the control parameters.
	 * @return absolute pitch setpoint [rad].
	 */
	ଦଶମିକ _calcPitchControlOutput(ଅଚଳ ନିବେଶ_2 &ନିବେଶ, ଅଚଳ Control_ମୂଲ୍ୟଗୁଡ଼ିକ &seb_rate, ଅଚଳ Param &param) ଅଚଳ;

	/**
	 * @brief Update controlled throttle setpoint.
	 *
	 * @param dt is the update time interval in [s].
	 * @param limit is the specific total energy rate limits in [m²/s³].
	 * @param specific_energy_rate is the calculated specific energy.
	 * @param param is the control parameters.
	 * @param flag is the control flags.
	 */
	ଶୂନ୍ୟ _calcThrottleControl(ଦଶମିକ dt, ଅଚଳ STERateLimit &limit, ଅଚଳ SpecificEnergyRates &specific_energy_rate,
				  ଅଚଳ Param &param, ଅଚଳ Flag &flag);

	/**
	 * @brief Calculate throttle control specific total energy
	 *
	 * @param limit is the specific total energy rate limits in [m²/s³].
	 * @param specific_energy_rate is the specific energy rates in [m²/s³].
	 * @param param is the control parameters.
	 * @return specific total energy rate values in [m²/s³]
	 */
	Control_ମୂଲ୍ୟଗୁଡ଼ିକ _calcThrottleControlSteRate(ଅଚଳ STERateLimit &limit, ଅଚଳ SpecificEnergyRates &specific_energy_rate,
			ଅଚଳ Param &param) ଅଚଳ;

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
	ଶୂନ୍ୟ _calcThrottleControlUpdate(ଦଶମିକ dt, ଅଚଳ STERateLimit &limit, ଅଚଳ Control_ମୂଲ୍ୟଗୁଡ଼ିକ &ste_rate, ଅଚଳ Param &param,
					ଅଚଳ Flag &flag);

	/**
	 * @brief Calculate the throttle control output function.
	 *
	 * @param limit is the specific total energy rate limits in [m²/s³].
	 * @param ste_rate is the specific total energy rates in [m²/s³].
	 * @param param is the control parameters.
	 * @param flag is the control flags.
	 * @return throttle setpoint in [0,1].
	 */
	ଦଶମିକ _calcThrottleControlOutput(ଅଚଳ STERateLimit &limit, ଅଚଳ Control_ମୂଲ୍ୟଗୁଡ଼ିକ &ste_rate, ଅଚଳ Param &param,
					 ଅଚଳ Flag &flag) ଅଚଳ;

ବ୍ୟକ୍ତିଗତ:
	// State
	AlphaFilter<ଦଶମିକ> _ste_rate_estimate_filter;		///< Low pass filter for the specific total energy rate.
	ଦଶମିକ _pitch_integ_state{0.0f};				///< Pitch integrator state [rad].
	ଦଶମିକ _fast_descend_prev{0.0f};				///< Fast descend ratio of the last update [0,1].
	ଦଶମିକ _throttle_integ_state{0.0f};			///< Throttle integrator state [-].

	// Output
	DebugOutput _debug_output;				///< Debug output.
	ଦଶମିକ _pitch_setpoint{0.0f};				///< Controlled absolute pitch setpoint [rad].
	ଦଶମିକ _throttle_setpoint{0.0f};				///< Controlled throttle setpoint [0,1].
	ଦଶମିକ _ratio_undersped{0.0f};				///< A continuous representation of how "undersped" the TAS is [0,1]
};

ଶ୍ରେଣୀ TECS
{
ସର୍ବସାଧାରଣ:
	ସଂରଚନା DebugOutput {
		TECSControl::DebugOutput control;
		ଦଶମିକ true_airspeed_sp;
		ଦଶମିକ true_airspeed_filtered;
		ଦଶମିକ true_airspeed_derivative;
		ଦଶମିକ altitude_reference;
		ଦଶମିକ height_rate_reference;
		ଦଶମିକ height_rate_direct;
		ଦଶମିକ fast_descend;
	};

	/**
	 * @brief Vehicle state, setpoints and limits for one control update.
	 *
	 */
	ସଂରଚନା ନିବେଶ_2 {
		ଦଶମିକ altitude;				///< Current altitude amsl [m].
		ଦଶମିକ altitude_rate;			///< Current altitude rate [m/s].
		ଦଶମିକ altitude_setpoint;		///< Altitude setpoint amsl [m], NAN if not set.
		ଦଶମିକ altitude_rate_setpoint{NAN};	///< Altitude rate setpoint [m/s], NAN if not set.
		ଦଶମିକ equivalent_airspeed;		///< Current equivalent airspeed [m/s].
		ଦଶମିକ speed_deriv_forward;		///< Current forward acceleration, used as true airspeed rate [m/s²].
		ଦଶମିକ equivalent_airspeed_setpoint;	///< Equivalent airspeed setpoint [m/s].
		ଦଶମିକ eas_to_tas;			///< Conversion factor from equivalent to true airspeed [-].
		ଦଶମିକ throttle_min;			///< Normalized throttle lower limit.
		ଦଶମିକ throttle_max;			///< Normalized throttle upper limit.
		ଦଶମିକ throttle_trim;			///< Normalized throttle required to fly level at the airspeed setpoint.
		ଦଶମିକ pitch_min;			///< Minimum absolute pitch angle setpoint [rad].
		ଦଶମିକ pitch_max;			///< Maximum absolute pitch angle setpoint [rad].
		ଦଶମିକ target_climbrate;			///< Climb rate the reference model targets [m/s].
		ଦଶମିକ target_sinkrate;			///< Sink rate the reference model targets [m/s].
	};

ସର୍ବସାଧାରଣ:
	TECS() = ପୂର୍ବନିର୍ଦ୍ଧାରିତ;
	~TECS() = ପୂର୍ବନିର୍ଦ୍ଧାରିତ;

	// no copy, assignment, move, move assignment
	TECS(ଅଚଳ TECS &) = delete;
	TECS &operator=(ଅଚଳ TECS &) = delete;
	TECS(TECS &&) = delete;
	TECS &operator=(TECS &&) = delete;

	ଅଚଳ DebugOutput &getStatus() ଅଚଳ { ଫେରନ୍ତୁ _debug_status; }

	/**
	 * Set the airspeed enable state
	 *
	 * @param[in] enabled is true if the airspeed measurement is used for control.
	 */
	ଶୂନ୍ୟ enable_airspeed(ତାର୍କିକ enabled) { _control_flag.airspeed_enabled = enabled; }

	/**
	 * @brief Update the control loop calculations
	 *
	 * All pitch angles, including the pitch limits and the resulting pitch setpoint, are absolute: the angle of the
	 * body x-axis above the horizon.
	 *
	 * @param[in] input is the current vehicle state, setpoints and limits.
	 */
	ଶୂନ୍ୟ update(ଅଚଳ ନିବେଶ_2 &ନିବେଶ);

	ଶୂନ୍ୟ resetIntegrals()
	{
		_control.resetIntegrals();
	}

	ଶୂନ୍ୟ set_detect_underspeed_enabled(ତାର୍କିକ enabled) { _control_flag.detect_underspeed_enabled = enabled; }

	// setters for parameters

	ଶୂନ୍ୟ set_integrator_gain_throttle(ଦଶମିକ gain) { _control_param.integrator_gain_throttle = gain;}
	ଶୂନ୍ୟ set_integrator_gain_pitch(ଦଶମିକ gain) { _control_param.integrator_gain_pitch = gain; }

	ଶୂନ୍ୟ set_max_sink_rate(ଦଶମିକ max_sink_rate) { _control_param.max_sink_rate = max_sink_rate; _reference_param.max_sink_rate = max_sink_rate; }
	ଶୂନ୍ୟ set_min_sink_rate(ଦଶମିକ min_sink_rate) { _control_param.min_sink_rate = min_sink_rate; }
	ଶୂନ୍ୟ set_max_climb_rate(ଦଶମିକ climb_rate) { _control_param.max_climb_rate = climb_rate; _reference_param.max_climb_rate = climb_rate; }

	ଶୂନ୍ୟ set_altitude_rate_ff(ଦଶମିକ altitude_rate_ff) { _control_param.altitude_setpoint_gain_ff = altitude_rate_ff; }
	ଶୂନ୍ୟ set_altitude_error_time_constant(ଦଶମିକ time_const) { _control_param.altitude_error_gain = 1.0f / math::max(time_const, 0.1f); }
	ଶୂନ୍ୟ set_fast_descend_altitude_error(ଦଶମିକ altitude_error) { _fast_descend_alt_err = altitude_error; }

	ଶୂନ୍ୟ set_equivalent_airspeed_min(ଦଶମିକ airspeed) { _equivalent_airspeed_min = airspeed; }
	ଶୂନ୍ୟ set_equivalent_airspeed_stall(ଦଶମିକ airspeed) { _equivalent_airspeed_stall = airspeed; }
	ଶୂନ୍ୟ set_equivalent_airspeed_max(ଦଶମିକ airspeed) { _equivalent_airspeed_max = airspeed; }
	ଶୂନ୍ୟ set_equivalent_airspeed_trim(ଦଶମିକ airspeed) { _control_param.equivalent_airspeed_trim = airspeed; _airspeed_filter_param.equivalent_airspeed_trim = airspeed; }

	ଶୂନ୍ୟ set_pitch_damping(ଦଶମିକ damping) { _control_param.pitch_damping_gain = damping; }
	ଶୂନ୍ୟ set_vertical_accel_limit(ଦଶମିକ limit) { _reference_param.vert_accel_limit = limit; _control_param.vert_accel_limit = limit; }

	ଶୂନ୍ୟ set_speed_weight(ଦଶମିକ weight) { _control_param.pitch_speed_weight = weight; }
	ଶୂନ୍ୟ set_airspeed_error_time_constant(ଦଶମିକ time_const) { _control_param.airspeed_error_gain = 1.0f / math::max(time_const, 0.1f); }

	ଶୂନ୍ୟ set_throttle_damp(ଦଶମିକ throttle_damp) { _control_param.throttle_damping_gain = throttle_damp; }
	ଶୂନ୍ୟ set_throttle_slewrate(ଦଶମିକ slewrate) { _control_param.throttle_slewrate = slewrate; }

	ଶୂନ୍ୟ set_roll_throttle_compensation(ଦଶମିକ compensation) { _control_param.load_factor_correction = compensation; }
	ଶୂନ୍ୟ set_load_factor(ଦଶମିକ load_factor) { _control_param.load_factor = load_factor; }

	ଶୂନ୍ୟ set_ste_rate_time_const(ଦଶମିକ time_const) { _control_param.ste_rate_time_const = time_const; }

	ଶୂନ୍ୟ set_seb_rate_ff_gain(ଦଶମିକ ff_gain) { _control_param.seb_rate_ff = ff_gain; }

	/**
	 * Set the pitch angle in level flight at trim airspeed [rad], fed forward as the pitch at zero climb angle
	 */
	ଶୂନ୍ୟ set_level_flight_pitch(ଦଶମିକ pitch) { _control_param.level_flight_pitch = pitch; }

	/**
	 * Handle the altitude reset
	 *
	 * If the estimation system resets the height in one discrete step this
	 * will gracefully even out the reset over time.
	 *
	 * @param[in] altitude is the altitude amsl after the reset [m].
	 * @param[in] altitude_rate is the current altitude rate [m/s].
	 */
	ଶୂନ୍ୟ handle_alt_step(ଦଶମିକ altitude, ଦଶମିକ altitude_rate)
	{
		TECSAltitudeReferenceModel::AltitudeReferenceState init_state{ .alt = altitude,
				.alt_rate = altitude_rate};

		// reset altitude reference model.
		_altitude_reference_model.initialize(init_state);
	}

	/**
	 * Returns the absolute pitch setpoint [rad]
	 */
	ଦଶମିକ get_pitch_setpoint() ଅଚଳ {ଫେରନ୍ତୁ _control.getPitchSetpoint();}
	ଦଶମିକ get_throttle_setpoint() ଅଚଳ {ଫେରନ୍ତୁ _control.getThrottleSetpoint();}

	/**
	 * Returns the altitude tracking time constant
	 */
	ଦଶମିକ get_altitude_error_time_constant() ଅଚଳ { ଫେରନ୍ତୁ 1.0f / _control_param.altitude_error_gain; }

	ଦଶମିକ get_underspeed_ratio() ଅଚଳ { ଫେରନ୍ତୁ _control.getUnderspeedRatio(); }

ବ୍ୟକ୍ତିଗତ:
	/**
	 * @brief Initialize the control parameters
	 *
	 * @param[in] input is the current vehicle state, setpoints and limits.
	 */
	ଶୂନ୍ୟ initControlParams(ଅଚଳ ନିବେଶ_2 &ନିବେଶ);

	/**
	 * @brief calculate true airspeed setpoint
	 *
	 * Calculate true airspeed setpoint based on input and fast descend ratio
	 *
	 * @param eas_to_tas is the eas to tas conversion factor
	 * @param eas_setpoint is the desired equivalent airspeed setpoint [m/s]
	 * @return true airspeed setpoint[m/s]
	 */
	ଦଶମିକ calcTrueAirspeedSetpoint(ଦଶମିକ eas_to_tas, ଦଶମିକ eas_setpoint);

	/**
	 * @brief Initialize the control loop
	 *
	 * @param[in] input is the current vehicle state, setpoints and limits.
	 */
	ଶୂନ୍ୟ initialize(ଅଚଳ ନିବେଶ_2 &ନିବେଶ);

	TECSControl 			_control;			///< Control submodule.
	TECSAirspeedFilter 		_airspeed_filter;		///< Airspeed filter submodule.
	TECSAltitudeReferenceModel 	_altitude_reference_model;	///< Setpoint reference model submodule.

	hrt_abstime _update_timestamp{0};				///< last timestamp of the update function call.

	ଦଶମିକ _equivalent_airspeed_min{10.0f};				///< equivalent airspeed demand lower limit (m/sec)
	ଦଶମିକ _equivalent_airspeed_stall{7.0f};				///< equivalent stall airspeed (m/sec)
	ଦଶମିକ _equivalent_airspeed_max{20.0f};				///< equivalent airspeed demand upper limit (m/sec)
	ଦଶମିକ _fast_descend_alt_err{-1.f};	 				///< Altitude difference between current altitude to altitude setpoint needed to descend with higher airspeed [m].
	ଦଶମିକ _fast_descend{0.f};					///< Value for fast descend in [0,1]. continuous value used to flatten the high speed value out when close to target altitude.
	hrt_abstime _enabled_fast_descend_timestamp{0U};		///< timestamp at activation of fast descend mode

	ସ୍ଥିର constexpr ଦଶମିକ DT_MIN = 0.001f;				///< minimum allowed value of _dt (sec)
	ସ୍ଥିର constexpr ଦଶମିକ DT_MAX = 1.0f;				///< max value of _dt allowed before a filter state reset is performed (sec)
	ସ୍ଥିର constexpr hrt_abstime FAST_DESCEND_RAMP_UP_TIME = 2_s; 	///< Ramp up time until fast descend is fully engaged
	ସ୍ଥିର constexpr ଦଶମିକ FAST_DESCEND_HEIGHT_MARGIN = 5.f;	///< Height above setpoint where fast descend ends [m]

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
		.airspeed_enabled = ମିଥ୍ୟା,
		.detect_underspeed_enabled = ମିଥ୍ୟା,
	};

	/**
	 * @brief Set fast descend value
	 *
	 * @param alt_setpoint is the altitude setpoint
	 * @param alt is the current altitude
	 */
	ଶୂନ୍ୟ _setFastDescend(ଦଶମିକ alt_setpoint, ଦଶମିକ alt);
};

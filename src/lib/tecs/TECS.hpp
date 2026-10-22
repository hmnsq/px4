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

#ਹਦਾਇਤ once

#ਅਨ੍ਤਰ੍ਭਾਵ <drivers/drv_hrt.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <mathlib/mathlib.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <matrix/math.hpp>
#ਅਨ੍ਤਰ੍ਭਾਵ <lib/mathlib/math/filter/AlphaFilter.hpp>
#ਅਨ੍ਤਰ੍ਭਾਵ <motion_planning/VelocitySmoothing.hpp>
#ਅਨ੍ਤਰ੍ਭਾਵ <motion_planning/ManualVelocitySmoothingZ.hpp>

ਵਰਤੋਂ ਨਾਮਾਕਾਸ਼ ਸਮਾਂ_literals;

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ FRIEND_TEST // for gtest
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FRIEND_TEST(a, b)
#ਜੇ_ਅੰਤ

ਵਰਗ TECSAirspeedFilter
{
ਜਨਤਕ:
	/**
	 * @brief State of the equivalent airspeed filter.
	 *
	 */
	ਬਣਤਰ AirspeedFilterState {
		ਦਸ਼ਮਲਵ ਰਫਤਾਰ;		///< speed of the air in EAS [m/s]
		ਦਸ਼ਮਲਵ ਰਫਤਾਰ_ਦਰ;	///< rate of speed of the air [m/s²]
	};

	/**
	 * @brief Parameters of the airspeed filter.
	 *
	 */
	ਬਣਤਰ Param {
		ਦਸ਼ਮਲਵ equivalent_airspeed_trim; 	///< the trim value of the equivalent airspeed [m/s].
	};

	/**
	 * @brief Input, which will be filtered.
	 *
	 */
	ਬਣਤਰ ਨਿਵੇਸ਼_2 {
		ਦਸ਼ਮਲਵ equivalent_airspeed; 	///< the measured equivalent airspeed in [m/s].
		ਦਸ਼ਮਲਵ equivalent_airspeed_ਦਰ;	///< the measured rate of equivalent airspeed in [m/s²].
	};
ਜਨਤਕ:
	TECSAirspeedFilter() = ਮੂਲ;
	~TECSAirspeedFilter() = ਮੂਲ;
	/**
	 * @brief Initialize filter
	 *
	 * @param[in] equivalent_airspeed is the equivalent airspeed in [m/s].
	 * @param[in] equivalent_airspeed_trim is the equivalent airspeed trim (vehicle setting) in [m/s].
	 * @param[in] airspeed_sensor_available boolean if the airspeed sensor is available.
	 */
	ਖਾਲੀ initialize(ਦਸ਼ਮਲਵ equivalent_airspeed, ਅਚਲ ਦਸ਼ਮਲਵ equivalent_airspeed_trim,
			ਅਚਲ ਤਾਰਕਿਕ airspeed_sensor_ਉਪਲਬਧ);

	/**
	 * @brief Update filter
	 *
	 * @param[in] dt is the timestep in [s].
	 * @param[in] input are the raw measured values.
	 * @param[in] param are the filter parameters.
	 * @param[in] airspeed_sensor_available boolean if the airspeed sensor is available.
	 */
	ਖਾਲੀ update(ਦਸ਼ਮਲਵ dt, ਅਚਲ ਨਿਵੇਸ਼_2 &ਨਿਵੇਸ਼, ਅਚਲ Param &param, ਅਚਲ ਤਾਰਕਿਕ airspeed_sensor_ਉਪਲਬਧ);

	/**
	 * @brief Get the filtered airspeed states.
	 *
	 * @return Current state of the airspeed filter.
	 */
	AirspeedFilterState getState() ਅਚਲ;

ਨਿਜੀ:
	// States
	AirspeedFilterState _airspeed_state{.ਰਫਤਾਰ = 0.0f, .ਰਫਤਾਰ_ਦਰ = 0.0f};	///< Complementary filter state

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
	ਸਥਿਰ constexpr ਦਸ਼ਮਲਵ kKg00{1.91940287f};
	ਸਥਿਰ constexpr ਦਸ਼ਮਲਵ kKg01{0.25925926f};
	ਸਥਿਰ constexpr ਦਸ਼ਮਲਵ kKg10{2.11640212f};
	ਸਥਿਰ constexpr ਦਸ਼ਮਲਵ kKg11{0.67179101f};
};

ਵਰਗ TECSAltitudeReferenceModel
{
ਜਨਤਕ:
	/**
	 * @brief Altitude reference state.
	 *
	 */
	ਬਣਤਰ AltitudeReferenceState {
		ਦਸ਼ਮਲਵ alt; 	///< Reference altitude amsl in [m].
		ਦਸ਼ਮਲਵ alt_ਦਰ;	///< Reference altitude rate in [m/s].
	};

	/**
	 * @brief Parameters for the reference model.
	 *
	 */
	ਬਣਤਰ Param {
		ਦਸ਼ਮਲਵ ਨਿਸ਼ਾਨਾ_climbrate;	///< The target climbrate in [m/s].
		ਦਸ਼ਮਲਵ ਨਿਸ਼ਾਨਾ_sinkrate;	///< The target sinkrate in [m/s].
		ਦਸ਼ਮਲਵ vert_accel_ਹੱਦਬੰਦੀ;	///< Magnitude of the maximum vertical acceleration allowed [m/s²].
		ਦਸ਼ਮਲਵ max_climb_ਦਰ;	///< Climb rate produced by max allowed throttle [m/s].
		ਦਸ਼ਮਲਵ max_sink_ਦਰ;	///< Maximum sink rate (with min throttle, max speed) [m/s].
	};

ਜਨਤਕ:
	TECSAltitudeReferenceModel() = ਮੂਲ;
	~TECSAltitudeReferenceModel() = ਮੂਲ;

	/**
	 * @brief Initialize reference models.
	 *
	 * @param[in] state is the current altitude state of the vehicle.
	 */
	ਖਾਲੀ initialize(ਅਚਲ AltitudeReferenceState &state);

	/**
	 * @brief Update reference models.
	 *
	 * @param[in] dt is the update interval in [s].
	 * @param[in] setpoint are the desired setpoints.
	 * @param[in] altitude is the altitude amsl in [m].
	 * @param[in] height_rate is the current height rate in [m/s].
	 * @param[in] param are the reference model parameters.
	 */
	ਖਾਲੀ update(ਦਸ਼ਮਲਵ dt, ਅਚਲ AltitudeReferenceState &setpoint, ਦਸ਼ਮਲਵ altitude, ਦਸ਼ਮਲਵ ਉਚਾਈ_ਦਰ, ਅਚਲ Param &param);

	/**
	 * @brief Get the current altitude reference of altitude reference model.
	 *
	 * @return Altitude reference state.
	 */
	AltitudeReferenceState getAltitudeReference() ਅਚਲ;

	/**
	 * @brief Get the Height Rate Setpoint directly from the velocity trajectory generator
	 *
	 * @return float direct height rate setpoint [m/s]
	 */
	ਦਸ਼ਮਲਵ get_ਉਚਾਈ_ਦਰ_Setpoint_ਸਿੱਧਾ() ਅਚਲ {ਵਾਪਸ ਉਚਾਈ_ਦਰ_setpoint_ਸਿੱਧਾ; }


ਨਿਜੀ:
	ਸਥਿਰ constexpr ਦਸ਼ਮਲਵ JERK_MAX = 1000.f;	///< Magnitude of the maximum jerk allowed [m/s³].

	// State
	VelocitySmoothing
	_alt_control_traj_generator;		///< Generates altitude rate and altitude setpoint trajectory when altitude is commanded.
	ManualVelocitySmoothingZ
	_velocity_control_traj_generator;	///< generates height rate trajectory when height rate is commanded
	ਦਸ਼ਮਲਵ ਉਚਾਈ_ਦਰ_setpoint_ਸਿੱਧਾ{NAN}; ///< generated direct height rate setpoint
};

ਵਰਗ TECSControl
{
ਜਨਤਕ:
	/**
	 * @brief The control parameters.
	 *
	 */
	ਬਣਤਰ Param {
		// Vehicle specific params
		ਦਸ਼ਮਲਵ max_sink_ਦਰ;			///< Maximum sink rate (with min throttle and max speed) [m/s].
		ਦਸ਼ਮਲਵ min_sink_ਦਰ;			///< Minimum sink rate (with min throttle and trim speed) [m/s].
		ਦਸ਼ਮਲਵ max_climb_ਦਰ;			///< Climb rate produced by max allowed throttle [m/s].
		ਦਸ਼ਮਲਵ vert_accel_ਹੱਦਬੰਦੀ;			///< Magnitude of the maximum vertical acceleration allowed [m/s²].
		ਦਸ਼ਮਲਵ equivalent_airspeed_trim;		///< Equivalent cruise airspeed for airspeed less mode [m/s].
		ਦਸ਼ਮਲਵ tas_min;				///< True airspeed demand lower limit [m/s].
		ਦਸ਼ਮਲਵ tas_stall;			///< True stall airspeed [m/s].
		ਦਸ਼ਮਲਵ tas_max;				///< True airspeed demand upper limit [m/s].
		ਦਸ਼ਮਲਵ tas_trim;				///< True airspeed trim [m/s].
		ਦਸ਼ਮਲਵ pitch_max;			///< Maximum absolute pitch angle setpoint [rad].
		ਦਸ਼ਮਲਵ pitch_min;			///< Minimum absolute pitch angle setpoint [rad].
		ਦਸ਼ਮਲਵ level_flight_pitch;		///< Pitch angle in level flight at trim airspeed, fed forward at zero climb angle [rad].
		ਦਸ਼ਮਲਵ throttle_trim;		///< Normalized throttle required to fly level at calibrated airspeed setpoint [0,1]
		ਦਸ਼ਮਲਵ throttle_max;			///< Normalized throttle upper limit.
		ਦਸ਼ਮਲਵ throttle_min;			///< Normalized throttle lower limit.

		// Altitude control param
		ਦਸ਼ਮਲਵ altitude_error_gain;		///< Altitude error inverse time constant [1/s].
		ਦਸ਼ਮਲਵ altitude_setpoint_gain_ff;	///< Gain from altitude demand derivative to demanded climb rate.

		// Airspeed control param
		ਦਸ਼ਮਲਵ airspeed_error_gain;				///< Airspeed error inverse time constant [1/s].

		// Energy control param
		ਦਸ਼ਮਲਵ ste_ਦਰ_ਸਮਾਂ_const;				///< Filter time constant for specific total energy rate (damping path) [s].
		ਦਸ਼ਮਲਵ seb_ਦਰ_ff;					///< Specific energy balance rate feedforward gain.

		// Pitch control param
		ਦਸ਼ਮਲਵ pitch_ਰਫਤਾਰ_ਭਾਰ;				///< Speed control weighting used by pitch demand calculation.
		ਦਸ਼ਮਲਵ integrator_gain_pitch;				///< Integrator gain used by the pitch demand calculation.
		ਦਸ਼ਮਲਵ pitch_damping_gain;				///< Damping gain of the pitch demand calculation [s].

		// Throttle control param
		ਦਸ਼ਮਲਵ integrator_gain_throttle;				///< Integrator gain used by the throttle demand calculation.
		ਦਸ਼ਮਲਵ throttle_damping_gain;				///< Damping gain of the throttle demand calculation [s].
		ਦਸ਼ਮਲਵ throttle_slewrate;				///< Throttle demand slew rate limit [1/s].

		ਦਸ਼ਮਲਵ ਲੱਦੋ_ਗੁਣਕ_correction;				///< Gain from normal load factor increase to total energy rate demand [m²/s³].
		ਦਸ਼ਮਲਵ ਲੱਦੋ_ਗੁਣਕ;					///< Additional normal load factor.

		ਦਸ਼ਮਲਵ fast_descend;
	};

	/**
	 * @brief The debug output
	 *
	 */
	ਬਣਤਰ DebugOutput {
		ਦਸ਼ਮਲਵ altitude_ਦਰ_control;			///< Altitude rate setpoint from altitude control loop [m/s].
		ਦਸ਼ਮਲਵ true_airspeed_derivative_control;		///< Airspeed rate setpoint from airspeed control loop [m/s²].
		ਦਸ਼ਮਲਵ ਕੁੱਲ_energy_ਦਰ_estimate;		///< Total energy rate estimate [m²/s³].
		ਦਸ਼ਮਲਵ ਕੁੱਲ_energy_ਦਰ_sp;			///< Total energy rate setpoint [m²/s³].
		ਦਸ਼ਮਲਵ energy_balance_ਦਰ_estimate;		///< Energy balance rate estimate [m²/s³].
		ਦਸ਼ਮਲਵ energy_balance_ਦਰ_sp;			///< Energy balance rate setpoint [m²/s³].
		ਦਸ਼ਮਲਵ pitch_integrator;				///< Pitch control integrator state, a correction added to the pitch setpoint [rad].
		ਦਸ਼ਮਲਵ throttle_integrator;			///< Throttle control integrator state [-].
	};

	/**
	 * @brief Given setpoint to control.
	 *
	 */
	ਬਣਤਰ Setpoint {
		TECSAltitudeReferenceModel::AltitudeReferenceState altitude_reference;	///< Altitude/height rate reference.
		ਦਸ਼ਮਲਵ altitude_ਦਰ_setpoint_ਸਿੱਧਾ;					///< Direct height rate setpoint.
		ਦਸ਼ਮਲਵ tas_setpoint;						///< True airspeed setpoint.
	};

	/**
	 * @brief Given current measurement from the UAS.
	 *
	 */
	ਬਣਤਰ ਨਿਵੇਸ਼_2 {
		ਦਸ਼ਮਲਵ altitude;		///< Current altitude amsl of the UAS [m].
		ਦਸ਼ਮਲਵ altitude_ਦਰ;	///< Current altitude rate of the UAS [m/s].
		ਦਸ਼ਮਲਵ tas;		///< Current true airspeed of the UAS [m/s].
		ਦਸ਼ਮਲਵ tas_ਦਰ;		///< Current true airspeed rate of the UAS [m/s²].
	};

	/**
	 * @brief Control flags.
	 *
	 */
	ਬਣਤਰ Flag {
		ਤਾਰਕਿਕ airspeed_enabled;			///< Flag if the airspeed sensor is enabled.
		ਤਾਰਕਿਕ detect_underspeed_enabled;		///< Flag if underspeed detection is enabled.
	};
ਜਨਤਕ:
	TECSControl() = ਮੂਲ;
	~TECSControl() = ਮੂਲ;
	/**
	 * @brief Initialization of the state.
	 *
	 * @param[in] setpoint is the current setpoint struct.
	 * @param[in] input is the current input measurements.
	 * @param[in] param is the current parameter set.
	 * @param[in] flag is the current activated flags.
	 */
	ਖਾਲੀ initialize(ਅਚਲ Setpoint &setpoint, ਅਚਲ ਨਿਵੇਸ਼_2 &ਨਿਵੇਸ਼, ਅਚਲ Param &param, ਅਚਲ Flag &flag);
	/**
	 * @brief Update state and output.
	 *
	 * @param[in] dt is the update time interval in [s].
	 * @param[in] setpoint is the current setpoint struct.
	 * @param[in] input is the current input measurements.
	 * @param[in] param is the current parameter set.
	 * @param[in] flag is the current activated flags.
	 */
	ਖਾਲੀ update(ਦਸ਼ਮਲਵ dt, ਅਚਲ Setpoint &setpoint, ਅਚਲ ਨਿਵੇਸ਼_2 &ਨਿਵੇਸ਼, ਅਚਲ Param &param, ਅਚਲ Flag &flag);
	/**
	 * @brief Reset the control loop integrals.
	 *
	 */
	ਖਾਲੀ resetIntegrals();
	/**
	 * @brief Get the underspeed ratio.
	 *
	 * @return Ratio of detected underspeed [0,1].
	 */
	ਦਸ਼ਮਲਵ get_Underspeed_ਅਨੁਪਾਤ() ਅਚਲ {ਵਾਪਸ ਅਨੁਪਾਤ_undersped;}
	/**
	 * @brief Get the throttle setpoint.
	 *
	 * @return throttle setpoint.
	 */
	ਦਸ਼ਮਲਵ getThrottleSetpoint() ਅਚਲ {ਵਾਪਸ _throttle_setpoint;}
	/**
	 * @brief Get the pitch setpoint.
	 *
	 * @return The commanded absolute pitch angle [rad].
	 */
	ਦਸ਼ਮਲਵ getPitchSetpoint() ਅਚਲ {ਵਾਪਸ _pitch_setpoint;}
	/**
	 * @brief Get the Debug Output
	 *
	 * @return the debug output struct.
	 */
	ਅਚਲ DebugOutput &getDebugOutput() ਅਚਲ { ਵਾਪਸ _debug_output; }

ਨਿਜੀ:
	// Allows the regression test to pre-corrupt _pitch_integ_state and verify the
	// safety reset in _calcPitchControlUpdate, which the public update() path cannot
	// reach on its own because the integrator-input guard rejects bad input first.
	FRIEND_TEST(TECSControlTest, PitchIntegratorResetsCorruptedState);

	/**
	 * @brief Specific total energy rate limit.
	 *
	 */
	ਬਣਤਰ STERate_ਹੱਦਬੰਦੀ {
		ਦਸ਼ਮਲਵ STE_ਦਰ_max;	///< Maximum specific total energy rate limit [m²/s³].
		ਦਸ਼ਮਲਵ STE_ਦਰ_min;	///< Minimal specific total energy rate limit [m²/s³].
	};

	/**
	 * @brief Control values.
	 * setpoint and current state estimate value as input to a controller.
	 *
	 */
	ਬਣਤਰ Control_ਮੁੱਲਾਂ {
		ਦਸ਼ਮਲਵ setpoint;		///< Control setpoint.
		ਦਸ਼ਮਲਵ estimate;		///< Control estimate of current state value.
	};

	/**
	 * @brief Calculated specific energy rates.
	 *
	 */
	ਬਣਤਰ SpecificEnergyRates {
		Control_ਮੁੱਲਾਂ ske_ਦਰ;	///< Specific kinetic energy rate [m²/s³].
		Control_ਮੁੱਲਾਂ spe_ਦਰ;	///< Specific potential energy rate [m²/s³].
	};

	/**
	 * @brief Altitude rate and airspeed setpoints from the outer loops, from which the energy rate setpoints follow.
	 *
	 */
	ਬਣਤਰ OuterLoopSetpoints {
		ਦਸ਼ਮਲਵ altitude_ਦਰ_setpoint;	///< Controlled altitude rate setpoint [m/s].
		ਦਸ਼ਮਲਵ tas_ਦਰ_setpoint;	///< Controlled true airspeed rate setpoint [m/s²].
	};

	/**
	 * @brief Weight factors for specific energy.
	 *
	 */
	ਬਣਤਰ SpecificEnergyWeighting {
		ਦਸ਼ਮਲਵ spe_weighting;	///< Specific potential energy weight.
		ਦਸ਼ਮਲਵ ske_weighting;	///< Specific kinetic energy weight.
	};

ਨਿਜੀ:
	/**
	 * @brief Get control error from setpoint and estimate
	 *
	 * @param val is the current control setpoint and estimate.
	 * @return error value
	 */
	ਸਥਿਰ constexpr ਦਸ਼ਮਲਵ _getControlError(TECSControl::Control_ਮੁੱਲਾਂ val) {ਵਾਪਸ (val.setpoint - val.estimate);}
	/**
	 * @brief Calculate specific total energy rate limits.
	 *
	 * @param[in] param are the control parameters.
	 * @return Specific total energy rate limits in [m²/s³].
	 */
	STERate_ਹੱਦਬੰਦੀ calculate_ਕੁੱਲ_Energy_ਦਰ_ਹੱਦਬੰਦੀ(ਅਚਲ Param &param) ਅਚਲ;
	/**
	 * @brief Gain from specific total energy rate error to throttle.
	 *
	 * @param[in] limit is the specific total energy rate limits in [m²/s³].
	 * @return throttle per specific total energy rate [s³/m²].
	 */
	ਸਥਿਰ ਦਸ਼ਮਲਵ ste_ਦਰ_ਮੰਜ਼ਿਲ_Throttle(ਅਚਲ STERate_ਹੱਦਬੰਦੀ &ਹੱਦਬੰਦੀ_2) { ਵਾਪਸ 1.0f / (ਹੱਦਬੰਦੀ_2.STE_ਦਰ_max - ਹੱਦਬੰਦੀ_2.STE_ਦਰ_min); }
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
	ਦਸ਼ਮਲਵ _calcAirspeedControlOutput(ਅਚਲ Setpoint &setpoint, ਅਚਲ ਨਿਵੇਸ਼_2 &ਨਿਵੇਸ਼, ਅਚਲ STERate_ਹੱਦਬੰਦੀ &ਹੱਦਬੰਦੀ_2,
					 ਅਚਲ Param &param, ਅਚਲ Flag &flag) ਅਚਲ;
	/**
	 * @brief calculate altitude control proportional output.
	 *
	 * @param setpoint is the control setpoints.
	 * @param input is the current input measurement of the UAS.
	 * @param param is the control parameters.
	 * @return controlled altitude rate setpoint in [m/s].
	 */
	ਦਸ਼ਮਲਵ _calcAltitudeControlOutput(ਅਚਲ Setpoint &setpoint, ਅਚਲ ਨਿਵੇਸ਼_2 &ਨਿਵੇਸ਼, ਅਚਲ Param &param) ਅਚਲ;
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
	SpecificEnergyRates _updateOuterLoops(ਅਚਲ Setpoint &setpoint, ਅਚਲ ਨਿਵੇਸ਼_2 &ਨਿਵੇਸ਼, ਅਚਲ STERate_ਹੱਦਬੰਦੀ &ਹੱਦਬੰਦੀ_2,
					      ਅਚਲ Param &param, ਅਚਲ Flag &flag);
	/**
	 * @brief Calculate specific energy rates.
	 *
	 * @param control_setpoint is the controlled altitude and airspeed rate setpoints.
	 * @param input is the current input measurement of the UAS.
	 * @return Specific energy rates in [m²/s³].
	 */
	SpecificEnergyRates _calcSpecificEnergyRates(ਅਚਲ OuterLoopSetpoints &control_setpoint, ਅਚਲ ਨਿਵੇਸ਼_2 &ਨਿਵੇਸ਼) ਅਚਲ;
	/**
	 * @brief Detect underspeed.
	 *
	 * @param input is the current input measurement of the UAS.
	 * @param param is the control parameters.
	 * @param flag is the control flags.
	 */
	ਖਾਲੀ _detectUnderspeed(ਅਚਲ ਨਿਵੇਸ਼_2 &ਨਿਵੇਸ਼, ਅਚਲ Param &param, ਅਚਲ Flag &flag);
	/**
	 * @brief Update specific energy balance weights.
	 *
	 * @param param is the control parameters.
	 * @param flag is the control flags.
	 * @return Weights used for the specific energy balance.
	 */
	SpecificEnergyWeighting update_ਰਫਤਾਰ_Altitude_Weights(ਅਚਲ Param &param, ਅਚਲ Flag &flag);
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
	ਦਸ਼ਮਲਵ calc_Pitch_Control_Ske_ਦਰ_Setpoint(ਅਚਲ Setpoint &setpoint, ਅਚਲ ਨਿਵੇਸ਼_2 &ਨਿਵੇਸ਼, ਅਚਲ STERate_ਹੱਦਬੰਦੀ &ਹੱਦਬੰਦੀ_2,
					       ਅਚਲ Param &param, ਅਚਲ Flag &flag, ਅਚਲ SpecificEnergyWeighting &ਭਾਰ) ਅਚਲ;
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
	SpecificEnergyRates _calcPitchControlEnergyRates(ਅਚਲ Setpoint &setpoint, ਅਚਲ ਨਿਵੇਸ਼_2 &ਨਿਵੇਸ਼,
			ਅਚਲ STERate_ਹੱਦਬੰਦੀ &ਹੱਦਬੰਦੀ_2, ਅਚਲ SpecificEnergyRates &specific_energy_rates, ਅਚਲ Param &param,
			ਅਚਲ Flag &flag, ਅਚਲ SpecificEnergyWeighting &ਭਾਰ) ਅਚਲ;
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
	ਖਾਲੀ _calcPitchControl(ਦਸ਼ਮਲਵ dt, ਅਚਲ Setpoint &setpoint, ਅਚਲ ਨਿਵੇਸ਼_2 &ਨਿਵੇਸ਼, ਅਚਲ STERate_ਹੱਦਬੰਦੀ &ਹੱਦਬੰਦੀ_2,
			       ਅਚਲ SpecificEnergyRates &specific_energy_ਦਰ, ਅਚਲ Param &param, ਅਚਲ Flag &flag);

	/**
	 * @brief Calculate pitch control specific energy balance rates.
	 *
	 * @param weight is the weighting use of the potential and kinetic energy.
	 * @param specific_energy_rate is the specific energy rates in [m²/s³].
	 * @return specific energy balance rate values in [m²/s³].
	 */
	Control_ਮੁੱਲਾਂ calc_Pitch_Control_Seb_ਦਰ(ਅਚਲ SpecificEnergyWeighting &ਭਾਰ,
					       ਅਚਲ SpecificEnergyRates &specific_energy_ਦਰ) ਅਚਲ;

	/**
	 * @brief Calculate the pitch control update function.
	 * Update the states of the pitch control (pitch integrator).
	 *
	 * @param dt is the update time interval in [s].
	 * @param input is the current input measurement of the UAS.
	 * @param seb_rate is the specific energy balance rate in [m²/s³].
	 * @param param is the control parameters.
	 */
	ਖਾਲੀ _calcPitchControlUpdate(ਦਸ਼ਮਲਵ dt, ਅਚਲ ਨਿਵੇਸ਼_2 &ਨਿਵੇਸ਼, ਅਚਲ Control_ਮੁੱਲਾਂ &seb_ਦਰ, ਅਚਲ Param &param);

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
	ਸਥਿਰ ਦਸ਼ਮਲਵ climb_Angle_ਮੰਜ਼ਿਲ_Seb_ਦਰ(ਅਚਲ ਨਿਵੇਸ਼_2 &ਨਿਵੇਸ਼, ਅਚਲ Param &param);

	/**
	 * @brief Calculate the pitch control output function.
	 *
	 * @param input is the current input measurement of the UAS.
	 * @param seb_rate is the specific energy balance rate in [m²/s³].
	 * @param param is the control parameters.
	 * @return absolute pitch setpoint [rad].
	 */
	ਦਸ਼ਮਲਵ _calcPitchControlOutput(ਅਚਲ ਨਿਵੇਸ਼_2 &ਨਿਵੇਸ਼, ਅਚਲ Control_ਮੁੱਲਾਂ &seb_ਦਰ, ਅਚਲ Param &param) ਅਚਲ;

	/**
	 * @brief Update controlled throttle setpoint.
	 *
	 * @param dt is the update time interval in [s].
	 * @param limit is the specific total energy rate limits in [m²/s³].
	 * @param specific_energy_rate is the calculated specific energy.
	 * @param param is the control parameters.
	 * @param flag is the control flags.
	 */
	ਖਾਲੀ _calcThrottleControl(ਦਸ਼ਮਲਵ dt, ਅਚਲ STERate_ਹੱਦਬੰਦੀ &ਹੱਦਬੰਦੀ_2, ਅਚਲ SpecificEnergyRates &specific_energy_ਦਰ,
				  ਅਚਲ Param &param, ਅਚਲ Flag &flag);

	/**
	 * @brief Calculate throttle control specific total energy
	 *
	 * @param limit is the specific total energy rate limits in [m²/s³].
	 * @param specific_energy_rate is the specific energy rates in [m²/s³].
	 * @param param is the control parameters.
	 * @return specific total energy rate values in [m²/s³]
	 */
	Control_ਮੁੱਲਾਂ calc_Throttle_Control_Ste_ਦਰ(ਅਚਲ STERate_ਹੱਦਬੰਦੀ &ਹੱਦਬੰਦੀ_2, ਅਚਲ SpecificEnergyRates &specific_energy_ਦਰ,
			ਅਚਲ Param &param) ਅਚਲ;

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
	ਖਾਲੀ _calcThrottleControlUpdate(ਦਸ਼ਮਲਵ dt, ਅਚਲ STERate_ਹੱਦਬੰਦੀ &ਹੱਦਬੰਦੀ_2, ਅਚਲ Control_ਮੁੱਲਾਂ &ste_ਦਰ, ਅਚਲ Param &param,
					ਅਚਲ Flag &flag);

	/**
	 * @brief Calculate the throttle control output function.
	 *
	 * @param limit is the specific total energy rate limits in [m²/s³].
	 * @param ste_rate is the specific total energy rates in [m²/s³].
	 * @param param is the control parameters.
	 * @param flag is the control flags.
	 * @return throttle setpoint in [0,1].
	 */
	ਦਸ਼ਮਲਵ _calcThrottleControlOutput(ਅਚਲ STERate_ਹੱਦਬੰਦੀ &ਹੱਦਬੰਦੀ_2, ਅਚਲ Control_ਮੁੱਲਾਂ &ste_ਦਰ, ਅਚਲ Param &param,
					 ਅਚਲ Flag &flag) ਅਚਲ;

ਨਿਜੀ:
	// State
	AlphaFilter<ਦਸ਼ਮਲਵ> ste_ਦਰ_estimate_filter;		///< Low pass filter for the specific total energy rate.
	ਦਸ਼ਮਲਵ _pitch_integ_state{0.0f};				///< Pitch integrator state [rad].
	ਦਸ਼ਮਲਵ _fast_descend_prev{0.0f};				///< Fast descend ratio of the last update [0,1].
	ਦਸ਼ਮਲਵ _throttle_integ_state{0.0f};			///< Throttle integrator state [-].

	// Output
	DebugOutput _debug_output;				///< Debug output.
	ਦਸ਼ਮਲਵ _pitch_setpoint{0.0f};				///< Controlled absolute pitch setpoint [rad].
	ਦਸ਼ਮਲਵ _throttle_setpoint{0.0f};				///< Controlled throttle setpoint [0,1].
	ਦਸ਼ਮਲਵ ਅਨੁਪਾਤ_undersped{0.0f};				///< A continuous representation of how "undersped" the TAS is [0,1]
};

ਵਰਗ TECS
{
ਜਨਤਕ:
	ਬਣਤਰ DebugOutput {
		TECSControl::DebugOutput control;
		ਦਸ਼ਮਲਵ true_airspeed_sp;
		ਦਸ਼ਮਲਵ true_airspeed_filtered;
		ਦਸ਼ਮਲਵ true_airspeed_derivative;
		ਦਸ਼ਮਲਵ altitude_reference;
		ਦਸ਼ਮਲਵ ਉਚਾਈ_ਦਰ_reference;
		ਦਸ਼ਮਲਵ ਉਚਾਈ_ਦਰ_ਸਿੱਧਾ;
		ਦਸ਼ਮਲਵ fast_descend;
	};

	/**
	 * @brief Vehicle state, setpoints and limits for one control update.
	 *
	 */
	ਬਣਤਰ ਨਿਵੇਸ਼_2 {
		ਦਸ਼ਮਲਵ altitude;				///< Current altitude amsl [m].
		ਦਸ਼ਮਲਵ altitude_ਦਰ;			///< Current altitude rate [m/s].
		ਦਸ਼ਮਲਵ altitude_setpoint;		///< Altitude setpoint amsl [m], NAN if not set.
		ਦਸ਼ਮਲਵ altitude_ਦਰ_setpoint{NAN};	///< Altitude rate setpoint [m/s], NAN if not set.
		ਦਸ਼ਮਲਵ equivalent_airspeed;		///< Current equivalent airspeed [m/s].
		ਦਸ਼ਮਲਵ ਰਫਤਾਰ_deriv_forward;		///< Current forward acceleration, used as true airspeed rate [m/s²].
		ਦਸ਼ਮਲਵ equivalent_airspeed_setpoint;	///< Equivalent airspeed setpoint [m/s].
		ਦਸ਼ਮਲਵ eas_ਮੰਜ਼ਿਲ_tas;			///< Conversion factor from equivalent to true airspeed [-].
		ਦਸ਼ਮਲਵ throttle_min;			///< Normalized throttle lower limit.
		ਦਸ਼ਮਲਵ throttle_max;			///< Normalized throttle upper limit.
		ਦਸ਼ਮਲਵ throttle_trim;			///< Normalized throttle required to fly level at the airspeed setpoint.
		ਦਸ਼ਮਲਵ pitch_min;			///< Minimum absolute pitch angle setpoint [rad].
		ਦਸ਼ਮਲਵ pitch_max;			///< Maximum absolute pitch angle setpoint [rad].
		ਦਸ਼ਮਲਵ ਨਿਸ਼ਾਨਾ_climbrate;			///< Climb rate the reference model targets [m/s].
		ਦਸ਼ਮਲਵ ਨਿਸ਼ਾਨਾ_sinkrate;			///< Sink rate the reference model targets [m/s].
	};

ਜਨਤਕ:
	TECS() = ਮੂਲ;
	~TECS() = ਮੂਲ;

	// no copy, assignment, move, move assignment
	TECS(ਅਚਲ TECS &) = ਮਿਟਾਓ;
	TECS &ਸੰਚਾਲਕ=(ਅਚਲ TECS &) = ਮਿਟਾਓ;
	TECS(TECS &&) = ਮਿਟਾਓ;
	TECS &ਸੰਚਾਲਕ=(TECS &&) = ਮਿਟਾਓ;

	ਅਚਲ DebugOutput &get_ਸਥਿਤੀ() ਅਚਲ { ਵਾਪਸ debug_ਸਥਿਤੀ; }

	/**
	 * Set the airspeed enable state
	 *
	 * @param[in] enabled is true if the airspeed measurement is used for control.
	 */
	ਖਾਲੀ enable_airspeed(ਤਾਰਕਿਕ enabled) { _control_flag.airspeed_enabled = enabled; }

	/**
	 * @brief Update the control loop calculations
	 *
	 * All pitch angles, including the pitch limits and the resulting pitch setpoint, are absolute: the angle of the
	 * body x-axis above the horizon.
	 *
	 * @param[in] input is the current vehicle state, setpoints and limits.
	 */
	ਖਾਲੀ update(ਅਚਲ ਨਿਵੇਸ਼_2 &ਨਿਵੇਸ਼);

	ਖਾਲੀ resetIntegrals()
	{
		_control.resetIntegrals();
	}

	ਖਾਲੀ ਸੈੱਟ_detect_underspeed_enabled(ਤਾਰਕਿਕ enabled) { _control_flag.detect_underspeed_enabled = enabled; }

	// setters for parameters

	ਖਾਲੀ ਸੈੱਟ_integrator_gain_throttle(ਦਸ਼ਮਲਵ gain) { _control_param.integrator_gain_throttle = gain;}
	ਖਾਲੀ ਸੈੱਟ_integrator_gain_pitch(ਦਸ਼ਮਲਵ gain) { _control_param.integrator_gain_pitch = gain; }

	ਖਾਲੀ ਸੈੱਟ_max_sink_ਦਰ(ਦਸ਼ਮਲਵ max_sink_ਦਰ) { _control_param.max_sink_ਦਰ = max_sink_ਦਰ; _reference_param.max_sink_ਦਰ = max_sink_ਦਰ; }
	ਖਾਲੀ ਸੈੱਟ_min_sink_ਦਰ(ਦਸ਼ਮਲਵ min_sink_ਦਰ) { _control_param.min_sink_ਦਰ = min_sink_ਦਰ; }
	ਖਾਲੀ ਸੈੱਟ_max_climb_ਦਰ(ਦਸ਼ਮਲਵ climb_ਦਰ) { _control_param.max_climb_ਦਰ = climb_ਦਰ; _reference_param.max_climb_ਦਰ = climb_ਦਰ; }

	ਖਾਲੀ ਸੈੱਟ_altitude_ਦਰ_ff(ਦਸ਼ਮਲਵ altitude_ਦਰ_ff) { _control_param.altitude_setpoint_gain_ff = altitude_ਦਰ_ff; }
	ਖਾਲੀ ਸੈੱਟ_altitude_error_ਸਮਾਂ_constant(ਦਸ਼ਮਲਵ ਸਮਾਂ_const) { _control_param.altitude_error_gain = 1.0f / math::max(ਸਮਾਂ_const, 0.1f); }
	ਖਾਲੀ ਸੈੱਟ_fast_descend_altitude_error(ਦਸ਼ਮਲਵ altitude_error) { _fast_descend_alt_err = altitude_error; }

	ਖਾਲੀ ਸੈੱਟ_equivalent_airspeed_min(ਦਸ਼ਮਲਵ airspeed) { _equivalent_airspeed_min = airspeed; }
	ਖਾਲੀ ਸੈੱਟ_equivalent_airspeed_stall(ਦਸ਼ਮਲਵ airspeed) { _equivalent_airspeed_stall = airspeed; }
	ਖਾਲੀ ਸੈੱਟ_equivalent_airspeed_max(ਦਸ਼ਮਲਵ airspeed) { _equivalent_airspeed_max = airspeed; }
	ਖਾਲੀ ਸੈੱਟ_equivalent_airspeed_trim(ਦਸ਼ਮਲਵ airspeed) { _control_param.equivalent_airspeed_trim = airspeed; _airspeed_filter_param.equivalent_airspeed_trim = airspeed; }

	ਖਾਲੀ ਸੈੱਟ_pitch_damping(ਦਸ਼ਮਲਵ damping) { _control_param.pitch_damping_gain = damping; }
	ਖਾਲੀ ਸੈੱਟ_vertical_accel_ਹੱਦਬੰਦੀ(ਦਸ਼ਮਲਵ ਹੱਦਬੰਦੀ_2) { _reference_param.vert_accel_ਹੱਦਬੰਦੀ = ਹੱਦਬੰਦੀ_2; _control_param.vert_accel_ਹੱਦਬੰਦੀ = ਹੱਦਬੰਦੀ_2; }

	ਖਾਲੀ ਸੈੱਟ_ਰਫਤਾਰ_ਭਾਰ(ਦਸ਼ਮਲਵ ਭਾਰ) { _control_param.pitch_ਰਫਤਾਰ_ਭਾਰ = ਭਾਰ; }
	ਖਾਲੀ ਸੈੱਟ_airspeed_error_ਸਮਾਂ_constant(ਦਸ਼ਮਲਵ ਸਮਾਂ_const) { _control_param.airspeed_error_gain = 1.0f / math::max(ਸਮਾਂ_const, 0.1f); }

	ਖਾਲੀ ਸੈੱਟ_throttle_damp(ਦਸ਼ਮਲਵ throttle_damp) { _control_param.throttle_damping_gain = throttle_damp; }
	ਖਾਲੀ ਸੈੱਟ_throttle_slewrate(ਦਸ਼ਮਲਵ slewrate) { _control_param.throttle_slewrate = slewrate; }

	ਖਾਲੀ ਸੈੱਟ_roll_throttle_compensation(ਦਸ਼ਮਲਵ compensation) { _control_param.ਲੱਦੋ_ਗੁਣਕ_correction = compensation; }
	ਖਾਲੀ ਸੈੱਟ_ਲੱਦੋ_ਗੁਣਕ(ਦਸ਼ਮਲਵ ਲੱਦੋ_ਗੁਣਕ) { _control_param.ਲੱਦੋ_ਗੁਣਕ = ਲੱਦੋ_ਗੁਣਕ; }

	ਖਾਲੀ ਸੈੱਟ_ste_ਦਰ_ਸਮਾਂ_const(ਦਸ਼ਮਲਵ ਸਮਾਂ_const) { _control_param.ste_ਦਰ_ਸਮਾਂ_const = ਸਮਾਂ_const; }

	ਖਾਲੀ ਸੈੱਟ_seb_ਦਰ_ff_gain(ਦਸ਼ਮਲਵ ff_gain) { _control_param.seb_ਦਰ_ff = ff_gain; }

	/**
	 * Set the pitch angle in level flight at trim airspeed [rad], fed forward as the pitch at zero climb angle
	 */
	ਖਾਲੀ ਸੈੱਟ_level_flight_pitch(ਦਸ਼ਮਲਵ pitch) { _control_param.level_flight_pitch = pitch; }

	/**
	 * Handle the altitude reset
	 *
	 * If the estimation system resets the height in one discrete step this
	 * will gracefully even out the reset over time.
	 *
	 * @param[in] altitude is the altitude amsl after the reset [m].
	 * @param[in] altitude_rate is the current altitude rate [m/s].
	 */
	ਖਾਲੀ handle_alt_ਕਦਮ(ਦਸ਼ਮਲਵ altitude, ਦਸ਼ਮਲਵ altitude_ਦਰ)
	{
		TECSAltitudeReferenceModel::AltitudeReferenceState init_state{ .alt = altitude,
				.alt_ਦਰ = altitude_ਦਰ};

		// reset altitude reference model.
		_altitude_reference_model.initialize(init_state);
	}

	/**
	 * Returns the absolute pitch setpoint [rad]
	 */
	ਦਸ਼ਮਲਵ get_pitch_setpoint() ਅਚਲ {ਵਾਪਸ _control.getPitchSetpoint();}
	ਦਸ਼ਮਲਵ get_throttle_setpoint() ਅਚਲ {ਵਾਪਸ _control.getThrottleSetpoint();}

	/**
	 * Returns the altitude tracking time constant
	 */
	ਦਸ਼ਮਲਵ get_altitude_error_ਸਮਾਂ_constant() ਅਚਲ { ਵਾਪਸ 1.0f / _control_param.altitude_error_gain; }

	ਦਸ਼ਮਲਵ get_underspeed_ਅਨੁਪਾਤ() ਅਚਲ { ਵਾਪਸ _control.get_Underspeed_ਅਨੁਪਾਤ(); }

ਨਿਜੀ:
	/**
	 * @brief Initialize the control parameters
	 *
	 * @param[in] input is the current vehicle state, setpoints and limits.
	 */
	ਖਾਲੀ initControlParams(ਅਚਲ ਨਿਵੇਸ਼_2 &ਨਿਵੇਸ਼);

	/**
	 * @brief calculate true airspeed setpoint
	 *
	 * Calculate true airspeed setpoint based on input and fast descend ratio
	 *
	 * @param eas_to_tas is the eas to tas conversion factor
	 * @param eas_setpoint is the desired equivalent airspeed setpoint [m/s]
	 * @return true airspeed setpoint[m/s]
	 */
	ਦਸ਼ਮਲਵ calcTrueAirspeedSetpoint(ਦਸ਼ਮਲਵ eas_ਮੰਜ਼ਿਲ_tas, ਦਸ਼ਮਲਵ eas_setpoint);

	/**
	 * @brief Initialize the control loop
	 *
	 * @param[in] input is the current vehicle state, setpoints and limits.
	 */
	ਖਾਲੀ initialize(ਅਚਲ ਨਿਵੇਸ਼_2 &ਨਿਵੇਸ਼);

	TECSControl 			_control;			///< Control submodule.
	TECSAirspeedFilter 		_airspeed_filter;		///< Airspeed filter submodule.
	TECSAltitudeReferenceModel 	_altitude_reference_model;	///< Setpoint reference model submodule.

	hrt_abstime _update_timestamp{0};				///< last timestamp of the update function call.

	ਦਸ਼ਮਲਵ _equivalent_airspeed_min{10.0f};				///< equivalent airspeed demand lower limit (m/sec)
	ਦਸ਼ਮਲਵ _equivalent_airspeed_stall{7.0f};				///< equivalent stall airspeed (m/sec)
	ਦਸ਼ਮਲਵ _equivalent_airspeed_max{20.0f};				///< equivalent airspeed demand upper limit (m/sec)
	ਦਸ਼ਮਲਵ _fast_descend_alt_err{-1.f};	 				///< Altitude difference between current altitude to altitude setpoint needed to descend with higher airspeed [m].
	ਦਸ਼ਮਲਵ _fast_descend{0.f};					///< Value for fast descend in [0,1]. continuous value used to flatten the high speed value out when close to target altitude.
	hrt_abstime _enabled_fast_descend_timestamp{0U};		///< timestamp at activation of fast descend mode

	ਸਥਿਰ constexpr ਦਸ਼ਮਲਵ DT_MIN = 0.001f;				///< minimum allowed value of _dt (sec)
	ਸਥਿਰ constexpr ਦਸ਼ਮਲਵ DT_MAX = 1.0f;				///< max value of _dt allowed before a filter state reset is performed (sec)
	ਸਥਿਰ constexpr hrt_abstime FAST_DESCEND_RAMP_ਉੱਪਰ_ਸਮਾਂ = 2_s; 	///< Ramp up time until fast descend is fully engaged
	ਸਥਿਰ constexpr ਦਸ਼ਮਲਵ FAST_DESCEND_ਉਚਾਈ_MARGIN = 5.f;	///< Height above setpoint where fast descend ends [m]

	DebugOutput debug_ਸਥਿਤੀ{};

	// Params
	/// Airspeed filter parameters.
	TECSAirspeedFilter::Param _airspeed_filter_param{
		.equivalent_airspeed_trim = 15.0f,
	};
	/// Reference model parameters.
	TECSAltitudeReferenceModel::Param _reference_param{
		.ਨਿਸ਼ਾਨਾ_climbrate = 2.0f,
		.ਨਿਸ਼ਾਨਾ_sinkrate = 2.0f,
		.vert_accel_ਹੱਦਬੰਦੀ = 0.0f,
		.max_climb_ਦਰ = 2.0f,
		.max_sink_ਦਰ = 2.0f,
	};
	/// Control parameters.
	TECSControl::Param _control_param{
		.max_sink_ਦਰ = 5.0f,
		.min_sink_ਦਰ = 2.0f,
		.max_climb_ਦਰ = 5.0f,
		.vert_accel_ਹੱਦਬੰਦੀ = 0.0f,
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
		.ste_ਦਰ_ਸਮਾਂ_const = 0.1f,
		.seb_ਦਰ_ff = 1.0f,
		.pitch_ਰਫਤਾਰ_ਭਾਰ = 1.0f,
		.integrator_gain_pitch = 0.0f,
		.pitch_damping_gain = 0.0f,
		.integrator_gain_throttle = 0.0f,
		.throttle_damping_gain = 0.0f,
		.throttle_slewrate = 0.0f,
		.ਲੱਦੋ_ਗੁਣਕ_correction = 0.0f,
		.ਲੱਦੋ_ਗੁਣਕ = 1.0f,
		.fast_descend = 0.f
	};

	TECSControl::Flag _control_flag{
		.airspeed_enabled = ਝੂਠ,
		.detect_underspeed_enabled = ਝੂਠ,
	};

	/**
	 * @brief Set fast descend value
	 *
	 * @param alt_setpoint is the altitude setpoint
	 * @param alt is the current altitude
	 */
	ਖਾਲੀ ਸੈੱਟ_Fast_Descend(ਦਸ਼ਮਲਵ alt_setpoint, ਦਸ਼ਮਲਵ alt);
};

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

#ಸೂಚನೆ once

#ಅನ್ತರ್ಭಾವ <drivers/drv_hrt.h>
#ಅನ್ತರ್ಭಾವ <mathlib/mathlib.h>
#ಅನ್ತರ್ಭಾವ <matrix/math.hpp>
#ಅನ್ತರ್ಭಾವ <lib/mathlib/math/filter/AlphaFilter.hpp>
#ಅನ್ತರ್ಭಾವ <motion_planning/VelocitySmoothing.hpp>
#ಅನ್ತರ್ಭಾವ <motion_planning/ManualVelocitySmoothingZ.hpp>

ಬಳಸಿ ನಾಮಾಕಾಶ ಸಮಯ_literals;

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ FRIEND_TEST // for gtest
#ವ್ಯಾಖ್ಯಾನಿಸು FRIEND_TEST(a, b)
#ಆದರೆ_ಅಂತ್ಯ

ವರ್ಗ TECSAirspeedFilter
{
ಸಾರ್ವಜನಿಕ:
	/**
	 * @brief State of the equivalent airspeed filter.
	 *
	 */
	ರಚನೆ AirspeedFilterState {
		ದಶಮಾಂಶ ವೇಗ;		///< speed of the air in EAS [m/s]
		ದಶಮಾಂಶ ವೇಗ_ದರ;	///< rate of speed of the air [m/s²]
	};

	/**
	 * @brief Parameters of the airspeed filter.
	 *
	 */
	ರಚನೆ Param {
		ದಶಮಾಂಶ equivalent_airspeed_trim; 	///< the trim value of the equivalent airspeed [m/s].
	};

	/**
	 * @brief Input, which will be filtered.
	 *
	 */
	ರಚನೆ ಒಳಹರಿವು_2 {
		ದಶಮಾಂಶ equivalent_airspeed; 	///< the measured equivalent airspeed in [m/s].
		ದಶಮಾಂಶ equivalent_airspeed_ದರ;	///< the measured rate of equivalent airspeed in [m/s²].
	};
ಸಾರ್ವಜನಿಕ:
	TECSAirspeedFilter() = ಪೂರ್ವನಿಯೋಜಿತ;
	~TECSAirspeedFilter() = ಪೂರ್ವನಿಯೋಜಿತ;
	/**
	 * @brief Initialize filter
	 *
	 * @param[in] equivalent_airspeed is the equivalent airspeed in [m/s].
	 * @param[in] equivalent_airspeed_trim is the equivalent airspeed trim (vehicle setting) in [m/s].
	 * @param[in] airspeed_sensor_available boolean if the airspeed sensor is available.
	 */
	ಶೂನ್ಯ initialize(ದಶಮಾಂಶ equivalent_airspeed, ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ equivalent_airspeed_trim,
			ಸ್ಥಿರಾಂಕ ತಾರ್ಕಿಕ airspeed_sensor_ಲಭ್ಯ);

	/**
	 * @brief Update filter
	 *
	 * @param[in] dt is the timestep in [s].
	 * @param[in] input are the raw measured values.
	 * @param[in] param are the filter parameters.
	 * @param[in] airspeed_sensor_available boolean if the airspeed sensor is available.
	 */
	ಶೂನ್ಯ update(ದಶಮಾಂಶ dt, ಸ್ಥಿರಾಂಕ ಒಳಹರಿವು_2 &ಒಳಹರಿವು, ಸ್ಥಿರಾಂಕ Param &param, ಸ್ಥಿರಾಂಕ ತಾರ್ಕಿಕ airspeed_sensor_ಲಭ್ಯ);

	/**
	 * @brief Get the filtered airspeed states.
	 *
	 * @return Current state of the airspeed filter.
	 */
	AirspeedFilterState getState() ಸ್ಥಿರಾಂಕ;

ಖಾಸಗಿ:
	// States
	AirspeedFilterState _airspeed_state{.ವೇಗ = 0.0f, .ವೇಗ_ದರ = 0.0f};	///< Complementary filter state

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
	ಸ್ಥಿರ constexpr ದಶಮಾಂಶ kKg00{1.91940287f};
	ಸ್ಥಿರ constexpr ದಶಮಾಂಶ kKg01{0.25925926f};
	ಸ್ಥಿರ constexpr ದಶಮಾಂಶ kKg10{2.11640212f};
	ಸ್ಥಿರ constexpr ದಶಮಾಂಶ kKg11{0.67179101f};
};

ವರ್ಗ TECSAltitudeReferenceModel
{
ಸಾರ್ವಜನಿಕ:
	/**
	 * @brief Altitude reference state.
	 *
	 */
	ರಚನೆ AltitudeReferenceState {
		ದಶಮಾಂಶ alt; 	///< Reference altitude amsl in [m].
		ದಶಮಾಂಶ alt_ದರ;	///< Reference altitude rate in [m/s].
	};

	/**
	 * @brief Parameters for the reference model.
	 *
	 */
	ರಚನೆ Param {
		ದಶಮಾಂಶ ಗುರಿ_climbrate;	///< The target climbrate in [m/s].
		ದಶಮಾಂಶ ಗುರಿ_sinkrate;	///< The target sinkrate in [m/s].
		ದಶಮಾಂಶ vert_accel_ಎಲ್ಲೆ;	///< Magnitude of the maximum vertical acceleration allowed [m/s²].
		ದಶಮಾಂಶ max_climb_ದರ;	///< Climb rate produced by max allowed throttle [m/s].
		ದಶಮಾಂಶ max_sink_ದರ;	///< Maximum sink rate (with min throttle, max speed) [m/s].
	};

ಸಾರ್ವಜನಿಕ:
	TECSAltitudeReferenceModel() = ಪೂರ್ವನಿಯೋಜಿತ;
	~TECSAltitudeReferenceModel() = ಪೂರ್ವನಿಯೋಜಿತ;

	/**
	 * @brief Initialize reference models.
	 *
	 * @param[in] state is the current altitude state of the vehicle.
	 */
	ಶೂನ್ಯ initialize(ಸ್ಥಿರಾಂಕ AltitudeReferenceState &state);

	/**
	 * @brief Update reference models.
	 *
	 * @param[in] dt is the update interval in [s].
	 * @param[in] setpoint are the desired setpoints.
	 * @param[in] altitude is the altitude amsl in [m].
	 * @param[in] height_rate is the current height rate in [m/s].
	 * @param[in] param are the reference model parameters.
	 */
	ಶೂನ್ಯ update(ದಶಮಾಂಶ dt, ಸ್ಥಿರಾಂಕ AltitudeReferenceState &setpoint, ದಶಮಾಂಶ altitude, ದಶಮಾಂಶ ಎತ್ತರ_ದರ, ಸ್ಥಿರಾಂಕ Param &param);

	/**
	 * @brief Get the current altitude reference of altitude reference model.
	 *
	 * @return Altitude reference state.
	 */
	AltitudeReferenceState getAltitudeReference() ಸ್ಥಿರಾಂಕ;

	/**
	 * @brief Get the Height Rate Setpoint directly from the velocity trajectory generator
	 *
	 * @return float direct height rate setpoint [m/s]
	 */
	ದಶಮಾಂಶ get_ಎತ್ತರ_ದರ_Setpoint_ನೇರ() ಸ್ಥಿರಾಂಕ {ಹಿಂತಿರುಗಿ ಎತ್ತರ_ದರ_setpoint_ನೇರ; }


ಖಾಸಗಿ:
	ಸ್ಥಿರ constexpr ದಶಮಾಂಶ JERK_MAX = 1000.f;	///< Magnitude of the maximum jerk allowed [m/s³].

	// State
	VelocitySmoothing
	_alt_control_traj_generator;		///< Generates altitude rate and altitude setpoint trajectory when altitude is commanded.
	ManualVelocitySmoothingZ
	_velocity_control_traj_generator;	///< generates height rate trajectory when height rate is commanded
	ದಶಮಾಂಶ ಎತ್ತರ_ದರ_setpoint_ನೇರ{NAN}; ///< generated direct height rate setpoint
};

ವರ್ಗ TECSControl
{
ಸಾರ್ವಜನಿಕ:
	/**
	 * @brief The control parameters.
	 *
	 */
	ರಚನೆ Param {
		// Vehicle specific params
		ದಶಮಾಂಶ max_sink_ದರ;			///< Maximum sink rate (with min throttle and max speed) [m/s].
		ದಶಮಾಂಶ min_sink_ದರ;			///< Minimum sink rate (with min throttle and trim speed) [m/s].
		ದಶಮಾಂಶ max_climb_ದರ;			///< Climb rate produced by max allowed throttle [m/s].
		ದಶಮಾಂಶ vert_accel_ಎಲ್ಲೆ;			///< Magnitude of the maximum vertical acceleration allowed [m/s²].
		ದಶಮಾಂಶ equivalent_airspeed_trim;		///< Equivalent cruise airspeed for airspeed less mode [m/s].
		ದಶಮಾಂಶ tas_min;				///< True airspeed demand lower limit [m/s].
		ದಶಮಾಂಶ tas_stall;			///< True stall airspeed [m/s].
		ದಶಮಾಂಶ tas_max;				///< True airspeed demand upper limit [m/s].
		ದಶಮಾಂಶ tas_trim;				///< True airspeed trim [m/s].
		ದಶಮಾಂಶ pitch_max;			///< Maximum absolute pitch angle setpoint [rad].
		ದಶಮಾಂಶ pitch_min;			///< Minimum absolute pitch angle setpoint [rad].
		ದಶಮಾಂಶ level_flight_pitch;		///< Pitch angle in level flight at trim airspeed, fed forward at zero climb angle [rad].
		ದಶಮಾಂಶ throttle_trim;		///< Normalized throttle required to fly level at calibrated airspeed setpoint [0,1]
		ದಶಮಾಂಶ throttle_max;			///< Normalized throttle upper limit.
		ದಶಮಾಂಶ throttle_min;			///< Normalized throttle lower limit.

		// Altitude control param
		ದಶಮಾಂಶ altitude_error_gain;		///< Altitude error inverse time constant [1/s].
		ದಶಮಾಂಶ altitude_setpoint_gain_ff;	///< Gain from altitude demand derivative to demanded climb rate.

		// Airspeed control param
		ದಶಮಾಂಶ airspeed_error_gain;				///< Airspeed error inverse time constant [1/s].

		// Energy control param
		ದಶಮಾಂಶ ste_ದರ_ಸಮಯ_const;				///< Filter time constant for specific total energy rate (damping path) [s].
		ದಶಮಾಂಶ seb_ದರ_ff;					///< Specific energy balance rate feedforward gain.

		// Pitch control param
		ದಶಮಾಂಶ pitch_ವೇಗ_ತೂಕ;				///< Speed control weighting used by pitch demand calculation.
		ದಶಮಾಂಶ integrator_gain_pitch;				///< Integrator gain used by the pitch demand calculation.
		ದಶಮಾಂಶ pitch_damping_gain;				///< Damping gain of the pitch demand calculation [s].

		// Throttle control param
		ದಶಮಾಂಶ integrator_gain_throttle;				///< Integrator gain used by the throttle demand calculation.
		ದಶಮಾಂಶ throttle_damping_gain;				///< Damping gain of the throttle demand calculation [s].
		ದಶಮಾಂಶ throttle_slewrate;				///< Throttle demand slew rate limit [1/s].

		ದಶಮಾಂಶ ತುಂಬು_ಗುಣಕ_correction;				///< Gain from normal load factor increase to total energy rate demand [m²/s³].
		ದಶಮಾಂಶ ತುಂಬು_ಗುಣಕ;					///< Additional normal load factor.

		ದಶಮಾಂಶ fast_descend;
	};

	/**
	 * @brief The debug output
	 *
	 */
	ರಚನೆ DebugOutput {
		ದಶಮಾಂಶ altitude_ದರ_control;			///< Altitude rate setpoint from altitude control loop [m/s].
		ದಶಮಾಂಶ true_airspeed_derivative_control;		///< Airspeed rate setpoint from airspeed control loop [m/s²].
		ದಶಮಾಂಶ ಒಟ್ಟು_energy_ದರ_estimate;		///< Total energy rate estimate [m²/s³].
		ದಶಮಾಂಶ ಒಟ್ಟು_energy_ದರ_sp;			///< Total energy rate setpoint [m²/s³].
		ದಶಮಾಂಶ energy_balance_ದರ_estimate;		///< Energy balance rate estimate [m²/s³].
		ದಶಮಾಂಶ energy_balance_ದರ_sp;			///< Energy balance rate setpoint [m²/s³].
		ದಶಮಾಂಶ pitch_integrator;				///< Pitch control integrator state, a correction added to the pitch setpoint [rad].
		ದಶಮಾಂಶ throttle_integrator;			///< Throttle control integrator state [-].
	};

	/**
	 * @brief Given setpoint to control.
	 *
	 */
	ರಚನೆ Setpoint {
		TECSAltitudeReferenceModel::AltitudeReferenceState altitude_reference;	///< Altitude/height rate reference.
		ದಶಮಾಂಶ altitude_ದರ_setpoint_ನೇರ;					///< Direct height rate setpoint.
		ದಶಮಾಂಶ tas_setpoint;						///< True airspeed setpoint.
	};

	/**
	 * @brief Given current measurement from the UAS.
	 *
	 */
	ರಚನೆ ಒಳಹರಿವು_2 {
		ದಶಮಾಂಶ altitude;		///< Current altitude amsl of the UAS [m].
		ದಶಮಾಂಶ altitude_ದರ;	///< Current altitude rate of the UAS [m/s].
		ದಶಮಾಂಶ tas;		///< Current true airspeed of the UAS [m/s].
		ದಶಮಾಂಶ tas_ದರ;		///< Current true airspeed rate of the UAS [m/s²].
	};

	/**
	 * @brief Control flags.
	 *
	 */
	ರಚನೆ Flag {
		ತಾರ್ಕಿಕ airspeed_enabled;			///< Flag if the airspeed sensor is enabled.
		ತಾರ್ಕಿಕ detect_underspeed_enabled;		///< Flag if underspeed detection is enabled.
	};
ಸಾರ್ವಜನಿಕ:
	TECSControl() = ಪೂರ್ವನಿಯೋಜಿತ;
	~TECSControl() = ಪೂರ್ವನಿಯೋಜಿತ;
	/**
	 * @brief Initialization of the state.
	 *
	 * @param[in] setpoint is the current setpoint struct.
	 * @param[in] input is the current input measurements.
	 * @param[in] param is the current parameter set.
	 * @param[in] flag is the current activated flags.
	 */
	ಶೂನ್ಯ initialize(ಸ್ಥಿರಾಂಕ Setpoint &setpoint, ಸ್ಥಿರಾಂಕ ಒಳಹರಿವು_2 &ಒಳಹರಿವು, ಸ್ಥಿರಾಂಕ Param &param, ಸ್ಥಿರಾಂಕ Flag &flag);
	/**
	 * @brief Update state and output.
	 *
	 * @param[in] dt is the update time interval in [s].
	 * @param[in] setpoint is the current setpoint struct.
	 * @param[in] input is the current input measurements.
	 * @param[in] param is the current parameter set.
	 * @param[in] flag is the current activated flags.
	 */
	ಶೂನ್ಯ update(ದಶಮಾಂಶ dt, ಸ್ಥಿರಾಂಕ Setpoint &setpoint, ಸ್ಥಿರಾಂಕ ಒಳಹರಿವು_2 &ಒಳಹರಿವು, ಸ್ಥಿರಾಂಕ Param &param, ಸ್ಥಿರಾಂಕ Flag &flag);
	/**
	 * @brief Reset the control loop integrals.
	 *
	 */
	ಶೂನ್ಯ resetIntegrals();
	/**
	 * @brief Get the underspeed ratio.
	 *
	 * @return Ratio of detected underspeed [0,1].
	 */
	ದಶಮಾಂಶ get_Underspeed_ಅನುಪಾತ() ಸ್ಥಿರಾಂಕ {ಹಿಂತಿರುಗಿ ಅನುಪಾತ_undersped;}
	/**
	 * @brief Get the throttle setpoint.
	 *
	 * @return throttle setpoint.
	 */
	ದಶಮಾಂಶ getThrottleSetpoint() ಸ್ಥಿರಾಂಕ {ಹಿಂತಿರುಗಿ _throttle_setpoint;}
	/**
	 * @brief Get the pitch setpoint.
	 *
	 * @return The commanded absolute pitch angle [rad].
	 */
	ದಶಮಾಂಶ getPitchSetpoint() ಸ್ಥಿರಾಂಕ {ಹಿಂತಿರುಗಿ _pitch_setpoint;}
	/**
	 * @brief Get the Debug Output
	 *
	 * @return the debug output struct.
	 */
	ಸ್ಥಿರಾಂಕ DebugOutput &getDebugOutput() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ _debug_output; }

ಖಾಸಗಿ:
	// Allows the regression test to pre-corrupt _pitch_integ_state and verify the
	// safety reset in _calcPitchControlUpdate, which the public update() path cannot
	// reach on its own because the integrator-input guard rejects bad input first.
	FRIEND_TEST(TECSControlTest, PitchIntegratorResetsCorruptedState);

	/**
	 * @brief Specific total energy rate limit.
	 *
	 */
	ರಚನೆ STERate_ಎಲ್ಲೆ {
		ದಶಮಾಂಶ STE_ದರ_max;	///< Maximum specific total energy rate limit [m²/s³].
		ದಶಮಾಂಶ STE_ದರ_min;	///< Minimal specific total energy rate limit [m²/s³].
	};

	/**
	 * @brief Control values.
	 * setpoint and current state estimate value as input to a controller.
	 *
	 */
	ರಚನೆ Control_ಮೌಲ್ಯಗಳು {
		ದಶಮಾಂಶ setpoint;		///< Control setpoint.
		ದಶಮಾಂಶ estimate;		///< Control estimate of current state value.
	};

	/**
	 * @brief Calculated specific energy rates.
	 *
	 */
	ರಚನೆ SpecificEnergyRates {
		Control_ಮೌಲ್ಯಗಳು ske_ದರ;	///< Specific kinetic energy rate [m²/s³].
		Control_ಮೌಲ್ಯಗಳು spe_ದರ;	///< Specific potential energy rate [m²/s³].
	};

	/**
	 * @brief Altitude rate and airspeed setpoints from the outer loops, from which the energy rate setpoints follow.
	 *
	 */
	ರಚನೆ OuterLoopSetpoints {
		ದಶಮಾಂಶ altitude_ದರ_setpoint;	///< Controlled altitude rate setpoint [m/s].
		ದಶಮಾಂಶ tas_ದರ_setpoint;	///< Controlled true airspeed rate setpoint [m/s²].
	};

	/**
	 * @brief Weight factors for specific energy.
	 *
	 */
	ರಚನೆ SpecificEnergyWeighting {
		ದಶಮಾಂಶ spe_weighting;	///< Specific potential energy weight.
		ದಶಮಾಂಶ ske_weighting;	///< Specific kinetic energy weight.
	};

ಖಾಸಗಿ:
	/**
	 * @brief Get control error from setpoint and estimate
	 *
	 * @param val is the current control setpoint and estimate.
	 * @return error value
	 */
	ಸ್ಥಿರ constexpr ದಶಮಾಂಶ _getControlError(TECSControl::Control_ಮೌಲ್ಯಗಳು val) {ಹಿಂತಿರುಗಿ (val.setpoint - val.estimate);}
	/**
	 * @brief Calculate specific total energy rate limits.
	 *
	 * @param[in] param are the control parameters.
	 * @return Specific total energy rate limits in [m²/s³].
	 */
	STERate_ಎಲ್ಲೆ calculate_ಒಟ್ಟು_Energy_ದರ_ಎಲ್ಲೆ(ಸ್ಥಿರಾಂಕ Param &param) ಸ್ಥಿರಾಂಕ;
	/**
	 * @brief Gain from specific total energy rate error to throttle.
	 *
	 * @param[in] limit is the specific total energy rate limits in [m²/s³].
	 * @return throttle per specific total energy rate [s³/m²].
	 */
	ಸ್ಥಿರ ದಶಮಾಂಶ ste_ದರ_ಗಮ್ಯ_Throttle(ಸ್ಥಿರಾಂಕ STERate_ಎಲ್ಲೆ &ಎಲ್ಲೆ_2) { ಹಿಂತಿರುಗಿ 1.0f / (ಎಲ್ಲೆ_2.STE_ದರ_max - ಎಲ್ಲೆ_2.STE_ದರ_min); }
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
	ದಶಮಾಂಶ _calcAirspeedControlOutput(ಸ್ಥಿರಾಂಕ Setpoint &setpoint, ಸ್ಥಿರಾಂಕ ಒಳಹರಿವು_2 &ಒಳಹರಿವು, ಸ್ಥಿರಾಂಕ STERate_ಎಲ್ಲೆ &ಎಲ್ಲೆ_2,
					 ಸ್ಥಿರಾಂಕ Param &param, ಸ್ಥಿರಾಂಕ Flag &flag) ಸ್ಥಿರಾಂಕ;
	/**
	 * @brief calculate altitude control proportional output.
	 *
	 * @param setpoint is the control setpoints.
	 * @param input is the current input measurement of the UAS.
	 * @param param is the control parameters.
	 * @return controlled altitude rate setpoint in [m/s].
	 */
	ದಶಮಾಂಶ _calcAltitudeControlOutput(ಸ್ಥಿರಾಂಕ Setpoint &setpoint, ಸ್ಥಿರಾಂಕ ಒಳಹರಿವು_2 &ಒಳಹರಿವು, ಸ್ಥಿರಾಂಕ Param &param) ಸ್ಥಿರಾಂಕ;
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
	SpecificEnergyRates _updateOuterLoops(ಸ್ಥಿರಾಂಕ Setpoint &setpoint, ಸ್ಥಿರಾಂಕ ಒಳಹರಿವು_2 &ಒಳಹರಿವು, ಸ್ಥಿರಾಂಕ STERate_ಎಲ್ಲೆ &ಎಲ್ಲೆ_2,
					      ಸ್ಥಿರಾಂಕ Param &param, ಸ್ಥಿರಾಂಕ Flag &flag);
	/**
	 * @brief Calculate specific energy rates.
	 *
	 * @param control_setpoint is the controlled altitude and airspeed rate setpoints.
	 * @param input is the current input measurement of the UAS.
	 * @return Specific energy rates in [m²/s³].
	 */
	SpecificEnergyRates _calcSpecificEnergyRates(ಸ್ಥಿರಾಂಕ OuterLoopSetpoints &control_setpoint, ಸ್ಥಿರಾಂಕ ಒಳಹರಿವು_2 &ಒಳಹರಿವು) ಸ್ಥಿರಾಂಕ;
	/**
	 * @brief Detect underspeed.
	 *
	 * @param input is the current input measurement of the UAS.
	 * @param param is the control parameters.
	 * @param flag is the control flags.
	 */
	ಶೂನ್ಯ _detectUnderspeed(ಸ್ಥಿರಾಂಕ ಒಳಹರಿವು_2 &ಒಳಹರಿವು, ಸ್ಥಿರಾಂಕ Param &param, ಸ್ಥಿರಾಂಕ Flag &flag);
	/**
	 * @brief Update specific energy balance weights.
	 *
	 * @param param is the control parameters.
	 * @param flag is the control flags.
	 * @return Weights used for the specific energy balance.
	 */
	SpecificEnergyWeighting update_ವೇಗ_Altitude_Weights(ಸ್ಥಿರಾಂಕ Param &param, ಸ್ಥಿರಾಂಕ Flag &flag);
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
	ದಶಮಾಂಶ calc_Pitch_Control_Ske_ದರ_Setpoint(ಸ್ಥಿರಾಂಕ Setpoint &setpoint, ಸ್ಥಿರಾಂಕ ಒಳಹರಿವು_2 &ಒಳಹರಿವು, ಸ್ಥಿರಾಂಕ STERate_ಎಲ್ಲೆ &ಎಲ್ಲೆ_2,
					       ಸ್ಥಿರಾಂಕ Param &param, ಸ್ಥಿರಾಂಕ Flag &flag, ಸ್ಥಿರಾಂಕ SpecificEnergyWeighting &ತೂಕ) ಸ್ಥಿರಾಂಕ;
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
	SpecificEnergyRates _calcPitchControlEnergyRates(ಸ್ಥಿರಾಂಕ Setpoint &setpoint, ಸ್ಥಿರಾಂಕ ಒಳಹರಿವು_2 &ಒಳಹರಿವು,
			ಸ್ಥಿರಾಂಕ STERate_ಎಲ್ಲೆ &ಎಲ್ಲೆ_2, ಸ್ಥಿರಾಂಕ SpecificEnergyRates &specific_energy_rates, ಸ್ಥಿರಾಂಕ Param &param,
			ಸ್ಥಿರಾಂಕ Flag &flag, ಸ್ಥಿರಾಂಕ SpecificEnergyWeighting &ತೂಕ) ಸ್ಥಿರಾಂಕ;
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
	ಶೂನ್ಯ _calcPitchControl(ದಶಮಾಂಶ dt, ಸ್ಥಿರಾಂಕ Setpoint &setpoint, ಸ್ಥಿರಾಂಕ ಒಳಹರಿವು_2 &ಒಳಹರಿವು, ಸ್ಥಿರಾಂಕ STERate_ಎಲ್ಲೆ &ಎಲ್ಲೆ_2,
			       ಸ್ಥಿರಾಂಕ SpecificEnergyRates &specific_energy_ದರ, ಸ್ಥಿರಾಂಕ Param &param, ಸ್ಥಿರಾಂಕ Flag &flag);

	/**
	 * @brief Calculate pitch control specific energy balance rates.
	 *
	 * @param weight is the weighting use of the potential and kinetic energy.
	 * @param specific_energy_rate is the specific energy rates in [m²/s³].
	 * @return specific energy balance rate values in [m²/s³].
	 */
	Control_ಮೌಲ್ಯಗಳು calc_Pitch_Control_Seb_ದರ(ಸ್ಥಿರಾಂಕ SpecificEnergyWeighting &ತೂಕ,
					       ಸ್ಥಿರಾಂಕ SpecificEnergyRates &specific_energy_ದರ) ಸ್ಥಿರಾಂಕ;

	/**
	 * @brief Calculate the pitch control update function.
	 * Update the states of the pitch control (pitch integrator).
	 *
	 * @param dt is the update time interval in [s].
	 * @param input is the current input measurement of the UAS.
	 * @param seb_rate is the specific energy balance rate in [m²/s³].
	 * @param param is the control parameters.
	 */
	ಶೂನ್ಯ _calcPitchControlUpdate(ದಶಮಾಂಶ dt, ಸ್ಥಿರಾಂಕ ಒಳಹರಿವು_2 &ಒಳಹರಿವು, ಸ್ಥಿರಾಂಕ Control_ಮೌಲ್ಯಗಳು &seb_ದರ, ಸ್ಥಿರಾಂಕ Param &param);

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
	ಸ್ಥಿರ ದಶಮಾಂಶ climb_Angle_ಗಮ್ಯ_Seb_ದರ(ಸ್ಥಿರಾಂಕ ಒಳಹರಿವು_2 &ಒಳಹರಿವು, ಸ್ಥಿರಾಂಕ Param &param);

	/**
	 * @brief Calculate the pitch control output function.
	 *
	 * @param input is the current input measurement of the UAS.
	 * @param seb_rate is the specific energy balance rate in [m²/s³].
	 * @param param is the control parameters.
	 * @return absolute pitch setpoint [rad].
	 */
	ದಶಮಾಂಶ _calcPitchControlOutput(ಸ್ಥಿರಾಂಕ ಒಳಹರಿವು_2 &ಒಳಹರಿವು, ಸ್ಥಿರಾಂಕ Control_ಮೌಲ್ಯಗಳು &seb_ದರ, ಸ್ಥಿರಾಂಕ Param &param) ಸ್ಥಿರಾಂಕ;

	/**
	 * @brief Update controlled throttle setpoint.
	 *
	 * @param dt is the update time interval in [s].
	 * @param limit is the specific total energy rate limits in [m²/s³].
	 * @param specific_energy_rate is the calculated specific energy.
	 * @param param is the control parameters.
	 * @param flag is the control flags.
	 */
	ಶೂನ್ಯ _calcThrottleControl(ದಶಮಾಂಶ dt, ಸ್ಥಿರಾಂಕ STERate_ಎಲ್ಲೆ &ಎಲ್ಲೆ_2, ಸ್ಥಿರಾಂಕ SpecificEnergyRates &specific_energy_ದರ,
				  ಸ್ಥಿರಾಂಕ Param &param, ಸ್ಥಿರಾಂಕ Flag &flag);

	/**
	 * @brief Calculate throttle control specific total energy
	 *
	 * @param limit is the specific total energy rate limits in [m²/s³].
	 * @param specific_energy_rate is the specific energy rates in [m²/s³].
	 * @param param is the control parameters.
	 * @return specific total energy rate values in [m²/s³]
	 */
	Control_ಮೌಲ್ಯಗಳು calc_Throttle_Control_Ste_ದರ(ಸ್ಥಿರಾಂಕ STERate_ಎಲ್ಲೆ &ಎಲ್ಲೆ_2, ಸ್ಥಿರಾಂಕ SpecificEnergyRates &specific_energy_ದರ,
			ಸ್ಥಿರಾಂಕ Param &param) ಸ್ಥಿರಾಂಕ;

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
	ಶೂನ್ಯ _calcThrottleControlUpdate(ದಶಮಾಂಶ dt, ಸ್ಥಿರಾಂಕ STERate_ಎಲ್ಲೆ &ಎಲ್ಲೆ_2, ಸ್ಥಿರಾಂಕ Control_ಮೌಲ್ಯಗಳು &ste_ದರ, ಸ್ಥಿರಾಂಕ Param &param,
					ಸ್ಥಿರಾಂಕ Flag &flag);

	/**
	 * @brief Calculate the throttle control output function.
	 *
	 * @param limit is the specific total energy rate limits in [m²/s³].
	 * @param ste_rate is the specific total energy rates in [m²/s³].
	 * @param param is the control parameters.
	 * @param flag is the control flags.
	 * @return throttle setpoint in [0,1].
	 */
	ದಶಮಾಂಶ _calcThrottleControlOutput(ಸ್ಥಿರಾಂಕ STERate_ಎಲ್ಲೆ &ಎಲ್ಲೆ_2, ಸ್ಥಿರಾಂಕ Control_ಮೌಲ್ಯಗಳು &ste_ದರ, ಸ್ಥಿರಾಂಕ Param &param,
					 ಸ್ಥಿರಾಂಕ Flag &flag) ಸ್ಥಿರಾಂಕ;

ಖಾಸಗಿ:
	// State
	AlphaFilter<ದಶಮಾಂಶ> ste_ದರ_estimate_filter;		///< Low pass filter for the specific total energy rate.
	ದಶಮಾಂಶ _pitch_integ_state{0.0f};				///< Pitch integrator state [rad].
	ದಶಮಾಂಶ _fast_descend_prev{0.0f};				///< Fast descend ratio of the last update [0,1].
	ದಶಮಾಂಶ _throttle_integ_state{0.0f};			///< Throttle integrator state [-].

	// Output
	DebugOutput _debug_output;				///< Debug output.
	ದಶಮಾಂಶ _pitch_setpoint{0.0f};				///< Controlled absolute pitch setpoint [rad].
	ದಶಮಾಂಶ _throttle_setpoint{0.0f};				///< Controlled throttle setpoint [0,1].
	ದಶಮಾಂಶ ಅನುಪಾತ_undersped{0.0f};				///< A continuous representation of how "undersped" the TAS is [0,1]
};

ವರ್ಗ TECS
{
ಸಾರ್ವಜನಿಕ:
	ರಚನೆ DebugOutput {
		TECSControl::DebugOutput control;
		ದಶಮಾಂಶ true_airspeed_sp;
		ದಶಮಾಂಶ true_airspeed_filtered;
		ದಶಮಾಂಶ true_airspeed_derivative;
		ದಶಮಾಂಶ altitude_reference;
		ದಶಮಾಂಶ ಎತ್ತರ_ದರ_reference;
		ದಶಮಾಂಶ ಎತ್ತರ_ದರ_ನೇರ;
		ದಶಮಾಂಶ fast_descend;
	};

	/**
	 * @brief Vehicle state, setpoints and limits for one control update.
	 *
	 */
	ರಚನೆ ಒಳಹರಿವು_2 {
		ದಶಮಾಂಶ altitude;				///< Current altitude amsl [m].
		ದಶಮಾಂಶ altitude_ದರ;			///< Current altitude rate [m/s].
		ದಶಮಾಂಶ altitude_setpoint;		///< Altitude setpoint amsl [m], NAN if not set.
		ದಶಮಾಂಶ altitude_ದರ_setpoint{NAN};	///< Altitude rate setpoint [m/s], NAN if not set.
		ದಶಮಾಂಶ equivalent_airspeed;		///< Current equivalent airspeed [m/s].
		ದಶಮಾಂಶ ವೇಗ_deriv_forward;		///< Current forward acceleration, used as true airspeed rate [m/s²].
		ದಶಮಾಂಶ equivalent_airspeed_setpoint;	///< Equivalent airspeed setpoint [m/s].
		ದಶಮಾಂಶ eas_ಗಮ್ಯ_tas;			///< Conversion factor from equivalent to true airspeed [-].
		ದಶಮಾಂಶ throttle_min;			///< Normalized throttle lower limit.
		ದಶಮಾಂಶ throttle_max;			///< Normalized throttle upper limit.
		ದಶಮಾಂಶ throttle_trim;			///< Normalized throttle required to fly level at the airspeed setpoint.
		ದಶಮಾಂಶ pitch_min;			///< Minimum absolute pitch angle setpoint [rad].
		ದಶಮಾಂಶ pitch_max;			///< Maximum absolute pitch angle setpoint [rad].
		ದಶಮಾಂಶ ಗುರಿ_climbrate;			///< Climb rate the reference model targets [m/s].
		ದಶಮಾಂಶ ಗುರಿ_sinkrate;			///< Sink rate the reference model targets [m/s].
	};

ಸಾರ್ವಜನಿಕ:
	TECS() = ಪೂರ್ವನಿಯೋಜಿತ;
	~TECS() = ಪೂರ್ವನಿಯೋಜಿತ;

	// no copy, assignment, move, move assignment
	TECS(ಸ್ಥಿರಾಂಕ TECS &) = ಅಳಿಸು;
	TECS &ಕ್ರಿಯಾಕಾರಕ=(ಸ್ಥಿರಾಂಕ TECS &) = ಅಳಿಸು;
	TECS(TECS &&) = ಅಳಿಸು;
	TECS &ಕ್ರಿಯಾಕಾರಕ=(TECS &&) = ಅಳಿಸು;

	ಸ್ಥಿರಾಂಕ DebugOutput &get_ಸ್ಥಿತಿ() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ debug_ಸ್ಥಿತಿ; }

	/**
	 * Set the airspeed enable state
	 *
	 * @param[in] enabled is true if the airspeed measurement is used for control.
	 */
	ಶೂನ್ಯ enable_airspeed(ತಾರ್ಕಿಕ enabled) { _control_flag.airspeed_enabled = enabled; }

	/**
	 * @brief Update the control loop calculations
	 *
	 * All pitch angles, including the pitch limits and the resulting pitch setpoint, are absolute: the angle of the
	 * body x-axis above the horizon.
	 *
	 * @param[in] input is the current vehicle state, setpoints and limits.
	 */
	ಶೂನ್ಯ update(ಸ್ಥಿರಾಂಕ ಒಳಹರಿವು_2 &ಒಳಹರಿವು);

	ಶೂನ್ಯ resetIntegrals()
	{
		_control.resetIntegrals();
	}

	ಶೂನ್ಯ ಗಣ_detect_underspeed_enabled(ತಾರ್ಕಿಕ enabled) { _control_flag.detect_underspeed_enabled = enabled; }

	// setters for parameters

	ಶೂನ್ಯ ಗಣ_integrator_gain_throttle(ದಶಮಾಂಶ gain) { _control_param.integrator_gain_throttle = gain;}
	ಶೂನ್ಯ ಗಣ_integrator_gain_pitch(ದಶಮಾಂಶ gain) { _control_param.integrator_gain_pitch = gain; }

	ಶೂನ್ಯ ಗಣ_max_sink_ದರ(ದಶಮಾಂಶ max_sink_ದರ) { _control_param.max_sink_ದರ = max_sink_ದರ; _reference_param.max_sink_ದರ = max_sink_ದರ; }
	ಶೂನ್ಯ ಗಣ_min_sink_ದರ(ದಶಮಾಂಶ min_sink_ದರ) { _control_param.min_sink_ದರ = min_sink_ದರ; }
	ಶೂನ್ಯ ಗಣ_max_climb_ದರ(ದಶಮಾಂಶ climb_ದರ) { _control_param.max_climb_ದರ = climb_ದರ; _reference_param.max_climb_ದರ = climb_ದರ; }

	ಶೂನ್ಯ ಗಣ_altitude_ದರ_ff(ದಶಮಾಂಶ altitude_ದರ_ff) { _control_param.altitude_setpoint_gain_ff = altitude_ದರ_ff; }
	ಶೂನ್ಯ ಗಣ_altitude_error_ಸಮಯ_constant(ದಶಮಾಂಶ ಸಮಯ_const) { _control_param.altitude_error_gain = 1.0f / math::max(ಸಮಯ_const, 0.1f); }
	ಶೂನ್ಯ ಗಣ_fast_descend_altitude_error(ದಶಮಾಂಶ altitude_error) { _fast_descend_alt_err = altitude_error; }

	ಶೂನ್ಯ ಗಣ_equivalent_airspeed_min(ದಶಮಾಂಶ airspeed) { _equivalent_airspeed_min = airspeed; }
	ಶೂನ್ಯ ಗಣ_equivalent_airspeed_stall(ದಶಮಾಂಶ airspeed) { _equivalent_airspeed_stall = airspeed; }
	ಶೂನ್ಯ ಗಣ_equivalent_airspeed_max(ದಶಮಾಂಶ airspeed) { _equivalent_airspeed_max = airspeed; }
	ಶೂನ್ಯ ಗಣ_equivalent_airspeed_trim(ದಶಮಾಂಶ airspeed) { _control_param.equivalent_airspeed_trim = airspeed; _airspeed_filter_param.equivalent_airspeed_trim = airspeed; }

	ಶೂನ್ಯ ಗಣ_pitch_damping(ದಶಮಾಂಶ damping) { _control_param.pitch_damping_gain = damping; }
	ಶೂನ್ಯ ಗಣ_vertical_accel_ಎಲ್ಲೆ(ದಶಮಾಂಶ ಎಲ್ಲೆ_2) { _reference_param.vert_accel_ಎಲ್ಲೆ = ಎಲ್ಲೆ_2; _control_param.vert_accel_ಎಲ್ಲೆ = ಎಲ್ಲೆ_2; }

	ಶೂನ್ಯ ಗಣ_ವೇಗ_ತೂಕ(ದಶಮಾಂಶ ತೂಕ) { _control_param.pitch_ವೇಗ_ತೂಕ = ತೂಕ; }
	ಶೂನ್ಯ ಗಣ_airspeed_error_ಸಮಯ_constant(ದಶಮಾಂಶ ಸಮಯ_const) { _control_param.airspeed_error_gain = 1.0f / math::max(ಸಮಯ_const, 0.1f); }

	ಶೂನ್ಯ ಗಣ_throttle_damp(ದಶಮಾಂಶ throttle_damp) { _control_param.throttle_damping_gain = throttle_damp; }
	ಶೂನ್ಯ ಗಣ_throttle_slewrate(ದಶಮಾಂಶ slewrate) { _control_param.throttle_slewrate = slewrate; }

	ಶೂನ್ಯ ಗಣ_roll_throttle_compensation(ದಶಮಾಂಶ compensation) { _control_param.ತುಂಬು_ಗುಣಕ_correction = compensation; }
	ಶೂನ್ಯ ಗಣ_ತುಂಬು_ಗುಣಕ(ದಶಮಾಂಶ ತುಂಬು_ಗುಣಕ) { _control_param.ತುಂಬು_ಗುಣಕ = ತುಂಬು_ಗುಣಕ; }

	ಶೂನ್ಯ ಗಣ_ste_ದರ_ಸಮಯ_const(ದಶಮಾಂಶ ಸಮಯ_const) { _control_param.ste_ದರ_ಸಮಯ_const = ಸಮಯ_const; }

	ಶೂನ್ಯ ಗಣ_seb_ದರ_ff_gain(ದಶಮಾಂಶ ff_gain) { _control_param.seb_ದರ_ff = ff_gain; }

	/**
	 * Set the pitch angle in level flight at trim airspeed [rad], fed forward as the pitch at zero climb angle
	 */
	ಶೂನ್ಯ ಗಣ_level_flight_pitch(ದಶಮಾಂಶ pitch) { _control_param.level_flight_pitch = pitch; }

	/**
	 * Handle the altitude reset
	 *
	 * If the estimation system resets the height in one discrete step this
	 * will gracefully even out the reset over time.
	 *
	 * @param[in] altitude is the altitude amsl after the reset [m].
	 * @param[in] altitude_rate is the current altitude rate [m/s].
	 */
	ಶೂನ್ಯ handle_alt_ಹೆಜ್ಜೆ(ದಶಮಾಂಶ altitude, ದಶಮಾಂಶ altitude_ದರ)
	{
		TECSAltitudeReferenceModel::AltitudeReferenceState init_state{ .alt = altitude,
				.alt_ದರ = altitude_ದರ};

		// reset altitude reference model.
		_altitude_reference_model.initialize(init_state);
	}

	/**
	 * Returns the absolute pitch setpoint [rad]
	 */
	ದಶಮಾಂಶ get_pitch_setpoint() ಸ್ಥಿರಾಂಕ {ಹಿಂತಿರುಗಿ _control.getPitchSetpoint();}
	ದಶಮಾಂಶ get_throttle_setpoint() ಸ್ಥಿರಾಂಕ {ಹಿಂತಿರುಗಿ _control.getThrottleSetpoint();}

	/**
	 * Returns the altitude tracking time constant
	 */
	ದಶಮಾಂಶ get_altitude_error_ಸಮಯ_constant() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ 1.0f / _control_param.altitude_error_gain; }

	ದಶಮಾಂಶ get_underspeed_ಅನುಪಾತ() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ _control.get_Underspeed_ಅನುಪಾತ(); }

ಖಾಸಗಿ:
	/**
	 * @brief Initialize the control parameters
	 *
	 * @param[in] input is the current vehicle state, setpoints and limits.
	 */
	ಶೂನ್ಯ initControlParams(ಸ್ಥಿರಾಂಕ ಒಳಹರಿವು_2 &ಒಳಹರಿವು);

	/**
	 * @brief calculate true airspeed setpoint
	 *
	 * Calculate true airspeed setpoint based on input and fast descend ratio
	 *
	 * @param eas_to_tas is the eas to tas conversion factor
	 * @param eas_setpoint is the desired equivalent airspeed setpoint [m/s]
	 * @return true airspeed setpoint[m/s]
	 */
	ದಶಮಾಂಶ calcTrueAirspeedSetpoint(ದಶಮಾಂಶ eas_ಗಮ್ಯ_tas, ದಶಮಾಂಶ eas_setpoint);

	/**
	 * @brief Initialize the control loop
	 *
	 * @param[in] input is the current vehicle state, setpoints and limits.
	 */
	ಶೂನ್ಯ initialize(ಸ್ಥಿರಾಂಕ ಒಳಹರಿವು_2 &ಒಳಹರಿವು);

	TECSControl 			_control;			///< Control submodule.
	TECSAirspeedFilter 		_airspeed_filter;		///< Airspeed filter submodule.
	TECSAltitudeReferenceModel 	_altitude_reference_model;	///< Setpoint reference model submodule.

	hrt_abstime _update_timestamp{0};				///< last timestamp of the update function call.

	ದಶಮಾಂಶ _equivalent_airspeed_min{10.0f};				///< equivalent airspeed demand lower limit (m/sec)
	ದಶಮಾಂಶ _equivalent_airspeed_stall{7.0f};				///< equivalent stall airspeed (m/sec)
	ದಶಮಾಂಶ _equivalent_airspeed_max{20.0f};				///< equivalent airspeed demand upper limit (m/sec)
	ದಶಮಾಂಶ _fast_descend_alt_err{-1.f};	 				///< Altitude difference between current altitude to altitude setpoint needed to descend with higher airspeed [m].
	ದಶಮಾಂಶ _fast_descend{0.f};					///< Value for fast descend in [0,1]. continuous value used to flatten the high speed value out when close to target altitude.
	hrt_abstime _enabled_fast_descend_timestamp{0U};		///< timestamp at activation of fast descend mode

	ಸ್ಥಿರ constexpr ದಶಮಾಂಶ DT_MIN = 0.001f;				///< minimum allowed value of _dt (sec)
	ಸ್ಥಿರ constexpr ದಶಮಾಂಶ DT_MAX = 1.0f;				///< max value of _dt allowed before a filter state reset is performed (sec)
	ಸ್ಥಿರ constexpr hrt_abstime FAST_DESCEND_RAMP_ಮೇಲೆ_ಸಮಯ = 2_s; 	///< Ramp up time until fast descend is fully engaged
	ಸ್ಥಿರ constexpr ದಶಮಾಂಶ FAST_DESCEND_ಎತ್ತರ_MARGIN = 5.f;	///< Height above setpoint where fast descend ends [m]

	DebugOutput debug_ಸ್ಥಿತಿ{};

	// Params
	/// Airspeed filter parameters.
	TECSAirspeedFilter::Param _airspeed_filter_param{
		.equivalent_airspeed_trim = 15.0f,
	};
	/// Reference model parameters.
	TECSAltitudeReferenceModel::Param _reference_param{
		.ಗುರಿ_climbrate = 2.0f,
		.ಗುರಿ_sinkrate = 2.0f,
		.vert_accel_ಎಲ್ಲೆ = 0.0f,
		.max_climb_ದರ = 2.0f,
		.max_sink_ದರ = 2.0f,
	};
	/// Control parameters.
	TECSControl::Param _control_param{
		.max_sink_ದರ = 5.0f,
		.min_sink_ದರ = 2.0f,
		.max_climb_ದರ = 5.0f,
		.vert_accel_ಎಲ್ಲೆ = 0.0f,
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
		.ste_ದರ_ಸಮಯ_const = 0.1f,
		.seb_ದರ_ff = 1.0f,
		.pitch_ವೇಗ_ತೂಕ = 1.0f,
		.integrator_gain_pitch = 0.0f,
		.pitch_damping_gain = 0.0f,
		.integrator_gain_throttle = 0.0f,
		.throttle_damping_gain = 0.0f,
		.throttle_slewrate = 0.0f,
		.ತುಂಬು_ಗುಣಕ_correction = 0.0f,
		.ತುಂಬು_ಗುಣಕ = 1.0f,
		.fast_descend = 0.f
	};

	TECSControl::Flag _control_flag{
		.airspeed_enabled = ಸುಳ್ಳು,
		.detect_underspeed_enabled = ಸುಳ್ಳು,
	};

	/**
	 * @brief Set fast descend value
	 *
	 * @param alt_setpoint is the altitude setpoint
	 * @param alt is the current altitude
	 */
	ಶೂನ್ಯ ಗಣ_Fast_Descend(ದಶಮಾಂಶ alt_setpoint, ದಶಮಾಂಶ alt);
};

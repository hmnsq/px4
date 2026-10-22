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

#அந்தர்பாவ <drivers/drv_hrt.h>
#அந்தர்பாவ <mathlib/mathlib.h>
#அந்தர்பாவ <matrix/math.hpp>
#அந்தர்பாவ <lib/mathlib/math/filter/AlphaFilter.hpp>
#அந்தர்பாவ <motion_planning/VelocitySmoothing.hpp>
#அந்தர்பாவ <motion_planning/ManualVelocitySmoothingZ.hpp>

using நாமாகாஶ time_literals;

#ifndef FRIEND_TEST // for gtest
#define FRIEND_TEST(a, b)
#endif

வகுப்பு TECSAirspeedFilter
{
பொது:
	/**
	 * @brief State of the equivalent airspeed filter.
	 *
	 */
	கட்டமைப்பு AirspeedFilterState {
		தசம speed;		///< speed of the air in EAS [m/s]
		தசம speed_rate;	///< rate of speed of the air [m/s²]
	};

	/**
	 * @brief Parameters of the airspeed filter.
	 *
	 */
	கட்டமைப்பு Param {
		தசம equivalent_airspeed_trim; 	///< the trim value of the equivalent airspeed [m/s].
	};

	/**
	 * @brief Input, which will be filtered.
	 *
	 */
	கட்டமைப்பு உள்ளீடு_2 {
		தசம equivalent_airspeed; 	///< the measured equivalent airspeed in [m/s].
		தசம equivalent_airspeed_rate;	///< the measured rate of equivalent airspeed in [m/s²].
	};
பொது:
	TECSAirspeedFilter() = இயல்பு;
	~TECSAirspeedFilter() = இயல்பு;
	/**
	 * @brief Initialize filter
	 *
	 * @param[in] equivalent_airspeed is the equivalent airspeed in [m/s].
	 * @param[in] equivalent_airspeed_trim is the equivalent airspeed trim (vehicle setting) in [m/s].
	 * @param[in] airspeed_sensor_available boolean if the airspeed sensor is available.
	 */
	வெறுமை initialize(தசம equivalent_airspeed, மாறிலி தசம equivalent_airspeed_trim,
			மாறிலி தருக்க airspeed_sensor_available);

	/**
	 * @brief Update filter
	 *
	 * @param[in] dt is the timestep in [s].
	 * @param[in] input are the raw measured values.
	 * @param[in] param are the filter parameters.
	 * @param[in] airspeed_sensor_available boolean if the airspeed sensor is available.
	 */
	வெறுமை update(தசம dt, மாறிலி உள்ளீடு_2 &உள்ளீடு, மாறிலி Param &param, மாறிலி தருக்க airspeed_sensor_available);

	/**
	 * @brief Get the filtered airspeed states.
	 *
	 * @return Current state of the airspeed filter.
	 */
	AirspeedFilterState getState() மாறிலி;

தனிப்பட்ட:
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
	நிலையான constexpr தசம kKg00{1.91940287f};
	நிலையான constexpr தசம kKg01{0.25925926f};
	நிலையான constexpr தசம kKg10{2.11640212f};
	நிலையான constexpr தசம kKg11{0.67179101f};
};

வகுப்பு TECSAltitudeReferenceModel
{
பொது:
	/**
	 * @brief Altitude reference state.
	 *
	 */
	கட்டமைப்பு AltitudeReferenceState {
		தசம alt; 	///< Reference altitude amsl in [m].
		தசம alt_rate;	///< Reference altitude rate in [m/s].
	};

	/**
	 * @brief Parameters for the reference model.
	 *
	 */
	கட்டமைப்பு Param {
		தசம target_climbrate;	///< The target climbrate in [m/s].
		தசம target_sinkrate;	///< The target sinkrate in [m/s].
		தசம vert_accel_limit;	///< Magnitude of the maximum vertical acceleration allowed [m/s²].
		தசம max_climb_rate;	///< Climb rate produced by max allowed throttle [m/s].
		தசம max_sink_rate;	///< Maximum sink rate (with min throttle, max speed) [m/s].
	};

பொது:
	TECSAltitudeReferenceModel() = இயல்பு;
	~TECSAltitudeReferenceModel() = இயல்பு;

	/**
	 * @brief Initialize reference models.
	 *
	 * @param[in] state is the current altitude state of the vehicle.
	 */
	வெறுமை initialize(மாறிலி AltitudeReferenceState &state);

	/**
	 * @brief Update reference models.
	 *
	 * @param[in] dt is the update interval in [s].
	 * @param[in] setpoint are the desired setpoints.
	 * @param[in] altitude is the altitude amsl in [m].
	 * @param[in] height_rate is the current height rate in [m/s].
	 * @param[in] param are the reference model parameters.
	 */
	வெறுமை update(தசம dt, மாறிலி AltitudeReferenceState &setpoint, தசம altitude, தசம height_rate, மாறிலி Param &param);

	/**
	 * @brief Get the current altitude reference of altitude reference model.
	 *
	 * @return Altitude reference state.
	 */
	AltitudeReferenceState getAltitudeReference() மாறிலி;

	/**
	 * @brief Get the Height Rate Setpoint directly from the velocity trajectory generator
	 *
	 * @return float direct height rate setpoint [m/s]
	 */
	தசம getHeightRateSetpointDirect() மாறிலி {திரும்பு _height_rate_setpoint_direct; }


தனிப்பட்ட:
	நிலையான constexpr தசம JERK_MAX = 1000.f;	///< Magnitude of the maximum jerk allowed [m/s³].

	// State
	VelocitySmoothing
	_alt_control_traj_generator;		///< Generates altitude rate and altitude setpoint trajectory when altitude is commanded.
	ManualVelocitySmoothingZ
	_velocity_control_traj_generator;	///< generates height rate trajectory when height rate is commanded
	தசம _height_rate_setpoint_direct{NAN}; ///< generated direct height rate setpoint
};

வகுப்பு TECSControl
{
பொது:
	/**
	 * @brief The control parameters.
	 *
	 */
	கட்டமைப்பு Param {
		// Vehicle specific params
		தசம max_sink_rate;			///< Maximum sink rate (with min throttle and max speed) [m/s].
		தசம min_sink_rate;			///< Minimum sink rate (with min throttle and trim speed) [m/s].
		தசம max_climb_rate;			///< Climb rate produced by max allowed throttle [m/s].
		தசம vert_accel_limit;			///< Magnitude of the maximum vertical acceleration allowed [m/s²].
		தசம equivalent_airspeed_trim;		///< Equivalent cruise airspeed for airspeed less mode [m/s].
		தசம tas_min;				///< True airspeed demand lower limit [m/s].
		தசம tas_stall;			///< True stall airspeed [m/s].
		தசம tas_max;				///< True airspeed demand upper limit [m/s].
		தசம tas_trim;				///< True airspeed trim [m/s].
		தசம pitch_max;			///< Maximum absolute pitch angle setpoint [rad].
		தசம pitch_min;			///< Minimum absolute pitch angle setpoint [rad].
		தசம level_flight_pitch;		///< Pitch angle in level flight at trim airspeed, fed forward at zero climb angle [rad].
		தசம throttle_trim;		///< Normalized throttle required to fly level at calibrated airspeed setpoint [0,1]
		தசம throttle_max;			///< Normalized throttle upper limit.
		தசம throttle_min;			///< Normalized throttle lower limit.

		// Altitude control param
		தசம altitude_error_gain;		///< Altitude error inverse time constant [1/s].
		தசம altitude_setpoint_gain_ff;	///< Gain from altitude demand derivative to demanded climb rate.

		// Airspeed control param
		தசம airspeed_error_gain;				///< Airspeed error inverse time constant [1/s].

		// Energy control param
		தசம ste_rate_time_const;				///< Filter time constant for specific total energy rate (damping path) [s].
		தசம seb_rate_ff;					///< Specific energy balance rate feedforward gain.

		// Pitch control param
		தசம pitch_speed_weight;				///< Speed control weighting used by pitch demand calculation.
		தசம integrator_gain_pitch;				///< Integrator gain used by the pitch demand calculation.
		தசம pitch_damping_gain;				///< Damping gain of the pitch demand calculation [s].

		// Throttle control param
		தசம integrator_gain_throttle;				///< Integrator gain used by the throttle demand calculation.
		தசம throttle_damping_gain;				///< Damping gain of the throttle demand calculation [s].
		தசம throttle_slewrate;				///< Throttle demand slew rate limit [1/s].

		தசம load_factor_correction;				///< Gain from normal load factor increase to total energy rate demand [m²/s³].
		தசம load_factor;					///< Additional normal load factor.

		தசம fast_descend;
	};

	/**
	 * @brief The debug output
	 *
	 */
	கட்டமைப்பு DebugOutput {
		தசம altitude_rate_control;			///< Altitude rate setpoint from altitude control loop [m/s].
		தசம true_airspeed_derivative_control;		///< Airspeed rate setpoint from airspeed control loop [m/s²].
		தசம total_energy_rate_estimate;		///< Total energy rate estimate [m²/s³].
		தசம total_energy_rate_sp;			///< Total energy rate setpoint [m²/s³].
		தசம energy_balance_rate_estimate;		///< Energy balance rate estimate [m²/s³].
		தசம energy_balance_rate_sp;			///< Energy balance rate setpoint [m²/s³].
		தசம pitch_integrator;				///< Pitch control integrator state, a correction added to the pitch setpoint [rad].
		தசம throttle_integrator;			///< Throttle control integrator state [-].
	};

	/**
	 * @brief Given setpoint to control.
	 *
	 */
	கட்டமைப்பு Setpoint {
		TECSAltitudeReferenceModel::AltitudeReferenceState altitude_reference;	///< Altitude/height rate reference.
		தசம altitude_rate_setpoint_direct;					///< Direct height rate setpoint.
		தசம tas_setpoint;						///< True airspeed setpoint.
	};

	/**
	 * @brief Given current measurement from the UAS.
	 *
	 */
	கட்டமைப்பு உள்ளீடு_2 {
		தசம altitude;		///< Current altitude amsl of the UAS [m].
		தசம altitude_rate;	///< Current altitude rate of the UAS [m/s].
		தசம tas;		///< Current true airspeed of the UAS [m/s].
		தசம tas_rate;		///< Current true airspeed rate of the UAS [m/s²].
	};

	/**
	 * @brief Control flags.
	 *
	 */
	கட்டமைப்பு Flag {
		தருக்க airspeed_enabled;			///< Flag if the airspeed sensor is enabled.
		தருக்க detect_underspeed_enabled;		///< Flag if underspeed detection is enabled.
	};
பொது:
	TECSControl() = இயல்பு;
	~TECSControl() = இயல்பு;
	/**
	 * @brief Initialization of the state.
	 *
	 * @param[in] setpoint is the current setpoint struct.
	 * @param[in] input is the current input measurements.
	 * @param[in] param is the current parameter set.
	 * @param[in] flag is the current activated flags.
	 */
	வெறுமை initialize(மாறிலி Setpoint &setpoint, மாறிலி உள்ளீடு_2 &உள்ளீடு, மாறிலி Param &param, மாறிலி Flag &flag);
	/**
	 * @brief Update state and output.
	 *
	 * @param[in] dt is the update time interval in [s].
	 * @param[in] setpoint is the current setpoint struct.
	 * @param[in] input is the current input measurements.
	 * @param[in] param is the current parameter set.
	 * @param[in] flag is the current activated flags.
	 */
	வெறுமை update(தசம dt, மாறிலி Setpoint &setpoint, மாறிலி உள்ளீடு_2 &உள்ளீடு, மாறிலி Param &param, மாறிலி Flag &flag);
	/**
	 * @brief Reset the control loop integrals.
	 *
	 */
	வெறுமை resetIntegrals();
	/**
	 * @brief Get the underspeed ratio.
	 *
	 * @return Ratio of detected underspeed [0,1].
	 */
	தசம getUnderspeedRatio() மாறிலி {திரும்பு _ratio_undersped;}
	/**
	 * @brief Get the throttle setpoint.
	 *
	 * @return throttle setpoint.
	 */
	தசம getThrottleSetpoint() மாறிலி {திரும்பு _throttle_setpoint;}
	/**
	 * @brief Get the pitch setpoint.
	 *
	 * @return The commanded absolute pitch angle [rad].
	 */
	தசம getPitchSetpoint() மாறிலி {திரும்பு _pitch_setpoint;}
	/**
	 * @brief Get the Debug Output
	 *
	 * @return the debug output struct.
	 */
	மாறிலி DebugOutput &getDebugOutput() மாறிலி { திரும்பு _debug_output; }

தனிப்பட்ட:
	// Allows the regression test to pre-corrupt _pitch_integ_state and verify the
	// safety reset in _calcPitchControlUpdate, which the public update() path cannot
	// reach on its own because the integrator-input guard rejects bad input first.
	FRIEND_TEST(TECSControlTest, PitchIntegratorResetsCorruptedState);

	/**
	 * @brief Specific total energy rate limit.
	 *
	 */
	கட்டமைப்பு STERateLimit {
		தசம STE_rate_max;	///< Maximum specific total energy rate limit [m²/s³].
		தசம STE_rate_min;	///< Minimal specific total energy rate limit [m²/s³].
	};

	/**
	 * @brief Control values.
	 * setpoint and current state estimate value as input to a controller.
	 *
	 */
	கட்டமைப்பு Control_மதிப்புகள் {
		தசம setpoint;		///< Control setpoint.
		தசம estimate;		///< Control estimate of current state value.
	};

	/**
	 * @brief Calculated specific energy rates.
	 *
	 */
	கட்டமைப்பு SpecificEnergyRates {
		Control_மதிப்புகள் ske_rate;	///< Specific kinetic energy rate [m²/s³].
		Control_மதிப்புகள் spe_rate;	///< Specific potential energy rate [m²/s³].
	};

	/**
	 * @brief Altitude rate and airspeed setpoints from the outer loops, from which the energy rate setpoints follow.
	 *
	 */
	கட்டமைப்பு OuterLoopSetpoints {
		தசம altitude_rate_setpoint;	///< Controlled altitude rate setpoint [m/s].
		தசம tas_rate_setpoint;	///< Controlled true airspeed rate setpoint [m/s²].
	};

	/**
	 * @brief Weight factors for specific energy.
	 *
	 */
	கட்டமைப்பு SpecificEnergyWeighting {
		தசம spe_weighting;	///< Specific potential energy weight.
		தசம ske_weighting;	///< Specific kinetic energy weight.
	};

தனிப்பட்ட:
	/**
	 * @brief Get control error from setpoint and estimate
	 *
	 * @param val is the current control setpoint and estimate.
	 * @return error value
	 */
	நிலையான constexpr தசம _getControlError(TECSControl::Control_மதிப்புகள் val) {திரும்பு (val.setpoint - val.estimate);}
	/**
	 * @brief Calculate specific total energy rate limits.
	 *
	 * @param[in] param are the control parameters.
	 * @return Specific total energy rate limits in [m²/s³].
	 */
	STERateLimit _calculateTotalEnergyRateLimit(மாறிலி Param &param) மாறிலி;
	/**
	 * @brief Gain from specific total energy rate error to throttle.
	 *
	 * @param[in] limit is the specific total energy rate limits in [m²/s³].
	 * @return throttle per specific total energy rate [s³/m²].
	 */
	நிலையான தசம _steRateToThrottle(மாறிலி STERateLimit &limit) { திரும்பு 1.0f / (limit.STE_rate_max - limit.STE_rate_min); }
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
	தசம _calcAirspeedControlOutput(மாறிலி Setpoint &setpoint, மாறிலி உள்ளீடு_2 &உள்ளீடு, மாறிலி STERateLimit &limit,
					 மாறிலி Param &param, மாறிலி Flag &flag) மாறிலி;
	/**
	 * @brief calculate altitude control proportional output.
	 *
	 * @param setpoint is the control setpoints.
	 * @param input is the current input measurement of the UAS.
	 * @param param is the control parameters.
	 * @return controlled altitude rate setpoint in [m/s].
	 */
	தசம _calcAltitudeControlOutput(மாறிலி Setpoint &setpoint, மாறிலி உள்ளீடு_2 &உள்ளீடு, மாறிலி Param &param) மாறிலி;
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
	SpecificEnergyRates _updateOuterLoops(மாறிலி Setpoint &setpoint, மாறிலி உள்ளீடு_2 &உள்ளீடு, மாறிலி STERateLimit &limit,
					      மாறிலி Param &param, மாறிலி Flag &flag);
	/**
	 * @brief Calculate specific energy rates.
	 *
	 * @param control_setpoint is the controlled altitude and airspeed rate setpoints.
	 * @param input is the current input measurement of the UAS.
	 * @return Specific energy rates in [m²/s³].
	 */
	SpecificEnergyRates _calcSpecificEnergyRates(மாறிலி OuterLoopSetpoints &control_setpoint, மாறிலி உள்ளீடு_2 &உள்ளீடு) மாறிலி;
	/**
	 * @brief Detect underspeed.
	 *
	 * @param input is the current input measurement of the UAS.
	 * @param param is the control parameters.
	 * @param flag is the control flags.
	 */
	வெறுமை _detectUnderspeed(மாறிலி உள்ளீடு_2 &உள்ளீடு, மாறிலி Param &param, மாறிலி Flag &flag);
	/**
	 * @brief Update specific energy balance weights.
	 *
	 * @param param is the control parameters.
	 * @param flag is the control flags.
	 * @return Weights used for the specific energy balance.
	 */
	SpecificEnergyWeighting _updateSpeedAltitudeWeights(மாறிலி Param &param, மாறிலி Flag &flag);
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
	தசம _calcPitchControlSkeRateSetpoint(மாறிலி Setpoint &setpoint, மாறிலி உள்ளீடு_2 &உள்ளீடு, மாறிலி STERateLimit &limit,
					       மாறிலி Param &param, மாறிலி Flag &flag, மாறிலி SpecificEnergyWeighting &weight) மாறிலி;
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
	SpecificEnergyRates _calcPitchControlEnergyRates(மாறிலி Setpoint &setpoint, மாறிலி உள்ளீடு_2 &உள்ளீடு,
			மாறிலி STERateLimit &limit, மாறிலி SpecificEnergyRates &specific_energy_rates, மாறிலி Param &param,
			மாறிலி Flag &flag, மாறிலி SpecificEnergyWeighting &weight) மாறிலி;
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
	வெறுமை _calcPitchControl(தசம dt, மாறிலி Setpoint &setpoint, மாறிலி உள்ளீடு_2 &உள்ளீடு, மாறிலி STERateLimit &limit,
			       மாறிலி SpecificEnergyRates &specific_energy_rate, மாறிலி Param &param, மாறிலி Flag &flag);

	/**
	 * @brief Calculate pitch control specific energy balance rates.
	 *
	 * @param weight is the weighting use of the potential and kinetic energy.
	 * @param specific_energy_rate is the specific energy rates in [m²/s³].
	 * @return specific energy balance rate values in [m²/s³].
	 */
	Control_மதிப்புகள் _calcPitchControlSebRate(மாறிலி SpecificEnergyWeighting &weight,
					       மாறிலி SpecificEnergyRates &specific_energy_rate) மாறிலி;

	/**
	 * @brief Calculate the pitch control update function.
	 * Update the states of the pitch control (pitch integrator).
	 *
	 * @param dt is the update time interval in [s].
	 * @param input is the current input measurement of the UAS.
	 * @param seb_rate is the specific energy balance rate in [m²/s³].
	 * @param param is the control parameters.
	 */
	வெறுமை _calcPitchControlUpdate(தசம dt, மாறிலி உள்ளீடு_2 &உள்ளீடு, மாறிலி Control_மதிப்புகள் &seb_rate, மாறிலி Param &param);

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
	நிலையான தசம _climbAngleToSebRate(மாறிலி உள்ளீடு_2 &உள்ளீடு, மாறிலி Param &param);

	/**
	 * @brief Calculate the pitch control output function.
	 *
	 * @param input is the current input measurement of the UAS.
	 * @param seb_rate is the specific energy balance rate in [m²/s³].
	 * @param param is the control parameters.
	 * @return absolute pitch setpoint [rad].
	 */
	தசம _calcPitchControlOutput(மாறிலி உள்ளீடு_2 &உள்ளீடு, மாறிலி Control_மதிப்புகள் &seb_rate, மாறிலி Param &param) மாறிலி;

	/**
	 * @brief Update controlled throttle setpoint.
	 *
	 * @param dt is the update time interval in [s].
	 * @param limit is the specific total energy rate limits in [m²/s³].
	 * @param specific_energy_rate is the calculated specific energy.
	 * @param param is the control parameters.
	 * @param flag is the control flags.
	 */
	வெறுமை _calcThrottleControl(தசம dt, மாறிலி STERateLimit &limit, மாறிலி SpecificEnergyRates &specific_energy_rate,
				  மாறிலி Param &param, மாறிலி Flag &flag);

	/**
	 * @brief Calculate throttle control specific total energy
	 *
	 * @param limit is the specific total energy rate limits in [m²/s³].
	 * @param specific_energy_rate is the specific energy rates in [m²/s³].
	 * @param param is the control parameters.
	 * @return specific total energy rate values in [m²/s³]
	 */
	Control_மதிப்புகள் _calcThrottleControlSteRate(மாறிலி STERateLimit &limit, மாறிலி SpecificEnergyRates &specific_energy_rate,
			மாறிலி Param &param) மாறிலி;

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
	வெறுமை _calcThrottleControlUpdate(தசம dt, மாறிலி STERateLimit &limit, மாறிலி Control_மதிப்புகள் &ste_rate, மாறிலி Param &param,
					மாறிலி Flag &flag);

	/**
	 * @brief Calculate the throttle control output function.
	 *
	 * @param limit is the specific total energy rate limits in [m²/s³].
	 * @param ste_rate is the specific total energy rates in [m²/s³].
	 * @param param is the control parameters.
	 * @param flag is the control flags.
	 * @return throttle setpoint in [0,1].
	 */
	தசம _calcThrottleControlOutput(மாறிலி STERateLimit &limit, மாறிலி Control_மதிப்புகள் &ste_rate, மாறிலி Param &param,
					 மாறிலி Flag &flag) மாறிலி;

தனிப்பட்ட:
	// State
	AlphaFilter<தசம> _ste_rate_estimate_filter;		///< Low pass filter for the specific total energy rate.
	தசம _pitch_integ_state{0.0f};				///< Pitch integrator state [rad].
	தசம _fast_descend_prev{0.0f};				///< Fast descend ratio of the last update [0,1].
	தசம _throttle_integ_state{0.0f};			///< Throttle integrator state [-].

	// Output
	DebugOutput _debug_output;				///< Debug output.
	தசம _pitch_setpoint{0.0f};				///< Controlled absolute pitch setpoint [rad].
	தசம _throttle_setpoint{0.0f};				///< Controlled throttle setpoint [0,1].
	தசம _ratio_undersped{0.0f};				///< A continuous representation of how "undersped" the TAS is [0,1]
};

வகுப்பு TECS
{
பொது:
	கட்டமைப்பு DebugOutput {
		TECSControl::DebugOutput control;
		தசம true_airspeed_sp;
		தசம true_airspeed_filtered;
		தசம true_airspeed_derivative;
		தசம altitude_reference;
		தசம height_rate_reference;
		தசம height_rate_direct;
		தசம fast_descend;
	};

	/**
	 * @brief Vehicle state, setpoints and limits for one control update.
	 *
	 */
	கட்டமைப்பு உள்ளீடு_2 {
		தசம altitude;				///< Current altitude amsl [m].
		தசம altitude_rate;			///< Current altitude rate [m/s].
		தசம altitude_setpoint;		///< Altitude setpoint amsl [m], NAN if not set.
		தசம altitude_rate_setpoint{NAN};	///< Altitude rate setpoint [m/s], NAN if not set.
		தசம equivalent_airspeed;		///< Current equivalent airspeed [m/s].
		தசம speed_deriv_forward;		///< Current forward acceleration, used as true airspeed rate [m/s²].
		தசம equivalent_airspeed_setpoint;	///< Equivalent airspeed setpoint [m/s].
		தசம eas_to_tas;			///< Conversion factor from equivalent to true airspeed [-].
		தசம throttle_min;			///< Normalized throttle lower limit.
		தசம throttle_max;			///< Normalized throttle upper limit.
		தசம throttle_trim;			///< Normalized throttle required to fly level at the airspeed setpoint.
		தசம pitch_min;			///< Minimum absolute pitch angle setpoint [rad].
		தசம pitch_max;			///< Maximum absolute pitch angle setpoint [rad].
		தசம target_climbrate;			///< Climb rate the reference model targets [m/s].
		தசம target_sinkrate;			///< Sink rate the reference model targets [m/s].
	};

பொது:
	TECS() = இயல்பு;
	~TECS() = இயல்பு;

	// no copy, assignment, move, move assignment
	TECS(மாறிலி TECS &) = delete;
	TECS &operator=(மாறிலி TECS &) = delete;
	TECS(TECS &&) = delete;
	TECS &operator=(TECS &&) = delete;

	மாறிலி DebugOutput &getStatus() மாறிலி { திரும்பு _debug_status; }

	/**
	 * Set the airspeed enable state
	 *
	 * @param[in] enabled is true if the airspeed measurement is used for control.
	 */
	வெறுமை enable_airspeed(தருக்க enabled) { _control_flag.airspeed_enabled = enabled; }

	/**
	 * @brief Update the control loop calculations
	 *
	 * All pitch angles, including the pitch limits and the resulting pitch setpoint, are absolute: the angle of the
	 * body x-axis above the horizon.
	 *
	 * @param[in] input is the current vehicle state, setpoints and limits.
	 */
	வெறுமை update(மாறிலி உள்ளீடு_2 &உள்ளீடு);

	வெறுமை resetIntegrals()
	{
		_control.resetIntegrals();
	}

	வெறுமை set_detect_underspeed_enabled(தருக்க enabled) { _control_flag.detect_underspeed_enabled = enabled; }

	// setters for parameters

	வெறுமை set_integrator_gain_throttle(தசம gain) { _control_param.integrator_gain_throttle = gain;}
	வெறுமை set_integrator_gain_pitch(தசம gain) { _control_param.integrator_gain_pitch = gain; }

	வெறுமை set_max_sink_rate(தசம max_sink_rate) { _control_param.max_sink_rate = max_sink_rate; _reference_param.max_sink_rate = max_sink_rate; }
	வெறுமை set_min_sink_rate(தசம min_sink_rate) { _control_param.min_sink_rate = min_sink_rate; }
	வெறுமை set_max_climb_rate(தசம climb_rate) { _control_param.max_climb_rate = climb_rate; _reference_param.max_climb_rate = climb_rate; }

	வெறுமை set_altitude_rate_ff(தசம altitude_rate_ff) { _control_param.altitude_setpoint_gain_ff = altitude_rate_ff; }
	வெறுமை set_altitude_error_time_constant(தசம time_const) { _control_param.altitude_error_gain = 1.0f / math::max(time_const, 0.1f); }
	வெறுமை set_fast_descend_altitude_error(தசம altitude_error) { _fast_descend_alt_err = altitude_error; }

	வெறுமை set_equivalent_airspeed_min(தசம airspeed) { _equivalent_airspeed_min = airspeed; }
	வெறுமை set_equivalent_airspeed_stall(தசம airspeed) { _equivalent_airspeed_stall = airspeed; }
	வெறுமை set_equivalent_airspeed_max(தசம airspeed) { _equivalent_airspeed_max = airspeed; }
	வெறுமை set_equivalent_airspeed_trim(தசம airspeed) { _control_param.equivalent_airspeed_trim = airspeed; _airspeed_filter_param.equivalent_airspeed_trim = airspeed; }

	வெறுமை set_pitch_damping(தசம damping) { _control_param.pitch_damping_gain = damping; }
	வெறுமை set_vertical_accel_limit(தசம limit) { _reference_param.vert_accel_limit = limit; _control_param.vert_accel_limit = limit; }

	வெறுமை set_speed_weight(தசம weight) { _control_param.pitch_speed_weight = weight; }
	வெறுமை set_airspeed_error_time_constant(தசம time_const) { _control_param.airspeed_error_gain = 1.0f / math::max(time_const, 0.1f); }

	வெறுமை set_throttle_damp(தசம throttle_damp) { _control_param.throttle_damping_gain = throttle_damp; }
	வெறுமை set_throttle_slewrate(தசம slewrate) { _control_param.throttle_slewrate = slewrate; }

	வெறுமை set_roll_throttle_compensation(தசம compensation) { _control_param.load_factor_correction = compensation; }
	வெறுமை set_load_factor(தசம load_factor) { _control_param.load_factor = load_factor; }

	வெறுமை set_ste_rate_time_const(தசம time_const) { _control_param.ste_rate_time_const = time_const; }

	வெறுமை set_seb_rate_ff_gain(தசம ff_gain) { _control_param.seb_rate_ff = ff_gain; }

	/**
	 * Set the pitch angle in level flight at trim airspeed [rad], fed forward as the pitch at zero climb angle
	 */
	வெறுமை set_level_flight_pitch(தசம pitch) { _control_param.level_flight_pitch = pitch; }

	/**
	 * Handle the altitude reset
	 *
	 * If the estimation system resets the height in one discrete step this
	 * will gracefully even out the reset over time.
	 *
	 * @param[in] altitude is the altitude amsl after the reset [m].
	 * @param[in] altitude_rate is the current altitude rate [m/s].
	 */
	வெறுமை handle_alt_step(தசம altitude, தசம altitude_rate)
	{
		TECSAltitudeReferenceModel::AltitudeReferenceState init_state{ .alt = altitude,
				.alt_rate = altitude_rate};

		// reset altitude reference model.
		_altitude_reference_model.initialize(init_state);
	}

	/**
	 * Returns the absolute pitch setpoint [rad]
	 */
	தசம get_pitch_setpoint() மாறிலி {திரும்பு _control.getPitchSetpoint();}
	தசம get_throttle_setpoint() மாறிலி {திரும்பு _control.getThrottleSetpoint();}

	/**
	 * Returns the altitude tracking time constant
	 */
	தசம get_altitude_error_time_constant() மாறிலி { திரும்பு 1.0f / _control_param.altitude_error_gain; }

	தசம get_underspeed_ratio() மாறிலி { திரும்பு _control.getUnderspeedRatio(); }

தனிப்பட்ட:
	/**
	 * @brief Initialize the control parameters
	 *
	 * @param[in] input is the current vehicle state, setpoints and limits.
	 */
	வெறுமை initControlParams(மாறிலி உள்ளீடு_2 &உள்ளீடு);

	/**
	 * @brief calculate true airspeed setpoint
	 *
	 * Calculate true airspeed setpoint based on input and fast descend ratio
	 *
	 * @param eas_to_tas is the eas to tas conversion factor
	 * @param eas_setpoint is the desired equivalent airspeed setpoint [m/s]
	 * @return true airspeed setpoint[m/s]
	 */
	தசம calcTrueAirspeedSetpoint(தசம eas_to_tas, தசம eas_setpoint);

	/**
	 * @brief Initialize the control loop
	 *
	 * @param[in] input is the current vehicle state, setpoints and limits.
	 */
	வெறுமை initialize(மாறிலி உள்ளீடு_2 &உள்ளீடு);

	TECSControl 			_control;			///< Control submodule.
	TECSAirspeedFilter 		_airspeed_filter;		///< Airspeed filter submodule.
	TECSAltitudeReferenceModel 	_altitude_reference_model;	///< Setpoint reference model submodule.

	hrt_abstime _update_timestamp{0};				///< last timestamp of the update function call.

	தசம _equivalent_airspeed_min{10.0f};				///< equivalent airspeed demand lower limit (m/sec)
	தசம _equivalent_airspeed_stall{7.0f};				///< equivalent stall airspeed (m/sec)
	தசம _equivalent_airspeed_max{20.0f};				///< equivalent airspeed demand upper limit (m/sec)
	தசம _fast_descend_alt_err{-1.f};	 				///< Altitude difference between current altitude to altitude setpoint needed to descend with higher airspeed [m].
	தசம _fast_descend{0.f};					///< Value for fast descend in [0,1]. continuous value used to flatten the high speed value out when close to target altitude.
	hrt_abstime _enabled_fast_descend_timestamp{0U};		///< timestamp at activation of fast descend mode

	நிலையான constexpr தசம DT_MIN = 0.001f;				///< minimum allowed value of _dt (sec)
	நிலையான constexpr தசம DT_MAX = 1.0f;				///< max value of _dt allowed before a filter state reset is performed (sec)
	நிலையான constexpr hrt_abstime FAST_DESCEND_RAMP_UP_TIME = 2_s; 	///< Ramp up time until fast descend is fully engaged
	நிலையான constexpr தசம FAST_DESCEND_HEIGHT_MARGIN = 5.f;	///< Height above setpoint where fast descend ends [m]

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
		.airspeed_enabled = பொய்,
		.detect_underspeed_enabled = பொய்,
	};

	/**
	 * @brief Set fast descend value
	 *
	 * @param alt_setpoint is the altitude setpoint
	 * @param alt is the current altitude
	 */
	வெறுமை _setFastDescend(தசம alt_setpoint, தசம alt);
};

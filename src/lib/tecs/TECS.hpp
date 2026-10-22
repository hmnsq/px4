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

#निर्देशः once

#अन्तर्भाव <drivers/drv_hrt.h>
#अन्तर्भाव <mathlib/mathlib.h>
#अन्तर्भाव <matrix/math.hpp>
#अन्तर्भाव <lib/mathlib/math/filter/AlphaFilter.hpp>
#अन्तर्भाव <motion_planning/VelocitySmoothing.hpp>
#अन्तर्भाव <motion_planning/ManualVelocitySmoothingZ.hpp>

उपयुज्य नामाकाश time_literals;

#यदि_अपरिभाषितम् FRIEND_TEST // for gtest
#परिभाषय FRIEND_TEST(a, b)
#यदि_अन्तः

वर्ग TECSAirspeedFilter
{
सार्वजनिक:
	/**
	 * @brief State of the equivalent airspeed filter.
	 *
	 */
	संरचना AirspeedFilterState {
		भिन्न जवः;		///< speed of the air in EAS [m/s]
		भिन्न जवः_दरः;	///< rate of speed of the air [m/s²]
	};

	/**
	 * @brief Parameters of the airspeed filter.
	 *
	 */
	संरचना Param {
		भिन्न equivalent_airspeed_trim; 	///< the trim value of the equivalent airspeed [m/s].
	};

	/**
	 * @brief Input, which will be filtered.
	 *
	 */
	संरचना निवेशः_2 {
		भिन्न equivalent_airspeed; 	///< the measured equivalent airspeed in [m/s].
		भिन्न equivalent_airspeed_दरः;	///< the measured rate of equivalent airspeed in [m/s²].
	};
सार्वजनिक:
	TECSAirspeedFilter() = स्वभाव;
	~TECSAirspeedFilter() = स्वभाव;
	/**
	 * @brief Initialize filter
	 *
	 * @param[in] equivalent_airspeed is the equivalent airspeed in [m/s].
	 * @param[in] equivalent_airspeed_trim is the equivalent airspeed trim (vehicle setting) in [m/s].
	 * @param[in] airspeed_sensor_available boolean if the airspeed sensor is available.
	 */
	शून्य initialize(भिन्न equivalent_airspeed, ध्रुव भिन्न equivalent_airspeed_trim,
			ध्रुव तार्किक airspeed_sensor_उपलब्धम्);

	/**
	 * @brief Update filter
	 *
	 * @param[in] dt is the timestep in [s].
	 * @param[in] input are the raw measured values.
	 * @param[in] param are the filter parameters.
	 * @param[in] airspeed_sensor_available boolean if the airspeed sensor is available.
	 */
	शून्य update(भिन्न dt, ध्रुव निवेशः_2 &निवेशः, ध्रुव Param &param, ध्रुव तार्किक airspeed_sensor_उपलब्धम्);

	/**
	 * @brief Get the filtered airspeed states.
	 *
	 * @return Current state of the airspeed filter.
	 */
	AirspeedFilterState getState() ध्रुव;

गुप्त:
	// States
	AirspeedFilterState _airspeed_state{.जवः = 0.0f, .जवः_दरः = 0.0f};	///< Complementary filter state

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
	स्थिर constexpr भिन्न kKg00{1.91940287f};
	स्थिर constexpr भिन्न kKg01{0.25925926f};
	स्थिर constexpr भिन्न kKg10{2.11640212f};
	स्थिर constexpr भिन्न kKg11{0.67179101f};
};

वर्ग TECSAltitudeReferenceModel
{
सार्वजनिक:
	/**
	 * @brief Altitude reference state.
	 *
	 */
	संरचना AltitudeReferenceState {
		भिन्न alt; 	///< Reference altitude amsl in [m].
		भिन्न alt_दरः;	///< Reference altitude rate in [m/s].
	};

	/**
	 * @brief Parameters for the reference model.
	 *
	 */
	संरचना Param {
		भिन्न target_climbrate;	///< The target climbrate in [m/s].
		भिन्न target_sinkrate;	///< The target sinkrate in [m/s].
		भिन्न vert_accel_सीमा;	///< Magnitude of the maximum vertical acceleration allowed [m/s²].
		भिन्न max_climb_दरः;	///< Climb rate produced by max allowed throttle [m/s].
		भिन्न max_sink_दरः;	///< Maximum sink rate (with min throttle, max speed) [m/s].
	};

सार्वजनिक:
	TECSAltitudeReferenceModel() = स्वभाव;
	~TECSAltitudeReferenceModel() = स्वभाव;

	/**
	 * @brief Initialize reference models.
	 *
	 * @param[in] state is the current altitude state of the vehicle.
	 */
	शून्य initialize(ध्रुव AltitudeReferenceState &state);

	/**
	 * @brief Update reference models.
	 *
	 * @param[in] dt is the update interval in [s].
	 * @param[in] setpoint are the desired setpoints.
	 * @param[in] altitude is the altitude amsl in [m].
	 * @param[in] height_rate is the current height rate in [m/s].
	 * @param[in] param are the reference model parameters.
	 */
	शून्य update(भिन्न dt, ध्रुव AltitudeReferenceState &setpoint, भिन्न altitude, भिन्न height_दरः, ध्रुव Param &param);

	/**
	 * @brief Get the current altitude reference of altitude reference model.
	 *
	 * @return Altitude reference state.
	 */
	AltitudeReferenceState getAltitudeReference() ध्रुव;

	/**
	 * @brief Get the Height Rate Setpoint directly from the velocity trajectory generator
	 *
	 * @return float direct height rate setpoint [m/s]
	 */
	भिन्न get_Height_दरः_Setpoint_प्रत्यक्षम्() ध्रुव {प्रत्यागम height_दरः_setpoint_प्रत्यक्षम्; }


गुप्त:
	स्थिर constexpr भिन्न JERK_MAX = 1000.f;	///< Magnitude of the maximum jerk allowed [m/s³].

	// State
	VelocitySmoothing
	_alt_control_traj_generator;		///< Generates altitude rate and altitude setpoint trajectory when altitude is commanded.
	ManualVelocitySmoothingZ
	_velocity_control_traj_generator;	///< generates height rate trajectory when height rate is commanded
	भिन्न height_दरः_setpoint_प्रत्यक्षम्{NAN}; ///< generated direct height rate setpoint
};

वर्ग TECSControl
{
सार्वजनिक:
	/**
	 * @brief The control parameters.
	 *
	 */
	संरचना Param {
		// Vehicle specific params
		भिन्न max_sink_दरः;			///< Maximum sink rate (with min throttle and max speed) [m/s].
		भिन्न min_sink_दरः;			///< Minimum sink rate (with min throttle and trim speed) [m/s].
		भिन्न max_climb_दरः;			///< Climb rate produced by max allowed throttle [m/s].
		भिन्न vert_accel_सीमा;			///< Magnitude of the maximum vertical acceleration allowed [m/s²].
		भिन्न equivalent_airspeed_trim;		///< Equivalent cruise airspeed for airspeed less mode [m/s].
		भिन्न tas_min;				///< True airspeed demand lower limit [m/s].
		भिन्न tas_stall;			///< True stall airspeed [m/s].
		भिन्न tas_max;				///< True airspeed demand upper limit [m/s].
		भिन्न tas_trim;				///< True airspeed trim [m/s].
		भिन्न pitch_max;			///< Maximum absolute pitch angle setpoint [rad].
		भिन्न pitch_min;			///< Minimum absolute pitch angle setpoint [rad].
		भिन्न level_flight_pitch;		///< Pitch angle in level flight at trim airspeed, fed forward at zero climb angle [rad].
		भिन्न throttle_trim;		///< Normalized throttle required to fly level at calibrated airspeed setpoint [0,1]
		भिन्न throttle_max;			///< Normalized throttle upper limit.
		भिन्न throttle_min;			///< Normalized throttle lower limit.

		// Altitude control param
		भिन्न altitude_error_gain;		///< Altitude error inverse time constant [1/s].
		भिन्न altitude_setpoint_gain_ff;	///< Gain from altitude demand derivative to demanded climb rate.

		// Airspeed control param
		भिन्न airspeed_error_gain;				///< Airspeed error inverse time constant [1/s].

		// Energy control param
		भिन्न ste_दरः_time_const;				///< Filter time constant for specific total energy rate (damping path) [s].
		भिन्न seb_दरः_ff;					///< Specific energy balance rate feedforward gain.

		// Pitch control param
		भिन्न pitch_जवः_weight;				///< Speed control weighting used by pitch demand calculation.
		भिन्न integrator_gain_pitch;				///< Integrator gain used by the pitch demand calculation.
		भिन्न pitch_damping_gain;				///< Damping gain of the pitch demand calculation [s].

		// Throttle control param
		भिन्न integrator_gain_throttle;				///< Integrator gain used by the throttle demand calculation.
		भिन्न throttle_damping_gain;				///< Damping gain of the throttle demand calculation [s].
		भिन्न throttle_slewrate;				///< Throttle demand slew rate limit [1/s].

		भिन्न आरोपय_गुणकः_correction;				///< Gain from normal load factor increase to total energy rate demand [m²/s³].
		भिन्न आरोपय_गुणकः;					///< Additional normal load factor.

		भिन्न fast_descend;
	};

	/**
	 * @brief The debug output
	 *
	 */
	संरचना DebugOutput {
		भिन्न altitude_दरः_control;			///< Altitude rate setpoint from altitude control loop [m/s].
		भिन्न true_airspeed_derivative_control;		///< Airspeed rate setpoint from airspeed control loop [m/s²].
		भिन्न योग_energy_दरः_estimate;		///< Total energy rate estimate [m²/s³].
		भिन्न योग_energy_दरः_sp;			///< Total energy rate setpoint [m²/s³].
		भिन्न energy_balance_दरः_estimate;		///< Energy balance rate estimate [m²/s³].
		भिन्न energy_balance_दरः_sp;			///< Energy balance rate setpoint [m²/s³].
		भिन्न pitch_integrator;				///< Pitch control integrator state, a correction added to the pitch setpoint [rad].
		भिन्न throttle_integrator;			///< Throttle control integrator state [-].
	};

	/**
	 * @brief Given setpoint to control.
	 *
	 */
	संरचना Setpoint {
		TECSAltitudeReferenceModel::AltitudeReferenceState altitude_reference;	///< Altitude/height rate reference.
		भिन्न altitude_दरः_setpoint_प्रत्यक्षम्;					///< Direct height rate setpoint.
		भिन्न tas_setpoint;						///< True airspeed setpoint.
	};

	/**
	 * @brief Given current measurement from the UAS.
	 *
	 */
	संरचना निवेशः_2 {
		भिन्न altitude;		///< Current altitude amsl of the UAS [m].
		भिन्न altitude_दरः;	///< Current altitude rate of the UAS [m/s].
		भिन्न tas;		///< Current true airspeed of the UAS [m/s].
		भिन्न tas_दरः;		///< Current true airspeed rate of the UAS [m/s²].
	};

	/**
	 * @brief Control flags.
	 *
	 */
	संरचना Flag {
		तार्किक airspeed_enabled;			///< Flag if the airspeed sensor is enabled.
		तार्किक detect_underspeed_enabled;		///< Flag if underspeed detection is enabled.
	};
सार्वजनिक:
	TECSControl() = स्वभाव;
	~TECSControl() = स्वभाव;
	/**
	 * @brief Initialization of the state.
	 *
	 * @param[in] setpoint is the current setpoint struct.
	 * @param[in] input is the current input measurements.
	 * @param[in] param is the current parameter set.
	 * @param[in] flag is the current activated flags.
	 */
	शून्य initialize(ध्रुव Setpoint &setpoint, ध्रुव निवेशः_2 &निवेशः, ध्रुव Param &param, ध्रुव Flag &flag);
	/**
	 * @brief Update state and output.
	 *
	 * @param[in] dt is the update time interval in [s].
	 * @param[in] setpoint is the current setpoint struct.
	 * @param[in] input is the current input measurements.
	 * @param[in] param is the current parameter set.
	 * @param[in] flag is the current activated flags.
	 */
	शून्य update(भिन्न dt, ध्रुव Setpoint &setpoint, ध्रुव निवेशः_2 &निवेशः, ध्रुव Param &param, ध्रुव Flag &flag);
	/**
	 * @brief Reset the control loop integrals.
	 *
	 */
	शून्य resetIntegrals();
	/**
	 * @brief Get the underspeed ratio.
	 *
	 * @return Ratio of detected underspeed [0,1].
	 */
	भिन्न get_Underspeed_अनुपातः() ध्रुव {प्रत्यागम अनुपातः_undersped;}
	/**
	 * @brief Get the throttle setpoint.
	 *
	 * @return throttle setpoint.
	 */
	भिन्न getThrottleSetpoint() ध्रुव {प्रत्यागम _throttle_setpoint;}
	/**
	 * @brief Get the pitch setpoint.
	 *
	 * @return The commanded absolute pitch angle [rad].
	 */
	भिन्न getPitchSetpoint() ध्रुव {प्रत्यागम _pitch_setpoint;}
	/**
	 * @brief Get the Debug Output
	 *
	 * @return the debug output struct.
	 */
	ध्रुव DebugOutput &getDebugOutput() ध्रुव { प्रत्यागम _debug_output; }

गुप्त:
	// Allows the regression test to pre-corrupt _pitch_integ_state and verify the
	// safety reset in _calcPitchControlUpdate, which the public update() path cannot
	// reach on its own because the integrator-input guard rejects bad input first.
	FRIEND_TEST(TECSControlTest, PitchIntegratorResetsCorruptedState);

	/**
	 * @brief Specific total energy rate limit.
	 *
	 */
	संरचना STERate_सीमा {
		भिन्न STE_दरः_max;	///< Maximum specific total energy rate limit [m²/s³].
		भिन्न STE_दरः_min;	///< Minimal specific total energy rate limit [m²/s³].
	};

	/**
	 * @brief Control values.
	 * setpoint and current state estimate value as input to a controller.
	 *
	 */
	संरचना Control_मानानि {
		भिन्न setpoint;		///< Control setpoint.
		भिन्न estimate;		///< Control estimate of current state value.
	};

	/**
	 * @brief Calculated specific energy rates.
	 *
	 */
	संरचना SpecificEnergyRates {
		Control_मानानि ske_दरः;	///< Specific kinetic energy rate [m²/s³].
		Control_मानानि spe_दरः;	///< Specific potential energy rate [m²/s³].
	};

	/**
	 * @brief Altitude rate and airspeed setpoints from the outer loops, from which the energy rate setpoints follow.
	 *
	 */
	संरचना OuterLoopSetpoints {
		भिन्न altitude_दरः_setpoint;	///< Controlled altitude rate setpoint [m/s].
		भिन्न tas_दरः_setpoint;	///< Controlled true airspeed rate setpoint [m/s²].
	};

	/**
	 * @brief Weight factors for specific energy.
	 *
	 */
	संरचना SpecificEnergyWeighting {
		भिन्न spe_weighting;	///< Specific potential energy weight.
		भिन्न ske_weighting;	///< Specific kinetic energy weight.
	};

गुप्त:
	/**
	 * @brief Get control error from setpoint and estimate
	 *
	 * @param val is the current control setpoint and estimate.
	 * @return error value
	 */
	स्थिर constexpr भिन्न _getControlError(TECSControl::Control_मानानि val) {प्रत्यागम (val.setpoint - val.estimate);}
	/**
	 * @brief Calculate specific total energy rate limits.
	 *
	 * @param[in] param are the control parameters.
	 * @return Specific total energy rate limits in [m²/s³].
	 */
	STERate_सीमा calculate_योग_Energy_दरः_सीमा(ध्रुव Param &param) ध्रुव;
	/**
	 * @brief Gain from specific total energy rate error to throttle.
	 *
	 * @param[in] limit is the specific total energy rate limits in [m²/s³].
	 * @return throttle per specific total energy rate [s³/m²].
	 */
	स्थिर भिन्न ste_दरः_गन्तव्यम्_Throttle(ध्रुव STERate_सीमा &सीमा_2) { प्रत्यागम 1.0f / (सीमा_2.STE_दरः_max - सीमा_2.STE_दरः_min); }
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
	भिन्न _calcAirspeedControlOutput(ध्रुव Setpoint &setpoint, ध्रुव निवेशः_2 &निवेशः, ध्रुव STERate_सीमा &सीमा_2,
					 ध्रुव Param &param, ध्रुव Flag &flag) ध्रुव;
	/**
	 * @brief calculate altitude control proportional output.
	 *
	 * @param setpoint is the control setpoints.
	 * @param input is the current input measurement of the UAS.
	 * @param param is the control parameters.
	 * @return controlled altitude rate setpoint in [m/s].
	 */
	भिन्न _calcAltitudeControlOutput(ध्रुव Setpoint &setpoint, ध्रुव निवेशः_2 &निवेशः, ध्रुव Param &param) ध्रुव;
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
	SpecificEnergyRates _updateOuterLoops(ध्रुव Setpoint &setpoint, ध्रुव निवेशः_2 &निवेशः, ध्रुव STERate_सीमा &सीमा_2,
					      ध्रुव Param &param, ध्रुव Flag &flag);
	/**
	 * @brief Calculate specific energy rates.
	 *
	 * @param control_setpoint is the controlled altitude and airspeed rate setpoints.
	 * @param input is the current input measurement of the UAS.
	 * @return Specific energy rates in [m²/s³].
	 */
	SpecificEnergyRates _calcSpecificEnergyRates(ध्रुव OuterLoopSetpoints &control_setpoint, ध्रुव निवेशः_2 &निवेशः) ध्रुव;
	/**
	 * @brief Detect underspeed.
	 *
	 * @param input is the current input measurement of the UAS.
	 * @param param is the control parameters.
	 * @param flag is the control flags.
	 */
	शून्य _detectUnderspeed(ध्रुव निवेशः_2 &निवेशः, ध्रुव Param &param, ध्रुव Flag &flag);
	/**
	 * @brief Update specific energy balance weights.
	 *
	 * @param param is the control parameters.
	 * @param flag is the control flags.
	 * @return Weights used for the specific energy balance.
	 */
	SpecificEnergyWeighting update_जवः_Altitude_Weights(ध्रुव Param &param, ध्रुव Flag &flag);
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
	भिन्न calc_Pitch_Control_Ske_दरः_Setpoint(ध्रुव Setpoint &setpoint, ध्रुव निवेशः_2 &निवेशः, ध्रुव STERate_सीमा &सीमा_2,
					       ध्रुव Param &param, ध्रुव Flag &flag, ध्रुव SpecificEnergyWeighting &weight) ध्रुव;
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
	SpecificEnergyRates _calcPitchControlEnergyRates(ध्रुव Setpoint &setpoint, ध्रुव निवेशः_2 &निवेशः,
			ध्रुव STERate_सीमा &सीमा_2, ध्रुव SpecificEnergyRates &specific_energy_rates, ध्रुव Param &param,
			ध्रुव Flag &flag, ध्रुव SpecificEnergyWeighting &weight) ध्रुव;
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
	शून्य _calcPitchControl(भिन्न dt, ध्रुव Setpoint &setpoint, ध्रुव निवेशः_2 &निवेशः, ध्रुव STERate_सीमा &सीमा_2,
			       ध्रुव SpecificEnergyRates &specific_energy_दरः, ध्रुव Param &param, ध्रुव Flag &flag);

	/**
	 * @brief Calculate pitch control specific energy balance rates.
	 *
	 * @param weight is the weighting use of the potential and kinetic energy.
	 * @param specific_energy_rate is the specific energy rates in [m²/s³].
	 * @return specific energy balance rate values in [m²/s³].
	 */
	Control_मानानि calc_Pitch_Control_Seb_दरः(ध्रुव SpecificEnergyWeighting &weight,
					       ध्रुव SpecificEnergyRates &specific_energy_दरः) ध्रुव;

	/**
	 * @brief Calculate the pitch control update function.
	 * Update the states of the pitch control (pitch integrator).
	 *
	 * @param dt is the update time interval in [s].
	 * @param input is the current input measurement of the UAS.
	 * @param seb_rate is the specific energy balance rate in [m²/s³].
	 * @param param is the control parameters.
	 */
	शून्य _calcPitchControlUpdate(भिन्न dt, ध्रुव निवेशः_2 &निवेशः, ध्रुव Control_मानानि &seb_दरः, ध्रुव Param &param);

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
	स्थिर भिन्न climb_Angle_गन्तव्यम्_Seb_दरः(ध्रुव निवेशः_2 &निवेशः, ध्रुव Param &param);

	/**
	 * @brief Calculate the pitch control output function.
	 *
	 * @param input is the current input measurement of the UAS.
	 * @param seb_rate is the specific energy balance rate in [m²/s³].
	 * @param param is the control parameters.
	 * @return absolute pitch setpoint [rad].
	 */
	भिन्न _calcPitchControlOutput(ध्रुव निवेशः_2 &निवेशः, ध्रुव Control_मानानि &seb_दरः, ध्रुव Param &param) ध्रुव;

	/**
	 * @brief Update controlled throttle setpoint.
	 *
	 * @param dt is the update time interval in [s].
	 * @param limit is the specific total energy rate limits in [m²/s³].
	 * @param specific_energy_rate is the calculated specific energy.
	 * @param param is the control parameters.
	 * @param flag is the control flags.
	 */
	शून्य _calcThrottleControl(भिन्न dt, ध्रुव STERate_सीमा &सीमा_2, ध्रुव SpecificEnergyRates &specific_energy_दरः,
				  ध्रुव Param &param, ध्रुव Flag &flag);

	/**
	 * @brief Calculate throttle control specific total energy
	 *
	 * @param limit is the specific total energy rate limits in [m²/s³].
	 * @param specific_energy_rate is the specific energy rates in [m²/s³].
	 * @param param is the control parameters.
	 * @return specific total energy rate values in [m²/s³]
	 */
	Control_मानानि calc_Throttle_Control_Ste_दरः(ध्रुव STERate_सीमा &सीमा_2, ध्रुव SpecificEnergyRates &specific_energy_दरः,
			ध्रुव Param &param) ध्रुव;

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
	शून्य _calcThrottleControlUpdate(भिन्न dt, ध्रुव STERate_सीमा &सीमा_2, ध्रुव Control_मानानि &ste_दरः, ध्रुव Param &param,
					ध्रुव Flag &flag);

	/**
	 * @brief Calculate the throttle control output function.
	 *
	 * @param limit is the specific total energy rate limits in [m²/s³].
	 * @param ste_rate is the specific total energy rates in [m²/s³].
	 * @param param is the control parameters.
	 * @param flag is the control flags.
	 * @return throttle setpoint in [0,1].
	 */
	भिन्न _calcThrottleControlOutput(ध्रुव STERate_सीमा &सीमा_2, ध्रुव Control_मानानि &ste_दरः, ध्रुव Param &param,
					 ध्रुव Flag &flag) ध्रुव;

गुप्त:
	// State
	AlphaFilter<भिन्न> ste_दरः_estimate_filter;		///< Low pass filter for the specific total energy rate.
	भिन्न _pitch_integ_state{0.0f};				///< Pitch integrator state [rad].
	भिन्न fast_descend_पूर्व{0.0f};				///< Fast descend ratio of the last update [0,1].
	भिन्न _throttle_integ_state{0.0f};			///< Throttle integrator state [-].

	// Output
	DebugOutput _debug_output;				///< Debug output.
	भिन्न _pitch_setpoint{0.0f};				///< Controlled absolute pitch setpoint [rad].
	भिन्न _throttle_setpoint{0.0f};				///< Controlled throttle setpoint [0,1].
	भिन्न अनुपातः_undersped{0.0f};				///< A continuous representation of how "undersped" the TAS is [0,1]
};

वर्ग TECS
{
सार्वजनिक:
	संरचना DebugOutput {
		TECSControl::DebugOutput control;
		भिन्न true_airspeed_sp;
		भिन्न true_airspeed_filtered;
		भिन्न true_airspeed_derivative;
		भिन्न altitude_reference;
		भिन्न height_दरः_reference;
		भिन्न height_दरः_प्रत्यक्षम्;
		भिन्न fast_descend;
	};

	/**
	 * @brief Vehicle state, setpoints and limits for one control update.
	 *
	 */
	संरचना निवेशः_2 {
		भिन्न altitude;				///< Current altitude amsl [m].
		भिन्न altitude_दरः;			///< Current altitude rate [m/s].
		भिन्न altitude_setpoint;		///< Altitude setpoint amsl [m], NAN if not set.
		भिन्न altitude_दरः_setpoint{NAN};	///< Altitude rate setpoint [m/s], NAN if not set.
		भिन्न equivalent_airspeed;		///< Current equivalent airspeed [m/s].
		भिन्न जवः_deriv_forward;		///< Current forward acceleration, used as true airspeed rate [m/s²].
		भिन्न equivalent_airspeed_setpoint;	///< Equivalent airspeed setpoint [m/s].
		भिन्न eas_गन्तव्यम्_tas;			///< Conversion factor from equivalent to true airspeed [-].
		भिन्न throttle_min;			///< Normalized throttle lower limit.
		भिन्न throttle_max;			///< Normalized throttle upper limit.
		भिन्न throttle_trim;			///< Normalized throttle required to fly level at the airspeed setpoint.
		भिन्न pitch_min;			///< Minimum absolute pitch angle setpoint [rad].
		भिन्न pitch_max;			///< Maximum absolute pitch angle setpoint [rad].
		भिन्न target_climbrate;			///< Climb rate the reference model targets [m/s].
		भिन्न target_sinkrate;			///< Sink rate the reference model targets [m/s].
	};

सार्वजनिक:
	TECS() = स्वभाव;
	~TECS() = स्वभाव;

	// no copy, assignment, move, move assignment
	TECS(ध्रुव TECS &) = विलोपय;
	TECS &संक्रियकः=(ध्रुव TECS &) = विलोपय;
	TECS(TECS &&) = विलोपय;
	TECS &संक्रियकः=(TECS &&) = विलोपय;

	ध्रुव DebugOutput &get_स्थितिः() ध्रुव { प्रत्यागम debug_स्थितिः; }

	/**
	 * Set the airspeed enable state
	 *
	 * @param[in] enabled is true if the airspeed measurement is used for control.
	 */
	शून्य enable_airspeed(तार्किक enabled) { _control_flag.airspeed_enabled = enabled; }

	/**
	 * @brief Update the control loop calculations
	 *
	 * All pitch angles, including the pitch limits and the resulting pitch setpoint, are absolute: the angle of the
	 * body x-axis above the horizon.
	 *
	 * @param[in] input is the current vehicle state, setpoints and limits.
	 */
	शून्य update(ध्रुव निवेशः_2 &निवेशः);

	शून्य resetIntegrals()
	{
		_control.resetIntegrals();
	}

	शून्य समुच्चयः_detect_underspeed_enabled(तार्किक enabled) { _control_flag.detect_underspeed_enabled = enabled; }

	// setters for parameters

	शून्य समुच्चयः_integrator_gain_throttle(भिन्न gain) { _control_param.integrator_gain_throttle = gain;}
	शून्य समुच्चयः_integrator_gain_pitch(भिन्न gain) { _control_param.integrator_gain_pitch = gain; }

	शून्य समुच्चयः_max_sink_दरः(भिन्न max_sink_दरः) { _control_param.max_sink_दरः = max_sink_दरः; _reference_param.max_sink_दरः = max_sink_दरः; }
	शून्य समुच्चयः_min_sink_दरः(भिन्न min_sink_दरः) { _control_param.min_sink_दरः = min_sink_दरः; }
	शून्य समुच्चयः_max_climb_दरः(भिन्न climb_दरः) { _control_param.max_climb_दरः = climb_दरः; _reference_param.max_climb_दरः = climb_दरः; }

	शून्य समुच्चयः_altitude_दरः_ff(भिन्न altitude_दरः_ff) { _control_param.altitude_setpoint_gain_ff = altitude_दरः_ff; }
	शून्य समुच्चयः_altitude_error_time_constant(भिन्न time_const) { _control_param.altitude_error_gain = 1.0f / math::max(time_const, 0.1f); }
	शून्य समुच्चयः_fast_descend_altitude_error(भिन्न altitude_error) { _fast_descend_alt_err = altitude_error; }

	शून्य समुच्चयः_equivalent_airspeed_min(भिन्न airspeed) { _equivalent_airspeed_min = airspeed; }
	शून्य समुच्चयः_equivalent_airspeed_stall(भिन्न airspeed) { _equivalent_airspeed_stall = airspeed; }
	शून्य समुच्चयः_equivalent_airspeed_max(भिन्न airspeed) { _equivalent_airspeed_max = airspeed; }
	शून्य समुच्चयः_equivalent_airspeed_trim(भिन्न airspeed) { _control_param.equivalent_airspeed_trim = airspeed; _airspeed_filter_param.equivalent_airspeed_trim = airspeed; }

	शून्य समुच्चयः_pitch_damping(भिन्न damping) { _control_param.pitch_damping_gain = damping; }
	शून्य समुच्चयः_vertical_accel_सीमा(भिन्न सीमा_2) { _reference_param.vert_accel_सीमा = सीमा_2; _control_param.vert_accel_सीमा = सीमा_2; }

	शून्य समुच्चयः_जवः_weight(भिन्न weight) { _control_param.pitch_जवः_weight = weight; }
	शून्य समुच्चयः_airspeed_error_time_constant(भिन्न time_const) { _control_param.airspeed_error_gain = 1.0f / math::max(time_const, 0.1f); }

	शून्य समुच्चयः_throttle_damp(भिन्न throttle_damp) { _control_param.throttle_damping_gain = throttle_damp; }
	शून्य समुच्चयः_throttle_slewrate(भिन्न slewrate) { _control_param.throttle_slewrate = slewrate; }

	शून्य समुच्चयः_roll_throttle_compensation(भिन्न compensation) { _control_param.आरोपय_गुणकः_correction = compensation; }
	शून्य समुच्चयः_आरोपय_गुणकः(भिन्न आरोपय_गुणकः) { _control_param.आरोपय_गुणकः = आरोपय_गुणकः; }

	शून्य समुच्चयः_ste_दरः_time_const(भिन्न time_const) { _control_param.ste_दरः_time_const = time_const; }

	शून्य समुच्चयः_seb_दरः_ff_gain(भिन्न ff_gain) { _control_param.seb_दरः_ff = ff_gain; }

	/**
	 * Set the pitch angle in level flight at trim airspeed [rad], fed forward as the pitch at zero climb angle
	 */
	शून्य समुच्चयः_level_flight_pitch(भिन्न pitch) { _control_param.level_flight_pitch = pitch; }

	/**
	 * Handle the altitude reset
	 *
	 * If the estimation system resets the height in one discrete step this
	 * will gracefully even out the reset over time.
	 *
	 * @param[in] altitude is the altitude amsl after the reset [m].
	 * @param[in] altitude_rate is the current altitude rate [m/s].
	 */
	शून्य handle_alt_चरणः(भिन्न altitude, भिन्न altitude_दरः)
	{
		TECSAltitudeReferenceModel::AltitudeReferenceState init_state{ .alt = altitude,
				.alt_दरः = altitude_दरः};

		// reset altitude reference model.
		_altitude_reference_model.initialize(init_state);
	}

	/**
	 * Returns the absolute pitch setpoint [rad]
	 */
	भिन्न get_pitch_setpoint() ध्रुव {प्रत्यागम _control.getPitchSetpoint();}
	भिन्न get_throttle_setpoint() ध्रुव {प्रत्यागम _control.getThrottleSetpoint();}

	/**
	 * Returns the altitude tracking time constant
	 */
	भिन्न get_altitude_error_time_constant() ध्रुव { प्रत्यागम 1.0f / _control_param.altitude_error_gain; }

	भिन्न get_underspeed_अनुपातः() ध्रुव { प्रत्यागम _control.get_Underspeed_अनुपातः(); }

गुप्त:
	/**
	 * @brief Initialize the control parameters
	 *
	 * @param[in] input is the current vehicle state, setpoints and limits.
	 */
	शून्य initControlParams(ध्रुव निवेशः_2 &निवेशः);

	/**
	 * @brief calculate true airspeed setpoint
	 *
	 * Calculate true airspeed setpoint based on input and fast descend ratio
	 *
	 * @param eas_to_tas is the eas to tas conversion factor
	 * @param eas_setpoint is the desired equivalent airspeed setpoint [m/s]
	 * @return true airspeed setpoint[m/s]
	 */
	भिन्न calcTrueAirspeedSetpoint(भिन्न eas_गन्तव्यम्_tas, भिन्न eas_setpoint);

	/**
	 * @brief Initialize the control loop
	 *
	 * @param[in] input is the current vehicle state, setpoints and limits.
	 */
	शून्य initialize(ध्रुव निवेशः_2 &निवेशः);

	TECSControl 			_control;			///< Control submodule.
	TECSAirspeedFilter 		_airspeed_filter;		///< Airspeed filter submodule.
	TECSAltitudeReferenceModel 	_altitude_reference_model;	///< Setpoint reference model submodule.

	hrt_abstime _update_timestamp{0};				///< last timestamp of the update function call.

	भिन्न _equivalent_airspeed_min{10.0f};				///< equivalent airspeed demand lower limit (m/sec)
	भिन्न _equivalent_airspeed_stall{7.0f};				///< equivalent stall airspeed (m/sec)
	भिन्न _equivalent_airspeed_max{20.0f};				///< equivalent airspeed demand upper limit (m/sec)
	भिन्न _fast_descend_alt_err{-1.f};	 				///< Altitude difference between current altitude to altitude setpoint needed to descend with higher airspeed [m].
	भिन्न _fast_descend{0.f};					///< Value for fast descend in [0,1]. continuous value used to flatten the high speed value out when close to target altitude.
	hrt_abstime _enabled_fast_descend_timestamp{0U};		///< timestamp at activation of fast descend mode

	स्थिर constexpr भिन्न DT_MIN = 0.001f;				///< minimum allowed value of _dt (sec)
	स्थिर constexpr भिन्न DT_MAX = 1.0f;				///< max value of _dt allowed before a filter state reset is performed (sec)
	स्थिर constexpr hrt_abstime FAST_DESCEND_RAMP_ऊर्ध्वम्_TIME = 2_s; 	///< Ramp up time until fast descend is fully engaged
	स्थिर constexpr भिन्न FAST_DESCEND_HEIGHT_MARGIN = 5.f;	///< Height above setpoint where fast descend ends [m]

	DebugOutput debug_स्थितिः{};

	// Params
	/// Airspeed filter parameters.
	TECSAirspeedFilter::Param _airspeed_filter_param{
		.equivalent_airspeed_trim = 15.0f,
	};
	/// Reference model parameters.
	TECSAltitudeReferenceModel::Param _reference_param{
		.target_climbrate = 2.0f,
		.target_sinkrate = 2.0f,
		.vert_accel_सीमा = 0.0f,
		.max_climb_दरः = 2.0f,
		.max_sink_दरः = 2.0f,
	};
	/// Control parameters.
	TECSControl::Param _control_param{
		.max_sink_दरः = 5.0f,
		.min_sink_दरः = 2.0f,
		.max_climb_दरः = 5.0f,
		.vert_accel_सीमा = 0.0f,
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
		.ste_दरः_time_const = 0.1f,
		.seb_दरः_ff = 1.0f,
		.pitch_जवः_weight = 1.0f,
		.integrator_gain_pitch = 0.0f,
		.pitch_damping_gain = 0.0f,
		.integrator_gain_throttle = 0.0f,
		.throttle_damping_gain = 0.0f,
		.throttle_slewrate = 0.0f,
		.आरोपय_गुणकः_correction = 0.0f,
		.आरोपय_गुणकः = 1.0f,
		.fast_descend = 0.f
	};

	TECSControl::Flag _control_flag{
		.airspeed_enabled = असत्य,
		.detect_underspeed_enabled = असत्य,
	};

	/**
	 * @brief Set fast descend value
	 *
	 * @param alt_setpoint is the altitude setpoint
	 * @param alt is the current altitude
	 */
	शून्य समुच्चयः_Fast_Descend(भिन्न alt_setpoint, भिन्न alt);
};

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

#ہدایت once

#ضمن <drivers/drv_hrt.h>
#ضمن <mathlib/mathlib.h>
#ضمن <matrix/math.hpp>
#ضمن <lib/mathlib/math/filter/AlphaFilter.hpp>
#ضمن <motion_planning/VelocitySmoothing.hpp>
#ضمن <motion_planning/ManualVelocitySmoothingZ.hpp>

ورتوں نطاق ویلا_literals;

#جے_بنا_تعریف FRIEND_TEST // for gtest
#تعریف_کرو FRIEND_TEST(a, b)
#جے_انت

صنف TECSAirspeedFilter
{
عام:
	/**
	 * @brief State of the equivalent airspeed filter.
	 *
	 */
	بنية AirspeedFilterState {
		عائم رفتار;		///< speed of the air in EAS [m/s]
		عائم رفتار_شرح;	///< rate of speed of the air [m/s²]
	};

	/**
	 * @brief Parameters of the airspeed filter.
	 *
	 */
	بنية Param {
		عائم equivalent_airspeed_trim; 	///< the trim value of the equivalent airspeed [m/s].
	};

	/**
	 * @brief Input, which will be filtered.
	 *
	 */
	بنية داخلہ_2 {
		عائم equivalent_airspeed; 	///< the measured equivalent airspeed in [m/s].
		عائم equivalent_airspeed_شرح;	///< the measured rate of equivalent airspeed in [m/s²].
	};
عام:
	TECSAirspeedFilter() = افتراضي;
	~TECSAirspeedFilter() = افتراضي;
	/**
	 * @brief Initialize filter
	 *
	 * @param[in] equivalent_airspeed is the equivalent airspeed in [m/s].
	 * @param[in] equivalent_airspeed_trim is the equivalent airspeed trim (vehicle setting) in [m/s].
	 * @param[in] airspeed_sensor_available boolean if the airspeed sensor is available.
	 */
	فراغ initialize(عائم equivalent_airspeed, ثابت عائم equivalent_airspeed_trim,
			ثابت منطقي airspeed_sensor_دستیاب);

	/**
	 * @brief Update filter
	 *
	 * @param[in] dt is the timestep in [s].
	 * @param[in] input are the raw measured values.
	 * @param[in] param are the filter parameters.
	 * @param[in] airspeed_sensor_available boolean if the airspeed sensor is available.
	 */
	فراغ update(عائم dt, ثابت داخلہ_2 &داخلہ, ثابت Param &param, ثابت منطقي airspeed_sensor_دستیاب);

	/**
	 * @brief Get the filtered airspeed states.
	 *
	 * @return Current state of the airspeed filter.
	 */
	AirspeedFilterState getState() ثابت;

خاص:
	// States
	AirspeedFilterState _airspeed_state{.رفتار = 0.0f, .رفتار_شرح = 0.0f};	///< Complementary filter state

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
	ساكن constexpr عائم kKg00{1.91940287f};
	ساكن constexpr عائم kKg01{0.25925926f};
	ساكن constexpr عائم kKg10{2.11640212f};
	ساكن constexpr عائم kKg11{0.67179101f};
};

صنف TECSAltitudeReferenceModel
{
عام:
	/**
	 * @brief Altitude reference state.
	 *
	 */
	بنية AltitudeReferenceState {
		عائم alt; 	///< Reference altitude amsl in [m].
		عائم alt_شرح;	///< Reference altitude rate in [m/s].
	};

	/**
	 * @brief Parameters for the reference model.
	 *
	 */
	بنية Param {
		عائم نشانہ_climbrate;	///< The target climbrate in [m/s].
		عائم نشانہ_sinkrate;	///< The target sinkrate in [m/s].
		عائم vert_accel_حد_بندی;	///< Magnitude of the maximum vertical acceleration allowed [m/s²].
		عائم max_climb_شرح;	///< Climb rate produced by max allowed throttle [m/s].
		عائم max_sink_شرح;	///< Maximum sink rate (with min throttle, max speed) [m/s].
	};

عام:
	TECSAltitudeReferenceModel() = افتراضي;
	~TECSAltitudeReferenceModel() = افتراضي;

	/**
	 * @brief Initialize reference models.
	 *
	 * @param[in] state is the current altitude state of the vehicle.
	 */
	فراغ initialize(ثابت AltitudeReferenceState &state);

	/**
	 * @brief Update reference models.
	 *
	 * @param[in] dt is the update interval in [s].
	 * @param[in] setpoint are the desired setpoints.
	 * @param[in] altitude is the altitude amsl in [m].
	 * @param[in] height_rate is the current height rate in [m/s].
	 * @param[in] param are the reference model parameters.
	 */
	فراغ update(عائم dt, ثابت AltitudeReferenceState &setpoint, عائم altitude, عائم اچائی_شرح, ثابت Param &param);

	/**
	 * @brief Get the current altitude reference of altitude reference model.
	 *
	 * @return Altitude reference state.
	 */
	AltitudeReferenceState getAltitudeReference() ثابت;

	/**
	 * @brief Get the Height Rate Setpoint directly from the velocity trajectory generator
	 *
	 * @return float direct height rate setpoint [m/s]
	 */
	عائم get_اچائی_شرح_Setpoint_سدھا() ثابت {أرجع اچائی_شرح_setpoint_سدھا; }


خاص:
	ساكن constexpr عائم JERK_MAX = 1000.f;	///< Magnitude of the maximum jerk allowed [m/s³].

	// State
	VelocitySmoothing
	_alt_control_traj_generator;		///< Generates altitude rate and altitude setpoint trajectory when altitude is commanded.
	ManualVelocitySmoothingZ
	_velocity_control_traj_generator;	///< generates height rate trajectory when height rate is commanded
	عائم اچائی_شرح_setpoint_سدھا{NAN}; ///< generated direct height rate setpoint
};

صنف TECSControl
{
عام:
	/**
	 * @brief The control parameters.
	 *
	 */
	بنية Param {
		// Vehicle specific params
		عائم max_sink_شرح;			///< Maximum sink rate (with min throttle and max speed) [m/s].
		عائم min_sink_شرح;			///< Minimum sink rate (with min throttle and trim speed) [m/s].
		عائم max_climb_شرح;			///< Climb rate produced by max allowed throttle [m/s].
		عائم vert_accel_حد_بندی;			///< Magnitude of the maximum vertical acceleration allowed [m/s²].
		عائم equivalent_airspeed_trim;		///< Equivalent cruise airspeed for airspeed less mode [m/s].
		عائم tas_min;				///< True airspeed demand lower limit [m/s].
		عائم tas_stall;			///< True stall airspeed [m/s].
		عائم tas_max;				///< True airspeed demand upper limit [m/s].
		عائم tas_trim;				///< True airspeed trim [m/s].
		عائم pitch_max;			///< Maximum absolute pitch angle setpoint [rad].
		عائم pitch_min;			///< Minimum absolute pitch angle setpoint [rad].
		عائم level_flight_pitch;		///< Pitch angle in level flight at trim airspeed, fed forward at zero climb angle [rad].
		عائم throttle_trim;		///< Normalized throttle required to fly level at calibrated airspeed setpoint [0,1]
		عائم throttle_max;			///< Normalized throttle upper limit.
		عائم throttle_min;			///< Normalized throttle lower limit.

		// Altitude control param
		عائم altitude_error_gain;		///< Altitude error inverse time constant [1/s].
		عائم altitude_setpoint_gain_ff;	///< Gain from altitude demand derivative to demanded climb rate.

		// Airspeed control param
		عائم airspeed_error_gain;				///< Airspeed error inverse time constant [1/s].

		// Energy control param
		عائم ste_شرح_ویلا_const;				///< Filter time constant for specific total energy rate (damping path) [s].
		عائم seb_شرح_ff;					///< Specific energy balance rate feedforward gain.

		// Pitch control param
		عائم pitch_رفتار_بھار;				///< Speed control weighting used by pitch demand calculation.
		عائم integrator_gain_pitch;				///< Integrator gain used by the pitch demand calculation.
		عائم pitch_damping_gain;				///< Damping gain of the pitch demand calculation [s].

		// Throttle control param
		عائم integrator_gain_throttle;				///< Integrator gain used by the throttle demand calculation.
		عائم throttle_damping_gain;				///< Damping gain of the throttle demand calculation [s].
		عائم throttle_slewrate;				///< Throttle demand slew rate limit [1/s].

		عائم لدو_ضارب_correction;				///< Gain from normal load factor increase to total energy rate demand [m²/s³].
		عائم لدو_ضارب;					///< Additional normal load factor.

		عائم fast_descend;
	};

	/**
	 * @brief The debug output
	 *
	 */
	بنية DebugOutput {
		عائم altitude_شرح_control;			///< Altitude rate setpoint from altitude control loop [m/s].
		عائم true_airspeed_derivative_control;		///< Airspeed rate setpoint from airspeed control loop [m/s²].
		عائم کل_energy_شرح_estimate;		///< Total energy rate estimate [m²/s³].
		عائم کل_energy_شرح_sp;			///< Total energy rate setpoint [m²/s³].
		عائم energy_balance_شرح_estimate;		///< Energy balance rate estimate [m²/s³].
		عائم energy_balance_شرح_sp;			///< Energy balance rate setpoint [m²/s³].
		عائم pitch_integrator;				///< Pitch control integrator state, a correction added to the pitch setpoint [rad].
		عائم throttle_integrator;			///< Throttle control integrator state [-].
	};

	/**
	 * @brief Given setpoint to control.
	 *
	 */
	بنية Setpoint {
		TECSAltitudeReferenceModel::AltitudeReferenceState altitude_reference;	///< Altitude/height rate reference.
		عائم altitude_شرح_setpoint_سدھا;					///< Direct height rate setpoint.
		عائم tas_setpoint;						///< True airspeed setpoint.
	};

	/**
	 * @brief Given current measurement from the UAS.
	 *
	 */
	بنية داخلہ_2 {
		عائم altitude;		///< Current altitude amsl of the UAS [m].
		عائم altitude_شرح;	///< Current altitude rate of the UAS [m/s].
		عائم tas;		///< Current true airspeed of the UAS [m/s].
		عائم tas_شرح;		///< Current true airspeed rate of the UAS [m/s²].
	};

	/**
	 * @brief Control flags.
	 *
	 */
	بنية Flag {
		منطقي airspeed_enabled;			///< Flag if the airspeed sensor is enabled.
		منطقي detect_underspeed_enabled;		///< Flag if underspeed detection is enabled.
	};
عام:
	TECSControl() = افتراضي;
	~TECSControl() = افتراضي;
	/**
	 * @brief Initialization of the state.
	 *
	 * @param[in] setpoint is the current setpoint struct.
	 * @param[in] input is the current input measurements.
	 * @param[in] param is the current parameter set.
	 * @param[in] flag is the current activated flags.
	 */
	فراغ initialize(ثابت Setpoint &setpoint, ثابت داخلہ_2 &داخلہ, ثابت Param &param, ثابت Flag &flag);
	/**
	 * @brief Update state and output.
	 *
	 * @param[in] dt is the update time interval in [s].
	 * @param[in] setpoint is the current setpoint struct.
	 * @param[in] input is the current input measurements.
	 * @param[in] param is the current parameter set.
	 * @param[in] flag is the current activated flags.
	 */
	فراغ update(عائم dt, ثابت Setpoint &setpoint, ثابت داخلہ_2 &داخلہ, ثابت Param &param, ثابت Flag &flag);
	/**
	 * @brief Reset the control loop integrals.
	 *
	 */
	فراغ resetIntegrals();
	/**
	 * @brief Get the underspeed ratio.
	 *
	 * @return Ratio of detected underspeed [0,1].
	 */
	عائم get_Underspeed_تناسب() ثابت {أرجع تناسب_undersped;}
	/**
	 * @brief Get the throttle setpoint.
	 *
	 * @return throttle setpoint.
	 */
	عائم getThrottleSetpoint() ثابت {أرجع _throttle_setpoint;}
	/**
	 * @brief Get the pitch setpoint.
	 *
	 * @return The commanded absolute pitch angle [rad].
	 */
	عائم getPitchSetpoint() ثابت {أرجع _pitch_setpoint;}
	/**
	 * @brief Get the Debug Output
	 *
	 * @return the debug output struct.
	 */
	ثابت DebugOutput &getDebugOutput() ثابت { أرجع _debug_output; }

خاص:
	// Allows the regression test to pre-corrupt _pitch_integ_state and verify the
	// safety reset in _calcPitchControlUpdate, which the public update() path cannot
	// reach on its own because the integrator-input guard rejects bad input first.
	FRIEND_TEST(TECSControlTest, PitchIntegratorResetsCorruptedState);

	/**
	 * @brief Specific total energy rate limit.
	 *
	 */
	بنية STERate_حد_بندی {
		عائم STE_شرح_max;	///< Maximum specific total energy rate limit [m²/s³].
		عائم STE_شرح_min;	///< Minimal specific total energy rate limit [m²/s³].
	};

	/**
	 * @brief Control values.
	 * setpoint and current state estimate value as input to a controller.
	 *
	 */
	بنية Control_اقدار {
		عائم setpoint;		///< Control setpoint.
		عائم estimate;		///< Control estimate of current state value.
	};

	/**
	 * @brief Calculated specific energy rates.
	 *
	 */
	بنية SpecificEnergyRates {
		Control_اقدار ske_شرح;	///< Specific kinetic energy rate [m²/s³].
		Control_اقدار spe_شرح;	///< Specific potential energy rate [m²/s³].
	};

	/**
	 * @brief Altitude rate and airspeed setpoints from the outer loops, from which the energy rate setpoints follow.
	 *
	 */
	بنية OuterLoopSetpoints {
		عائم altitude_شرح_setpoint;	///< Controlled altitude rate setpoint [m/s].
		عائم tas_شرح_setpoint;	///< Controlled true airspeed rate setpoint [m/s²].
	};

	/**
	 * @brief Weight factors for specific energy.
	 *
	 */
	بنية SpecificEnergyWeighting {
		عائم spe_weighting;	///< Specific potential energy weight.
		عائم ske_weighting;	///< Specific kinetic energy weight.
	};

خاص:
	/**
	 * @brief Get control error from setpoint and estimate
	 *
	 * @param val is the current control setpoint and estimate.
	 * @return error value
	 */
	ساكن constexpr عائم _getControlError(TECSControl::Control_اقدار val) {أرجع (val.setpoint - val.estimate);}
	/**
	 * @brief Calculate specific total energy rate limits.
	 *
	 * @param[in] param are the control parameters.
	 * @return Specific total energy rate limits in [m²/s³].
	 */
	STERate_حد_بندی calculate_کل_Energy_شرح_حد_بندی(ثابت Param &param) ثابت;
	/**
	 * @brief Gain from specific total energy rate error to throttle.
	 *
	 * @param[in] limit is the specific total energy rate limits in [m²/s³].
	 * @return throttle per specific total energy rate [s³/m²].
	 */
	ساكن عائم ste_شرح_منزل_Throttle(ثابت STERate_حد_بندی &حد_بندی_2) { أرجع 1.0f / (حد_بندی_2.STE_شرح_max - حد_بندی_2.STE_شرح_min); }
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
	عائم _calcAirspeedControlOutput(ثابت Setpoint &setpoint, ثابت داخلہ_2 &داخلہ, ثابت STERate_حد_بندی &حد_بندی_2,
					 ثابت Param &param, ثابت Flag &flag) ثابت;
	/**
	 * @brief calculate altitude control proportional output.
	 *
	 * @param setpoint is the control setpoints.
	 * @param input is the current input measurement of the UAS.
	 * @param param is the control parameters.
	 * @return controlled altitude rate setpoint in [m/s].
	 */
	عائم _calcAltitudeControlOutput(ثابت Setpoint &setpoint, ثابت داخلہ_2 &داخلہ, ثابت Param &param) ثابت;
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
	SpecificEnergyRates _updateOuterLoops(ثابت Setpoint &setpoint, ثابت داخلہ_2 &داخلہ, ثابت STERate_حد_بندی &حد_بندی_2,
					      ثابت Param &param, ثابت Flag &flag);
	/**
	 * @brief Calculate specific energy rates.
	 *
	 * @param control_setpoint is the controlled altitude and airspeed rate setpoints.
	 * @param input is the current input measurement of the UAS.
	 * @return Specific energy rates in [m²/s³].
	 */
	SpecificEnergyRates _calcSpecificEnergyRates(ثابت OuterLoopSetpoints &control_setpoint, ثابت داخلہ_2 &داخلہ) ثابت;
	/**
	 * @brief Detect underspeed.
	 *
	 * @param input is the current input measurement of the UAS.
	 * @param param is the control parameters.
	 * @param flag is the control flags.
	 */
	فراغ _detectUnderspeed(ثابت داخلہ_2 &داخلہ, ثابت Param &param, ثابت Flag &flag);
	/**
	 * @brief Update specific energy balance weights.
	 *
	 * @param param is the control parameters.
	 * @param flag is the control flags.
	 * @return Weights used for the specific energy balance.
	 */
	SpecificEnergyWeighting update_رفتار_Altitude_Weights(ثابت Param &param, ثابت Flag &flag);
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
	عائم calc_Pitch_Control_Ske_شرح_Setpoint(ثابت Setpoint &setpoint, ثابت داخلہ_2 &داخلہ, ثابت STERate_حد_بندی &حد_بندی_2,
					       ثابت Param &param, ثابت Flag &flag, ثابت SpecificEnergyWeighting &بھار) ثابت;
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
	SpecificEnergyRates _calcPitchControlEnergyRates(ثابت Setpoint &setpoint, ثابت داخلہ_2 &داخلہ,
			ثابت STERate_حد_بندی &حد_بندی_2, ثابت SpecificEnergyRates &specific_energy_rates, ثابت Param &param,
			ثابت Flag &flag, ثابت SpecificEnergyWeighting &بھار) ثابت;
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
	فراغ _calcPitchControl(عائم dt, ثابت Setpoint &setpoint, ثابت داخلہ_2 &داخلہ, ثابت STERate_حد_بندی &حد_بندی_2,
			       ثابت SpecificEnergyRates &specific_energy_شرح, ثابت Param &param, ثابت Flag &flag);

	/**
	 * @brief Calculate pitch control specific energy balance rates.
	 *
	 * @param weight is the weighting use of the potential and kinetic energy.
	 * @param specific_energy_rate is the specific energy rates in [m²/s³].
	 * @return specific energy balance rate values in [m²/s³].
	 */
	Control_اقدار calc_Pitch_Control_Seb_شرح(ثابت SpecificEnergyWeighting &بھار,
					       ثابت SpecificEnergyRates &specific_energy_شرح) ثابت;

	/**
	 * @brief Calculate the pitch control update function.
	 * Update the states of the pitch control (pitch integrator).
	 *
	 * @param dt is the update time interval in [s].
	 * @param input is the current input measurement of the UAS.
	 * @param seb_rate is the specific energy balance rate in [m²/s³].
	 * @param param is the control parameters.
	 */
	فراغ _calcPitchControlUpdate(عائم dt, ثابت داخلہ_2 &داخلہ, ثابت Control_اقدار &seb_شرح, ثابت Param &param);

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
	ساكن عائم climb_Angle_منزل_Seb_شرح(ثابت داخلہ_2 &داخلہ, ثابت Param &param);

	/**
	 * @brief Calculate the pitch control output function.
	 *
	 * @param input is the current input measurement of the UAS.
	 * @param seb_rate is the specific energy balance rate in [m²/s³].
	 * @param param is the control parameters.
	 * @return absolute pitch setpoint [rad].
	 */
	عائم _calcPitchControlOutput(ثابت داخلہ_2 &داخلہ, ثابت Control_اقدار &seb_شرح, ثابت Param &param) ثابت;

	/**
	 * @brief Update controlled throttle setpoint.
	 *
	 * @param dt is the update time interval in [s].
	 * @param limit is the specific total energy rate limits in [m²/s³].
	 * @param specific_energy_rate is the calculated specific energy.
	 * @param param is the control parameters.
	 * @param flag is the control flags.
	 */
	فراغ _calcThrottleControl(عائم dt, ثابت STERate_حد_بندی &حد_بندی_2, ثابت SpecificEnergyRates &specific_energy_شرح,
				  ثابت Param &param, ثابت Flag &flag);

	/**
	 * @brief Calculate throttle control specific total energy
	 *
	 * @param limit is the specific total energy rate limits in [m²/s³].
	 * @param specific_energy_rate is the specific energy rates in [m²/s³].
	 * @param param is the control parameters.
	 * @return specific total energy rate values in [m²/s³]
	 */
	Control_اقدار calc_Throttle_Control_Ste_شرح(ثابت STERate_حد_بندی &حد_بندی_2, ثابت SpecificEnergyRates &specific_energy_شرح,
			ثابت Param &param) ثابت;

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
	فراغ _calcThrottleControlUpdate(عائم dt, ثابت STERate_حد_بندی &حد_بندی_2, ثابت Control_اقدار &ste_شرح, ثابت Param &param,
					ثابت Flag &flag);

	/**
	 * @brief Calculate the throttle control output function.
	 *
	 * @param limit is the specific total energy rate limits in [m²/s³].
	 * @param ste_rate is the specific total energy rates in [m²/s³].
	 * @param param is the control parameters.
	 * @param flag is the control flags.
	 * @return throttle setpoint in [0,1].
	 */
	عائم _calcThrottleControlOutput(ثابت STERate_حد_بندی &حد_بندی_2, ثابت Control_اقدار &ste_شرح, ثابت Param &param,
					 ثابت Flag &flag) ثابت;

خاص:
	// State
	AlphaFilter<عائم> ste_شرح_estimate_filter;		///< Low pass filter for the specific total energy rate.
	عائم _pitch_integ_state{0.0f};				///< Pitch integrator state [rad].
	عائم _fast_descend_prev{0.0f};				///< Fast descend ratio of the last update [0,1].
	عائم _throttle_integ_state{0.0f};			///< Throttle integrator state [-].

	// Output
	DebugOutput _debug_output;				///< Debug output.
	عائم _pitch_setpoint{0.0f};				///< Controlled absolute pitch setpoint [rad].
	عائم _throttle_setpoint{0.0f};				///< Controlled throttle setpoint [0,1].
	عائم تناسب_undersped{0.0f};				///< A continuous representation of how "undersped" the TAS is [0,1]
};

صنف TECS
{
عام:
	بنية DebugOutput {
		TECSControl::DebugOutput control;
		عائم true_airspeed_sp;
		عائم true_airspeed_filtered;
		عائم true_airspeed_derivative;
		عائم altitude_reference;
		عائم اچائی_شرح_reference;
		عائم اچائی_شرح_سدھا;
		عائم fast_descend;
	};

	/**
	 * @brief Vehicle state, setpoints and limits for one control update.
	 *
	 */
	بنية داخلہ_2 {
		عائم altitude;				///< Current altitude amsl [m].
		عائم altitude_شرح;			///< Current altitude rate [m/s].
		عائم altitude_setpoint;		///< Altitude setpoint amsl [m], NAN if not set.
		عائم altitude_شرح_setpoint{NAN};	///< Altitude rate setpoint [m/s], NAN if not set.
		عائم equivalent_airspeed;		///< Current equivalent airspeed [m/s].
		عائم رفتار_deriv_forward;		///< Current forward acceleration, used as true airspeed rate [m/s²].
		عائم equivalent_airspeed_setpoint;	///< Equivalent airspeed setpoint [m/s].
		عائم eas_منزل_tas;			///< Conversion factor from equivalent to true airspeed [-].
		عائم throttle_min;			///< Normalized throttle lower limit.
		عائم throttle_max;			///< Normalized throttle upper limit.
		عائم throttle_trim;			///< Normalized throttle required to fly level at the airspeed setpoint.
		عائم pitch_min;			///< Minimum absolute pitch angle setpoint [rad].
		عائم pitch_max;			///< Maximum absolute pitch angle setpoint [rad].
		عائم نشانہ_climbrate;			///< Climb rate the reference model targets [m/s].
		عائم نشانہ_sinkrate;			///< Sink rate the reference model targets [m/s].
	};

عام:
	TECS() = افتراضي;
	~TECS() = افتراضي;

	// no copy, assignment, move, move assignment
	TECS(ثابت TECS &) = مٹاؤ;
	TECS &عامل=(ثابت TECS &) = مٹاؤ;
	TECS(TECS &&) = مٹاؤ;
	TECS &عامل=(TECS &&) = مٹاؤ;

	ثابت DebugOutput &get_حالت() ثابت { أرجع debug_حالت; }

	/**
	 * Set the airspeed enable state
	 *
	 * @param[in] enabled is true if the airspeed measurement is used for control.
	 */
	فراغ enable_airspeed(منطقي enabled) { _control_flag.airspeed_enabled = enabled; }

	/**
	 * @brief Update the control loop calculations
	 *
	 * All pitch angles, including the pitch limits and the resulting pitch setpoint, are absolute: the angle of the
	 * body x-axis above the horizon.
	 *
	 * @param[in] input is the current vehicle state, setpoints and limits.
	 */
	فراغ update(ثابت داخلہ_2 &داخلہ);

	فراغ resetIntegrals()
	{
		_control.resetIntegrals();
	}

	فراغ سیٹ_detect_underspeed_enabled(منطقي enabled) { _control_flag.detect_underspeed_enabled = enabled; }

	// setters for parameters

	فراغ سیٹ_integrator_gain_throttle(عائم gain) { _control_param.integrator_gain_throttle = gain;}
	فراغ سیٹ_integrator_gain_pitch(عائم gain) { _control_param.integrator_gain_pitch = gain; }

	فراغ سیٹ_max_sink_شرح(عائم max_sink_شرح) { _control_param.max_sink_شرح = max_sink_شرح; _reference_param.max_sink_شرح = max_sink_شرح; }
	فراغ سیٹ_min_sink_شرح(عائم min_sink_شرح) { _control_param.min_sink_شرح = min_sink_شرح; }
	فراغ سیٹ_max_climb_شرح(عائم climb_شرح) { _control_param.max_climb_شرح = climb_شرح; _reference_param.max_climb_شرح = climb_شرح; }

	فراغ سیٹ_altitude_شرح_ff(عائم altitude_شرح_ff) { _control_param.altitude_setpoint_gain_ff = altitude_شرح_ff; }
	فراغ سیٹ_altitude_error_ویلا_constant(عائم ویلا_const) { _control_param.altitude_error_gain = 1.0f / math::max(ویلا_const, 0.1f); }
	فراغ سیٹ_fast_descend_altitude_error(عائم altitude_error) { _fast_descend_alt_err = altitude_error; }

	فراغ سیٹ_equivalent_airspeed_min(عائم airspeed) { _equivalent_airspeed_min = airspeed; }
	فراغ سیٹ_equivalent_airspeed_stall(عائم airspeed) { _equivalent_airspeed_stall = airspeed; }
	فراغ سیٹ_equivalent_airspeed_max(عائم airspeed) { _equivalent_airspeed_max = airspeed; }
	فراغ سیٹ_equivalent_airspeed_trim(عائم airspeed) { _control_param.equivalent_airspeed_trim = airspeed; _airspeed_filter_param.equivalent_airspeed_trim = airspeed; }

	فراغ سیٹ_pitch_damping(عائم damping) { _control_param.pitch_damping_gain = damping; }
	فراغ سیٹ_vertical_accel_حد_بندی(عائم حد_بندی_2) { _reference_param.vert_accel_حد_بندی = حد_بندی_2; _control_param.vert_accel_حد_بندی = حد_بندی_2; }

	فراغ سیٹ_رفتار_بھار(عائم بھار) { _control_param.pitch_رفتار_بھار = بھار; }
	فراغ سیٹ_airspeed_error_ویلا_constant(عائم ویلا_const) { _control_param.airspeed_error_gain = 1.0f / math::max(ویلا_const, 0.1f); }

	فراغ سیٹ_throttle_damp(عائم throttle_damp) { _control_param.throttle_damping_gain = throttle_damp; }
	فراغ سیٹ_throttle_slewrate(عائم slewrate) { _control_param.throttle_slewrate = slewrate; }

	فراغ سیٹ_roll_throttle_compensation(عائم compensation) { _control_param.لدو_ضارب_correction = compensation; }
	فراغ سیٹ_لدو_ضارب(عائم لدو_ضارب) { _control_param.لدو_ضارب = لدو_ضارب; }

	فراغ سیٹ_ste_شرح_ویلا_const(عائم ویلا_const) { _control_param.ste_شرح_ویلا_const = ویلا_const; }

	فراغ سیٹ_seb_شرح_ff_gain(عائم ff_gain) { _control_param.seb_شرح_ff = ff_gain; }

	/**
	 * Set the pitch angle in level flight at trim airspeed [rad], fed forward as the pitch at zero climb angle
	 */
	فراغ سیٹ_level_flight_pitch(عائم pitch) { _control_param.level_flight_pitch = pitch; }

	/**
	 * Handle the altitude reset
	 *
	 * If the estimation system resets the height in one discrete step this
	 * will gracefully even out the reset over time.
	 *
	 * @param[in] altitude is the altitude amsl after the reset [m].
	 * @param[in] altitude_rate is the current altitude rate [m/s].
	 */
	فراغ handle_alt_قدم(عائم altitude, عائم altitude_شرح)
	{
		TECSAltitudeReferenceModel::AltitudeReferenceState init_state{ .alt = altitude,
				.alt_شرح = altitude_شرح};

		// reset altitude reference model.
		_altitude_reference_model.initialize(init_state);
	}

	/**
	 * Returns the absolute pitch setpoint [rad]
	 */
	عائم get_pitch_setpoint() ثابت {أرجع _control.getPitchSetpoint();}
	عائم get_throttle_setpoint() ثابت {أرجع _control.getThrottleSetpoint();}

	/**
	 * Returns the altitude tracking time constant
	 */
	عائم get_altitude_error_ویلا_constant() ثابت { أرجع 1.0f / _control_param.altitude_error_gain; }

	عائم get_underspeed_تناسب() ثابت { أرجع _control.get_Underspeed_تناسب(); }

خاص:
	/**
	 * @brief Initialize the control parameters
	 *
	 * @param[in] input is the current vehicle state, setpoints and limits.
	 */
	فراغ initControlParams(ثابت داخلہ_2 &داخلہ);

	/**
	 * @brief calculate true airspeed setpoint
	 *
	 * Calculate true airspeed setpoint based on input and fast descend ratio
	 *
	 * @param eas_to_tas is the eas to tas conversion factor
	 * @param eas_setpoint is the desired equivalent airspeed setpoint [m/s]
	 * @return true airspeed setpoint[m/s]
	 */
	عائم calcTrueAirspeedSetpoint(عائم eas_منزل_tas, عائم eas_setpoint);

	/**
	 * @brief Initialize the control loop
	 *
	 * @param[in] input is the current vehicle state, setpoints and limits.
	 */
	فراغ initialize(ثابت داخلہ_2 &داخلہ);

	TECSControl 			_control;			///< Control submodule.
	TECSAirspeedFilter 		_airspeed_filter;		///< Airspeed filter submodule.
	TECSAltitudeReferenceModel 	_altitude_reference_model;	///< Setpoint reference model submodule.

	hrt_abstime _update_timestamp{0};				///< last timestamp of the update function call.

	عائم _equivalent_airspeed_min{10.0f};				///< equivalent airspeed demand lower limit (m/sec)
	عائم _equivalent_airspeed_stall{7.0f};				///< equivalent stall airspeed (m/sec)
	عائم _equivalent_airspeed_max{20.0f};				///< equivalent airspeed demand upper limit (m/sec)
	عائم _fast_descend_alt_err{-1.f};	 				///< Altitude difference between current altitude to altitude setpoint needed to descend with higher airspeed [m].
	عائم _fast_descend{0.f};					///< Value for fast descend in [0,1]. continuous value used to flatten the high speed value out when close to target altitude.
	hrt_abstime _enabled_fast_descend_timestamp{0U};		///< timestamp at activation of fast descend mode

	ساكن constexpr عائم DT_MIN = 0.001f;				///< minimum allowed value of _dt (sec)
	ساكن constexpr عائم DT_MAX = 1.0f;				///< max value of _dt allowed before a filter state reset is performed (sec)
	ساكن constexpr hrt_abstime FAST_DESCEND_RAMP_اتے_ویلا = 2_s; 	///< Ramp up time until fast descend is fully engaged
	ساكن constexpr عائم FAST_DESCEND_اچائی_MARGIN = 5.f;	///< Height above setpoint where fast descend ends [m]

	DebugOutput debug_حالت{};

	// Params
	/// Airspeed filter parameters.
	TECSAirspeedFilter::Param _airspeed_filter_param{
		.equivalent_airspeed_trim = 15.0f,
	};
	/// Reference model parameters.
	TECSAltitudeReferenceModel::Param _reference_param{
		.نشانہ_climbrate = 2.0f,
		.نشانہ_sinkrate = 2.0f,
		.vert_accel_حد_بندی = 0.0f,
		.max_climb_شرح = 2.0f,
		.max_sink_شرح = 2.0f,
	};
	/// Control parameters.
	TECSControl::Param _control_param{
		.max_sink_شرح = 5.0f,
		.min_sink_شرح = 2.0f,
		.max_climb_شرح = 5.0f,
		.vert_accel_حد_بندی = 0.0f,
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
		.ste_شرح_ویلا_const = 0.1f,
		.seb_شرح_ff = 1.0f,
		.pitch_رفتار_بھار = 1.0f,
		.integrator_gain_pitch = 0.0f,
		.pitch_damping_gain = 0.0f,
		.integrator_gain_throttle = 0.0f,
		.throttle_damping_gain = 0.0f,
		.throttle_slewrate = 0.0f,
		.لدو_ضارب_correction = 0.0f,
		.لدو_ضارب = 1.0f,
		.fast_descend = 0.f
	};

	TECSControl::Flag _control_flag{
		.airspeed_enabled = خطأ,
		.detect_underspeed_enabled = خطأ,
	};

	/**
	 * @brief Set fast descend value
	 *
	 * @param alt_setpoint is the altitude setpoint
	 * @param alt is the current altitude
	 */
	فراغ سیٹ_Fast_Descend(عائم alt_setpoint, عائم alt);
};

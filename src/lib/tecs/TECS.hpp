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

#شامل <drivers/drv_hrt.h>
#شامل <mathlib/mathlib.h>
#شامل <matrix/math.hpp>
#شامل <lib/mathlib/math/filter/AlphaFilter.hpp>
#شامل <motion_planning/VelocitySmoothing.hpp>
#شامل <motion_planning/ManualVelocitySmoothingZ.hpp>

استعمال نامخلا وقت_literals;

#اگر_غیر_تعریف_شدہ FRIEND_TEST // for gtest
#تعریف_کرو FRIEND_TEST(a, b)
#اگر_ختم

جماعت TECSAirspeedFilter
{
عوامی:
	/**
	 * @brief State of the equivalent airspeed filter.
	 *
	 */
	ساخت AirspeedFilterState {
		اعشاری رفتار;		///< speed of the air in EAS [m/s]
		اعشاری رفتار_شرح;	///< rate of speed of the air [m/s²]
	};

	/**
	 * @brief Parameters of the airspeed filter.
	 *
	 */
	ساخت Param {
		اعشاری equivalent_airspeed_trim; 	///< the trim value of the equivalent airspeed [m/s].
	};

	/**
	 * @brief Input, which will be filtered.
	 *
	 */
	ساخت داخلہ_2 {
		اعشاری equivalent_airspeed; 	///< the measured equivalent airspeed in [m/s].
		اعشاری equivalent_airspeed_شرح;	///< the measured rate of equivalent airspeed in [m/s²].
	};
عوامی:
	TECSAirspeedFilter() = طےشدہ;
	~TECSAirspeedFilter() = طےشدہ;
	/**
	 * @brief Initialize filter
	 *
	 * @param[in] equivalent_airspeed is the equivalent airspeed in [m/s].
	 * @param[in] equivalent_airspeed_trim is the equivalent airspeed trim (vehicle setting) in [m/s].
	 * @param[in] airspeed_sensor_available boolean if the airspeed sensor is available.
	 */
	خالی initialize(اعشاری equivalent_airspeed, ثابت اعشاری equivalent_airspeed_trim,
			ثابت منطقی airspeed_sensor_دستیاب);

	/**
	 * @brief Update filter
	 *
	 * @param[in] dt is the timestep in [s].
	 * @param[in] input are the raw measured values.
	 * @param[in] param are the filter parameters.
	 * @param[in] airspeed_sensor_available boolean if the airspeed sensor is available.
	 */
	خالی update(اعشاری dt, ثابت داخلہ_2 &داخلہ, ثابت Param &param, ثابت منطقی airspeed_sensor_دستیاب);

	/**
	 * @brief Get the filtered airspeed states.
	 *
	 * @return Current state of the airspeed filter.
	 */
	AirspeedFilterState getState() ثابت;

نجی:
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
	ساکن constexpr اعشاری kKg00{1.91940287f};
	ساکن constexpr اعشاری kKg01{0.25925926f};
	ساکن constexpr اعشاری kKg10{2.11640212f};
	ساکن constexpr اعشاری kKg11{0.67179101f};
};

جماعت TECSAltitudeReferenceModel
{
عوامی:
	/**
	 * @brief Altitude reference state.
	 *
	 */
	ساخت AltitudeReferenceState {
		اعشاری alt; 	///< Reference altitude amsl in [m].
		اعشاری alt_شرح;	///< Reference altitude rate in [m/s].
	};

	/**
	 * @brief Parameters for the reference model.
	 *
	 */
	ساخت Param {
		اعشاری ہدف_climbrate;	///< The target climbrate in [m/s].
		اعشاری ہدف_sinkrate;	///< The target sinkrate in [m/s].
		اعشاری vert_accel_حد_بندی;	///< Magnitude of the maximum vertical acceleration allowed [m/s²].
		اعشاری max_climb_شرح;	///< Climb rate produced by max allowed throttle [m/s].
		اعشاری max_sink_شرح;	///< Maximum sink rate (with min throttle, max speed) [m/s].
	};

عوامی:
	TECSAltitudeReferenceModel() = طےشدہ;
	~TECSAltitudeReferenceModel() = طےشدہ;

	/**
	 * @brief Initialize reference models.
	 *
	 * @param[in] state is the current altitude state of the vehicle.
	 */
	خالی initialize(ثابت AltitudeReferenceState &state);

	/**
	 * @brief Update reference models.
	 *
	 * @param[in] dt is the update interval in [s].
	 * @param[in] setpoint are the desired setpoints.
	 * @param[in] altitude is the altitude amsl in [m].
	 * @param[in] height_rate is the current height rate in [m/s].
	 * @param[in] param are the reference model parameters.
	 */
	خالی update(اعشاری dt, ثابت AltitudeReferenceState &setpoint, اعشاری altitude, اعشاری اونچائی_شرح, ثابت Param &param);

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
	اعشاری get_اونچائی_شرح_Setpoint_براہ_راست() ثابت {واپس اونچائی_شرح_setpoint_براہ_راست; }


نجی:
	ساکن constexpr اعشاری JERK_MAX = 1000.f;	///< Magnitude of the maximum jerk allowed [m/s³].

	// State
	VelocitySmoothing
	_alt_control_traj_generator;		///< Generates altitude rate and altitude setpoint trajectory when altitude is commanded.
	ManualVelocitySmoothingZ
	_velocity_control_traj_generator;	///< generates height rate trajectory when height rate is commanded
	اعشاری اونچائی_شرح_setpoint_براہ_راست{NAN}; ///< generated direct height rate setpoint
};

جماعت TECSControl
{
عوامی:
	/**
	 * @brief The control parameters.
	 *
	 */
	ساخت Param {
		// Vehicle specific params
		اعشاری max_sink_شرح;			///< Maximum sink rate (with min throttle and max speed) [m/s].
		اعشاری min_sink_شرح;			///< Minimum sink rate (with min throttle and trim speed) [m/s].
		اعشاری max_climb_شرح;			///< Climb rate produced by max allowed throttle [m/s].
		اعشاری vert_accel_حد_بندی;			///< Magnitude of the maximum vertical acceleration allowed [m/s²].
		اعشاری equivalent_airspeed_trim;		///< Equivalent cruise airspeed for airspeed less mode [m/s].
		اعشاری tas_min;				///< True airspeed demand lower limit [m/s].
		اعشاری tas_stall;			///< True stall airspeed [m/s].
		اعشاری tas_max;				///< True airspeed demand upper limit [m/s].
		اعشاری tas_trim;				///< True airspeed trim [m/s].
		اعشاری pitch_max;			///< Maximum absolute pitch angle setpoint [rad].
		اعشاری pitch_min;			///< Minimum absolute pitch angle setpoint [rad].
		اعشاری level_flight_pitch;		///< Pitch angle in level flight at trim airspeed, fed forward at zero climb angle [rad].
		اعشاری throttle_trim;		///< Normalized throttle required to fly level at calibrated airspeed setpoint [0,1]
		اعشاری throttle_max;			///< Normalized throttle upper limit.
		اعشاری throttle_min;			///< Normalized throttle lower limit.

		// Altitude control param
		اعشاری altitude_error_gain;		///< Altitude error inverse time constant [1/s].
		اعشاری altitude_setpoint_gain_ff;	///< Gain from altitude demand derivative to demanded climb rate.

		// Airspeed control param
		اعشاری airspeed_error_gain;				///< Airspeed error inverse time constant [1/s].

		// Energy control param
		اعشاری ste_شرح_وقت_const;				///< Filter time constant for specific total energy rate (damping path) [s].
		اعشاری seb_شرح_ff;					///< Specific energy balance rate feedforward gain.

		// Pitch control param
		اعشاری pitch_رفتار_وزن;				///< Speed control weighting used by pitch demand calculation.
		اعشاری integrator_gain_pitch;				///< Integrator gain used by the pitch demand calculation.
		اعشاری pitch_damping_gain;				///< Damping gain of the pitch demand calculation [s].

		// Throttle control param
		اعشاری integrator_gain_throttle;				///< Integrator gain used by the throttle demand calculation.
		اعشاری throttle_damping_gain;				///< Damping gain of the throttle demand calculation [s].
		اعشاری throttle_slewrate;				///< Throttle demand slew rate limit [1/s].

		اعشاری بھرو_ضربی_عدد_correction;				///< Gain from normal load factor increase to total energy rate demand [m²/s³].
		اعشاری بھرو_ضربی_عدد;					///< Additional normal load factor.

		اعشاری fast_descend;
	};

	/**
	 * @brief The debug output
	 *
	 */
	ساخت DebugOutput {
		اعشاری altitude_شرح_control;			///< Altitude rate setpoint from altitude control loop [m/s].
		اعشاری true_airspeed_derivative_control;		///< Airspeed rate setpoint from airspeed control loop [m/s²].
		اعشاری کل_energy_شرح_estimate;		///< Total energy rate estimate [m²/s³].
		اعشاری کل_energy_شرح_sp;			///< Total energy rate setpoint [m²/s³].
		اعشاری energy_balance_شرح_estimate;		///< Energy balance rate estimate [m²/s³].
		اعشاری energy_balance_شرح_sp;			///< Energy balance rate setpoint [m²/s³].
		اعشاری pitch_integrator;				///< Pitch control integrator state, a correction added to the pitch setpoint [rad].
		اعشاری throttle_integrator;			///< Throttle control integrator state [-].
	};

	/**
	 * @brief Given setpoint to control.
	 *
	 */
	ساخت Setpoint {
		TECSAltitudeReferenceModel::AltitudeReferenceState altitude_reference;	///< Altitude/height rate reference.
		اعشاری altitude_شرح_setpoint_براہ_راست;					///< Direct height rate setpoint.
		اعشاری tas_setpoint;						///< True airspeed setpoint.
	};

	/**
	 * @brief Given current measurement from the UAS.
	 *
	 */
	ساخت داخلہ_2 {
		اعشاری altitude;		///< Current altitude amsl of the UAS [m].
		اعشاری altitude_شرح;	///< Current altitude rate of the UAS [m/s].
		اعشاری tas;		///< Current true airspeed of the UAS [m/s].
		اعشاری tas_شرح;		///< Current true airspeed rate of the UAS [m/s²].
	};

	/**
	 * @brief Control flags.
	 *
	 */
	ساخت Flag {
		منطقی airspeed_enabled;			///< Flag if the airspeed sensor is enabled.
		منطقی detect_underspeed_enabled;		///< Flag if underspeed detection is enabled.
	};
عوامی:
	TECSControl() = طےشدہ;
	~TECSControl() = طےشدہ;
	/**
	 * @brief Initialization of the state.
	 *
	 * @param[in] setpoint is the current setpoint struct.
	 * @param[in] input is the current input measurements.
	 * @param[in] param is the current parameter set.
	 * @param[in] flag is the current activated flags.
	 */
	خالی initialize(ثابت Setpoint &setpoint, ثابت داخلہ_2 &داخلہ, ثابت Param &param, ثابت Flag &flag);
	/**
	 * @brief Update state and output.
	 *
	 * @param[in] dt is the update time interval in [s].
	 * @param[in] setpoint is the current setpoint struct.
	 * @param[in] input is the current input measurements.
	 * @param[in] param is the current parameter set.
	 * @param[in] flag is the current activated flags.
	 */
	خالی update(اعشاری dt, ثابت Setpoint &setpoint, ثابت داخلہ_2 &داخلہ, ثابت Param &param, ثابت Flag &flag);
	/**
	 * @brief Reset the control loop integrals.
	 *
	 */
	خالی resetIntegrals();
	/**
	 * @brief Get the underspeed ratio.
	 *
	 * @return Ratio of detected underspeed [0,1].
	 */
	اعشاری get_Underspeed_تناسب() ثابت {واپس تناسب_undersped;}
	/**
	 * @brief Get the throttle setpoint.
	 *
	 * @return throttle setpoint.
	 */
	اعشاری getThrottleSetpoint() ثابت {واپس _throttle_setpoint;}
	/**
	 * @brief Get the pitch setpoint.
	 *
	 * @return The commanded absolute pitch angle [rad].
	 */
	اعشاری getPitchSetpoint() ثابت {واپس _pitch_setpoint;}
	/**
	 * @brief Get the Debug Output
	 *
	 * @return the debug output struct.
	 */
	ثابت DebugOutput &getDebugOutput() ثابت { واپس _debug_output; }

نجی:
	// Allows the regression test to pre-corrupt _pitch_integ_state and verify the
	// safety reset in _calcPitchControlUpdate, which the public update() path cannot
	// reach on its own because the integrator-input guard rejects bad input first.
	FRIEND_TEST(TECSControlTest, PitchIntegratorResetsCorruptedState);

	/**
	 * @brief Specific total energy rate limit.
	 *
	 */
	ساخت STERate_حد_بندی {
		اعشاری STE_شرح_max;	///< Maximum specific total energy rate limit [m²/s³].
		اعشاری STE_شرح_min;	///< Minimal specific total energy rate limit [m²/s³].
	};

	/**
	 * @brief Control values.
	 * setpoint and current state estimate value as input to a controller.
	 *
	 */
	ساخت Control_اقدار {
		اعشاری setpoint;		///< Control setpoint.
		اعشاری estimate;		///< Control estimate of current state value.
	};

	/**
	 * @brief Calculated specific energy rates.
	 *
	 */
	ساخت SpecificEnergyRates {
		Control_اقدار ske_شرح;	///< Specific kinetic energy rate [m²/s³].
		Control_اقدار spe_شرح;	///< Specific potential energy rate [m²/s³].
	};

	/**
	 * @brief Altitude rate and airspeed setpoints from the outer loops, from which the energy rate setpoints follow.
	 *
	 */
	ساخت OuterLoopSetpoints {
		اعشاری altitude_شرح_setpoint;	///< Controlled altitude rate setpoint [m/s].
		اعشاری tas_شرح_setpoint;	///< Controlled true airspeed rate setpoint [m/s²].
	};

	/**
	 * @brief Weight factors for specific energy.
	 *
	 */
	ساخت SpecificEnergyWeighting {
		اعشاری spe_weighting;	///< Specific potential energy weight.
		اعشاری ske_weighting;	///< Specific kinetic energy weight.
	};

نجی:
	/**
	 * @brief Get control error from setpoint and estimate
	 *
	 * @param val is the current control setpoint and estimate.
	 * @return error value
	 */
	ساکن constexpr اعشاری _getControlError(TECSControl::Control_اقدار val) {واپس (val.setpoint - val.estimate);}
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
	ساکن اعشاری ste_شرح_مقصد_Throttle(ثابت STERate_حد_بندی &حد_بندی_2) { واپس 1.0f / (حد_بندی_2.STE_شرح_max - حد_بندی_2.STE_شرح_min); }
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
	اعشاری _calcAirspeedControlOutput(ثابت Setpoint &setpoint, ثابت داخلہ_2 &داخلہ, ثابت STERate_حد_بندی &حد_بندی_2,
					 ثابت Param &param, ثابت Flag &flag) ثابت;
	/**
	 * @brief calculate altitude control proportional output.
	 *
	 * @param setpoint is the control setpoints.
	 * @param input is the current input measurement of the UAS.
	 * @param param is the control parameters.
	 * @return controlled altitude rate setpoint in [m/s].
	 */
	اعشاری _calcAltitudeControlOutput(ثابت Setpoint &setpoint, ثابت داخلہ_2 &داخلہ, ثابت Param &param) ثابت;
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
	خالی _detectUnderspeed(ثابت داخلہ_2 &داخلہ, ثابت Param &param, ثابت Flag &flag);
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
	اعشاری calc_Pitch_Control_Ske_شرح_Setpoint(ثابت Setpoint &setpoint, ثابت داخلہ_2 &داخلہ, ثابت STERate_حد_بندی &حد_بندی_2,
					       ثابت Param &param, ثابت Flag &flag, ثابت SpecificEnergyWeighting &وزن) ثابت;
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
			ثابت Flag &flag, ثابت SpecificEnergyWeighting &وزن) ثابت;
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
	خالی _calcPitchControl(اعشاری dt, ثابت Setpoint &setpoint, ثابت داخلہ_2 &داخلہ, ثابت STERate_حد_بندی &حد_بندی_2,
			       ثابت SpecificEnergyRates &specific_energy_شرح, ثابت Param &param, ثابت Flag &flag);

	/**
	 * @brief Calculate pitch control specific energy balance rates.
	 *
	 * @param weight is the weighting use of the potential and kinetic energy.
	 * @param specific_energy_rate is the specific energy rates in [m²/s³].
	 * @return specific energy balance rate values in [m²/s³].
	 */
	Control_اقدار calc_Pitch_Control_Seb_شرح(ثابت SpecificEnergyWeighting &وزن,
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
	خالی _calcPitchControlUpdate(اعشاری dt, ثابت داخلہ_2 &داخلہ, ثابت Control_اقدار &seb_شرح, ثابت Param &param);

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
	ساکن اعشاری climb_Angle_مقصد_Seb_شرح(ثابت داخلہ_2 &داخلہ, ثابت Param &param);

	/**
	 * @brief Calculate the pitch control output function.
	 *
	 * @param input is the current input measurement of the UAS.
	 * @param seb_rate is the specific energy balance rate in [m²/s³].
	 * @param param is the control parameters.
	 * @return absolute pitch setpoint [rad].
	 */
	اعشاری _calcPitchControlOutput(ثابت داخلہ_2 &داخلہ, ثابت Control_اقدار &seb_شرح, ثابت Param &param) ثابت;

	/**
	 * @brief Update controlled throttle setpoint.
	 *
	 * @param dt is the update time interval in [s].
	 * @param limit is the specific total energy rate limits in [m²/s³].
	 * @param specific_energy_rate is the calculated specific energy.
	 * @param param is the control parameters.
	 * @param flag is the control flags.
	 */
	خالی _calcThrottleControl(اعشاری dt, ثابت STERate_حد_بندی &حد_بندی_2, ثابت SpecificEnergyRates &specific_energy_شرح,
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
	خالی _calcThrottleControlUpdate(اعشاری dt, ثابت STERate_حد_بندی &حد_بندی_2, ثابت Control_اقدار &ste_شرح, ثابت Param &param,
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
	اعشاری _calcThrottleControlOutput(ثابت STERate_حد_بندی &حد_بندی_2, ثابت Control_اقدار &ste_شرح, ثابت Param &param,
					 ثابت Flag &flag) ثابت;

نجی:
	// State
	AlphaFilter<اعشاری> ste_شرح_estimate_filter;		///< Low pass filter for the specific total energy rate.
	اعشاری _pitch_integ_state{0.0f};				///< Pitch integrator state [rad].
	اعشاری fast_descend_پچھلا{0.0f};				///< Fast descend ratio of the last update [0,1].
	اعشاری _throttle_integ_state{0.0f};			///< Throttle integrator state [-].

	// Output
	DebugOutput _debug_output;				///< Debug output.
	اعشاری _pitch_setpoint{0.0f};				///< Controlled absolute pitch setpoint [rad].
	اعشاری _throttle_setpoint{0.0f};				///< Controlled throttle setpoint [0,1].
	اعشاری تناسب_undersped{0.0f};				///< A continuous representation of how "undersped" the TAS is [0,1]
};

جماعت TECS
{
عوامی:
	ساخت DebugOutput {
		TECSControl::DebugOutput control;
		اعشاری true_airspeed_sp;
		اعشاری true_airspeed_filtered;
		اعشاری true_airspeed_derivative;
		اعشاری altitude_reference;
		اعشاری اونچائی_شرح_reference;
		اعشاری اونچائی_شرح_براہ_راست;
		اعشاری fast_descend;
	};

	/**
	 * @brief Vehicle state, setpoints and limits for one control update.
	 *
	 */
	ساخت داخلہ_2 {
		اعشاری altitude;				///< Current altitude amsl [m].
		اعشاری altitude_شرح;			///< Current altitude rate [m/s].
		اعشاری altitude_setpoint;		///< Altitude setpoint amsl [m], NAN if not set.
		اعشاری altitude_شرح_setpoint{NAN};	///< Altitude rate setpoint [m/s], NAN if not set.
		اعشاری equivalent_airspeed;		///< Current equivalent airspeed [m/s].
		اعشاری رفتار_deriv_forward;		///< Current forward acceleration, used as true airspeed rate [m/s²].
		اعشاری equivalent_airspeed_setpoint;	///< Equivalent airspeed setpoint [m/s].
		اعشاری eas_مقصد_tas;			///< Conversion factor from equivalent to true airspeed [-].
		اعشاری throttle_min;			///< Normalized throttle lower limit.
		اعشاری throttle_max;			///< Normalized throttle upper limit.
		اعشاری throttle_trim;			///< Normalized throttle required to fly level at the airspeed setpoint.
		اعشاری pitch_min;			///< Minimum absolute pitch angle setpoint [rad].
		اعشاری pitch_max;			///< Maximum absolute pitch angle setpoint [rad].
		اعشاری ہدف_climbrate;			///< Climb rate the reference model targets [m/s].
		اعشاری ہدف_sinkrate;			///< Sink rate the reference model targets [m/s].
	};

عوامی:
	TECS() = طےشدہ;
	~TECS() = طےشدہ;

	// no copy, assignment, move, move assignment
	TECS(ثابت TECS &) = حذف;
	TECS &عامل=(ثابت TECS &) = حذف;
	TECS(TECS &&) = حذف;
	TECS &عامل=(TECS &&) = حذف;

	ثابت DebugOutput &get_حالت() ثابت { واپس debug_حالت; }

	/**
	 * Set the airspeed enable state
	 *
	 * @param[in] enabled is true if the airspeed measurement is used for control.
	 */
	خالی enable_airspeed(منطقی enabled) { _control_flag.airspeed_enabled = enabled; }

	/**
	 * @brief Update the control loop calculations
	 *
	 * All pitch angles, including the pitch limits and the resulting pitch setpoint, are absolute: the angle of the
	 * body x-axis above the horizon.
	 *
	 * @param[in] input is the current vehicle state, setpoints and limits.
	 */
	خالی update(ثابت داخلہ_2 &داخلہ);

	خالی resetIntegrals()
	{
		_control.resetIntegrals();
	}

	خالی سیٹ_detect_underspeed_enabled(منطقی enabled) { _control_flag.detect_underspeed_enabled = enabled; }

	// setters for parameters

	خالی سیٹ_integrator_gain_throttle(اعشاری gain) { _control_param.integrator_gain_throttle = gain;}
	خالی سیٹ_integrator_gain_pitch(اعشاری gain) { _control_param.integrator_gain_pitch = gain; }

	خالی سیٹ_max_sink_شرح(اعشاری max_sink_شرح) { _control_param.max_sink_شرح = max_sink_شرح; _reference_param.max_sink_شرح = max_sink_شرح; }
	خالی سیٹ_min_sink_شرح(اعشاری min_sink_شرح) { _control_param.min_sink_شرح = min_sink_شرح; }
	خالی سیٹ_max_climb_شرح(اعشاری climb_شرح) { _control_param.max_climb_شرح = climb_شرح; _reference_param.max_climb_شرح = climb_شرح; }

	خالی سیٹ_altitude_شرح_ff(اعشاری altitude_شرح_ff) { _control_param.altitude_setpoint_gain_ff = altitude_شرح_ff; }
	خالی سیٹ_altitude_error_وقت_constant(اعشاری وقت_const) { _control_param.altitude_error_gain = 1.0f / math::max(وقت_const, 0.1f); }
	خالی سیٹ_fast_descend_altitude_error(اعشاری altitude_error) { _fast_descend_alt_err = altitude_error; }

	خالی سیٹ_equivalent_airspeed_min(اعشاری airspeed) { _equivalent_airspeed_min = airspeed; }
	خالی سیٹ_equivalent_airspeed_stall(اعشاری airspeed) { _equivalent_airspeed_stall = airspeed; }
	خالی سیٹ_equivalent_airspeed_max(اعشاری airspeed) { _equivalent_airspeed_max = airspeed; }
	خالی سیٹ_equivalent_airspeed_trim(اعشاری airspeed) { _control_param.equivalent_airspeed_trim = airspeed; _airspeed_filter_param.equivalent_airspeed_trim = airspeed; }

	خالی سیٹ_pitch_damping(اعشاری damping) { _control_param.pitch_damping_gain = damping; }
	خالی سیٹ_vertical_accel_حد_بندی(اعشاری حد_بندی_2) { _reference_param.vert_accel_حد_بندی = حد_بندی_2; _control_param.vert_accel_حد_بندی = حد_بندی_2; }

	خالی سیٹ_رفتار_وزن(اعشاری وزن) { _control_param.pitch_رفتار_وزن = وزن; }
	خالی سیٹ_airspeed_error_وقت_constant(اعشاری وقت_const) { _control_param.airspeed_error_gain = 1.0f / math::max(وقت_const, 0.1f); }

	خالی سیٹ_throttle_damp(اعشاری throttle_damp) { _control_param.throttle_damping_gain = throttle_damp; }
	خالی سیٹ_throttle_slewrate(اعشاری slewrate) { _control_param.throttle_slewrate = slewrate; }

	خالی سیٹ_roll_throttle_compensation(اعشاری compensation) { _control_param.بھرو_ضربی_عدد_correction = compensation; }
	خالی سیٹ_بھرو_ضربی_عدد(اعشاری بھرو_ضربی_عدد) { _control_param.بھرو_ضربی_عدد = بھرو_ضربی_عدد; }

	خالی سیٹ_ste_شرح_وقت_const(اعشاری وقت_const) { _control_param.ste_شرح_وقت_const = وقت_const; }

	خالی سیٹ_seb_شرح_ff_gain(اعشاری ff_gain) { _control_param.seb_شرح_ff = ff_gain; }

	/**
	 * Set the pitch angle in level flight at trim airspeed [rad], fed forward as the pitch at zero climb angle
	 */
	خالی سیٹ_level_flight_pitch(اعشاری pitch) { _control_param.level_flight_pitch = pitch; }

	/**
	 * Handle the altitude reset
	 *
	 * If the estimation system resets the height in one discrete step this
	 * will gracefully even out the reset over time.
	 *
	 * @param[in] altitude is the altitude amsl after the reset [m].
	 * @param[in] altitude_rate is the current altitude rate [m/s].
	 */
	خالی handle_alt_قدم(اعشاری altitude, اعشاری altitude_شرح)
	{
		TECSAltitudeReferenceModel::AltitudeReferenceState init_state{ .alt = altitude,
				.alt_شرح = altitude_شرح};

		// reset altitude reference model.
		_altitude_reference_model.initialize(init_state);
	}

	/**
	 * Returns the absolute pitch setpoint [rad]
	 */
	اعشاری get_pitch_setpoint() ثابت {واپس _control.getPitchSetpoint();}
	اعشاری get_throttle_setpoint() ثابت {واپس _control.getThrottleSetpoint();}

	/**
	 * Returns the altitude tracking time constant
	 */
	اعشاری get_altitude_error_وقت_constant() ثابت { واپس 1.0f / _control_param.altitude_error_gain; }

	اعشاری get_underspeed_تناسب() ثابت { واپس _control.get_Underspeed_تناسب(); }

نجی:
	/**
	 * @brief Initialize the control parameters
	 *
	 * @param[in] input is the current vehicle state, setpoints and limits.
	 */
	خالی initControlParams(ثابت داخلہ_2 &داخلہ);

	/**
	 * @brief calculate true airspeed setpoint
	 *
	 * Calculate true airspeed setpoint based on input and fast descend ratio
	 *
	 * @param eas_to_tas is the eas to tas conversion factor
	 * @param eas_setpoint is the desired equivalent airspeed setpoint [m/s]
	 * @return true airspeed setpoint[m/s]
	 */
	اعشاری calcTrueAirspeedSetpoint(اعشاری eas_مقصد_tas, اعشاری eas_setpoint);

	/**
	 * @brief Initialize the control loop
	 *
	 * @param[in] input is the current vehicle state, setpoints and limits.
	 */
	خالی initialize(ثابت داخلہ_2 &داخلہ);

	TECSControl 			_control;			///< Control submodule.
	TECSAirspeedFilter 		_airspeed_filter;		///< Airspeed filter submodule.
	TECSAltitudeReferenceModel 	_altitude_reference_model;	///< Setpoint reference model submodule.

	hrt_abstime _update_timestamp{0};				///< last timestamp of the update function call.

	اعشاری _equivalent_airspeed_min{10.0f};				///< equivalent airspeed demand lower limit (m/sec)
	اعشاری _equivalent_airspeed_stall{7.0f};				///< equivalent stall airspeed (m/sec)
	اعشاری _equivalent_airspeed_max{20.0f};				///< equivalent airspeed demand upper limit (m/sec)
	اعشاری _fast_descend_alt_err{-1.f};	 				///< Altitude difference between current altitude to altitude setpoint needed to descend with higher airspeed [m].
	اعشاری _fast_descend{0.f};					///< Value for fast descend in [0,1]. continuous value used to flatten the high speed value out when close to target altitude.
	hrt_abstime _enabled_fast_descend_timestamp{0U};		///< timestamp at activation of fast descend mode

	ساکن constexpr اعشاری DT_MIN = 0.001f;				///< minimum allowed value of _dt (sec)
	ساکن constexpr اعشاری DT_MAX = 1.0f;				///< max value of _dt allowed before a filter state reset is performed (sec)
	ساکن constexpr hrt_abstime FAST_DESCEND_RAMP_اوپر_وقت = 2_s; 	///< Ramp up time until fast descend is fully engaged
	ساکن constexpr اعشاری FAST_DESCEND_اونچائی_MARGIN = 5.f;	///< Height above setpoint where fast descend ends [m]

	DebugOutput debug_حالت{};

	// Params
	/// Airspeed filter parameters.
	TECSAirspeedFilter::Param _airspeed_filter_param{
		.equivalent_airspeed_trim = 15.0f,
	};
	/// Reference model parameters.
	TECSAltitudeReferenceModel::Param _reference_param{
		.ہدف_climbrate = 2.0f,
		.ہدف_sinkrate = 2.0f,
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
		.ste_شرح_وقت_const = 0.1f,
		.seb_شرح_ff = 1.0f,
		.pitch_رفتار_وزن = 1.0f,
		.integrator_gain_pitch = 0.0f,
		.pitch_damping_gain = 0.0f,
		.integrator_gain_throttle = 0.0f,
		.throttle_damping_gain = 0.0f,
		.throttle_slewrate = 0.0f,
		.بھرو_ضربی_عدد_correction = 0.0f,
		.بھرو_ضربی_عدد = 1.0f,
		.fast_descend = 0.f
	};

	TECSControl::Flag _control_flag{
		.airspeed_enabled = جھوٹ,
		.detect_underspeed_enabled = جھوٹ,
	};

	/**
	 * @brief Set fast descend value
	 *
	 * @param alt_setpoint is the altitude setpoint
	 * @param alt is the current altitude
	 */
	خالی سیٹ_Fast_Descend(اعشاری alt_setpoint, اعشاری alt);
};

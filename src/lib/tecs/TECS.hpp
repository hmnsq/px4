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

#సూచన once

#అన్తర్భావ <drivers/drv_hrt.h>
#అన్తర్భావ <mathlib/mathlib.h>
#అన్తర్భావ <matrix/math.hpp>
#అన్తర్భావ <lib/mathlib/math/filter/AlphaFilter.hpp>
#అన్తర్భావ <motion_planning/VelocitySmoothing.hpp>
#అన్తర్భావ <motion_planning/ManualVelocitySmoothingZ.hpp>

ఉపయోగించి నామాకాశ సమయం_literals;

#నిర్వచితం_కాకపోతే FRIEND_TEST // for gtest
#నిర్వచించు FRIEND_TEST(a, b)
#అయితే_ముగింపు

తరగతి TECSAirspeedFilter
{
బహిరంగ:
	/**
	 * @brief State of the equivalent airspeed filter.
	 *
	 */
	నిర్మాణం AirspeedFilterState {
		దశాంశం వేగం;		///< speed of the air in EAS [m/s]
		దశాంశం వేగం_రేటు;	///< rate of speed of the air [m/s²]
	};

	/**
	 * @brief Parameters of the airspeed filter.
	 *
	 */
	నిర్మాణం Param {
		దశాంశం equivalent_airspeed_trim; 	///< the trim value of the equivalent airspeed [m/s].
	};

	/**
	 * @brief Input, which will be filtered.
	 *
	 */
	నిర్మాణం ఆదానం_2 {
		దశాంశం equivalent_airspeed; 	///< the measured equivalent airspeed in [m/s].
		దశాంశం equivalent_airspeed_రేటు;	///< the measured rate of equivalent airspeed in [m/s²].
	};
బహిరంగ:
	TECSAirspeedFilter() = అప్రమేయ;
	~TECSAirspeedFilter() = అప్రమేయ;
	/**
	 * @brief Initialize filter
	 *
	 * @param[in] equivalent_airspeed is the equivalent airspeed in [m/s].
	 * @param[in] equivalent_airspeed_trim is the equivalent airspeed trim (vehicle setting) in [m/s].
	 * @param[in] airspeed_sensor_available boolean if the airspeed sensor is available.
	 */
	శూన్య initialize(దశాంశం equivalent_airspeed, స్థిరరాశి దశాంశం equivalent_airspeed_trim,
			స్థిరరాశి తార్కిక airspeed_sensor_అందుబాటు);

	/**
	 * @brief Update filter
	 *
	 * @param[in] dt is the timestep in [s].
	 * @param[in] input are the raw measured values.
	 * @param[in] param are the filter parameters.
	 * @param[in] airspeed_sensor_available boolean if the airspeed sensor is available.
	 */
	శూన్య update(దశాంశం dt, స్థిరరాశి ఆదానం_2 &ఆదానం, స్థిరరాశి Param &param, స్థిరరాశి తార్కిక airspeed_sensor_అందుబాటు);

	/**
	 * @brief Get the filtered airspeed states.
	 *
	 * @return Current state of the airspeed filter.
	 */
	AirspeedFilterState getState() స్థిరరాశి;

అంతరంగ:
	// States
	AirspeedFilterState _airspeed_state{.వేగం = 0.0f, .వేగం_రేటు = 0.0f};	///< Complementary filter state

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
	స్థిర constexpr దశాంశం kKg00{1.91940287f};
	స్థిర constexpr దశాంశం kKg01{0.25925926f};
	స్థిర constexpr దశాంశం kKg10{2.11640212f};
	స్థిర constexpr దశాంశం kKg11{0.67179101f};
};

తరగతి TECSAltitudeReferenceModel
{
బహిరంగ:
	/**
	 * @brief Altitude reference state.
	 *
	 */
	నిర్మాణం AltitudeReferenceState {
		దశాంశం alt; 	///< Reference altitude amsl in [m].
		దశాంశం alt_రేటు;	///< Reference altitude rate in [m/s].
	};

	/**
	 * @brief Parameters for the reference model.
	 *
	 */
	నిర్మాణం Param {
		దశాంశం లక్ష్యం_climbrate;	///< The target climbrate in [m/s].
		దశాంశం లక్ష్యం_sinkrate;	///< The target sinkrate in [m/s].
		దశాంశం vert_accel_హద్దు;	///< Magnitude of the maximum vertical acceleration allowed [m/s²].
		దశాంశం max_climb_రేటు;	///< Climb rate produced by max allowed throttle [m/s].
		దశాంశం max_sink_రేటు;	///< Maximum sink rate (with min throttle, max speed) [m/s].
	};

బహిరంగ:
	TECSAltitudeReferenceModel() = అప్రమేయ;
	~TECSAltitudeReferenceModel() = అప్రమేయ;

	/**
	 * @brief Initialize reference models.
	 *
	 * @param[in] state is the current altitude state of the vehicle.
	 */
	శూన్య initialize(స్థిరరాశి AltitudeReferenceState &state);

	/**
	 * @brief Update reference models.
	 *
	 * @param[in] dt is the update interval in [s].
	 * @param[in] setpoint are the desired setpoints.
	 * @param[in] altitude is the altitude amsl in [m].
	 * @param[in] height_rate is the current height rate in [m/s].
	 * @param[in] param are the reference model parameters.
	 */
	శూన్య update(దశాంశం dt, స్థిరరాశి AltitudeReferenceState &setpoint, దశాంశం altitude, దశాంశం ఎత్తు_రేటు, స్థిరరాశి Param &param);

	/**
	 * @brief Get the current altitude reference of altitude reference model.
	 *
	 * @return Altitude reference state.
	 */
	AltitudeReferenceState getAltitudeReference() స్థిరరాశి;

	/**
	 * @brief Get the Height Rate Setpoint directly from the velocity trajectory generator
	 *
	 * @return float direct height rate setpoint [m/s]
	 */
	దశాంశం get_ఎత్తు_రేటు_Setpoint_ప్రత్యక్ష() స్థిరరాశి {తిరిగి ఎత్తు_రేటు_setpoint_ప్రత్యక్ష; }


అంతరంగ:
	స్థిర constexpr దశాంశం JERK_MAX = 1000.f;	///< Magnitude of the maximum jerk allowed [m/s³].

	// State
	VelocitySmoothing
	_alt_control_traj_generator;		///< Generates altitude rate and altitude setpoint trajectory when altitude is commanded.
	ManualVelocitySmoothingZ
	_velocity_control_traj_generator;	///< generates height rate trajectory when height rate is commanded
	దశాంశం ఎత్తు_రేటు_setpoint_ప్రత్యక్ష{NAN}; ///< generated direct height rate setpoint
};

తరగతి TECSControl
{
బహిరంగ:
	/**
	 * @brief The control parameters.
	 *
	 */
	నిర్మాణం Param {
		// Vehicle specific params
		దశాంశం max_sink_రేటు;			///< Maximum sink rate (with min throttle and max speed) [m/s].
		దశాంశం min_sink_రేటు;			///< Minimum sink rate (with min throttle and trim speed) [m/s].
		దశాంశం max_climb_రేటు;			///< Climb rate produced by max allowed throttle [m/s].
		దశాంశం vert_accel_హద్దు;			///< Magnitude of the maximum vertical acceleration allowed [m/s²].
		దశాంశం equivalent_airspeed_trim;		///< Equivalent cruise airspeed for airspeed less mode [m/s].
		దశాంశం tas_min;				///< True airspeed demand lower limit [m/s].
		దశాంశం tas_stall;			///< True stall airspeed [m/s].
		దశాంశం tas_max;				///< True airspeed demand upper limit [m/s].
		దశాంశం tas_trim;				///< True airspeed trim [m/s].
		దశాంశం pitch_max;			///< Maximum absolute pitch angle setpoint [rad].
		దశాంశం pitch_min;			///< Minimum absolute pitch angle setpoint [rad].
		దశాంశం level_flight_pitch;		///< Pitch angle in level flight at trim airspeed, fed forward at zero climb angle [rad].
		దశాంశం throttle_trim;		///< Normalized throttle required to fly level at calibrated airspeed setpoint [0,1]
		దశాంశం throttle_max;			///< Normalized throttle upper limit.
		దశాంశం throttle_min;			///< Normalized throttle lower limit.

		// Altitude control param
		దశాంశం altitude_error_gain;		///< Altitude error inverse time constant [1/s].
		దశాంశం altitude_setpoint_gain_ff;	///< Gain from altitude demand derivative to demanded climb rate.

		// Airspeed control param
		దశాంశం airspeed_error_gain;				///< Airspeed error inverse time constant [1/s].

		// Energy control param
		దశాంశం ste_రేటు_సమయం_const;				///< Filter time constant for specific total energy rate (damping path) [s].
		దశాంశం seb_రేటు_ff;					///< Specific energy balance rate feedforward gain.

		// Pitch control param
		దశాంశం pitch_వేగం_బరువు;				///< Speed control weighting used by pitch demand calculation.
		దశాంశం integrator_gain_pitch;				///< Integrator gain used by the pitch demand calculation.
		దశాంశం pitch_damping_gain;				///< Damping gain of the pitch demand calculation [s].

		// Throttle control param
		దశాంశం integrator_gain_throttle;				///< Integrator gain used by the throttle demand calculation.
		దశాంశం throttle_damping_gain;				///< Damping gain of the throttle demand calculation [s].
		దశాంశం throttle_slewrate;				///< Throttle demand slew rate limit [1/s].

		దశాంశం ఎక్కించు_గుణకం_correction;				///< Gain from normal load factor increase to total energy rate demand [m²/s³].
		దశాంశం ఎక్కించు_గుణకం;					///< Additional normal load factor.

		దశాంశం fast_descend;
	};

	/**
	 * @brief The debug output
	 *
	 */
	నిర్మాణం DebugOutput {
		దశాంశం altitude_రేటు_control;			///< Altitude rate setpoint from altitude control loop [m/s].
		దశాంశం true_airspeed_derivative_control;		///< Airspeed rate setpoint from airspeed control loop [m/s²].
		దశాంశం మొత్తం_energy_రేటు_estimate;		///< Total energy rate estimate [m²/s³].
		దశాంశం మొత్తం_energy_రేటు_sp;			///< Total energy rate setpoint [m²/s³].
		దశాంశం energy_balance_రేటు_estimate;		///< Energy balance rate estimate [m²/s³].
		దశాంశం energy_balance_రేటు_sp;			///< Energy balance rate setpoint [m²/s³].
		దశాంశం pitch_integrator;				///< Pitch control integrator state, a correction added to the pitch setpoint [rad].
		దశాంశం throttle_integrator;			///< Throttle control integrator state [-].
	};

	/**
	 * @brief Given setpoint to control.
	 *
	 */
	నిర్మాణం Setpoint {
		TECSAltitudeReferenceModel::AltitudeReferenceState altitude_reference;	///< Altitude/height rate reference.
		దశాంశం altitude_రేటు_setpoint_ప్రత్యక్ష;					///< Direct height rate setpoint.
		దశాంశం tas_setpoint;						///< True airspeed setpoint.
	};

	/**
	 * @brief Given current measurement from the UAS.
	 *
	 */
	నిర్మాణం ఆదానం_2 {
		దశాంశం altitude;		///< Current altitude amsl of the UAS [m].
		దశాంశం altitude_రేటు;	///< Current altitude rate of the UAS [m/s].
		దశాంశం tas;		///< Current true airspeed of the UAS [m/s].
		దశాంశం tas_రేటు;		///< Current true airspeed rate of the UAS [m/s²].
	};

	/**
	 * @brief Control flags.
	 *
	 */
	నిర్మాణం Flag {
		తార్కిక airspeed_enabled;			///< Flag if the airspeed sensor is enabled.
		తార్కిక detect_underspeed_enabled;		///< Flag if underspeed detection is enabled.
	};
బహిరంగ:
	TECSControl() = అప్రమేయ;
	~TECSControl() = అప్రమేయ;
	/**
	 * @brief Initialization of the state.
	 *
	 * @param[in] setpoint is the current setpoint struct.
	 * @param[in] input is the current input measurements.
	 * @param[in] param is the current parameter set.
	 * @param[in] flag is the current activated flags.
	 */
	శూన్య initialize(స్థిరరాశి Setpoint &setpoint, స్థిరరాశి ఆదానం_2 &ఆదానం, స్థిరరాశి Param &param, స్థిరరాశి Flag &flag);
	/**
	 * @brief Update state and output.
	 *
	 * @param[in] dt is the update time interval in [s].
	 * @param[in] setpoint is the current setpoint struct.
	 * @param[in] input is the current input measurements.
	 * @param[in] param is the current parameter set.
	 * @param[in] flag is the current activated flags.
	 */
	శూన్య update(దశాంశం dt, స్థిరరాశి Setpoint &setpoint, స్థిరరాశి ఆదానం_2 &ఆదానం, స్థిరరాశి Param &param, స్థిరరాశి Flag &flag);
	/**
	 * @brief Reset the control loop integrals.
	 *
	 */
	శూన్య resetIntegrals();
	/**
	 * @brief Get the underspeed ratio.
	 *
	 * @return Ratio of detected underspeed [0,1].
	 */
	దశాంశం get_Underspeed_నిష్పత్తి() స్థిరరాశి {తిరిగి నిష్పత్తి_undersped;}
	/**
	 * @brief Get the throttle setpoint.
	 *
	 * @return throttle setpoint.
	 */
	దశాంశం getThrottleSetpoint() స్థిరరాశి {తిరిగి _throttle_setpoint;}
	/**
	 * @brief Get the pitch setpoint.
	 *
	 * @return The commanded absolute pitch angle [rad].
	 */
	దశాంశం getPitchSetpoint() స్థిరరాశి {తిరిగి _pitch_setpoint;}
	/**
	 * @brief Get the Debug Output
	 *
	 * @return the debug output struct.
	 */
	స్థిరరాశి DebugOutput &getDebugOutput() స్థిరరాశి { తిరిగి _debug_output; }

అంతరంగ:
	// Allows the regression test to pre-corrupt _pitch_integ_state and verify the
	// safety reset in _calcPitchControlUpdate, which the public update() path cannot
	// reach on its own because the integrator-input guard rejects bad input first.
	FRIEND_TEST(TECSControlTest, PitchIntegratorResetsCorruptedState);

	/**
	 * @brief Specific total energy rate limit.
	 *
	 */
	నిర్మాణం STERate_హద్దు {
		దశాంశం STE_రేటు_max;	///< Maximum specific total energy rate limit [m²/s³].
		దశాంశం STE_రేటు_min;	///< Minimal specific total energy rate limit [m²/s³].
	};

	/**
	 * @brief Control values.
	 * setpoint and current state estimate value as input to a controller.
	 *
	 */
	నిర్మాణం Control_విలువలు {
		దశాంశం setpoint;		///< Control setpoint.
		దశాంశం estimate;		///< Control estimate of current state value.
	};

	/**
	 * @brief Calculated specific energy rates.
	 *
	 */
	నిర్మాణం SpecificEnergyRates {
		Control_విలువలు ske_రేటు;	///< Specific kinetic energy rate [m²/s³].
		Control_విలువలు spe_రేటు;	///< Specific potential energy rate [m²/s³].
	};

	/**
	 * @brief Altitude rate and airspeed setpoints from the outer loops, from which the energy rate setpoints follow.
	 *
	 */
	నిర్మాణం OuterLoopSetpoints {
		దశాంశం altitude_రేటు_setpoint;	///< Controlled altitude rate setpoint [m/s].
		దశాంశం tas_రేటు_setpoint;	///< Controlled true airspeed rate setpoint [m/s²].
	};

	/**
	 * @brief Weight factors for specific energy.
	 *
	 */
	నిర్మాణం SpecificEnergyWeighting {
		దశాంశం spe_weighting;	///< Specific potential energy weight.
		దశాంశం ske_weighting;	///< Specific kinetic energy weight.
	};

అంతరంగ:
	/**
	 * @brief Get control error from setpoint and estimate
	 *
	 * @param val is the current control setpoint and estimate.
	 * @return error value
	 */
	స్థిర constexpr దశాంశం _getControlError(TECSControl::Control_విలువలు val) {తిరిగి (val.setpoint - val.estimate);}
	/**
	 * @brief Calculate specific total energy rate limits.
	 *
	 * @param[in] param are the control parameters.
	 * @return Specific total energy rate limits in [m²/s³].
	 */
	STERate_హద్దు calculate_మొత్తం_Energy_రేటు_హద్దు(స్థిరరాశి Param &param) స్థిరరాశి;
	/**
	 * @brief Gain from specific total energy rate error to throttle.
	 *
	 * @param[in] limit is the specific total energy rate limits in [m²/s³].
	 * @return throttle per specific total energy rate [s³/m²].
	 */
	స్థిర దశాంశం ste_రేటు_గమ్యం_Throttle(స్థిరరాశి STERate_హద్దు &హద్దు_2) { తిరిగి 1.0f / (హద్దు_2.STE_రేటు_max - హద్దు_2.STE_రేటు_min); }
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
	దశాంశం _calcAirspeedControlOutput(స్థిరరాశి Setpoint &setpoint, స్థిరరాశి ఆదానం_2 &ఆదానం, స్థిరరాశి STERate_హద్దు &హద్దు_2,
					 స్థిరరాశి Param &param, స్థిరరాశి Flag &flag) స్థిరరాశి;
	/**
	 * @brief calculate altitude control proportional output.
	 *
	 * @param setpoint is the control setpoints.
	 * @param input is the current input measurement of the UAS.
	 * @param param is the control parameters.
	 * @return controlled altitude rate setpoint in [m/s].
	 */
	దశాంశం _calcAltitudeControlOutput(స్థిరరాశి Setpoint &setpoint, స్థిరరాశి ఆదానం_2 &ఆదానం, స్థిరరాశి Param &param) స్థిరరాశి;
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
	SpecificEnergyRates _updateOuterLoops(స్థిరరాశి Setpoint &setpoint, స్థిరరాశి ఆదానం_2 &ఆదానం, స్థిరరాశి STERate_హద్దు &హద్దు_2,
					      స్థిరరాశి Param &param, స్థిరరాశి Flag &flag);
	/**
	 * @brief Calculate specific energy rates.
	 *
	 * @param control_setpoint is the controlled altitude and airspeed rate setpoints.
	 * @param input is the current input measurement of the UAS.
	 * @return Specific energy rates in [m²/s³].
	 */
	SpecificEnergyRates _calcSpecificEnergyRates(స్థిరరాశి OuterLoopSetpoints &control_setpoint, స్థిరరాశి ఆదానం_2 &ఆదానం) స్థిరరాశి;
	/**
	 * @brief Detect underspeed.
	 *
	 * @param input is the current input measurement of the UAS.
	 * @param param is the control parameters.
	 * @param flag is the control flags.
	 */
	శూన్య _detectUnderspeed(స్థిరరాశి ఆదానం_2 &ఆదానం, స్థిరరాశి Param &param, స్థిరరాశి Flag &flag);
	/**
	 * @brief Update specific energy balance weights.
	 *
	 * @param param is the control parameters.
	 * @param flag is the control flags.
	 * @return Weights used for the specific energy balance.
	 */
	SpecificEnergyWeighting update_వేగం_Altitude_Weights(స్థిరరాశి Param &param, స్థిరరాశి Flag &flag);
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
	దశాంశం calc_Pitch_Control_Ske_రేటు_Setpoint(స్థిరరాశి Setpoint &setpoint, స్థిరరాశి ఆదానం_2 &ఆదానం, స్థిరరాశి STERate_హద్దు &హద్దు_2,
					       స్థిరరాశి Param &param, స్థిరరాశి Flag &flag, స్థిరరాశి SpecificEnergyWeighting &బరువు) స్థిరరాశి;
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
	SpecificEnergyRates _calcPitchControlEnergyRates(స్థిరరాశి Setpoint &setpoint, స్థిరరాశి ఆదానం_2 &ఆదానం,
			స్థిరరాశి STERate_హద్దు &హద్దు_2, స్థిరరాశి SpecificEnergyRates &specific_energy_rates, స్థిరరాశి Param &param,
			స్థిరరాశి Flag &flag, స్థిరరాశి SpecificEnergyWeighting &బరువు) స్థిరరాశి;
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
	శూన్య _calcPitchControl(దశాంశం dt, స్థిరరాశి Setpoint &setpoint, స్థిరరాశి ఆదానం_2 &ఆదానం, స్థిరరాశి STERate_హద్దు &హద్దు_2,
			       స్థిరరాశి SpecificEnergyRates &specific_energy_రేటు, స్థిరరాశి Param &param, స్థిరరాశి Flag &flag);

	/**
	 * @brief Calculate pitch control specific energy balance rates.
	 *
	 * @param weight is the weighting use of the potential and kinetic energy.
	 * @param specific_energy_rate is the specific energy rates in [m²/s³].
	 * @return specific energy balance rate values in [m²/s³].
	 */
	Control_విలువలు calc_Pitch_Control_Seb_రేటు(స్థిరరాశి SpecificEnergyWeighting &బరువు,
					       స్థిరరాశి SpecificEnergyRates &specific_energy_రేటు) స్థిరరాశి;

	/**
	 * @brief Calculate the pitch control update function.
	 * Update the states of the pitch control (pitch integrator).
	 *
	 * @param dt is the update time interval in [s].
	 * @param input is the current input measurement of the UAS.
	 * @param seb_rate is the specific energy balance rate in [m²/s³].
	 * @param param is the control parameters.
	 */
	శూన్య _calcPitchControlUpdate(దశాంశం dt, స్థిరరాశి ఆదానం_2 &ఆదానం, స్థిరరాశి Control_విలువలు &seb_రేటు, స్థిరరాశి Param &param);

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
	స్థిర దశాంశం climb_Angle_గమ్యం_Seb_రేటు(స్థిరరాశి ఆదానం_2 &ఆదానం, స్థిరరాశి Param &param);

	/**
	 * @brief Calculate the pitch control output function.
	 *
	 * @param input is the current input measurement of the UAS.
	 * @param seb_rate is the specific energy balance rate in [m²/s³].
	 * @param param is the control parameters.
	 * @return absolute pitch setpoint [rad].
	 */
	దశాంశం _calcPitchControlOutput(స్థిరరాశి ఆదానం_2 &ఆదానం, స్థిరరాశి Control_విలువలు &seb_రేటు, స్థిరరాశి Param &param) స్థిరరాశి;

	/**
	 * @brief Update controlled throttle setpoint.
	 *
	 * @param dt is the update time interval in [s].
	 * @param limit is the specific total energy rate limits in [m²/s³].
	 * @param specific_energy_rate is the calculated specific energy.
	 * @param param is the control parameters.
	 * @param flag is the control flags.
	 */
	శూన్య _calcThrottleControl(దశాంశం dt, స్థిరరాశి STERate_హద్దు &హద్దు_2, స్థిరరాశి SpecificEnergyRates &specific_energy_రేటు,
				  స్థిరరాశి Param &param, స్థిరరాశి Flag &flag);

	/**
	 * @brief Calculate throttle control specific total energy
	 *
	 * @param limit is the specific total energy rate limits in [m²/s³].
	 * @param specific_energy_rate is the specific energy rates in [m²/s³].
	 * @param param is the control parameters.
	 * @return specific total energy rate values in [m²/s³]
	 */
	Control_విలువలు calc_Throttle_Control_Ste_రేటు(స్థిరరాశి STERate_హద్దు &హద్దు_2, స్థిరరాశి SpecificEnergyRates &specific_energy_రేటు,
			స్థిరరాశి Param &param) స్థిరరాశి;

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
	శూన్య _calcThrottleControlUpdate(దశాంశం dt, స్థిరరాశి STERate_హద్దు &హద్దు_2, స్థిరరాశి Control_విలువలు &ste_రేటు, స్థిరరాశి Param &param,
					స్థిరరాశి Flag &flag);

	/**
	 * @brief Calculate the throttle control output function.
	 *
	 * @param limit is the specific total energy rate limits in [m²/s³].
	 * @param ste_rate is the specific total energy rates in [m²/s³].
	 * @param param is the control parameters.
	 * @param flag is the control flags.
	 * @return throttle setpoint in [0,1].
	 */
	దశాంశం _calcThrottleControlOutput(స్థిరరాశి STERate_హద్దు &హద్దు_2, స్థిరరాశి Control_విలువలు &ste_రేటు, స్థిరరాశి Param &param,
					 స్థిరరాశి Flag &flag) స్థిరరాశి;

అంతరంగ:
	// State
	AlphaFilter<దశాంశం> ste_రేటు_estimate_filter;		///< Low pass filter for the specific total energy rate.
	దశాంశం _pitch_integ_state{0.0f};				///< Pitch integrator state [rad].
	దశాంశం _fast_descend_prev{0.0f};				///< Fast descend ratio of the last update [0,1].
	దశాంశం _throttle_integ_state{0.0f};			///< Throttle integrator state [-].

	// Output
	DebugOutput _debug_output;				///< Debug output.
	దశాంశం _pitch_setpoint{0.0f};				///< Controlled absolute pitch setpoint [rad].
	దశాంశం _throttle_setpoint{0.0f};				///< Controlled throttle setpoint [0,1].
	దశాంశం నిష్పత్తి_undersped{0.0f};				///< A continuous representation of how "undersped" the TAS is [0,1]
};

తరగతి TECS
{
బహిరంగ:
	నిర్మాణం DebugOutput {
		TECSControl::DebugOutput control;
		దశాంశం true_airspeed_sp;
		దశాంశం true_airspeed_filtered;
		దశాంశం true_airspeed_derivative;
		దశాంశం altitude_reference;
		దశాంశం ఎత్తు_రేటు_reference;
		దశాంశం ఎత్తు_రేటు_ప్రత్యక్ష;
		దశాంశం fast_descend;
	};

	/**
	 * @brief Vehicle state, setpoints and limits for one control update.
	 *
	 */
	నిర్మాణం ఆదానం_2 {
		దశాంశం altitude;				///< Current altitude amsl [m].
		దశాంశం altitude_రేటు;			///< Current altitude rate [m/s].
		దశాంశం altitude_setpoint;		///< Altitude setpoint amsl [m], NAN if not set.
		దశాంశం altitude_రేటు_setpoint{NAN};	///< Altitude rate setpoint [m/s], NAN if not set.
		దశాంశం equivalent_airspeed;		///< Current equivalent airspeed [m/s].
		దశాంశం వేగం_deriv_forward;		///< Current forward acceleration, used as true airspeed rate [m/s²].
		దశాంశం equivalent_airspeed_setpoint;	///< Equivalent airspeed setpoint [m/s].
		దశాంశం eas_గమ్యం_tas;			///< Conversion factor from equivalent to true airspeed [-].
		దశాంశం throttle_min;			///< Normalized throttle lower limit.
		దశాంశం throttle_max;			///< Normalized throttle upper limit.
		దశాంశం throttle_trim;			///< Normalized throttle required to fly level at the airspeed setpoint.
		దశాంశం pitch_min;			///< Minimum absolute pitch angle setpoint [rad].
		దశాంశం pitch_max;			///< Maximum absolute pitch angle setpoint [rad].
		దశాంశం లక్ష్యం_climbrate;			///< Climb rate the reference model targets [m/s].
		దశాంశం లక్ష్యం_sinkrate;			///< Sink rate the reference model targets [m/s].
	};

బహిరంగ:
	TECS() = అప్రమేయ;
	~TECS() = అప్రమేయ;

	// no copy, assignment, move, move assignment
	TECS(స్థిరరాశి TECS &) = తొలగించు;
	TECS &క్రియాకారకం=(స్థిరరాశి TECS &) = తొలగించు;
	TECS(TECS &&) = తొలగించు;
	TECS &క్రియాకారకం=(TECS &&) = తొలగించు;

	స్థిరరాశి DebugOutput &get_స్థితి() స్థిరరాశి { తిరిగి debug_స్థితి; }

	/**
	 * Set the airspeed enable state
	 *
	 * @param[in] enabled is true if the airspeed measurement is used for control.
	 */
	శూన్య enable_airspeed(తార్కిక enabled) { _control_flag.airspeed_enabled = enabled; }

	/**
	 * @brief Update the control loop calculations
	 *
	 * All pitch angles, including the pitch limits and the resulting pitch setpoint, are absolute: the angle of the
	 * body x-axis above the horizon.
	 *
	 * @param[in] input is the current vehicle state, setpoints and limits.
	 */
	శూన్య update(స్థిరరాశి ఆదానం_2 &ఆదానం);

	శూన్య resetIntegrals()
	{
		_control.resetIntegrals();
	}

	శూన్య సమితి_detect_underspeed_enabled(తార్కిక enabled) { _control_flag.detect_underspeed_enabled = enabled; }

	// setters for parameters

	శూన్య సమితి_integrator_gain_throttle(దశాంశం gain) { _control_param.integrator_gain_throttle = gain;}
	శూన్య సమితి_integrator_gain_pitch(దశాంశం gain) { _control_param.integrator_gain_pitch = gain; }

	శూన్య సమితి_max_sink_రేటు(దశాంశం max_sink_రేటు) { _control_param.max_sink_రేటు = max_sink_రేటు; _reference_param.max_sink_రేటు = max_sink_రేటు; }
	శూన్య సమితి_min_sink_రేటు(దశాంశం min_sink_రేటు) { _control_param.min_sink_రేటు = min_sink_రేటు; }
	శూన్య సమితి_max_climb_రేటు(దశాంశం climb_రేటు) { _control_param.max_climb_రేటు = climb_రేటు; _reference_param.max_climb_రేటు = climb_రేటు; }

	శూన్య సమితి_altitude_రేటు_ff(దశాంశం altitude_రేటు_ff) { _control_param.altitude_setpoint_gain_ff = altitude_రేటు_ff; }
	శూన్య సమితి_altitude_error_సమయం_constant(దశాంశం సమయం_const) { _control_param.altitude_error_gain = 1.0f / math::max(సమయం_const, 0.1f); }
	శూన్య సమితి_fast_descend_altitude_error(దశాంశం altitude_error) { _fast_descend_alt_err = altitude_error; }

	శూన్య సమితి_equivalent_airspeed_min(దశాంశం airspeed) { _equivalent_airspeed_min = airspeed; }
	శూన్య సమితి_equivalent_airspeed_stall(దశాంశం airspeed) { _equivalent_airspeed_stall = airspeed; }
	శూన్య సమితి_equivalent_airspeed_max(దశాంశం airspeed) { _equivalent_airspeed_max = airspeed; }
	శూన్య సమితి_equivalent_airspeed_trim(దశాంశం airspeed) { _control_param.equivalent_airspeed_trim = airspeed; _airspeed_filter_param.equivalent_airspeed_trim = airspeed; }

	శూన్య సమితి_pitch_damping(దశాంశం damping) { _control_param.pitch_damping_gain = damping; }
	శూన్య సమితి_vertical_accel_హద్దు(దశాంశం హద్దు_2) { _reference_param.vert_accel_హద్దు = హద్దు_2; _control_param.vert_accel_హద్దు = హద్దు_2; }

	శూన్య సమితి_వేగం_బరువు(దశాంశం బరువు) { _control_param.pitch_వేగం_బరువు = బరువు; }
	శూన్య సమితి_airspeed_error_సమయం_constant(దశాంశం సమయం_const) { _control_param.airspeed_error_gain = 1.0f / math::max(సమయం_const, 0.1f); }

	శూన్య సమితి_throttle_damp(దశాంశం throttle_damp) { _control_param.throttle_damping_gain = throttle_damp; }
	శూన్య సమితి_throttle_slewrate(దశాంశం slewrate) { _control_param.throttle_slewrate = slewrate; }

	శూన్య సమితి_roll_throttle_compensation(దశాంశం compensation) { _control_param.ఎక్కించు_గుణకం_correction = compensation; }
	శూన్య సమితి_ఎక్కించు_గుణకం(దశాంశం ఎక్కించు_గుణకం) { _control_param.ఎక్కించు_గుణకం = ఎక్కించు_గుణకం; }

	శూన్య సమితి_ste_రేటు_సమయం_const(దశాంశం సమయం_const) { _control_param.ste_రేటు_సమయం_const = సమయం_const; }

	శూన్య సమితి_seb_రేటు_ff_gain(దశాంశం ff_gain) { _control_param.seb_రేటు_ff = ff_gain; }

	/**
	 * Set the pitch angle in level flight at trim airspeed [rad], fed forward as the pitch at zero climb angle
	 */
	శూన్య సమితి_level_flight_pitch(దశాంశం pitch) { _control_param.level_flight_pitch = pitch; }

	/**
	 * Handle the altitude reset
	 *
	 * If the estimation system resets the height in one discrete step this
	 * will gracefully even out the reset over time.
	 *
	 * @param[in] altitude is the altitude amsl after the reset [m].
	 * @param[in] altitude_rate is the current altitude rate [m/s].
	 */
	శూన్య handle_alt_అడుగు(దశాంశం altitude, దశాంశం altitude_రేటు)
	{
		TECSAltitudeReferenceModel::AltitudeReferenceState init_state{ .alt = altitude,
				.alt_రేటు = altitude_రేటు};

		// reset altitude reference model.
		_altitude_reference_model.initialize(init_state);
	}

	/**
	 * Returns the absolute pitch setpoint [rad]
	 */
	దశాంశం get_pitch_setpoint() స్థిరరాశి {తిరిగి _control.getPitchSetpoint();}
	దశాంశం get_throttle_setpoint() స్థిరరాశి {తిరిగి _control.getThrottleSetpoint();}

	/**
	 * Returns the altitude tracking time constant
	 */
	దశాంశం get_altitude_error_సమయం_constant() స్థిరరాశి { తిరిగి 1.0f / _control_param.altitude_error_gain; }

	దశాంశం get_underspeed_నిష్పత్తి() స్థిరరాశి { తిరిగి _control.get_Underspeed_నిష్పత్తి(); }

అంతరంగ:
	/**
	 * @brief Initialize the control parameters
	 *
	 * @param[in] input is the current vehicle state, setpoints and limits.
	 */
	శూన్య initControlParams(స్థిరరాశి ఆదానం_2 &ఆదానం);

	/**
	 * @brief calculate true airspeed setpoint
	 *
	 * Calculate true airspeed setpoint based on input and fast descend ratio
	 *
	 * @param eas_to_tas is the eas to tas conversion factor
	 * @param eas_setpoint is the desired equivalent airspeed setpoint [m/s]
	 * @return true airspeed setpoint[m/s]
	 */
	దశాంశం calcTrueAirspeedSetpoint(దశాంశం eas_గమ్యం_tas, దశాంశం eas_setpoint);

	/**
	 * @brief Initialize the control loop
	 *
	 * @param[in] input is the current vehicle state, setpoints and limits.
	 */
	శూన్య initialize(స్థిరరాశి ఆదానం_2 &ఆదానం);

	TECSControl 			_control;			///< Control submodule.
	TECSAirspeedFilter 		_airspeed_filter;		///< Airspeed filter submodule.
	TECSAltitudeReferenceModel 	_altitude_reference_model;	///< Setpoint reference model submodule.

	hrt_abstime _update_timestamp{0};				///< last timestamp of the update function call.

	దశాంశం _equivalent_airspeed_min{10.0f};				///< equivalent airspeed demand lower limit (m/sec)
	దశాంశం _equivalent_airspeed_stall{7.0f};				///< equivalent stall airspeed (m/sec)
	దశాంశం _equivalent_airspeed_max{20.0f};				///< equivalent airspeed demand upper limit (m/sec)
	దశాంశం _fast_descend_alt_err{-1.f};	 				///< Altitude difference between current altitude to altitude setpoint needed to descend with higher airspeed [m].
	దశాంశం _fast_descend{0.f};					///< Value for fast descend in [0,1]. continuous value used to flatten the high speed value out when close to target altitude.
	hrt_abstime _enabled_fast_descend_timestamp{0U};		///< timestamp at activation of fast descend mode

	స్థిర constexpr దశాంశం DT_MIN = 0.001f;				///< minimum allowed value of _dt (sec)
	స్థిర constexpr దశాంశం DT_MAX = 1.0f;				///< max value of _dt allowed before a filter state reset is performed (sec)
	స్థిర constexpr hrt_abstime FAST_DESCEND_RAMP_పైకి_సమయం = 2_s; 	///< Ramp up time until fast descend is fully engaged
	స్థిర constexpr దశాంశం FAST_DESCEND_ఎత్తు_MARGIN = 5.f;	///< Height above setpoint where fast descend ends [m]

	DebugOutput debug_స్థితి{};

	// Params
	/// Airspeed filter parameters.
	TECSAirspeedFilter::Param _airspeed_filter_param{
		.equivalent_airspeed_trim = 15.0f,
	};
	/// Reference model parameters.
	TECSAltitudeReferenceModel::Param _reference_param{
		.లక్ష్యం_climbrate = 2.0f,
		.లక్ష్యం_sinkrate = 2.0f,
		.vert_accel_హద్దు = 0.0f,
		.max_climb_రేటు = 2.0f,
		.max_sink_రేటు = 2.0f,
	};
	/// Control parameters.
	TECSControl::Param _control_param{
		.max_sink_రేటు = 5.0f,
		.min_sink_రేటు = 2.0f,
		.max_climb_రేటు = 5.0f,
		.vert_accel_హద్దు = 0.0f,
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
		.ste_రేటు_సమయం_const = 0.1f,
		.seb_రేటు_ff = 1.0f,
		.pitch_వేగం_బరువు = 1.0f,
		.integrator_gain_pitch = 0.0f,
		.pitch_damping_gain = 0.0f,
		.integrator_gain_throttle = 0.0f,
		.throttle_damping_gain = 0.0f,
		.throttle_slewrate = 0.0f,
		.ఎక్కించు_గుణకం_correction = 0.0f,
		.ఎక్కించు_గుణకం = 1.0f,
		.fast_descend = 0.f
	};

	TECSControl::Flag _control_flag{
		.airspeed_enabled = అబద్ధం,
		.detect_underspeed_enabled = అబద్ధం,
	};

	/**
	 * @brief Set fast descend value
	 *
	 * @param alt_setpoint is the altitude setpoint
	 * @param alt is the current altitude
	 */
	శూన్య సమితి_Fast_Descend(దశాంశం alt_setpoint, దశాంశం alt);
};

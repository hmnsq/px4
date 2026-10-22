/****************************************************************************
 *
 *   Copyright (C) 2026 PX4 Development Team. All rights reserved.
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

#అన్తర్భావ <gtest/gtest.h>
#అన్తర్భావ <cmath>
#అన్తర్భావ <unistd.h>

#అన్తర్భావ "TECS.hpp"

ఉపయోగించి నామాకాశ math;

నామాకాశ
{

// Representative fixed-wing parameter set, enough to exercise the pitch control loop.
TECSControl::Param makeParam()
{
	TECSControl::Param param{};
	param.max_sink_రేటు = 5.f;
	param.min_sink_రేటు = 2.f;
	param.max_climb_రేటు = 5.f;
	param.vert_accel_హద్దు = 10.f;
	param.equivalent_airspeed_trim = 15.f;
	param.tas_min = 10.f;
	param.tas_stall = 7.f;
	param.tas_max = 30.f;
	param.tas_trim = 15.f;
	param.pitch_max = radians(15.f);
	param.pitch_min = radians(-15.f);
	param.level_flight_pitch = 0.f;
	param.throttle_trim = 0.5f;
	param.throttle_max = 1.f;
	param.throttle_min = 0.f;
	param.altitude_error_gain = 0.2f;
	param.altitude_setpoint_gain_ff = 0.f;
	param.airspeed_error_gain = 0.1f;
	param.ste_రేటు_సమయం_const = 0.1f;
	param.seb_రేటు_ff = 1.f;
	param.pitch_వేగం_బరువు = 1.f;
	param.integrator_gain_pitch = 0.4f;
	param.pitch_damping_gain = 0.1f;
	param.integrator_gain_throttle = 0.3f;
	param.throttle_damping_gain = 0.1f;
	param.throttle_slewrate = 0.f;
	param.ఎక్కించు_గుణకం_correction = 0.f;
	param.ఎక్కించు_గుణకం = 1.f;
	param.fast_descend = 0.f;
	తిరిగి param;
}

TECSControl::Flag makeFlag()
{
	TECSControl::Flag flag{};
	flag.airspeed_enabled = నిజం;
	flag.detect_underspeed_enabled = నిజం;
	తిరిగి flag;
}

TECSControl::ఆదానం_2 make_ఆదానం()
{
	TECSControl::ఆదానం_2 ఆదానం{};
	ఆదానం.altitude = 100.f;
	ఆదానం.altitude_రేటు = 0.f;
	ఆదానం.tas = 15.f;
	ఆదానం.tas_రేటు = 0.f;
	తిరిగి ఆదానం;
}

TECSControl::Setpoint makeSetpoint()
{
	TECSControl::Setpoint setpoint{};
	setpoint.altitude_reference.alt = 100.f;
	setpoint.altitude_reference.alt_రేటు = 0.f;
	setpoint.altitude_రేటు_setpoint_ప్రత్యక్ష = NAN;
	setpoint.tas_setpoint = 15.f;
	తిరిగి setpoint;
}

// Run the detector for one cycle and report the resulting underspeed ratio.
దశాంశం underspeed_నిష్పత్తి(దశాంశం tas, TECSControl::Param param, స్థిరరాశి TECSControl::Flag &flag)
{
	TECSControl control;
	TECSControl::ఆదానం_2 ఆదానం = make_ఆదానం();
	ఆదానం.tas = tas;

	control.initialize(makeSetpoint(), ఆదానం, param, flag);
	control.update(0.02f, makeSetpoint(), ఆదానం, param, flag);

	తిరిగి control.get_Underspeed_నిష్పత్తి();
}

} // namespace

// A single non-finite specific-energy-balance rate setpoint (here injected through the
// true airspeed setpoint, mirroring what happens on the energy/speed side when exiting
// offboard velocity mode) must not permanently corrupt the pitch integrator. Regression
// test for the in-flight NaN lockup reported in #25906.
TEST(TECSControlTest, Pitch_Integrator_Rejects_Non_Finite_ఆదానం)
{
	TECSControl control;
	TECSControl::Param param = makeParam();
	స్థిరరాశి TECSControl::Flag flag = makeFlag();
	స్థిరరాశి TECSControl::ఆదానం_2 ఆదానం = make_ఆదానం();

	control.initialize(makeSetpoint(), ఆదానం, param, flag);

	// Run a few healthy cycles so the integrator builds up some steady-state memory.
	TECSControl::Setpoint setpoint = makeSetpoint();
	setpoint.altitude_reference.alt = 120.f; // climb demand drives the integrator off zero
	స్థిరరాశి దశాంశం dt = 0.02f;

	కొరకు (పూర్ణాంకం i = 0; i < 50; i++) {
		control.update(dt, setpoint, ఆదానం, param, flag);
	}

	స్థిరరాశి దశాంశం integrator_before = control.getDebugOutput().pitch_integrator;
	ASSERT_TRUE(PX4_ISFINITE(integrator_before));

	// Inject a single non-finite setpoint, the kind of one-frame glitch seen on mode exit.
	TECSControl::Setpoint bad_setpoint = setpoint;
	bad_setpoint.tas_setpoint = NAN;
	control.update(dt, bad_setpoint, ఆదానం, param, flag);

	// The bad frame must not propagate into the integrator or the pitch demand.
	EXPECT_TRUE(PX4_ISFINITE(control.getDebugOutput().pitch_integrator));
	EXPECT_TRUE(PX4_ISFINITE(control.getPitchSetpoint()));

	// Resume healthy updates, the controller must still produce finite output.
	కొరకు (పూర్ణాంకం i = 0; i < 10; i++) {
		control.update(dt, setpoint, ఆదానం, param, flag);
		EXPECT_TRUE(PX4_ISFINITE(control.getDebugOutput().pitch_integrator));
		EXPECT_TRUE(PX4_ISFINITE(control.getPitchSetpoint()));
	}
}

// Directly corrupt the integrator state and confirm the safety reset in
// _calcPitchControlUpdate cleans it. The public update() path zeroes bad
// integrator *input* before it can reach the state, so this branch is only
// reachable if the state itself is already non-finite (e.g. corrupted memory
// or an unguarded upstream write). The FRIEND_TEST seam lets us reach it.
TEST(TECSControlTest, PitchIntegratorResetsCorruptedState)
{
	TECSControl control;
	TECSControl::Param param = makeParam();
	స్థిరరాశి TECSControl::Flag flag = makeFlag();
	స్థిరరాశి TECSControl::ఆదానం_2 ఆదానం = make_ఆదానం();

	control.initialize(makeSetpoint(), ఆదానం, param, flag);

	TECSControl::Setpoint setpoint = makeSetpoint();
	setpoint.altitude_reference.alt = 120.f;
	స్థిరరాశి దశాంశం dt = 0.02f;

	// Pre-corrupt the integrator state, bypassing the input guard entirely.
	control._pitch_integ_state = NAN;

	// A single healthy update must detect and clear the corrupted state.
	control.update(dt, setpoint, ఆదానం, param, flag);

	EXPECT_TRUE(PX4_ISFINITE(control._pitch_integ_state));
	EXPECT_TRUE(PX4_ISFINITE(control.getDebugOutput().pitch_integrator));
	EXPECT_TRUE(PX4_ISFINITE(control.getPitchSetpoint()));

	// And the controller keeps producing finite output afterwards.
	కొరకు (పూర్ణాంకం i = 0; i < 10; i++) {
		control.update(dt, setpoint, ఆదానం, param, flag);
		EXPECT_TRUE(PX4_ISFINITE(control._pitch_integ_state));
		EXPECT_TRUE(PX4_ISFINITE(control.getPitchSetpoint()));
	}
}

// Repeated non-finite input across many consecutive frames must never brick the controller:
// every cycle has to keep both the integrator state and the pitch setpoint finite.
TEST(TECSControlTest, Pitch_Integrator_Survives_Sustained_Non_Finite_ఆదానం)
{
	TECSControl control;
	TECSControl::Param param = makeParam();
	స్థిరరాశి TECSControl::Flag flag = makeFlag();
	స్థిరరాశి TECSControl::ఆదానం_2 ఆదానం = make_ఆదానం();

	control.initialize(makeSetpoint(), ఆదానం, param, flag);

	TECSControl::Setpoint bad_setpoint = makeSetpoint();
	bad_setpoint.tas_setpoint = NAN;
	స్థిరరాశి దశాంశం dt = 0.02f;

	కొరకు (పూర్ణాంకం i = 0; i < 100; i++) {
		control.update(dt, bad_setpoint, ఆదానం, param, flag);
		EXPECT_TRUE(PX4_ISFINITE(control.getDebugOutput().pitch_integrator));
		EXPECT_TRUE(PX4_ISFINITE(control.getPitchSetpoint()));
	}
}

// Below the minimum airspeed, the conversion from energy balance rate to pitch must use the minimum airspeed,
// as the pitch integrator does, so the pitch command stays bounded as airspeed drops instead of growing with
// the inverse of the airspeed.
TEST(TECSControlTest, Pitch_Output_ఉపయోగిస్తుంది_Minimum_Airspeed_క్రింద_It)
{
	TECSControl::Param param = makeParam();
	param.pitch_వేగం_బరువు = 0.f; // pitch controls height only, so airspeed enters solely through the conversion
	param.pitch_max = 1.5f; // keep both commands clear of the pitch limits
	param.pitch_min = -1.5f;

	TECSControl::Flag flag = makeFlag();
	flag.detect_underspeed_enabled = అబద్ధం; // underspeed handling would shift the weights to speed

	TECSControl::Setpoint setpoint = makeSetpoint();
	setpoint.altitude_reference.alt = 102.f; // small climb demand

	TECSControl::ఆదానం_2 క్రింద_min = make_ఆదానం();
	క్రింద_min.tas = 0.2f * param.tas_min;
	TECSControl::ఆదానం_2 at_min = make_ఆదానం();
	at_min.tas = param.tas_min;

	TECSControl control_క్రింద_min;
	control_క్రింద_min.initialize(setpoint, క్రింద_min, param, flag);
	TECSControl control_at_min;
	control_at_min.initialize(setpoint, at_min, param, flag);

	EXPECT_GT(control_at_min.getPitchSetpoint(), 0.f);
	EXPECT_FLOAT_EQ(control_క్రింద_min.getPitchSetpoint(), control_at_min.getPitchSetpoint());
}

// The reported altitude time constant must be the one the controller uses, including above 100 s.
TEST(TECSTest, Reports_Configured_Altitude_సమయం_Constant)
{
	TECS tecs;

	tecs.సమితి_altitude_error_సమయం_constant(200.f);
	EXPECT_FLOAT_EQ(tecs.get_altitude_error_సమయం_constant(), 200.f);

	tecs.సమితి_altitude_error_సమయం_constant(0.f);
	EXPECT_FLOAT_EQ(tecs.get_altitude_error_సమయం_constant(), 0.1f);
}

// A reset with the airspeed sensor disabled and an invalid reading must seed the controller with the filtered
// (trim) airspeed, not the raw reading, so neither the reset nor the updates after it produce non-finite commands.
TEST(TECSTest, ResetWithoutValidAirspeedKeepsCommandsFinite)
{
	TECS tecs;
	tecs.enable_airspeed(అబద్ధం);
	tecs.సమితి_integrator_gain_pitch(0.1f);
	tecs.సమితి_integrator_gain_throttle(0.1f);

	స్థిరరాశి TECS::ఆదానం_2 ఆదానం{
		.altitude = 100.f,
		.altitude_రేటు = 0.f,
		.altitude_setpoint = 110.f,
		.altitude_రేటు_setpoint = NAN,
		.equivalent_airspeed = NAN,
		.వేగం_deriv_forward = 0.f,
		.equivalent_airspeed_setpoint = 15.f,
		.eas_గమ్యం_tas = 1.f,
		.throttle_min = 0.f,
		.throttle_max = 1.f,
		.throttle_trim = 0.5f,
		.pitch_min = -0.3f,
		.pitch_max = 0.3f,
		.లక్ష్యం_climbrate = 3.f,
		.లక్ష్యం_sinkrate = 3.f,
	};

	// The first update resets the controller, the following ones run it with a valid time step.
	కొరకు (పూర్ణాంకం i = 0; i < 20; i++) {
		usleep(20000);
		tecs.update(ఆదానం);
		EXPECT_TRUE(PX4_ISFINITE(tecs.get_pitch_setpoint())) << "update " << i;
		EXPECT_TRUE(PX4_ISFINITE(tecs.get_throttle_setpoint())) << "update " << i;
	}
}

నామాకాశ
{

constexpr దశాంశం kDt = 0.02f;

TECSAltitudeReferenceModel::Param makeReferenceParam()
{
	TECSAltitudeReferenceModel::Param param{};
	param.లక్ష్యం_climbrate = 3.f;
	param.లక్ష్యం_sinkrate = 3.f;
	param.vert_accel_హద్దు = 5.f;
	param.max_climb_రేటు = 5.f;
	param.max_sink_రేటు = 5.f;
	తిరిగి param;
}

} // namespace

// ---------------------------------------------------------------------------------------------------------------------
// Airspeed filter
// ---------------------------------------------------------------------------------------------------------------------

TEST(TECSAirspeedFilterTest, Converges_గమ్యం_Measurement)
{
	TECSAirspeedFilter filter;
	filter.initialize(10.f, 15.f, నిజం);

	కొరకు (పూర్ణాంకం i = 0; i < 500; i++) {
		filter.update(kDt, {.equivalent_airspeed = 20.f, .equivalent_airspeed_రేటు = 0.f}, {.equivalent_airspeed_trim = 15.f},
			      నిజం);
	}

	EXPECT_NEAR(filter.getState().వేగం, 20.f, 0.01f);
	EXPECT_NEAR(filter.getState().వేగం_రేటు, 0.f, 0.01f);
}

// Without a sensor, the filter must settle on trim airspeed: the airspeed-less controller relies on it for the
// conversion between climb angle and energy balance rate.
TEST(TECSAirspeedFilterTest, Falls_వెనుక_గమ్యం_Trim_Without_Sensor)
{
	TECSAirspeedFilter filter;
	filter.initialize(20.f, 15.f, నిజం);

	కొరకు (పూర్ణాంకం i = 0; i < 500; i++) {
		filter.update(kDt, {.equivalent_airspeed = 20.f, .equivalent_airspeed_రేటు = 0.f}, {.equivalent_airspeed_trim = 15.f},
			      అబద్ధం);
	}

	EXPECT_NEAR(filter.getState().వేగం, 15.f, 0.01f);
}

TEST(TECSAirspeedFilterTest, Initializes_గమ్యం_Trim_Without_Valid_Measurement)
{
	TECSAirspeedFilter filter;

	filter.initialize(NAN, 15.f, నిజం);
	EXPECT_FLOAT_EQ(filter.getState().వేగం, 15.f);

	filter.initialize(20.f, 15.f, అబద్ధం);
	EXPECT_FLOAT_EQ(filter.getState().వేగం, 15.f);
	EXPECT_FLOAT_EQ(filter.getState().వేగం_రేటు, 0.f);
}

TEST(TECSAirspeedFilterTest, NeverEstimatesNegativeAirspeed)
{
	TECSAirspeedFilter filter;
	filter.initialize(1.f, 15.f, నిజం);

	కొరకు (పూర్ణాంకం i = 0; i < 200; i++) {
		filter.update(kDt, {.equivalent_airspeed = 0.f, .equivalent_airspeed_రేటు = -50.f}, {.equivalent_airspeed_trim = 15.f},
			      నిజం);
		ASSERT_GE(filter.getState().వేగం, 0.f) << "update " << i;
	}
}

// ---------------------------------------------------------------------------------------------------------------------
// Altitude reference model
// ---------------------------------------------------------------------------------------------------------------------

// The reference climbs no faster than the target climb rate and reaches the altitude setpoint.
TEST(TECSAltitudeReferenceModelTest, Climbs_At_లక్ష్యం_రేటు_గమ్యం_Setpoint)
{
	స్థిరరాశి TECSAltitudeReferenceModel::Param param = makeReferenceParam();
	TECSAltitudeReferenceModel model;
	model.initialize({.alt = 100.f, .alt_రేటు = 0.f});

	దశాంశం max_రేటు = 0.f;

	కొరకు (పూర్ణాంకం i = 0; i < 3000; i++) {
		model.update(kDt, {.alt = 150.f, .alt_రేటు = NAN}, 100.f, 0.f, param);
		max_రేటు = fmaxf(max_రేటు, model.getAltitudeReference().alt_రేటు);
	}

	EXPECT_LE(max_రేటు, param.లక్ష్యం_climbrate + 1e-3f);
	EXPECT_NEAR(max_రేటు, param.లక్ష్యం_climbrate, 0.05f);
	EXPECT_NEAR(model.getAltitudeReference().alt, 150.f, 0.01f);
	EXPECT_NEAR(model.getAltitudeReference().alt_రేటు, 0.f, 0.01f);
	EXPECT_FALSE(PX4_ISFINITE(model.get_ఎత్తు_రేటు_Setpoint_ప్రత్యక్ష()));
}

// A height rate setpoint is passed on as a direct, ramped height rate setpoint instead of an altitude reference.
TEST(TECSAltitudeReferenceModelTest, Follows_ఎత్తు_రేటు_Setpoint)
{
	స్థిరరాశి TECSAltitudeReferenceModel::Param param = makeReferenceParam();
	TECSAltitudeReferenceModel model;
	model.initialize({.alt = 100.f, .alt_రేటు = 0.f});

	కొరకు (పూర్ణాంకం i = 0; i < 500; i++) {
		model.update(kDt, {.alt = NAN, .alt_రేటు = 2.f}, 100.f, 0.f, param);
	}

	EXPECT_NEAR(model.get_ఎత్తు_రేటు_Setpoint_ప్రత్యక్ష(), 2.f, 0.01f);
}

// Without any setpoint, the reference holds the current altitude.
TEST(TECSAltitudeReferenceModelTest, HoldsCurrentAltitudeWithoutSetpoint)
{
	స్థిరరాశి TECSAltitudeReferenceModel::Param param = makeReferenceParam();
	TECSAltitudeReferenceModel model;
	model.initialize({.alt = 100.f, .alt_రేటు = 0.f});

	కొరకు (పూర్ణాంకం i = 0; i < 500; i++) {
		model.update(kDt, {.alt = NAN, .alt_రేటు = NAN}, 120.f, 0.f, param);
	}

	EXPECT_NEAR(model.getAltitudeReference().alt, 120.f, 0.01f);
}

// ---------------------------------------------------------------------------------------------------------------------
// Controller
// ---------------------------------------------------------------------------------------------------------------------

// Initialization always runs the altitude loop: a direct height rate setpoint must not change the initial commands.
TEST(TECSControlTest, Initialize_Ignores_ప్రత్యక్ష_ఎత్తు_రేటు_Setpoint)
{
	స్థిరరాశి TECSControl::Param param = makeParam();
	స్థిరరాశి TECSControl::Flag flag = makeFlag();
	స్థిరరాశి TECSControl::ఆదానం_2 ఆదానం = make_ఆదానం();

	TECSControl::Setpoint without_ప్రత్యక్ష = makeSetpoint();
	without_ప్రత్యక్ష.altitude_reference.alt = 110.f;
	TECSControl::Setpoint with_ప్రత్యక్ష = without_ప్రత్యక్ష;
	with_ప్రత్యక్ష.altitude_రేటు_setpoint_ప్రత్యక్ష = 4.f;

	TECSControl control_without;
	control_without.initialize(without_ప్రత్యక్ష, ఆదానం, param, flag);
	TECSControl control_with;
	control_with.initialize(with_ప్రత్యక్ష, ఆదానం, param, flag);

	EXPECT_FLOAT_EQ(control_with.getPitchSetpoint(), control_without.getPitchSetpoint());
	EXPECT_FLOAT_EQ(control_with.getThrottleSetpoint(), control_without.getThrottleSetpoint());
	EXPECT_FLOAT_EQ(control_with.getDebugOutput().altitude_రేటు_control,
			control_without.getDebugOutput().altitude_రేటు_control);
}

// During updates, a finite direct height rate setpoint replaces the altitude loop.
TEST(TECSControlTest, Update_ఉపయోగిస్తుంది_ప్రత్యక్ష_ఎత్తు_రేటు_Setpoint)
{
	స్థిరరాశి TECSControl::Param param = makeParam();
	స్థిరరాశి TECSControl::Flag flag = makeFlag();
	స్థిరరాశి TECSControl::ఆదానం_2 ఆదానం = make_ఆదానం();

	TECSControl control;
	control.initialize(makeSetpoint(), ఆదానం, param, flag);

	TECSControl::Setpoint setpoint = makeSetpoint();
	setpoint.altitude_రేటు_setpoint_ప్రత్యక్ష = 2.f;
	control.update(kDt, setpoint, ఆదానం, param, flag);

	EXPECT_FLOAT_EQ(control.getDebugOutput().altitude_రేటు_control, 2.f);
}

// Once the pitch command saturates, the pitch integrator must not wind up further.
TEST(TECSControlTest, Pitch_Integrator_Does_Not_Wind_పైకి_At_Pitch_హద్దు)
{
	TECSControl::Param param = makeParam();
	param.pitch_max = radians(3.f);
	స్థిరరాశి TECSControl::Flag flag = makeFlag();
	స్థిరరాశి TECSControl::ఆదానం_2 ఆదానం = make_ఆదానం();

	TECSControl::Setpoint setpoint = makeSetpoint();
	setpoint.altitude_reference.alt = 150.f; // climb demand the pitch limit cannot satisfy

	TECSControl control;
	control.initialize(makeSetpoint(), ఆదానం, param, flag);

	కొరకు (పూర్ణాంకం i = 0; i < 100; i++) {
		control.update(kDt, setpoint, ఆదానం, param, flag);
	}

	ASSERT_FLOAT_EQ(control.getPitchSetpoint(), param.pitch_max);
	స్థిరరాశి దశాంశం integrator_saturated = control.getDebugOutput().pitch_integrator;

	కొరకు (పూర్ణాంకం i = 0; i < 500; i++) {
		control.update(kDt, setpoint, ఆదానం, param, flag);
		EXPECT_LE(control.getDebugOutput().pitch_integrator, integrator_saturated) << "update " << i;
	}
}

// With equal throttle limits the throttle sits at both limits at once, so the throttle integrator must not move in
// either direction, whether the energy demand asks for more throttle (climb) or less (descent).
TEST(TECSControlTest, ThrottleIntegratorHoldsWithEqualThrottleLimits)
{
	TECSControl::Param param = makeParam();
	param.throttle_min = 0.4f;
	param.throttle_max = 0.4f;
	param.throttle_trim = 0.4f;
	స్థిరరాశి TECSControl::Flag flag = makeFlag();
	స్థిరరాశి TECSControl::ఆదానం_2 ఆదానం = make_ఆదానం();

	కొరకు (స్థిరరాశి దశాంశం altitude_setpoint : {150.f, 50.f}) {
		TECSControl::Setpoint setpoint = makeSetpoint();
		setpoint.altitude_reference.alt = altitude_setpoint;

		TECSControl control;
		control.initialize(makeSetpoint(), ఆదానం, param, flag);

		కొరకు (పూర్ణాంకం i = 0; i < 200; i++) {
			control.update(kDt, setpoint, ఆదానం, param, flag);
			EXPECT_FLOAT_EQ(control.getThrottleSetpoint(), 0.4f);
			EXPECT_FLOAT_EQ(control.getDebugOutput().throttle_integrator, 0.f)
					<< "altitude setpoint " << altitude_setpoint << ", update " << i;
		}
	}
}

TEST(TECSControlTest, Throttle_Respects_Slew_రేటు)
{
	TECSControl::Param param = makeParam();
	param.throttle_slewrate = 0.5f;
	స్థిరరాశి TECSControl::Flag flag = makeFlag();
	స్థిరరాశి TECSControl::ఆదానం_2 ఆదానం = make_ఆదానం();

	TECSControl::Setpoint setpoint = makeSetpoint();
	setpoint.altitude_reference.alt = 150.f; // step in demand

	TECSControl control;
	control.initialize(makeSetpoint(), ఆదానం, param, flag);
	స్థిరరాశి దశాంశం max_అడుగు = kDt * (param.throttle_max - param.throttle_min) * param.throttle_slewrate;

	దశాంశం మునుపటి = control.getThrottleSetpoint();

	కొరకు (పూర్ణాంకం i = 0; i < 100; i++) {
		control.update(kDt, setpoint, ఆదానం, param, flag);
		EXPECT_LE(fabsf(control.getThrottleSetpoint() - మునుపటి), max_అడుగు + 1e-6f) << "update " << i;
		మునుపటి = control.getThrottleSetpoint();
	}

	EXPECT_GT(మునుపటి, param.throttle_trim); // the throttle did move towards the climb demand
}

// The pitch command changes no faster than the vertical acceleration limit allows at the current airspeed.
TEST(TECSControlTest, Pitch_Respects_Vertical_Acceleration_హద్దు)
{
	TECSControl::Param param = makeParam();
	param.vert_accel_హద్దు = 2.f;
	స్థిరరాశి TECSControl::Flag flag = makeFlag();
	స్థిరరాశి TECSControl::ఆదానం_2 ఆదానం = make_ఆదానం();

	TECSControl::Setpoint setpoint = makeSetpoint();
	setpoint.altitude_reference.alt = 150.f;

	TECSControl control;
	control.initialize(makeSetpoint(), ఆదానం, param, flag);
	స్థిరరాశి దశాంశం max_అడుగు = kDt * param.vert_accel_హద్దు / ఆదానం.tas;

	దశాంశం మునుపటి = control.getPitchSetpoint();

	కొరకు (పూర్ణాంకం i = 0; i < 100; i++) {
		control.update(kDt, setpoint, ఆదానం, param, flag);
		EXPECT_LE(fabsf(control.getPitchSetpoint() - మునుపటి), max_అడుగు + 1e-6f) << "update " << i;
		మునుపటి = control.getPitchSetpoint();
	}

	EXPECT_GT(మునుపటి, 0.f);
}

// Fully engaged fast descend controls airspeed through pitch and gives minimum throttle.
TEST(TECSControlTest, FastDescendCommandsMinimumThrottle)
{
	TECSControl::Param param = makeParam();
	param.throttle_min = 0.1f;
	param.fast_descend = 1.f;
	స్థిరరాశి TECSControl::Flag flag = makeFlag();
	స్థిరరాశి TECSControl::ఆదానం_2 ఆదానం = make_ఆదానం();

	TECSControl control;
	control.initialize(makeSetpoint(), ఆదానం, param, flag);

	కొరకు (పూర్ణాంకం i = 0; i < 50; i++) {
		control.update(kDt, makeSetpoint(), ఆదానం, param, flag);
	}

	EXPECT_FLOAT_EQ(control.getThrottleSetpoint(), param.throttle_min);
}

// Far below the minimum airspeed the controller is fully undersped and commands maximum throttle.
TEST(TECSControlTest, Underspeed_Commands_గరిష్ఠ_Throttle)
{
	స్థిరరాశి TECSControl::Param param = makeParam();
	స్థిరరాశి TECSControl::Flag flag = makeFlag();
	TECSControl::ఆదానం_2 ఆదానం = make_ఆదానం();
	ఆదానం.tas = 0.3f * param.tas_min;

	TECSControl control;
	control.initialize(makeSetpoint(), ఆదానం, param, flag);

	కొరకు (పూర్ణాంకం i = 0; i < 50; i++) {
		control.update(kDt, makeSetpoint(), ఆదానం, param, flag);
	}

	EXPECT_FLOAT_EQ(control.get_Underspeed_నిష్పత్తి(), 1.f);
	EXPECT_FLOAT_EQ(control.getThrottleSetpoint(), param.throttle_max);
}

// Without airspeed, there is no underspeed detection and airspeed errors do not reach pitch or throttle.
TEST(TECSControlTest, AirspeedlessIgnoresAirspeed)
{
	స్థిరరాశి TECSControl::Param param = makeParam();
	TECSControl::Flag flag = makeFlag();
	flag.airspeed_enabled = అబద్ధం;

	TECSControl::ఆదానం_2 slow = make_ఆదానం();
	slow.tas = 0.3f * param.tas_min;
	TECSControl::Setpoint fast_setpoint = makeSetpoint();
	fast_setpoint.tas_setpoint = 25.f;

	TECSControl control_slow;
	control_slow.initialize(makeSetpoint(), slow, param, flag);
	TECSControl control_reference;
	control_reference.initialize(makeSetpoint(), slow, param, flag);

	కొరకు (పూర్ణాంకం i = 0; i < 50; i++) {
		control_slow.update(kDt, fast_setpoint, slow, param, flag);
		control_reference.update(kDt, makeSetpoint(), slow, param, flag);
	}

	EXPECT_FLOAT_EQ(control_slow.get_Underspeed_నిష్పత్తి(), 0.f);
	EXPECT_FLOAT_EQ(control_slow.getDebugOutput().true_airspeed_derivative_control, 0.f);
	EXPECT_FLOAT_EQ(control_slow.getPitchSetpoint(), control_reference.getPitchSetpoint());
	EXPECT_FLOAT_EQ(control_slow.getThrottleSetpoint(), control_reference.getThrottleSetpoint());
}

// Below the minimum airspeed, the pitch integrator must integrate exactly as fast as at the minimum airspeed: it uses
// the same floored airspeed as the pitch output.
TEST(TECSControlTest, Pitch_Integrator_ఉపయోగిస్తుంది_Minimum_Airspeed_క్రింద_It)
{
	TECSControl::Param param = makeParam();
	param.pitch_వేగం_బరువు = 0.f; // pitch controls height only
	param.pitch_max = 1.5f;
	param.pitch_min = -1.5f;

	TECSControl::Flag flag = makeFlag();
	flag.detect_underspeed_enabled = అబద్ధం;

	TECSControl::Setpoint setpoint = makeSetpoint();
	setpoint.altitude_reference.alt = 102.f;

	TECSControl::ఆదానం_2 క్రింద_min = make_ఆదానం();
	క్రింద_min.tas = 0.2f * param.tas_min;
	TECSControl::ఆదానం_2 at_min = make_ఆదానం();
	at_min.tas = param.tas_min;

	TECSControl control_క్రింద_min;
	control_క్రింద_min.initialize(setpoint, క్రింద_min, param, flag);
	control_క్రింద_min.update(kDt, setpoint, క్రింద_min, param, flag);
	TECSControl control_at_min;
	control_at_min.initialize(setpoint, at_min, param, flag);
	control_at_min.update(kDt, setpoint, at_min, param, flag);

	EXPECT_GT(control_at_min.getDebugOutput().pitch_integrator, 0.f);
	EXPECT_FLOAT_EQ(control_క్రింద_min.getDebugOutput().pitch_integrator,
			control_at_min.getDebugOutput().pitch_integrator);
}

// A non-finite airspeed must not produce a non-finite pitch command.
TEST(TECSControlTest, NonFiniteAirspeedKeepsPitchFinite)
{
	స్థిరరాశి TECSControl::Param param = makeParam();
	స్థిరరాశి TECSControl::Flag flag = makeFlag();

	TECSControl control;
	control.initialize(makeSetpoint(), make_ఆదానం(), param, flag);

	TECSControl::ఆదానం_2 ఆదానం = make_ఆదానం();
	ఆదానం.tas = NAN;

	కొరకు (పూర్ణాంకం i = 0; i < 50; i++) {
		control.update(kDt, makeSetpoint(), ఆదానం, param, flag);
		EXPECT_TRUE(PX4_ISFINITE(control.getPitchSetpoint())) << "update " << i;
	}
}

// Banking raises induced drag: the load factor correction must add throttle compared to level flight.
TEST(TECSControlTest, ఎక్కించు_గుణకం_Correction_Adds_Throttle)
{
	TECSControl::Param level = makeParam();
	level.ఎక్కించు_గుణకం_correction = 15.f;
	level.integrator_gain_throttle = 0.f;
	TECSControl::Param banked = level;
	banked.ఎక్కించు_గుణకం = 1.f / cosf(radians(30.f));
	స్థిరరాశి TECSControl::Flag flag = makeFlag();
	స్థిరరాశి TECSControl::ఆదానం_2 ఆదానం = make_ఆదానం();

	TECSControl control_level;
	control_level.initialize(makeSetpoint(), ఆదానం, level, flag);
	TECSControl control_banked;
	control_banked.initialize(makeSetpoint(), ఆదానం, banked, flag);

	EXPECT_GT(control_banked.getThrottleSetpoint(), control_level.getThrottleSetpoint());
}

// The level-flight pitch is fed forward: it shifts the pitch command by exactly its value while unconstrained.
TEST(TECSControlTest, LevelFlightPitchIsFedForward)
{
	TECSControl::Param param = makeParam();
	స్థిరరాశి TECSControl::Flag flag = makeFlag();
	స్థిరరాశి TECSControl::ఆదానం_2 ఆదానం = make_ఆదానం();

	TECSControl::Setpoint setpoint = makeSetpoint();
	setpoint.altitude_reference.alt = 102.f; // small climb demand, well inside the pitch limits

	TECSControl control_without;
	control_without.initialize(setpoint, ఆదానం, param, flag);

	param.level_flight_pitch = radians(4.f);
	TECSControl control_with;
	control_with.initialize(setpoint, ఆదానం, param, flag);

	EXPECT_NEAR(control_with.getPitchSetpoint() - control_without.getPitchSetpoint(), radians(4.f), 1e-6f);
}

// Pitch limits are absolute: a level-flight pitch above the maximum pitch must not lift the command past it.
TEST(TECSControlTest, PitchLimitsAreAbsolute)
{
	TECSControl::Param param = makeParam();
	param.level_flight_pitch = radians(10.f);
	param.pitch_max = radians(5.f);
	స్థిరరాశి TECSControl::Flag flag = makeFlag();
	స్థిరరాశి TECSControl::ఆదానం_2 ఆదానం = make_ఆదానం();

	TECSControl control;
	control.initialize(makeSetpoint(), ఆదానం, param, flag);
	EXPECT_FLOAT_EQ(control.getPitchSetpoint(), param.pitch_max);

	కొరకు (పూర్ణాంకం i = 0; i < 100; i++) {
		control.update(kDt, makeSetpoint(), ఆదానం, param, flag);
		EXPECT_LE(control.getPitchSetpoint(), param.pitch_max) << "update " << i;
	}

	EXPECT_FLOAT_EQ(control.getPitchSetpoint(), param.pitch_max);
}

// A non-finite demand falls back to the integrator on top of the level-flight pitch, not to the bare integrator.
TEST(TECSControlTest, Non_Finite_Demand_Falls_వెనుక_గమ్యం_Level_Flight_Pitch)
{
	TECSControl::Param param = makeParam();
	param.level_flight_pitch = radians(4.f);
	స్థిరరాశి TECSControl::Flag flag = makeFlag();

	TECSControl::Setpoint bad_setpoint = makeSetpoint();
	bad_setpoint.tas_setpoint = NAN;

	TECSControl control;
	control.initialize(bad_setpoint, make_ఆదానం(), param, flag); // integrator reset to zero

	EXPECT_FLOAT_EQ(control.getPitchSetpoint(), param.level_flight_pitch);
}

// The underspeed ramp is anchored on tas_min itself: at or above the minimum airspeed
// there is no mitigation, and it ramps in linearly over a band 10% of tas_min wide.
TEST(TECSUnderspeedTest, NoMitigationAtOrAboveMinimumAirspeed)
{
	TECSControl::Param param = makeParam(); // tas_min = 10, trim = 15
	స్థిరరాశి TECSControl::Flag flag = makeFlag();

	EXPECT_FLOAT_EQ(underspeed_నిష్పత్తి(20.f, param, flag), 0.f);
	EXPECT_FLOAT_EQ(underspeed_నిష్పత్తి(12.f, param, flag), 0.f);
	EXPECT_FLOAT_EQ(underspeed_నిష్పత్తి(10.f, param, flag), 0.f); // exactly at tas_min
}

TEST(TECSUnderspeedTest, Ramps_Linearly_Over_Band_క్రింద_Minimum_Airspeed)
{
	TECSControl::Param param = makeParam(); // band is [9, 10] m/s
	స్థిరరాశి TECSControl::Flag flag = makeFlag();

	EXPECT_NEAR(underspeed_నిష్పత్తి(9.75f, param, flag), 0.25f, 1e-5f);
	EXPECT_NEAR(underspeed_నిష్పత్తి(9.5f, param, flag), 0.5f, 1e-5f);
	EXPECT_NEAR(underspeed_నిష్పత్తి(9.25f, param, flag), 0.75f, 1e-5f);
}

TEST(TECSUnderspeedTest, Saturates_At_నిండిన_Mitigation_క్రింద_Band)
{
	TECSControl::Param param = makeParam();
	స్థిరరాశి TECSControl::Flag flag = makeFlag();

	EXPECT_FLOAT_EQ(underspeed_నిష్పత్తి(9.f, param, flag), 1.f); // bottom of the band
	EXPECT_FLOAT_EQ(underspeed_నిష్పత్తి(5.f, param, flag), 1.f);
	EXPECT_FLOAT_EQ(underspeed_నిష్పత్తి(0.f, param, flag), 1.f);
}

// The ramp width scales with tas_min rather than being a fixed airspeed offset.
TEST(TECSUnderspeedTest, Ramp_వెడల్పు_Scales_With_Minimum_Airspeed)
{
	TECSControl::Param param = makeParam();
	param.tas_min = 20.f; // band is [18, 20] m/s
	స్థిరరాశి TECSControl::Flag flag = makeFlag();

	EXPECT_FLOAT_EQ(underspeed_నిష్పత్తి(20.f, param, flag), 0.f);
	EXPECT_NEAR(underspeed_నిష్పత్తి(19.f, param, flag), 0.5f, 1e-5f);
	EXPECT_FLOAT_EQ(underspeed_నిష్పత్తి(18.f, param, flag), 1.f);
}

// With the stall airspeed close to tas_min the band ends halfway between the two.
TEST(TECSUnderspeedTest, Ramp_చివర్లు_Halfway_గమ్యం_Stall_Airspeed)
{
	TECSControl::Param param = makeParam();
	param.tas_stall = 9.f; // band is [9.5, 10] m/s
	స్థిరరాశి TECSControl::Flag flag = makeFlag();

	EXPECT_FLOAT_EQ(underspeed_నిష్పత్తి(10.f, param, flag), 0.f);
	EXPECT_NEAR(underspeed_నిష్పత్తి(9.75f, param, flag), 0.5f, 1e-5f);
	EXPECT_FLOAT_EQ(underspeed_నిష్పత్తి(9.5f, param, flag), 1.f);
}

// A stall airspeed at or above tas_min collapses the band to a step at tas_min.
TEST(TECSUnderspeedTest, StallAirspeedAtOrAboveMinimumAirspeed)
{
	TECSControl::Param param = makeParam();
	స్థిరరాశి TECSControl::Flag flag = makeFlag();

	కొరకు (స్థిరరాశి దశాంశం tas_stall : {10.f, 12.f}) {
		param.tas_stall = tas_stall;
		EXPECT_FLOAT_EQ(underspeed_నిష్పత్తి(10.f, param, flag), 0.f);
		EXPECT_FLOAT_EQ(underspeed_నిష్పత్తి(9.99f, param, flag), 1.f);
	}
}

TEST(TECSUnderspeedTest, NoMitigationWhenDetectionDisabled)
{
	TECSControl::Param param = makeParam();

	TECSControl::Flag flag = makeFlag();
	flag.detect_underspeed_enabled = అబద్ధం;
	EXPECT_FLOAT_EQ(underspeed_నిష్పత్తి(5.f, param, flag), 0.f);

	flag = makeFlag();
	flag.airspeed_enabled = అబద్ధం;
	EXPECT_FLOAT_EQ(underspeed_నిష్పత్తి(5.f, param, flag), 0.f);
}

నామాకాశ
{

// Pitch setpoint after holding altitude for a while with the given airspeed, airspeed setpoint and airspeed rate.
దశాంశం pitch_Setpoint_At_వేగం(దశాంశం tas, దశాంశం tas_setpoint, దశాంశం pitch_వేగం_బరువు, దశాంశం tas_రేటు = 0.f)
{
	TECSControl control;
	TECSControl::Param param = makeParam();
	param.pitch_వేగం_బరువు = pitch_వేగం_బరువు;
	స్థిరరాశి TECSControl::Flag flag = makeFlag();

	TECSControl::ఆదానం_2 ఆదానం = make_ఆదానం();
	ఆదానం.tas = tas;
	ఆదానం.tas_రేటు = tas_రేటు;

	TECSControl::Setpoint setpoint = makeSetpoint();
	setpoint.tas_setpoint = tas_setpoint;

	control.initialize(setpoint, ఆదానం, param, flag);

	కొరకు (పూర్ణాంకం i = 0; i < 100; i++) {
		control.update(0.02f, setpoint, ఆదానం, param, flag);
	}

	తిరిగి control.getPitchSetpoint();
}

// Fixed-wing TECS configured the way the fw controller does, driven at ~50 Hz.
శూన్య configureTECS(TECS &tecs)
{
	tecs.enable_airspeed(నిజం);
	tecs.సమితి_detect_underspeed_enabled(నిజం);
	tecs.సమితి_equivalent_airspeed_min(10.f);
	tecs.సమితి_equivalent_airspeed_max(30.f);
	tecs.సమితి_equivalent_airspeed_trim(15.f);
	tecs.సమితి_max_climb_రేటు(5.f);
	tecs.సమితి_max_sink_రేటు(5.f);
	tecs.సమితి_min_sink_రేటు(2.f);
	tecs.సమితి_vertical_accel_హద్దు(10.f);
	tecs.సమితి_altitude_error_సమయం_constant(5.f);
	tecs.సమితి_airspeed_error_సమయం_constant(2.f);
	tecs.సమితి_వేగం_బరువు(1.f);
}

శూన్య నిరంతరం_TECS(TECS &tecs, దశాంశం altitude, దశాంశం hgt_setpoint, దశాంశం equivalent_airspeed, పూర్ణాంకం చక్రాలు,
	     దశాంశం eas_గమ్యం_tas = 1.f)
{
	కొరకు (పూర్ణాంకం i = 0; i < చక్రాలు; i++) {
		tecs.update({
			.altitude = altitude,
			.altitude_రేటు = 0.f,
			.altitude_setpoint = hgt_setpoint,
			.altitude_రేటు_setpoint = NAN,
			.equivalent_airspeed = equivalent_airspeed,
			.వేగం_deriv_forward = 0.f,
			.equivalent_airspeed_setpoint = 15.f,
			.eas_గమ్యం_tas = eas_గమ్యం_tas,
			.throttle_min = 0.f,
			.throttle_max = 1.f,
			.throttle_trim = 0.5f,
			.pitch_min = radians(-15.f),
			.pitch_max = radians(15.f),
			.లక్ష్యం_climbrate = 5.f,
			.లక్ష్యం_sinkrate = 5.f,
		});
		usleep(20000); // 20 ms, keeps dt inside [DT_MIN, DT_MAX]
	}
}

} // namespace

// Above trim airspeed, accelerating towards a higher airspeed setpoint is left to the throttle:
// the pitch loop must hold altitude instead of pitching down.
TEST(TECSControlTest, No_Pitch_కిందికి_గమ్యం_Accelerate_Above_Trim)
{
	// WHEN flying above trim airspeed (15 m/s) with a higher airspeed setpoint, at nominal and altitude priority
	స్థిరరాశి దశాంశం pitch_nominal_బరువు = pitch_Setpoint_At_వేగం(20.f, 30.f, 1.f);
	స్థిరరాశి దశాంశం pitch_altitude_ప్రాధాన్యత = pitch_Setpoint_At_వేగం(20.f, 30.f, 0.f);

	// THEN the pitch loop holds altitude and does not pitch down
	EXPECT_NEAR(pitch_nominal_బరువు, 0.f, 1e-4f);
	EXPECT_NEAR(pitch_altitude_ప్రాధాన్యత, 0.f, 1e-4f);
}

// Below trim airspeed, pitch still trades altitude for speed, but only up to trim.
TEST(TECSControlTest, Pitch_కిందికి_గమ్యం_Accelerate_క్రింద_Trim)
{
	// WHEN flying below trim airspeed with the setpoint at trim, and with the setpoint above trim
	స్థిరరాశి దశాంశం pitch_setpoint_at_trim = pitch_Setpoint_At_వేగం(12.f, 15.f, 1.f);
	స్థిరరాశి దశాంశం pitch_setpoint_above_trim = pitch_Setpoint_At_వేగం(12.f, 30.f, 1.f);

	// THEN the pitch loop pitches down, but only as much as needed to reach trim airspeed
	EXPECT_LT(pitch_setpoint_at_trim, 0.f);
	EXPECT_NEAR(pitch_setpoint_above_trim, pitch_setpoint_at_trim, 1e-4f);
}

// With full speed priority (e.g. gliders), pitch is the only way to track the airspeed setpoint.
TEST(TECSControlTest, Pitch_కిందికి_గమ్యం_Accelerate_Above_Trim_With_వేగం_ప్రాధాన్యత)
{
	// WHEN flying above trim airspeed with a higher airspeed setpoint and full speed priority
	స్థిరరాశి దశాంశం pitch = pitch_Setpoint_At_వేగం(20.f, 30.f, 2.f);

	// THEN the pitch loop pitches down to accelerate
	EXPECT_LT(pitch, 0.f);
}

// Between trim airspeed and the setpoint the throttle alone accelerates the vehicle: the pitch loop must not react to
// that acceleration, otherwise it pitches up and turns the throttle's energy into height instead of speed.
TEST(TECSControlTest, No_Pitch_పైకి_While_Throttle_Accelerates_Above_Trim)
{
	// WHEN accelerating between trim airspeed (15 m/s) and the airspeed setpoint, at nominal and altitude priority
	స్థిరరాశి దశాంశం pitch_accelerating = pitch_Setpoint_At_వేగం(18.f, 25.f, 1.f, 1.f);
	స్థిరరాశి దశాంశం pitch_steady = pitch_Setpoint_At_వేగం(18.f, 25.f, 1.f, 0.f);

	// THEN the pitch setpoint is the same as without acceleration
	EXPECT_NEAR(pitch_accelerating, pitch_steady, 1e-5f);
}

// Above the airspeed setpoint pitch takes back speed control: an acceleration there is countered with pitch up.
TEST(TECSControlTest, Pitch_పైకి_On_Acceleration_Above_Setpoint)
{
	// WHEN accelerating above the airspeed setpoint
	స్థిరరాశి దశాంశం pitch_accelerating = pitch_Setpoint_At_వేగం(22.f, 20.f, 1.f, 1.f);
	స్థిరరాశి దశాంశం pitch_steady = pitch_Setpoint_At_వేగం(22.f, 20.f, 1.f, 0.f);

	// THEN the pitch loop pitches up more than without acceleration
	EXPECT_GT(pitch_accelerating, pitch_steady + 1e-3f);
}

// An overspeed above the airspeed setpoint is still corrected with pitch.
TEST(TECSControlTest, Pitch_పైకి_On_Overspeed)
{
	// WHEN flying faster than the airspeed setpoint
	స్థిరరాశి దశాంశం pitch = pitch_Setpoint_At_వేగం(25.f, 20.f, 1.f);

	// THEN the pitch loop pitches up to slow down
	EXPECT_GT(pitch, 0.f);
}

// Between trim airspeed and the setpoint, a speed weight between 1 and 2 must fade the pitch loop's kinetic setpoint
// and estimate equally: accelerating exactly as commanded while holding altitude leaves no energy balance error.
TEST(TECSControlTest, KineticSetpointAndEstimateFadeEquallyAboveTrim)
{
	TECSControl::Param param = makeParam();
	param.pitch_వేగం_బరువు = 1.5f;
	TECSControl::Flag flag = makeFlag();
	flag.detect_underspeed_enabled = అబద్ధం;

	TECSControl::Setpoint setpoint = makeSetpoint(); // altitude reference at the current altitude
	setpoint.tas_setpoint = 18.f;

	TECSControl::ఆదానం_2 ఆదానం = make_ఆదానం();
	ఆదానం.tas = 17.f; // between trim (15 m/s) and the setpoint
	ఆదానం.tas_రేటు = (setpoint.tas_setpoint - ఆదానం.tas) * param.airspeed_error_gain; // the commanded acceleration

	TECSControl control;
	control.initialize(setpoint, ఆదానం, param, flag);

	స్థిరరాశి TECSControl::DebugOutput &debug = control.getDebugOutput();
	EXPECT_NEAR(debug.energy_balance_రేటు_sp, debug.energy_balance_రేటు_estimate, 1e-4f);
}

// The stall airspeed is converted to TAS the same way as the minimum airspeed.
TEST(TECSUnderspeedTest, Stall_Airspeed_Is_Converted_గమ్యం_True_Airspeed)
{
	కొరకు (స్థిరరాశి దశాంశం eas_గమ్యం_tas : {1.f, 1.5f}) {
		TECS tecs;
		configureTECS(tecs);
		tecs.సమితి_equivalent_airspeed_stall(9.f); // band is [9.5, 10] m/s EAS

		నిరంతరం_TECS(tecs, 100.f, 100.f, 9.75f, 3, eas_గమ్యం_tas);

		EXPECT_NEAR(tecs.get_underspeed_నిష్పత్తి(), 0.5f, 1e-5f);
	}
}

// While the aircraft is undersped the altitude reference is pinned to the current
// altitude, so no altitude error accumulates that would command an aggressive pitch-up
// (and a possible secondary stall) once the speed recovers.
TEST(TECSUnderspeedTest, AltitudeReferenceIsHeldAtCurrentAltitudeWhileUndersped)
{
	TECS tecs;
	configureTECS(tecs);

	స్థిరరాశి దశాంశం altitude = 100.f;
	స్థిరరాశి దశాంశం hgt_setpoint = 200.f; // large standing climb demand

	నిరంతరం_TECS(tecs, altitude, hgt_setpoint, 8.f /* well below the 10 m/s minimum */, 25);

	ASSERT_GT(tecs.get_underspeed_నిష్పత్తి(), 0.5f);
	EXPECT_NEAR(tecs.get_స్థితి().altitude_reference, altitude, 1e-3f);
	EXPECT_NEAR(tecs.get_స్థితి().ఎత్తు_రేటు_reference, 0.f, 1e-3f);
}

// Sanity counterpart: at a healthy airspeed the same climb demand does move the reference.
TEST(TECSUnderspeedTest, AltitudeReferenceTracksSetpointWhenNotUndersped)
{
	TECS tecs;
	configureTECS(tecs);

	స్థిరరాశి దశాంశం altitude = 100.f;
	స్థిరరాశి దశాంశం hgt_setpoint = 200.f;

	నిరంతరం_TECS(tecs, altitude, hgt_setpoint, 20.f, 25);

	ASSERT_FLOAT_EQ(tecs.get_underspeed_నిష్పత్తి(), 0.f);
	EXPECT_GT(tecs.get_స్థితి().altitude_reference, altitude);
}

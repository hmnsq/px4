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

#ಅನ್ತರ್ಭಾವ <gtest/gtest.h>
#ಅನ್ತರ್ಭಾವ <cmath>
#ಅನ್ತರ್ಭಾವ <unistd.h>

#ಅನ್ತರ್ಭಾವ "TECS.hpp"

ಬಳಸಿ ನಾಮಾಕಾಶ math;

ನಾಮಾಕಾಶ
{

// Representative fixed-wing parameter set, enough to exercise the pitch control loop.
TECSControl::Param makeParam()
{
	TECSControl::Param param{};
	param.max_sink_ದರ = 5.f;
	param.min_sink_ದರ = 2.f;
	param.max_climb_ದರ = 5.f;
	param.vert_accel_ಎಲ್ಲೆ = 10.f;
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
	param.ste_ದರ_ಸಮಯ_const = 0.1f;
	param.seb_ದರ_ff = 1.f;
	param.pitch_ವೇಗ_ತೂಕ = 1.f;
	param.integrator_gain_pitch = 0.4f;
	param.pitch_damping_gain = 0.1f;
	param.integrator_gain_throttle = 0.3f;
	param.throttle_damping_gain = 0.1f;
	param.throttle_slewrate = 0.f;
	param.ತುಂಬು_ಗುಣಕ_correction = 0.f;
	param.ತುಂಬು_ಗುಣಕ = 1.f;
	param.fast_descend = 0.f;
	ಹಿಂತಿರುಗಿ param;
}

TECSControl::Flag makeFlag()
{
	TECSControl::Flag flag{};
	flag.airspeed_enabled = ನಿಜ;
	flag.detect_underspeed_enabled = ನಿಜ;
	ಹಿಂತಿರುಗಿ flag;
}

TECSControl::ಒಳಹರಿವು_2 make_ಒಳಹರಿವು()
{
	TECSControl::ಒಳಹರಿವು_2 ಒಳಹರಿವು{};
	ಒಳಹರಿವು.altitude = 100.f;
	ಒಳಹರಿವು.altitude_ದರ = 0.f;
	ಒಳಹರಿವು.tas = 15.f;
	ಒಳಹರಿವು.tas_ದರ = 0.f;
	ಹಿಂತಿರುಗಿ ಒಳಹರಿವು;
}

TECSControl::Setpoint makeSetpoint()
{
	TECSControl::Setpoint setpoint{};
	setpoint.altitude_reference.alt = 100.f;
	setpoint.altitude_reference.alt_ದರ = 0.f;
	setpoint.altitude_ದರ_setpoint_ನೇರ = NAN;
	setpoint.tas_setpoint = 15.f;
	ಹಿಂತಿರುಗಿ setpoint;
}

// Run the detector for one cycle and report the resulting underspeed ratio.
ದಶಮಾಂಶ underspeed_ಅನುಪಾತ(ದಶಮಾಂಶ tas, TECSControl::Param param, ಸ್ಥಿರಾಂಕ TECSControl::Flag &flag)
{
	TECSControl control;
	TECSControl::ಒಳಹರಿವು_2 ಒಳಹರಿವು = make_ಒಳಹರಿವು();
	ಒಳಹರಿವು.tas = tas;

	control.initialize(makeSetpoint(), ಒಳಹರಿವು, param, flag);
	control.update(0.02f, makeSetpoint(), ಒಳಹರಿವು, param, flag);

	ಹಿಂತಿರುಗಿ control.get_Underspeed_ಅನುಪಾತ();
}

} // namespace

// A single non-finite specific-energy-balance rate setpoint (here injected through the
// true airspeed setpoint, mirroring what happens on the energy/speed side when exiting
// offboard velocity mode) must not permanently corrupt the pitch integrator. Regression
// test for the in-flight NaN lockup reported in #25906.
TEST(TECSControlTest, Pitch_Integrator_Rejects_Non_Finite_ಒಳಹರಿವು)
{
	TECSControl control;
	TECSControl::Param param = makeParam();
	ಸ್ಥಿರಾಂಕ TECSControl::Flag flag = makeFlag();
	ಸ್ಥಿರಾಂಕ TECSControl::ಒಳಹರಿವು_2 ಒಳಹರಿವು = make_ಒಳಹರಿವು();

	control.initialize(makeSetpoint(), ಒಳಹರಿವು, param, flag);

	// Run a few healthy cycles so the integrator builds up some steady-state memory.
	TECSControl::Setpoint setpoint = makeSetpoint();
	setpoint.altitude_reference.alt = 120.f; // climb demand drives the integrator off zero
	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ dt = 0.02f;

	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < 50; i++) {
		control.update(dt, setpoint, ಒಳಹರಿವು, param, flag);
	}

	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ integrator_before = control.getDebugOutput().pitch_integrator;
	ASSERT_TRUE(PX4_ISFINITE(integrator_before));

	// Inject a single non-finite setpoint, the kind of one-frame glitch seen on mode exit.
	TECSControl::Setpoint bad_setpoint = setpoint;
	bad_setpoint.tas_setpoint = NAN;
	control.update(dt, bad_setpoint, ಒಳಹರಿವು, param, flag);

	// The bad frame must not propagate into the integrator or the pitch demand.
	EXPECT_TRUE(PX4_ISFINITE(control.getDebugOutput().pitch_integrator));
	EXPECT_TRUE(PX4_ISFINITE(control.getPitchSetpoint()));

	// Resume healthy updates, the controller must still produce finite output.
	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < 10; i++) {
		control.update(dt, setpoint, ಒಳಹರಿವು, param, flag);
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
	ಸ್ಥಿರಾಂಕ TECSControl::Flag flag = makeFlag();
	ಸ್ಥಿರಾಂಕ TECSControl::ಒಳಹರಿವು_2 ಒಳಹರಿವು = make_ಒಳಹರಿವು();

	control.initialize(makeSetpoint(), ಒಳಹರಿವು, param, flag);

	TECSControl::Setpoint setpoint = makeSetpoint();
	setpoint.altitude_reference.alt = 120.f;
	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ dt = 0.02f;

	// Pre-corrupt the integrator state, bypassing the input guard entirely.
	control._pitch_integ_state = NAN;

	// A single healthy update must detect and clear the corrupted state.
	control.update(dt, setpoint, ಒಳಹರಿವು, param, flag);

	EXPECT_TRUE(PX4_ISFINITE(control._pitch_integ_state));
	EXPECT_TRUE(PX4_ISFINITE(control.getDebugOutput().pitch_integrator));
	EXPECT_TRUE(PX4_ISFINITE(control.getPitchSetpoint()));

	// And the controller keeps producing finite output afterwards.
	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < 10; i++) {
		control.update(dt, setpoint, ಒಳಹರಿವು, param, flag);
		EXPECT_TRUE(PX4_ISFINITE(control._pitch_integ_state));
		EXPECT_TRUE(PX4_ISFINITE(control.getPitchSetpoint()));
	}
}

// Repeated non-finite input across many consecutive frames must never brick the controller:
// every cycle has to keep both the integrator state and the pitch setpoint finite.
TEST(TECSControlTest, Pitch_Integrator_Survives_Sustained_Non_Finite_ಒಳಹರಿವು)
{
	TECSControl control;
	TECSControl::Param param = makeParam();
	ಸ್ಥಿರಾಂಕ TECSControl::Flag flag = makeFlag();
	ಸ್ಥಿರಾಂಕ TECSControl::ಒಳಹರಿವು_2 ಒಳಹರಿವು = make_ಒಳಹರಿವು();

	control.initialize(makeSetpoint(), ಒಳಹರಿವು, param, flag);

	TECSControl::Setpoint bad_setpoint = makeSetpoint();
	bad_setpoint.tas_setpoint = NAN;
	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ dt = 0.02f;

	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < 100; i++) {
		control.update(dt, bad_setpoint, ಒಳಹರಿವು, param, flag);
		EXPECT_TRUE(PX4_ISFINITE(control.getDebugOutput().pitch_integrator));
		EXPECT_TRUE(PX4_ISFINITE(control.getPitchSetpoint()));
	}
}

// Below the minimum airspeed, the conversion from energy balance rate to pitch must use the minimum airspeed,
// as the pitch integrator does, so the pitch command stays bounded as airspeed drops instead of growing with
// the inverse of the airspeed.
TEST(TECSControlTest, Pitch_Output_ಬಳಸುತ್ತದೆ_Minimum_Airspeed_ಕೆಳಗೆ_It)
{
	TECSControl::Param param = makeParam();
	param.pitch_ವೇಗ_ತೂಕ = 0.f; // pitch controls height only, so airspeed enters solely through the conversion
	param.pitch_max = 1.5f; // keep both commands clear of the pitch limits
	param.pitch_min = -1.5f;

	TECSControl::Flag flag = makeFlag();
	flag.detect_underspeed_enabled = ಸುಳ್ಳು; // underspeed handling would shift the weights to speed

	TECSControl::Setpoint setpoint = makeSetpoint();
	setpoint.altitude_reference.alt = 102.f; // small climb demand

	TECSControl::ಒಳಹರಿವು_2 ಕೆಳಗೆ_min = make_ಒಳಹರಿವು();
	ಕೆಳಗೆ_min.tas = 0.2f * param.tas_min;
	TECSControl::ಒಳಹರಿವು_2 at_min = make_ಒಳಹರಿವು();
	at_min.tas = param.tas_min;

	TECSControl control_ಕೆಳಗೆ_min;
	control_ಕೆಳಗೆ_min.initialize(setpoint, ಕೆಳಗೆ_min, param, flag);
	TECSControl control_at_min;
	control_at_min.initialize(setpoint, at_min, param, flag);

	EXPECT_GT(control_at_min.getPitchSetpoint(), 0.f);
	EXPECT_FLOAT_EQ(control_ಕೆಳಗೆ_min.getPitchSetpoint(), control_at_min.getPitchSetpoint());
}

// The reported altitude time constant must be the one the controller uses, including above 100 s.
TEST(TECSTest, Reports_Configured_Altitude_ಸಮಯ_Constant)
{
	TECS tecs;

	tecs.ಗಣ_altitude_error_ಸಮಯ_constant(200.f);
	EXPECT_FLOAT_EQ(tecs.get_altitude_error_ಸಮಯ_constant(), 200.f);

	tecs.ಗಣ_altitude_error_ಸಮಯ_constant(0.f);
	EXPECT_FLOAT_EQ(tecs.get_altitude_error_ಸಮಯ_constant(), 0.1f);
}

// A reset with the airspeed sensor disabled and an invalid reading must seed the controller with the filtered
// (trim) airspeed, not the raw reading, so neither the reset nor the updates after it produce non-finite commands.
TEST(TECSTest, ResetWithoutValidAirspeedKeepsCommandsFinite)
{
	TECS tecs;
	tecs.enable_airspeed(ಸುಳ್ಳು);
	tecs.ಗಣ_integrator_gain_pitch(0.1f);
	tecs.ಗಣ_integrator_gain_throttle(0.1f);

	ಸ್ಥಿರಾಂಕ TECS::ಒಳಹರಿವು_2 ಒಳಹರಿವು{
		.altitude = 100.f,
		.altitude_ದರ = 0.f,
		.altitude_setpoint = 110.f,
		.altitude_ದರ_setpoint = NAN,
		.equivalent_airspeed = NAN,
		.ವೇಗ_deriv_forward = 0.f,
		.equivalent_airspeed_setpoint = 15.f,
		.eas_ಗಮ್ಯ_tas = 1.f,
		.throttle_min = 0.f,
		.throttle_max = 1.f,
		.throttle_trim = 0.5f,
		.pitch_min = -0.3f,
		.pitch_max = 0.3f,
		.ಗುರಿ_climbrate = 3.f,
		.ಗುರಿ_sinkrate = 3.f,
	};

	// The first update resets the controller, the following ones run it with a valid time step.
	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < 20; i++) {
		usleep(20000);
		tecs.update(ಒಳಹರಿವು);
		EXPECT_TRUE(PX4_ISFINITE(tecs.get_pitch_setpoint())) << "update " << i;
		EXPECT_TRUE(PX4_ISFINITE(tecs.get_throttle_setpoint())) << "update " << i;
	}
}

ನಾಮಾಕಾಶ
{

constexpr ದಶಮಾಂಶ kDt = 0.02f;

TECSAltitudeReferenceModel::Param makeReferenceParam()
{
	TECSAltitudeReferenceModel::Param param{};
	param.ಗುರಿ_climbrate = 3.f;
	param.ಗುರಿ_sinkrate = 3.f;
	param.vert_accel_ಎಲ್ಲೆ = 5.f;
	param.max_climb_ದರ = 5.f;
	param.max_sink_ದರ = 5.f;
	ಹಿಂತಿರುಗಿ param;
}

} // namespace

// ---------------------------------------------------------------------------------------------------------------------
// Airspeed filter
// ---------------------------------------------------------------------------------------------------------------------

TEST(TECSAirspeedFilterTest, Converges_ಗಮ್ಯ_Measurement)
{
	TECSAirspeedFilter filter;
	filter.initialize(10.f, 15.f, ನಿಜ);

	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < 500; i++) {
		filter.update(kDt, {.equivalent_airspeed = 20.f, .equivalent_airspeed_ದರ = 0.f}, {.equivalent_airspeed_trim = 15.f},
			      ನಿಜ);
	}

	EXPECT_NEAR(filter.getState().ವೇಗ, 20.f, 0.01f);
	EXPECT_NEAR(filter.getState().ವೇಗ_ದರ, 0.f, 0.01f);
}

// Without a sensor, the filter must settle on trim airspeed: the airspeed-less controller relies on it for the
// conversion between climb angle and energy balance rate.
TEST(TECSAirspeedFilterTest, Falls_ಹಿಂಭಾಗ_ಗಮ್ಯ_Trim_Without_Sensor)
{
	TECSAirspeedFilter filter;
	filter.initialize(20.f, 15.f, ನಿಜ);

	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < 500; i++) {
		filter.update(kDt, {.equivalent_airspeed = 20.f, .equivalent_airspeed_ದರ = 0.f}, {.equivalent_airspeed_trim = 15.f},
			      ಸುಳ್ಳು);
	}

	EXPECT_NEAR(filter.getState().ವೇಗ, 15.f, 0.01f);
}

TEST(TECSAirspeedFilterTest, Initializes_ಗಮ್ಯ_Trim_Without_Valid_Measurement)
{
	TECSAirspeedFilter filter;

	filter.initialize(NAN, 15.f, ನಿಜ);
	EXPECT_FLOAT_EQ(filter.getState().ವೇಗ, 15.f);

	filter.initialize(20.f, 15.f, ಸುಳ್ಳು);
	EXPECT_FLOAT_EQ(filter.getState().ವೇಗ, 15.f);
	EXPECT_FLOAT_EQ(filter.getState().ವೇಗ_ದರ, 0.f);
}

TEST(TECSAirspeedFilterTest, NeverEstimatesNegativeAirspeed)
{
	TECSAirspeedFilter filter;
	filter.initialize(1.f, 15.f, ನಿಜ);

	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < 200; i++) {
		filter.update(kDt, {.equivalent_airspeed = 0.f, .equivalent_airspeed_ದರ = -50.f}, {.equivalent_airspeed_trim = 15.f},
			      ನಿಜ);
		ASSERT_GE(filter.getState().ವೇಗ, 0.f) << "update " << i;
	}
}

// ---------------------------------------------------------------------------------------------------------------------
// Altitude reference model
// ---------------------------------------------------------------------------------------------------------------------

// The reference climbs no faster than the target climb rate and reaches the altitude setpoint.
TEST(TECSAltitudeReferenceModelTest, Climbs_At_ಗುರಿ_ದರ_ಗಮ್ಯ_Setpoint)
{
	ಸ್ಥಿರಾಂಕ TECSAltitudeReferenceModel::Param param = makeReferenceParam();
	TECSAltitudeReferenceModel model;
	model.initialize({.alt = 100.f, .alt_ದರ = 0.f});

	ದಶಮಾಂಶ max_ದರ = 0.f;

	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < 3000; i++) {
		model.update(kDt, {.alt = 150.f, .alt_ದರ = NAN}, 100.f, 0.f, param);
		max_ದರ = fmaxf(max_ದರ, model.getAltitudeReference().alt_ದರ);
	}

	EXPECT_LE(max_ದರ, param.ಗುರಿ_climbrate + 1e-3f);
	EXPECT_NEAR(max_ದರ, param.ಗುರಿ_climbrate, 0.05f);
	EXPECT_NEAR(model.getAltitudeReference().alt, 150.f, 0.01f);
	EXPECT_NEAR(model.getAltitudeReference().alt_ದರ, 0.f, 0.01f);
	EXPECT_FALSE(PX4_ISFINITE(model.get_ಎತ್ತರ_ದರ_Setpoint_ನೇರ()));
}

// A height rate setpoint is passed on as a direct, ramped height rate setpoint instead of an altitude reference.
TEST(TECSAltitudeReferenceModelTest, Follows_ಎತ್ತರ_ದರ_Setpoint)
{
	ಸ್ಥಿರಾಂಕ TECSAltitudeReferenceModel::Param param = makeReferenceParam();
	TECSAltitudeReferenceModel model;
	model.initialize({.alt = 100.f, .alt_ದರ = 0.f});

	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < 500; i++) {
		model.update(kDt, {.alt = NAN, .alt_ದರ = 2.f}, 100.f, 0.f, param);
	}

	EXPECT_NEAR(model.get_ಎತ್ತರ_ದರ_Setpoint_ನೇರ(), 2.f, 0.01f);
}

// Without any setpoint, the reference holds the current altitude.
TEST(TECSAltitudeReferenceModelTest, HoldsCurrentAltitudeWithoutSetpoint)
{
	ಸ್ಥಿರಾಂಕ TECSAltitudeReferenceModel::Param param = makeReferenceParam();
	TECSAltitudeReferenceModel model;
	model.initialize({.alt = 100.f, .alt_ದರ = 0.f});

	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < 500; i++) {
		model.update(kDt, {.alt = NAN, .alt_ದರ = NAN}, 120.f, 0.f, param);
	}

	EXPECT_NEAR(model.getAltitudeReference().alt, 120.f, 0.01f);
}

// ---------------------------------------------------------------------------------------------------------------------
// Controller
// ---------------------------------------------------------------------------------------------------------------------

// Initialization always runs the altitude loop: a direct height rate setpoint must not change the initial commands.
TEST(TECSControlTest, Initialize_Ignores_ನೇರ_ಎತ್ತರ_ದರ_Setpoint)
{
	ಸ್ಥಿರಾಂಕ TECSControl::Param param = makeParam();
	ಸ್ಥಿರಾಂಕ TECSControl::Flag flag = makeFlag();
	ಸ್ಥಿರಾಂಕ TECSControl::ಒಳಹರಿವು_2 ಒಳಹರಿವು = make_ಒಳಹರಿವು();

	TECSControl::Setpoint without_ನೇರ = makeSetpoint();
	without_ನೇರ.altitude_reference.alt = 110.f;
	TECSControl::Setpoint with_ನೇರ = without_ನೇರ;
	with_ನೇರ.altitude_ದರ_setpoint_ನೇರ = 4.f;

	TECSControl control_without;
	control_without.initialize(without_ನೇರ, ಒಳಹರಿವು, param, flag);
	TECSControl control_with;
	control_with.initialize(with_ನೇರ, ಒಳಹರಿವು, param, flag);

	EXPECT_FLOAT_EQ(control_with.getPitchSetpoint(), control_without.getPitchSetpoint());
	EXPECT_FLOAT_EQ(control_with.getThrottleSetpoint(), control_without.getThrottleSetpoint());
	EXPECT_FLOAT_EQ(control_with.getDebugOutput().altitude_ದರ_control,
			control_without.getDebugOutput().altitude_ದರ_control);
}

// During updates, a finite direct height rate setpoint replaces the altitude loop.
TEST(TECSControlTest, Update_ಬಳಸುತ್ತದೆ_ನೇರ_ಎತ್ತರ_ದರ_Setpoint)
{
	ಸ್ಥಿರಾಂಕ TECSControl::Param param = makeParam();
	ಸ್ಥಿರಾಂಕ TECSControl::Flag flag = makeFlag();
	ಸ್ಥಿರಾಂಕ TECSControl::ಒಳಹರಿವು_2 ಒಳಹರಿವು = make_ಒಳಹರಿವು();

	TECSControl control;
	control.initialize(makeSetpoint(), ಒಳಹರಿವು, param, flag);

	TECSControl::Setpoint setpoint = makeSetpoint();
	setpoint.altitude_ದರ_setpoint_ನೇರ = 2.f;
	control.update(kDt, setpoint, ಒಳಹರಿವು, param, flag);

	EXPECT_FLOAT_EQ(control.getDebugOutput().altitude_ದರ_control, 2.f);
}

// Once the pitch command saturates, the pitch integrator must not wind up further.
TEST(TECSControlTest, Pitch_Integrator_Does_Not_Wind_ಮೇಲೆ_At_Pitch_ಎಲ್ಲೆ)
{
	TECSControl::Param param = makeParam();
	param.pitch_max = radians(3.f);
	ಸ್ಥಿರಾಂಕ TECSControl::Flag flag = makeFlag();
	ಸ್ಥಿರಾಂಕ TECSControl::ಒಳಹರಿವು_2 ಒಳಹರಿವು = make_ಒಳಹರಿವು();

	TECSControl::Setpoint setpoint = makeSetpoint();
	setpoint.altitude_reference.alt = 150.f; // climb demand the pitch limit cannot satisfy

	TECSControl control;
	control.initialize(makeSetpoint(), ಒಳಹರಿವು, param, flag);

	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < 100; i++) {
		control.update(kDt, setpoint, ಒಳಹರಿವು, param, flag);
	}

	ASSERT_FLOAT_EQ(control.getPitchSetpoint(), param.pitch_max);
	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ integrator_saturated = control.getDebugOutput().pitch_integrator;

	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < 500; i++) {
		control.update(kDt, setpoint, ಒಳಹರಿವು, param, flag);
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
	ಸ್ಥಿರಾಂಕ TECSControl::Flag flag = makeFlag();
	ಸ್ಥಿರಾಂಕ TECSControl::ಒಳಹರಿವು_2 ಒಳಹರಿವು = make_ಒಳಹರಿವು();

	ಗಾಗಿ (ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ altitude_setpoint : {150.f, 50.f}) {
		TECSControl::Setpoint setpoint = makeSetpoint();
		setpoint.altitude_reference.alt = altitude_setpoint;

		TECSControl control;
		control.initialize(makeSetpoint(), ಒಳಹರಿವು, param, flag);

		ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < 200; i++) {
			control.update(kDt, setpoint, ಒಳಹರಿವು, param, flag);
			EXPECT_FLOAT_EQ(control.getThrottleSetpoint(), 0.4f);
			EXPECT_FLOAT_EQ(control.getDebugOutput().throttle_integrator, 0.f)
					<< "altitude setpoint " << altitude_setpoint << ", update " << i;
		}
	}
}

TEST(TECSControlTest, Throttle_Respects_Slew_ದರ)
{
	TECSControl::Param param = makeParam();
	param.throttle_slewrate = 0.5f;
	ಸ್ಥಿರಾಂಕ TECSControl::Flag flag = makeFlag();
	ಸ್ಥಿರಾಂಕ TECSControl::ಒಳಹರಿವು_2 ಒಳಹರಿವು = make_ಒಳಹರಿವು();

	TECSControl::Setpoint setpoint = makeSetpoint();
	setpoint.altitude_reference.alt = 150.f; // step in demand

	TECSControl control;
	control.initialize(makeSetpoint(), ಒಳಹರಿವು, param, flag);
	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ max_ಹೆಜ್ಜೆ = kDt * (param.throttle_max - param.throttle_min) * param.throttle_slewrate;

	ದಶಮಾಂಶ ಹಿಂದಿನ = control.getThrottleSetpoint();

	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < 100; i++) {
		control.update(kDt, setpoint, ಒಳಹರಿವು, param, flag);
		EXPECT_LE(fabsf(control.getThrottleSetpoint() - ಹಿಂದಿನ), max_ಹೆಜ್ಜೆ + 1e-6f) << "update " << i;
		ಹಿಂದಿನ = control.getThrottleSetpoint();
	}

	EXPECT_GT(ಹಿಂದಿನ, param.throttle_trim); // the throttle did move towards the climb demand
}

// The pitch command changes no faster than the vertical acceleration limit allows at the current airspeed.
TEST(TECSControlTest, Pitch_Respects_Vertical_Acceleration_ಎಲ್ಲೆ)
{
	TECSControl::Param param = makeParam();
	param.vert_accel_ಎಲ್ಲೆ = 2.f;
	ಸ್ಥಿರಾಂಕ TECSControl::Flag flag = makeFlag();
	ಸ್ಥಿರಾಂಕ TECSControl::ಒಳಹರಿವು_2 ಒಳಹರಿವು = make_ಒಳಹರಿವು();

	TECSControl::Setpoint setpoint = makeSetpoint();
	setpoint.altitude_reference.alt = 150.f;

	TECSControl control;
	control.initialize(makeSetpoint(), ಒಳಹರಿವು, param, flag);
	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ max_ಹೆಜ್ಜೆ = kDt * param.vert_accel_ಎಲ್ಲೆ / ಒಳಹರಿವು.tas;

	ದಶಮಾಂಶ ಹಿಂದಿನ = control.getPitchSetpoint();

	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < 100; i++) {
		control.update(kDt, setpoint, ಒಳಹರಿವು, param, flag);
		EXPECT_LE(fabsf(control.getPitchSetpoint() - ಹಿಂದಿನ), max_ಹೆಜ್ಜೆ + 1e-6f) << "update " << i;
		ಹಿಂದಿನ = control.getPitchSetpoint();
	}

	EXPECT_GT(ಹಿಂದಿನ, 0.f);
}

// Fully engaged fast descend controls airspeed through pitch and gives minimum throttle.
TEST(TECSControlTest, FastDescendCommandsMinimumThrottle)
{
	TECSControl::Param param = makeParam();
	param.throttle_min = 0.1f;
	param.fast_descend = 1.f;
	ಸ್ಥಿರಾಂಕ TECSControl::Flag flag = makeFlag();
	ಸ್ಥಿರಾಂಕ TECSControl::ಒಳಹರಿವು_2 ಒಳಹರಿವು = make_ಒಳಹರಿವು();

	TECSControl control;
	control.initialize(makeSetpoint(), ಒಳಹರಿವು, param, flag);

	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < 50; i++) {
		control.update(kDt, makeSetpoint(), ಒಳಹರಿವು, param, flag);
	}

	EXPECT_FLOAT_EQ(control.getThrottleSetpoint(), param.throttle_min);
}

// Far below the minimum airspeed the controller is fully undersped and commands maximum throttle.
TEST(TECSControlTest, Underspeed_Commands_ಗರಿಷ್ಠ_Throttle)
{
	ಸ್ಥಿರಾಂಕ TECSControl::Param param = makeParam();
	ಸ್ಥಿರಾಂಕ TECSControl::Flag flag = makeFlag();
	TECSControl::ಒಳಹರಿವು_2 ಒಳಹರಿವು = make_ಒಳಹರಿವು();
	ಒಳಹರಿವು.tas = 0.3f * param.tas_min;

	TECSControl control;
	control.initialize(makeSetpoint(), ಒಳಹರಿವು, param, flag);

	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < 50; i++) {
		control.update(kDt, makeSetpoint(), ಒಳಹರಿವು, param, flag);
	}

	EXPECT_FLOAT_EQ(control.get_Underspeed_ಅನುಪಾತ(), 1.f);
	EXPECT_FLOAT_EQ(control.getThrottleSetpoint(), param.throttle_max);
}

// Without airspeed, there is no underspeed detection and airspeed errors do not reach pitch or throttle.
TEST(TECSControlTest, AirspeedlessIgnoresAirspeed)
{
	ಸ್ಥಿರಾಂಕ TECSControl::Param param = makeParam();
	TECSControl::Flag flag = makeFlag();
	flag.airspeed_enabled = ಸುಳ್ಳು;

	TECSControl::ಒಳಹರಿವು_2 slow = make_ಒಳಹರಿವು();
	slow.tas = 0.3f * param.tas_min;
	TECSControl::Setpoint fast_setpoint = makeSetpoint();
	fast_setpoint.tas_setpoint = 25.f;

	TECSControl control_slow;
	control_slow.initialize(makeSetpoint(), slow, param, flag);
	TECSControl control_reference;
	control_reference.initialize(makeSetpoint(), slow, param, flag);

	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < 50; i++) {
		control_slow.update(kDt, fast_setpoint, slow, param, flag);
		control_reference.update(kDt, makeSetpoint(), slow, param, flag);
	}

	EXPECT_FLOAT_EQ(control_slow.get_Underspeed_ಅನುಪಾತ(), 0.f);
	EXPECT_FLOAT_EQ(control_slow.getDebugOutput().true_airspeed_derivative_control, 0.f);
	EXPECT_FLOAT_EQ(control_slow.getPitchSetpoint(), control_reference.getPitchSetpoint());
	EXPECT_FLOAT_EQ(control_slow.getThrottleSetpoint(), control_reference.getThrottleSetpoint());
}

// Below the minimum airspeed, the pitch integrator must integrate exactly as fast as at the minimum airspeed: it uses
// the same floored airspeed as the pitch output.
TEST(TECSControlTest, Pitch_Integrator_ಬಳಸುತ್ತದೆ_Minimum_Airspeed_ಕೆಳಗೆ_It)
{
	TECSControl::Param param = makeParam();
	param.pitch_ವೇಗ_ತೂಕ = 0.f; // pitch controls height only
	param.pitch_max = 1.5f;
	param.pitch_min = -1.5f;

	TECSControl::Flag flag = makeFlag();
	flag.detect_underspeed_enabled = ಸುಳ್ಳು;

	TECSControl::Setpoint setpoint = makeSetpoint();
	setpoint.altitude_reference.alt = 102.f;

	TECSControl::ಒಳಹರಿವು_2 ಕೆಳಗೆ_min = make_ಒಳಹರಿವು();
	ಕೆಳಗೆ_min.tas = 0.2f * param.tas_min;
	TECSControl::ಒಳಹರಿವು_2 at_min = make_ಒಳಹರಿವು();
	at_min.tas = param.tas_min;

	TECSControl control_ಕೆಳಗೆ_min;
	control_ಕೆಳಗೆ_min.initialize(setpoint, ಕೆಳಗೆ_min, param, flag);
	control_ಕೆಳಗೆ_min.update(kDt, setpoint, ಕೆಳಗೆ_min, param, flag);
	TECSControl control_at_min;
	control_at_min.initialize(setpoint, at_min, param, flag);
	control_at_min.update(kDt, setpoint, at_min, param, flag);

	EXPECT_GT(control_at_min.getDebugOutput().pitch_integrator, 0.f);
	EXPECT_FLOAT_EQ(control_ಕೆಳಗೆ_min.getDebugOutput().pitch_integrator,
			control_at_min.getDebugOutput().pitch_integrator);
}

// A non-finite airspeed must not produce a non-finite pitch command.
TEST(TECSControlTest, NonFiniteAirspeedKeepsPitchFinite)
{
	ಸ್ಥಿರಾಂಕ TECSControl::Param param = makeParam();
	ಸ್ಥಿರಾಂಕ TECSControl::Flag flag = makeFlag();

	TECSControl control;
	control.initialize(makeSetpoint(), make_ಒಳಹರಿವು(), param, flag);

	TECSControl::ಒಳಹರಿವು_2 ಒಳಹರಿವು = make_ಒಳಹರಿವು();
	ಒಳಹರಿವು.tas = NAN;

	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < 50; i++) {
		control.update(kDt, makeSetpoint(), ಒಳಹರಿವು, param, flag);
		EXPECT_TRUE(PX4_ISFINITE(control.getPitchSetpoint())) << "update " << i;
	}
}

// Banking raises induced drag: the load factor correction must add throttle compared to level flight.
TEST(TECSControlTest, ತುಂಬು_ಗುಣಕ_Correction_Adds_Throttle)
{
	TECSControl::Param level = makeParam();
	level.ತುಂಬು_ಗುಣಕ_correction = 15.f;
	level.integrator_gain_throttle = 0.f;
	TECSControl::Param banked = level;
	banked.ತುಂಬು_ಗುಣಕ = 1.f / cosf(radians(30.f));
	ಸ್ಥಿರಾಂಕ TECSControl::Flag flag = makeFlag();
	ಸ್ಥಿರಾಂಕ TECSControl::ಒಳಹರಿವು_2 ಒಳಹರಿವು = make_ಒಳಹರಿವು();

	TECSControl control_level;
	control_level.initialize(makeSetpoint(), ಒಳಹರಿವು, level, flag);
	TECSControl control_banked;
	control_banked.initialize(makeSetpoint(), ಒಳಹರಿವು, banked, flag);

	EXPECT_GT(control_banked.getThrottleSetpoint(), control_level.getThrottleSetpoint());
}

// The level-flight pitch is fed forward: it shifts the pitch command by exactly its value while unconstrained.
TEST(TECSControlTest, LevelFlightPitchIsFedForward)
{
	TECSControl::Param param = makeParam();
	ಸ್ಥಿರಾಂಕ TECSControl::Flag flag = makeFlag();
	ಸ್ಥಿರಾಂಕ TECSControl::ಒಳಹರಿವು_2 ಒಳಹರಿವು = make_ಒಳಹರಿವು();

	TECSControl::Setpoint setpoint = makeSetpoint();
	setpoint.altitude_reference.alt = 102.f; // small climb demand, well inside the pitch limits

	TECSControl control_without;
	control_without.initialize(setpoint, ಒಳಹರಿವು, param, flag);

	param.level_flight_pitch = radians(4.f);
	TECSControl control_with;
	control_with.initialize(setpoint, ಒಳಹರಿವು, param, flag);

	EXPECT_NEAR(control_with.getPitchSetpoint() - control_without.getPitchSetpoint(), radians(4.f), 1e-6f);
}

// Pitch limits are absolute: a level-flight pitch above the maximum pitch must not lift the command past it.
TEST(TECSControlTest, PitchLimitsAreAbsolute)
{
	TECSControl::Param param = makeParam();
	param.level_flight_pitch = radians(10.f);
	param.pitch_max = radians(5.f);
	ಸ್ಥಿರಾಂಕ TECSControl::Flag flag = makeFlag();
	ಸ್ಥಿರಾಂಕ TECSControl::ಒಳಹರಿವು_2 ಒಳಹರಿವು = make_ಒಳಹರಿವು();

	TECSControl control;
	control.initialize(makeSetpoint(), ಒಳಹರಿವು, param, flag);
	EXPECT_FLOAT_EQ(control.getPitchSetpoint(), param.pitch_max);

	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < 100; i++) {
		control.update(kDt, makeSetpoint(), ಒಳಹರಿವು, param, flag);
		EXPECT_LE(control.getPitchSetpoint(), param.pitch_max) << "update " << i;
	}

	EXPECT_FLOAT_EQ(control.getPitchSetpoint(), param.pitch_max);
}

// A non-finite demand falls back to the integrator on top of the level-flight pitch, not to the bare integrator.
TEST(TECSControlTest, Non_Finite_Demand_Falls_ಹಿಂಭಾಗ_ಗಮ್ಯ_Level_Flight_Pitch)
{
	TECSControl::Param param = makeParam();
	param.level_flight_pitch = radians(4.f);
	ಸ್ಥಿರಾಂಕ TECSControl::Flag flag = makeFlag();

	TECSControl::Setpoint bad_setpoint = makeSetpoint();
	bad_setpoint.tas_setpoint = NAN;

	TECSControl control;
	control.initialize(bad_setpoint, make_ಒಳಹರಿವು(), param, flag); // integrator reset to zero

	EXPECT_FLOAT_EQ(control.getPitchSetpoint(), param.level_flight_pitch);
}

// The underspeed ramp is anchored on tas_min itself: at or above the minimum airspeed
// there is no mitigation, and it ramps in linearly over a band 10% of tas_min wide.
TEST(TECSUnderspeedTest, NoMitigationAtOrAboveMinimumAirspeed)
{
	TECSControl::Param param = makeParam(); // tas_min = 10, trim = 15
	ಸ್ಥಿರಾಂಕ TECSControl::Flag flag = makeFlag();

	EXPECT_FLOAT_EQ(underspeed_ಅನುಪಾತ(20.f, param, flag), 0.f);
	EXPECT_FLOAT_EQ(underspeed_ಅನುಪಾತ(12.f, param, flag), 0.f);
	EXPECT_FLOAT_EQ(underspeed_ಅನುಪಾತ(10.f, param, flag), 0.f); // exactly at tas_min
}

TEST(TECSUnderspeedTest, Ramps_Linearly_Over_Band_ಕೆಳಗೆ_Minimum_Airspeed)
{
	TECSControl::Param param = makeParam(); // band is [9, 10] m/s
	ಸ್ಥಿರಾಂಕ TECSControl::Flag flag = makeFlag();

	EXPECT_NEAR(underspeed_ಅನುಪಾತ(9.75f, param, flag), 0.25f, 1e-5f);
	EXPECT_NEAR(underspeed_ಅನುಪಾತ(9.5f, param, flag), 0.5f, 1e-5f);
	EXPECT_NEAR(underspeed_ಅನುಪಾತ(9.25f, param, flag), 0.75f, 1e-5f);
}

TEST(TECSUnderspeedTest, Saturates_At_ತುಂಬಿದ_Mitigation_ಕೆಳಗೆ_Band)
{
	TECSControl::Param param = makeParam();
	ಸ್ಥಿರಾಂಕ TECSControl::Flag flag = makeFlag();

	EXPECT_FLOAT_EQ(underspeed_ಅನುಪಾತ(9.f, param, flag), 1.f); // bottom of the band
	EXPECT_FLOAT_EQ(underspeed_ಅನುಪಾತ(5.f, param, flag), 1.f);
	EXPECT_FLOAT_EQ(underspeed_ಅನುಪಾತ(0.f, param, flag), 1.f);
}

// The ramp width scales with tas_min rather than being a fixed airspeed offset.
TEST(TECSUnderspeedTest, Ramp_ಅಗಲ_Scales_With_Minimum_Airspeed)
{
	TECSControl::Param param = makeParam();
	param.tas_min = 20.f; // band is [18, 20] m/s
	ಸ್ಥಿರಾಂಕ TECSControl::Flag flag = makeFlag();

	EXPECT_FLOAT_EQ(underspeed_ಅನುಪಾತ(20.f, param, flag), 0.f);
	EXPECT_NEAR(underspeed_ಅನುಪಾತ(19.f, param, flag), 0.5f, 1e-5f);
	EXPECT_FLOAT_EQ(underspeed_ಅನುಪಾತ(18.f, param, flag), 1.f);
}

// With the stall airspeed close to tas_min the band ends halfway between the two.
TEST(TECSUnderspeedTest, Ramp_ತುದಿಗಳು_Halfway_ಗಮ್ಯ_Stall_Airspeed)
{
	TECSControl::Param param = makeParam();
	param.tas_stall = 9.f; // band is [9.5, 10] m/s
	ಸ್ಥಿರಾಂಕ TECSControl::Flag flag = makeFlag();

	EXPECT_FLOAT_EQ(underspeed_ಅನುಪಾತ(10.f, param, flag), 0.f);
	EXPECT_NEAR(underspeed_ಅನುಪಾತ(9.75f, param, flag), 0.5f, 1e-5f);
	EXPECT_FLOAT_EQ(underspeed_ಅನುಪಾತ(9.5f, param, flag), 1.f);
}

// A stall airspeed at or above tas_min collapses the band to a step at tas_min.
TEST(TECSUnderspeedTest, StallAirspeedAtOrAboveMinimumAirspeed)
{
	TECSControl::Param param = makeParam();
	ಸ್ಥಿರಾಂಕ TECSControl::Flag flag = makeFlag();

	ಗಾಗಿ (ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ tas_stall : {10.f, 12.f}) {
		param.tas_stall = tas_stall;
		EXPECT_FLOAT_EQ(underspeed_ಅನುಪಾತ(10.f, param, flag), 0.f);
		EXPECT_FLOAT_EQ(underspeed_ಅನುಪಾತ(9.99f, param, flag), 1.f);
	}
}

TEST(TECSUnderspeedTest, NoMitigationWhenDetectionDisabled)
{
	TECSControl::Param param = makeParam();

	TECSControl::Flag flag = makeFlag();
	flag.detect_underspeed_enabled = ಸುಳ್ಳು;
	EXPECT_FLOAT_EQ(underspeed_ಅನುಪಾತ(5.f, param, flag), 0.f);

	flag = makeFlag();
	flag.airspeed_enabled = ಸುಳ್ಳು;
	EXPECT_FLOAT_EQ(underspeed_ಅನುಪಾತ(5.f, param, flag), 0.f);
}

ನಾಮಾಕಾಶ
{

// Pitch setpoint after holding altitude for a while with the given airspeed, airspeed setpoint and airspeed rate.
ದಶಮಾಂಶ pitch_Setpoint_At_ವೇಗ(ದಶಮಾಂಶ tas, ದಶಮಾಂಶ tas_setpoint, ದಶಮಾಂಶ pitch_ವೇಗ_ತೂಕ, ದಶಮಾಂಶ tas_ದರ = 0.f)
{
	TECSControl control;
	TECSControl::Param param = makeParam();
	param.pitch_ವೇಗ_ತೂಕ = pitch_ವೇಗ_ತೂಕ;
	ಸ್ಥಿರಾಂಕ TECSControl::Flag flag = makeFlag();

	TECSControl::ಒಳಹರಿವು_2 ಒಳಹರಿವು = make_ಒಳಹರಿವು();
	ಒಳಹರಿವು.tas = tas;
	ಒಳಹರಿವು.tas_ದರ = tas_ದರ;

	TECSControl::Setpoint setpoint = makeSetpoint();
	setpoint.tas_setpoint = tas_setpoint;

	control.initialize(setpoint, ಒಳಹರಿವು, param, flag);

	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < 100; i++) {
		control.update(0.02f, setpoint, ಒಳಹರಿವು, param, flag);
	}

	ಹಿಂತಿರುಗಿ control.getPitchSetpoint();
}

// Fixed-wing TECS configured the way the fw controller does, driven at ~50 Hz.
ಶೂನ್ಯ configureTECS(TECS &tecs)
{
	tecs.enable_airspeed(ನಿಜ);
	tecs.ಗಣ_detect_underspeed_enabled(ನಿಜ);
	tecs.ಗಣ_equivalent_airspeed_min(10.f);
	tecs.ಗಣ_equivalent_airspeed_max(30.f);
	tecs.ಗಣ_equivalent_airspeed_trim(15.f);
	tecs.ಗಣ_max_climb_ದರ(5.f);
	tecs.ಗಣ_max_sink_ದರ(5.f);
	tecs.ಗಣ_min_sink_ದರ(2.f);
	tecs.ಗಣ_vertical_accel_ಎಲ್ಲೆ(10.f);
	tecs.ಗಣ_altitude_error_ಸಮಯ_constant(5.f);
	tecs.ಗಣ_airspeed_error_ಸಮಯ_constant(2.f);
	tecs.ಗಣ_ವೇಗ_ತೂಕ(1.f);
}

ಶೂನ್ಯ ಸತತ_TECS(TECS &tecs, ದಶಮಾಂಶ altitude, ದಶಮಾಂಶ hgt_setpoint, ದಶಮಾಂಶ equivalent_airspeed, ಪೂರ್ಣಾಂಕ ಚಕ್ರಗಳು,
	     ದಶಮಾಂಶ eas_ಗಮ್ಯ_tas = 1.f)
{
	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < ಚಕ್ರಗಳು; i++) {
		tecs.update({
			.altitude = altitude,
			.altitude_ದರ = 0.f,
			.altitude_setpoint = hgt_setpoint,
			.altitude_ದರ_setpoint = NAN,
			.equivalent_airspeed = equivalent_airspeed,
			.ವೇಗ_deriv_forward = 0.f,
			.equivalent_airspeed_setpoint = 15.f,
			.eas_ಗಮ್ಯ_tas = eas_ಗಮ್ಯ_tas,
			.throttle_min = 0.f,
			.throttle_max = 1.f,
			.throttle_trim = 0.5f,
			.pitch_min = radians(-15.f),
			.pitch_max = radians(15.f),
			.ಗುರಿ_climbrate = 5.f,
			.ಗುರಿ_sinkrate = 5.f,
		});
		usleep(20000); // 20 ms, keeps dt inside [DT_MIN, DT_MAX]
	}
}

} // namespace

// Above trim airspeed, accelerating towards a higher airspeed setpoint is left to the throttle:
// the pitch loop must hold altitude instead of pitching down.
TEST(TECSControlTest, No_Pitch_ಕೆಳಕ್ಕೆ_ಗಮ್ಯ_Accelerate_Above_Trim)
{
	// WHEN flying above trim airspeed (15 m/s) with a higher airspeed setpoint, at nominal and altitude priority
	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ pitch_nominal_ತೂಕ = pitch_Setpoint_At_ವೇಗ(20.f, 30.f, 1.f);
	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ pitch_altitude_ಆದ್ಯತೆ = pitch_Setpoint_At_ವೇಗ(20.f, 30.f, 0.f);

	// THEN the pitch loop holds altitude and does not pitch down
	EXPECT_NEAR(pitch_nominal_ತೂಕ, 0.f, 1e-4f);
	EXPECT_NEAR(pitch_altitude_ಆದ್ಯತೆ, 0.f, 1e-4f);
}

// Below trim airspeed, pitch still trades altitude for speed, but only up to trim.
TEST(TECSControlTest, Pitch_ಕೆಳಕ್ಕೆ_ಗಮ್ಯ_Accelerate_ಕೆಳಗೆ_Trim)
{
	// WHEN flying below trim airspeed with the setpoint at trim, and with the setpoint above trim
	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ pitch_setpoint_at_trim = pitch_Setpoint_At_ವೇಗ(12.f, 15.f, 1.f);
	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ pitch_setpoint_above_trim = pitch_Setpoint_At_ವೇಗ(12.f, 30.f, 1.f);

	// THEN the pitch loop pitches down, but only as much as needed to reach trim airspeed
	EXPECT_LT(pitch_setpoint_at_trim, 0.f);
	EXPECT_NEAR(pitch_setpoint_above_trim, pitch_setpoint_at_trim, 1e-4f);
}

// With full speed priority (e.g. gliders), pitch is the only way to track the airspeed setpoint.
TEST(TECSControlTest, Pitch_ಕೆಳಕ್ಕೆ_ಗಮ್ಯ_Accelerate_Above_Trim_With_ವೇಗ_ಆದ್ಯತೆ)
{
	// WHEN flying above trim airspeed with a higher airspeed setpoint and full speed priority
	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ pitch = pitch_Setpoint_At_ವೇಗ(20.f, 30.f, 2.f);

	// THEN the pitch loop pitches down to accelerate
	EXPECT_LT(pitch, 0.f);
}

// Between trim airspeed and the setpoint the throttle alone accelerates the vehicle: the pitch loop must not react to
// that acceleration, otherwise it pitches up and turns the throttle's energy into height instead of speed.
TEST(TECSControlTest, No_Pitch_ಮೇಲೆ_While_Throttle_Accelerates_Above_Trim)
{
	// WHEN accelerating between trim airspeed (15 m/s) and the airspeed setpoint, at nominal and altitude priority
	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ pitch_accelerating = pitch_Setpoint_At_ವೇಗ(18.f, 25.f, 1.f, 1.f);
	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ pitch_steady = pitch_Setpoint_At_ವೇಗ(18.f, 25.f, 1.f, 0.f);

	// THEN the pitch setpoint is the same as without acceleration
	EXPECT_NEAR(pitch_accelerating, pitch_steady, 1e-5f);
}

// Above the airspeed setpoint pitch takes back speed control: an acceleration there is countered with pitch up.
TEST(TECSControlTest, Pitch_ಮೇಲೆ_On_Acceleration_Above_Setpoint)
{
	// WHEN accelerating above the airspeed setpoint
	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ pitch_accelerating = pitch_Setpoint_At_ವೇಗ(22.f, 20.f, 1.f, 1.f);
	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ pitch_steady = pitch_Setpoint_At_ವೇಗ(22.f, 20.f, 1.f, 0.f);

	// THEN the pitch loop pitches up more than without acceleration
	EXPECT_GT(pitch_accelerating, pitch_steady + 1e-3f);
}

// An overspeed above the airspeed setpoint is still corrected with pitch.
TEST(TECSControlTest, Pitch_ಮೇಲೆ_On_Overspeed)
{
	// WHEN flying faster than the airspeed setpoint
	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ pitch = pitch_Setpoint_At_ವೇಗ(25.f, 20.f, 1.f);

	// THEN the pitch loop pitches up to slow down
	EXPECT_GT(pitch, 0.f);
}

// Between trim airspeed and the setpoint, a speed weight between 1 and 2 must fade the pitch loop's kinetic setpoint
// and estimate equally: accelerating exactly as commanded while holding altitude leaves no energy balance error.
TEST(TECSControlTest, KineticSetpointAndEstimateFadeEquallyAboveTrim)
{
	TECSControl::Param param = makeParam();
	param.pitch_ವೇಗ_ತೂಕ = 1.5f;
	TECSControl::Flag flag = makeFlag();
	flag.detect_underspeed_enabled = ಸುಳ್ಳು;

	TECSControl::Setpoint setpoint = makeSetpoint(); // altitude reference at the current altitude
	setpoint.tas_setpoint = 18.f;

	TECSControl::ಒಳಹರಿವು_2 ಒಳಹರಿವು = make_ಒಳಹರಿವು();
	ಒಳಹರಿವು.tas = 17.f; // between trim (15 m/s) and the setpoint
	ಒಳಹರಿವು.tas_ದರ = (setpoint.tas_setpoint - ಒಳಹರಿವು.tas) * param.airspeed_error_gain; // the commanded acceleration

	TECSControl control;
	control.initialize(setpoint, ಒಳಹರಿವು, param, flag);

	ಸ್ಥಿರಾಂಕ TECSControl::DebugOutput &debug = control.getDebugOutput();
	EXPECT_NEAR(debug.energy_balance_ದರ_sp, debug.energy_balance_ದರ_estimate, 1e-4f);
}

// The stall airspeed is converted to TAS the same way as the minimum airspeed.
TEST(TECSUnderspeedTest, Stall_Airspeed_Is_Converted_ಗಮ್ಯ_True_Airspeed)
{
	ಗಾಗಿ (ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ eas_ಗಮ್ಯ_tas : {1.f, 1.5f}) {
		TECS tecs;
		configureTECS(tecs);
		tecs.ಗಣ_equivalent_airspeed_stall(9.f); // band is [9.5, 10] m/s EAS

		ಸತತ_TECS(tecs, 100.f, 100.f, 9.75f, 3, eas_ಗಮ್ಯ_tas);

		EXPECT_NEAR(tecs.get_underspeed_ಅನುಪಾತ(), 0.5f, 1e-5f);
	}
}

// While the aircraft is undersped the altitude reference is pinned to the current
// altitude, so no altitude error accumulates that would command an aggressive pitch-up
// (and a possible secondary stall) once the speed recovers.
TEST(TECSUnderspeedTest, AltitudeReferenceIsHeldAtCurrentAltitudeWhileUndersped)
{
	TECS tecs;
	configureTECS(tecs);

	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ altitude = 100.f;
	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ hgt_setpoint = 200.f; // large standing climb demand

	ಸತತ_TECS(tecs, altitude, hgt_setpoint, 8.f /* well below the 10 m/s minimum */, 25);

	ASSERT_GT(tecs.get_underspeed_ಅನುಪಾತ(), 0.5f);
	EXPECT_NEAR(tecs.get_ಸ್ಥಿತಿ().altitude_reference, altitude, 1e-3f);
	EXPECT_NEAR(tecs.get_ಸ್ಥಿತಿ().ಎತ್ತರ_ದರ_reference, 0.f, 1e-3f);
}

// Sanity counterpart: at a healthy airspeed the same climb demand does move the reference.
TEST(TECSUnderspeedTest, AltitudeReferenceTracksSetpointWhenNotUndersped)
{
	TECS tecs;
	configureTECS(tecs);

	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ altitude = 100.f;
	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ hgt_setpoint = 200.f;

	ಸತತ_TECS(tecs, altitude, hgt_setpoint, 20.f, 25);

	ASSERT_FLOAT_EQ(tecs.get_underspeed_ಅನುಪಾತ(), 0.f);
	EXPECT_GT(tecs.get_ಸ್ಥಿತಿ().altitude_reference, altitude);
}

/****************************************************************************
 *
 *   Copyright (C) 2022 PX4 Development Team. All rights reserved.
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
#అన్తర్భావ <PID.hpp>

// Run all PID unit tests:
//   make tests TESTFILTER=unit-PID
// Run a subset directly (after building) by filtering gtest cases, e.g.:
//   ./build/px4_sitl_test/unit-PID --gtest_filter=PIDTest.AntiWindup*

TEST(PIDTest, AllZeroCase)
{
	PID pid;
	EXPECT_FLOAT_EQ(pid.update(0.f, 0.f, అబద్ధం), 0.f);
}

TEST(PIDTest, Output_హద్దు)
{
	PID pid;
	pid.సమితి_Output_హద్దు(.01f);
	pid.సమితి_Gains(.1f, 0.f, 0.f);
	pid.సమితి_Setpoint(1.f);
	EXPECT_FLOAT_EQ(pid.update(0.f, 0.f, అబద్ధం), .01f);
	EXPECT_FLOAT_EQ(pid.update(.9f, 0.f, అబద్ధం), .01f);
	EXPECT_NEAR(pid.update(.95f, 0.f, అబద్ధం), .005f, 1e-6f);
	EXPECT_FLOAT_EQ(pid.update(1.f, 0.f, అబద్ధం), 0.f);
	EXPECT_NEAR(pid.update(1.05f, 0.f, అబద్ధం), -.005f, 1e-6f);
	EXPECT_FLOAT_EQ(pid.update(1.1f, 0.f, అబద్ధం), -.01f);
	EXPECT_FLOAT_EQ(pid.update(1.15f, 0.f, అబద్ధం), -.01f);
	EXPECT_FLOAT_EQ(pid.update(2.f, 0.f, అబద్ధం), -.01f);
}

TEST(PIDTest, ProportinalOnly)
{
	PID pid;
	pid.సమితి_Output_హద్దు(1.f);
	pid.సమితి_Gains(.1f, 0.f, 0.f);
	EXPECT_FLOAT_EQ(pid.update(0.f, 0.f, అబద్ధం), 0.f);
	pid.సమితి_Setpoint(1.f);
	EXPECT_FLOAT_EQ(pid.update(0.f, 0.f, అబద్ధం), .1f);
	EXPECT_FLOAT_EQ(pid.update(1.f, 0.f, అబద్ధం), 0.f);

	దశాంశం plant = 0.f;
	దశాంశం output = 10000.f;
	పూర్ణాంకం i; // need function scope to check how many steps

	కొరకు (i = 1000; i > 0; i--) {
		స్థిరరాశి దశాంశం output_new = pid.update(plant, 0.f, అబద్ధం);
		plant += output_new;

		// expect the output to get smaller with each iteration
		ఒకవేళ (output_new >= output) {
			ఆపు;
		}

		output = output_new;
	}

	EXPECT_FLOAT_EQ(plant, 1.f);
	EXPECT_GT(i, 0); // it shouldn't have taken longer than an iteration timeout to converge
}

TEST(PIDTest, InteralOpenLoop)
{
	PID pid;
	pid.సమితి_Output_హద్దు(1.f);
	pid.సమితి_Gains(0.f, .1f, 0.f);
	pid.సమితి_Integral_హద్దు(.05f);
	pid.సమితి_Setpoint(1.f);

	// Zero error
	EXPECT_FLOAT_EQ(pid.update(1.f, 0.f, నిజం), 0.f);
	EXPECT_FLOAT_EQ(pid.update(1.f, 0.f, నిజం), 0.f);
	EXPECT_FLOAT_EQ(pid.update(1.f, 0.f, నిజం), 0.f);

	// Open loop ramp up
	EXPECT_FLOAT_EQ(pid.update(0.f, 0.1f, నిజం), 0.f);
	EXPECT_FLOAT_EQ(pid.update(0.f, 0.1f, నిజం), .01f);
	EXPECT_FLOAT_EQ(pid.update(0.f, 0.1f, నిజం), .02f);
	EXPECT_FLOAT_EQ(pid.update(0.f, 0.1f, నిజం), .03f);
	EXPECT_FLOAT_EQ(pid.update(0.f, 0.1f, నిజం), .04f);
	EXPECT_FLOAT_EQ(pid.update(0.f, 0.1f, నిజం), .05f);
	EXPECT_FLOAT_EQ(pid.update(0.f, 0.1f, నిజం), .05f);
	EXPECT_FLOAT_EQ(pid.update(0.f, 0.1f, నిజం), .05f);

	// Open loop ramp down
	pid.సమితి_Setpoint(-1.f);
	EXPECT_FLOAT_EQ(pid.update(0.f, 0.1f, నిజం), .05f);
	EXPECT_FLOAT_EQ(pid.update(0.f, 0.1f, నిజం), .04f);
	EXPECT_FLOAT_EQ(pid.update(0.f, 0.1f, నిజం), .03f);
	EXPECT_FLOAT_EQ(pid.update(0.f, 0.1f, నిజం), .02f);
	EXPECT_NEAR(pid.update(0.f, 0.1f, నిజం), .01f, 1e-6f);
	EXPECT_NEAR(pid.update(0.f, 0.1f, నిజం), 0.f, 1e-6f);
	EXPECT_NEAR(pid.update(0.f, 0.1f, నిజం), -.01f, 1e-6f);
	EXPECT_FLOAT_EQ(pid.update(0.f, 0.1f, నిజం), -.02f);
	EXPECT_NEAR(pid.update(0.f, 0.1f, నిజం), -.03f, 1e-6f);
	EXPECT_FLOAT_EQ(pid.update(0.f, 0.1f, నిజం), -.04f);
	EXPECT_FLOAT_EQ(pid.update(0.f, 0.1f, నిజం), -.05f);
	EXPECT_FLOAT_EQ(pid.update(0.f, 0.1f, నిజం), -.05f);
	EXPECT_FLOAT_EQ(pid.update(0.f, 0.1f, నిజం), -.05f);
	pid.resetIntegral();
	EXPECT_FLOAT_EQ(pid.update(0.f, 0.1f, నిజం), 0.f);
	EXPECT_FLOAT_EQ(pid.update(0.f, 0.1f, నిజం), -.01f);
}

TEST(PIDTest, Anti_Windup_ఎగువ_Saturation)
{
	// Integrator-only controller whose output saturates before the integral limit.
	PID pid;
	pid.సమితి_Gains(0.f, .1f, 0.f);
	pid.సమితి_Integral_హద్దు(1.f); // large, so the output limit saturates first
	pid.సమితి_Output_హద్దు(.05f);
	pid.సమితి_Setpoint(1.f);

	// Output uses the pre-update integral, so the first step is not yet saturated.
	EXPECT_FLOAT_EQ(pid.update(0.f, 1.f, నిజం), 0.f);
	EXPECT_FLOAT_EQ(pid.getIntegral(), .1f);

	// Output now saturates high; integrating further up would wind up -> frozen.
	EXPECT_FLOAT_EQ(pid.update(0.f, 1.f, నిజం), .05f);
	EXPECT_FLOAT_EQ(pid.getIntegral(), .1f);
	EXPECT_FLOAT_EQ(pid.update(0.f, 1.f, నిజం), .05f);
	EXPECT_FLOAT_EQ(pid.getIntegral(), .1f);

	// Error reverses: still saturated high, but integrating down unwinds -> allowed.
	pid.సమితి_Setpoint(-1.f);
	EXPECT_FLOAT_EQ(pid.update(0.f, 1.f, నిజం), .05f);
	EXPECT_FLOAT_EQ(pid.getIntegral(), 0.f);
}

TEST(PIDTest, Anti_Windup_దిగువ_Saturation)
{
	PID pid;
	pid.సమితి_Gains(0.f, .1f, 0.f);
	pid.సమితి_Integral_హద్దు(1.f);
	pid.సమితి_Output_హద్దు(.05f);
	pid.సమితి_Setpoint(-1.f);

	EXPECT_FLOAT_EQ(pid.update(0.f, 1.f, నిజం), 0.f);
	EXPECT_FLOAT_EQ(pid.getIntegral(), -.1f);

	// Output saturates low; integrating further down would wind up -> frozen.
	EXPECT_FLOAT_EQ(pid.update(0.f, 1.f, నిజం), -.05f);
	EXPECT_FLOAT_EQ(pid.getIntegral(), -.1f);

	// Error reverses: still saturated low, but integrating up unwinds -> allowed.
	pid.సమితి_Setpoint(1.f);
	EXPECT_FLOAT_EQ(pid.update(0.f, 1.f, నిజం), -.05f);
	EXPECT_FLOAT_EQ(pid.getIntegral(), 0.f);
}

TEST(PIDTest, DerivativeOnlyDampsFeedbackMotion)
{
	// Derivative-on-measurement: the D term must oppose feedback motion so the
	// output damps the plant rather than reinforcing its movement.
	PID pid;
	pid.సమితి_Output_హద్దు(100.f);
	pid.సమితి_Gains(0.f, 0.f, 1.f);
	pid.సమితి_Setpoint(0.f);

	// First sample seeds _last_feedback; derivative contribution is zero.
	EXPECT_FLOAT_EQ(pid.update(0.f, 0.1f, అబద్ధం), 0.f);

	// Feedback rises by 1.0 over dt = 0.1 -> d(feedback)/dt = +10.
	// Expected output = -Kd * d(feedback)/dt = -10.
	EXPECT_FLOAT_EQ(pid.update(1.f, 0.1f, అబద్ధం), -10.f);

	// Feedback falls by 1.0 over dt = 0.1 -> d(feedback)/dt = -10.
	// Expected output = +10 (still damping).
	EXPECT_FLOAT_EQ(pid.update(0.f, 0.1f, అబద్ధం), 10.f);
}

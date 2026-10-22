/****************************************************************************
 *
 *   Copyright (c) 2019 ECL Development Team. All rights reserved.
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
 * @file AlphaFilterTest.cpp
 *
 * @brief Unit tests for the alpha filter class
 */

#ಅನ್ತರ್ಭಾವ <gtest/gtest.h>
#ಅನ್ತರ್ಭಾವ <cmath>
#ಅನ್ತರ್ಭಾವ <matrix/math.hpp>

#ಅನ್ತರ್ಭಾವ <lib/mathlib/math/filter/AlphaFilter.hpp>

using matrix::Vector3f;
using matrix::Quatf;
using matrix::Eulerf;

TEST(AlphaFilterTest, initializeToZero)
{
	AlphaFilter<ಭಿನ್ನ> filter_float{};
	ASSERT_EQ(filter_float.getState(), 0.f);
}

TEST(AlphaFilterTest, reset_To_ಮೌಲ್ಯ)
{
	AlphaFilter<ಭಿನ್ನ> filter_float{};
	ಧ್ರುವ ಭಿನ್ನ reset_ಮೌಲ್ಯ = 42.42f;
	filter_float.reset(reset_ಮೌಲ್ಯ);
	ASSERT_EQ(filter_float.getState(), reset_ಮೌಲ್ಯ);
}

TEST(AlphaFilterTest, runZero)
{
	AlphaFilter<ಭಿನ್ನ> filter_float{};
	ಧ್ರುವ ಭಿನ್ನ ಒಳಹರಿವು = 0.f;

	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < 10; i++) {
		filter_float.update(ಒಳಹರಿವು);
	}

	ASSERT_EQ(filter_float.getState(), ಒಳಹರಿವು);
}

TEST(AlphaFilterTest, runPositive)
{
	// GIVEN an input of 1 in a filter with a default time constant of 9 (alpha = 0.1)
	AlphaFilter<ಭಿನ್ನ> filter_float{};
	ಧ್ರುವ ಭಿನ್ನ ಒಳಹರಿವು = 1.f;
	filter_float.setAlpha(.1f);

	// WHEN we run the filter 9 times
	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < 9; i++) {
		filter_float.update(ಒಳಹರಿವು);
	}

	// THEN the state of the filter should have reached 63%
	ASSERT_NEAR(filter_float.getState(), 0.63f, 0.02);
}

TEST(AlphaFilterTest, runNegative)
{
	// GIVEN an input of 1 in a filter with a default time constant of 9 (alpha = 0.1)
	AlphaFilter<ಭಿನ್ನ> filter_float{};
	ಧ್ರುವ ಭಿನ್ನ ಒಳಹರಿವು = -1.f;
	filter_float.setAlpha(.1f);

	// WHEN we run the filter 9 times
	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < 9; i++) {
		filter_float.update(ಒಳಹರಿವು);
	}

	// THEN the state of the filter should have reached 63%
	ASSERT_NEAR(filter_float.getState(), -0.63f, 0.02);
}

TEST(AlphaFilterTest, riseTime)
{
	// GIVEN an input of 1 in a filter with a default time constant of 9 (alpha = 0.1)
	AlphaFilter<ಭಿನ್ನ> filter_float{};
	ಧ್ರುವ ಭಿನ್ನ ಒಳಹರಿವು = 1.f;
	filter_float.setAlpha(.1f);

	// WHEN we run the filter 27 times (3 * time constant)
	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < 3 * 9; i++) {
		filter_float.update(ಒಳಹರಿವು);
	}

	// THEN the state of the filter should have reached 95%
	ASSERT_NEAR(filter_float.getState(), 0.95f, 0.02f);
}

TEST(AlphaFilterTest, convergence)
{
	// GIVEN an input of 1 in a filter with a default time constant of 9 (alpha = 0.1)
	AlphaFilter<ಭಿನ್ನ> filter_float{};
	ಧ್ರುವ ಭಿನ್ನ ಒಳಹರಿವು = 1.f;
	filter_float.setAlpha(.1f);

	// WHEN we run the filter 45 times (5 * time constant)
	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < 5 * 9; i++) {
		filter_float.update(ಒಳಹರಿವು);
	}

	// THEN the state of the filter should have converged to the input
	ASSERT_NEAR(filter_float.getState(), 1.f, 0.01f);
}

TEST(AlphaFilterTest, convergenceVector3f)
{
	// GIVEN an Vector3f input in a filter with a default time constant of 9 (alpha = 0.1)
	AlphaFilter<Vector3f> filter_v3{};
	ಧ್ರುವ Vector3f ಒಳಹರಿವು = {3.f, 7.f, -11.f};
	filter_v3.setAlpha(.1f);

	// WHEN we run the filter 45 times (5 * time constant)
	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < 5 * 9; i++) {
		filter_v3.update(ಒಳಹರಿವು);
	}

	// THEN the state of the filter should have converged to the input (1% error allowed)
	Vector3f output = filter_v3.getState();

	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < 3; i++) {
		ASSERT_NEAR(output(i), ಒಳಹರಿವು(i), fabsf(0.01f * ಒಳಹರಿವು(i)));
	}
}

TEST(AlphaFilterTest, convergenceVector3fAlpha)
{
	// GIVEN a Vector3f input in a filter with a defined time constant and the default sampling time
	AlphaFilter<Vector3f> filter_v3{};
	ಧ್ರುವ Vector3f ಒಳಹರಿವು = {3.f, 7.f, -11.f};
	ಧ್ರುವ ಭಿನ್ನ tau = 18.f;
	ಧ್ರುವ ಭಿನ್ನ dt = 1.f;
	filter_v3.setParameters(dt, tau);

	// WHEN we run the filter 18 times (1 * time constant)
	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < 18; i++) {
		filter_v3.update(ಒಳಹರಿವು); // dt is assumed equal to 1
	}

	// THEN the state of the filter should have reached 65% (2% error allowed)
	Vector3f output = filter_v3.getState();

	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < 3; i++) {
		ASSERT_NEAR(output(i), 0.63f * ಒಳಹರಿವು(i), fabsf(0.02f * ಒಳಹರಿವು(i)));
	}
}

TEST(AlphaFilterTest, convergenceVector3fTauDt)
{
	// GIVEN a Vector3f input in a filter with a defined time constant and sampling time
	AlphaFilter<Vector3f> filter_v3{};
	ಧ್ರುವ Vector3f ಒಳಹರಿವು = {51.f, 7.f, -11.f};
	ಧ್ರುವ ಭಿನ್ನ tau = 2.f;
	ಧ್ರುವ ಭಿನ್ನ dt = 0.1f;
	filter_v3.setParameters(dt, tau);

	// WHEN we run the filter (1 * time constant)
	ಧ್ರುವ ಭಿನ್ನ n = tau / dt;

	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < n; i++) {
		filter_v3.update(ಒಳಹರಿವು);
	}

	// THEN the state of the filter should have reached 65% (2% error allowed)
	Vector3f output = filter_v3.getState();

	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < 3; i++) {
		ASSERT_NEAR(output(i), 0.63f * ಒಳಹರಿವು(i), fabsf(0.02f * ಒಳಹರಿವು(i)));
	}

	// ALSO when the filter is reset to a specified value
	ಧ್ರುವ Vector3f reset_vector = {-1.f, 71.f, -42.f};
	filter_v3.reset(reset_vector);
	output = filter_v3.getState();

	// THEN the filter should exactly contain those values
	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < 3; i++) {
		ASSERT_EQ(output(i), reset_vector(i));
	}
}

TEST(AlphaFilterTest, AllZeroTest)
{
	AlphaFilter<ಭಿನ್ನ> _alpha_filter;
	_alpha_filter.update(0.f);
	EXPECT_FLOAT_EQ(_alpha_filter.getState(), 0.f);
}

TEST(AlphaFilterTest, AlphaZeroTest)
{
	AlphaFilter<ಭಿನ್ನ> _alpha_filter;
	_alpha_filter.setParameters(1e-5f, 1e5f);

	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < 100; i++) {
		_alpha_filter.update(1.f);
		EXPECT_NEAR(_alpha_filter.getState(), 0.f, 1e-4f);
	}
}

TEST(AlphaFilterTest, AlphaOneTest)
{
	AlphaFilter<ಭಿನ್ನ> _alpha_filter;
	_alpha_filter.setParameters(.1f, 0.f);

	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < 100; i++) {
		ಧ್ರುವ ಭಿನ್ನ new_smaple = static_cast<ಭಿನ್ನ>(i);
		_alpha_filter.update(new_smaple);
		EXPECT_FLOAT_EQ(_alpha_filter.getState(), new_smaple);
	}
}

TEST(AlphaFilterTest, SetFrequencyTest)
{
	AlphaFilter<ಭಿನ್ನ> _alpha_filter;
	ಧ್ರುವ ಭಿನ್ನ fs = .1f;

	EXPECT_FALSE(_alpha_filter.setCutoffFreq(fs, 0.f));
	EXPECT_FALSE(_alpha_filter.setCutoffFreq(fs, fs)); // Cutoff above Nyquist freq
	EXPECT_FALSE(_alpha_filter.setCutoffFreq(0.f, fs / 4.f));
	EXPECT_FALSE(_alpha_filter.setCutoffFreq(0.f, 0.f));
	EXPECT_FALSE(_alpha_filter.setCutoffFreq(-fs, fs / 4.f));
	EXPECT_FALSE(_alpha_filter.setCutoffFreq(-fs, -fs / 4.f));
	EXPECT_FALSE(_alpha_filter.setCutoffFreq(fs, -fs / 4.f));

	EXPECT_TRUE(_alpha_filter.setCutoffFreq(fs, fs / 4.f));
}

TEST(AlphaFilterTest, ConvergenceTest)
{
	AlphaFilter<ಭಿನ್ನ> _alpha_filter;
	_alpha_filter.setParameters(.1f, 1.f);

	ಭಿನ್ನ last_ಮೌಲ್ಯ{0.f};

	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < 100; i++) {
		_alpha_filter.update(1.f);
		EXPECT_GE(_alpha_filter.getState(), last_ಮೌಲ್ಯ);
		last_ಮೌಲ್ಯ = _alpha_filter.getState();
	}

	EXPECT_NEAR(last_ಮೌಲ್ಯ, 1.f, 1e-4f);

	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < 1000; i++) {
		_alpha_filter.update(-100.f);
		EXPECT_LE(_alpha_filter.getState(), last_ಮೌಲ್ಯ);
		last_ಮೌಲ್ಯ = _alpha_filter.getState();
	}

	EXPECT_NEAR(last_ಮೌಲ್ಯ, -100.f, 1e-4f);
}

TEST(AlphaFilterTest, runPositiveRot3)
{
	// GIVEN a filter with a default time constant of 9 (alpha = 0.1)
	ಧ್ರುವ Quatf q_init(Eulerf(0.f, 0.f, -0.01f));
	ಧ್ರುವ Quatf ಒಳಹರಿವು(Eulerf(0.f, 0.f, M_PI_F / 2.f));

	AlphaFilter<Quatf> filter{};
	filter.setAlpha(.1f);
	filter.reset(q_init);

	// WHEN we run the filter 9 times
	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < 9; i++) {
		filter.update(ಒಳಹರಿವು);
	}

	// THEN the state of the filter should have reached 63%
	EXPECT_NEAR(Eulerf(filter.getState()).psi(), 0.63f * (M_PI_F / 2.f - 0.01f), 0.03);

	EXPECT_FLOAT_EQ(Eulerf(filter.getState()).phi(), 0.f);
	EXPECT_FLOAT_EQ(Eulerf(filter.getState()).theta(), 0.f);
}

TEST(AlphaFilterTest, runNecgativeRot3)
{
	// GIVEN a filter with a default time constant of 9 (alpha = 0.1)
	ಧ್ರುವ ಭಿನ್ನ roll = -M_PI_F;
	ಧ್ರುವ ಭಿನ್ನ pitch = 0.f;
	ಧ್ರುವ ಭಿನ್ನ yaw_init = 0.01f;
	ಧ್ರುವ ಭಿನ್ನ yaw_ಒಳಹರಿವು = -M_PI_F / 2.f;
	ಧ್ರುವ Quatf q_init(Eulerf(roll, pitch, yaw_init));
	ಧ್ರುವ Quatf ಒಳಹರಿವು(Eulerf(roll, pitch, yaw_ಒಳಹರಿವು));

	AlphaFilter<Quatf> filter{};
	filter.setAlpha(.1f);
	filter.reset(q_init);

	// WHEN we run the filter 9 times
	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < 9; i++) {
		filter.update(ಒಳಹರಿವು);
	}

	// THEN the state of the filter should have reached 63%
	EXPECT_NEAR(Eulerf(filter.getState()).psi(), 0.63f * (yaw_ಒಳಹರಿವು + yaw_init), 0.03);

	EXPECT_TRUE(matrix::wrap_pi(Eulerf(filter.getState()).phi() - roll) < FLT_EPSILON);
	EXPECT_TRUE(matrix::wrap_pi(Eulerf(filter.getState()).theta() - pitch) < FLT_EPSILON);
}

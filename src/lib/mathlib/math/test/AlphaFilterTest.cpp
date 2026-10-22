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

#ਅਨ੍ਤਰ੍ਭਾਵ <gtest/gtest.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <cmath>
#ਅਨ੍ਤਰ੍ਭਾਵ <matrix/math.hpp>

#ਅਨ੍ਤਰ੍ਭਾਵ <lib/mathlib/math/filter/AlphaFilter.hpp>

ਵਰਤੋਂ ਮੈਟ੍ਰਿਕਸ::Vector3f;
ਵਰਤੋਂ ਮੈਟ੍ਰਿਕਸ::Quatf;
ਵਰਤੋਂ ਮੈਟ੍ਰਿਕਸ::Eulerf;

TEST(AlphaFilterTest, initialize_ਮੰਜ਼ਿਲ_Zero)
{
	AlphaFilter<ਦਸ਼ਮਲਵ> filter_float{};
	ASSERT_EQ(filter_float.getState(), 0.f);
}

TEST(AlphaFilterTest, reset_ਮੰਜ਼ਿਲ_ਮੁੱਲ)
{
	AlphaFilter<ਦਸ਼ਮਲਵ> filter_float{};
	ਅਚਲ ਦਸ਼ਮਲਵ reset_ਮੁੱਲ = 42.42f;
	filter_float.reset(reset_ਮੁੱਲ);
	ASSERT_EQ(filter_float.getState(), reset_ਮੁੱਲ);
}

TEST(AlphaFilterTest, ਲੜੀਵਾਰ_Zero)
{
	AlphaFilter<ਦਸ਼ਮਲਵ> filter_float{};
	ਅਚਲ ਦਸ਼ਮਲਵ ਨਿਵੇਸ਼ = 0.f;

	ਲਈ (ਪੂਰਨ i = 0; i < 10; i++) {
		filter_float.update(ਨਿਵੇਸ਼);
	}

	ASSERT_EQ(filter_float.getState(), ਨਿਵੇਸ਼);
}

TEST(AlphaFilterTest, ਲੜੀਵਾਰ_ਸਕਾਰਾਤਮਕ)
{
	// GIVEN an input of 1 in a filter with a default time constant of 9 (alpha = 0.1)
	AlphaFilter<ਦਸ਼ਮਲਵ> filter_float{};
	ਅਚਲ ਦਸ਼ਮਲਵ ਨਿਵੇਸ਼ = 1.f;
	filter_float.ਸੈੱਟ_Alpha(.1f);

	// WHEN we run the filter 9 times
	ਲਈ (ਪੂਰਨ i = 0; i < 9; i++) {
		filter_float.update(ਨਿਵੇਸ਼);
	}

	// THEN the state of the filter should have reached 63%
	ASSERT_NEAR(filter_float.getState(), 0.63f, 0.02);
}

TEST(AlphaFilterTest, ਲੜੀਵਾਰ_Negative)
{
	// GIVEN an input of 1 in a filter with a default time constant of 9 (alpha = 0.1)
	AlphaFilter<ਦਸ਼ਮਲਵ> filter_float{};
	ਅਚਲ ਦਸ਼ਮਲਵ ਨਿਵੇਸ਼ = -1.f;
	filter_float.ਸੈੱਟ_Alpha(.1f);

	// WHEN we run the filter 9 times
	ਲਈ (ਪੂਰਨ i = 0; i < 9; i++) {
		filter_float.update(ਨਿਵੇਸ਼);
	}

	// THEN the state of the filter should have reached 63%
	ASSERT_NEAR(filter_float.getState(), -0.63f, 0.02);
}

TEST(AlphaFilterTest, rise_ਸਮਾਂ)
{
	// GIVEN an input of 1 in a filter with a default time constant of 9 (alpha = 0.1)
	AlphaFilter<ਦਸ਼ਮਲਵ> filter_float{};
	ਅਚਲ ਦਸ਼ਮਲਵ ਨਿਵੇਸ਼ = 1.f;
	filter_float.ਸੈੱਟ_Alpha(.1f);

	// WHEN we run the filter 27 times (3 * time constant)
	ਲਈ (ਪੂਰਨ i = 0; i < 3 * 9; i++) {
		filter_float.update(ਨਿਵੇਸ਼);
	}

	// THEN the state of the filter should have reached 95%
	ASSERT_NEAR(filter_float.getState(), 0.95f, 0.02f);
}

TEST(AlphaFilterTest, convergence)
{
	// GIVEN an input of 1 in a filter with a default time constant of 9 (alpha = 0.1)
	AlphaFilter<ਦਸ਼ਮਲਵ> filter_float{};
	ਅਚਲ ਦਸ਼ਮਲਵ ਨਿਵੇਸ਼ = 1.f;
	filter_float.ਸੈੱਟ_Alpha(.1f);

	// WHEN we run the filter 45 times (5 * time constant)
	ਲਈ (ਪੂਰਨ i = 0; i < 5 * 9; i++) {
		filter_float.update(ਨਿਵੇਸ਼);
	}

	// THEN the state of the filter should have converged to the input
	ASSERT_NEAR(filter_float.getState(), 1.f, 0.01f);
}

TEST(AlphaFilterTest, convergenceVector3f)
{
	// GIVEN an Vector3f input in a filter with a default time constant of 9 (alpha = 0.1)
	AlphaFilter<Vector3f> filter_v3{};
	ਅਚਲ Vector3f ਨਿਵੇਸ਼ = {3.f, 7.f, -11.f};
	filter_v3.ਸੈੱਟ_Alpha(.1f);

	// WHEN we run the filter 45 times (5 * time constant)
	ਲਈ (ਪੂਰਨ i = 0; i < 5 * 9; i++) {
		filter_v3.update(ਨਿਵੇਸ਼);
	}

	// THEN the state of the filter should have converged to the input (1% error allowed)
	Vector3f output = filter_v3.getState();

	ਲਈ (ਪੂਰਨ i = 0; i < 3; i++) {
		ASSERT_NEAR(output(i), ਨਿਵੇਸ਼(i), fabsf(0.01f * ਨਿਵੇਸ਼(i)));
	}
}

TEST(AlphaFilterTest, convergenceVector3fAlpha)
{
	// GIVEN a Vector3f input in a filter with a defined time constant and the default sampling time
	AlphaFilter<Vector3f> filter_v3{};
	ਅਚਲ Vector3f ਨਿਵੇਸ਼ = {3.f, 7.f, -11.f};
	ਅਚਲ ਦਸ਼ਮਲਵ tau = 18.f;
	ਅਚਲ ਦਸ਼ਮਲਵ dt = 1.f;
	filter_v3.ਸੈੱਟ_Parameters(dt, tau);

	// WHEN we run the filter 18 times (1 * time constant)
	ਲਈ (ਪੂਰਨ i = 0; i < 18; i++) {
		filter_v3.update(ਨਿਵੇਸ਼); // dt is assumed equal to 1
	}

	// THEN the state of the filter should have reached 65% (2% error allowed)
	Vector3f output = filter_v3.getState();

	ਲਈ (ਪੂਰਨ i = 0; i < 3; i++) {
		ASSERT_NEAR(output(i), 0.63f * ਨਿਵੇਸ਼(i), fabsf(0.02f * ਨਿਵੇਸ਼(i)));
	}
}

TEST(AlphaFilterTest, convergenceVector3fTauDt)
{
	// GIVEN a Vector3f input in a filter with a defined time constant and sampling time
	AlphaFilter<Vector3f> filter_v3{};
	ਅਚਲ Vector3f ਨਿਵੇਸ਼ = {51.f, 7.f, -11.f};
	ਅਚਲ ਦਸ਼ਮਲਵ tau = 2.f;
	ਅਚਲ ਦਸ਼ਮਲਵ dt = 0.1f;
	filter_v3.ਸੈੱਟ_Parameters(dt, tau);

	// WHEN we run the filter (1 * time constant)
	ਅਚਲ ਦਸ਼ਮਲਵ n = tau / dt;

	ਲਈ (ਪੂਰਨ i = 0; i < n; i++) {
		filter_v3.update(ਨਿਵੇਸ਼);
	}

	// THEN the state of the filter should have reached 65% (2% error allowed)
	Vector3f output = filter_v3.getState();

	ਲਈ (ਪੂਰਨ i = 0; i < 3; i++) {
		ASSERT_NEAR(output(i), 0.63f * ਨਿਵੇਸ਼(i), fabsf(0.02f * ਨਿਵੇਸ਼(i)));
	}

	// ALSO when the filter is reset to a specified value
	ਅਚਲ Vector3f reset_ਸਦਿਸ਼ = {-1.f, 71.f, -42.f};
	filter_v3.reset(reset_ਸਦਿਸ਼);
	output = filter_v3.getState();

	// THEN the filter should exactly contain those values
	ਲਈ (ਪੂਰਨ i = 0; i < 3; i++) {
		ASSERT_EQ(output(i), reset_ਸਦਿਸ਼(i));
	}
}

TEST(AlphaFilterTest, AllZeroTest)
{
	AlphaFilter<ਦਸ਼ਮਲਵ> _alpha_filter;
	_alpha_filter.update(0.f);
	EXPECT_FLOAT_EQ(_alpha_filter.getState(), 0.f);
}

TEST(AlphaFilterTest, AlphaZeroTest)
{
	AlphaFilter<ਦਸ਼ਮਲਵ> _alpha_filter;
	_alpha_filter.ਸੈੱਟ_Parameters(1e-5f, 1e5f);

	ਲਈ (ਪੂਰਨ i = 0; i < 100; i++) {
		_alpha_filter.update(1.f);
		EXPECT_NEAR(_alpha_filter.getState(), 0.f, 1e-4f);
	}
}

TEST(AlphaFilterTest, AlphaOneTest)
{
	AlphaFilter<ਦਸ਼ਮਲਵ> _alpha_filter;
	_alpha_filter.ਸੈੱਟ_Parameters(.1f, 0.f);

	ਲਈ (ਪੂਰਨ i = 0; i < 100; i++) {
		ਅਚਲ ਦਸ਼ਮਲਵ new_smaple = static_cast<ਦਸ਼ਮਲਵ>(i);
		_alpha_filter.update(new_smaple);
		EXPECT_FLOAT_EQ(_alpha_filter.getState(), new_smaple);
	}
}

TEST(AlphaFilterTest, ਸੈੱਟ_Frequency_Test)
{
	AlphaFilter<ਦਸ਼ਮਲਵ> _alpha_filter;
	ਅਚਲ ਦਸ਼ਮਲਵ fs = .1f;

	EXPECT_FALSE(_alpha_filter.ਸੈੱਟ_Cutoff_Freq(fs, 0.f));
	EXPECT_FALSE(_alpha_filter.ਸੈੱਟ_Cutoff_Freq(fs, fs)); // Cutoff above Nyquist freq
	EXPECT_FALSE(_alpha_filter.ਸੈੱਟ_Cutoff_Freq(0.f, fs / 4.f));
	EXPECT_FALSE(_alpha_filter.ਸੈੱਟ_Cutoff_Freq(0.f, 0.f));
	EXPECT_FALSE(_alpha_filter.ਸੈੱਟ_Cutoff_Freq(-fs, fs / 4.f));
	EXPECT_FALSE(_alpha_filter.ਸੈੱਟ_Cutoff_Freq(-fs, -fs / 4.f));
	EXPECT_FALSE(_alpha_filter.ਸੈੱਟ_Cutoff_Freq(fs, -fs / 4.f));

	EXPECT_TRUE(_alpha_filter.ਸੈੱਟ_Cutoff_Freq(fs, fs / 4.f));
}

TEST(AlphaFilterTest, ConvergenceTest)
{
	AlphaFilter<ਦਸ਼ਮਲਵ> _alpha_filter;
	_alpha_filter.ਸੈੱਟ_Parameters(.1f, 1.f);

	ਦਸ਼ਮਲਵ ਆਖਰੀ_ਮੁੱਲ{0.f};

	ਲਈ (ਪੂਰਨ i = 0; i < 100; i++) {
		_alpha_filter.update(1.f);
		EXPECT_GE(_alpha_filter.getState(), ਆਖਰੀ_ਮੁੱਲ);
		ਆਖਰੀ_ਮੁੱਲ = _alpha_filter.getState();
	}

	EXPECT_NEAR(ਆਖਰੀ_ਮੁੱਲ, 1.f, 1e-4f);

	ਲਈ (ਪੂਰਨ i = 0; i < 1000; i++) {
		_alpha_filter.update(-100.f);
		EXPECT_LE(_alpha_filter.getState(), ਆਖਰੀ_ਮੁੱਲ);
		ਆਖਰੀ_ਮੁੱਲ = _alpha_filter.getState();
	}

	EXPECT_NEAR(ਆਖਰੀ_ਮੁੱਲ, -100.f, 1e-4f);
}

TEST(AlphaFilterTest, ਲੜੀਵਾਰ_ਸਕਾਰਾਤਮਕ_Rot3)
{
	// GIVEN a filter with a default time constant of 9 (alpha = 0.1)
	ਅਚਲ Quatf q_init(Eulerf(0.f, 0.f, -0.01f));
	ਅਚਲ Quatf ਨਿਵੇਸ਼(Eulerf(0.f, 0.f, M_PI_F / 2.f));

	AlphaFilter<Quatf> filter{};
	filter.ਸੈੱਟ_Alpha(.1f);
	filter.reset(q_init);

	// WHEN we run the filter 9 times
	ਲਈ (ਪੂਰਨ i = 0; i < 9; i++) {
		filter.update(ਨਿਵੇਸ਼);
	}

	// THEN the state of the filter should have reached 63%
	EXPECT_NEAR(Eulerf(filter.getState()).psi(), 0.63f * (M_PI_F / 2.f - 0.01f), 0.03);

	EXPECT_FLOAT_EQ(Eulerf(filter.getState()).phi(), 0.f);
	EXPECT_FLOAT_EQ(Eulerf(filter.getState()).theta(), 0.f);
}

TEST(AlphaFilterTest, ਲੜੀਵਾਰ_Necgative_Rot3)
{
	// GIVEN a filter with a default time constant of 9 (alpha = 0.1)
	ਅਚਲ ਦਸ਼ਮਲਵ roll = -M_PI_F;
	ਅਚਲ ਦਸ਼ਮਲਵ pitch = 0.f;
	ਅਚਲ ਦਸ਼ਮਲਵ yaw_init = 0.01f;
	ਅਚਲ ਦਸ਼ਮਲਵ yaw_ਨਿਵੇਸ਼ = -M_PI_F / 2.f;
	ਅਚਲ Quatf q_init(Eulerf(roll, pitch, yaw_init));
	ਅਚਲ Quatf ਨਿਵੇਸ਼(Eulerf(roll, pitch, yaw_ਨਿਵੇਸ਼));

	AlphaFilter<Quatf> filter{};
	filter.ਸੈੱਟ_Alpha(.1f);
	filter.reset(q_init);

	// WHEN we run the filter 9 times
	ਲਈ (ਪੂਰਨ i = 0; i < 9; i++) {
		filter.update(ਨਿਵੇਸ਼);
	}

	// THEN the state of the filter should have reached 63%
	EXPECT_NEAR(Eulerf(filter.getState()).psi(), 0.63f * (yaw_ਨਿਵੇਸ਼ + yaw_init), 0.03);

	EXPECT_TRUE(ਮੈਟ੍ਰਿਕਸ::wrap_pi(Eulerf(filter.getState()).phi() - roll) < FLT_EPSILON);
	EXPECT_TRUE(ਮੈਟ੍ਰਿਕਸ::wrap_pi(Eulerf(filter.getState()).theta() - pitch) < FLT_EPSILON);
}

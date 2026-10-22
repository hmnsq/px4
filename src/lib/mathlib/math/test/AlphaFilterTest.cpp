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

#అన్తర్భావ <gtest/gtest.h>
#అన్తర్భావ <cmath>
#అన్తర్భావ <matrix/math.hpp>

#అన్తర్భావ <lib/mathlib/math/filter/AlphaFilter.hpp>

ఉపయోగించి మాతృక::Vector3f;
ఉపయోగించి మాతృక::Quatf;
ఉపయోగించి మాతృక::Eulerf;

TEST(AlphaFilterTest, initialize_గమ్యం_Zero)
{
	AlphaFilter<దశాంశం> filter_float{};
	ASSERT_EQ(filter_float.getState(), 0.f);
}

TEST(AlphaFilterTest, reset_గమ్యం_విలువ)
{
	AlphaFilter<దశాంశం> filter_float{};
	స్థిరరాశి దశాంశం reset_విలువ = 42.42f;
	filter_float.reset(reset_విలువ);
	ASSERT_EQ(filter_float.getState(), reset_విలువ);
}

TEST(AlphaFilterTest, నిరంతరం_Zero)
{
	AlphaFilter<దశాంశం> filter_float{};
	స్థిరరాశి దశాంశం ఆదానం = 0.f;

	కొరకు (పూర్ణాంకం i = 0; i < 10; i++) {
		filter_float.update(ఆదానం);
	}

	ASSERT_EQ(filter_float.getState(), ఆదానం);
}

TEST(AlphaFilterTest, నిరంతరం_ధనాత్మక)
{
	// GIVEN an input of 1 in a filter with a default time constant of 9 (alpha = 0.1)
	AlphaFilter<దశాంశం> filter_float{};
	స్థిరరాశి దశాంశం ఆదానం = 1.f;
	filter_float.సమితి_Alpha(.1f);

	// WHEN we run the filter 9 times
	కొరకు (పూర్ణాంకం i = 0; i < 9; i++) {
		filter_float.update(ఆదానం);
	}

	// THEN the state of the filter should have reached 63%
	ASSERT_NEAR(filter_float.getState(), 0.63f, 0.02);
}

TEST(AlphaFilterTest, నిరంతరం_Negative)
{
	// GIVEN an input of 1 in a filter with a default time constant of 9 (alpha = 0.1)
	AlphaFilter<దశాంశం> filter_float{};
	స్థిరరాశి దశాంశం ఆదానం = -1.f;
	filter_float.సమితి_Alpha(.1f);

	// WHEN we run the filter 9 times
	కొరకు (పూర్ణాంకం i = 0; i < 9; i++) {
		filter_float.update(ఆదానం);
	}

	// THEN the state of the filter should have reached 63%
	ASSERT_NEAR(filter_float.getState(), -0.63f, 0.02);
}

TEST(AlphaFilterTest, rise_సమయం)
{
	// GIVEN an input of 1 in a filter with a default time constant of 9 (alpha = 0.1)
	AlphaFilter<దశాంశం> filter_float{};
	స్థిరరాశి దశాంశం ఆదానం = 1.f;
	filter_float.సమితి_Alpha(.1f);

	// WHEN we run the filter 27 times (3 * time constant)
	కొరకు (పూర్ణాంకం i = 0; i < 3 * 9; i++) {
		filter_float.update(ఆదానం);
	}

	// THEN the state of the filter should have reached 95%
	ASSERT_NEAR(filter_float.getState(), 0.95f, 0.02f);
}

TEST(AlphaFilterTest, convergence)
{
	// GIVEN an input of 1 in a filter with a default time constant of 9 (alpha = 0.1)
	AlphaFilter<దశాంశం> filter_float{};
	స్థిరరాశి దశాంశం ఆదానం = 1.f;
	filter_float.సమితి_Alpha(.1f);

	// WHEN we run the filter 45 times (5 * time constant)
	కొరకు (పూర్ణాంకం i = 0; i < 5 * 9; i++) {
		filter_float.update(ఆదానం);
	}

	// THEN the state of the filter should have converged to the input
	ASSERT_NEAR(filter_float.getState(), 1.f, 0.01f);
}

TEST(AlphaFilterTest, convergenceVector3f)
{
	// GIVEN an Vector3f input in a filter with a default time constant of 9 (alpha = 0.1)
	AlphaFilter<Vector3f> filter_v3{};
	స్థిరరాశి Vector3f ఆదానం = {3.f, 7.f, -11.f};
	filter_v3.సమితి_Alpha(.1f);

	// WHEN we run the filter 45 times (5 * time constant)
	కొరకు (పూర్ణాంకం i = 0; i < 5 * 9; i++) {
		filter_v3.update(ఆదానం);
	}

	// THEN the state of the filter should have converged to the input (1% error allowed)
	Vector3f output = filter_v3.getState();

	కొరకు (పూర్ణాంకం i = 0; i < 3; i++) {
		ASSERT_NEAR(output(i), ఆదానం(i), fabsf(0.01f * ఆదానం(i)));
	}
}

TEST(AlphaFilterTest, convergenceVector3fAlpha)
{
	// GIVEN a Vector3f input in a filter with a defined time constant and the default sampling time
	AlphaFilter<Vector3f> filter_v3{};
	స్థిరరాశి Vector3f ఆదానం = {3.f, 7.f, -11.f};
	స్థిరరాశి దశాంశం tau = 18.f;
	స్థిరరాశి దశాంశం dt = 1.f;
	filter_v3.సమితి_Parameters(dt, tau);

	// WHEN we run the filter 18 times (1 * time constant)
	కొరకు (పూర్ణాంకం i = 0; i < 18; i++) {
		filter_v3.update(ఆదానం); // dt is assumed equal to 1
	}

	// THEN the state of the filter should have reached 65% (2% error allowed)
	Vector3f output = filter_v3.getState();

	కొరకు (పూర్ణాంకం i = 0; i < 3; i++) {
		ASSERT_NEAR(output(i), 0.63f * ఆదానం(i), fabsf(0.02f * ఆదానం(i)));
	}
}

TEST(AlphaFilterTest, convergenceVector3fTauDt)
{
	// GIVEN a Vector3f input in a filter with a defined time constant and sampling time
	AlphaFilter<Vector3f> filter_v3{};
	స్థిరరాశి Vector3f ఆదానం = {51.f, 7.f, -11.f};
	స్థిరరాశి దశాంశం tau = 2.f;
	స్థిరరాశి దశాంశం dt = 0.1f;
	filter_v3.సమితి_Parameters(dt, tau);

	// WHEN we run the filter (1 * time constant)
	స్థిరరాశి దశాంశం n = tau / dt;

	కొరకు (పూర్ణాంకం i = 0; i < n; i++) {
		filter_v3.update(ఆదానం);
	}

	// THEN the state of the filter should have reached 65% (2% error allowed)
	Vector3f output = filter_v3.getState();

	కొరకు (పూర్ణాంకం i = 0; i < 3; i++) {
		ASSERT_NEAR(output(i), 0.63f * ఆదానం(i), fabsf(0.02f * ఆదానం(i)));
	}

	// ALSO when the filter is reset to a specified value
	స్థిరరాశి Vector3f reset_సదిశ = {-1.f, 71.f, -42.f};
	filter_v3.reset(reset_సదిశ);
	output = filter_v3.getState();

	// THEN the filter should exactly contain those values
	కొరకు (పూర్ణాంకం i = 0; i < 3; i++) {
		ASSERT_EQ(output(i), reset_సదిశ(i));
	}
}

TEST(AlphaFilterTest, AllZeroTest)
{
	AlphaFilter<దశాంశం> _alpha_filter;
	_alpha_filter.update(0.f);
	EXPECT_FLOAT_EQ(_alpha_filter.getState(), 0.f);
}

TEST(AlphaFilterTest, AlphaZeroTest)
{
	AlphaFilter<దశాంశం> _alpha_filter;
	_alpha_filter.సమితి_Parameters(1e-5f, 1e5f);

	కొరకు (పూర్ణాంకం i = 0; i < 100; i++) {
		_alpha_filter.update(1.f);
		EXPECT_NEAR(_alpha_filter.getState(), 0.f, 1e-4f);
	}
}

TEST(AlphaFilterTest, AlphaOneTest)
{
	AlphaFilter<దశాంశం> _alpha_filter;
	_alpha_filter.సమితి_Parameters(.1f, 0.f);

	కొరకు (పూర్ణాంకం i = 0; i < 100; i++) {
		స్థిరరాశి దశాంశం new_smaple = static_cast<దశాంశం>(i);
		_alpha_filter.update(new_smaple);
		EXPECT_FLOAT_EQ(_alpha_filter.getState(), new_smaple);
	}
}

TEST(AlphaFilterTest, సమితి_Frequency_Test)
{
	AlphaFilter<దశాంశం> _alpha_filter;
	స్థిరరాశి దశాంశం fs = .1f;

	EXPECT_FALSE(_alpha_filter.సమితి_Cutoff_Freq(fs, 0.f));
	EXPECT_FALSE(_alpha_filter.సమితి_Cutoff_Freq(fs, fs)); // Cutoff above Nyquist freq
	EXPECT_FALSE(_alpha_filter.సమితి_Cutoff_Freq(0.f, fs / 4.f));
	EXPECT_FALSE(_alpha_filter.సమితి_Cutoff_Freq(0.f, 0.f));
	EXPECT_FALSE(_alpha_filter.సమితి_Cutoff_Freq(-fs, fs / 4.f));
	EXPECT_FALSE(_alpha_filter.సమితి_Cutoff_Freq(-fs, -fs / 4.f));
	EXPECT_FALSE(_alpha_filter.సమితి_Cutoff_Freq(fs, -fs / 4.f));

	EXPECT_TRUE(_alpha_filter.సమితి_Cutoff_Freq(fs, fs / 4.f));
}

TEST(AlphaFilterTest, ConvergenceTest)
{
	AlphaFilter<దశాంశం> _alpha_filter;
	_alpha_filter.సమితి_Parameters(.1f, 1.f);

	దశాంశం చివరి_విలువ{0.f};

	కొరకు (పూర్ణాంకం i = 0; i < 100; i++) {
		_alpha_filter.update(1.f);
		EXPECT_GE(_alpha_filter.getState(), చివరి_విలువ);
		చివరి_విలువ = _alpha_filter.getState();
	}

	EXPECT_NEAR(చివరి_విలువ, 1.f, 1e-4f);

	కొరకు (పూర్ణాంకం i = 0; i < 1000; i++) {
		_alpha_filter.update(-100.f);
		EXPECT_LE(_alpha_filter.getState(), చివరి_విలువ);
		చివరి_విలువ = _alpha_filter.getState();
	}

	EXPECT_NEAR(చివరి_విలువ, -100.f, 1e-4f);
}

TEST(AlphaFilterTest, నిరంతరం_ధనాత్మక_Rot3)
{
	// GIVEN a filter with a default time constant of 9 (alpha = 0.1)
	స్థిరరాశి Quatf q_init(Eulerf(0.f, 0.f, -0.01f));
	స్థిరరాశి Quatf ఆదానం(Eulerf(0.f, 0.f, M_PI_F / 2.f));

	AlphaFilter<Quatf> filter{};
	filter.సమితి_Alpha(.1f);
	filter.reset(q_init);

	// WHEN we run the filter 9 times
	కొరకు (పూర్ణాంకం i = 0; i < 9; i++) {
		filter.update(ఆదానం);
	}

	// THEN the state of the filter should have reached 63%
	EXPECT_NEAR(Eulerf(filter.getState()).psi(), 0.63f * (M_PI_F / 2.f - 0.01f), 0.03);

	EXPECT_FLOAT_EQ(Eulerf(filter.getState()).phi(), 0.f);
	EXPECT_FLOAT_EQ(Eulerf(filter.getState()).theta(), 0.f);
}

TEST(AlphaFilterTest, నిరంతరం_Necgative_Rot3)
{
	// GIVEN a filter with a default time constant of 9 (alpha = 0.1)
	స్థిరరాశి దశాంశం roll = -M_PI_F;
	స్థిరరాశి దశాంశం pitch = 0.f;
	స్థిరరాశి దశాంశం yaw_init = 0.01f;
	స్థిరరాశి దశాంశం yaw_ఆదానం = -M_PI_F / 2.f;
	స్థిరరాశి Quatf q_init(Eulerf(roll, pitch, yaw_init));
	స్థిరరాశి Quatf ఆదానం(Eulerf(roll, pitch, yaw_ఆదానం));

	AlphaFilter<Quatf> filter{};
	filter.సమితి_Alpha(.1f);
	filter.reset(q_init);

	// WHEN we run the filter 9 times
	కొరకు (పూర్ణాంకం i = 0; i < 9; i++) {
		filter.update(ఆదానం);
	}

	// THEN the state of the filter should have reached 63%
	EXPECT_NEAR(Eulerf(filter.getState()).psi(), 0.63f * (yaw_ఆదానం + yaw_init), 0.03);

	EXPECT_TRUE(మాతృక::wrap_pi(Eulerf(filter.getState()).phi() - roll) < FLT_EPSILON);
	EXPECT_TRUE(మాతృక::wrap_pi(Eulerf(filter.getState()).theta() - pitch) < FLT_EPSILON);
}

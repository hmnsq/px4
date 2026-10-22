/****************************************************************************
 *
 *   Copyright (C) 2024 PX4 Development Team. All rights reserved.
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
 * Test code for the Heading Smoothing library
 * Run exclusively this test with the command "make tests TESTFILTER=HeadingSmoothing"
 */

#అన్తర్భావ "mathlib/math/Limits.hpp"
#అన్తర్భావ <gtest/gtest.h>

#అన్తర్భావ <motion_planning/HeadingSmoothing.hpp>

ఉపయోగించి నామాకాశ మాతృక;

తరగతి HeadingSmoothingTest : బహిరంగ ::testing::Test
{
బహిరంగ:
	HeadingSmoothingTest()
	{
		_smoothing.సమితి_Max_Heading_రేటు(15.f);
		_smoothing.సమితి_Max_Heading_Accel(1.f);
	}

	HeadingSmoothing _smoothing;
	దశాంశం _dt{.1f};
};

TEST_F(HeadingSmoothingTest, convergence)
{
	స్థిరరాశి దశాంశం heading_current = math::radians(0.f);
	స్థిరరాశి దశాంశం heading_లక్ష్యం = math::radians(5.f);
	_smoothing.reset(heading_current, 0.f);

	స్థిరరాశి పూర్ణాంకం nb_అడుగులు = ceilf(1.f / _dt);

	కొరకు (పూర్ణాంకం i = 0; i < nb_అడుగులు; i++) {
		_smoothing.update(heading_లక్ష్యం, _dt);
	}

	స్థిరరాశి దశాంశం heading = _smoothing.getSmoothedHeading();
	స్థిరరాశి దశాంశం heading_రేటు = _smoothing.get_Smoothed_Heading_రేటు();
	EXPECT_EQ(heading, heading_లక్ష్యం) << "heading (deg): " << math::degrees(heading);
	EXPECT_EQ(heading_రేటు, 0.f);
}

TEST_F(HeadingSmoothingTest, zero_దాటడం)
{
	స్థిరరాశి దశాంశం heading_current = math::radians(-95.f);
	స్థిరరాశి దశాంశం heading_లక్ష్యం = math::radians(5.f);
	_smoothing.reset(heading_current, 0.f);

	స్థిరరాశి పూర్ణాంకం nb_అడుగులు = ceilf(4.f / _dt);

	కొరకు (పూర్ణాంకం i = 0; i < nb_అడుగులు; i++) {
		_smoothing.update(heading_లక్ష్యం, _dt);
	}

	స్థిరరాశి దశాంశం heading = _smoothing.getSmoothedHeading();
	స్థిరరాశి దశాంశం heading_రేటు = _smoothing.get_Smoothed_Heading_రేటు();
	EXPECT_EQ(heading, heading_లక్ష్యం) << "heading (deg): " << math::degrees(heading);
	EXPECT_EQ(heading_రేటు, 0.f);
}

TEST_F(HeadingSmoothingTest, wrap_pi)
{
	స్థిరరాశి దశాంశం heading_current = math::radians(-170.f);
	స్థిరరాశి దశాంశం heading_లక్ష్యం = math::radians(170.f);
	_smoothing.reset(heading_current, 0.f);

	స్థిరరాశి పూర్ణాంకం nb_అడుగులు = ceilf(2.f / _dt);

	కొరకు (పూర్ణాంకం i = 0; i < nb_అడుగులు; i++) {
		_smoothing.update(heading_లక్ష్యం, _dt);
	}

	స్థిరరాశి దశాంశం heading = _smoothing.getSmoothedHeading();
	స్థిరరాశి దశాంశం heading_రేటు = _smoothing.get_Smoothed_Heading_రేటు();
	EXPECT_EQ(heading, heading_లక్ష్యం) << "heading (deg): " << math::degrees(heading);
	EXPECT_EQ(heading_రేటు, 0.f);
}

TEST_F(HeadingSmoothingTest, ధనాత్మక_రేటు)
{
	స్థిరరాశి దశాంశం max_heading_రేటు = .15f;
	_smoothing.సమితి_Max_Heading_రేటు(max_heading_రేటు);
	స్థిరరాశి దశాంశం heading_current = math::radians(-170.f);
	స్థిరరాశి దశాంశం heading_లక్ష్యం = math::radians(-20.f);
	_smoothing.reset(heading_current, 0.f);

	స్థిరరాశి పూర్ణాంకం nb_అడుగులు = ceilf(2.f / _dt);

	కొరకు (పూర్ణాంకం i = 0; i < nb_అడుగులు; i++) {
		_smoothing.update(heading_లక్ష్యం, _dt);
	}

	స్థిరరాశి దశాంశం heading_రేటు = _smoothing.get_Smoothed_Heading_రేటు();
	EXPECT_EQ(heading_రేటు, max_heading_రేటు);
}

TEST_F(HeadingSmoothingTest, negative_రేటు)
{
	స్థిరరాశి దశాంశం max_heading_రేటు = 0.15;
	_smoothing.సమితి_Max_Heading_రేటు(max_heading_రేటు);
	స్థిరరాశి దశాంశం heading_current = math::radians(-20.f);
	స్థిరరాశి దశాంశం heading_లక్ష్యం = math::radians(-140.f);
	_smoothing.reset(heading_current, 0.f);

	స్థిరరాశి పూర్ణాంకం nb_అడుగులు = ceilf(2.f / _dt);

	కొరకు (పూర్ణాంకం i = 0; i < nb_అడుగులు; i++) {
		_smoothing.update(heading_లక్ష్యం, _dt);
	}

	స్థిరరాశి దశాంశం heading_రేటు = _smoothing.get_Smoothed_Heading_రేటు();
	EXPECT_EQ(heading_రేటు, -max_heading_రేటు);
}

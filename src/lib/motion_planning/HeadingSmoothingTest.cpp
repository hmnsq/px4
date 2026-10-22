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

#ضمن "mathlib/math/Limits.hpp"
#ضمن <gtest/gtest.h>

#ضمن <motion_planning/HeadingSmoothing.hpp>

استفاده نطاق ماتریس;

صنف HeadingSmoothingTest : عام ::testing::Test
{
عام:
	HeadingSmoothingTest()
	{
		_smoothing.مجموعه_Max_Heading_نرخ(15.f);
		_smoothing.مجموعه_Max_Heading_Accel(1.f);
	}

	HeadingSmoothing _smoothing;
	عائم _dt{.1f};
};

TEST_F(HeadingSmoothingTest, convergence)
{
	ثابت عائم heading_current = math::radians(0.f);
	ثابت عائم heading_هدف = math::radians(5.f);
	_smoothing.reset(heading_current, 0.f);

	ثابت صحيح nb_گامها = ceilf(1.f / _dt);

	لكل (صحيح i = 0; i < nb_گامها; i++) {
		_smoothing.update(heading_هدف, _dt);
	}

	ثابت عائم heading = _smoothing.getSmoothedHeading();
	ثابت عائم heading_نرخ = _smoothing.get_Smoothed_Heading_نرخ();
	EXPECT_EQ(heading, heading_هدف) << "heading (deg): " << math::degrees(heading);
	EXPECT_EQ(heading_نرخ, 0.f);
}

TEST_F(HeadingSmoothingTest, zero_تقاطع)
{
	ثابت عائم heading_current = math::radians(-95.f);
	ثابت عائم heading_هدف = math::radians(5.f);
	_smoothing.reset(heading_current, 0.f);

	ثابت صحيح nb_گامها = ceilf(4.f / _dt);

	لكل (صحيح i = 0; i < nb_گامها; i++) {
		_smoothing.update(heading_هدف, _dt);
	}

	ثابت عائم heading = _smoothing.getSmoothedHeading();
	ثابت عائم heading_نرخ = _smoothing.get_Smoothed_Heading_نرخ();
	EXPECT_EQ(heading, heading_هدف) << "heading (deg): " << math::degrees(heading);
	EXPECT_EQ(heading_نرخ, 0.f);
}

TEST_F(HeadingSmoothingTest, wrap_pi)
{
	ثابت عائم heading_current = math::radians(-170.f);
	ثابت عائم heading_هدف = math::radians(170.f);
	_smoothing.reset(heading_current, 0.f);

	ثابت صحيح nb_گامها = ceilf(2.f / _dt);

	لكل (صحيح i = 0; i < nb_گامها; i++) {
		_smoothing.update(heading_هدف, _dt);
	}

	ثابت عائم heading = _smoothing.getSmoothedHeading();
	ثابت عائم heading_نرخ = _smoothing.get_Smoothed_Heading_نرخ();
	EXPECT_EQ(heading, heading_هدف) << "heading (deg): " << math::degrees(heading);
	EXPECT_EQ(heading_نرخ, 0.f);
}

TEST_F(HeadingSmoothingTest, مثبت_نرخ)
{
	ثابت عائم max_heading_نرخ = .15f;
	_smoothing.مجموعه_Max_Heading_نرخ(max_heading_نرخ);
	ثابت عائم heading_current = math::radians(-170.f);
	ثابت عائم heading_هدف = math::radians(-20.f);
	_smoothing.reset(heading_current, 0.f);

	ثابت صحيح nb_گامها = ceilf(2.f / _dt);

	لكل (صحيح i = 0; i < nb_گامها; i++) {
		_smoothing.update(heading_هدف, _dt);
	}

	ثابت عائم heading_نرخ = _smoothing.get_Smoothed_Heading_نرخ();
	EXPECT_EQ(heading_نرخ, max_heading_نرخ);
}

TEST_F(HeadingSmoothingTest, negative_نرخ)
{
	ثابت عائم max_heading_نرخ = 0.15;
	_smoothing.مجموعه_Max_Heading_نرخ(max_heading_نرخ);
	ثابت عائم heading_current = math::radians(-20.f);
	ثابت عائم heading_هدف = math::radians(-140.f);
	_smoothing.reset(heading_current, 0.f);

	ثابت صحيح nb_گامها = ceilf(2.f / _dt);

	لكل (صحيح i = 0; i < nb_گامها; i++) {
		_smoothing.update(heading_هدف, _dt);
	}

	ثابت عائم heading_نرخ = _smoothing.get_Smoothed_Heading_نرخ();
	EXPECT_EQ(heading_نرخ, -max_heading_نرخ);
}

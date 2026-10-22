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

#ಅನ್ತರ್ಭಾವ "mathlib/math/Limits.hpp"
#ಅನ್ತರ್ಭಾವ <gtest/gtest.h>

#ಅನ್ತರ್ಭಾವ <motion_planning/HeadingSmoothing.hpp>

ಬಳಸಿ ನಾಮಾಕಾಶ ಮಾತೃಕೆ;

ವರ್ಗ HeadingSmoothingTest : ಸಾರ್ವಜನಿಕ ::testing::Test
{
ಸಾರ್ವಜನಿಕ:
	HeadingSmoothingTest()
	{
		_smoothing.ಗಣ_Max_Heading_ದರ(15.f);
		_smoothing.ಗಣ_Max_Heading_Accel(1.f);
	}

	HeadingSmoothing _smoothing;
	ದಶಮಾಂಶ _dt{.1f};
};

TEST_F(HeadingSmoothingTest, convergence)
{
	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ heading_current = math::radians(0.f);
	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ heading_ಗುರಿ = math::radians(5.f);
	_smoothing.reset(heading_current, 0.f);

	ಸ್ಥಿರಾಂಕ ಪೂರ್ಣಾಂಕ nb_ಹೆಜ್ಜೆಗಳು = ceilf(1.f / _dt);

	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < nb_ಹೆಜ್ಜೆಗಳು; i++) {
		_smoothing.update(heading_ಗುರಿ, _dt);
	}

	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ heading = _smoothing.getSmoothedHeading();
	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ heading_ದರ = _smoothing.get_Smoothed_Heading_ದರ();
	EXPECT_EQ(heading, heading_ಗುರಿ) << "heading (deg): " << math::degrees(heading);
	EXPECT_EQ(heading_ದರ, 0.f);
}

TEST_F(HeadingSmoothingTest, zero_ದಾಟುವಿಕೆ)
{
	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ heading_current = math::radians(-95.f);
	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ heading_ಗುರಿ = math::radians(5.f);
	_smoothing.reset(heading_current, 0.f);

	ಸ್ಥಿರಾಂಕ ಪೂರ್ಣಾಂಕ nb_ಹೆಜ್ಜೆಗಳು = ceilf(4.f / _dt);

	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < nb_ಹೆಜ್ಜೆಗಳು; i++) {
		_smoothing.update(heading_ಗುರಿ, _dt);
	}

	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ heading = _smoothing.getSmoothedHeading();
	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ heading_ದರ = _smoothing.get_Smoothed_Heading_ದರ();
	EXPECT_EQ(heading, heading_ಗುರಿ) << "heading (deg): " << math::degrees(heading);
	EXPECT_EQ(heading_ದರ, 0.f);
}

TEST_F(HeadingSmoothingTest, wrap_pi)
{
	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ heading_current = math::radians(-170.f);
	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ heading_ಗುರಿ = math::radians(170.f);
	_smoothing.reset(heading_current, 0.f);

	ಸ್ಥಿರಾಂಕ ಪೂರ್ಣಾಂಕ nb_ಹೆಜ್ಜೆಗಳು = ceilf(2.f / _dt);

	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < nb_ಹೆಜ್ಜೆಗಳು; i++) {
		_smoothing.update(heading_ಗುರಿ, _dt);
	}

	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ heading = _smoothing.getSmoothedHeading();
	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ heading_ದರ = _smoothing.get_Smoothed_Heading_ದರ();
	EXPECT_EQ(heading, heading_ಗುರಿ) << "heading (deg): " << math::degrees(heading);
	EXPECT_EQ(heading_ದರ, 0.f);
}

TEST_F(HeadingSmoothingTest, ಧನಾತ್ಮಕ_ದರ)
{
	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ max_heading_ದರ = .15f;
	_smoothing.ಗಣ_Max_Heading_ದರ(max_heading_ದರ);
	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ heading_current = math::radians(-170.f);
	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ heading_ಗುರಿ = math::radians(-20.f);
	_smoothing.reset(heading_current, 0.f);

	ಸ್ಥಿರಾಂಕ ಪೂರ್ಣಾಂಕ nb_ಹೆಜ್ಜೆಗಳು = ceilf(2.f / _dt);

	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < nb_ಹೆಜ್ಜೆಗಳು; i++) {
		_smoothing.update(heading_ಗುರಿ, _dt);
	}

	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ heading_ದರ = _smoothing.get_Smoothed_Heading_ದರ();
	EXPECT_EQ(heading_ದರ, max_heading_ದರ);
}

TEST_F(HeadingSmoothingTest, negative_ದರ)
{
	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ max_heading_ದರ = 0.15;
	_smoothing.ಗಣ_Max_Heading_ದರ(max_heading_ದರ);
	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ heading_current = math::radians(-20.f);
	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ heading_ಗುರಿ = math::radians(-140.f);
	_smoothing.reset(heading_current, 0.f);

	ಸ್ಥಿರಾಂಕ ಪೂರ್ಣಾಂಕ nb_ಹೆಜ್ಜೆಗಳು = ceilf(2.f / _dt);

	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < nb_ಹೆಜ್ಜೆಗಳು; i++) {
		_smoothing.update(heading_ಗುರಿ, _dt);
	}

	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ heading_ದರ = _smoothing.get_Smoothed_Heading_ದರ();
	EXPECT_EQ(heading_ದರ, -max_heading_ದರ);
}

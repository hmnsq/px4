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

#अन्तर्भाव "mathlib/math/Limits.hpp"
#अन्तर्भाव <gtest/gtest.h>

#अन्तर्भाव <motion_planning/HeadingSmoothing.hpp>

उपयुज्य नामाकाश matrix;

वर्ग HeadingSmoothingTest : सार्वजनिक ::testing::Test
{
सार्वजनिक:
	HeadingSmoothingTest()
	{
		_smoothing.समुच्चयः_Max_Heading_दरः(15.f);
		_smoothing.समुच्चयः_Max_Heading_Accel(1.f);
	}

	HeadingSmoothing _smoothing;
	भिन्न _dt{.1f};
};

TEST_F(HeadingSmoothingTest, convergence)
{
	ध्रुव भिन्न heading_current = math::radians(0.f);
	ध्रुव भिन्न heading_target = math::radians(5.f);
	_smoothing.reset(heading_current, 0.f);

	ध्रुव पूर्णाङ्क nb_चरणाः = ceilf(1.f / _dt);

	प्रति (पूर्णाङ्क i = 0; i < nb_चरणाः; i++) {
		_smoothing.update(heading_target, _dt);
	}

	ध्रुव भिन्न heading = _smoothing.getSmoothedHeading();
	ध्रुव भिन्न heading_दरः = _smoothing.get_Smoothed_Heading_दरः();
	EXPECT_EQ(heading, heading_target) << "heading (deg): " << math::degrees(heading);
	EXPECT_EQ(heading_दरः, 0.f);
}

TEST_F(HeadingSmoothingTest, zero_crossing)
{
	ध्रुव भिन्न heading_current = math::radians(-95.f);
	ध्रुव भिन्न heading_target = math::radians(5.f);
	_smoothing.reset(heading_current, 0.f);

	ध्रुव पूर्णाङ्क nb_चरणाः = ceilf(4.f / _dt);

	प्रति (पूर्णाङ्क i = 0; i < nb_चरणाः; i++) {
		_smoothing.update(heading_target, _dt);
	}

	ध्रुव भिन्न heading = _smoothing.getSmoothedHeading();
	ध्रुव भिन्न heading_दरः = _smoothing.get_Smoothed_Heading_दरः();
	EXPECT_EQ(heading, heading_target) << "heading (deg): " << math::degrees(heading);
	EXPECT_EQ(heading_दरः, 0.f);
}

TEST_F(HeadingSmoothingTest, wrap_pi)
{
	ध्रुव भिन्न heading_current = math::radians(-170.f);
	ध्रुव भिन्न heading_target = math::radians(170.f);
	_smoothing.reset(heading_current, 0.f);

	ध्रुव पूर्णाङ्क nb_चरणाः = ceilf(2.f / _dt);

	प्रति (पूर्णाङ्क i = 0; i < nb_चरणाः; i++) {
		_smoothing.update(heading_target, _dt);
	}

	ध्रुव भिन्न heading = _smoothing.getSmoothedHeading();
	ध्रुव भिन्न heading_दरः = _smoothing.get_Smoothed_Heading_दरः();
	EXPECT_EQ(heading, heading_target) << "heading (deg): " << math::degrees(heading);
	EXPECT_EQ(heading_दरः, 0.f);
}

TEST_F(HeadingSmoothingTest, धनात्मकम्_दरः)
{
	ध्रुव भिन्न max_heading_दरः = .15f;
	_smoothing.समुच्चयः_Max_Heading_दरः(max_heading_दरः);
	ध्रुव भिन्न heading_current = math::radians(-170.f);
	ध्रुव भिन्न heading_target = math::radians(-20.f);
	_smoothing.reset(heading_current, 0.f);

	ध्रुव पूर्णाङ्क nb_चरणाः = ceilf(2.f / _dt);

	प्रति (पूर्णाङ्क i = 0; i < nb_चरणाः; i++) {
		_smoothing.update(heading_target, _dt);
	}

	ध्रुव भिन्न heading_दरः = _smoothing.get_Smoothed_Heading_दरः();
	EXPECT_EQ(heading_दरः, max_heading_दरः);
}

TEST_F(HeadingSmoothingTest, negative_दरः)
{
	ध्रुव भिन्न max_heading_दरः = 0.15;
	_smoothing.समुच्चयः_Max_Heading_दरः(max_heading_दरः);
	ध्रुव भिन्न heading_current = math::radians(-20.f);
	ध्रुव भिन्न heading_target = math::radians(-140.f);
	_smoothing.reset(heading_current, 0.f);

	ध्रुव पूर्णाङ्क nb_चरणाः = ceilf(2.f / _dt);

	प्रति (पूर्णाङ्क i = 0; i < nb_चरणाः; i++) {
		_smoothing.update(heading_target, _dt);
	}

	ध्रुव भिन्न heading_दरः = _smoothing.get_Smoothed_Heading_दरः();
	EXPECT_EQ(heading_दरः, -max_heading_दरः);
}

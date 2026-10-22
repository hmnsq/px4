/****************************************************************************
 *
 *  Copyright (C) 2012-2019 PX4 Development Team. All rights reserved.
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
 * @file HysteresisTest.cpp
 * Tests for system timing hysteresis.
 */

#ಅನ್ತರ್ಭಾವ <gtest/gtest.h>

#ಅನ್ತರ್ಭಾವ "hysteresis.h"


ಸ್ಥಿರ constexpr hrt_abstime SOME_ಆರಂಭ_ಸಮಯ = 1558359616134000llu;


TEST(Hysteresis, InitFalse)
{
	systemlib::Hysteresis hysteresis(ಸುಳ್ಳು);
	EXPECT_FALSE(hysteresis.get_state());
}

TEST(Hysteresis, InitTrue)
{
	systemlib::Hysteresis hysteresis(ನಿಜ);
	EXPECT_TRUE(hysteresis.get_state());
}

TEST(Hysteresis, Zero)
{
	hrt_abstime ಸಮಯ_us = SOME_ಆರಂಭ_ಸಮಯ;

	// Default is 0 hysteresis.
	systemlib::Hysteresis hysteresis(ಸುಳ್ಳು);
	EXPECT_FALSE(hysteresis.get_state());

	// Change and see result immediately.
	hysteresis.ಗಣ_state_and_update(ನಿಜ, ಸಮಯ_us);
	EXPECT_TRUE(hysteresis.get_state());
	hysteresis.ಗಣ_state_and_update(ಸುಳ್ಳು, ಸಮಯ_us);
	EXPECT_FALSE(hysteresis.get_state());
	hysteresis.ಗಣ_state_and_update(ನಿಜ, ಸಮಯ_us);
	EXPECT_TRUE(hysteresis.get_state());

	ಸಮಯ_us += 1000;
	// A wait won't change anything.
	hysteresis.update(ಸಮಯ_us);
	EXPECT_TRUE(hysteresis.get_state());
}

TEST(Hysteresis, Change_After_ಸಮಯ)
{
	hrt_abstime ಸಮಯ_us = SOME_ಆರಂಭ_ಸಮಯ;

	systemlib::Hysteresis hysteresis(ಸುಳ್ಳು);
	hysteresis.ಗಣ_hysteresis_ಸಮಯ_from(ಸುಳ್ಳು, 5000);
	hysteresis.ಗಣ_hysteresis_ಸಮಯ_from(ನಿಜ, 3000);

	// Change to true.
	hysteresis.ಗಣ_state_and_update(ನಿಜ, ಸಮಯ_us);
	EXPECT_FALSE(hysteresis.get_state());
	ಸಮಯ_us += 4000;
	hysteresis.update(ಸಮಯ_us);
	EXPECT_FALSE(hysteresis.get_state());
	ಸಮಯ_us += 2000;
	hysteresis.update(ಸಮಯ_us);
	EXPECT_TRUE(hysteresis.get_state());

	// Change back to false.
	hysteresis.ಗಣ_state_and_update(ಸುಳ್ಳು, ಸಮಯ_us);
	EXPECT_TRUE(hysteresis.get_state());
	ಸಮಯ_us += 1000;
	hysteresis.update(ಸಮಯ_us);
	EXPECT_TRUE(hysteresis.get_state());
	ಸಮಯ_us += 3000;
	hysteresis.update(ಸಮಯ_us);
	EXPECT_FALSE(hysteresis.get_state());
}

TEST(Hysteresis, Hysteresis_ಬದಲಾಯಿತು)
{
	hrt_abstime ಸಮಯ_us = SOME_ಆರಂಭ_ಸಮಯ;

	systemlib::Hysteresis hysteresis(ಸುಳ್ಳು);
	hysteresis.ಗಣ_hysteresis_ಸಮಯ_from(ನಿಜ, 2000);
	hysteresis.ಗಣ_hysteresis_ಸಮಯ_from(ಸುಳ್ಳು, 5000);

	// Change to true.
	hysteresis.ಗಣ_state_and_update(ನಿಜ, ಸಮಯ_us);
	EXPECT_FALSE(hysteresis.get_state());
	ಸಮಯ_us += 3000;
	hysteresis.update(ಸಮಯ_us);
	EXPECT_FALSE(hysteresis.get_state());
	ಸಮಯ_us += 3000;
	hysteresis.update(ಸಮಯ_us);
	EXPECT_TRUE(hysteresis.get_state());

	// Change hysteresis time.
	hysteresis.ಗಣ_hysteresis_ಸಮಯ_from(ನಿಜ, 10000);

	// Change back to false.
	hysteresis.ಗಣ_state_and_update(ಸುಳ್ಳು, ಸಮಯ_us);
	EXPECT_TRUE(hysteresis.get_state());
	ಸಮಯ_us += 7000;
	hysteresis.update(ಸಮಯ_us);
	EXPECT_TRUE(hysteresis.get_state());
	ಸಮಯ_us += 5000;
	hysteresis.update(ಸಮಯ_us);
	EXPECT_FALSE(hysteresis.get_state());
}

TEST(Hysteresis, Change_After_ಅಪವರ್ತ್ಯ_Sets)
{
	hrt_abstime ಸಮಯ_us = SOME_ಆರಂಭ_ಸಮಯ;

	systemlib::Hysteresis hysteresis(ಸುಳ್ಳು);
	hysteresis.ಗಣ_hysteresis_ಸಮಯ_from(ನಿಜ, 5000);
	hysteresis.ಗಣ_hysteresis_ಸಮಯ_from(ಸುಳ್ಳು, 5000);

	// Change to true.
	hysteresis.ಗಣ_state_and_update(ನಿಜ, ಸಮಯ_us);
	EXPECT_FALSE(hysteresis.get_state());
	ಸಮಯ_us += 3000;
	hysteresis.ಗಣ_state_and_update(ನಿಜ, ಸಮಯ_us);
	EXPECT_FALSE(hysteresis.get_state());
	ಸಮಯ_us += 3000;
	hysteresis.ಗಣ_state_and_update(ನಿಜ, ಸಮಯ_us);
	EXPECT_TRUE(hysteresis.get_state());

	// Change to false.
	hysteresis.ಗಣ_state_and_update(ಸುಳ್ಳು, ಸಮಯ_us);
	EXPECT_TRUE(hysteresis.get_state());
	ಸಮಯ_us += 3000;
	hysteresis.ಗಣ_state_and_update(ಸುಳ್ಳು, ಸಮಯ_us);
	EXPECT_TRUE(hysteresis.get_state());
	ಸಮಯ_us += 3000;
	hysteresis.ಗಣ_state_and_update(ಸುಳ್ಳು, ಸಮಯ_us);
	EXPECT_FALSE(hysteresis.get_state());
}

TEST(Hysteresis, Take_Change_ಹಿಂಭಾಗ)
{
	hrt_abstime ಸಮಯ_us = SOME_ಆರಂಭ_ಸಮಯ;

	systemlib::Hysteresis hysteresis(ಸುಳ್ಳು);
	hysteresis.ಗಣ_hysteresis_ಸಮಯ_from(ಸುಳ್ಳು, 5000);

	// Change to true.
	hysteresis.ಗಣ_state_and_update(ನಿಜ, ಸಮಯ_us);
	EXPECT_FALSE(hysteresis.get_state());
	ಸಮಯ_us += 3000;
	hysteresis.update(ಸಮಯ_us);
	EXPECT_FALSE(hysteresis.get_state());
	// Change your mind to false.
	hysteresis.ಗಣ_state_and_update(ಸುಳ್ಳು, ಸಮಯ_us);
	EXPECT_FALSE(hysteresis.get_state());
	ಸಮಯ_us += 6000;
	hysteresis.update(ಸಮಯ_us);
	EXPECT_FALSE(hysteresis.get_state());

	// And true again
	hysteresis.ಗಣ_state_and_update(ನಿಜ, ಸಮಯ_us);
	EXPECT_FALSE(hysteresis.get_state());
	ಸಮಯ_us += 3000;
	hysteresis.update(ಸಮಯ_us);
	EXPECT_FALSE(hysteresis.get_state());
	ಸಮಯ_us += 3000;
	hysteresis.update(ಸಮಯ_us);
	EXPECT_TRUE(hysteresis.get_state());

	// The other directory is immediate.
	hysteresis.ಗಣ_state_and_update(ಸುಳ್ಳು, ಸಮಯ_us);
	EXPECT_FALSE(hysteresis.get_state());
}

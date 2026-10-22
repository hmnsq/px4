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

#అన్తర్భావ <gtest/gtest.h>

#అన్తర్భావ "hysteresis.h"


స్థిర constexpr hrt_abstime SOME_ప్రారంభం_సమయం = 1558359616134000llu;


TEST(Hysteresis, InitFalse)
{
	systemlib::Hysteresis hysteresis(అబద్ధం);
	EXPECT_FALSE(hysteresis.get_state());
}

TEST(Hysteresis, InitTrue)
{
	systemlib::Hysteresis hysteresis(నిజం);
	EXPECT_TRUE(hysteresis.get_state());
}

TEST(Hysteresis, Zero)
{
	hrt_abstime సమయం_us = SOME_ప్రారంభం_సమయం;

	// Default is 0 hysteresis.
	systemlib::Hysteresis hysteresis(అబద్ధం);
	EXPECT_FALSE(hysteresis.get_state());

	// Change and see result immediately.
	hysteresis.సమితి_state_and_update(నిజం, సమయం_us);
	EXPECT_TRUE(hysteresis.get_state());
	hysteresis.సమితి_state_and_update(అబద్ధం, సమయం_us);
	EXPECT_FALSE(hysteresis.get_state());
	hysteresis.సమితి_state_and_update(నిజం, సమయం_us);
	EXPECT_TRUE(hysteresis.get_state());

	సమయం_us += 1000;
	// A wait won't change anything.
	hysteresis.update(సమయం_us);
	EXPECT_TRUE(hysteresis.get_state());
}

TEST(Hysteresis, Change_After_సమయం)
{
	hrt_abstime సమయం_us = SOME_ప్రారంభం_సమయం;

	systemlib::Hysteresis hysteresis(అబద్ధం);
	hysteresis.సమితి_hysteresis_సమయం_from(అబద్ధం, 5000);
	hysteresis.సమితి_hysteresis_సమయం_from(నిజం, 3000);

	// Change to true.
	hysteresis.సమితి_state_and_update(నిజం, సమయం_us);
	EXPECT_FALSE(hysteresis.get_state());
	సమయం_us += 4000;
	hysteresis.update(సమయం_us);
	EXPECT_FALSE(hysteresis.get_state());
	సమయం_us += 2000;
	hysteresis.update(సమయం_us);
	EXPECT_TRUE(hysteresis.get_state());

	// Change back to false.
	hysteresis.సమితి_state_and_update(అబద్ధం, సమయం_us);
	EXPECT_TRUE(hysteresis.get_state());
	సమయం_us += 1000;
	hysteresis.update(సమయం_us);
	EXPECT_TRUE(hysteresis.get_state());
	సమయం_us += 3000;
	hysteresis.update(సమయం_us);
	EXPECT_FALSE(hysteresis.get_state());
}

TEST(Hysteresis, Hysteresis_మారింది)
{
	hrt_abstime సమయం_us = SOME_ప్రారంభం_సమయం;

	systemlib::Hysteresis hysteresis(అబద్ధం);
	hysteresis.సమితి_hysteresis_సమయం_from(నిజం, 2000);
	hysteresis.సమితి_hysteresis_సమయం_from(అబద్ధం, 5000);

	// Change to true.
	hysteresis.సమితి_state_and_update(నిజం, సమయం_us);
	EXPECT_FALSE(hysteresis.get_state());
	సమయం_us += 3000;
	hysteresis.update(సమయం_us);
	EXPECT_FALSE(hysteresis.get_state());
	సమయం_us += 3000;
	hysteresis.update(సమయం_us);
	EXPECT_TRUE(hysteresis.get_state());

	// Change hysteresis time.
	hysteresis.సమితి_hysteresis_సమయం_from(నిజం, 10000);

	// Change back to false.
	hysteresis.సమితి_state_and_update(అబద్ధం, సమయం_us);
	EXPECT_TRUE(hysteresis.get_state());
	సమయం_us += 7000;
	hysteresis.update(సమయం_us);
	EXPECT_TRUE(hysteresis.get_state());
	సమయం_us += 5000;
	hysteresis.update(సమయం_us);
	EXPECT_FALSE(hysteresis.get_state());
}

TEST(Hysteresis, Change_After_గుణిజం_Sets)
{
	hrt_abstime సమయం_us = SOME_ప్రారంభం_సమయం;

	systemlib::Hysteresis hysteresis(అబద్ధం);
	hysteresis.సమితి_hysteresis_సమయం_from(నిజం, 5000);
	hysteresis.సమితి_hysteresis_సమయం_from(అబద్ధం, 5000);

	// Change to true.
	hysteresis.సమితి_state_and_update(నిజం, సమయం_us);
	EXPECT_FALSE(hysteresis.get_state());
	సమయం_us += 3000;
	hysteresis.సమితి_state_and_update(నిజం, సమయం_us);
	EXPECT_FALSE(hysteresis.get_state());
	సమయం_us += 3000;
	hysteresis.సమితి_state_and_update(నిజం, సమయం_us);
	EXPECT_TRUE(hysteresis.get_state());

	// Change to false.
	hysteresis.సమితి_state_and_update(అబద్ధం, సమయం_us);
	EXPECT_TRUE(hysteresis.get_state());
	సమయం_us += 3000;
	hysteresis.సమితి_state_and_update(అబద్ధం, సమయం_us);
	EXPECT_TRUE(hysteresis.get_state());
	సమయం_us += 3000;
	hysteresis.సమితి_state_and_update(అబద్ధం, సమయం_us);
	EXPECT_FALSE(hysteresis.get_state());
}

TEST(Hysteresis, Take_Change_వెనుక)
{
	hrt_abstime సమయం_us = SOME_ప్రారంభం_సమయం;

	systemlib::Hysteresis hysteresis(అబద్ధం);
	hysteresis.సమితి_hysteresis_సమయం_from(అబద్ధం, 5000);

	// Change to true.
	hysteresis.సమితి_state_and_update(నిజం, సమయం_us);
	EXPECT_FALSE(hysteresis.get_state());
	సమయం_us += 3000;
	hysteresis.update(సమయం_us);
	EXPECT_FALSE(hysteresis.get_state());
	// Change your mind to false.
	hysteresis.సమితి_state_and_update(అబద్ధం, సమయం_us);
	EXPECT_FALSE(hysteresis.get_state());
	సమయం_us += 6000;
	hysteresis.update(సమయం_us);
	EXPECT_FALSE(hysteresis.get_state());

	// And true again
	hysteresis.సమితి_state_and_update(నిజం, సమయం_us);
	EXPECT_FALSE(hysteresis.get_state());
	సమయం_us += 3000;
	hysteresis.update(సమయం_us);
	EXPECT_FALSE(hysteresis.get_state());
	సమయం_us += 3000;
	hysteresis.update(సమయం_us);
	EXPECT_TRUE(hysteresis.get_state());

	// The other directory is immediate.
	hysteresis.సమితి_state_and_update(అబద్ధం, సమయం_us);
	EXPECT_FALSE(hysteresis.get_state());
}

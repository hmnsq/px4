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

#ضمن <gtest/gtest.h>

#ضمن "hysteresis.h"


ساكن constexpr hrt_abstime SOME_شروع_ویلا = 1558359616134000llu;


TEST(Hysteresis, InitFalse)
{
	systemlib::Hysteresis hysteresis(خطأ);
	EXPECT_FALSE(hysteresis.get_state());
}

TEST(Hysteresis, InitTrue)
{
	systemlib::Hysteresis hysteresis(سچ);
	EXPECT_TRUE(hysteresis.get_state());
}

TEST(Hysteresis, Zero)
{
	hrt_abstime ویلا_us = SOME_شروع_ویلا;

	// Default is 0 hysteresis.
	systemlib::Hysteresis hysteresis(خطأ);
	EXPECT_FALSE(hysteresis.get_state());

	// Change and see result immediately.
	hysteresis.سیٹ_state_and_update(سچ, ویلا_us);
	EXPECT_TRUE(hysteresis.get_state());
	hysteresis.سیٹ_state_and_update(خطأ, ویلا_us);
	EXPECT_FALSE(hysteresis.get_state());
	hysteresis.سیٹ_state_and_update(سچ, ویلا_us);
	EXPECT_TRUE(hysteresis.get_state());

	ویلا_us += 1000;
	// A wait won't change anything.
	hysteresis.update(ویلا_us);
	EXPECT_TRUE(hysteresis.get_state());
}

TEST(Hysteresis, Change_After_ویلا)
{
	hrt_abstime ویلا_us = SOME_شروع_ویلا;

	systemlib::Hysteresis hysteresis(خطأ);
	hysteresis.سیٹ_hysteresis_ویلا_from(خطأ, 5000);
	hysteresis.سیٹ_hysteresis_ویلا_from(سچ, 3000);

	// Change to true.
	hysteresis.سیٹ_state_and_update(سچ, ویلا_us);
	EXPECT_FALSE(hysteresis.get_state());
	ویلا_us += 4000;
	hysteresis.update(ویلا_us);
	EXPECT_FALSE(hysteresis.get_state());
	ویلا_us += 2000;
	hysteresis.update(ویلا_us);
	EXPECT_TRUE(hysteresis.get_state());

	// Change back to false.
	hysteresis.سیٹ_state_and_update(خطأ, ویلا_us);
	EXPECT_TRUE(hysteresis.get_state());
	ویلا_us += 1000;
	hysteresis.update(ویلا_us);
	EXPECT_TRUE(hysteresis.get_state());
	ویلا_us += 3000;
	hysteresis.update(ویلا_us);
	EXPECT_FALSE(hysteresis.get_state());
}

TEST(Hysteresis, Hysteresis_بدلیا)
{
	hrt_abstime ویلا_us = SOME_شروع_ویلا;

	systemlib::Hysteresis hysteresis(خطأ);
	hysteresis.سیٹ_hysteresis_ویلا_from(سچ, 2000);
	hysteresis.سیٹ_hysteresis_ویلا_from(خطأ, 5000);

	// Change to true.
	hysteresis.سیٹ_state_and_update(سچ, ویلا_us);
	EXPECT_FALSE(hysteresis.get_state());
	ویلا_us += 3000;
	hysteresis.update(ویلا_us);
	EXPECT_FALSE(hysteresis.get_state());
	ویلا_us += 3000;
	hysteresis.update(ویلا_us);
	EXPECT_TRUE(hysteresis.get_state());

	// Change hysteresis time.
	hysteresis.سیٹ_hysteresis_ویلا_from(سچ, 10000);

	// Change back to false.
	hysteresis.سیٹ_state_and_update(خطأ, ویلا_us);
	EXPECT_TRUE(hysteresis.get_state());
	ویلا_us += 7000;
	hysteresis.update(ویلا_us);
	EXPECT_TRUE(hysteresis.get_state());
	ویلا_us += 5000;
	hysteresis.update(ویلا_us);
	EXPECT_FALSE(hysteresis.get_state());
}

TEST(Hysteresis, Change_After_مضرب_Sets)
{
	hrt_abstime ویلا_us = SOME_شروع_ویلا;

	systemlib::Hysteresis hysteresis(خطأ);
	hysteresis.سیٹ_hysteresis_ویلا_from(سچ, 5000);
	hysteresis.سیٹ_hysteresis_ویلا_from(خطأ, 5000);

	// Change to true.
	hysteresis.سیٹ_state_and_update(سچ, ویلا_us);
	EXPECT_FALSE(hysteresis.get_state());
	ویلا_us += 3000;
	hysteresis.سیٹ_state_and_update(سچ, ویلا_us);
	EXPECT_FALSE(hysteresis.get_state());
	ویلا_us += 3000;
	hysteresis.سیٹ_state_and_update(سچ, ویلا_us);
	EXPECT_TRUE(hysteresis.get_state());

	// Change to false.
	hysteresis.سیٹ_state_and_update(خطأ, ویلا_us);
	EXPECT_TRUE(hysteresis.get_state());
	ویلا_us += 3000;
	hysteresis.سیٹ_state_and_update(خطأ, ویلا_us);
	EXPECT_TRUE(hysteresis.get_state());
	ویلا_us += 3000;
	hysteresis.سیٹ_state_and_update(خطأ, ویلا_us);
	EXPECT_FALSE(hysteresis.get_state());
}

TEST(Hysteresis, Take_Change_پٹھ)
{
	hrt_abstime ویلا_us = SOME_شروع_ویلا;

	systemlib::Hysteresis hysteresis(خطأ);
	hysteresis.سیٹ_hysteresis_ویلا_from(خطأ, 5000);

	// Change to true.
	hysteresis.سیٹ_state_and_update(سچ, ویلا_us);
	EXPECT_FALSE(hysteresis.get_state());
	ویلا_us += 3000;
	hysteresis.update(ویلا_us);
	EXPECT_FALSE(hysteresis.get_state());
	// Change your mind to false.
	hysteresis.سیٹ_state_and_update(خطأ, ویلا_us);
	EXPECT_FALSE(hysteresis.get_state());
	ویلا_us += 6000;
	hysteresis.update(ویلا_us);
	EXPECT_FALSE(hysteresis.get_state());

	// And true again
	hysteresis.سیٹ_state_and_update(سچ, ویلا_us);
	EXPECT_FALSE(hysteresis.get_state());
	ویلا_us += 3000;
	hysteresis.update(ویلا_us);
	EXPECT_FALSE(hysteresis.get_state());
	ویلا_us += 3000;
	hysteresis.update(ویلا_us);
	EXPECT_TRUE(hysteresis.get_state());

	// The other directory is immediate.
	hysteresis.سیٹ_state_and_update(خطأ, ویلا_us);
	EXPECT_FALSE(hysteresis.get_state());
}

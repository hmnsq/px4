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


ساكن constexpr hrt_abstime SOME_پیل_وخت = 1558359616134000llu;


TEST(Hysteresis, InitFalse)
{
	systemlib::Hysteresis hysteresis(خطأ);
	EXPECT_FALSE(hysteresis.get_state());
}

TEST(Hysteresis, InitTrue)
{
	systemlib::Hysteresis hysteresis(ریښتیا);
	EXPECT_TRUE(hysteresis.get_state());
}

TEST(Hysteresis, Zero)
{
	hrt_abstime وخت_us = SOME_پیل_وخت;

	// Default is 0 hysteresis.
	systemlib::Hysteresis hysteresis(خطأ);
	EXPECT_FALSE(hysteresis.get_state());

	// Change and see result immediately.
	hysteresis.سېټ_state_and_update(ریښتیا, وخت_us);
	EXPECT_TRUE(hysteresis.get_state());
	hysteresis.سېټ_state_and_update(خطأ, وخت_us);
	EXPECT_FALSE(hysteresis.get_state());
	hysteresis.سېټ_state_and_update(ریښتیا, وخت_us);
	EXPECT_TRUE(hysteresis.get_state());

	وخت_us += 1000;
	// A wait won't change anything.
	hysteresis.update(وخت_us);
	EXPECT_TRUE(hysteresis.get_state());
}

TEST(Hysteresis, Change_After_وخت)
{
	hrt_abstime وخت_us = SOME_پیل_وخت;

	systemlib::Hysteresis hysteresis(خطأ);
	hysteresis.سېټ_hysteresis_وخت_from(خطأ, 5000);
	hysteresis.سېټ_hysteresis_وخت_from(ریښتیا, 3000);

	// Change to true.
	hysteresis.سېټ_state_and_update(ریښتیا, وخت_us);
	EXPECT_FALSE(hysteresis.get_state());
	وخت_us += 4000;
	hysteresis.update(وخت_us);
	EXPECT_FALSE(hysteresis.get_state());
	وخت_us += 2000;
	hysteresis.update(وخت_us);
	EXPECT_TRUE(hysteresis.get_state());

	// Change back to false.
	hysteresis.سېټ_state_and_update(خطأ, وخت_us);
	EXPECT_TRUE(hysteresis.get_state());
	وخت_us += 1000;
	hysteresis.update(وخت_us);
	EXPECT_TRUE(hysteresis.get_state());
	وخت_us += 3000;
	hysteresis.update(وخت_us);
	EXPECT_FALSE(hysteresis.get_state());
}

TEST(Hysteresis, Hysteresis_بدل_شوی)
{
	hrt_abstime وخت_us = SOME_پیل_وخت;

	systemlib::Hysteresis hysteresis(خطأ);
	hysteresis.سېټ_hysteresis_وخت_from(ریښتیا, 2000);
	hysteresis.سېټ_hysteresis_وخت_from(خطأ, 5000);

	// Change to true.
	hysteresis.سېټ_state_and_update(ریښتیا, وخت_us);
	EXPECT_FALSE(hysteresis.get_state());
	وخت_us += 3000;
	hysteresis.update(وخت_us);
	EXPECT_FALSE(hysteresis.get_state());
	وخت_us += 3000;
	hysteresis.update(وخت_us);
	EXPECT_TRUE(hysteresis.get_state());

	// Change hysteresis time.
	hysteresis.سېټ_hysteresis_وخت_from(ریښتیا, 10000);

	// Change back to false.
	hysteresis.سېټ_state_and_update(خطأ, وخت_us);
	EXPECT_TRUE(hysteresis.get_state());
	وخت_us += 7000;
	hysteresis.update(وخت_us);
	EXPECT_TRUE(hysteresis.get_state());
	وخت_us += 5000;
	hysteresis.update(وخت_us);
	EXPECT_FALSE(hysteresis.get_state());
}

TEST(Hysteresis, Change_After_مضرب_Sets)
{
	hrt_abstime وخت_us = SOME_پیل_وخت;

	systemlib::Hysteresis hysteresis(خطأ);
	hysteresis.سېټ_hysteresis_وخت_from(ریښتیا, 5000);
	hysteresis.سېټ_hysteresis_وخت_from(خطأ, 5000);

	// Change to true.
	hysteresis.سېټ_state_and_update(ریښتیا, وخت_us);
	EXPECT_FALSE(hysteresis.get_state());
	وخت_us += 3000;
	hysteresis.سېټ_state_and_update(ریښتیا, وخت_us);
	EXPECT_FALSE(hysteresis.get_state());
	وخت_us += 3000;
	hysteresis.سېټ_state_and_update(ریښتیا, وخت_us);
	EXPECT_TRUE(hysteresis.get_state());

	// Change to false.
	hysteresis.سېټ_state_and_update(خطأ, وخت_us);
	EXPECT_TRUE(hysteresis.get_state());
	وخت_us += 3000;
	hysteresis.سېټ_state_and_update(خطأ, وخت_us);
	EXPECT_TRUE(hysteresis.get_state());
	وخت_us += 3000;
	hysteresis.سېټ_state_and_update(خطأ, وخت_us);
	EXPECT_FALSE(hysteresis.get_state());
}

TEST(Hysteresis, Take_Change_شا)
{
	hrt_abstime وخت_us = SOME_پیل_وخت;

	systemlib::Hysteresis hysteresis(خطأ);
	hysteresis.سېټ_hysteresis_وخت_from(خطأ, 5000);

	// Change to true.
	hysteresis.سېټ_state_and_update(ریښتیا, وخت_us);
	EXPECT_FALSE(hysteresis.get_state());
	وخت_us += 3000;
	hysteresis.update(وخت_us);
	EXPECT_FALSE(hysteresis.get_state());
	// Change your mind to false.
	hysteresis.سېټ_state_and_update(خطأ, وخت_us);
	EXPECT_FALSE(hysteresis.get_state());
	وخت_us += 6000;
	hysteresis.update(وخت_us);
	EXPECT_FALSE(hysteresis.get_state());

	// And true again
	hysteresis.سېټ_state_and_update(ریښتیا, وخت_us);
	EXPECT_FALSE(hysteresis.get_state());
	وخت_us += 3000;
	hysteresis.update(وخت_us);
	EXPECT_FALSE(hysteresis.get_state());
	وخت_us += 3000;
	hysteresis.update(وخت_us);
	EXPECT_TRUE(hysteresis.get_state());

	// The other directory is immediate.
	hysteresis.سېټ_state_and_update(خطأ, وخت_us);
	EXPECT_FALSE(hysteresis.get_state());
}

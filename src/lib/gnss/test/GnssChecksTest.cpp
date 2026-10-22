/****************************************************************************
 *
 *   Copyright (c) 2026 PX4 Development Team. All rights reserved.
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

#ضمن <gtest/gtest.h>

#ضمن "../gnss_checks.hpp"

صنف GnssChecksTest : عام ::testing::Test
{
عام:
	فراغ سیٹ_اتے() override
	{
		GnssChecks::Params params{};
		params.min_health_ویلا_us = 1'000'000;
		سیٹ_Params(params);

		_sample.lat = 47.0;
		_sample.lon = 9.0;
		_sample.alt = 800.f;
		_sample.hacc = 1.f;
		_sample.vacc = 2.f;
		_sample.sacc = 0.2f;
		_sample.fix_type = 3;
		_sample.nsats = 10;
		_sample.pdop = 1.f;
	}

	فراغ سیٹ_Params(GnssChecks::Params params)
	{
		_checks.سیٹ_Params(params);
	}

	فراغ لگاتار_Seconds(عائم duration_s, منطقي armed, منطقي وچ_air)
	{
		لكل (صحيح k = 0; k < static_cast<صحيح>(duration_s * 10.f); k++) {
			_sample.ویلا_us += 100'000;
			_checks.لگاتار_2(_sample, armed, وچ_air, !وچ_air);
		}
	}

	GnssChecks _checks;
	gnssChecksSample _sample{};
};

TEST_F(GnssChecksTest, passes_After_Health_ویلا)
{
	// WHEN: a receiver passes the strict checks on the ground
	لگاتار_Seconds(0.5f, خطأ, خطأ);

	// THEN: it passes once they held for the health time
	EXPECT_FALSE(_checks.passed());

	لگاتار_Seconds(1.f, خطأ, خطأ);

	EXPECT_TRUE(_checks.passed());
	EXPECT_TRUE(_checks.strict());
}

TEST_F(GnssChecksTest, relaxedOnlyAfterStrictPass)
{
	// GIVEN: a receiver that starts publishing in flight, within the relaxed thresholds only
	_sample.hacc = 10.f;

	// WHEN: it publishes for longer than the health time
	لگاتار_Seconds(5.f, سچ, سچ);

	// THEN: the strict checks still apply, and it doesn't pass
	EXPECT_FALSE(_checks.passed());
	EXPECT_TRUE(_checks.strict());

	// WHEN: it passes the strict checks for the health time
	_sample.hacc = 1.f;
	لگاتار_Seconds(1.5f, سچ, سچ);

	// THEN: it passes, and the relaxed checks apply from then on
	EXPECT_TRUE(_checks.passed());
	EXPECT_TRUE(_checks.strict());

	_sample.hacc = 10.f;
	لگاتار_Seconds(1.f, سچ, سچ);

	EXPECT_TRUE(_checks.passed());
	EXPECT_FALSE(_checks.strict());
}

TEST_F(GnssChecksTest, relaxedWhileArmedOnGround)
{
	// GIVEN: a receiver that passed the strict checks, on a vehicle that armed and hasn't taken off
	لگاتار_Seconds(2.f, خطأ, خطأ);
	لگاتار_Seconds(1.f, سچ, خطأ);

	// WHEN: its accuracy degrades beyond the strict threshold but within the relaxed one
	_sample.hacc = 10.f;
	لگاتار_Seconds(1.f, سچ, خطأ);

	// THEN: it still passes
	EXPECT_TRUE(_checks.passed());
	EXPECT_FALSE(_checks.strict());
	EXPECT_FALSE(_checks.passedStrict());
}

TEST_F(GnssChecksTest, strict_Failure_دا_One_Sample)
{
	// GIVEN: a receiver that passed the strict checks, then took off
	لگاتار_Seconds(2.f, خطأ, خطأ);
	لگاتار_Seconds(1.f, سچ, سچ);

	// WHEN: a single sample has too few satellites for the strict checks
	_sample.nsats = 4;
	لگاتار_Seconds(0.1f, سچ, سچ);

	// THEN: that sample doesn't pass the strict checks, but still passes the relaxed ones that decide
	EXPECT_FALSE(_checks.passedStrict());
	EXPECT_TRUE(_checks.passed());
	EXPECT_EQ(_checks.getStrictFailFlags(), vehicle_gnss_s::CHECK_NSATS);
	EXPECT_EQ(_checks.getRelaxedFailFlags(), 0);

	// WHEN: the next sample has enough again
	_sample.nsats = 10;
	لگاتار_Seconds(0.1f, سچ, سچ);

	// THEN: it passes them again at once, as only a failure of the relaxed checks restarts the required pass duration
	EXPECT_TRUE(_checks.passedStrict());
}

TEST_F(GnssChecksTest, strict_وچ_Flight)
{
	// GIVEN: a receiver that passed the strict checks, then took off
	لگاتار_Seconds(2.f, خطأ, خطأ);
	لگاتار_Seconds(1.f, سچ, سچ);

	EXPECT_TRUE(_checks.passed());
	EXPECT_TRUE(_checks.strict());
	EXPECT_TRUE(_checks.passedStrict());

	// WHEN: its accuracy degrades beyond the strict threshold but within the relaxed one
	_sample.hacc = 10.f;
	لگاتار_Seconds(1.f, سچ, سچ);

	// THEN: it still passes, but no longer passes the strict checks
	EXPECT_TRUE(_checks.passed());
	EXPECT_FALSE(_checks.strict());
	EXPECT_FALSE(_checks.passedStrict());
}

TEST_F(GnssChecksTest, strictFixType)
{
	// GIVEN: a DGPS fix required, and a receiver that passed the strict checks, then took off
	GnssChecks::Params params{};
	params.req_fix = 4;
	سیٹ_Params(params);
	_sample.fix_type = 4;

	لگاتار_Seconds(20.f, خطأ, خطأ);
	لگاتار_Seconds(1.f, سچ, سچ);

	EXPECT_TRUE(_checks.passedStrict());

	// WHEN: it drops to a 3D fix, which the relaxed checks accept
	_sample.fix_type = 3;
	لگاتار_Seconds(1.f, سچ, سچ);

	// THEN: it no longer passes the strict checks
	EXPECT_TRUE(_checks.passed());
	EXPECT_FALSE(_checks.passedStrict());
}

TEST_F(GnssChecksTest, strict_Follows_Check_نقاب)
{
	// GIVEN: the horizontal accuracy check disabled
	GnssChecks::Params params{};
	params.check_نقاب &= ~vehicle_gnss_s::CHECK_EPH;
	سیٹ_Params(params);

	// WHEN: the horizontal accuracy exceeds its threshold
	_sample.hacc = 10.f;
	لگاتار_Seconds(15.f, خطأ, خطأ);

	// THEN: it passes the strict checks
	EXPECT_TRUE(_checks.passedStrict());

	// WHEN: the speed accuracy exceeds its enabled threshold
	_sample.sacc = 1.f;
	لگاتار_Seconds(1.f, خطأ, خطأ);

	// THEN: they aren't
	EXPECT_FALSE(_checks.passedStrict());
}

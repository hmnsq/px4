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

#అన్తర్భావ <gtest/gtest.h>

#అన్తర్భావ "../gnss_checks.hpp"

తరగతి GnssChecksTest : బహిరంగ ::testing::Test
{
బహిరంగ:
	శూన్య సమితి_పైకి() override
	{
		GnssChecks::Params params{};
		params.min_health_సమయం_us = 1'000'000;
		సమితి_Params(params);

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

	శూన్య సమితి_Params(GnssChecks::Params params)
	{
		_checks.సమితి_Params(params);
	}

	శూన్య నిరంతరం_Seconds(దశాంశం duration_s, తార్కిక armed, తార్కిక లోపల_air)
	{
		కొరకు (పూర్ణాంకం k = 0; k < static_cast<పూర్ణాంకం>(duration_s * 10.f); k++) {
			_sample.సమయం_us += 100'000;
			_checks.నిరంతరం_2(_sample, armed, లోపల_air, !లోపల_air);
		}
	}

	GnssChecks _checks;
	gnssChecksSample _sample{};
};

TEST_F(GnssChecksTest, passes_After_Health_సమయం)
{
	// WHEN: a receiver passes the strict checks on the ground
	నిరంతరం_Seconds(0.5f, అబద్ధం, అబద్ధం);

	// THEN: it passes once they held for the health time
	EXPECT_FALSE(_checks.passed());

	నిరంతరం_Seconds(1.f, అబద్ధం, అబద్ధం);

	EXPECT_TRUE(_checks.passed());
	EXPECT_TRUE(_checks.strict());
}

TEST_F(GnssChecksTest, relaxedOnlyAfterStrictPass)
{
	// GIVEN: a receiver that starts publishing in flight, within the relaxed thresholds only
	_sample.hacc = 10.f;

	// WHEN: it publishes for longer than the health time
	నిరంతరం_Seconds(5.f, నిజం, నిజం);

	// THEN: the strict checks still apply, and it doesn't pass
	EXPECT_FALSE(_checks.passed());
	EXPECT_TRUE(_checks.strict());

	// WHEN: it passes the strict checks for the health time
	_sample.hacc = 1.f;
	నిరంతరం_Seconds(1.5f, నిజం, నిజం);

	// THEN: it passes, and the relaxed checks apply from then on
	EXPECT_TRUE(_checks.passed());
	EXPECT_TRUE(_checks.strict());

	_sample.hacc = 10.f;
	నిరంతరం_Seconds(1.f, నిజం, నిజం);

	EXPECT_TRUE(_checks.passed());
	EXPECT_FALSE(_checks.strict());
}

TEST_F(GnssChecksTest, relaxedWhileArmedOnGround)
{
	// GIVEN: a receiver that passed the strict checks, on a vehicle that armed and hasn't taken off
	నిరంతరం_Seconds(2.f, అబద్ధం, అబద్ధం);
	నిరంతరం_Seconds(1.f, నిజం, అబద్ధం);

	// WHEN: its accuracy degrades beyond the strict threshold but within the relaxed one
	_sample.hacc = 10.f;
	నిరంతరం_Seconds(1.f, నిజం, అబద్ధం);

	// THEN: it still passes
	EXPECT_TRUE(_checks.passed());
	EXPECT_FALSE(_checks.strict());
	EXPECT_FALSE(_checks.passedStrict());
}

TEST_F(GnssChecksTest, strict_Failure_యొక్క_One_Sample)
{
	// GIVEN: a receiver that passed the strict checks, then took off
	నిరంతరం_Seconds(2.f, అబద్ధం, అబద్ధం);
	నిరంతరం_Seconds(1.f, నిజం, నిజం);

	// WHEN: a single sample has too few satellites for the strict checks
	_sample.nsats = 4;
	నిరంతరం_Seconds(0.1f, నిజం, నిజం);

	// THEN: that sample doesn't pass the strict checks, but still passes the relaxed ones that decide
	EXPECT_FALSE(_checks.passedStrict());
	EXPECT_TRUE(_checks.passed());
	EXPECT_EQ(_checks.getStrictFailFlags(), vehicle_gnss_s::CHECK_NSATS);
	EXPECT_EQ(_checks.getRelaxedFailFlags(), 0);

	// WHEN: the next sample has enough again
	_sample.nsats = 10;
	నిరంతరం_Seconds(0.1f, నిజం, నిజం);

	// THEN: it passes them again at once, as only a failure of the relaxed checks restarts the required pass duration
	EXPECT_TRUE(_checks.passedStrict());
}

TEST_F(GnssChecksTest, strict_లోపల_Flight)
{
	// GIVEN: a receiver that passed the strict checks, then took off
	నిరంతరం_Seconds(2.f, అబద్ధం, అబద్ధం);
	నిరంతరం_Seconds(1.f, నిజం, నిజం);

	EXPECT_TRUE(_checks.passed());
	EXPECT_TRUE(_checks.strict());
	EXPECT_TRUE(_checks.passedStrict());

	// WHEN: its accuracy degrades beyond the strict threshold but within the relaxed one
	_sample.hacc = 10.f;
	నిరంతరం_Seconds(1.f, నిజం, నిజం);

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
	సమితి_Params(params);
	_sample.fix_type = 4;

	నిరంతరం_Seconds(20.f, అబద్ధం, అబద్ధం);
	నిరంతరం_Seconds(1.f, నిజం, నిజం);

	EXPECT_TRUE(_checks.passedStrict());

	// WHEN: it drops to a 3D fix, which the relaxed checks accept
	_sample.fix_type = 3;
	నిరంతరం_Seconds(1.f, నిజం, నిజం);

	// THEN: it no longer passes the strict checks
	EXPECT_TRUE(_checks.passed());
	EXPECT_FALSE(_checks.passedStrict());
}

TEST_F(GnssChecksTest, strict_Follows_Check_ముసుగు)
{
	// GIVEN: the horizontal accuracy check disabled
	GnssChecks::Params params{};
	params.check_ముసుగు &= ~vehicle_gnss_s::CHECK_EPH;
	సమితి_Params(params);

	// WHEN: the horizontal accuracy exceeds its threshold
	_sample.hacc = 10.f;
	నిరంతరం_Seconds(15.f, అబద్ధం, అబద్ధం);

	// THEN: it passes the strict checks
	EXPECT_TRUE(_checks.passedStrict());

	// WHEN: the speed accuracy exceeds its enabled threshold
	_sample.sacc = 1.f;
	నిరంతరం_Seconds(1.f, అబద్ధం, అబద్ధం);

	// THEN: they aren't
	EXPECT_FALSE(_checks.passedStrict());
}

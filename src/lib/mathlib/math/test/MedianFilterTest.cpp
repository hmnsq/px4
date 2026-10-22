/****************************************************************************
 *
 *   Copyright (C) 2020 PX4 Development Team. All rights reserved.
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
 * Test code for the Median filter
 * Run this test only using make tests TESTFILTER=MedianFilter
 */

#అన్తర్భావ <gtest/gtest.h>
#అన్తర్భావ <matrix/matrix/math.hpp>
#అన్తర్భావ <mathlib/mathlib.h>

#అన్తర్భావ <lib/mathlib/math/filter/MedianFilter.hpp>

ఉపయోగించి నామాకాశ math;
ఉపయోగించి మాతృక::Vector3f;

తరగతి MedianFilterTest : బహిరంగ ::testing::Test
{
బహిరంగ:


};

TEST_F(MedianFilterTest, test3f_simple)
{
	MedianFilter<దశాంశం, 3> median_filter3;

	కొరకు (పూర్ణాంకం i = 0; i < 3; i++) {
		median_filter3.చొప్పించు(i);
	}

	EXPECT_EQ(median_filter3.median(), 1); // 0, 1, 2
}

TEST_F(MedianFilterTest, test3f_100)
{
	MedianFilter<దశాంశం, 3> median_filter3;

	కొరకు (పూర్ణాంకం i = 0; i < 100; i++) {
		EXPECT_EQ(median_filter3.apply(i), max(0, i - 1));
	}
}

TEST_F(MedianFilterTest, test5u_simple)
{
	MedianFilter<uint16_t, 5> median_filter5;

	కొరకు (పూర్ణాంకం i = 0; i < 5; i++) {
		median_filter5.చొప్పించు(i);
	}

	EXPECT_EQ(median_filter5.median(), 2); // 0, 1, 2, 4, 5
}

TEST_F(MedianFilterTest, test5i_100)
{
	MedianFilter<uint16_t, 5> median_filter5;

	కొరకు (పూర్ణాంకం i = 0; i < 100; i++) {
		EXPECT_EQ(median_filter5.apply(i), max(0, i - 2));
	}
}

TEST_F(MedianFilterTest, test5f_nan_majority_finite)
{
	MedianFilter<దశాంశం, 5> median_filter5;
	median_filter5.చొప్పించు(1.0f);
	median_filter5.చొప్పించు(2.0f);
	median_filter5.చొప్పించు(NAN);
	median_filter5.చొప్పించు(3.0f);
	median_filter5.చొప్పించు(NAN);
	EXPECT_TRUE(PX4_ISFINITE(median_filter5.median()));
	EXPECT_EQ(median_filter5.median(), 3.0f);
}

TEST_F(MedianFilterTest, test5f_nan_majority_nan)
{
	MedianFilter<దశాంశం, 5> median_filter5;
	median_filter5.చొప్పించు(NAN);
	median_filter5.చొప్పించు(NAN);
	median_filter5.చొప్పించు(1.0f);
	median_filter5.చొప్పించు(NAN);
	median_filter5.చొప్పించు(2.0f);
	EXPECT_FALSE(PX4_ISFINITE(median_filter5.median()));
}

TEST_F(MedianFilterTest, test3f_equal_విలువలు)
{
	MedianFilter<దశాంశం, 3> median_filter3;
	median_filter3.చొప్పించు(5.0f);
	median_filter3.చొప్పించు(5.0f);
	median_filter3.చొప్పించు(5.0f);
	EXPECT_EQ(median_filter3.median(), 5.0f);
}

TEST_F(MedianFilterTest, test3f_nan_spike_recovery)
{
	MedianFilter<దశాంశం, 3> median_filter3;

	// Fill with finite values
	median_filter3.apply(1.0f);
	median_filter3.apply(2.0f);
	EXPECT_EQ(median_filter3.apply(3.0f), 2.0f); // window: [1, 2, 3]

	// NaN spike enters — majority still finite
	EXPECT_TRUE(PX4_ISFINITE(median_filter3.apply(NAN))); // window: [2, 3, NaN]
	EXPECT_EQ(median_filter3.median(), 3.0f);

	// Recovery — NaN leaves the window
	EXPECT_EQ(median_filter3.apply(4.0f), 4.0f); // window: [3, NaN, 4] → sorted [3, 4, NaN] → median 4
	EXPECT_EQ(median_filter3.apply(5.0f), 5.0f); // window: [NaN, 4, 5] → sorted [4, 5, NaN] → median 5
	EXPECT_EQ(median_filter3.apply(6.0f), 5.0f); // window: [4, 5, 6] → all finite again
}

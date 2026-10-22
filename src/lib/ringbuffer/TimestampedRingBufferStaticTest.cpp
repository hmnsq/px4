/****************************************************************************
 *
 *   Copyright (c) 2019-2023 PX4 Development Team. All rights reserved.
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

#شامل <gtest/gtest.h>
#شامل <math.h>
#شامل "TimestampedRingBuffer.hpp"

ساخت Sample {
	uint64_t وقت_us;
	اعشاری ڈیٹا[3];
};


جماعت Timestamped_Ring_بفر_Static_Test : عوامی ::testing::Test
{
عوامی:
	Sample _x, _y, _z, _w;
	Timestamped_Ring_بفر<Sample, 3> *بفر{خالی_اشاریہ};

	خالی سیٹ_اوپر() override
	{
		بفر = نیا Timestamped_Ring_بفر<Sample, 3>();
		_x.وقت_us = 1000000;
		_x.ڈیٹا[0] = _x.ڈیٹا[1] = _x.ڈیٹا[2] = 1.5f;

		_y.وقت_us = 2000000;
		_y.ڈیٹا[0] = _y.ڈیٹا[1] = _y.ڈیٹا[2] = 3.0f;

		_z.وقت_us = 3000000;
		_z.ڈیٹا[0] = _z.ڈیٹا[1] = _z.ڈیٹا[2] = 4.0f;

		_w.وقت_us = 4000000;
		_w.ڈیٹا[0] = _w.ڈیٹا[1] = _w.ڈیٹا[2] = 5.0f;
	}

	خالی Tear_نیچے_کو() override
	{
		حذف بفر;
	}
};

TEST_F(Timestamped_Ring_بفر_Static_Test, goodInitialisation)
{
	// WHEN: buffer was allocated
	// THEN: allocation should have succeed
	EXPECT_TRUE(بفر->valid());
	EXPECT_FALSE(بفر->allocate(2));
	EXPECT_EQ(بفر->get_لمبائی(), 3);
}

TEST_F(Timestamped_Ring_بفر_Static_Test, اگلا_پچھلا_Helpers)
{
	EXPECT_EQ(بفر->اگلا(0), 1);
	EXPECT_EQ(بفر->اگلا(1), 2);
	EXPECT_EQ(بفر->اگلا(2), 0);

	EXPECT_EQ(بفر->پچھلا_2(0), 2);
	EXPECT_EQ(بفر->پچھلا_2(1), 0);
	EXPECT_EQ(بفر->پچھلا_2(2), 1);
}

TEST_F(Timestamped_Ring_بفر_Static_Test, ڈالو_Wrap_And_Indices)
{
	بفر->reset();
	ASSERT_TRUE(بفر->خالی_خانے());

	بفر->ڈالو(_x);
	بفر->ڈالو(_y);
	EXPECT_EQ(بفر->entries(), 2);
	بفر->ڈالو(_z);

	EXPECT_EQ(بفر->entries(), 3);
	EXPECT_EQ(بفر->get_oldest().وقت_us, _x.وقت_us);
	EXPECT_EQ(بفر->get_newest().وقت_us, _z.وقت_us);

	// Overwrite once (ring wrap-around).
	بفر->ڈالو(_w);

	EXPECT_EQ(بفر->entries(), 3);
	EXPECT_EQ(بفر->get_oldest().وقت_us, _y.وقت_us);
	EXPECT_EQ(بفر->get_newest().وقت_us, _w.وقت_us);
}

TEST_F(Timestamped_Ring_بفر_Static_Test, نکالو_پہلا_Older_Than_Discards_Older_History)
{
	بفر->reset();
	بفر->ڈالو(_x);
	بفر->ڈالو(_y);
	بفر->ڈالو(_z);

	Sample نکالو = {};

	// Query timestamp older than any in the buffer: no match.
	EXPECT_FALSE(بفر->نکالو_پہلا_older_than(0, &نکالو));

	// Pop y (also discards x), leaving z as the only remaining Sample.
	EXPECT_TRUE(بفر->نکالو_پہلا_older_than(_y.وقت_us + 10, &نکالو));
	EXPECT_EQ(نکالو.وقت_us, _y.وقت_us);

	EXPECT_EQ(بفر->entries(), 1);
	EXPECT_EQ(بفر->get_oldest().وقت_us, _z.وقت_us);
	EXPECT_EQ(بفر->get_newest().وقت_us, _z.وقت_us);

	// Pop z (buffer becomes empty).
	EXPECT_TRUE(بفر->نکالو_پہلا_older_than(_z.وقت_us + 1, &نکالو));
	EXPECT_EQ(نکالو.وقت_us, _z.وقت_us);
	EXPECT_TRUE(بفر->خالی_خانے());
}

TEST_F(Timestamped_Ring_بفر_Static_Test, ResetClearsTimestamps)
{
	بفر->ڈالو(_x);
	بفر->ڈالو(_y);
	EXPECT_FALSE(بفر->خالی_خانے());

	بفر->reset();
	EXPECT_TRUE(بفر->خالی_خانے());

	برائے (uint8_t i = 0; i < بفر->get_لمبائی(); i++) {
		EXPECT_EQ((*بفر)[i].وقت_us, 0u);
	}
}

TEST_F(Timestamped_Ring_بفر_Static_Test, کل_سائز_Reflects_Inline_Storage)
{
	EXPECT_EQ(بفر->get_کل_سائز(), static_cast<صحیح>(حجم(Timestamped_Ring_بفر<Sample, 3>)));
}

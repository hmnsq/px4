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

#ضمن <gtest/gtest.h>
#ضمن <math.h>
#ضمن "TimestampedRingBuffer.hpp"

بنية Sample {
	uint64_t زمان_us;
	عائم داده[3];
};


صنف Timestamped_Ring_میانگیر_Static_Test : عام ::testing::Test
{
عام:
	Sample _x, _y, _z, _w;
	Timestamped_Ring_میانگیر<Sample, 3> *میانگیر{اشاره_تهی};

	فراغ مجموعه_به_بالا() override
	{
		میانگیر = جديد Timestamped_Ring_میانگیر<Sample, 3>();
		_x.زمان_us = 1000000;
		_x.داده[0] = _x.داده[1] = _x.داده[2] = 1.5f;

		_y.زمان_us = 2000000;
		_y.داده[0] = _y.داده[1] = _y.داده[2] = 3.0f;

		_z.زمان_us = 3000000;
		_z.داده[0] = _z.داده[1] = _z.داده[2] = 4.0f;

		_w.زمان_us = 4000000;
		_w.داده[0] = _w.داده[1] = _w.داده[2] = 5.0f;
	}

	فراغ Tear_پایین() override
	{
		حذف میانگیر;
	}
};

TEST_F(Timestamped_Ring_میانگیر_Static_Test, goodInitialisation)
{
	// WHEN: buffer was allocated
	// THEN: allocation should have succeed
	EXPECT_TRUE(میانگیر->valid());
	EXPECT_FALSE(میانگیر->allocate(2));
	EXPECT_EQ(میانگیر->get_درازی(), 3);
}

TEST_F(Timestamped_Ring_میانگیر_Static_Test, بعدی_Prev_Helpers)
{
	EXPECT_EQ(میانگیر->بعدی(0), 1);
	EXPECT_EQ(میانگیر->بعدی(1), 2);
	EXPECT_EQ(میانگیر->بعدی(2), 0);

	EXPECT_EQ(میانگیر->prev(0), 2);
	EXPECT_EQ(میانگیر->prev(1), 0);
	EXPECT_EQ(میانگیر->prev(2), 1);
}

TEST_F(Timestamped_Ring_میانگیر_Static_Test, افزودن_Wrap_And_Indices)
{
	میانگیر->reset();
	ASSERT_TRUE(میانگیر->خالی());

	میانگیر->افزودن(_x);
	میانگیر->افزودن(_y);
	EXPECT_EQ(میانگیر->entries(), 2);
	میانگیر->افزودن(_z);

	EXPECT_EQ(میانگیر->entries(), 3);
	EXPECT_EQ(میانگیر->get_oldest().زمان_us, _x.زمان_us);
	EXPECT_EQ(میانگیر->get_newest().زمان_us, _z.زمان_us);

	// Overwrite once (ring wrap-around).
	میانگیر->افزودن(_w);

	EXPECT_EQ(میانگیر->entries(), 3);
	EXPECT_EQ(میانگیر->get_oldest().زمان_us, _y.زمان_us);
	EXPECT_EQ(میانگیر->get_newest().زمان_us, _w.زمان_us);
}

TEST_F(Timestamped_Ring_میانگیر_Static_Test, برداشت_اولی_Older_Than_Discards_Older_History)
{
	میانگیر->reset();
	میانگیر->افزودن(_x);
	میانگیر->افزودن(_y);
	میانگیر->افزودن(_z);

	Sample برداشت = {};

	// Query timestamp older than any in the buffer: no match.
	EXPECT_FALSE(میانگیر->برداشت_اولی_older_than(0, &برداشت));

	// Pop y (also discards x), leaving z as the only remaining Sample.
	EXPECT_TRUE(میانگیر->برداشت_اولی_older_than(_y.زمان_us + 10, &برداشت));
	EXPECT_EQ(برداشت.زمان_us, _y.زمان_us);

	EXPECT_EQ(میانگیر->entries(), 1);
	EXPECT_EQ(میانگیر->get_oldest().زمان_us, _z.زمان_us);
	EXPECT_EQ(میانگیر->get_newest().زمان_us, _z.زمان_us);

	// Pop z (buffer becomes empty).
	EXPECT_TRUE(میانگیر->برداشت_اولی_older_than(_z.زمان_us + 1, &برداشت));
	EXPECT_EQ(برداشت.زمان_us, _z.زمان_us);
	EXPECT_TRUE(میانگیر->خالی());
}

TEST_F(Timestamped_Ring_میانگیر_Static_Test, ResetClearsTimestamps)
{
	میانگیر->افزودن(_x);
	میانگیر->افزودن(_y);
	EXPECT_FALSE(میانگیر->خالی());

	میانگیر->reset();
	EXPECT_TRUE(میانگیر->خالی());

	لكل (uint8_t i = 0; i < میانگیر->get_درازی(); i++) {
		EXPECT_EQ((*میانگیر)[i].زمان_us, 0u);
	}
}

TEST_F(Timestamped_Ring_میانگیر_Static_Test, مجموع_اندازه_Reflects_Inline_Storage)
{
	EXPECT_EQ(میانگیر->get_مجموع_اندازه(), static_cast<صحيح>(حجم(Timestamped_Ring_میانگیر<Sample, 3>)));
}

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

بنية sample {
	uint64_t زمان_us;
	عائم داده[3];
};


صنف Timestamped_Ring_میانگیر_Dynamic_Test : عام ::testing::Test
{
عام:

	sample _x, _y, _z;
	Timestamped_Ring_میانگیر<sample> *میانگیر{اشاره_تهی};

	فراغ مجموعه_به_بالا() override
	{
		میانگیر = جديد Timestamped_Ring_میانگیر<sample>(3);
		_x.زمان_us = 1000000;
		_x.داده[0] = _x.داده[1] = _x.داده[2] = 1.5f;

		_y.زمان_us = 2000000;
		_y.داده[0] = _y.داده[1] = _y.داده[2] = 3.0f;

		_z.زمان_us = 3000000;
		_z.داده[0] = _z.داده[1] = _z.داده[2] = 4.0f;
	}

	فراغ Tear_پایین() override
	{
		حذف میانگیر;
	}
};

TEST_F(Timestamped_Ring_میانگیر_Dynamic_Test, goodInitialisation)
{
	// WHEN: buffer was allocated
	// THEN: allocation should have succeed
	EXPECT_TRUE(میانگیر->allocate(3));
	EXPECT_TRUE(میانگیر->valid());
	EXPECT_EQ(میانگیر->get_درازی(), 3);
}

TEST_F(Timestamped_Ring_میانگیر_Dynamic_Test, Rejects_Invalid_اندازه)
{
	EXPECT_FALSE(میانگیر->allocate(0));
	EXPECT_FALSE(میانگیر->allocate(UINT8_MAX + 1u));
	EXPECT_TRUE(میانگیر->valid());
	EXPECT_EQ(میانگیر->get_درازی(), 3);
}

TEST_F(Timestamped_Ring_میانگیر_Dynamic_Test, badInitialisation)
{
	// WHEN: buffer allocation input is bad
	// THEN: allocation should fail

	// TODO: Change buffer implementation to pass this test
	// ASSERT_EQ(false, _buffer->allocate(-1));
	// ASSERT_EQ(false, _buffer->allocate(0));
}

TEST_F(Timestamped_Ring_میانگیر_Dynamic_Test, ترتیب_از_Samples)
{
	ASSERT_EQ(درست, میانگیر->allocate(3));
	// GIVEN: allocated buffer
	// WHEN: adding multiple samples
	// THEN: they should be added in order
	میانگیر->افزودن(_x);

	EXPECT_EQ(_x.زمان_us, میانگیر->get_newest().زمان_us);
	EXPECT_EQ(_x.زمان_us, میانگیر->get_oldest().زمان_us);

	میانگیر->افزودن(_z);
	میانگیر->افزودن(_y);

	EXPECT_EQ(_x.زمان_us, میانگیر->get_oldest().زمان_us);
	EXPECT_EQ(_y.زمان_us, میانگیر->get_newest().زمان_us);
}

TEST_F(Timestamped_Ring_میانگیر_Dynamic_Test, برداشت_Sample)
{
	ASSERT_EQ(درست, میانگیر->allocate(3));
	میانگیر->افزودن(_x);
	میانگیر->افزودن(_y);
	میانگیر->افزودن(_z);

	// GIVEN: allocated and filled buffer

	sample برداشت = {};
	// WHEN: we want to retrieve a sample that is older than any in the buffer
	// THEN: we should not get any sample
	EXPECT_FALSE(میانگیر->برداشت_اولی_older_than(0, &برداشت));

	// WHEN: when calling "pop_first_older_than"
	// THEN: we should get the first sample from the head that is older
	EXPECT_TRUE(میانگیر->برداشت_اولی_older_than(_x.زمان_us + 1, &برداشت));
	EXPECT_EQ(_x.زمان_us, برداشت.زمان_us);
	EXPECT_TRUE(میانگیر->برداشت_اولی_older_than(_y.زمان_us + 10, &برداشت));
	EXPECT_EQ(_y.زمان_us, برداشت.زمان_us);
	EXPECT_TRUE(میانگیر->برداشت_اولی_older_than(_z.زمان_us + 100, &برداشت));
	EXPECT_EQ(_z.زمان_us, برداشت.زمان_us);
	// TODO: When changing the order of popping sample it does not behave as expected, fix this
}

TEST_F(Timestamped_Ring_میانگیر_Dynamic_Test, askingForTooNewSample)
{
	ASSERT_EQ(درست, میانگیر->allocate(3));
	میانگیر->افزودن(_x);
	میانگیر->افزودن(_y);
	میانگیر->افزودن(_z);

	sample برداشت = {};
	// WHEN: all buffered samples are older by 0.1s than your query timestamp
	// THEN: should get no sample returned
	EXPECT_TRUE(میانگیر->برداشت_اولی_older_than(_z.زمان_us + 99000, &برداشت));
	EXPECT_FALSE(میانگیر->برداشت_اولی_older_than(_y.زمان_us + 100000, &برداشت));
}

TEST_F(Timestamped_Ring_میانگیر_Dynamic_Test, reallocate_میانگیر)
{
	ASSERT_EQ(درست, میانگیر->allocate(5));
	میانگیر->افزودن(_x);
	میانگیر->افزودن(_y);
	میانگیر->افزودن(_y);
	میانگیر->افزودن(_y);
	میانگیر->افزودن(_z);

	// GIVEN: allocated and filled buffer
	// WHEN: do another allocate call
	EXPECT_TRUE(میانگیر->allocate(3));
	// THEN: its length should update
	EXPECT_EQ(3, میانگیر->get_درازی());
}

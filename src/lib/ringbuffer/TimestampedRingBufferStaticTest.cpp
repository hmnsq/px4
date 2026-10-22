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
	uint64_t وخت_us;
	عائم معلومات[3];
};


صنف Timestamped_Ring_بفر_Static_Test : عام ::testing::Test
{
عام:
	Sample _x, _y, _z, _w;
	Timestamped_Ring_بفر<Sample, 3> *بفر{تش_اشاره};

	فراغ سېټ_پورته() override
	{
		بفر = جديد Timestamped_Ring_بفر<Sample, 3>();
		_x.وخت_us = 1000000;
		_x.معلومات[0] = _x.معلومات[1] = _x.معلومات[2] = 1.5f;

		_y.وخت_us = 2000000;
		_y.معلومات[0] = _y.معلومات[1] = _y.معلومات[2] = 3.0f;

		_z.وخت_us = 3000000;
		_z.معلومات[0] = _z.معلومات[1] = _z.معلومات[2] = 4.0f;

		_w.وخت_us = 4000000;
		_w.معلومات[0] = _w.معلومات[1] = _w.معلومات[2] = 5.0f;
	}

	فراغ Tear_ښکته() override
	{
		ړنګول بفر;
	}
};

TEST_F(Timestamped_Ring_بفر_Static_Test, goodInitialisation)
{
	// WHEN: buffer was allocated
	// THEN: allocation should have succeed
	EXPECT_TRUE(بفر->valid());
	EXPECT_FALSE(بفر->allocate(2));
	EXPECT_EQ(بفر->get_اوږدتیا(), 3);
}

TEST_F(Timestamped_Ring_بفر_Static_Test, بل_Prev_Helpers)
{
	EXPECT_EQ(بفر->بل(0), 1);
	EXPECT_EQ(بفر->بل(1), 2);
	EXPECT_EQ(بفر->بل(2), 0);

	EXPECT_EQ(بفر->prev(0), 2);
	EXPECT_EQ(بفر->prev(1), 0);
	EXPECT_EQ(بفر->prev(2), 1);
}

TEST_F(Timestamped_Ring_بفر_Static_Test, ټېل_Wrap_And_Indices)
{
	بفر->reset();
	ASSERT_TRUE(بفر->تشه());

	بفر->ټېل(_x);
	بفر->ټېل(_y);
	EXPECT_EQ(بفر->entries(), 2);
	بفر->ټېل(_z);

	EXPECT_EQ(بفر->entries(), 3);
	EXPECT_EQ(بفر->get_oldest().وخت_us, _x.وخت_us);
	EXPECT_EQ(بفر->get_newest().وخت_us, _z.وخت_us);

	// Overwrite once (ring wrap-around).
	بفر->ټېل(_w);

	EXPECT_EQ(بفر->entries(), 3);
	EXPECT_EQ(بفر->get_oldest().وخت_us, _y.وخت_us);
	EXPECT_EQ(بفر->get_newest().وخت_us, _w.وخت_us);
}

TEST_F(Timestamped_Ring_بفر_Static_Test, راوباسه_لومړی_Older_Than_Discards_Older_History)
{
	بفر->reset();
	بفر->ټېل(_x);
	بفر->ټېل(_y);
	بفر->ټېل(_z);

	Sample راوباسه = {};

	// Query timestamp older than any in the buffer: no match.
	EXPECT_FALSE(بفر->راوباسه_لومړی_older_than(0, &راوباسه));

	// Pop y (also discards x), leaving z as the only remaining Sample.
	EXPECT_TRUE(بفر->راوباسه_لومړی_older_than(_y.وخت_us + 10, &راوباسه));
	EXPECT_EQ(راوباسه.وخت_us, _y.وخت_us);

	EXPECT_EQ(بفر->entries(), 1);
	EXPECT_EQ(بفر->get_oldest().وخت_us, _z.وخت_us);
	EXPECT_EQ(بفر->get_newest().وخت_us, _z.وخت_us);

	// Pop z (buffer becomes empty).
	EXPECT_TRUE(بفر->راوباسه_لومړی_older_than(_z.وخت_us + 1, &راوباسه));
	EXPECT_EQ(راوباسه.وخت_us, _z.وخت_us);
	EXPECT_TRUE(بفر->تشه());
}

TEST_F(Timestamped_Ring_بفر_Static_Test, ResetClearsTimestamps)
{
	بفر->ټېل(_x);
	بفر->ټېل(_y);
	EXPECT_FALSE(بفر->تشه());

	بفر->reset();
	EXPECT_TRUE(بفر->تشه());

	لكل (uint8_t i = 0; i < بفر->get_اوږدتیا(); i++) {
		EXPECT_EQ((*بفر)[i].وخت_us, 0u);
	}
}

TEST_F(Timestamped_Ring_بفر_Static_Test, ټول_اندازه_Reflects_Inline_Storage)
{
	EXPECT_EQ(بفر->get_ټول_اندازه(), static_cast<صحيح>(حجم(Timestamped_Ring_بفر<Sample, 3>)));
}

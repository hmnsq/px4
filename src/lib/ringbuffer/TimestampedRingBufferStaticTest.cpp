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

#ਅਨ੍ਤਰ੍ਭਾਵ <gtest/gtest.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <math.h>
#ਅਨ੍ਤਰ੍ਭਾਵ "TimestampedRingBuffer.hpp"

ਬਣਤਰ Sample {
	uint64_t ਸਮਾਂ_us;
	ਦਸ਼ਮਲਵ ਅੰਕੜੇ[3];
};


ਵਰਗ Timestamped_Ring_ਬਫਰ_Static_Test : ਜਨਤਕ ::testing::Test
{
ਜਨਤਕ:
	Sample _x, _y, _z, _w;
	Timestamped_Ring_ਬਫਰ<Sample, 3> *ਬਫਰ{ਖਾਲੀ_ਸੂਚਕ};

	ਖਾਲੀ ਸੈੱਟ_ਉੱਪਰ() override
	{
		ਬਫਰ = ਨਵਾਂ Timestamped_Ring_ਬਫਰ<Sample, 3>();
		_x.ਸਮਾਂ_us = 1000000;
		_x.ਅੰਕੜੇ[0] = _x.ਅੰਕੜੇ[1] = _x.ਅੰਕੜੇ[2] = 1.5f;

		_y.ਸਮਾਂ_us = 2000000;
		_y.ਅੰਕੜੇ[0] = _y.ਅੰਕੜੇ[1] = _y.ਅੰਕੜੇ[2] = 3.0f;

		_z.ਸਮਾਂ_us = 3000000;
		_z.ਅੰਕੜੇ[0] = _z.ਅੰਕੜੇ[1] = _z.ਅੰਕੜੇ[2] = 4.0f;

		_w.ਸਮਾਂ_us = 4000000;
		_w.ਅੰਕੜੇ[0] = _w.ਅੰਕੜੇ[1] = _w.ਅੰਕੜੇ[2] = 5.0f;
	}

	ਖਾਲੀ Tear_ਥੱਲੇ() override
	{
		ਮਿਟਾਓ ਬਫਰ;
	}
};

TEST_F(Timestamped_Ring_ਬਫਰ_Static_Test, goodInitialisation)
{
	// WHEN: buffer was allocated
	// THEN: allocation should have succeed
	EXPECT_TRUE(ਬਫਰ->valid());
	EXPECT_FALSE(ਬਫਰ->allocate(2));
	EXPECT_EQ(ਬਫਰ->get_ਲੰਮਾਈ(), 3);
}

TEST_F(Timestamped_Ring_ਬਫਰ_Static_Test, ਅਗਲਾ_Prev_Helpers)
{
	EXPECT_EQ(ਬਫਰ->ਅਗਲਾ(0), 1);
	EXPECT_EQ(ਬਫਰ->ਅਗਲਾ(1), 2);
	EXPECT_EQ(ਬਫਰ->ਅਗਲਾ(2), 0);

	EXPECT_EQ(ਬਫਰ->prev(0), 2);
	EXPECT_EQ(ਬਫਰ->prev(1), 0);
	EXPECT_EQ(ਬਫਰ->prev(2), 1);
}

TEST_F(Timestamped_Ring_ਬਫਰ_Static_Test, ਧੱਕੋ_Wrap_And_Indices)
{
	ਬਫਰ->reset();
	ASSERT_TRUE(ਬਫਰ->ਖਾਲੀ_ਥਾਵਾਂ());

	ਬਫਰ->ਧੱਕੋ(_x);
	ਬਫਰ->ਧੱਕੋ(_y);
	EXPECT_EQ(ਬਫਰ->entries(), 2);
	ਬਫਰ->ਧੱਕੋ(_z);

	EXPECT_EQ(ਬਫਰ->entries(), 3);
	EXPECT_EQ(ਬਫਰ->get_oldest().ਸਮਾਂ_us, _x.ਸਮਾਂ_us);
	EXPECT_EQ(ਬਫਰ->get_newest().ਸਮਾਂ_us, _z.ਸਮਾਂ_us);

	// Overwrite once (ring wrap-around).
	ਬਫਰ->ਧੱਕੋ(_w);

	EXPECT_EQ(ਬਫਰ->entries(), 3);
	EXPECT_EQ(ਬਫਰ->get_oldest().ਸਮਾਂ_us, _y.ਸਮਾਂ_us);
	EXPECT_EQ(ਬਫਰ->get_newest().ਸਮਾਂ_us, _w.ਸਮਾਂ_us);
}

TEST_F(Timestamped_Ring_ਬਫਰ_Static_Test, ਕੱਢੋ_ਪਹਿਲਾ_Older_Than_Discards_Older_History)
{
	ਬਫਰ->reset();
	ਬਫਰ->ਧੱਕੋ(_x);
	ਬਫਰ->ਧੱਕੋ(_y);
	ਬਫਰ->ਧੱਕੋ(_z);

	Sample ਕੱਢੋ = {};

	// Query timestamp older than any in the buffer: no match.
	EXPECT_FALSE(ਬਫਰ->ਕੱਢੋ_ਪਹਿਲਾ_older_than(0, &ਕੱਢੋ));

	// Pop y (also discards x), leaving z as the only remaining Sample.
	EXPECT_TRUE(ਬਫਰ->ਕੱਢੋ_ਪਹਿਲਾ_older_than(_y.ਸਮਾਂ_us + 10, &ਕੱਢੋ));
	EXPECT_EQ(ਕੱਢੋ.ਸਮਾਂ_us, _y.ਸਮਾਂ_us);

	EXPECT_EQ(ਬਫਰ->entries(), 1);
	EXPECT_EQ(ਬਫਰ->get_oldest().ਸਮਾਂ_us, _z.ਸਮਾਂ_us);
	EXPECT_EQ(ਬਫਰ->get_newest().ਸਮਾਂ_us, _z.ਸਮਾਂ_us);

	// Pop z (buffer becomes empty).
	EXPECT_TRUE(ਬਫਰ->ਕੱਢੋ_ਪਹਿਲਾ_older_than(_z.ਸਮਾਂ_us + 1, &ਕੱਢੋ));
	EXPECT_EQ(ਕੱਢੋ.ਸਮਾਂ_us, _z.ਸਮਾਂ_us);
	EXPECT_TRUE(ਬਫਰ->ਖਾਲੀ_ਥਾਵਾਂ());
}

TEST_F(Timestamped_Ring_ਬਫਰ_Static_Test, ResetClearsTimestamps)
{
	ਬਫਰ->ਧੱਕੋ(_x);
	ਬਫਰ->ਧੱਕੋ(_y);
	EXPECT_FALSE(ਬਫਰ->ਖਾਲੀ_ਥਾਵਾਂ());

	ਬਫਰ->reset();
	EXPECT_TRUE(ਬਫਰ->ਖਾਲੀ_ਥਾਵਾਂ());

	ਲਈ (uint8_t i = 0; i < ਬਫਰ->get_ਲੰਮਾਈ(); i++) {
		EXPECT_EQ((*ਬਫਰ)[i].ਸਮਾਂ_us, 0u);
	}
}

TEST_F(Timestamped_Ring_ਬਫਰ_Static_Test, ਕੁੱਲ_ਮਾਪ_Reflects_Inline_Storage)
{
	EXPECT_EQ(ਬਫਰ->get_ਕੁੱਲ_ਮਾਪ(), static_cast<ਪੂਰਨ>(ਆਕਾਰ(Timestamped_Ring_ਬਫਰ<Sample, 3>)));
}

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

#ଅନ୍ତର୍ଭାଵ <gtest/gtest.h>
#ଅନ୍ତର୍ଭାଵ <math.h>
#ଅନ୍ତର୍ଭାଵ "TimestampedRingBuffer.hpp"

ସଂରଚନା Sample {
	uint64_t time_us;
	ଦଶମିକ data[3];
};


ଶ୍ରେଣୀ TimestampedRingBufferStaticTest : ସର୍ବସାଧାରଣ ::testing::Test
{
ସର୍ବସାଧାରଣ:
	Sample _x, _y, _z, _w;
	TimestampedRingBuffer<Sample, 3> *_buffer{nullptr};

	ଶୂନ୍ୟ SetUp() override
	{
		_buffer = ନୂତନ TimestampedRingBuffer<Sample, 3>();
		_x.time_us = 1000000;
		_x.data[0] = _x.data[1] = _x.data[2] = 1.5f;

		_y.time_us = 2000000;
		_y.data[0] = _y.data[1] = _y.data[2] = 3.0f;

		_z.time_us = 3000000;
		_z.data[0] = _z.data[1] = _z.data[2] = 4.0f;

		_w.time_us = 4000000;
		_w.data[0] = _w.data[1] = _w.data[2] = 5.0f;
	}

	ଶୂନ୍ୟ TearDown() override
	{
		delete _buffer;
	}
};

TEST_F(TimestampedRingBufferStaticTest, goodInitialisation)
{
	// WHEN: buffer was allocated
	// THEN: allocation should have succeed
	EXPECT_TRUE(_buffer->valid());
	EXPECT_FALSE(_buffer->allocate(2));
	EXPECT_EQ(_buffer->get_length(), 3);
}

TEST_F(TimestampedRingBufferStaticTest, NextPrevHelpers)
{
	EXPECT_EQ(_buffer->next(0), 1);
	EXPECT_EQ(_buffer->next(1), 2);
	EXPECT_EQ(_buffer->next(2), 0);

	EXPECT_EQ(_buffer->prev(0), 2);
	EXPECT_EQ(_buffer->prev(1), 0);
	EXPECT_EQ(_buffer->prev(2), 1);
}

TEST_F(TimestampedRingBufferStaticTest, ଠେଲ_Wrap_And_Indices)
{
	_buffer->reset();
	ASSERT_TRUE(_buffer->empty());

	_buffer->ଠେଲ(_x);
	_buffer->ଠେଲ(_y);
	EXPECT_EQ(_buffer->entries(), 2);
	_buffer->ଠେଲ(_z);

	EXPECT_EQ(_buffer->entries(), 3);
	EXPECT_EQ(_buffer->get_oldest().time_us, _x.time_us);
	EXPECT_EQ(_buffer->get_newest().time_us, _z.time_us);

	// Overwrite once (ring wrap-around).
	_buffer->ଠେଲ(_w);

	EXPECT_EQ(_buffer->entries(), 3);
	EXPECT_EQ(_buffer->get_oldest().time_us, _y.time_us);
	EXPECT_EQ(_buffer->get_newest().time_us, _w.time_us);
}

TEST_F(TimestampedRingBufferStaticTest, କାଢ଼_First_Older_Than_Discards_Older_History)
{
	_buffer->reset();
	_buffer->ଠେଲ(_x);
	_buffer->ଠେଲ(_y);
	_buffer->ଠେଲ(_z);

	Sample କାଢ଼ = {};

	// Query timestamp older than any in the buffer: no match.
	EXPECT_FALSE(_buffer->କାଢ଼_first_older_than(0, &କାଢ଼));

	// Pop y (also discards x), leaving z as the only remaining Sample.
	EXPECT_TRUE(_buffer->କାଢ଼_first_older_than(_y.time_us + 10, &କାଢ଼));
	EXPECT_EQ(କାଢ଼.time_us, _y.time_us);

	EXPECT_EQ(_buffer->entries(), 1);
	EXPECT_EQ(_buffer->get_oldest().time_us, _z.time_us);
	EXPECT_EQ(_buffer->get_newest().time_us, _z.time_us);

	// Pop z (buffer becomes empty).
	EXPECT_TRUE(_buffer->କାଢ଼_first_older_than(_z.time_us + 1, &କାଢ଼));
	EXPECT_EQ(କାଢ଼.time_us, _z.time_us);
	EXPECT_TRUE(_buffer->empty());
}

TEST_F(TimestampedRingBufferStaticTest, ResetClearsTimestamps)
{
	_buffer->ଠେଲ(_x);
	_buffer->ଠେଲ(_y);
	EXPECT_FALSE(_buffer->empty());

	_buffer->reset();
	EXPECT_TRUE(_buffer->empty());

	ପାଇଁ (uint8_t i = 0; i < _buffer->get_length(); i++) {
		EXPECT_EQ((*_buffer)[i].time_us, 0u);
	}
}

TEST_F(TimestampedRingBufferStaticTest, TotalSizeReflectsInlineStorage)
{
	EXPECT_EQ(_buffer->get_total_size(), static_cast<ପୂର୍ଣ୍ଣସଂଖ୍ୟା>(ଆକାର(TimestampedRingBuffer<Sample, 3>)));
}

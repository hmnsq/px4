/****************************************************************************
 *
 *   Copyright (C) 2023 PX4 Development Team. All rights reserved.
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
#شامل <matrix/matrix/math.hpp>
#شامل "Functions.hpp"

#شامل "TrajMath.hpp"

استعمال نامخلا math;
استعمال میٹرکس::Vector2f;

TEST(Max_فاصلہ_مقصد_Circle, noDirection)
{
	Vector2f pos;
	Vector2f circle_center;
	اعشاری radius = 3.f;
	Vector2f direction;
	اعشاری فاصلہ = trajectory::get_Max_فاصلہ_مقصد_Circle(pos, circle_center, radius, direction);
	EXPECT_TRUE(isnan(فاصلہ));
}

TEST(Max_فاصلہ_مقصد_Circle, insideCircle)
{
	// North position, South direction
	Vector2f pos(1.f, 0.f);
	Vector2f circle_center;
	اعشاری radius = 3.f;
	Vector2f direction(-1.f, 0.f);
	اعشاری فاصلہ = trajectory::get_Max_فاصلہ_مقصد_Circle(pos, circle_center, radius, direction);
	EXPECT_FLOAT_EQ(فاصلہ, 4.f);

	// North position, West direction (direction doesn't need to be unit length)
	direction = Vector2f(0.f, -3.42f);
	فاصلہ = trajectory::get_Max_فاصلہ_مقصد_Circle(pos, circle_center, radius, direction);
	EXPECT_FLOAT_EQ(فاصلہ, sqrtf(radius * radius - 1.f));

	// SE position, directed towards the center
	pos = Vector2f(-2.f, 1.f);
	direction = Vector2f(2.f, -1.f);
	فاصلہ = trajectory::get_Max_فاصلہ_مقصد_Circle(pos, circle_center, radius, direction);
	EXPECT_FLOAT_EQ(فاصلہ, pos.norm() + radius);
}

TEST(Max_فاصلہ_مقصد_Circle, outsideCircle)
{
	// North position, South direction
	Vector2f pos(4.f, 0.f);
	Vector2f circle_center;
	اعشاری radius = 3.f;
	Vector2f direction(-1.f, 0.f);
	اعشاری فاصلہ = trajectory::get_Max_فاصلہ_مقصد_Circle(pos, circle_center, radius, direction);
	EXPECT_FLOAT_EQ(فاصلہ, pos(0) + radius);

	// looking away from the circle
	direction = Vector2f(0.f, -3.42f);
	فاصلہ = trajectory::get_Max_فاصلہ_مقصد_Circle(pos, circle_center, radius, direction);
	EXPECT_TRUE(isnan(فاصلہ));

	// SE position, directed towards the center
	pos = Vector2f(-4.f, 2.f);
	direction = Vector2f(2.f, -1.f);
	فاصلہ = trajectory::get_Max_فاصلہ_مقصد_Circle(pos, circle_center, radius, direction);
	EXPECT_FLOAT_EQ(فاصلہ, pos.norm() + radius);
}

TEST(Max_فاصلہ_مقصد_Circle, onCircle)
{
	// South, looking North
	Vector2f pos(-4.f, 1.f);
	Vector2f circle_center(-1.f, 1.f);
	اعشاری radius = 3.f;
	Vector2f direction(1.f, 0.f);
	اعشاری فاصلہ = trajectory::get_Max_فاصلہ_مقصد_Circle(pos, circle_center, radius, direction);
	EXPECT_FLOAT_EQ(فاصلہ, 2.f * radius);

	// looking tangent to the circle
	direction = Vector2f(0.f, -3.42f);
	فاصلہ = trajectory::get_Max_فاصلہ_مقصد_Circle(pos, circle_center, radius, direction);
	EXPECT_FLOAT_EQ(فاصلہ, 0.f);

	// looking away from the circle
	direction = Vector2f(-10.f, -3.42f);
	فاصلہ = trajectory::get_Max_فاصلہ_مقصد_Circle(pos, circle_center, radius, direction);
	EXPECT_FLOAT_EQ(فاصلہ, 0.f);

	// SE position, directed towards the center
	pos = Vector2f(-sqrtf(2.f) / 2.f, sqrtf(2.f) / 2.f) * radius + circle_center;
	direction = -pos;
	فاصلہ = trajectory::get_Max_فاصلہ_مقصد_Circle(pos, circle_center, radius, direction);
	EXPECT_FLOAT_EQ(فاصلہ, 2.f * radius);
}

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

#ਅਨ੍ਤਰ੍ਭਾਵ <gtest/gtest.h>
#ਅਨ੍ਤਰ੍ਭਾਵ "SlewRateYaw.hpp"

TEST(Slew_ਦਰ_Yaw_Test, Slew_ਉੱਪਰ_Limited)
{
	Slew_ਦਰ_Yaw<ਦਸ਼ਮਲਵ> slew_ਦਰ_yaw;
	slew_ਦਰ_yaw.ਸੈੱਟ_Slew_ਦਰ(.15f);
	slew_ਦਰ_yaw.ਸੈੱਟ_Forced_ਮੁੱਲ(1.1f);

	ਲਈ (ਪੂਰਨ i = 1; i <= 10; i++) {
		EXPECT_FLOAT_EQ(slew_ਦਰ_yaw.update(3.f, 1.f), 1.1f + i * .15f);
	}
}

TEST(Slew_ਦਰ_Yaw_Test, Slew_ਥੱਲੇ_Limited)
{
	Slew_ਦਰ_Yaw<ਦਸ਼ਮਲਵ> slew_ਦਰ_yaw;
	slew_ਦਰ_yaw.ਸੈੱਟ_Slew_ਦਰ(.1f);
	slew_ਦਰ_yaw.ਸੈੱਟ_Forced_ਮੁੱਲ(.5f);

	ਲਈ (ਪੂਰਨ i = 1; i <= 10; i++) {
		EXPECT_NEAR(slew_ਦਰ_yaw.update(-2.f, 1.f), .5f - i * .1f, 1e-7f);
	}
}

TEST(Slew_ਦਰ_Yaw_Test, ਪਹੁੰਚ_ਮੁੱਲ_Slewed)
{
	Slew_ਦਰ_Yaw<ਦਸ਼ਮਲਵ> slew_ਦਰ_yaw;
	slew_ਦਰ_yaw.ਸੈੱਟ_Slew_ਦਰ(.2f);
	slew_ਦਰ_yaw.ਸੈੱਟ_Forced_ਮੁੱਲ(1.f);

	ਲਈ (ਪੂਰਨ i = 1; i <= 10; i++) {
		EXPECT_FLOAT_EQ(slew_ਦਰ_yaw.update(3.f, 1.f), 1.f + i * .2f);
	}

	ਲਈ (ਪੂਰਨ i = 1; i <= 10; i++) {
		EXPECT_FLOAT_EQ(slew_ਦਰ_yaw.update(3.f, 1.f), 3.f);
	}
}

TEST(Slew_ਦਰ_Yaw_Test, Slew_ਉੱਪਰ_Wrapped_Output)
{
	// put the goal value always a bit further away such that at some point the output has to wrap
	Slew_ਦਰ_Yaw<ਦਸ਼ਮਲਵ> slew_ਦਰ_yaw;
	slew_ਦਰ_yaw.ਸੈੱਟ_Slew_ਦਰ(.2f);
	slew_ਦਰ_yaw.ਸੈੱਟ_Forced_ਮੁੱਲ(0.f);

	ਲਈ (ਪੂਰਨ i = 1; i <= 30; i++) {
		EXPECT_NEAR(slew_ਦਰ_yaw.update(i * .25f, 1.f), ਮੈਟ੍ਰਿਕਸ::wrap_pi(i * .2f), 1e-5f);
	}
}

TEST(Slew_ਦਰ_Yaw_Test, Slew_ਥੱਲੇ_Wrapped_Output)
{
	// put the goal value always a bit further away such that at some point the output has to wrap
	Slew_ਦਰ_Yaw<ਦਸ਼ਮਲਵ> slew_ਦਰ_yaw;
	slew_ਦਰ_yaw.ਸੈੱਟ_Slew_ਦਰ(.2f);
	slew_ਦਰ_yaw.ਸੈੱਟ_Forced_ਮੁੱਲ(0.f);

	ਲਈ (ਪੂਰਨ i = 1; i <= 50; i++) {
		EXPECT_NEAR(slew_ਦਰ_yaw.update(i * -.25f, 1.f), ਮੈਟ੍ਰਿਕਸ::wrap_pi(i * -.2f), 1e-5f);
	}
}

TEST(Slew_ਦਰ_Yaw_Test, Slew_ਉੱਪਰ_Wrapped_ਨਿਵੇਸ਼)
{
	// put the goal value always a bit further away such that at some point the output has to wrap
	Slew_ਦਰ_Yaw<ਦਸ਼ਮਲਵ> slew_ਦਰ_yaw;
	slew_ਦਰ_yaw.ਸੈੱਟ_Slew_ਦਰ(.2f);
	slew_ਦਰ_yaw.ਸੈੱਟ_Forced_ਮੁੱਲ(0.f);

	ਲਈ (ਪੂਰਨ i = 1; i <= 50; i++) {
		EXPECT_NEAR(slew_ਦਰ_yaw.update(ਮੈਟ੍ਰਿਕਸ::wrap_pi(i * .25f), 1.f), ਮੈਟ੍ਰਿਕਸ::wrap_pi(i * .2f), 1e-5f);
	}
}

TEST(Slew_ਦਰ_Yaw_Test, Slew_Short_Way_ਨਿਵੇਸ਼)
{
	Slew_ਦਰ_Yaw<ਦਸ਼ਮਲਵ> slew_ਦਰ_yaw;
	slew_ਦਰ_yaw.ਸੈੱਟ_Slew_ਦਰ(1.f);
	slew_ਦਰ_yaw.ਸੈੱਟ_Forced_ਮੁੱਲ(0.f);

	EXPECT_FLOAT_EQ(slew_ਦਰ_yaw.update(3.f, 1.f), 1.f);
	EXPECT_FLOAT_EQ(slew_ਦਰ_yaw.update(5.f, 1.f), 0.f);
	EXPECT_FLOAT_EQ(slew_ਦਰ_yaw.update(5.f, 1.f), -1.f);
	EXPECT_FLOAT_EQ(slew_ਦਰ_yaw.update(4.f, 1.f), -2.f);
	EXPECT_FLOAT_EQ(slew_ਦਰ_yaw.update(3.f, 1.f), -3.f);
	EXPECT_FLOAT_EQ(slew_ਦਰ_yaw.update(5.f, 1.f), -2.f);
}

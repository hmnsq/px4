/****************************************************************************
 *
 *   Copyright (C) 2025 PX4 Development Team. All rights reserved.
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

/******************************************************************
 * Test code for the Pure Pursuit algorithm
 * Run this test only using "make tests TESTFILTER=RoverControl"
******************************************************************/

#అన్తర్భావ <gtest/gtest.h>
#అన్తర్భావ "RoverControl.hpp"

TEST(calcWaypointTransitionAngle, invalidInputs)
{
	Vector2f prev_wp_ned(NAN, NAN);
	Vector2f curr_wp_ned(10.f, 10.f);
	Vector2f తదుపరి_wp_ned(10.f, 10.f);
	దశాంశం prevInvalid = RoverControl::calcWaypointTransitionAngle(prev_wp_ned, curr_wp_ned, తదుపరి_wp_ned);
	prev_wp_ned = Vector2f(10.f, 10.f);
	curr_wp_ned = Vector2f(NAN, NAN);
	దశాంశం currInvalid = RoverControl::calcWaypointTransitionAngle(prev_wp_ned, curr_wp_ned, తదుపరి_wp_ned);
	curr_wp_ned = Vector2f(10.f, 10.f);
	తదుపరి_wp_ned = Vector2f(NAN, NAN);
	దశాంశం తదుపరి_Invalid = RoverControl::calcWaypointTransitionAngle(prev_wp_ned, curr_wp_ned, తదుపరి_wp_ned);
	EXPECT_FALSE(PX4_ISFINITE(prevInvalid));
	EXPECT_FALSE(PX4_ISFINITE(currInvalid));
	EXPECT_FALSE(PX4_ISFINITE(తదుపరి_Invalid));

}

TEST(calcWaypointTransitionAngle, validInputs)
{
	// P -- C -- N
	Vector2f prev_wp_ned(0.f, 0.f);
	Vector2f curr_wp_ned(10.f, 0.f);
	Vector2f తదుపరి_wp_ned(20.f, 0.f);
	స్థిరరాశి దశాంశం angle1 = RoverControl::calcWaypointTransitionAngle(prev_wp_ned, curr_wp_ned, తదుపరి_wp_ned);
	EXPECT_FLOAT_EQ(angle1, M_PI_F);

	/**
	 *	    N
	 *	  /
	 * P -- C
	 */
	prev_wp_ned = Vector2f(0.f, 0.f);
	curr_wp_ned = Vector2f(10.f, 0.f);
	తదుపరి_wp_ned = Vector2f(20.f, 10.f);
	స్థిరరాశి దశాంశం angle2 = RoverControl::calcWaypointTransitionAngle(prev_wp_ned, curr_wp_ned, తదుపరి_wp_ned);
	EXPECT_FLOAT_EQ(angle2, M_PI_F - M_PI_4_F);

	/**
	 *	N
	 *	|
	 * P -- C
	 */
	prev_wp_ned = Vector2f(0.f, 0.f);
	curr_wp_ned = Vector2f(10.f, 0.f);
	తదుపరి_wp_ned = Vector2f(10.f, 10.f);
	స్థిరరాశి దశాంశం angle3 = RoverControl::calcWaypointTransitionAngle(prev_wp_ned, curr_wp_ned, తదుపరి_wp_ned);
	EXPECT_FLOAT_EQ(angle3, M_PI_2_F);

	/**
	 * N
	 *    \
	 * P -- C
	 */
	prev_wp_ned = Vector2f(0.f, 0.f);
	curr_wp_ned = Vector2f(10.f, 0.f);
	తదుపరి_wp_ned = Vector2f(0.f, 10.f);
	స్థిరరాశి దశాంశం angle4 = RoverControl::calcWaypointTransitionAngle(prev_wp_ned, curr_wp_ned, తదుపరి_wp_ned);
	EXPECT_FLOAT_EQ(angle4, M_PI_4_F);

	// P/C -- N
	prev_wp_ned = Vector2f(0.f, 0.f);
	curr_wp_ned = Vector2f(0.f, 0.f);
	తదుపరి_wp_ned = Vector2f(10.f, 0.f);
	స్థిరరాశి దశాంశం angle5 = RoverControl::calcWaypointTransitionAngle(prev_wp_ned, curr_wp_ned, తదుపరి_wp_ned);
	EXPECT_FALSE(PX4_ISFINITE(angle5));

	// P -- C/N
	prev_wp_ned = Vector2f(0.f, 0.f);
	curr_wp_ned = Vector2f(10.f, 0.f);
	తదుపరి_wp_ned = Vector2f(10.f, 0.f);
	స్థిరరాశి దశాంశం angle6 = RoverControl::calcWaypointTransitionAngle(prev_wp_ned, curr_wp_ned, తదుపరి_wp_ned);
	EXPECT_FALSE(PX4_ISFINITE(angle6));
}

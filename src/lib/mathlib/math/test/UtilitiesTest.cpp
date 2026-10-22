/****************************************************************************
 *
 *   Copyright (c) 2019 ECL Development Team. All rights reserved.
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
 * @file UtilitiesTest.cpp
 *
 * @brief Unit tests for the miscellaneous EKF utilities
 */

#అన్తర్భావ <gtest/gtest.h>
#అన్తర్భావ <cmath>
#అన్తర్భావ <vector>
#అన్తర్భావ <mathlib/mathlib.h>

ఉపయోగించి నామాకాశ math::Utilities;

TEST(euler321YawTest, fromQuaternion)
{
	మాతృక::Quatf q1(3.5f, 2.4f, -0.5f, -3.f);
	q1.normalize();
	స్థిరరాశి మాతృక::Eulerf euler1(q1);
	EXPECT_FLOAT_EQ(euler1(2), getEuler321Yaw(q1));

	మాతృక::Quatf q2(0.f, 0, -1.f, 0.f);
	q2.normalize();
	స్థిరరాశి మాతృక::Eulerf euler2(q2);
	EXPECT_FLOAT_EQ(euler2(2), getEuler321Yaw(q2));
}

TEST(euler312YawTest, fromQuaternion)
{
	// Use orientations with more pitch than roll so 312 sequence is appropriate
	మాతృక::Quatf q1(3.5f, 2.4f, -0.5f, -3.f);
	q1.normalize();
	స్థిరరాశి మాతృక::Dcmf R1(q1);
	EXPECT_FLOAT_EQ(getEuler312Yaw(R1), getEuler312Yaw(q1));

	మాతృక::Quatf q2(0.f, 0, -1.f, 0.f);
	q2.normalize();
	స్థిరరాశి మాతృక::Dcmf R2(q2);
	EXPECT_FLOAT_EQ(getEuler312Yaw(R2), getEuler312Yaw(q2));

	// Pure yaw rotation — 312 and 321 yaw should agree
	మాతృక::Quatf q3(మాతృక::Eulerf(0.f, 0.f, 1.2f));
	స్థిరరాశి మాతృక::Dcmf R3(q3);
	EXPECT_FLOAT_EQ(getEuler312Yaw(R3), getEuler312Yaw(q3));
	EXPECT_NEAR(getEuler312Yaw(q3), getEuler321Yaw(q3), 1e-5f);
}

TEST(shouldUse321RotationSequenceTest, pitch90)
{
	మాతృక::Eulerf euler(0.f, math::radians(90), 0.f);
	మాతృక::Dcmf R(euler);
	EXPECT_FALSE(shouldUse321RotationSequence(R));
}

TEST(shouldUse321RotationSequenceTest, roll90)
{
	మాతృక::Eulerf euler(math::radians(90.f), 0.f, 0.f);
	మాతృక::Dcmf R(euler);
	EXPECT_TRUE(shouldUse321RotationSequence(R));
}

TEST(shouldUse321RotationSequenceTest, మరిన్ని_Roll_Than_Pitch)
{
	మాతృక::Eulerf euler(math::radians(45.f), math::radians(30.f), 0.f);
	మాతృక::Dcmf R(euler);
	EXPECT_TRUE(shouldUse321RotationSequence(R));
}

TEST(shouldUse321RotationSequenceTest, మరిన్ని_Pitch_Than_Roll)
{
	మాతృక::Eulerf euler(math::radians(30.f), math::radians(45.f), 0.f);
	మాతృక::Dcmf R(euler);
	EXPECT_FALSE(shouldUse321RotationSequence(R));
}

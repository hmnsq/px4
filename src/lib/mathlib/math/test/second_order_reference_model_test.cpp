/****************************************************************************
 *
 *   Copyright (c) 2022 ECL Development Team. All rights reserved.
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
 * @file second_order_reference_model_test.cpp
 *
 * @brief Unit tests for the second order reference model implementation
 */

#ضمن <gtest/gtest.h>
#ضمن <matrix/math.hpp>
#ضمن <lib/mathlib/math/filter/second_order_reference_model.hpp>

ورتوں math::دوجا_ترتیب_Reference_Model;
ورتوں میٹرکس::Vector3f;

TEST(دوجا_ترتیب_Reference_Model, Float_Default_Constructor_Initializes_حالتاں_منزل_Zero)
{
	دوجا_ترتیب_Reference_Model<عائم> sys_float;

	// default constructor leaves states initialized to zero
	EXPECT_FLOAT_EQ(sys_float.getState(), 0.0f);
	EXPECT_FLOAT_EQ(sys_float.get_شرح(), 0.0f);
	EXPECT_FLOAT_EQ(sys_float.getAccel(), 0.0f);
}

TEST(دوجا_ترتیب_Reference_Model, FloatReset)
{
	دوجا_ترتیب_Reference_Model<عائم> sys_float;

	// reset the system states
	عائم init_state = 3.0f;
	عائم init_شرح = 1.1f;
	sys_float.reset(init_state, init_شرح);
	EXPECT_FLOAT_EQ(sys_float.getState(), init_state);
	EXPECT_FLOAT_EQ(sys_float.get_شرح(), init_شرح);
	EXPECT_FLOAT_EQ(sys_float.getAccel(), 0.0f);

	// reset the system without providing rate
	init_state = 5.5f;
	sys_float.reset(init_state);
	EXPECT_FLOAT_EQ(sys_float.getState(), init_state);
	EXPECT_FLOAT_EQ(sys_float.get_شرح(), 0.0f);
	EXPECT_FLOAT_EQ(sys_float.getAccel(), 0.0f);
}

TEST(دوجا_ترتیب_Reference_Model, Vector3_Default_Constructor_Initializes_حالتاں_منزل_Zero)
{
	دوجا_ترتیب_Reference_Model<Vector3f> sys_vector3f;

	// default constructor leaves states initialized to zero
	Vector3f state = sys_vector3f.getState();
	EXPECT_FLOAT_EQ(state(0), 0.0f);
	EXPECT_FLOAT_EQ(state(1), 0.0f);
	EXPECT_FLOAT_EQ(state(2), 0.0f);
	Vector3f شرح = sys_vector3f.get_شرح();
	EXPECT_FLOAT_EQ(شرح(0), 0.0f);
	EXPECT_FLOAT_EQ(شرح(1), 0.0f);
	EXPECT_FLOAT_EQ(شرح(2), 0.0f);
	Vector3f accel = sys_vector3f.getAccel();
	EXPECT_FLOAT_EQ(accel(0), 0.0f);
	EXPECT_FLOAT_EQ(accel(1), 0.0f);
	EXPECT_FLOAT_EQ(accel(2), 0.0f);
}

TEST(دوجا_ترتیب_Reference_Model, Vector3Reset)
{
	دوجا_ترتیب_Reference_Model<Vector3f> sys_vector3f;

	// reset the system states
	Vector3f init_state(1.0f, 2.0f, 3.0f);
	Vector3f init_شرح(0.1f, 0.2f, 0.3f);
	sys_vector3f.reset(init_state, init_شرح);
	Vector3f state = sys_vector3f.getState();
	EXPECT_FLOAT_EQ(state(0), init_state(0));
	EXPECT_FLOAT_EQ(state(1), init_state(1));
	EXPECT_FLOAT_EQ(state(2), init_state(2));
	Vector3f شرح = sys_vector3f.get_شرح();
	EXPECT_FLOAT_EQ(شرح(0), init_شرح(0));
	EXPECT_FLOAT_EQ(شرح(1), init_شرح(1));
	EXPECT_FLOAT_EQ(شرح(2), init_شرح(2));
	Vector3f accel = sys_vector3f.getAccel();
	EXPECT_FLOAT_EQ(accel(0), 0.0f);
	EXPECT_FLOAT_EQ(accel(1), 0.0f);
	EXPECT_FLOAT_EQ(accel(2), 0.0f);

	// reset the system without providing rate
	init_state = Vector3f(4.0f, 5.0f, 6.0f);
	sys_vector3f.reset(init_state);
	state = sys_vector3f.getState();
	EXPECT_FLOAT_EQ(state(0), init_state(0));
	EXPECT_FLOAT_EQ(state(1), init_state(1));
	EXPECT_FLOAT_EQ(state(2), init_state(2));
	شرح = sys_vector3f.get_شرح();
	EXPECT_FLOAT_EQ(شرح(0), 0.0f);
	EXPECT_FLOAT_EQ(شرح(1), 0.0f);
	EXPECT_FLOAT_EQ(شرح(2), 0.0f);
	accel = sys_vector3f.getAccel();
	EXPECT_FLOAT_EQ(accel(0), 0.0f);
	EXPECT_FLOAT_EQ(accel(1), 0.0f);
	EXPECT_FLOAT_EQ(accel(2), 0.0f);
}

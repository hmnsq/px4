/****************************************************************************
 *
 *   Copyright (C) 2019 PX4 Development Team. All rights reserved.
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
 * @file ControlAllocationPseudoInverseTest.cpp
 *
 * Tests for Control Allocation Algorithms
 *
 * @author Julien Lecoeur <julien.lecoeur@gmail.com>
 */

#ضمن <gtest/gtest.h>
#ضمن <ControlAllocationPseudoInverse.hpp>

ورتوں نطاق میٹرکس;

TEST(Control_ونڈ_Test, AllZeroCase)
{
	Control_ونڈ_Pseudo_Inverse طریقہ;

	میٹرکس::سمتیہ<عائم, 6> control_sp;
	میٹرکس::سمتیہ<عائم, 6> control_allocated;
	میٹرکس::سمتیہ<عائم, 6> control_allocated_expected;
	میٹرکس::میٹرکس_2<عائم, 6, 16> effectiveness;
	میٹرکس::سمتیہ<عائم, 16> actuator_sp;
	میٹرکس::سمتیہ<عائم, 16> actuator_trim;
	میٹرکس::سمتیہ<عائم, 16> linearization_نقطہ;
	میٹرکس::سمتیہ<عائم, 16> actuator_sp_expected;

	طریقہ.سیٹ_Effectiveness_میٹرکس(effectiveness, actuator_trim, linearization_نقطہ, 16, خطأ);
	طریقہ.سیٹ_Control_Setpoint(control_sp);
	طریقہ.allocate();
	طریقہ.clipActuatorSetpoint();
	actuator_sp = طریقہ.getActuatorSetpoint();
	control_allocated_expected = طریقہ.getAllocatedControl();

	EXPECT_EQ(actuator_sp, actuator_sp_expected);
	EXPECT_EQ(control_allocated, control_allocated_expected);
}

TEST(Control_ونڈ_Metric_Test, AllZeroCase)
{
	Control_ونڈ_Pseudo_Inverse طریقہ;

	میٹرکس::سمتیہ<عائم, 6> control_sp;
	میٹرکس::سمتیہ<عائم, 6> control_allocated;
	میٹرکس::سمتیہ<عائم, 6> control_allocated_expected;
	میٹرکس::میٹرکس_2<عائم, 6, 16> effectiveness;
	میٹرکس::سمتیہ<عائم, 16> actuator_sp;
	میٹرکس::سمتیہ<عائم, 16> actuator_trim;
	میٹرکس::سمتیہ<عائم, 16> linearization_نقطہ;
	میٹرکس::سمتیہ<عائم, 16> actuator_sp_expected;

	طریقہ.سیٹ_Metric_ونڈ(سچ);
	طریقہ.سیٹ_Effectiveness_میٹرکس(effectiveness, actuator_trim, linearization_نقطہ, 16, خطأ);
	طریقہ.سیٹ_Control_Setpoint(control_sp);
	طریقہ.allocate();
	actuator_sp = طریقہ.getActuatorSetpoint();
	control_allocated_expected = طریقہ.getAllocatedControl();

	EXPECT_EQ(actuator_sp, actuator_sp_expected);
	EXPECT_EQ(control_allocated, control_allocated_expected);
}

نطاق
{

// Surveyed hexarotor; rows roll/pitch/yaw/thrust xyz, one column per rotor
constexpr عائم kSurveyedHexarotor[6][6] = {
	{-5.947500f,  5.934500f,  2.964000f, -2.853500f, -2.990000f,  2.840500f},
	{ 0.071500f,  0.071500f,  5.063500f, -4.972500f,  5.063500f, -4.972500f},
	{-0.325000f,  0.325000f, -0.325000f,  0.325000f,  0.325000f, -0.325000f},
	{ 0.f,        0.f,        0.f,        0.f,        0.f,        0.f},
	{ 0.f,        0.f,        0.f,        0.f,        0.f,        0.f},
	{-6.500000f, -6.500000f, -6.500000f, -6.500000f, -6.500000f, -6.500000f}
};

میٹرکس_2<عائم, 6, 16> surveyedHexarotor(uint16_t stopped_نقاب)
{
	میٹرکس_2<عائم, 6, 16> effectiveness;
	effectiveness.سیٹ_Zero();

	لكل (صحيح i = 0; i < 6; i++) {
		إذا (stopped_نقاب & (1u << i)) {
			استمر;
		}

		لكل (صحيح axis = 0; axis < 6; axis++) {
			effectiveness(axis, i) = kSurveyedHexarotor[axis][i];
		}
	}

	أرجع effectiveness;
}

بنية Rotor {
	عائم px;
	عائم py;
	عائم km;
};

// upward rotors as in ActuatorEffectivenessRotors
میٹرکس_2<عائم, 6, 16> planarRotors(ثابت Rotor *rotors, صحيح گنتی, uint16_t stopped_نقاب = 0)
{
	constexpr عائم ct = 6.5f;
	میٹرکس_2<عائم, 6, 16> effectiveness;
	effectiveness.سیٹ_Zero();

	لكل (صحيح i = 0; i < گنتی; i++) {
		إذا (stopped_نقاب & (1u << i)) {
			استمر;
		}

		effectiveness(0, i) = -ct * rotors[i].py;
		effectiveness(1, i) = ct * rotors[i].px;
		effectiveness(2, i) = ct * rotors[i].km;
		effectiveness(5, i) = -ct;
	}

	أرجع effectiveness;
}

// In-tree geometries 4001_quad_x, 6001_hexa_x, 8001_octo_x
constexpr Rotor kQuadX[] = {{1.f, 1.f, 0.05f}, {-1.f, -1.f, 0.05f}, {1.f, -1.f, -0.05f}, {-1.f, 1.f, -0.05f}};
constexpr Rotor kHexaX[] = {{0.f, 0.5f, -0.05f}, {0.f, -0.5f, 0.05f}, {0.43f, -0.25f, -0.05f},
	{-0.43f, 0.25f, 0.05f}, {0.43f, 0.25f, 0.05f}, {-0.43f, -0.25f, -0.05f}
};
constexpr Rotor kOctoX[] = {{0.46f, 0.19f, -0.05f}, {-0.46f, -0.19f, -0.05f}, {0.19f, 0.46f, 0.05f},
	{-0.46f, 0.19f, 0.05f}, {0.46f, -0.19f, 0.05f}, {-0.19f, -0.46f, 0.05f}, {0.19f, -0.46f, -0.05f},
	{-0.19f, 0.46f, -0.05f}
};

} // namespace

TEST(Control_ونڈ_Pseudo_Inverse_Test, DropDependentAxesKeepsIndependentGeometries)
{
	ثابت میٹرکس_2<عائم, 6, 16> independent[] = {
		planarRotors(kQuadX, 4),
		planarRotors(kHexaX, 6),
		planarRotors(kOctoX, 8),
		surveyedHexarotor(0),
		surveyedHexarotor(1u << 0),		// one motor removed: still rank 4
		planarRotors(kHexaX, 6, 1u << 2),
		planarRotors(kOctoX, 8, (1u << 0) | (1u << 1)),
	};

	لكل (ثابت خودکار &original : independent) {
		میٹرکس_2<عائم, 6, 16> effectiveness = original;
		EXPECT_EQ(Control_ونڈ_Pseudo_Inverse::dropDependentAxes(effectiveness), 0);
		EXPECT_EQ(effectiveness, original);
	}
}

TEST(Control_ونڈ_Pseudo_Inverse_Test, DropDependentAxesDropsYawCollinearWithRoll)
{
	// motor and its opposite stopped: roll and yaw collinear, yaw must go, never roll
	ثابت میٹرکس_2<عائم, 6, 16> reduced[] = {
		surveyedHexarotor((1u << 0) | (1u << 1)),
		surveyedHexarotor((1u << 2) | (1u << 3)),
		surveyedHexarotor((1u << 4) | (1u << 5)),
		planarRotors(kHexaX, 6, (1u << 0) | (1u << 1)),
		planarRotors(kHexaX, 6, (1u << 2) | (1u << 3)),
	};

	لكل (ثابت خودکار &original : reduced) {
		میٹرکس_2<عائم, 6, 16> effectiveness = original;
		ثابت uint8_t dropped = Control_ونڈ_Pseudo_Inverse::dropDependentAxes(effectiveness);
		EXPECT_EQ(dropped, 1u << Control_ونڈ::YAW);

		لكل (صحيح axis = 0; axis < 6; axis++) {
			لكل (صحيح i = 0; i < 16; i++) {
				ثابت عائم expected = (axis == Control_ونڈ::YAW) ? 0.f : original(axis, i);
				EXPECT_FLOAT_EQ(effectiveness(axis, i), expected);
			}
		}
	}
}

TEST(Control_ونڈ_Pseudo_Inverse_Test, DroppedAxisIsUnallocatedOthersExact)
{
	Control_ونڈ_Pseudo_Inverse طریقہ;
	سمتیہ<عائم, 16> actuator_trim;
	سمتیہ<عائم, 16> linearization_نقطہ;

	طریقہ.سیٹ_Effectiveness_میٹرکس(surveyedHexarotor((1u << 0) | (1u << 1)), actuator_trim, linearization_نقطہ,
				      16, خطأ);
	EXPECT_EQ(طریقہ.getDroppedAxes(), 1u << Control_ونڈ::YAW);

	// yaw demand has nothing to act on
	سمتیہ<عائم, 6> control_sp;
	control_sp(Control_ونڈ::YAW) = 1.f;
	طریقہ.سیٹ_Control_Setpoint(control_sp);
	طریقہ.allocate();
	ثابت سمتیہ<عائم, 16> no_actuation;
	EXPECT_EQ(طریقہ.getActuatorSetpoint(), no_actuation);
	EXPECT_FLOAT_EQ(طریقہ.getAllocatedControl()(Control_ونڈ::YAW), 0.f);

	// remaining axes allocated exactly
	control_sp.سیٹ_Zero();
	control_sp(Control_ونڈ::ROLL) = 0.01f;
	control_sp(Control_ونڈ::PITCH) = 0.024f;
	control_sp(Control_ونڈ::THRUST_Z) = -0.001f;
	طریقہ.سیٹ_Control_Setpoint(control_sp);
	طریقہ.allocate();

	ثابت سمتیہ<عائم, 6> allocated = طریقہ.getAllocatedControl();
	EXPECT_NEAR(allocated(Control_ونڈ::ROLL), 0.01f, 1e-4f);
	EXPECT_NEAR(allocated(Control_ونڈ::PITCH), 0.024f, 1e-4f);
	EXPECT_NEAR(allocated(Control_ونڈ::THRUST_Z), -0.001f, 1e-4f);
	EXPECT_FLOAT_EQ(allocated(Control_ونڈ::YAW), 0.f);

	لكل (صحيح i = 0; i < 16; i++) {
		EXPECT_LT(fabsf(طریقہ.getActuatorSetpoint()(i)), 0.01f) << "actuator " << i;
	}
}

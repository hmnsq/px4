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

#अन्तर्भाव <gtest/gtest.h>
#अन्तर्भाव <ControlAllocationPseudoInverse.hpp>

उपयुज्य नामाकाश matrix;

TEST(Control_आवण्टनम्_Test, AllZeroCase)
{
	Control_आवण्टनम्_Pseudo_Inverse विधानम्;

	matrix::सदिशम्<भिन्न, 6> control_sp;
	matrix::सदिशम्<भिन्न, 6> control_allocated;
	matrix::सदिशम्<भिन्न, 6> control_allocated_expected;
	matrix::Matrix<भिन्न, 6, 16> effectiveness;
	matrix::सदिशम्<भिन्न, 16> actuator_sp;
	matrix::सदिशम्<भिन्न, 16> actuator_trim;
	matrix::सदिशम्<भिन्न, 16> linearization_point;
	matrix::सदिशम्<भिन्न, 16> actuator_sp_expected;

	विधानम्.समुच्चयः_Effectiveness_Matrix(effectiveness, actuator_trim, linearization_point, 16, असत्य);
	विधानम्.समुच्चयः_Control_Setpoint(control_sp);
	विधानम्.allocate();
	विधानम्.clipActuatorSetpoint();
	actuator_sp = विधानम्.getActuatorSetpoint();
	control_allocated_expected = विधानम्.getAllocatedControl();

	EXPECT_EQ(actuator_sp, actuator_sp_expected);
	EXPECT_EQ(control_allocated, control_allocated_expected);
}

TEST(Control_आवण्टनम्_Metric_Test, AllZeroCase)
{
	Control_आवण्टनम्_Pseudo_Inverse विधानम्;

	matrix::सदिशम्<भिन्न, 6> control_sp;
	matrix::सदिशम्<भिन्न, 6> control_allocated;
	matrix::सदिशम्<भिन्न, 6> control_allocated_expected;
	matrix::Matrix<भिन्न, 6, 16> effectiveness;
	matrix::सदिशम्<भिन्न, 16> actuator_sp;
	matrix::सदिशम्<भिन्न, 16> actuator_trim;
	matrix::सदिशम्<भिन्न, 16> linearization_point;
	matrix::सदिशम्<भिन्न, 16> actuator_sp_expected;

	विधानम्.समुच्चयः_Metric_आवण्टनम्(सत्य);
	विधानम्.समुच्चयः_Effectiveness_Matrix(effectiveness, actuator_trim, linearization_point, 16, असत्य);
	विधानम्.समुच्चयः_Control_Setpoint(control_sp);
	विधानम्.allocate();
	actuator_sp = विधानम्.getActuatorSetpoint();
	control_allocated_expected = विधानम्.getAllocatedControl();

	EXPECT_EQ(actuator_sp, actuator_sp_expected);
	EXPECT_EQ(control_allocated, control_allocated_expected);
}

नामाकाश
{

// Surveyed hexarotor; rows roll/pitch/yaw/thrust xyz, one column per rotor
constexpr भिन्न kSurveyedHexarotor[6][6] = {
	{-5.947500f,  5.934500f,  2.964000f, -2.853500f, -2.990000f,  2.840500f},
	{ 0.071500f,  0.071500f,  5.063500f, -4.972500f,  5.063500f, -4.972500f},
	{-0.325000f,  0.325000f, -0.325000f,  0.325000f,  0.325000f, -0.325000f},
	{ 0.f,        0.f,        0.f,        0.f,        0.f,        0.f},
	{ 0.f,        0.f,        0.f,        0.f,        0.f,        0.f},
	{-6.500000f, -6.500000f, -6.500000f, -6.500000f, -6.500000f, -6.500000f}
};

Matrix<भिन्न, 6, 16> surveyedHexarotor(uint16_t stopped_आवरणम्)
{
	Matrix<भिन्न, 6, 16> effectiveness;
	effectiveness.समुच्चयः_Zero();

	प्रति (पूर्णाङ्क i = 0; i < 6; i++) {
		यदि (stopped_आवरणम् & (1u << i)) {
			अनुवर्त;
		}

		प्रति (पूर्णाङ्क axis = 0; axis < 6; axis++) {
			effectiveness(axis, i) = kSurveyedHexarotor[axis][i];
		}
	}

	प्रत्यागम effectiveness;
}

संरचना Rotor {
	भिन्न px;
	भिन्न py;
	भिन्न km;
};

// upward rotors as in ActuatorEffectivenessRotors
Matrix<भिन्न, 6, 16> planarRotors(ध्रुव Rotor *rotors, पूर्णाङ्क गणनम्, uint16_t stopped_आवरणम् = 0)
{
	constexpr भिन्न ct = 6.5f;
	Matrix<भिन्न, 6, 16> effectiveness;
	effectiveness.समुच्चयः_Zero();

	प्रति (पूर्णाङ्क i = 0; i < गणनम्; i++) {
		यदि (stopped_आवरणम् & (1u << i)) {
			अनुवर्त;
		}

		effectiveness(0, i) = -ct * rotors[i].py;
		effectiveness(1, i) = ct * rotors[i].px;
		effectiveness(2, i) = ct * rotors[i].km;
		effectiveness(5, i) = -ct;
	}

	प्रत्यागम effectiveness;
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

TEST(Control_आवण्टनम्_Pseudo_Inverse_Test, DropDependentAxesKeepsIndependentGeometries)
{
	ध्रुव Matrix<भिन्न, 6, 16> independent[] = {
		planarRotors(kQuadX, 4),
		planarRotors(kHexaX, 6),
		planarRotors(kOctoX, 8),
		surveyedHexarotor(0),
		surveyedHexarotor(1u << 0),		// one motor removed: still rank 4
		planarRotors(kHexaX, 6, 1u << 2),
		planarRotors(kOctoX, 8, (1u << 0) | (1u << 1)),
	};

	प्रति (ध्रुव स्वचालित &original : independent) {
		Matrix<भिन्न, 6, 16> effectiveness = original;
		EXPECT_EQ(Control_आवण्टनम्_Pseudo_Inverse::dropDependentAxes(effectiveness), 0);
		EXPECT_EQ(effectiveness, original);
	}
}

TEST(Control_आवण्टनम्_Pseudo_Inverse_Test, DropDependentAxesDropsYawCollinearWithRoll)
{
	// motor and its opposite stopped: roll and yaw collinear, yaw must go, never roll
	ध्रुव Matrix<भिन्न, 6, 16> reduced[] = {
		surveyedHexarotor((1u << 0) | (1u << 1)),
		surveyedHexarotor((1u << 2) | (1u << 3)),
		surveyedHexarotor((1u << 4) | (1u << 5)),
		planarRotors(kHexaX, 6, (1u << 0) | (1u << 1)),
		planarRotors(kHexaX, 6, (1u << 2) | (1u << 3)),
	};

	प्रति (ध्रुव स्वचालित &original : reduced) {
		Matrix<भिन्न, 6, 16> effectiveness = original;
		ध्रुव uint8_t dropped = Control_आवण्टनम्_Pseudo_Inverse::dropDependentAxes(effectiveness);
		EXPECT_EQ(dropped, 1u << Control_आवण्टनम्::YAW);

		प्रति (पूर्णाङ्क axis = 0; axis < 6; axis++) {
			प्रति (पूर्णाङ्क i = 0; i < 16; i++) {
				ध्रुव भिन्न expected = (axis == Control_आवण्टनम्::YAW) ? 0.f : original(axis, i);
				EXPECT_FLOAT_EQ(effectiveness(axis, i), expected);
			}
		}
	}
}

TEST(Control_आवण्टनम्_Pseudo_Inverse_Test, DroppedAxisIsUnallocatedOthersExact)
{
	Control_आवण्टनम्_Pseudo_Inverse विधानम्;
	सदिशम्<भिन्न, 16> actuator_trim;
	सदिशम्<भिन्न, 16> linearization_point;

	विधानम्.समुच्चयः_Effectiveness_Matrix(surveyedHexarotor((1u << 0) | (1u << 1)), actuator_trim, linearization_point,
				      16, असत्य);
	EXPECT_EQ(विधानम्.getDroppedAxes(), 1u << Control_आवण्टनम्::YAW);

	// yaw demand has nothing to act on
	सदिशम्<भिन्न, 6> control_sp;
	control_sp(Control_आवण्टनम्::YAW) = 1.f;
	विधानम्.समुच्चयः_Control_Setpoint(control_sp);
	विधानम्.allocate();
	ध्रुव सदिशम्<भिन्न, 16> no_actuation;
	EXPECT_EQ(विधानम्.getActuatorSetpoint(), no_actuation);
	EXPECT_FLOAT_EQ(विधानम्.getAllocatedControl()(Control_आवण्टनम्::YAW), 0.f);

	// remaining axes allocated exactly
	control_sp.समुच्चयः_Zero();
	control_sp(Control_आवण्टनम्::ROLL) = 0.01f;
	control_sp(Control_आवण्टनम्::PITCH) = 0.024f;
	control_sp(Control_आवण्टनम्::THRUST_Z) = -0.001f;
	विधानम्.समुच्चयः_Control_Setpoint(control_sp);
	विधानम्.allocate();

	ध्रुव सदिशम्<भिन्न, 6> allocated = विधानम्.getAllocatedControl();
	EXPECT_NEAR(allocated(Control_आवण्टनम्::ROLL), 0.01f, 1e-4f);
	EXPECT_NEAR(allocated(Control_आवण्टनम्::PITCH), 0.024f, 1e-4f);
	EXPECT_NEAR(allocated(Control_आवण्टनम्::THRUST_Z), -0.001f, 1e-4f);
	EXPECT_FLOAT_EQ(allocated(Control_आवण्टनम्::YAW), 0.f);

	प्रति (पूर्णाङ्क i = 0; i < 16; i++) {
		EXPECT_LT(fabsf(विधानम्.getActuatorSetpoint()(i)), 0.01f) << "actuator " << i;
	}
}

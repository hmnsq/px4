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

#അന്തര്ഭാവ <gtest/gtest.h>
#അന്തര്ഭാവ <ControlAllocationPseudoInverse.hpp>

using നാമാകാശ matrix;

TEST(ControlAllocationTest, AllZeroCase)
{
	ControlAllocationPseudoInverse method;

	matrix::Vector<ദശാംശം, 6> control_sp;
	matrix::Vector<ദശാംശം, 6> control_allocated;
	matrix::Vector<ദശാംശം, 6> control_allocated_expected;
	matrix::Matrix<ദശാംശം, 6, 16> effectiveness;
	matrix::Vector<ദശാംശം, 16> actuator_sp;
	matrix::Vector<ദശാംശം, 16> actuator_trim;
	matrix::Vector<ദശാംശം, 16> linearization_point;
	matrix::Vector<ദശാംശം, 16> actuator_sp_expected;

	method.setEffectivenessMatrix(effectiveness, actuator_trim, linearization_point, 16, അസത്യം);
	method.setControlSetpoint(control_sp);
	method.allocate();
	method.clipActuatorSetpoint();
	actuator_sp = method.getActuatorSetpoint();
	control_allocated_expected = method.getAllocatedControl();

	EXPECT_EQ(actuator_sp, actuator_sp_expected);
	EXPECT_EQ(control_allocated, control_allocated_expected);
}

TEST(ControlAllocationMetricTest, AllZeroCase)
{
	ControlAllocationPseudoInverse method;

	matrix::Vector<ദശാംശം, 6> control_sp;
	matrix::Vector<ദശാംശം, 6> control_allocated;
	matrix::Vector<ദശാംശം, 6> control_allocated_expected;
	matrix::Matrix<ദശാംശം, 6, 16> effectiveness;
	matrix::Vector<ദശാംശം, 16> actuator_sp;
	matrix::Vector<ദശാംശം, 16> actuator_trim;
	matrix::Vector<ദശാംശം, 16> linearization_point;
	matrix::Vector<ദശാംശം, 16> actuator_sp_expected;

	method.setMetricAllocation(സത്യം);
	method.setEffectivenessMatrix(effectiveness, actuator_trim, linearization_point, 16, അസത്യം);
	method.setControlSetpoint(control_sp);
	method.allocate();
	actuator_sp = method.getActuatorSetpoint();
	control_allocated_expected = method.getAllocatedControl();

	EXPECT_EQ(actuator_sp, actuator_sp_expected);
	EXPECT_EQ(control_allocated, control_allocated_expected);
}

നാമാകാശ
{

// Surveyed hexarotor; rows roll/pitch/yaw/thrust xyz, one column per rotor
constexpr ദശാംശം kSurveyedHexarotor[6][6] = {
	{-5.947500f,  5.934500f,  2.964000f, -2.853500f, -2.990000f,  2.840500f},
	{ 0.071500f,  0.071500f,  5.063500f, -4.972500f,  5.063500f, -4.972500f},
	{-0.325000f,  0.325000f, -0.325000f,  0.325000f,  0.325000f, -0.325000f},
	{ 0.f,        0.f,        0.f,        0.f,        0.f,        0.f},
	{ 0.f,        0.f,        0.f,        0.f,        0.f,        0.f},
	{-6.500000f, -6.500000f, -6.500000f, -6.500000f, -6.500000f, -6.500000f}
};

Matrix<ദശാംശം, 6, 16> surveyedHexarotor(uint16_t stopped_mask)
{
	Matrix<ദശാംശം, 6, 16> effectiveness;
	effectiveness.setZero();

	വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < 6; i++) {
		എങ്കിൽ (stopped_mask & (1u << i)) {
			തുടരുക;
		}

		വേണ്ടി (പൂർണ്ണസംഖ്യ axis = 0; axis < 6; axis++) {
			effectiveness(axis, i) = kSurveyedHexarotor[axis][i];
		}
	}

	തിരികെ effectiveness;
}

ഘടന Rotor {
	ദശാംശം px;
	ദശാംശം py;
	ദശാംശം km;
};

// upward rotors as in ActuatorEffectivenessRotors
Matrix<ദശാംശം, 6, 16> planarRotors(സ്ഥിരം Rotor *rotors, പൂർണ്ണസംഖ്യ എണ്ണം, uint16_t stopped_mask = 0)
{
	constexpr ദശാംശം ct = 6.5f;
	Matrix<ദശാംശം, 6, 16> effectiveness;
	effectiveness.setZero();

	വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < എണ്ണം; i++) {
		എങ്കിൽ (stopped_mask & (1u << i)) {
			തുടരുക;
		}

		effectiveness(0, i) = -ct * rotors[i].py;
		effectiveness(1, i) = ct * rotors[i].px;
		effectiveness(2, i) = ct * rotors[i].km;
		effectiveness(5, i) = -ct;
	}

	തിരികെ effectiveness;
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

TEST(ControlAllocationPseudoInverseTest, DropDependentAxesKeepsIndependentGeometries)
{
	സ്ഥിരം Matrix<ദശാംശം, 6, 16> independent[] = {
		planarRotors(kQuadX, 4),
		planarRotors(kHexaX, 6),
		planarRotors(kOctoX, 8),
		surveyedHexarotor(0),
		surveyedHexarotor(1u << 0),		// one motor removed: still rank 4
		planarRotors(kHexaX, 6, 1u << 2),
		planarRotors(kOctoX, 8, (1u << 0) | (1u << 1)),
	};

	വേണ്ടി (സ്ഥിരം auto &original : independent) {
		Matrix<ദശാംശം, 6, 16> effectiveness = original;
		EXPECT_EQ(ControlAllocationPseudoInverse::dropDependentAxes(effectiveness), 0);
		EXPECT_EQ(effectiveness, original);
	}
}

TEST(ControlAllocationPseudoInverseTest, DropDependentAxesDropsYawCollinearWithRoll)
{
	// motor and its opposite stopped: roll and yaw collinear, yaw must go, never roll
	സ്ഥിരം Matrix<ദശാംശം, 6, 16> reduced[] = {
		surveyedHexarotor((1u << 0) | (1u << 1)),
		surveyedHexarotor((1u << 2) | (1u << 3)),
		surveyedHexarotor((1u << 4) | (1u << 5)),
		planarRotors(kHexaX, 6, (1u << 0) | (1u << 1)),
		planarRotors(kHexaX, 6, (1u << 2) | (1u << 3)),
	};

	വേണ്ടി (സ്ഥിരം auto &original : reduced) {
		Matrix<ദശാംശം, 6, 16> effectiveness = original;
		സ്ഥിരം uint8_t dropped = ControlAllocationPseudoInverse::dropDependentAxes(effectiveness);
		EXPECT_EQ(dropped, 1u << ControlAllocation::YAW);

		വേണ്ടി (പൂർണ്ണസംഖ്യ axis = 0; axis < 6; axis++) {
			വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < 16; i++) {
				സ്ഥിരം ദശാംശം expected = (axis == ControlAllocation::YAW) ? 0.f : original(axis, i);
				EXPECT_FLOAT_EQ(effectiveness(axis, i), expected);
			}
		}
	}
}

TEST(ControlAllocationPseudoInverseTest, DroppedAxisIsUnallocatedOthersExact)
{
	ControlAllocationPseudoInverse method;
	Vector<ദശാംശം, 16> actuator_trim;
	Vector<ദശാംശം, 16> linearization_point;

	method.setEffectivenessMatrix(surveyedHexarotor((1u << 0) | (1u << 1)), actuator_trim, linearization_point,
				      16, അസത്യം);
	EXPECT_EQ(method.getDroppedAxes(), 1u << ControlAllocation::YAW);

	// yaw demand has nothing to act on
	Vector<ദശാംശം, 6> control_sp;
	control_sp(ControlAllocation::YAW) = 1.f;
	method.setControlSetpoint(control_sp);
	method.allocate();
	സ്ഥിരം Vector<ദശാംശം, 16> no_actuation;
	EXPECT_EQ(method.getActuatorSetpoint(), no_actuation);
	EXPECT_FLOAT_EQ(method.getAllocatedControl()(ControlAllocation::YAW), 0.f);

	// remaining axes allocated exactly
	control_sp.setZero();
	control_sp(ControlAllocation::ROLL) = 0.01f;
	control_sp(ControlAllocation::PITCH) = 0.024f;
	control_sp(ControlAllocation::THRUST_Z) = -0.001f;
	method.setControlSetpoint(control_sp);
	method.allocate();

	സ്ഥിരം Vector<ദശാംശം, 6> allocated = method.getAllocatedControl();
	EXPECT_NEAR(allocated(ControlAllocation::ROLL), 0.01f, 1e-4f);
	EXPECT_NEAR(allocated(ControlAllocation::PITCH), 0.024f, 1e-4f);
	EXPECT_NEAR(allocated(ControlAllocation::THRUST_Z), -0.001f, 1e-4f);
	EXPECT_FLOAT_EQ(allocated(ControlAllocation::YAW), 0.f);

	വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < 16; i++) {
		EXPECT_LT(fabsf(method.getActuatorSetpoint()(i)), 0.01f) << "actuator " << i;
	}
}

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

#ಅನ್ತರ್ಭಾವ <gtest/gtest.h>
#ಅನ್ತರ್ಭಾವ <ControlAllocationPseudoInverse.hpp>

ಬಳಸಿ ನಾಮಾಕಾಶ ಮಾತೃಕೆ;

TEST(Control_ಹಂಚಿಕೆ_Test, AllZeroCase)
{
	Control_ಹಂಚಿಕೆ_Pseudo_Inverse ವಿಧಾನ;

	ಮಾತೃಕೆ::ಸದಿಶ<ದಶಮಾಂಶ, 6> control_sp;
	ಮಾತೃಕೆ::ಸದಿಶ<ದಶಮಾಂಶ, 6> control_allocated;
	ಮಾತೃಕೆ::ಸದಿಶ<ದಶಮಾಂಶ, 6> control_allocated_expected;
	ಮಾತೃಕೆ::ಮಾತೃಕೆ_2<ದಶಮಾಂಶ, 6, 16> effectiveness;
	ಮಾತೃಕೆ::ಸದಿಶ<ದಶಮಾಂಶ, 16> actuator_sp;
	ಮಾತೃಕೆ::ಸದಿಶ<ದಶಮಾಂಶ, 16> actuator_trim;
	ಮಾತೃಕೆ::ಸದಿಶ<ದಶಮಾಂಶ, 16> linearization_ಬಿಂದು;
	ಮಾತೃಕೆ::ಸದಿಶ<ದಶಮಾಂಶ, 16> actuator_sp_expected;

	ವಿಧಾನ.ಗಣ_Effectiveness_ಮಾತೃಕೆ(effectiveness, actuator_trim, linearization_ಬಿಂದು, 16, ಸುಳ್ಳು);
	ವಿಧಾನ.ಗಣ_Control_Setpoint(control_sp);
	ವಿಧಾನ.allocate();
	ವಿಧಾನ.clipActuatorSetpoint();
	actuator_sp = ವಿಧಾನ.getActuatorSetpoint();
	control_allocated_expected = ವಿಧಾನ.getAllocatedControl();

	EXPECT_EQ(actuator_sp, actuator_sp_expected);
	EXPECT_EQ(control_allocated, control_allocated_expected);
}

TEST(Control_ಹಂಚಿಕೆ_Metric_Test, AllZeroCase)
{
	Control_ಹಂಚಿಕೆ_Pseudo_Inverse ವಿಧಾನ;

	ಮಾತೃಕೆ::ಸದಿಶ<ದಶಮಾಂಶ, 6> control_sp;
	ಮಾತೃಕೆ::ಸದಿಶ<ದಶಮಾಂಶ, 6> control_allocated;
	ಮಾತೃಕೆ::ಸದಿಶ<ದಶಮಾಂಶ, 6> control_allocated_expected;
	ಮಾತೃಕೆ::ಮಾತೃಕೆ_2<ದಶಮಾಂಶ, 6, 16> effectiveness;
	ಮಾತೃಕೆ::ಸದಿಶ<ದಶಮಾಂಶ, 16> actuator_sp;
	ಮಾತೃಕೆ::ಸದಿಶ<ದಶಮಾಂಶ, 16> actuator_trim;
	ಮಾತೃಕೆ::ಸದಿಶ<ದಶಮಾಂಶ, 16> linearization_ಬಿಂದು;
	ಮಾತೃಕೆ::ಸದಿಶ<ದಶಮಾಂಶ, 16> actuator_sp_expected;

	ವಿಧಾನ.ಗಣ_Metric_ಹಂಚಿಕೆ(ನಿಜ);
	ವಿಧಾನ.ಗಣ_Effectiveness_ಮಾತೃಕೆ(effectiveness, actuator_trim, linearization_ಬಿಂದು, 16, ಸುಳ್ಳು);
	ವಿಧಾನ.ಗಣ_Control_Setpoint(control_sp);
	ವಿಧಾನ.allocate();
	actuator_sp = ವಿಧಾನ.getActuatorSetpoint();
	control_allocated_expected = ವಿಧಾನ.getAllocatedControl();

	EXPECT_EQ(actuator_sp, actuator_sp_expected);
	EXPECT_EQ(control_allocated, control_allocated_expected);
}

ನಾಮಾಕಾಶ
{

// Surveyed hexarotor; rows roll/pitch/yaw/thrust xyz, one column per rotor
constexpr ದಶಮಾಂಶ kSurveyedHexarotor[6][6] = {
	{-5.947500f,  5.934500f,  2.964000f, -2.853500f, -2.990000f,  2.840500f},
	{ 0.071500f,  0.071500f,  5.063500f, -4.972500f,  5.063500f, -4.972500f},
	{-0.325000f,  0.325000f, -0.325000f,  0.325000f,  0.325000f, -0.325000f},
	{ 0.f,        0.f,        0.f,        0.f,        0.f,        0.f},
	{ 0.f,        0.f,        0.f,        0.f,        0.f,        0.f},
	{-6.500000f, -6.500000f, -6.500000f, -6.500000f, -6.500000f, -6.500000f}
};

ಮಾತೃಕೆ_2<ದಶಮಾಂಶ, 6, 16> surveyedHexarotor(uint16_t stopped_ಮುಖವಾಡ)
{
	ಮಾತೃಕೆ_2<ದಶಮಾಂಶ, 6, 16> effectiveness;
	effectiveness.ಗಣ_Zero();

	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < 6; i++) {
		ಆದರೆ (stopped_ಮುಖವಾಡ & (1u << i)) {
			ಮುಂದುವರಿ;
		}

		ಗಾಗಿ (ಪೂರ್ಣಾಂಕ axis = 0; axis < 6; axis++) {
			effectiveness(axis, i) = kSurveyedHexarotor[axis][i];
		}
	}

	ಹಿಂತಿರುಗಿ effectiveness;
}

ರಚನೆ Rotor {
	ದಶಮಾಂಶ px;
	ದಶಮಾಂಶ py;
	ದಶಮಾಂಶ km;
};

// upward rotors as in ActuatorEffectivenessRotors
ಮಾತೃಕೆ_2<ದಶಮಾಂಶ, 6, 16> planarRotors(ಸ್ಥಿರಾಂಕ Rotor *rotors, ಪೂರ್ಣಾಂಕ ಎಣಿಕೆ_2, uint16_t stopped_ಮುಖವಾಡ = 0)
{
	constexpr ದಶಮಾಂಶ ct = 6.5f;
	ಮಾತೃಕೆ_2<ದಶಮಾಂಶ, 6, 16> effectiveness;
	effectiveness.ಗಣ_Zero();

	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < ಎಣಿಕೆ_2; i++) {
		ಆದರೆ (stopped_ಮುಖವಾಡ & (1u << i)) {
			ಮುಂದುವರಿ;
		}

		effectiveness(0, i) = -ct * rotors[i].py;
		effectiveness(1, i) = ct * rotors[i].px;
		effectiveness(2, i) = ct * rotors[i].km;
		effectiveness(5, i) = -ct;
	}

	ಹಿಂತಿರುಗಿ effectiveness;
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

TEST(Control_ಹಂಚಿಕೆ_Pseudo_Inverse_Test, DropDependentAxesKeepsIndependentGeometries)
{
	ಸ್ಥಿರಾಂಕ ಮಾತೃಕೆ_2<ದಶಮಾಂಶ, 6, 16> independent[] = {
		planarRotors(kQuadX, 4),
		planarRotors(kHexaX, 6),
		planarRotors(kOctoX, 8),
		surveyedHexarotor(0),
		surveyedHexarotor(1u << 0),		// one motor removed: still rank 4
		planarRotors(kHexaX, 6, 1u << 2),
		planarRotors(kOctoX, 8, (1u << 0) | (1u << 1)),
	};

	ಗಾಗಿ (ಸ್ಥಿರಾಂಕ ಸ್ವಯಂಚಾಲಿತ &original : independent) {
		ಮಾತೃಕೆ_2<ದಶಮಾಂಶ, 6, 16> effectiveness = original;
		EXPECT_EQ(Control_ಹಂಚಿಕೆ_Pseudo_Inverse::dropDependentAxes(effectiveness), 0);
		EXPECT_EQ(effectiveness, original);
	}
}

TEST(Control_ಹಂಚಿಕೆ_Pseudo_Inverse_Test, DropDependentAxesDropsYawCollinearWithRoll)
{
	// motor and its opposite stopped: roll and yaw collinear, yaw must go, never roll
	ಸ್ಥಿರಾಂಕ ಮಾತೃಕೆ_2<ದಶಮಾಂಶ, 6, 16> reduced[] = {
		surveyedHexarotor((1u << 0) | (1u << 1)),
		surveyedHexarotor((1u << 2) | (1u << 3)),
		surveyedHexarotor((1u << 4) | (1u << 5)),
		planarRotors(kHexaX, 6, (1u << 0) | (1u << 1)),
		planarRotors(kHexaX, 6, (1u << 2) | (1u << 3)),
	};

	ಗಾಗಿ (ಸ್ಥಿರಾಂಕ ಸ್ವಯಂಚಾಲಿತ &original : reduced) {
		ಮಾತೃಕೆ_2<ದಶಮಾಂಶ, 6, 16> effectiveness = original;
		ಸ್ಥಿರಾಂಕ uint8_t dropped = Control_ಹಂಚಿಕೆ_Pseudo_Inverse::dropDependentAxes(effectiveness);
		EXPECT_EQ(dropped, 1u << Control_ಹಂಚಿಕೆ::YAW);

		ಗಾಗಿ (ಪೂರ್ಣಾಂಕ axis = 0; axis < 6; axis++) {
			ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < 16; i++) {
				ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ expected = (axis == Control_ಹಂಚಿಕೆ::YAW) ? 0.f : original(axis, i);
				EXPECT_FLOAT_EQ(effectiveness(axis, i), expected);
			}
		}
	}
}

TEST(Control_ಹಂಚಿಕೆ_Pseudo_Inverse_Test, DroppedAxisIsUnallocatedOthersExact)
{
	Control_ಹಂಚಿಕೆ_Pseudo_Inverse ವಿಧಾನ;
	ಸದಿಶ<ದಶಮಾಂಶ, 16> actuator_trim;
	ಸದಿಶ<ದಶಮಾಂಶ, 16> linearization_ಬಿಂದು;

	ವಿಧಾನ.ಗಣ_Effectiveness_ಮಾತೃಕೆ(surveyedHexarotor((1u << 0) | (1u << 1)), actuator_trim, linearization_ಬಿಂದು,
				      16, ಸುಳ್ಳು);
	EXPECT_EQ(ವಿಧಾನ.getDroppedAxes(), 1u << Control_ಹಂಚಿಕೆ::YAW);

	// yaw demand has nothing to act on
	ಸದಿಶ<ದಶಮಾಂಶ, 6> control_sp;
	control_sp(Control_ಹಂಚಿಕೆ::YAW) = 1.f;
	ವಿಧಾನ.ಗಣ_Control_Setpoint(control_sp);
	ವಿಧಾನ.allocate();
	ಸ್ಥಿರಾಂಕ ಸದಿಶ<ದಶಮಾಂಶ, 16> no_actuation;
	EXPECT_EQ(ವಿಧಾನ.getActuatorSetpoint(), no_actuation);
	EXPECT_FLOAT_EQ(ವಿಧಾನ.getAllocatedControl()(Control_ಹಂಚಿಕೆ::YAW), 0.f);

	// remaining axes allocated exactly
	control_sp.ಗಣ_Zero();
	control_sp(Control_ಹಂಚಿಕೆ::ROLL) = 0.01f;
	control_sp(Control_ಹಂಚಿಕೆ::PITCH) = 0.024f;
	control_sp(Control_ಹಂಚಿಕೆ::THRUST_Z) = -0.001f;
	ವಿಧಾನ.ಗಣ_Control_Setpoint(control_sp);
	ವಿಧಾನ.allocate();

	ಸ್ಥಿರಾಂಕ ಸದಿಶ<ದಶಮಾಂಶ, 6> allocated = ವಿಧಾನ.getAllocatedControl();
	EXPECT_NEAR(allocated(Control_ಹಂಚಿಕೆ::ROLL), 0.01f, 1e-4f);
	EXPECT_NEAR(allocated(Control_ಹಂಚಿಕೆ::PITCH), 0.024f, 1e-4f);
	EXPECT_NEAR(allocated(Control_ಹಂಚಿಕೆ::THRUST_Z), -0.001f, 1e-4f);
	EXPECT_FLOAT_EQ(allocated(Control_ಹಂಚಿಕೆ::YAW), 0.f);

	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < 16; i++) {
		EXPECT_LT(fabsf(ವಿಧಾನ.getActuatorSetpoint()(i)), 0.01f) << "actuator " << i;
	}
}

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

#ਅਨ੍ਤਰ੍ਭਾਵ <gtest/gtest.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <ControlAllocationPseudoInverse.hpp>

ਵਰਤੋਂ ਨਾਮਾਕਾਸ਼ ਮੈਟ੍ਰਿਕਸ;

TEST(Control_ਵੰਡ_Test, AllZeroCase)
{
	Control_ਵੰਡ_Pseudo_Inverse ਵਿਧੀ;

	ਮੈਟ੍ਰਿਕਸ::ਸਦਿਸ਼<ਦਸ਼ਮਲਵ, 6> control_sp;
	ਮੈਟ੍ਰਿਕਸ::ਸਦਿਸ਼<ਦਸ਼ਮਲਵ, 6> control_allocated;
	ਮੈਟ੍ਰਿਕਸ::ਸਦਿਸ਼<ਦਸ਼ਮਲਵ, 6> control_allocated_expected;
	ਮੈਟ੍ਰਿਕਸ::ਮੈਟ੍ਰਿਕਸ_2<ਦਸ਼ਮਲਵ, 6, 16> effectiveness;
	ਮੈਟ੍ਰਿਕਸ::ਸਦਿਸ਼<ਦਸ਼ਮਲਵ, 16> actuator_sp;
	ਮੈਟ੍ਰਿਕਸ::ਸਦਿਸ਼<ਦਸ਼ਮਲਵ, 16> actuator_trim;
	ਮੈਟ੍ਰਿਕਸ::ਸਦਿਸ਼<ਦਸ਼ਮਲਵ, 16> linearization_ਬਿੰਦੂ;
	ਮੈਟ੍ਰਿਕਸ::ਸਦਿਸ਼<ਦਸ਼ਮਲਵ, 16> actuator_sp_expected;

	ਵਿਧੀ.ਸੈੱਟ_Effectiveness_ਮੈਟ੍ਰਿਕਸ(effectiveness, actuator_trim, linearization_ਬਿੰਦੂ, 16, ਝੂਠ);
	ਵਿਧੀ.ਸੈੱਟ_Control_Setpoint(control_sp);
	ਵਿਧੀ.allocate();
	ਵਿਧੀ.clipActuatorSetpoint();
	actuator_sp = ਵਿਧੀ.getActuatorSetpoint();
	control_allocated_expected = ਵਿਧੀ.getAllocatedControl();

	EXPECT_EQ(actuator_sp, actuator_sp_expected);
	EXPECT_EQ(control_allocated, control_allocated_expected);
}

TEST(Control_ਵੰਡ_Metric_Test, AllZeroCase)
{
	Control_ਵੰਡ_Pseudo_Inverse ਵਿਧੀ;

	ਮੈਟ੍ਰਿਕਸ::ਸਦਿਸ਼<ਦਸ਼ਮਲਵ, 6> control_sp;
	ਮੈਟ੍ਰਿਕਸ::ਸਦਿਸ਼<ਦਸ਼ਮਲਵ, 6> control_allocated;
	ਮੈਟ੍ਰਿਕਸ::ਸਦਿਸ਼<ਦਸ਼ਮਲਵ, 6> control_allocated_expected;
	ਮੈਟ੍ਰਿਕਸ::ਮੈਟ੍ਰਿਕਸ_2<ਦਸ਼ਮਲਵ, 6, 16> effectiveness;
	ਮੈਟ੍ਰਿਕਸ::ਸਦਿਸ਼<ਦਸ਼ਮਲਵ, 16> actuator_sp;
	ਮੈਟ੍ਰਿਕਸ::ਸਦਿਸ਼<ਦਸ਼ਮਲਵ, 16> actuator_trim;
	ਮੈਟ੍ਰਿਕਸ::ਸਦਿਸ਼<ਦਸ਼ਮਲਵ, 16> linearization_ਬਿੰਦੂ;
	ਮੈਟ੍ਰਿਕਸ::ਸਦਿਸ਼<ਦਸ਼ਮਲਵ, 16> actuator_sp_expected;

	ਵਿਧੀ.ਸੈੱਟ_Metric_ਵੰਡ(ਸੱਚ);
	ਵਿਧੀ.ਸੈੱਟ_Effectiveness_ਮੈਟ੍ਰਿਕਸ(effectiveness, actuator_trim, linearization_ਬਿੰਦੂ, 16, ਝੂਠ);
	ਵਿਧੀ.ਸੈੱਟ_Control_Setpoint(control_sp);
	ਵਿਧੀ.allocate();
	actuator_sp = ਵਿਧੀ.getActuatorSetpoint();
	control_allocated_expected = ਵਿਧੀ.getAllocatedControl();

	EXPECT_EQ(actuator_sp, actuator_sp_expected);
	EXPECT_EQ(control_allocated, control_allocated_expected);
}

ਨਾਮਾਕਾਸ਼
{

// Surveyed hexarotor; rows roll/pitch/yaw/thrust xyz, one column per rotor
constexpr ਦਸ਼ਮਲਵ kSurveyedHexarotor[6][6] = {
	{-5.947500f,  5.934500f,  2.964000f, -2.853500f, -2.990000f,  2.840500f},
	{ 0.071500f,  0.071500f,  5.063500f, -4.972500f,  5.063500f, -4.972500f},
	{-0.325000f,  0.325000f, -0.325000f,  0.325000f,  0.325000f, -0.325000f},
	{ 0.f,        0.f,        0.f,        0.f,        0.f,        0.f},
	{ 0.f,        0.f,        0.f,        0.f,        0.f,        0.f},
	{-6.500000f, -6.500000f, -6.500000f, -6.500000f, -6.500000f, -6.500000f}
};

ਮੈਟ੍ਰਿਕਸ_2<ਦਸ਼ਮਲਵ, 6, 16> surveyedHexarotor(uint16_t stopped_ਨਕਾਬ)
{
	ਮੈਟ੍ਰਿਕਸ_2<ਦਸ਼ਮਲਵ, 6, 16> effectiveness;
	effectiveness.ਸੈੱਟ_Zero();

	ਲਈ (ਪੂਰਨ i = 0; i < 6; i++) {
		ਜੇ (stopped_ਨਕਾਬ & (1u << i)) {
			ਜਾਰੀ;
		}

		ਲਈ (ਪੂਰਨ axis = 0; axis < 6; axis++) {
			effectiveness(axis, i) = kSurveyedHexarotor[axis][i];
		}
	}

	ਵਾਪਸ effectiveness;
}

ਬਣਤਰ Rotor {
	ਦਸ਼ਮਲਵ px;
	ਦਸ਼ਮਲਵ py;
	ਦਸ਼ਮਲਵ km;
};

// upward rotors as in ActuatorEffectivenessRotors
ਮੈਟ੍ਰਿਕਸ_2<ਦਸ਼ਮਲਵ, 6, 16> planarRotors(ਅਚਲ Rotor *rotors, ਪੂਰਨ ਗਿਣਤੀ_2, uint16_t stopped_ਨਕਾਬ = 0)
{
	constexpr ਦਸ਼ਮਲਵ ct = 6.5f;
	ਮੈਟ੍ਰਿਕਸ_2<ਦਸ਼ਮਲਵ, 6, 16> effectiveness;
	effectiveness.ਸੈੱਟ_Zero();

	ਲਈ (ਪੂਰਨ i = 0; i < ਗਿਣਤੀ_2; i++) {
		ਜੇ (stopped_ਨਕਾਬ & (1u << i)) {
			ਜਾਰੀ;
		}

		effectiveness(0, i) = -ct * rotors[i].py;
		effectiveness(1, i) = ct * rotors[i].px;
		effectiveness(2, i) = ct * rotors[i].km;
		effectiveness(5, i) = -ct;
	}

	ਵਾਪਸ effectiveness;
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

TEST(Control_ਵੰਡ_Pseudo_Inverse_Test, DropDependentAxesKeepsIndependentGeometries)
{
	ਅਚਲ ਮੈਟ੍ਰਿਕਸ_2<ਦਸ਼ਮਲਵ, 6, 16> independent[] = {
		planarRotors(kQuadX, 4),
		planarRotors(kHexaX, 6),
		planarRotors(kOctoX, 8),
		surveyedHexarotor(0),
		surveyedHexarotor(1u << 0),		// one motor removed: still rank 4
		planarRotors(kHexaX, 6, 1u << 2),
		planarRotors(kOctoX, 8, (1u << 0) | (1u << 1)),
	};

	ਲਈ (ਅਚਲ ਸਵੈਚਾਲਿਤ &original : independent) {
		ਮੈਟ੍ਰਿਕਸ_2<ਦਸ਼ਮਲਵ, 6, 16> effectiveness = original;
		EXPECT_EQ(Control_ਵੰਡ_Pseudo_Inverse::dropDependentAxes(effectiveness), 0);
		EXPECT_EQ(effectiveness, original);
	}
}

TEST(Control_ਵੰਡ_Pseudo_Inverse_Test, DropDependentAxesDropsYawCollinearWithRoll)
{
	// motor and its opposite stopped: roll and yaw collinear, yaw must go, never roll
	ਅਚਲ ਮੈਟ੍ਰਿਕਸ_2<ਦਸ਼ਮਲਵ, 6, 16> reduced[] = {
		surveyedHexarotor((1u << 0) | (1u << 1)),
		surveyedHexarotor((1u << 2) | (1u << 3)),
		surveyedHexarotor((1u << 4) | (1u << 5)),
		planarRotors(kHexaX, 6, (1u << 0) | (1u << 1)),
		planarRotors(kHexaX, 6, (1u << 2) | (1u << 3)),
	};

	ਲਈ (ਅਚਲ ਸਵੈਚਾਲਿਤ &original : reduced) {
		ਮੈਟ੍ਰਿਕਸ_2<ਦਸ਼ਮਲਵ, 6, 16> effectiveness = original;
		ਅਚਲ uint8_t dropped = Control_ਵੰਡ_Pseudo_Inverse::dropDependentAxes(effectiveness);
		EXPECT_EQ(dropped, 1u << Control_ਵੰਡ::YAW);

		ਲਈ (ਪੂਰਨ axis = 0; axis < 6; axis++) {
			ਲਈ (ਪੂਰਨ i = 0; i < 16; i++) {
				ਅਚਲ ਦਸ਼ਮਲਵ expected = (axis == Control_ਵੰਡ::YAW) ? 0.f : original(axis, i);
				EXPECT_FLOAT_EQ(effectiveness(axis, i), expected);
			}
		}
	}
}

TEST(Control_ਵੰਡ_Pseudo_Inverse_Test, DroppedAxisIsUnallocatedOthersExact)
{
	Control_ਵੰਡ_Pseudo_Inverse ਵਿਧੀ;
	ਸਦਿਸ਼<ਦਸ਼ਮਲਵ, 16> actuator_trim;
	ਸਦਿਸ਼<ਦਸ਼ਮਲਵ, 16> linearization_ਬਿੰਦੂ;

	ਵਿਧੀ.ਸੈੱਟ_Effectiveness_ਮੈਟ੍ਰਿਕਸ(surveyedHexarotor((1u << 0) | (1u << 1)), actuator_trim, linearization_ਬਿੰਦੂ,
				      16, ਝੂਠ);
	EXPECT_EQ(ਵਿਧੀ.getDroppedAxes(), 1u << Control_ਵੰਡ::YAW);

	// yaw demand has nothing to act on
	ਸਦਿਸ਼<ਦਸ਼ਮਲਵ, 6> control_sp;
	control_sp(Control_ਵੰਡ::YAW) = 1.f;
	ਵਿਧੀ.ਸੈੱਟ_Control_Setpoint(control_sp);
	ਵਿਧੀ.allocate();
	ਅਚਲ ਸਦਿਸ਼<ਦਸ਼ਮਲਵ, 16> no_actuation;
	EXPECT_EQ(ਵਿਧੀ.getActuatorSetpoint(), no_actuation);
	EXPECT_FLOAT_EQ(ਵਿਧੀ.getAllocatedControl()(Control_ਵੰਡ::YAW), 0.f);

	// remaining axes allocated exactly
	control_sp.ਸੈੱਟ_Zero();
	control_sp(Control_ਵੰਡ::ROLL) = 0.01f;
	control_sp(Control_ਵੰਡ::PITCH) = 0.024f;
	control_sp(Control_ਵੰਡ::THRUST_Z) = -0.001f;
	ਵਿਧੀ.ਸੈੱਟ_Control_Setpoint(control_sp);
	ਵਿਧੀ.allocate();

	ਅਚਲ ਸਦਿਸ਼<ਦਸ਼ਮਲਵ, 6> allocated = ਵਿਧੀ.getAllocatedControl();
	EXPECT_NEAR(allocated(Control_ਵੰਡ::ROLL), 0.01f, 1e-4f);
	EXPECT_NEAR(allocated(Control_ਵੰਡ::PITCH), 0.024f, 1e-4f);
	EXPECT_NEAR(allocated(Control_ਵੰਡ::THRUST_Z), -0.001f, 1e-4f);
	EXPECT_FLOAT_EQ(allocated(Control_ਵੰਡ::YAW), 0.f);

	ਲਈ (ਪੂਰਨ i = 0; i < 16; i++) {
		EXPECT_LT(fabsf(ਵਿਧੀ.getActuatorSetpoint()(i)), 0.01f) << "actuator " << i;
	}
}

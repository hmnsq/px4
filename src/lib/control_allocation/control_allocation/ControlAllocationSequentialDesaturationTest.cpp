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

#ಅನ್ತರ್ಭಾವ <gtest/gtest.h>
#ಅನ್ತರ್ಭಾವ <ActuatorEffectivenessRotors.hpp>
#ಅನ್ತರ್ಭಾವ "ControlAllocationSequentialDesaturation.hpp"

ಬಳಸಿ ನಾಮಾಕಾಶ ಮಾತೃಕೆ;
ಬಳಸಿ Actuator_ಸದಿಶ = Control_ಹಂಚಿಕೆ::Actuator_ಸದಿಶ;

TEST(Control_ಹಂಚಿಕೆ_Sequential_Desaturation_Test, AllZeroCase)
{
	Control_ಹಂಚಿಕೆ_Sequential_Desaturation control_ಹಂಚಿಕೆ;
	EXPECT_EQ(control_ಹಂಚಿಕೆ.getActuatorSetpoint(), Actuator_ಸದಿಶ());
	control_ಹಂಚಿಕೆ.allocate();
	EXPECT_EQ(control_ಹಂಚಿಕೆ.getActuatorSetpoint(), Actuator_ಸದಿಶ());
}

TEST(Control_ಹಂಚಿಕೆ_Sequential_Desaturation_Test, ಗಣ_Get_Actuator_Setpoint)
{
	Control_ಹಂಚಿಕೆ_Sequential_Desaturation control_ಹಂಚಿಕೆ;
	ದಶಮಾಂಶ actuator_setpoint_array[Control_ಹಂಚಿಕೆ::ಅಂಕೆ_ACTUATORS] = {1.f, 2.f, 3.f, 4.f, 5.f, 6.f};
	Actuator_ಸದಿಶ actuator_setpoint(actuator_setpoint_array);
	control_ಹಂಚಿಕೆ.ಗಣ_Actuator_Setpoint(actuator_setpoint);
	EXPECT_EQ(control_ಹಂಚಿಕೆ.getActuatorSetpoint(), actuator_setpoint);
}

ವರ್ಗ Control_ಹಂಚಿಕೆ_Sequential_Desaturation_Test_Quad_X : ಸಾರ್ವಜನಿಕ ::testing::Test
{
ಸಾರ್ವಜನಿಕ:
	ಸ್ಥಿರ constexpr uint8_t ಅಂಕೆ_ACTUATORS = 4;
	Control_ಹಂಚಿಕೆ_Sequential_Desaturation control_ಹಂಚಿಕೆ_2;

	ಶೂನ್ಯ ಗಣ_ಮೇಲೆ() override
	{
		param_control_autosave(ಸುಳ್ಳು); // Disable autosaving parameters to avoid busy loop in param_set()
		ಗಣ_Airmode(0); // No airmode by default

		// Quadrotor x geometry
		ActuatorEffectivenessRotors::Geometry quadx_geometry{};
		quadx_geometry.ಅಂಕೆ_rotors = 4;
		quadx_geometry.rotors[0].position = {1.f, 1.f, 0.f}; // clockwise motor numbering
		quadx_geometry.rotors[1].position = {-1.f, 1.f, 0.f};
		quadx_geometry.rotors[2].position = {-1.f, -1.f, 0.f};
		quadx_geometry.rotors[3].position = {1.f, -1.f, 0.f};
		quadx_geometry.rotors[0].moment_ಅನುಪಾತ = 1.f;
		quadx_geometry.rotors[1].moment_ಅನುಪಾತ = -1.f;
		quadx_geometry.rotors[2].moment_ಅನುಪಾತ = 1.f;
		quadx_geometry.rotors[3].moment_ಅನುಪಾತ = -1.f;

		ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < 4; ++i) {
			quadx_geometry.rotors[i].axis = Vector3f(0.f, 0.f, -1.f); // thrust downwards
			quadx_geometry.rotors[i].thrust_coef = 1.f;
			quadx_geometry.rotors[i].tilt_ಸೂಚ್ಯಂಕ = -1;
		}

		// Compute actuator effectiveness
		ActuatorEffectiveness::Configuration actuator_configuration{};
		ಪೂರ್ಣಾಂಕ ಅಂಕೆ_actuators = ActuatorEffectivenessRotors::compute_Effectiveness_ಮಾತೃಕೆ(quadx_geometry,
				    actuator_configuration.effectiveness_matrices[0],
				    actuator_configuration.ಅಂಕೆ_actuators_ಮಾತೃಕೆ[0]);
		EXPECT_EQ(ಅಂಕೆ_actuators, ಅಂಕೆ_ACTUATORS);
		actuator_configuration.actuatorsAdded(ActuatorType::MOTORS, ಅಂಕೆ_actuators);

		// Load effectiveness into allocation
		control_ಹಂಚಿಕೆ_2.ಗಣ_Effectiveness_ಮಾತೃಕೆ(actuator_configuration.effectiveness_matrices[0],
				actuator_configuration.trim[0], actuator_configuration.linearization_ಬಿಂದು[0],
				actuator_configuration.ಅಂಕೆ_actuators_ಮಾತೃಕೆ[0], ನಿಜ /*update_normalization_scale*/);
	}

	ಶೂನ್ಯ ಗಣ_Airmode(ಸ್ಥಿರಾಂಕ int32_t ಬಹುಲಕ)
	{
		param_t param = param_ಕಂಡುಹಿಡಿ("MC_AIRMODE");
		param_ಗಣ(param, &ಬಹುಲಕ);
		control_ಹಂಚಿಕೆ_2.updateParameters();
	}

	Vector4f allocate(ದಶಮಾಂಶ roll, ದಶಮಾಂಶ pitch, ದಶಮಾಂಶ yaw, ದಶಮಾಂಶ thrust)
	{
		ಸದಿಶ<ದಶಮಾಂಶ, Control_ಹಂಚಿಕೆ::ಅಂಕೆ_AXES> control_setpoint{};
		control_setpoint(Control_ಹಂಚಿಕೆ::ControlAxis::ROLL) = roll;
		control_setpoint(Control_ಹಂಚಿಕೆ::ControlAxis::PITCH) = pitch;
		control_setpoint(Control_ಹಂಚಿಕೆ::ControlAxis::YAW) = yaw;
		control_setpoint(Control_ಹಂಚಿಕೆ::ControlAxis::THRUST_Z) = thrust;
		control_ಹಂಚಿಕೆ_2.ಗಣ_Control_Setpoint(control_setpoint);
		control_ಹಂಚಿಕೆ_2.allocate();
		ಹಿಂತಿರುಗಿ getQuadOutputs();
	}

	Vector4f getQuadOutputs()
	{
		ಸ್ಥಿರಾಂಕ Actuator_ಸದಿಶ &actuator_setpoint = control_ಹಂಚಿಕೆ_2.getActuatorSetpoint();
		// All unused actuators shall stay zero
		ಸ್ಥಿರ constexpr uint8_t ಅಂಕೆ_ಬಳಸದ_ACTUATORS = Control_ಹಂಚಿಕೆ::ಅಂಕೆ_ACTUATORS - 4;
		EXPECT_EQ(
			(ಸದಿಶ<ದಶಮಾಂಶ, ಅಂಕೆ_ಬಳಸದ_ACTUATORS>(actuator_setpoint.ತುಂಡು<ಅಂಕೆ_ಬಳಸದ_ACTUATORS, 1>(4, 0))),
			(ಸದಿಶ<ದಶಮಾಂಶ, ಅಂಕೆ_ಬಳಸದ_ACTUATORS>())
		);
		ಹಿಂತಿರುಗಿ Vector4f(actuator_setpoint.ತುಂಡು<4, 1>(0, 0));
	}
};

// Make constant available, see https://stackoverflow.com/questions/42756443/undefined-reference-with-gtest
constexpr uint8_t Control_ಹಂಚಿಕೆ_Sequential_Desaturation_Test_Quad_X::ಅಂಕೆ_ACTUATORS;

TEST_F(Control_ಹಂಚಿಕೆ_Sequential_Desaturation_Test_Quad_X, Zero)
{
	EXPECT_EQ(allocate(0.f, 0.f, 0.f, 0.f), Vector4f());
}


TEST_F(Control_ಹಂಚಿಕೆ_Sequential_Desaturation_Test_Quad_X, CollectiveThrust)
{
	ಗಾಗಿ (ದಶಮಾಂಶ thrust = 0.f; thrust <= (1.f + FLT_EPSILON); thrust += .1f) {
		EXPECT_EQ(allocate(0.f, 0.f, 0.f, -thrust), Vector4f(thrust, thrust, thrust, thrust));
	}
}

TEST_F(Control_ಹಂಚಿಕೆ_Sequential_Desaturation_Test_Quad_X, RollPitchYaw)
{
	EXPECT_EQ(allocate(1.f, 0.f, 0.f, -.5f), Vector4f(.25f, .25f, .75f, .75f));
	EXPECT_EQ(allocate(-1.f, 0.f, 0.f, -.5f), Vector4f(.75f, .75f, .25f, .25f));
	EXPECT_EQ(allocate(0.f, 1.f, 0.f, -.5f), Vector4f(.75f, .25f, .25f, .75f));
	EXPECT_EQ(allocate(0.f, -1.f, 0.f, -.5f), Vector4f(.25f, .75f, .75f, .25f));
	EXPECT_EQ(allocate(0.f, 0.f, 1.f, -.5f), Vector4f(.75f, .25f, .75f, .25f));
	EXPECT_EQ(allocate(0.f, 0.f, -1.f, -.5f), Vector4f(.25f, .75f, .25f, .75f));
}

TEST_F(Control_ಹಂಚಿಕೆ_Sequential_Desaturation_Test_Quad_X, Roll_Pitch_Yaw_ತುಂಬಿದ_Thrust)
{
	EXPECT_EQ(allocate(1.f, 0.f, 0.f, -1.f), Vector4f(.5f, .5f, 1.f, 1.f));
	EXPECT_EQ(allocate(-1.f, 0.f, 0.f, -1.f), Vector4f(1.f, 1.f, .5f, .5f));
	EXPECT_EQ(allocate(0.f, 1.f, 0.f, -1.f), Vector4f(1.f, .5f, .5f, 1.f));
	EXPECT_EQ(allocate(0.f, -1.f, 0.f, -1.f), Vector4f(.5f, 1.f, 1.f, .5f));
	// There is a special case to deprioritize yaw down to 30% authority with maximum thrust
	EXPECT_EQ(allocate(0.f, 0.f, 1.f, -1.f), Vector4f(1.f, .7f, 1.f, .7f));
	EXPECT_EQ(allocate(0.f, 0.f, -1.f, -1.f), Vector4f(.7f, 1.f, .7f, 1.f));
}

TEST_F(Control_ಹಂಚಿಕೆ_Sequential_Desaturation_Test_Quad_X, RollPitchYawZeroThrust)
{
	// No axis is allocated
	EXPECT_EQ(allocate(1.f, 0.f, 0.f, 0.f), Vector4f());
	EXPECT_EQ(allocate(-1.f, 0.f, 0.f, 0.f), Vector4f());
	EXPECT_EQ(allocate(0.f, 1.f, 0.f, 0.f), Vector4f());
	EXPECT_EQ(allocate(0.f, -1.f, 0.f, 0.f), Vector4f());
	EXPECT_EQ(allocate(0.f, 0.f, 1.f, 0.f), Vector4f());
	EXPECT_EQ(allocate(0.f, 0.f, -1.f, 0.f), Vector4f());
}

TEST_F(Control_ಹಂಚಿಕೆ_Sequential_Desaturation_Test_Quad_X, RollPitchYawZeroThrustAirmodeRP)
{
	ಗಣ_Airmode(1); // Roll and pitch airmode
	// Roll and pitch get fully allocated
	EXPECT_EQ(allocate(1.f, 0.f, 0.f, 0.f), Vector4f(0.f, 0.f, 0.5f, 0.5f));
	EXPECT_EQ(allocate(-1.f, 0.f, 0.f, 0.f), Vector4f(0.5f, 0.5f, 0.f, 0.f));
	EXPECT_EQ(allocate(0.f, 1.f, 0.f, 0.f), Vector4f(0.5f, 0.f, 0.f, 0.5f));
	EXPECT_EQ(allocate(0.f, -1.f, 0.f, 0.f), Vector4f(0.f, 0.5f, 0.5f, 0.f));
	// Yaw is not allocated
	EXPECT_EQ(allocate(0.f, 0.f, 1.f, 0.f), Vector4f());
	EXPECT_EQ(allocate(0.f, 0.f, -1.f, 0.f), Vector4f());
}

TEST_F(Control_ಹಂಚಿಕೆ_Sequential_Desaturation_Test_Quad_X, RollPitchYawZeroThrustAirmodeRPY)
{
	ಗಣ_Airmode(2); // Roll, pitch and yaw airmode
	// All axis are fully allocated
	EXPECT_EQ(allocate(1.f, 0.f, 0.f, 0.f), Vector4f(0.f, 0.f, 0.5f, 0.5f));
	EXPECT_EQ(allocate(-1.f, 0.f, 0.f, 0.f), Vector4f(0.5f, 0.5f, 0.f, 0.f));
	EXPECT_EQ(allocate(0.f, 1.f, 0.f, 0.f), Vector4f(0.5f, 0.f, 0.f, 0.5f));
	EXPECT_EQ(allocate(0.f, -1.f, 0.f, 0.f), Vector4f(0.f, 0.5f, 0.5f, 0.f));
	EXPECT_EQ(allocate(0.f, 0.f, 1.f, 0.f), Vector4f(.5f, 0.f, .5f, 0.f));
	EXPECT_EQ(allocate(0.f, 0.f, -1.f, 0.f), Vector4f(0.f, .5f, 0.f, .5f));
}

// This tests that yaw-only control setpoint at zero actuator setpoint results in zero actuator
// allocation.
TEST_F(Control_ಹಂಚಿಕೆ_Sequential_Desaturation_Test_Quad_X, AirmodeDisabledOnlyYaw)
{
	EXPECT_EQ(allocate(0.f, 0.f, 1.f, 0.f), Vector4f(0.f, 0.f, 0.f, 0.f));
}

// This tests that a control setpoint for z-thrust returns the desired actuator setpoint.
// Each motor should have an actuator setpoint that when summed together should be equal to
// control setpoint.
TEST_F(Control_ಹಂಚಿಕೆ_Sequential_Desaturation_Test_Quad_X, AirmodeDisabledThrustZ)
{
	constexpr ದಶಮಾಂಶ THRUST = 0.75f;
	EXPECT_EQ(allocate(0.f, 0.f, 0.f, -THRUST), Vector4f(THRUST, THRUST, THRUST, THRUST));
}

// This tests that a control setpoint for z-thrust + yaw returns the desired actuator setpoint.
// This test does not saturate the yaw response.
TEST_F(Control_ಹಂಚಿಕೆ_Sequential_Desaturation_Test_Quad_X, AirmodeDisabledThrustAndYaw)
{
	constexpr ದಶಮಾಂಶ THRUST = 0.75f;
	constexpr ದಶಮಾಂಶ YAW_TORQUE = 0.02f;
	constexpr ದಶಮಾಂಶ YAW = YAW_TORQUE / ಅಂಕೆ_ACTUATORS;
	EXPECT_EQ(allocate(0.f, 0.f, YAW_TORQUE, -THRUST), Vector4f(THRUST + YAW, THRUST - YAW, THRUST + YAW, THRUST - YAW));
}

// This tests that a control setpoint for z-thrust + yaw returns the desired actuator setpoint.
// This test saturates the yaw response, but does not reduce total thrust.
TEST_F(Control_ಹಂಚಿಕೆ_Sequential_Desaturation_Test_Quad_X, AirmodeDisabledThrustAndSaturatedYaw)
{
	constexpr ದಶಮಾಂಶ THRUST = 0.75f;
	constexpr ದಶಮಾಂಶ YAW_TORQUE = 1.f;
	constexpr ದಶಮಾಂಶ YAW = YAW_TORQUE / ಅಂಕೆ_ACTUATORS;
	EXPECT_EQ(allocate(0.f, 0.f, YAW_TORQUE, -THRUST), Vector4f(THRUST + YAW, THRUST - YAW, THRUST + YAW, THRUST - YAW));
}

// This tests that a control setpoint for z-thrust + pitch returns the desired actuator setpoint.
// This test does not saturate the pitch response.
TEST_F(Control_ಹಂಚಿಕೆ_Sequential_Desaturation_Test_Quad_X, AirmodeDisabledThrustAndPitch)
{
	constexpr ದಶಮಾಂಶ THRUST = 0.75f;
	constexpr ದಶಮಾಂಶ PITCH_TORQUE = 0.1f;
	constexpr ದಶಮಾಂಶ PITCH = PITCH_TORQUE / ಅಂಕೆ_ACTUATORS;
	EXPECT_EQ(allocate(0.f, PITCH_TORQUE, 0.f, -THRUST),
		  Vector4f(THRUST + PITCH, THRUST - PITCH, THRUST - PITCH, THRUST + PITCH));
}

// This tests that a control setpoint for z-thrust + yaw returns the desired actuator setpoint.
// This test saturates yaw and demonstrates reduction of thrust for yaw.
TEST_F(Control_ಹಂಚಿಕೆ_Sequential_Desaturation_Test_Quad_X, AirmodeDisabledReducedThrustAndYaw)
{
	constexpr ದಶಮಾಂಶ YAW_MARGIN = Control_ಹಂಚಿಕೆ_Sequential_Desaturation::MINIMUM_YAW_MARGIN;
	EXPECT_EQ(allocate(0.f, 0.f, 1.f, -3.2f), Vector4f(1.f, 1.f - (2.f * YAW_MARGIN), 1.f, 1.f - (2.f * YAW_MARGIN)));
}

// This tests that a control setpoint for z-thrust + pitch returns the desired actuator setpoint.
// This test saturates the pitch response such that thrust is reduced to (partially) compensate.
TEST_F(Control_ಹಂಚಿಕೆ_Sequential_Desaturation_Test_Quad_X, AirmodeDisabledReducedThrustAndPitch)
{
	EXPECT_EQ(allocate(0.f, 2.f, 0.f, -3.f), Vector4f(1.f, 0.f, 0.f, 1.f));
}

TEST_F(Control_ಹಂಚಿಕೆ_Sequential_Desaturation_Test_Quad_X, ಹಿಂದಿನ_Mixing_Tests_No_Airmode)
{
	ಗಣ_Airmode(0); // No airmode
	EXPECT_EQ(allocate(0.000f, 0.000f, 0.000f, -0.000f), Vector4f(0.000000f, 0.000000f, 0.000000f, 0.000000f)); // 1
	EXPECT_EQ(allocate(0.000f, 0.000f, 0.000f, -0.100f), Vector4f(0.100000f, 0.100000f, 0.100000f, 0.100000f)); // 2
	EXPECT_EQ(allocate(0.000f, 0.000f, 0.000f, -0.450f), Vector4f(0.450000f, 0.450000f, 0.450000f, 0.450000f)); // 3
	EXPECT_EQ(allocate(0.000f, 0.000f, 0.000f, -0.900f), Vector4f(0.900000f, 0.900000f, 0.900000f, 0.900000f)); // 4
	EXPECT_EQ(allocate(0.000f, 0.000f, 0.000f, -1.000f), Vector4f(1.000000f, 1.000000f, 1.000000f, 1.000000f)); // 5
	EXPECT_EQ(allocate(-0.050f, 0.000f, 0.000f, -0.000f), Vector4f(0.000000f, 0.000000f, 0.000000f, 0.000000f)); // 6
	EXPECT_EQ(allocate(-0.050f, 0.000f, 0.000f, -0.100f), Vector4f(0.112500f, 0.112500f, 0.087500f, 0.087500f)); // 7
	EXPECT_EQ(allocate(-0.050f, 0.000f, 0.000f, -0.450f), Vector4f(0.462500f, 0.462500f, 0.437500f, 0.437500f)); // 8
	EXPECT_EQ(allocate(-0.050f, 0.000f, 0.000f, -0.900f), Vector4f(0.912500f, 0.912500f, 0.887500f, 0.887500f)); // 9
	EXPECT_EQ(allocate(-0.050f, 0.000f, 0.000f, -1.000f), Vector4f(1.000000f, 1.000000f, 0.975000f, 0.975000f)); // 10
	EXPECT_EQ(allocate(0.050f, -0.050f, 0.000f, -0.000f), Vector4f(0.000000f, 0.000000f, 0.000000f, 0.000000f)); // 11
	EXPECT_EQ(allocate(0.050f, -0.050f, 0.000f, -0.100f), Vector4f(0.075000f, 0.100000f, 0.125000f, 0.100000f)); // 12
	EXPECT_EQ(allocate(0.050f, -0.050f, 0.000f, -0.450f), Vector4f(0.425000f, 0.450000f, 0.475000f, 0.450000f)); // 13
	EXPECT_EQ(allocate(0.050f, -0.050f, 0.000f, -0.900f), Vector4f(0.875000f, 0.900000f, 0.925000f, 0.900000f)); // 14
	EXPECT_EQ(allocate(0.050f, -0.050f, 0.000f, -1.000f), Vector4f(0.950000f, 0.975000f, 1.000000f, 0.975000f)); // 15
	EXPECT_EQ(allocate(0.050f, 0.050f, -0.025f, -0.000f), Vector4f(0.000000f, 0.000000f, 0.000000f, 0.000000f)); // 16
	EXPECT_EQ(allocate(0.050f, 0.050f, -0.025f, -0.100f), Vector4f(0.093750f, 0.081250f, 0.093750f, 0.131250f)); // 17
	EXPECT_EQ(allocate(0.050f, 0.050f, -0.025f, -0.450f), Vector4f(0.443750f, 0.431250f, 0.443750f, 0.481250f)); // 18
	EXPECT_EQ(allocate(0.050f, 0.050f, -0.025f, -0.900f), Vector4f(0.893750f, 0.881250f, 0.893750f, 0.931250f)); // 19
	EXPECT_EQ(allocate(0.050f, 0.050f, -0.025f, -1.000f), Vector4f(0.962500f, 0.950000f, 0.962500f, 1.000000f)); // 20
	EXPECT_EQ(allocate(0.000f, 0.200f, -0.025f, -0.000f), Vector4f(0.000000f, 0.000000f, 0.000000f, 0.000000f)); // 21
	EXPECT_EQ(allocate(0.000f, 0.200f, -0.025f, -0.100f), Vector4f(0.143750f, 0.056250f, 0.043750f, 0.156250f)); // 22
	EXPECT_EQ(allocate(0.000f, 0.200f, -0.025f, -0.450f), Vector4f(0.493750f, 0.406250f, 0.393750f, 0.506250f)); // 23
	EXPECT_EQ(allocate(0.000f, 0.200f, -0.025f, -0.900f), Vector4f(0.943750f, 0.856250f, 0.843750f, 0.956250f)); // 24
	EXPECT_EQ(allocate(0.000f, 0.200f, -0.025f, -1.000f), Vector4f(0.987500f, 0.900000f, 0.887500f, 1.000000f)); // 25
	EXPECT_EQ(allocate(0.200f, 0.050f, 0.090f, -0.000f), Vector4f(0.000000f, 0.000000f, 0.000000f, 0.000000f)); // 26
	EXPECT_EQ(allocate(0.200f, 0.050f, 0.090f, -0.100f), Vector4f(0.085000f, 0.015000f, 0.160000f, 0.140000f)); // 27
	EXPECT_EQ(allocate(0.200f, 0.050f, 0.090f, -0.450f), Vector4f(0.435000f, 0.365000f, 0.510000f, 0.490000f)); // 28
	EXPECT_EQ(allocate(0.200f, 0.050f, 0.090f, -0.900f), Vector4f(0.885000f, 0.815000f, 0.960000f, 0.940000f)); // 29
	EXPECT_EQ(allocate(0.200f, 0.050f, 0.090f, -1.000f), Vector4f(0.922500f, 0.852500f, 0.997500f, 0.977500f)); // 30
	EXPECT_EQ(allocate(-0.125f, 0.020f, 0.040f, -0.000f), Vector4f(0.000000f, 0.000000f, 0.000000f, 0.000000f)); // 31
	EXPECT_EQ(allocate(-0.125f, 0.020f, 0.040f, -0.100f), Vector4f(0.146250f, 0.116250f, 0.073750f, 0.063750f)); // 32
	EXPECT_EQ(allocate(-0.125f, 0.020f, 0.040f, -0.450f), Vector4f(0.496250f, 0.466250f, 0.423750f, 0.413750f)); // 33
	EXPECT_EQ(allocate(-0.125f, 0.020f, 0.040f, -0.900f), Vector4f(0.946250f, 0.916250f, 0.873750f, 0.863750f)); // 34
	EXPECT_EQ(allocate(-0.125f, 0.020f, 0.040f, -1.000f), Vector4f(1.000000f, 0.970000f, 0.927500f, 0.917500f)); // 35
	EXPECT_EQ(allocate(1.000f, 0.000f, 0.000f, -0.000f), Vector4f(0.000000f, 0.000000f, 0.000000f, 0.000000f)); // 36
	EXPECT_EQ(allocate(1.000f, 0.000f, 0.000f, -0.100f), Vector4f(0.000000f, 0.000000f, 0.200000f, 0.200000f)); // 37
	EXPECT_EQ(allocate(1.000f, 0.000f, 0.000f, -0.450f), Vector4f(0.200000f, 0.200000f, 0.700000f, 0.700000f)); // 38
	EXPECT_EQ(allocate(1.000f, 0.000f, 0.000f, -0.900f), Vector4f(0.500000f, 0.500000f, 1.000000f, 1.000000f)); // 39
	EXPECT_EQ(allocate(1.000f, 0.000f, 0.000f, -1.000f), Vector4f(0.500000f, 0.500000f, 1.000000f, 1.000000f)); // 40
	EXPECT_EQ(allocate(0.000f, -1.000f, 0.000f, -0.000f), Vector4f(0.000000f, 0.000000f, 0.000000f, 0.000000f)); // 41
	EXPECT_EQ(allocate(0.000f, -1.000f, 0.000f, -0.100f), Vector4f(0.000000f, 0.200000f, 0.200000f, 0.000000f)); // 42
	EXPECT_EQ(allocate(0.000f, -1.000f, 0.000f, -0.450f), Vector4f(0.200000f, 0.700000f, 0.700000f, 0.200000f)); // 43
	EXPECT_EQ(allocate(0.000f, -1.000f, 0.000f, -0.900f), Vector4f(0.500000f, 1.000000f, 1.000000f, 0.500000f)); // 44
	EXPECT_EQ(allocate(0.000f, -1.000f, 0.000f, -1.000f), Vector4f(0.500000f, 1.000000f, 1.000000f, 0.500000f)); // 45
	EXPECT_EQ(allocate(0.000f, 0.000f, 1.000f, -0.000f), Vector4f(0.000000f, 0.000000f, 0.000000f, 0.000000f)); // 46
	EXPECT_EQ(allocate(0.000f, 0.000f, 1.000f, -0.100f), Vector4f(0.200000f, 0.000000f, 0.200000f, 0.000000f)); // 47
	EXPECT_EQ(allocate(0.000f, 0.000f, 1.000f, -0.450f), Vector4f(0.700000f, 0.200000f, 0.700000f, 0.200000f)); // 48
	EXPECT_EQ(allocate(0.000f, 0.000f, 1.000f, -0.900f), Vector4f(1.000000f, 0.500000f, 1.000000f, 0.500000f)); // 49
	EXPECT_EQ(allocate(0.000f, 0.000f, 1.000f, -1.000f), Vector4f(1.000000f, 0.700000f, 1.000000f, 0.700000f)); // 50
	EXPECT_EQ(allocate(1.000f, 1.000f, -1.000f, -0.000f), Vector4f(0.000000f, 0.000000f, 0.000000f, 0.000000f)); // 51
	EXPECT_EQ(allocate(1.000f, 1.000f, -1.000f, -0.100f), Vector4f(0.200000f, 0.000000f, 0.000000f, 0.200000f)); // 52
	EXPECT_EQ(allocate(1.000f, 1.000f, -1.000f, -0.450f), Vector4f(0.100000f, 0.100000f, 0.000000f, 1.000000f)); // 53
	EXPECT_EQ(allocate(1.000f, 1.000f, -1.000f, -0.900f), Vector4f(0.200000f, 0.000000f, 0.200000f, 1.000000f)); // 54
	EXPECT_EQ(allocate(1.000f, 1.000f, -1.000f, -1.000f), Vector4f(0.200000f, 0.000000f, 0.200000f, 1.000000f)); // 55
	EXPECT_EQ(allocate(-1.000f, 0.900f, -0.900f, -0.000f), Vector4f(0.000000f, 0.000000f, 0.000000f, 0.000000f)); // 56
	EXPECT_EQ(allocate(-1.000f, 0.900f, -0.900f, -0.100f), Vector4f(0.200000f, 0.000000f, 0.000000f, 0.200000f)); // 57
	EXPECT_EQ(allocate(-1.000f, 0.900f, -0.900f, -0.450f), Vector4f(0.900000f, 0.450000f, 0.000000f, 0.450000f)); // 58
	EXPECT_EQ(allocate(-1.000f, 0.900f, -0.900f, -0.900f), Vector4f(0.950000f, 0.600000f, 0.000000f, 0.550000f)); // 59
	EXPECT_EQ(allocate(-1.000f, 0.900f, -0.900f, -1.000f), Vector4f(0.950000f, 0.600000f, 0.000000f, 0.550000f)); // 60
	EXPECT_EQ(allocate(-1.000f, 0.900f, 0.000f, -0.000f), Vector4f(0.000000f, 0.000000f, 0.000000f, 0.000000f)); // 61
	EXPECT_EQ(allocate(-1.000f, 0.900f, 0.000f, -0.100f), Vector4f(0.200000f, 0.000000f, 0.000000f, 0.200000f)); // 62
	EXPECT_EQ(allocate(-1.000f, 0.900f, 0.000f, -0.450f), Vector4f(0.900000f, 0.450000f, 0.000000f, 0.450000f)); // 63
	EXPECT_EQ(allocate(-1.000f, 0.900f, 0.000f, -0.900f), Vector4f(1.000000f, 0.550000f, 0.050000f, 0.500000f)); // 64
	EXPECT_EQ(allocate(-1.000f, 0.900f, 0.000f, -1.000f), Vector4f(1.000000f, 0.550000f, 0.050000f, 0.500000f)); // 65
}

TEST_F(Control_ಹಂಚಿಕೆ_Sequential_Desaturation_Test_Quad_X, ಹಿಂದಿನ_Mixing_Tests_Airmode_RP)
{
	ಗಣ_Airmode(1); // Roll and pitch airmode
	EXPECT_EQ(allocate(0.000f, 0.000f, 0.000f, -0.000f), Vector4f(0.000000f, 0.000000f, 0.000000f, 0.000000f)); // 1
	EXPECT_EQ(allocate(0.000f, 0.000f, 0.000f, -0.100f), Vector4f(0.100000f, 0.100000f, 0.100000f, 0.100000f)); // 2
	EXPECT_EQ(allocate(0.000f, 0.000f, 0.000f, -0.450f), Vector4f(0.450000f, 0.450000f, 0.450000f, 0.450000f)); // 3
	EXPECT_EQ(allocate(0.000f, 0.000f, 0.000f, -0.900f), Vector4f(0.900000f, 0.900000f, 0.900000f, 0.900000f)); // 4
	EXPECT_EQ(allocate(0.000f, 0.000f, 0.000f, -1.000f), Vector4f(1.000000f, 1.000000f, 1.000000f, 1.000000f)); // 5
	EXPECT_EQ(allocate(-0.050f, 0.000f, 0.000f, -0.000f), Vector4f(0.025000f, 0.025000f, 0.000000f, 0.000000f)); // 6
	EXPECT_EQ(allocate(-0.050f, 0.000f, 0.000f, -0.100f), Vector4f(0.112500f, 0.112500f, 0.087500f, 0.087500f)); // 7
	EXPECT_EQ(allocate(-0.050f, 0.000f, 0.000f, -0.450f), Vector4f(0.462500f, 0.462500f, 0.437500f, 0.437500f)); // 8
	EXPECT_EQ(allocate(-0.050f, 0.000f, 0.000f, -0.900f), Vector4f(0.912500f, 0.912500f, 0.887500f, 0.887500f)); // 9
	EXPECT_EQ(allocate(-0.050f, 0.000f, 0.000f, -1.000f), Vector4f(1.000000f, 1.000000f, 0.975000f, 0.975000f)); // 10
	EXPECT_EQ(allocate(0.050f, -0.050f, 0.000f, -0.000f), Vector4f(0.000000f, 0.025000f, 0.050000f, 0.025000f)); // 11
	EXPECT_EQ(allocate(0.050f, -0.050f, 0.000f, -0.100f), Vector4f(0.075000f, 0.100000f, 0.125000f, 0.100000f)); // 12
	EXPECT_EQ(allocate(0.050f, -0.050f, 0.000f, -0.450f), Vector4f(0.425000f, 0.450000f, 0.475000f, 0.450000f)); // 13
	EXPECT_EQ(allocate(0.050f, -0.050f, 0.000f, -0.900f), Vector4f(0.875000f, 0.900000f, 0.925000f, 0.900000f)); // 14
	EXPECT_EQ(allocate(0.050f, -0.050f, 0.000f, -1.000f), Vector4f(0.950000f, 0.975000f, 1.000000f, 0.975000f)); // 15
	EXPECT_EQ(allocate(0.050f, 0.050f, -0.025f, -0.000f), Vector4f(0.018750f, 0.006250f, 0.018750f, 0.056250f)); // 16
	EXPECT_EQ(allocate(0.050f, 0.050f, -0.025f, -0.100f), Vector4f(0.093750f, 0.081250f, 0.093750f, 0.131250f)); // 17
	EXPECT_EQ(allocate(0.050f, 0.050f, -0.025f, -0.450f), Vector4f(0.443750f, 0.431250f, 0.443750f, 0.481250f)); // 18
	EXPECT_EQ(allocate(0.050f, 0.050f, -0.025f, -0.900f), Vector4f(0.893750f, 0.881250f, 0.893750f, 0.931250f)); // 19
	EXPECT_EQ(allocate(0.050f, 0.050f, -0.025f, -1.000f), Vector4f(0.962500f, 0.950000f, 0.962500f, 1.000000f)); // 20
	EXPECT_EQ(allocate(0.000f, 0.200f, -0.025f, -0.000f), Vector4f(0.100000f, 0.000000f, 0.000000f, 0.100000f)); // 21
	EXPECT_EQ(allocate(0.000f, 0.200f, -0.025f, -0.100f), Vector4f(0.143750f, 0.056250f, 0.043750f, 0.156250f)); // 22
	EXPECT_EQ(allocate(0.000f, 0.200f, -0.025f, -0.450f), Vector4f(0.493750f, 0.406250f, 0.393750f, 0.506250f)); // 23
	EXPECT_EQ(allocate(0.000f, 0.200f, -0.025f, -0.900f), Vector4f(0.943750f, 0.856250f, 0.843750f, 0.956250f)); // 24
	EXPECT_EQ(allocate(0.000f, 0.200f, -0.025f, -1.000f), Vector4f(0.987500f, 0.900000f, 0.887500f, 1.000000f)); // 25
	EXPECT_EQ(allocate(0.200f, 0.050f, 0.090f, -0.000f), Vector4f(0.025000f, 0.000000f, 0.100000f, 0.125000f)); // 26
	EXPECT_EQ(allocate(0.200f, 0.050f, 0.090f, -0.100f), Vector4f(0.085000f, 0.015000f, 0.160000f, 0.140000f)); // 27
	EXPECT_EQ(allocate(0.200f, 0.050f, 0.090f, -0.450f), Vector4f(0.435000f, 0.365000f, 0.510000f, 0.490000f)); // 28
	EXPECT_EQ(allocate(0.200f, 0.050f, 0.090f, -0.900f), Vector4f(0.885000f, 0.815000f, 0.960000f, 0.940000f)); // 29
	EXPECT_EQ(allocate(0.200f, 0.050f, 0.090f, -1.000f), Vector4f(0.922500f, 0.852500f, 0.997500f, 0.977500f)); // 30
	EXPECT_EQ(allocate(-0.125f, 0.020f, 0.040f, -0.000f), Vector4f(0.082500f, 0.052500f, 0.010000f, 0.000000f)); // 31
	EXPECT_EQ(allocate(-0.125f, 0.020f, 0.040f, -0.100f), Vector4f(0.146250f, 0.116250f, 0.073750f, 0.063750f)); // 32
	EXPECT_EQ(allocate(-0.125f, 0.020f, 0.040f, -0.450f), Vector4f(0.496250f, 0.466250f, 0.423750f, 0.413750f)); // 33
	EXPECT_EQ(allocate(-0.125f, 0.020f, 0.040f, -0.900f), Vector4f(0.946250f, 0.916250f, 0.873750f, 0.863750f)); // 34
	EXPECT_EQ(allocate(-0.125f, 0.020f, 0.040f, -1.000f), Vector4f(1.000000f, 0.970000f, 0.927500f, 0.917500f)); // 35
	EXPECT_EQ(allocate(1.000f, 0.000f, 0.000f, -0.000f), Vector4f(0.000000f, 0.000000f, 0.500000f, 0.500000f)); // 36
	EXPECT_EQ(allocate(1.000f, 0.000f, 0.000f, -0.100f), Vector4f(0.000000f, 0.000000f, 0.500000f, 0.500000f)); // 37
	EXPECT_EQ(allocate(1.000f, 0.000f, 0.000f, -0.450f), Vector4f(0.200000f, 0.200000f, 0.700000f, 0.700000f)); // 38
	EXPECT_EQ(allocate(1.000f, 0.000f, 0.000f, -0.900f), Vector4f(0.500000f, 0.500000f, 1.000000f, 1.000000f)); // 39
	EXPECT_EQ(allocate(1.000f, 0.000f, 0.000f, -1.000f), Vector4f(0.500000f, 0.500000f, 1.000000f, 1.000000f)); // 40
	EXPECT_EQ(allocate(0.000f, -1.000f, 0.000f, -0.000f), Vector4f(0.000000f, 0.500000f, 0.500000f, 0.000000f)); // 41
	EXPECT_EQ(allocate(0.000f, -1.000f, 0.000f, -0.100f), Vector4f(0.000000f, 0.500000f, 0.500000f, 0.000000f)); // 42
	EXPECT_EQ(allocate(0.000f, -1.000f, 0.000f, -0.450f), Vector4f(0.200000f, 0.700000f, 0.700000f, 0.200000f)); // 43
	EXPECT_EQ(allocate(0.000f, -1.000f, 0.000f, -0.900f), Vector4f(0.500000f, 1.000000f, 1.000000f, 0.500000f)); // 44
	EXPECT_EQ(allocate(0.000f, -1.000f, 0.000f, -1.000f), Vector4f(0.500000f, 1.000000f, 1.000000f, 0.500000f)); // 45
	EXPECT_EQ(allocate(0.000f, 0.000f, 1.000f, -0.000f), Vector4f(0.000000f, 0.000000f, 0.000000f, 0.000000f)); // 46
	EXPECT_EQ(allocate(0.000f, 0.000f, 1.000f, -0.100f), Vector4f(0.200000f, 0.000000f, 0.200000f, 0.000000f)); // 47
	EXPECT_EQ(allocate(0.000f, 0.000f, 1.000f, -0.450f), Vector4f(0.700000f, 0.200000f, 0.700000f, 0.200000f)); // 48
	EXPECT_EQ(allocate(0.000f, 0.000f, 1.000f, -0.900f), Vector4f(1.000000f, 0.500000f, 1.000000f, 0.500000f)); // 49
	EXPECT_EQ(allocate(0.000f, 0.000f, 1.000f, -1.000f), Vector4f(1.000000f, 0.700000f, 1.000000f, 0.700000f)); // 50
	EXPECT_EQ(allocate(1.000f, 1.000f, -1.000f, -0.000f), Vector4f(0.200000f, 0.000000f, 0.200000f, 1.000000f)); // 51
	EXPECT_EQ(allocate(1.000f, 1.000f, -1.000f, -0.100f), Vector4f(0.200000f, 0.000000f, 0.200000f, 1.000000f)); // 52
	EXPECT_EQ(allocate(1.000f, 1.000f, -1.000f, -0.450f), Vector4f(0.200000f, 0.000000f, 0.200000f, 1.000000f)); // 53
	EXPECT_EQ(allocate(1.000f, 1.000f, -1.000f, -0.900f), Vector4f(0.200000f, 0.000000f, 0.200000f, 1.000000f)); // 54
	EXPECT_EQ(allocate(1.000f, 1.000f, -1.000f, -1.000f), Vector4f(0.200000f, 0.000000f, 0.200000f, 1.000000f)); // 55
	EXPECT_EQ(allocate(-1.000f, 0.900f, -0.900f, -0.000f), Vector4f(0.950000f, 0.500000f, 0.000000f, 0.450000f)); // 56
	EXPECT_EQ(allocate(-1.000f, 0.900f, -0.900f, -0.100f), Vector4f(0.950000f, 0.500000f, 0.000000f, 0.450000f)); // 57
	EXPECT_EQ(allocate(-1.000f, 0.900f, -0.900f, -0.450f), Vector4f(0.950000f, 0.500000f, 0.000000f, 0.450000f)); // 58
	EXPECT_EQ(allocate(-1.000f, 0.900f, -0.900f, -0.900f), Vector4f(0.950000f, 0.600000f, 0.000000f, 0.550000f)); // 59
	EXPECT_EQ(allocate(-1.000f, 0.900f, -0.900f, -1.000f), Vector4f(0.950000f, 0.600000f, 0.000000f, 0.550000f)); // 60
	EXPECT_EQ(allocate(-1.000f, 0.900f, 0.000f, -0.000f), Vector4f(0.950000f, 0.500000f, 0.000000f, 0.450000f)); // 61
	EXPECT_EQ(allocate(-1.000f, 0.900f, 0.000f, -0.100f), Vector4f(0.950000f, 0.500000f, 0.000000f, 0.450000f)); // 62
	EXPECT_EQ(allocate(-1.000f, 0.900f, 0.000f, -0.450f), Vector4f(0.950000f, 0.500000f, 0.000000f, 0.450000f)); // 63
	EXPECT_EQ(allocate(-1.000f, 0.900f, 0.000f, -0.900f), Vector4f(1.000000f, 0.550000f, 0.050000f, 0.500000f)); // 64
	EXPECT_EQ(allocate(-1.000f, 0.900f, 0.000f, -1.000f), Vector4f(1.000000f, 0.550000f, 0.050000f, 0.500000f)); // 65
}

TEST_F(Control_ಹಂಚಿಕೆ_Sequential_Desaturation_Test_Quad_X, ಹಿಂದಿನ_Mixing_Tests_Airmode_RPY)
{
	ಗಣ_Airmode(2); // Roll, pitch and yaw airmode
	EXPECT_EQ(allocate(0.000f, 0.000f, 0.000f, -0.000f), Vector4f(0.000000f, 0.000000f, 0.000000f, 0.000000f)); // 1
	EXPECT_EQ(allocate(0.000f, 0.000f, 0.000f, -0.100f), Vector4f(0.100000f, 0.100000f, 0.100000f, 0.100000f)); // 2
	EXPECT_EQ(allocate(0.000f, 0.000f, 0.000f, -0.450f), Vector4f(0.450000f, 0.450000f, 0.450000f, 0.450000f)); // 3
	EXPECT_EQ(allocate(0.000f, 0.000f, 0.000f, -0.900f), Vector4f(0.900000f, 0.900000f, 0.900000f, 0.900000f)); // 4
	EXPECT_EQ(allocate(0.000f, 0.000f, 0.000f, -1.000f), Vector4f(1.000000f, 1.000000f, 1.000000f, 1.000000f)); // 5
	EXPECT_EQ(allocate(-0.050f, 0.000f, 0.000f, -0.000f), Vector4f(0.025000f, 0.025000f, 0.000000f, 0.000000f)); // 6
	EXPECT_EQ(allocate(-0.050f, 0.000f, 0.000f, -0.100f), Vector4f(0.112500f, 0.112500f, 0.087500f, 0.087500f)); // 7
	EXPECT_EQ(allocate(-0.050f, 0.000f, 0.000f, -0.450f), Vector4f(0.462500f, 0.462500f, 0.437500f, 0.437500f)); // 8
	EXPECT_EQ(allocate(-0.050f, 0.000f, 0.000f, -0.900f), Vector4f(0.912500f, 0.912500f, 0.887500f, 0.887500f)); // 9
	EXPECT_EQ(allocate(-0.050f, 0.000f, 0.000f, -1.000f), Vector4f(1.000000f, 1.000000f, 0.975000f, 0.975000f)); // 10
	EXPECT_EQ(allocate(0.050f, -0.050f, 0.000f, -0.000f), Vector4f(0.000000f, 0.025000f, 0.050000f, 0.025000f)); // 11
	EXPECT_EQ(allocate(0.050f, -0.050f, 0.000f, -0.100f), Vector4f(0.075000f, 0.100000f, 0.125000f, 0.100000f)); // 12
	EXPECT_EQ(allocate(0.050f, -0.050f, 0.000f, -0.450f), Vector4f(0.425000f, 0.450000f, 0.475000f, 0.450000f)); // 13
	EXPECT_EQ(allocate(0.050f, -0.050f, 0.000f, -0.900f), Vector4f(0.875000f, 0.900000f, 0.925000f, 0.900000f)); // 14
	EXPECT_EQ(allocate(0.050f, -0.050f, 0.000f, -1.000f), Vector4f(0.950000f, 0.975000f, 1.000000f, 0.975000f)); // 15
	EXPECT_EQ(allocate(0.050f, 0.050f, -0.025f, -0.000f), Vector4f(0.012500f, 0.000000f, 0.012500f, 0.050000f)); // 16
	EXPECT_EQ(allocate(0.050f, 0.050f, -0.025f, -0.100f), Vector4f(0.093750f, 0.081250f, 0.093750f, 0.131250f)); // 17
	EXPECT_EQ(allocate(0.050f, 0.050f, -0.025f, -0.450f), Vector4f(0.443750f, 0.431250f, 0.443750f, 0.481250f)); // 18
	EXPECT_EQ(allocate(0.050f, 0.050f, -0.025f, -0.900f), Vector4f(0.893750f, 0.881250f, 0.893750f, 0.931250f)); // 19
	EXPECT_EQ(allocate(0.050f, 0.050f, -0.025f, -1.000f), Vector4f(0.962500f, 0.950000f, 0.962500f, 1.000000f)); // 20
	EXPECT_EQ(allocate(0.000f, 0.200f, -0.025f, -0.000f), Vector4f(0.100000f, 0.012500f, 0.000000f, 0.112500f)); // 21
	EXPECT_EQ(allocate(0.000f, 0.200f, -0.025f, -0.100f), Vector4f(0.143750f, 0.056250f, 0.043750f, 0.156250f)); // 22
	EXPECT_EQ(allocate(0.000f, 0.200f, -0.025f, -0.450f), Vector4f(0.493750f, 0.406250f, 0.393750f, 0.506250f)); // 23
	EXPECT_EQ(allocate(0.000f, 0.200f, -0.025f, -0.900f), Vector4f(0.943750f, 0.856250f, 0.843750f, 0.956250f)); // 24
	EXPECT_EQ(allocate(0.000f, 0.200f, -0.025f, -1.000f), Vector4f(0.987500f, 0.900000f, 0.887500f, 1.000000f)); // 25
	EXPECT_EQ(allocate(0.200f, 0.050f, 0.090f, -0.000f), Vector4f(0.070000f, 0.000000f, 0.145000f, 0.125000f)); // 26
	EXPECT_EQ(allocate(0.200f, 0.050f, 0.090f, -0.100f), Vector4f(0.085000f, 0.015000f, 0.160000f, 0.140000f)); // 27
	EXPECT_EQ(allocate(0.200f, 0.050f, 0.090f, -0.450f), Vector4f(0.435000f, 0.365000f, 0.510000f, 0.490000f)); // 28
	EXPECT_EQ(allocate(0.200f, 0.050f, 0.090f, -0.900f), Vector4f(0.885000f, 0.815000f, 0.960000f, 0.940000f)); // 29
	EXPECT_EQ(allocate(0.200f, 0.050f, 0.090f, -1.000f), Vector4f(0.925000f, 0.855000f, 1.000000f, 0.980000f)); // 30
	EXPECT_EQ(allocate(-0.125f, 0.020f, 0.040f, -0.000f), Vector4f(0.082500f, 0.052500f, 0.010000f, 0.000000f)); // 31
	EXPECT_EQ(allocate(-0.125f, 0.020f, 0.040f, -0.100f), Vector4f(0.146250f, 0.116250f, 0.073750f, 0.063750f)); // 32
	EXPECT_EQ(allocate(-0.125f, 0.020f, 0.040f, -0.450f), Vector4f(0.496250f, 0.466250f, 0.423750f, 0.413750f)); // 33
	EXPECT_EQ(allocate(-0.125f, 0.020f, 0.040f, -0.900f), Vector4f(0.946250f, 0.916250f, 0.873750f, 0.863750f)); // 34
	EXPECT_EQ(allocate(-0.125f, 0.020f, 0.040f, -1.000f), Vector4f(1.000000f, 0.970000f, 0.927500f, 0.917500f)); // 35
	EXPECT_EQ(allocate(1.000f, 0.000f, 0.000f, -0.000f), Vector4f(0.000000f, 0.000000f, 0.500000f, 0.500000f)); // 36
	EXPECT_EQ(allocate(1.000f, 0.000f, 0.000f, -0.100f), Vector4f(0.000000f, 0.000000f, 0.500000f, 0.500000f)); // 37
	EXPECT_EQ(allocate(1.000f, 0.000f, 0.000f, -0.450f), Vector4f(0.200000f, 0.200000f, 0.700000f, 0.700000f)); // 38
	EXPECT_EQ(allocate(1.000f, 0.000f, 0.000f, -0.900f), Vector4f(0.500000f, 0.500000f, 1.000000f, 1.000000f)); // 39
	EXPECT_EQ(allocate(1.000f, 0.000f, 0.000f, -1.000f), Vector4f(0.500000f, 0.500000f, 1.000000f, 1.000000f)); // 40
	EXPECT_EQ(allocate(0.000f, -1.000f, 0.000f, -0.000f), Vector4f(0.000000f, 0.500000f, 0.500000f, 0.000000f)); // 41
	EXPECT_EQ(allocate(0.000f, -1.000f, 0.000f, -0.100f), Vector4f(0.000000f, 0.500000f, 0.500000f, 0.000000f)); // 42
	EXPECT_EQ(allocate(0.000f, -1.000f, 0.000f, -0.450f), Vector4f(0.200000f, 0.700000f, 0.700000f, 0.200000f)); // 43
	EXPECT_EQ(allocate(0.000f, -1.000f, 0.000f, -0.900f), Vector4f(0.500000f, 1.000000f, 1.000000f, 0.500000f)); // 44
	EXPECT_EQ(allocate(0.000f, -1.000f, 0.000f, -1.000f), Vector4f(0.500000f, 1.000000f, 1.000000f, 0.500000f)); // 45
	EXPECT_EQ(allocate(0.000f, 0.000f, 1.000f, -0.000f), Vector4f(0.500000f, 0.000000f, 0.500000f, 0.000000f)); // 46
	EXPECT_EQ(allocate(0.000f, 0.000f, 1.000f, -0.100f), Vector4f(0.500000f, 0.000000f, 0.500000f, 0.000000f)); // 47
	EXPECT_EQ(allocate(0.000f, 0.000f, 1.000f, -0.450f), Vector4f(0.700000f, 0.200000f, 0.700000f, 0.200000f)); // 48
	EXPECT_EQ(allocate(0.000f, 0.000f, 1.000f, -0.900f), Vector4f(1.000000f, 0.500000f, 1.000000f, 0.500000f)); // 49
	EXPECT_EQ(allocate(0.000f, 0.000f, 1.000f, -1.000f), Vector4f(1.000000f, 0.500000f, 1.000000f, 0.500000f)); // 50
	EXPECT_EQ(allocate(1.000f, 1.000f, -1.000f, -0.000f), Vector4f(0.000000f, 0.000000f, 0.000000f, 1.000000f)); // 51
	EXPECT_EQ(allocate(1.000f, 1.000f, -1.000f, -0.100f), Vector4f(0.000000f, 0.000000f, 0.000000f, 1.000000f)); // 52
	EXPECT_EQ(allocate(1.000f, 1.000f, -1.000f, -0.450f), Vector4f(0.000000f, 0.000000f, 0.000000f, 1.000000f)); // 53
	EXPECT_EQ(allocate(1.000f, 1.000f, -1.000f, -0.900f), Vector4f(0.000000f, 0.000000f, 0.000000f, 1.000000f)); // 54
	EXPECT_EQ(allocate(1.000f, 1.000f, -1.000f, -1.000f), Vector4f(0.000000f, 0.000000f, 0.000000f, 1.000000f)); // 55
	EXPECT_EQ(allocate(-1.000f, 0.900f, -0.900f, -0.000f), Vector4f(0.950000f, 0.950000f, 0.000000f, 0.900000f)); // 56
	EXPECT_EQ(allocate(-1.000f, 0.900f, -0.900f, -0.100f), Vector4f(0.950000f, 0.950000f, 0.000000f, 0.900000f)); // 57
	EXPECT_EQ(allocate(-1.000f, 0.900f, -0.900f, -0.450f), Vector4f(0.950000f, 0.950000f, 0.000000f, 0.900000f)); // 58
	EXPECT_EQ(allocate(-1.000f, 0.900f, -0.900f, -0.900f), Vector4f(1.000000f, 1.000000f, 0.050000f, 0.950000f)); // 59
	EXPECT_EQ(allocate(-1.000f, 0.900f, -0.900f, -1.000f), Vector4f(1.000000f, 1.000000f, 0.050000f, 0.950000f)); // 60
	EXPECT_EQ(allocate(-1.000f, 0.900f, 0.000f, -0.000f), Vector4f(0.950000f, 0.500000f, 0.000000f, 0.450000f)); // 61
	EXPECT_EQ(allocate(-1.000f, 0.900f, 0.000f, -0.100f), Vector4f(0.950000f, 0.500000f, 0.000000f, 0.450000f)); // 62
	EXPECT_EQ(allocate(-1.000f, 0.900f, 0.000f, -0.450f), Vector4f(0.950000f, 0.500000f, 0.000000f, 0.450000f)); // 63
	EXPECT_EQ(allocate(-1.000f, 0.900f, 0.000f, -0.900f), Vector4f(1.000000f, 0.550000f, 0.050000f, 0.500000f)); // 64
	EXPECT_EQ(allocate(-1.000f, 0.900f, 0.000f, -1.000f), Vector4f(1.000000f, 0.550000f, 0.050000f, 0.500000f)); // 65
}

// Hexarotor with a motor and its opposite stopped: roll and yaw collinear. The float pseudo-inverse
// of that matrix used to be garbage and drove the remaining motors to full at zero thrust demand.
TEST(Control_ಹಂಚಿಕೆ_Sequential_Desaturation_Test, AirmodeDisabledNeverAddsThrustAfterMotorFailure)
{
	// rows roll/pitch/yaw/thrust xyz, one column per rotor
	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ intact[6][6] = {
		{-5.947500f,  5.934500f,  2.964000f, -2.853500f, -2.990000f,  2.840500f},
		{ 0.071500f,  0.071500f,  5.063500f, -4.972500f,  5.063500f, -4.972500f},
		{-0.325000f,  0.325000f, -0.325000f,  0.325000f,  0.325000f, -0.325000f},
		{ 0.f,        0.f,        0.f,        0.f,        0.f,        0.f},
		{ 0.f,        0.f,        0.f,        0.f,        0.f,        0.f},
		{-6.500000f, -6.500000f, -6.500000f, -6.500000f, -6.500000f, -6.500000f}
	};

	ಸ್ವಯಂಚಾಲಿತ make_effectiveness = [&intact](uint16_t stopped_ಮುಖವಾಡ) {
		ActuatorEffectiveness::Effectiveness_ಮಾತೃಕೆ effectiveness;
		effectiveness.ಗಣ_Zero();

		ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < 6; i++) {
			ಆದರೆ (stopped_ಮುಖವಾಡ & (1u << i)) {
				ಮುಂದುವರಿ;
			}

			ಗಾಗಿ (ಪೂರ್ಣಾಂಕ axis = 0; axis < 6; axis++) {
				effectiveness(axis, i) = intact[axis][i];
			}
		}

		ಹಿಂತಿರುಗಿ effectiveness;
	};

	Control_ಹಂಚಿಕೆ_Sequential_Desaturation allocator;
	allocator.ಗಣ_Normalize_RPY(ನಿಜ);
	ಮಾತೃಕೆ::ಸದಿಶ<ದಶಮಾಂಶ, ActuatorEffectiveness::ಅಂಕೆ_ACTUATORS> actuator_trim;
	ಮಾತೃಕೆ::ಸದಿಶ<ದಶಮಾಂಶ, ActuatorEffectiveness::ಅಂಕೆ_ACTUATORS> linearization_ಬಿಂದು;

	// intact geometry first to capture the normalization scale, as ControlAllocator does
	allocator.ಗಣ_Effectiveness_ಮಾತೃಕೆ(make_effectiveness(0), actuator_trim, linearization_ಬಿಂದು,
					 ActuatorEffectiveness::ಅಂಕೆ_ACTUATORS, ನಿಜ);
	allocator.allocate();

	// rotors 0 and 1 stop, scale not recomputed
	allocator.ಗಣ_Had_Actuator_Failure(ನಿಜ);
	allocator.ಗಣ_Effectiveness_ಮಾತೃಕೆ(make_effectiveness((1u << 0) | (1u << 1)), actuator_trim,
					 linearization_ಬಿಂದು, ActuatorEffectiveness::ಅಂಕೆ_ACTUATORS, ಸುಳ್ಳು);

	// attitude hold at zero thrust demand
	ಮಾತೃಕೆ::ಸದಿಶ<ದಶಮಾಂಶ, ActuatorEffectiveness::ಅಂಕೆ_AXES> control_sp;
	control_sp(Control_ಹಂಚಿಕೆ::ControlAxis::ROLL) = 0.010f;
	control_sp(Control_ಹಂಚಿಕೆ::ControlAxis::PITCH) = 0.024f;
	control_sp(Control_ಹಂಚಿಕೆ::ControlAxis::YAW) = 0.f;
	control_sp(Control_ಹಂಚಿಕೆ::ControlAxis::THRUST_X) = 0.f;
	control_sp(Control_ಹಂಚಿಕೆ::ControlAxis::THRUST_Y) = 0.f;
	control_sp(Control_ಹಂಚಿಕೆ::ControlAxis::THRUST_Z) = -0.001f;
	allocator.ಗಣ_Control_Setpoint(control_sp);

	// Since MC_AIRMODE was not set explicitly, assume airmode is disabled.
	allocator.allocate();

	ಸ್ಥಿರಾಂಕ ಸ್ವಯಂಚಾಲಿತ &actuator_sp = allocator.getActuatorSetpoint();

	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < ActuatorEffectiveness::ಅಂಕೆ_ACTUATORS; ++i) {
		EXPECT_LT(actuator_sp(i), 0.1f) << "motor " << i;
	}
}

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

#شامل <gtest/gtest.h>
#شامل <ActuatorEffectivenessRotors.hpp>
#شامل "ControlAllocationSequentialDesaturation.hpp"

استعمال نامخلا میٹرکس;
استعمال Actuator_سمتیہ = Control_تخصیص::Actuator_سمتیہ;

TEST(Control_تخصیص_Sequential_Desaturation_Test, AllZeroCase)
{
	Control_تخصیص_Sequential_Desaturation control_تخصیص;
	EXPECT_EQ(control_تخصیص.getActuatorSetpoint(), Actuator_سمتیہ());
	control_تخصیص.allocate();
	EXPECT_EQ(control_تخصیص.getActuatorSetpoint(), Actuator_سمتیہ());
}

TEST(Control_تخصیص_Sequential_Desaturation_Test, سیٹ_Get_Actuator_Setpoint)
{
	Control_تخصیص_Sequential_Desaturation control_تخصیص;
	اعشاری actuator_setpoint_array[Control_تخصیص::عدد_ACTUATORS] = {1.f, 2.f, 3.f, 4.f, 5.f, 6.f};
	Actuator_سمتیہ actuator_setpoint(actuator_setpoint_array);
	control_تخصیص.سیٹ_Actuator_Setpoint(actuator_setpoint);
	EXPECT_EQ(control_تخصیص.getActuatorSetpoint(), actuator_setpoint);
}

جماعت Control_تخصیص_Sequential_Desaturation_Test_Quad_X : عوامی ::testing::Test
{
عوامی:
	ساکن constexpr uint8_t عدد_ACTUATORS = 4;
	Control_تخصیص_Sequential_Desaturation control_تخصیص_2;

	خالی سیٹ_اوپر() override
	{
		param_control_autosave(جھوٹ); // Disable autosaving parameters to avoid busy loop in param_set()
		سیٹ_Airmode(0); // No airmode by default

		// Quadrotor x geometry
		ActuatorEffectivenessRotors::Geometry quadx_geometry{};
		quadx_geometry.عدد_rotors = 4;
		quadx_geometry.rotors[0].position = {1.f, 1.f, 0.f}; // clockwise motor numbering
		quadx_geometry.rotors[1].position = {-1.f, 1.f, 0.f};
		quadx_geometry.rotors[2].position = {-1.f, -1.f, 0.f};
		quadx_geometry.rotors[3].position = {1.f, -1.f, 0.f};
		quadx_geometry.rotors[0].moment_تناسب = 1.f;
		quadx_geometry.rotors[1].moment_تناسب = -1.f;
		quadx_geometry.rotors[2].moment_تناسب = 1.f;
		quadx_geometry.rotors[3].moment_تناسب = -1.f;

		برائے (صحیح i = 0; i < 4; ++i) {
			quadx_geometry.rotors[i].axis = Vector3f(0.f, 0.f, -1.f); // thrust downwards
			quadx_geometry.rotors[i].thrust_coef = 1.f;
			quadx_geometry.rotors[i].tilt_اشاریہ = -1;
		}

		// Compute actuator effectiveness
		ActuatorEffectiveness::Configuration actuator_configuration{};
		صحیح عدد_actuators = ActuatorEffectivenessRotors::compute_Effectiveness_میٹرکس(quadx_geometry,
				    actuator_configuration.effectiveness_matrices[0],
				    actuator_configuration.عدد_actuators_میٹرکس[0]);
		EXPECT_EQ(عدد_actuators, عدد_ACTUATORS);
		actuator_configuration.actuatorsAdded(ActuatorType::MOTORS, عدد_actuators);

		// Load effectiveness into allocation
		control_تخصیص_2.سیٹ_Effectiveness_میٹرکس(actuator_configuration.effectiveness_matrices[0],
				actuator_configuration.trim[0], actuator_configuration.linearization_نقطہ[0],
				actuator_configuration.عدد_actuators_میٹرکس[0], سچ /*update_normalization_scale*/);
	}

	خالی سیٹ_Airmode(ثابت int32_t منوال)
	{
		param_t param = param_ڈھونڈو("MC_AIRMODE");
		param_سیٹ(param, &منوال);
		control_تخصیص_2.updateParameters();
	}

	Vector4f allocate(اعشاری roll, اعشاری pitch, اعشاری yaw, اعشاری thrust)
	{
		سمتیہ<اعشاری, Control_تخصیص::عدد_AXES> control_setpoint{};
		control_setpoint(Control_تخصیص::ControlAxis::ROLL) = roll;
		control_setpoint(Control_تخصیص::ControlAxis::PITCH) = pitch;
		control_setpoint(Control_تخصیص::ControlAxis::YAW) = yaw;
		control_setpoint(Control_تخصیص::ControlAxis::THRUST_Z) = thrust;
		control_تخصیص_2.سیٹ_Control_Setpoint(control_setpoint);
		control_تخصیص_2.allocate();
		واپس getQuadOutputs();
	}

	Vector4f getQuadOutputs()
	{
		ثابت Actuator_سمتیہ &actuator_setpoint = control_تخصیص_2.getActuatorSetpoint();
		// All unused actuators shall stay zero
		ساکن constexpr uint8_t عدد_غیر_مستعمل_ACTUATORS = Control_تخصیص::عدد_ACTUATORS - 4;
		EXPECT_EQ(
			(سمتیہ<اعشاری, عدد_غیر_مستعمل_ACTUATORS>(actuator_setpoint.قاش<عدد_غیر_مستعمل_ACTUATORS, 1>(4, 0))),
			(سمتیہ<اعشاری, عدد_غیر_مستعمل_ACTUATORS>())
		);
		واپس Vector4f(actuator_setpoint.قاش<4, 1>(0, 0));
	}
};

// Make constant available, see https://stackoverflow.com/questions/42756443/undefined-reference-with-gtest
constexpr uint8_t Control_تخصیص_Sequential_Desaturation_Test_Quad_X::عدد_ACTUATORS;

TEST_F(Control_تخصیص_Sequential_Desaturation_Test_Quad_X, Zero)
{
	EXPECT_EQ(allocate(0.f, 0.f, 0.f, 0.f), Vector4f());
}


TEST_F(Control_تخصیص_Sequential_Desaturation_Test_Quad_X, CollectiveThrust)
{
	برائے (اعشاری thrust = 0.f; thrust <= (1.f + FLT_EPSILON); thrust += .1f) {
		EXPECT_EQ(allocate(0.f, 0.f, 0.f, -thrust), Vector4f(thrust, thrust, thrust, thrust));
	}
}

TEST_F(Control_تخصیص_Sequential_Desaturation_Test_Quad_X, RollPitchYaw)
{
	EXPECT_EQ(allocate(1.f, 0.f, 0.f, -.5f), Vector4f(.25f, .25f, .75f, .75f));
	EXPECT_EQ(allocate(-1.f, 0.f, 0.f, -.5f), Vector4f(.75f, .75f, .25f, .25f));
	EXPECT_EQ(allocate(0.f, 1.f, 0.f, -.5f), Vector4f(.75f, .25f, .25f, .75f));
	EXPECT_EQ(allocate(0.f, -1.f, 0.f, -.5f), Vector4f(.25f, .75f, .75f, .25f));
	EXPECT_EQ(allocate(0.f, 0.f, 1.f, -.5f), Vector4f(.75f, .25f, .75f, .25f));
	EXPECT_EQ(allocate(0.f, 0.f, -1.f, -.5f), Vector4f(.25f, .75f, .25f, .75f));
}

TEST_F(Control_تخصیص_Sequential_Desaturation_Test_Quad_X, Roll_Pitch_Yaw_بھرا_Thrust)
{
	EXPECT_EQ(allocate(1.f, 0.f, 0.f, -1.f), Vector4f(.5f, .5f, 1.f, 1.f));
	EXPECT_EQ(allocate(-1.f, 0.f, 0.f, -1.f), Vector4f(1.f, 1.f, .5f, .5f));
	EXPECT_EQ(allocate(0.f, 1.f, 0.f, -1.f), Vector4f(1.f, .5f, .5f, 1.f));
	EXPECT_EQ(allocate(0.f, -1.f, 0.f, -1.f), Vector4f(.5f, 1.f, 1.f, .5f));
	// There is a special case to deprioritize yaw down to 30% authority with maximum thrust
	EXPECT_EQ(allocate(0.f, 0.f, 1.f, -1.f), Vector4f(1.f, .7f, 1.f, .7f));
	EXPECT_EQ(allocate(0.f, 0.f, -1.f, -1.f), Vector4f(.7f, 1.f, .7f, 1.f));
}

TEST_F(Control_تخصیص_Sequential_Desaturation_Test_Quad_X, RollPitchYawZeroThrust)
{
	// No axis is allocated
	EXPECT_EQ(allocate(1.f, 0.f, 0.f, 0.f), Vector4f());
	EXPECT_EQ(allocate(-1.f, 0.f, 0.f, 0.f), Vector4f());
	EXPECT_EQ(allocate(0.f, 1.f, 0.f, 0.f), Vector4f());
	EXPECT_EQ(allocate(0.f, -1.f, 0.f, 0.f), Vector4f());
	EXPECT_EQ(allocate(0.f, 0.f, 1.f, 0.f), Vector4f());
	EXPECT_EQ(allocate(0.f, 0.f, -1.f, 0.f), Vector4f());
}

TEST_F(Control_تخصیص_Sequential_Desaturation_Test_Quad_X, RollPitchYawZeroThrustAirmodeRP)
{
	سیٹ_Airmode(1); // Roll and pitch airmode
	// Roll and pitch get fully allocated
	EXPECT_EQ(allocate(1.f, 0.f, 0.f, 0.f), Vector4f(0.f, 0.f, 0.5f, 0.5f));
	EXPECT_EQ(allocate(-1.f, 0.f, 0.f, 0.f), Vector4f(0.5f, 0.5f, 0.f, 0.f));
	EXPECT_EQ(allocate(0.f, 1.f, 0.f, 0.f), Vector4f(0.5f, 0.f, 0.f, 0.5f));
	EXPECT_EQ(allocate(0.f, -1.f, 0.f, 0.f), Vector4f(0.f, 0.5f, 0.5f, 0.f));
	// Yaw is not allocated
	EXPECT_EQ(allocate(0.f, 0.f, 1.f, 0.f), Vector4f());
	EXPECT_EQ(allocate(0.f, 0.f, -1.f, 0.f), Vector4f());
}

TEST_F(Control_تخصیص_Sequential_Desaturation_Test_Quad_X, RollPitchYawZeroThrustAirmodeRPY)
{
	سیٹ_Airmode(2); // Roll, pitch and yaw airmode
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
TEST_F(Control_تخصیص_Sequential_Desaturation_Test_Quad_X, AirmodeDisabledOnlyYaw)
{
	EXPECT_EQ(allocate(0.f, 0.f, 1.f, 0.f), Vector4f(0.f, 0.f, 0.f, 0.f));
}

// This tests that a control setpoint for z-thrust returns the desired actuator setpoint.
// Each motor should have an actuator setpoint that when summed together should be equal to
// control setpoint.
TEST_F(Control_تخصیص_Sequential_Desaturation_Test_Quad_X, AirmodeDisabledThrustZ)
{
	constexpr اعشاری THRUST = 0.75f;
	EXPECT_EQ(allocate(0.f, 0.f, 0.f, -THRUST), Vector4f(THRUST, THRUST, THRUST, THRUST));
}

// This tests that a control setpoint for z-thrust + yaw returns the desired actuator setpoint.
// This test does not saturate the yaw response.
TEST_F(Control_تخصیص_Sequential_Desaturation_Test_Quad_X, AirmodeDisabledThrustAndYaw)
{
	constexpr اعشاری THRUST = 0.75f;
	constexpr اعشاری YAW_TORQUE = 0.02f;
	constexpr اعشاری YAW = YAW_TORQUE / عدد_ACTUATORS;
	EXPECT_EQ(allocate(0.f, 0.f, YAW_TORQUE, -THRUST), Vector4f(THRUST + YAW, THRUST - YAW, THRUST + YAW, THRUST - YAW));
}

// This tests that a control setpoint for z-thrust + yaw returns the desired actuator setpoint.
// This test saturates the yaw response, but does not reduce total thrust.
TEST_F(Control_تخصیص_Sequential_Desaturation_Test_Quad_X, AirmodeDisabledThrustAndSaturatedYaw)
{
	constexpr اعشاری THRUST = 0.75f;
	constexpr اعشاری YAW_TORQUE = 1.f;
	constexpr اعشاری YAW = YAW_TORQUE / عدد_ACTUATORS;
	EXPECT_EQ(allocate(0.f, 0.f, YAW_TORQUE, -THRUST), Vector4f(THRUST + YAW, THRUST - YAW, THRUST + YAW, THRUST - YAW));
}

// This tests that a control setpoint for z-thrust + pitch returns the desired actuator setpoint.
// This test does not saturate the pitch response.
TEST_F(Control_تخصیص_Sequential_Desaturation_Test_Quad_X, AirmodeDisabledThrustAndPitch)
{
	constexpr اعشاری THRUST = 0.75f;
	constexpr اعشاری PITCH_TORQUE = 0.1f;
	constexpr اعشاری PITCH = PITCH_TORQUE / عدد_ACTUATORS;
	EXPECT_EQ(allocate(0.f, PITCH_TORQUE, 0.f, -THRUST),
		  Vector4f(THRUST + PITCH, THRUST - PITCH, THRUST - PITCH, THRUST + PITCH));
}

// This tests that a control setpoint for z-thrust + yaw returns the desired actuator setpoint.
// This test saturates yaw and demonstrates reduction of thrust for yaw.
TEST_F(Control_تخصیص_Sequential_Desaturation_Test_Quad_X, AirmodeDisabledReducedThrustAndYaw)
{
	constexpr اعشاری YAW_MARGIN = Control_تخصیص_Sequential_Desaturation::MINIMUM_YAW_MARGIN;
	EXPECT_EQ(allocate(0.f, 0.f, 1.f, -3.2f), Vector4f(1.f, 1.f - (2.f * YAW_MARGIN), 1.f, 1.f - (2.f * YAW_MARGIN)));
}

// This tests that a control setpoint for z-thrust + pitch returns the desired actuator setpoint.
// This test saturates the pitch response such that thrust is reduced to (partially) compensate.
TEST_F(Control_تخصیص_Sequential_Desaturation_Test_Quad_X, AirmodeDisabledReducedThrustAndPitch)
{
	EXPECT_EQ(allocate(0.f, 2.f, 0.f, -3.f), Vector4f(1.f, 0.f, 0.f, 1.f));
}

TEST_F(Control_تخصیص_Sequential_Desaturation_Test_Quad_X, پچھلا_Mixing_Tests_No_Airmode)
{
	سیٹ_Airmode(0); // No airmode
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

TEST_F(Control_تخصیص_Sequential_Desaturation_Test_Quad_X, پچھلا_Mixing_Tests_Airmode_RP)
{
	سیٹ_Airmode(1); // Roll and pitch airmode
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

TEST_F(Control_تخصیص_Sequential_Desaturation_Test_Quad_X, پچھلا_Mixing_Tests_Airmode_RPY)
{
	سیٹ_Airmode(2); // Roll, pitch and yaw airmode
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
TEST(Control_تخصیص_Sequential_Desaturation_Test, AirmodeDisabledNeverAddsThrustAfterMotorFailure)
{
	// rows roll/pitch/yaw/thrust xyz, one column per rotor
	ثابت اعشاری intact[6][6] = {
		{-5.947500f,  5.934500f,  2.964000f, -2.853500f, -2.990000f,  2.840500f},
		{ 0.071500f,  0.071500f,  5.063500f, -4.972500f,  5.063500f, -4.972500f},
		{-0.325000f,  0.325000f, -0.325000f,  0.325000f,  0.325000f, -0.325000f},
		{ 0.f,        0.f,        0.f,        0.f,        0.f,        0.f},
		{ 0.f,        0.f,        0.f,        0.f,        0.f,        0.f},
		{-6.500000f, -6.500000f, -6.500000f, -6.500000f, -6.500000f, -6.500000f}
	};

	خودکار make_effectiveness = [&intact](uint16_t stopped_نقاب) {
		ActuatorEffectiveness::Effectiveness_میٹرکس effectiveness;
		effectiveness.سیٹ_Zero();

		برائے (صحیح i = 0; i < 6; i++) {
			اگر (stopped_نقاب & (1u << i)) {
				جاری;
			}

			برائے (صحیح axis = 0; axis < 6; axis++) {
				effectiveness(axis, i) = intact[axis][i];
			}
		}

		واپس effectiveness;
	};

	Control_تخصیص_Sequential_Desaturation allocator;
	allocator.سیٹ_Normalize_RPY(سچ);
	میٹرکس::سمتیہ<اعشاری, ActuatorEffectiveness::عدد_ACTUATORS> actuator_trim;
	میٹرکس::سمتیہ<اعشاری, ActuatorEffectiveness::عدد_ACTUATORS> linearization_نقطہ;

	// intact geometry first to capture the normalization scale, as ControlAllocator does
	allocator.سیٹ_Effectiveness_میٹرکس(make_effectiveness(0), actuator_trim, linearization_نقطہ,
					 ActuatorEffectiveness::عدد_ACTUATORS, سچ);
	allocator.allocate();

	// rotors 0 and 1 stop, scale not recomputed
	allocator.سیٹ_Had_Actuator_Failure(سچ);
	allocator.سیٹ_Effectiveness_میٹرکس(make_effectiveness((1u << 0) | (1u << 1)), actuator_trim,
					 linearization_نقطہ, ActuatorEffectiveness::عدد_ACTUATORS, جھوٹ);

	// attitude hold at zero thrust demand
	میٹرکس::سمتیہ<اعشاری, ActuatorEffectiveness::عدد_AXES> control_sp;
	control_sp(Control_تخصیص::ControlAxis::ROLL) = 0.010f;
	control_sp(Control_تخصیص::ControlAxis::PITCH) = 0.024f;
	control_sp(Control_تخصیص::ControlAxis::YAW) = 0.f;
	control_sp(Control_تخصیص::ControlAxis::THRUST_X) = 0.f;
	control_sp(Control_تخصیص::ControlAxis::THRUST_Y) = 0.f;
	control_sp(Control_تخصیص::ControlAxis::THRUST_Z) = -0.001f;
	allocator.سیٹ_Control_Setpoint(control_sp);

	// Since MC_AIRMODE was not set explicitly, assume airmode is disabled.
	allocator.allocate();

	ثابت خودکار &actuator_sp = allocator.getActuatorSetpoint();

	برائے (صحیح i = 0; i < ActuatorEffectiveness::عدد_ACTUATORS; ++i) {
		EXPECT_LT(actuator_sp(i), 0.1f) << "motor " << i;
	}
}

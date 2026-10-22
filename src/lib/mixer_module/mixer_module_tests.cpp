/****************************************************************************
 *
 *   Copyright (C) 2021 PX4 Development Team. All rights reserved.
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

#ضمن <gtest/gtest.h>
#ضمن <array>

#ضمن <parameters/param.h>
#ضمن <uORB/topics/actuator_motors.h>
#ضمن <uORB/topics/actuator_servos.h>
#ضمن <uORB/topics/actuator_armed.h>
#ضمن <uORB/topics/actuator_test.h>
#ضمن <uORB/Publication.hpp>
#ضمن <uORB/Subscription.hpp>

#ضمن "mixer_module.hpp"

#إذا defined(CONFIG_ARCH_BOARD_PX4_SITL)
#define PARAM_PREFIX "PWM_MAIN"
#وإلا
#define PARAM_PREFIX "HIL_ACT"
#endif

ساكن constexpr صحيح MAX_ژمارە_OUTPUTS = 8;
ساكن constexpr صحيح DISARMED_بەها = 900;
ساكن constexpr صحيح FAILSAFE_بەها = 800;
ساكن constexpr صحيح MIN_بەها = 1000;
ساكن constexpr صحيح CENTER_بەها = 1500;
ساكن constexpr صحيح MAX_بەها = 2000;

صنف MixerModuleTest : عام ::testing::Test
{
عام:
	فراغ SetUp() override
	{
		param_control_autosave(خطأ);
	}

	صحيح update(MixingOutput &mixing_output)
	{
		mixing_output.update();
		// make sure output_limit switches to ON (if outputs enabled)
		px4_usleep(50000 * 2);
		mixing_output.update();
		mixing_output.update();
		أرجع 3; // expected number of output updates
	}

};

صنف OutputModuleTest : عام OutputModuleInterface
{
عام:
	OutputModuleTest() : OutputModuleInterface(MODULE_NAME, px4::wq_configurations::hp_default) {};

	~OutputModuleTest() override
	{
		// Hide this test's commands from the next test: a zero actuator command is not inert
		_actuator_motors_pub.unadvertise();
		_actuator_servos_pub.unadvertise();
		_actuator_armed_pub.unadvertise();
	}

	فراغ Run() override
	{
		was_scheduled = true;
	}

	منطقي updateOutputs(عائم outputs_[MAX_ACTUATORS], unsigned ژمارە_outputs_2, unsigned ژمارە_control_groups_updated) override
	{
		memcpy(outputs, outputs_, حجم(outputs));
		ژمارە_outputs = ژمارە_outputs_2;
		++ژمارە_updates;
		أرجع true;
	}

	فراغ mixerChanged() override
	{
		mixer_changed = true;
	}

	فراغ configureFunctions(ثابت std::array<int32_t, MAX_ژمارە_OUTPUTS> &functions)
	{
		لكل (صحيح i = 0; i < MAX_ژمارە_OUTPUTS; ++i) {
			حرف buffer[17];

			snprintf(buffer, حجم(buffer), "%s_FUNC%u", PARAM_PREFIX, i + 1);
			param_set(param_find(buffer), &functions[i]);
		}

		updateParams();
	}

	فراغ sendMotors(ثابت std::array<عائم, actuator_motors_s::ژمارە_CONTROLS> &motors, uint16_t reversible = 0)
	{
		actuator_motors_s actuator_motors{};
		actuator_motors.timestamp = hrt_absolute_time();
		actuator_motors.reversible_flags = reversible;

		لكل (unsigned i = 0; i < motors.size(); ++i) {
			actuator_motors.control[i] = motors[i];
		}

		_actuator_motors_pub.publish(actuator_motors);
	}

	فراغ sendServos(ثابت std::array<عائم, actuator_servos_s::ژمارە_CONTROLS> &servos)
	{
		actuator_servos_s actuator_servos{};
		actuator_servos.timestamp = hrt_absolute_time();

		لكل (unsigned i = 0; i < servos.size(); ++i) {
			actuator_servos.control[i] = servos[i];
		}

		_actuator_servos_pub.publish(actuator_servos);
	}

	فراغ sendActuatorMotorTest(صحيح function, عائم value, منطقي release_control)
	{
		actuator_test_s actuator_test{};
		actuator_test.timestamp = hrt_absolute_time();
		actuator_test.function = function;
		actuator_test.value = value;
		actuator_test.action = release_control ? actuator_test_s::ACTION_RELEASE_CONTROL : actuator_test_s::ACTION_DO_CONTROL;
		actuator_test.timeout_ms = 0;
		_actuator_test_pub.publish(actuator_test);
	}

	فراغ sendActuatorArmed(منطقي armed, منطقي termination = خطأ, منطقي kill = خطأ, منطقي prearm = خطأ)
	{
		actuator_armed_s actuator_armed{};
		actuator_armed.timestamp = hrt_absolute_time();
		actuator_armed.armed = armed;
		actuator_armed.termination = termination;
		actuator_armed.kill = kill;
		actuator_armed.prearmed = prearm;
		_actuator_armed_pub.publish(actuator_armed);
	}

	فراغ reset()
	{
		memset(outputs, 0, حجم(outputs));
		ژمارە_outputs = 0;
		ژمارە_updates = 0;
		mixer_changed = خطأ;
	}

	عائم outputs[MAX_ACTUATORS] {};
	صحيح ژمارە_outputs{0};
	صحيح ژمارە_updates{0};
	منطقي was_scheduled{خطأ};
	منطقي mixer_changed{خطأ};

خاص:
	uORB::Publication<actuator_test_s> _actuator_test_pub{ORB_ID(actuator_test)};
	uORB::Publication<actuator_motors_s> _actuator_motors_pub{ORB_ID(actuator_motors)};
	uORB::Publication<actuator_servos_s> _actuator_servos_pub{ORB_ID(actuator_servos)};
	uORB::Publication<actuator_armed_s> _actuator_armed_pub{ORB_ID(actuator_armed)};
};

TEST_F(MixerModuleTest, basic)
{
	OutputModuleTest test_module;
	test_module.configureFunctions({});
	MixingOutput mixing_output{PARAM_PREFIX, MAX_ژمارە_OUTPUTS, test_module, MixingOutput::SchedulingPolicy::Disabled, خطأ, خطأ};
	mixing_output.set_All_Disarmed_بەهاکان(DISARMED_بەها);
	mixing_output.set_All_Failsafe_بەهاکان(FAILSAFE_بەها);
	mixing_output.set_All_Min_بەهاکان(MIN_بەها);
	mixing_output.set_All_Center_بەهاکان(CENTER_بەها);
	mixing_output.set_All_Max_بەهاکان(MAX_بەها);
	EXPECT_EQ(test_module.ژمارە_updates, 0);

	// all functions disabled: expect to get one single update to process disabling the output signal
	mixing_output.update();
	mixing_output.updateSubscriptions(خطأ);
	mixing_output.update();
	EXPECT_EQ(test_module.ژمارە_updates, 1);
	mixing_output.update();
	mixing_output.updateSubscriptions(خطأ);
	mixing_output.update();
	EXPECT_EQ(test_module.ژمارە_updates, 1);
	test_module.reset();

	// configure motor, ensure all still disarmed
	test_module.configureFunctions({(صحيح)OutputFunction::Motor1});
	mixing_output.updateSubscriptions(خطأ);
	EXPECT_TRUE(test_module.mixer_changed);
	EXPECT_EQ(test_module.ژمارە_updates, update(mixing_output));
	EXPECT_EQ(test_module.ژمارە_outputs, MAX_ژمارە_OUTPUTS);

	لكل (صحيح i = 0; i < test_module.ژمارە_outputs; ++i) {
		EXPECT_EQ(test_module.outputs[i], DISARMED_بەها);
	}

	test_module.reset();

	// send motors -> still disarmed
	test_module.sendMotors({1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f});
	test_module.configureFunctions({
		0,
		(صحيح)OutputFunction::Motor3,
		(صحيح)OutputFunction::Motor1,
		(صحيح)OutputFunction::Motor5});

	mixing_output.updateSubscriptions(خطأ);
	EXPECT_EQ(test_module.ژمارە_updates, update(mixing_output));
	EXPECT_EQ(test_module.ژمارە_outputs, MAX_ژمارە_OUTPUTS);

	لكل (صحيح i = 0; i < test_module.ژمارە_outputs; ++i) {
		EXPECT_EQ(test_module.outputs[i], DISARMED_بەها);
	}

	test_module.reset();


	// actuator test
	test_module.sendActuatorMotorTest((صحيح)OutputFunction::Motor5, 1.f, خطأ);
	test_module.sendMotors({1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f});
	mixing_output.updateSubscriptions(خطأ);
	EXPECT_EQ(test_module.ژمارە_updates, update(mixing_output));
	EXPECT_EQ(test_module.ژمارە_outputs, MAX_ژمارە_OUTPUTS);

	لكل (صحيح i = 0; i < test_module.ژمارە_outputs; ++i) {
		إذا (i == 3) {
			EXPECT_EQ(test_module.outputs[i], MAX_بەها);

		} وإلا {
			EXPECT_EQ(test_module.outputs[i], DISARMED_بەها);
		}
	}

	test_module.reset();

	// stop
	test_module.sendActuatorMotorTest((صحيح)OutputFunction::Motor5, 0.f, true);
	test_module.sendMotors({1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f});
	mixing_output.updateSubscriptions(خطأ);
	EXPECT_EQ(test_module.ژمارە_updates, update(mixing_output));
	EXPECT_EQ(test_module.ژمارە_outputs, MAX_ژمارە_OUTPUTS);

	لكل (صحيح i = 0; i < test_module.ژمارە_outputs; ++i) {
		EXPECT_EQ(test_module.outputs[i], DISARMED_بەها);
	}

	test_module.reset();

	EXPECT_FALSE(test_module.was_scheduled);
}

TEST_F(MixerModuleTest, arming)
{
	OutputModuleTest test_module;
	test_module.configureFunctions({
		0,
		(صحيح)OutputFunction::Motor3,
		(صحيح)OutputFunction::Motor1,
		(صحيح)OutputFunction::Motor5,
		(صحيح)OutputFunction::Servo3});
	MixingOutput mixing_output{PARAM_PREFIX, MAX_ژمارە_OUTPUTS, test_module, MixingOutput::SchedulingPolicy::Disabled, خطأ, خطأ};
	mixing_output.set_All_Disarmed_بەهاکان(DISARMED_بەها);
	mixing_output.set_All_Failsafe_بەهاکان(FAILSAFE_بەها);
	mixing_output.set_All_Min_بەهاکان(MIN_بەها);
	mixing_output.set_All_Center_بەهاکان(CENTER_بەها);
	mixing_output.set_All_Max_بەهاکان(MAX_بەها);

	test_module.sendMotors({1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f});
	test_module.sendActuatorArmed(خطأ);

	// ensure all disarmed
	mixing_output.updateSubscriptions(خطأ);
	EXPECT_EQ(test_module.ژمارە_updates, update(mixing_output));
	EXPECT_EQ(test_module.ژمارە_outputs, MAX_ژمارە_OUTPUTS);

	لكل (صحيح i = 0; i < MAX_ژمارە_OUTPUTS; ++i) {
		EXPECT_EQ(test_module.outputs[i], DISARMED_بەها);
	}

	test_module.reset();

	// arming
	test_module.sendMotors({0.5f, 1.f, 0.1f, 0.2f, 1.f, 1.f, 1.f, 1.f});
	test_module.sendActuatorArmed(true);

	EXPECT_EQ(test_module.ژمارە_updates, update(mixing_output));
	EXPECT_EQ(test_module.ژمارە_outputs, MAX_ژمارە_OUTPUTS);

	لكل (صحيح i = 0; i < MAX_ژمارە_OUTPUTS; ++i) {
		إذا (i == 1) {
			EXPECT_EQ(test_module.outputs[i], (MAX_بەها - MIN_بەها) * 0.1f + MIN_بەها);

		} وإلا إذا (i == 2) {
			EXPECT_EQ(test_module.outputs[i], (MAX_بەها - MIN_بەها) * 0.5f + MIN_بەها);

		} وإلا إذا (i == 3) {
			EXPECT_EQ(test_module.outputs[i], MAX_بەها);

		} وإلا {
			EXPECT_EQ(test_module.outputs[i], DISARMED_بەها);
		}
	}

	test_module.reset();

	// update motors
	test_module.sendMotors({0.9f, 1.f, 0.24f, 0.2f, 0.f, 1.f, 1.f, 1.f});
	mixing_output.updateSubscriptions(خطأ);
	mixing_output.update();

	لكل (صحيح i = 0; i < MAX_ژمارە_OUTPUTS; ++i) {
		إذا (i == 1) {
			EXPECT_EQ(test_module.outputs[i], (MAX_بەها - MIN_بەها) * 0.24f + MIN_بەها);

		} وإلا إذا (i == 2) {
			EXPECT_EQ(test_module.outputs[i], (MAX_بەها - MIN_بەها) * 0.9f + MIN_بەها);

		} وإلا إذا (i == 3) {
			EXPECT_EQ(test_module.outputs[i], MIN_بەها);

		} وإلا {
			EXPECT_EQ(test_module.outputs[i], DISARMED_بەها);
		}
	}

	test_module.reset();

	// failsafe
	test_module.sendActuatorArmed(true, true);
	test_module.sendMotors({0.5f, 1.f, 0.1f, 0.2f, 1.f, 1.f, 1.f, 1.f});
	mixing_output.update();

	لكل (صحيح i = 0; i < MAX_ژمارە_OUTPUTS; ++i) {
		EXPECT_EQ(test_module.outputs[i], FAILSAFE_بەها);
	}

	test_module.reset();

	// restore
	test_module.sendActuatorArmed(true, خطأ);
	test_module.sendMotors({1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f});
	mixing_output.update();

	لكل (صحيح i = 0; i < MAX_ژمارە_OUTPUTS; ++i) {
		إذا (i >= 1 && i <= 3) {
			EXPECT_EQ(test_module.outputs[i], MAX_بەها);

		} وإلا {
			EXPECT_EQ(test_module.outputs[i], DISARMED_بەها);
		}
	}

	test_module.reset();

	// manual lockdown
	test_module.sendActuatorArmed(true, خطأ, true);
	test_module.sendMotors({0.5f, 1.f, 0.1f, 0.2f, 1.f, 1.f, 1.f, 1.f});
	mixing_output.update();

	لكل (صحيح i = 0; i < MAX_ژمارە_OUTPUTS; ++i) {
		EXPECT_EQ(test_module.outputs[i], DISARMED_بەها);
	}

	test_module.reset();

	// restore
	test_module.sendActuatorArmed(true, خطأ);
	test_module.sendMotors({0.f, 0.f, 0.f, 0.f, 0.f, 1.f, 1.f, 1.f});
	mixing_output.update();

	لكل (صحيح i = 0; i < MAX_ژمارە_OUTPUTS; ++i) {
		إذا (i >= 1 && i <= 3) {
			EXPECT_EQ(test_module.outputs[i], MIN_بەها);

		} وإلا {
			EXPECT_EQ(test_module.outputs[i], DISARMED_بەها);
		}
	}

	test_module.reset();

	// set motor 5 reversible: expect output to be in center when commanding to 0
	test_module.sendActuatorArmed(true, خطأ);
	test_module.sendMotors({0.f, 0.f, 0.f, 0.f, 0.f, 1.f, 1.f, 1.f}, 1u << 4);
	mixing_output.update();
	EXPECT_EQ(mixing_output.reversibleOutputs(), 1u << 3);

	لكل (صحيح i = 0; i < MAX_ژمارە_OUTPUTS; ++i) {
		إذا (i == 1) {
			EXPECT_EQ(test_module.outputs[i], MIN_بەها);

		} وإلا إذا (i == 2) {
			EXPECT_EQ(test_module.outputs[i], MIN_بەها);

		} وإلا إذا (i == 3) {
			EXPECT_EQ(test_module.outputs[i], (MAX_بەها - MIN_بەها) * 0.5f + MIN_بەها);

		} وإلا {
			EXPECT_EQ(test_module.outputs[i], DISARMED_بەها);
		}
	}

	test_module.reset();

	// disarm
	test_module.sendActuatorArmed(خطأ);
	test_module.sendMotors({0.f, 0.f, 0.f, 0.f, 0.f, 1.f, 1.f, 1.f}, 1u << 4);
	mixing_output.update();

	لكل (صحيح i = 0; i < MAX_ژمارە_OUTPUTS; ++i) {
		EXPECT_EQ(test_module.outputs[i], DISARMED_بەها);
	}

	test_module.reset();

	EXPECT_FALSE(test_module.was_scheduled);
}

TEST_F(MixerModuleTest, prearm)
{
	OutputModuleTest test_module;
	test_module.configureFunctions({
		(صحيح)OutputFunction::Motor1,
		(صحيح)OutputFunction::Servo1});
	MixingOutput mixing_output{PARAM_PREFIX, MAX_ژمارە_OUTPUTS, test_module, MixingOutput::SchedulingPolicy::Disabled, خطأ, خطأ};
	mixing_output.set_All_Disarmed_بەهاکان(DISARMED_بەها);
	mixing_output.set_All_Failsafe_بەهاکان(FAILSAFE_بەها);
	mixing_output.set_All_Min_بەهاکان(MIN_بەها);
	mixing_output.set_All_Max_بەهاکان(MAX_بەها);

	test_module.sendMotors({1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f});
	test_module.sendServos({1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f});
	test_module.sendActuatorArmed(خطأ, خطأ, خطأ, true);

	// ensure all disarmed, except the servo
	mixing_output.updateSubscriptions(خطأ);
	EXPECT_EQ(test_module.ژمارە_updates, update(mixing_output));
	EXPECT_EQ(test_module.ژمارە_outputs, MAX_ژمارە_OUTPUTS);

	لكل (صحيح i = 0; i < MAX_ژمارە_OUTPUTS; ++i) {
		إذا (i == 1) {
			EXPECT_EQ(test_module.outputs[i], MAX_بەها);

		} وإلا {
			EXPECT_EQ(test_module.outputs[i], DISARMED_بەها);
		}
	}

	test_module.reset();

	EXPECT_FALSE(test_module.was_scheduled);
}

صنف TestMixingOutput : عام MixingOutput
{
عام:
	TestMixingOutput(ثابت حرف *param_prefix, uint8_t max_ژمارە_outputs, OutputModuleInterface &interface,
			 SchedulingPolicy scheduling_policy,
			 منطقي support_esc_calibration, منطقي ramp_up = true)
		: MixingOutput(param_prefix, max_ژمارە_outputs, interface, scheduling_policy, support_esc_calibration, ramp_up)
	{};
	عائم output_limit_calc_single(صحيح i, عائم value) ثابت { أرجع MixingOutput::output_limit_calc_single(i, value); }
};

TEST_F(MixerModuleTest, OutputLimitCalcSingle)
{
	OutputModuleTest test_module;
	test_module.configureFunctions({(صحيح)OutputFunction::Motor1});
	TestMixingOutput mixing_output{PARAM_PREFIX, MAX_ژمارە_OUTPUTS, test_module, MixingOutput::SchedulingPolicy::Disabled, خطأ, خطأ};

	mixing_output.set_All_Min_بەهاکان(MIN_بەها); // default range [1000,2000]
	mixing_output.set_All_Max_بەهاکان(MAX_بەها);
	mixing_output.set_All_Center_بەهاکان(CENTER_بەها); // Set center to middle value
	EXPECT_EQ(mixing_output.output_limit_calc_single(0, -1.f), 1000); // In range
	EXPECT_EQ(mixing_output.output_limit_calc_single(0, -.5f), 1250);
	EXPECT_EQ(mixing_output.output_limit_calc_single(0, 0.f), 1500);
	EXPECT_EQ(mixing_output.output_limit_calc_single(0, .5f), 1750);
	EXPECT_EQ(mixing_output.output_limit_calc_single(0, 1.f), 2000);
	EXPECT_EQ(mixing_output.output_limit_calc_single(0, -1.1f), 1000); // Out of range
	EXPECT_EQ(mixing_output.output_limit_calc_single(0, 1.1f), 2000);
	EXPECT_EQ(mixing_output.output_limit_calc_single(0, -1000.f), 1000); // Way ouf of range
	EXPECT_EQ(mixing_output.output_limit_calc_single(0, 1000.f), 2000);
	EXPECT_EQ(mixing_output.output_limit_calc_single(0, 0.0005), 1500.25f);
	EXPECT_EQ(static_cast<uint16_t>(lroundf(mixing_output.output_limit_calc_single(0, 0.0005))), 1500); // Rounding down
	EXPECT_EQ(mixing_output.output_limit_calc_single(0, 0.0015), 1500.75f);
	EXPECT_EQ(static_cast<uint16_t>(lroundf(mixing_output.output_limit_calc_single(0, 0.0015))), 1501); // Rounding up
	EXPECT_EQ(mixing_output.output_limit_calc_single(0, 0.002), 1501); // Exact value

	mixing_output.set_All_Min_بەهاکان(0); // lower range [0,20]
	mixing_output.set_All_Max_بەهاکان(20);
	mixing_output.set_All_Center_بەهاکان(10); // Set center to middle value
	EXPECT_EQ(mixing_output.output_limit_calc_single(0, -1.f), 0); // In range
	EXPECT_EQ(mixing_output.output_limit_calc_single(0, -.5f), 5);
	EXPECT_EQ(mixing_output.output_limit_calc_single(0, 0.f), 10);
	EXPECT_EQ(mixing_output.output_limit_calc_single(0, .5f), 15);
	EXPECT_EQ(mixing_output.output_limit_calc_single(0, 1.f), 20);
	EXPECT_EQ(mixing_output.output_limit_calc_single(0, -1.1f), 0); // Out of range
	EXPECT_EQ(mixing_output.output_limit_calc_single(0, 1.1f), 20);
	EXPECT_EQ(mixing_output.output_limit_calc_single(0, -1000.f), 0); // Way ouf of range
	EXPECT_EQ(mixing_output.output_limit_calc_single(0, 1000.f), 20);
	EXPECT_EQ(mixing_output.output_limit_calc_single(0, 0.025), 10.25f);
	EXPECT_EQ(static_cast<uint16_t>(lroundf(mixing_output.output_limit_calc_single(0, 0.025))), 10); // Rounding down
	EXPECT_EQ(mixing_output.output_limit_calc_single(0, 0.075), 10.75f);
	EXPECT_EQ(static_cast<uint16_t>(lroundf(mixing_output.output_limit_calc_single(0, 0.075))), 11); // Rounding up
	EXPECT_EQ(mixing_output.output_limit_calc_single(0, 0.1), 11); // Exact value

	mixing_output.set_All_Min_بەهاکان(20); // inverted range [20,0]
	mixing_output.set_All_Max_بەهاکان(0);
	mixing_output.set_All_Center_بەهاکان(10); // Set center to middle value
	EXPECT_EQ(mixing_output.output_limit_calc_single(0, -1.f), 20); // In range
	EXPECT_EQ(mixing_output.output_limit_calc_single(0, -.5f), 15);
	EXPECT_EQ(mixing_output.output_limit_calc_single(0, 0.f), 10);
	EXPECT_EQ(mixing_output.output_limit_calc_single(0, .5f), 5);
	EXPECT_EQ(mixing_output.output_limit_calc_single(0, 1.f), 0);
	EXPECT_EQ(mixing_output.output_limit_calc_single(0, -1.1f), 20); // Out of range
	EXPECT_EQ(mixing_output.output_limit_calc_single(0, 1.1f), 0);
	EXPECT_EQ(mixing_output.output_limit_calc_single(0, -1000.f), 20); // Way ouf of range
	EXPECT_EQ(mixing_output.output_limit_calc_single(0, 1000.f), 0);
	EXPECT_EQ(mixing_output.output_limit_calc_single(0, 0.025), 9.75f);
	EXPECT_EQ(static_cast<uint16_t>(lroundf(mixing_output.output_limit_calc_single(0, 0.025))), 10); // Rounding down
	EXPECT_EQ(mixing_output.output_limit_calc_single(0, 0.075), 9.25f);
	EXPECT_EQ(static_cast<uint16_t>(lroundf(mixing_output.output_limit_calc_single(0, 0.075))), 9); // Rounding up
	EXPECT_EQ(mixing_output.output_limit_calc_single(0, 0.1), 9); // Exact value
}

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

#അന്തര്ഭാവ <gtest/gtest.h>
#അന്തര്ഭാവ <array>

#അന്തര്ഭാവ <parameters/param.h>
#അന്തര്ഭാവ <uORB/topics/actuator_motors.h>
#അന്തര്ഭാവ <uORB/topics/actuator_servos.h>
#അന്തര്ഭാവ <uORB/topics/actuator_armed.h>
#അന്തര്ഭാവ <uORB/topics/actuator_test.h>
#അന്തര്ഭാവ <uORB/Publication.hpp>
#അന്തര്ഭാവ <uORB/Subscription.hpp>

#അന്തര്ഭാവ "mixer_module.hpp"

#എങ്കിൽ defined(CONFIG_ARCH_BOARD_PX4_SITL)
#define PARAM_PREFIX "PWM_MAIN"
#അല്ലെങ്കിൽ
#define PARAM_PREFIX "HIL_ACT"
#endif

സ്ഥിതം constexpr പൂർണ്ണസംഖ്യ MAX_അക്കം_OUTPUTS = 8;
സ്ഥിതം constexpr പൂർണ്ണസംഖ്യ DISARMED_മൂല്യം = 900;
സ്ഥിതം constexpr പൂർണ്ണസംഖ്യ FAILSAFE_മൂല്യം = 800;
സ്ഥിതം constexpr പൂർണ്ണസംഖ്യ MIN_മൂല്യം = 1000;
സ്ഥിതം constexpr പൂർണ്ണസംഖ്യ CENTER_മൂല്യം = 1500;
സ്ഥിതം constexpr പൂർണ്ണസംഖ്യ MAX_മൂല്യം = 2000;

വർഗ്ഗം MixerModuleTest : പൊതു ::testing::Test
{
പൊതു:
	ശൂന്യം SetUp() override
	{
		param_control_autosave(അസത്യം);
	}

	പൂർണ്ണസംഖ്യ update(MixingOutput &mixing_output)
	{
		mixing_output.update();
		// make sure output_limit switches to ON (if outputs enabled)
		px4_usleep(50000 * 2);
		mixing_output.update();
		mixing_output.update();
		തിരികെ 3; // expected number of output updates
	}

};

വർഗ്ഗം OutputModuleTest : പൊതു OutputModuleInterface
{
പൊതു:
	OutputModuleTest() : OutputModuleInterface(MODULE_NAME, px4::wq_configurations::hp_default) {};

	~OutputModuleTest() override
	{
		// Hide this test's commands from the next test: a zero actuator command is not inert
		_actuator_motors_pub.unadvertise();
		_actuator_servos_pub.unadvertise();
		_actuator_armed_pub.unadvertise();
	}

	ശൂന്യം Run() override
	{
		was_scheduled = സത്യം;
	}

	തർക്കം updateOutputs(ദശാംശം outputs_[MAX_ACTUATORS], unsigned അക്കം_outputs_2, unsigned അക്കം_control_groups_updated) override
	{
		memcpy(outputs, outputs_, വലുപ്പം(outputs));
		അക്കം_outputs = അക്കം_outputs_2;
		++അക്കം_updates;
		തിരികെ സത്യം;
	}

	ശൂന്യം mixerChanged() override
	{
		mixer_changed = സത്യം;
	}

	ശൂന്യം configureFunctions(സ്ഥിരം std::array<int32_t, MAX_അക്കം_OUTPUTS> &functions)
	{
		വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < MAX_അക്കം_OUTPUTS; ++i) {
			അക്ഷരം buffer[17];

			snprintf(buffer, വലുപ്പം(buffer), "%s_FUNC%u", PARAM_PREFIX, i + 1);
			param_set(param_find(buffer), &functions[i]);
		}

		updateParams();
	}

	ശൂന്യം sendMotors(സ്ഥിരം std::array<ദശാംശം, actuator_motors_s::അക്കം_CONTROLS> &motors, uint16_t reversible = 0)
	{
		actuator_motors_s actuator_motors{};
		actuator_motors.timestamp = hrt_absolute_time();
		actuator_motors.reversible_flags = reversible;

		വേണ്ടി (unsigned i = 0; i < motors.size(); ++i) {
			actuator_motors.control[i] = motors[i];
		}

		_actuator_motors_pub.publish(actuator_motors);
	}

	ശൂന്യം sendServos(സ്ഥിരം std::array<ദശാംശം, actuator_servos_s::അക്കം_CONTROLS> &servos)
	{
		actuator_servos_s actuator_servos{};
		actuator_servos.timestamp = hrt_absolute_time();

		വേണ്ടി (unsigned i = 0; i < servos.size(); ++i) {
			actuator_servos.control[i] = servos[i];
		}

		_actuator_servos_pub.publish(actuator_servos);
	}

	ശൂന്യം sendActuatorMotorTest(പൂർണ്ണസംഖ്യ function, ദശാംശം value, തർക്കം release_control)
	{
		actuator_test_s actuator_test{};
		actuator_test.timestamp = hrt_absolute_time();
		actuator_test.function = function;
		actuator_test.value = value;
		actuator_test.action = release_control ? actuator_test_s::ACTION_RELEASE_CONTROL : actuator_test_s::ACTION_DO_CONTROL;
		actuator_test.timeout_ms = 0;
		_actuator_test_pub.publish(actuator_test);
	}

	ശൂന്യം sendActuatorArmed(തർക്കം armed, തർക്കം termination = അസത്യം, തർക്കം kill = അസത്യം, തർക്കം prearm = അസത്യം)
	{
		actuator_armed_s actuator_armed{};
		actuator_armed.timestamp = hrt_absolute_time();
		actuator_armed.armed = armed;
		actuator_armed.termination = termination;
		actuator_armed.kill = kill;
		actuator_armed.prearmed = prearm;
		_actuator_armed_pub.publish(actuator_armed);
	}

	ശൂന്യം reset()
	{
		memset(outputs, 0, വലുപ്പം(outputs));
		അക്കം_outputs = 0;
		അക്കം_updates = 0;
		mixer_changed = അസത്യം;
	}

	ദശാംശം outputs[MAX_ACTUATORS] {};
	പൂർണ്ണസംഖ്യ അക്കം_outputs{0};
	പൂർണ്ണസംഖ്യ അക്കം_updates{0};
	തർക്കം was_scheduled{അസത്യം};
	തർക്കം mixer_changed{അസത്യം};

സ്വകാര്യ:
	uORB::Publication<actuator_test_s> _actuator_test_pub{ORB_ID(actuator_test)};
	uORB::Publication<actuator_motors_s> _actuator_motors_pub{ORB_ID(actuator_motors)};
	uORB::Publication<actuator_servos_s> _actuator_servos_pub{ORB_ID(actuator_servos)};
	uORB::Publication<actuator_armed_s> _actuator_armed_pub{ORB_ID(actuator_armed)};
};

TEST_F(MixerModuleTest, basic)
{
	OutputModuleTest test_module;
	test_module.configureFunctions({});
	MixingOutput mixing_output{PARAM_PREFIX, MAX_അക്കം_OUTPUTS, test_module, MixingOutput::SchedulingPolicy::Disabled, അസത്യം, അസത്യം};
	mixing_output.set_All_Disarmed_മൂല്യങ്ങൾ(DISARMED_മൂല്യം);
	mixing_output.set_All_Failsafe_മൂല്യങ്ങൾ(FAILSAFE_മൂല്യം);
	mixing_output.set_All_Min_മൂല്യങ്ങൾ(MIN_മൂല്യം);
	mixing_output.set_All_Center_മൂല്യങ്ങൾ(CENTER_മൂല്യം);
	mixing_output.set_All_Max_മൂല്യങ്ങൾ(MAX_മൂല്യം);
	EXPECT_EQ(test_module.അക്കം_updates, 0);

	// all functions disabled: expect to get one single update to process disabling the output signal
	mixing_output.update();
	mixing_output.updateSubscriptions(അസത്യം);
	mixing_output.update();
	EXPECT_EQ(test_module.അക്കം_updates, 1);
	mixing_output.update();
	mixing_output.updateSubscriptions(അസത്യം);
	mixing_output.update();
	EXPECT_EQ(test_module.അക്കം_updates, 1);
	test_module.reset();

	// configure motor, ensure all still disarmed
	test_module.configureFunctions({(പൂർണ്ണസംഖ്യ)OutputFunction::Motor1});
	mixing_output.updateSubscriptions(അസത്യം);
	EXPECT_TRUE(test_module.mixer_changed);
	EXPECT_EQ(test_module.അക്കം_updates, update(mixing_output));
	EXPECT_EQ(test_module.അക്കം_outputs, MAX_അക്കം_OUTPUTS);

	വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < test_module.അക്കം_outputs; ++i) {
		EXPECT_EQ(test_module.outputs[i], DISARMED_മൂല്യം);
	}

	test_module.reset();

	// send motors -> still disarmed
	test_module.sendMotors({1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f});
	test_module.configureFunctions({
		0,
		(പൂർണ്ണസംഖ്യ)OutputFunction::Motor3,
		(പൂർണ്ണസംഖ്യ)OutputFunction::Motor1,
		(പൂർണ്ണസംഖ്യ)OutputFunction::Motor5});

	mixing_output.updateSubscriptions(അസത്യം);
	EXPECT_EQ(test_module.അക്കം_updates, update(mixing_output));
	EXPECT_EQ(test_module.അക്കം_outputs, MAX_അക്കം_OUTPUTS);

	വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < test_module.അക്കം_outputs; ++i) {
		EXPECT_EQ(test_module.outputs[i], DISARMED_മൂല്യം);
	}

	test_module.reset();


	// actuator test
	test_module.sendActuatorMotorTest((പൂർണ്ണസംഖ്യ)OutputFunction::Motor5, 1.f, അസത്യം);
	test_module.sendMotors({1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f});
	mixing_output.updateSubscriptions(അസത്യം);
	EXPECT_EQ(test_module.അക്കം_updates, update(mixing_output));
	EXPECT_EQ(test_module.അക്കം_outputs, MAX_അക്കം_OUTPUTS);

	വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < test_module.അക്കം_outputs; ++i) {
		എങ്കിൽ (i == 3) {
			EXPECT_EQ(test_module.outputs[i], MAX_മൂല്യം);

		} അല്ലെങ്കിൽ {
			EXPECT_EQ(test_module.outputs[i], DISARMED_മൂല്യം);
		}
	}

	test_module.reset();

	// stop
	test_module.sendActuatorMotorTest((പൂർണ്ണസംഖ്യ)OutputFunction::Motor5, 0.f, സത്യം);
	test_module.sendMotors({1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f});
	mixing_output.updateSubscriptions(അസത്യം);
	EXPECT_EQ(test_module.അക്കം_updates, update(mixing_output));
	EXPECT_EQ(test_module.അക്കം_outputs, MAX_അക്കം_OUTPUTS);

	വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < test_module.അക്കം_outputs; ++i) {
		EXPECT_EQ(test_module.outputs[i], DISARMED_മൂല്യം);
	}

	test_module.reset();

	EXPECT_FALSE(test_module.was_scheduled);
}

TEST_F(MixerModuleTest, arming)
{
	OutputModuleTest test_module;
	test_module.configureFunctions({
		0,
		(പൂർണ്ണസംഖ്യ)OutputFunction::Motor3,
		(പൂർണ്ണസംഖ്യ)OutputFunction::Motor1,
		(പൂർണ്ണസംഖ്യ)OutputFunction::Motor5,
		(പൂർണ്ണസംഖ്യ)OutputFunction::Servo3});
	MixingOutput mixing_output{PARAM_PREFIX, MAX_അക്കം_OUTPUTS, test_module, MixingOutput::SchedulingPolicy::Disabled, അസത്യം, അസത്യം};
	mixing_output.set_All_Disarmed_മൂല്യങ്ങൾ(DISARMED_മൂല്യം);
	mixing_output.set_All_Failsafe_മൂല്യങ്ങൾ(FAILSAFE_മൂല്യം);
	mixing_output.set_All_Min_മൂല്യങ്ങൾ(MIN_മൂല്യം);
	mixing_output.set_All_Center_മൂല്യങ്ങൾ(CENTER_മൂല്യം);
	mixing_output.set_All_Max_മൂല്യങ്ങൾ(MAX_മൂല്യം);

	test_module.sendMotors({1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f});
	test_module.sendActuatorArmed(അസത്യം);

	// ensure all disarmed
	mixing_output.updateSubscriptions(അസത്യം);
	EXPECT_EQ(test_module.അക്കം_updates, update(mixing_output));
	EXPECT_EQ(test_module.അക്കം_outputs, MAX_അക്കം_OUTPUTS);

	വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < MAX_അക്കം_OUTPUTS; ++i) {
		EXPECT_EQ(test_module.outputs[i], DISARMED_മൂല്യം);
	}

	test_module.reset();

	// arming
	test_module.sendMotors({0.5f, 1.f, 0.1f, 0.2f, 1.f, 1.f, 1.f, 1.f});
	test_module.sendActuatorArmed(സത്യം);

	EXPECT_EQ(test_module.അക്കം_updates, update(mixing_output));
	EXPECT_EQ(test_module.അക്കം_outputs, MAX_അക്കം_OUTPUTS);

	വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < MAX_അക്കം_OUTPUTS; ++i) {
		എങ്കിൽ (i == 1) {
			EXPECT_EQ(test_module.outputs[i], (MAX_മൂല്യം - MIN_മൂല്യം) * 0.1f + MIN_മൂല്യം);

		} അല്ലെങ്കിൽ എങ്കിൽ (i == 2) {
			EXPECT_EQ(test_module.outputs[i], (MAX_മൂല്യം - MIN_മൂല്യം) * 0.5f + MIN_മൂല്യം);

		} അല്ലെങ്കിൽ എങ്കിൽ (i == 3) {
			EXPECT_EQ(test_module.outputs[i], MAX_മൂല്യം);

		} അല്ലെങ്കിൽ {
			EXPECT_EQ(test_module.outputs[i], DISARMED_മൂല്യം);
		}
	}

	test_module.reset();

	// update motors
	test_module.sendMotors({0.9f, 1.f, 0.24f, 0.2f, 0.f, 1.f, 1.f, 1.f});
	mixing_output.updateSubscriptions(അസത്യം);
	mixing_output.update();

	വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < MAX_അക്കം_OUTPUTS; ++i) {
		എങ്കിൽ (i == 1) {
			EXPECT_EQ(test_module.outputs[i], (MAX_മൂല്യം - MIN_മൂല്യം) * 0.24f + MIN_മൂല്യം);

		} അല്ലെങ്കിൽ എങ്കിൽ (i == 2) {
			EXPECT_EQ(test_module.outputs[i], (MAX_മൂല്യം - MIN_മൂല്യം) * 0.9f + MIN_മൂല്യം);

		} അല്ലെങ്കിൽ എങ്കിൽ (i == 3) {
			EXPECT_EQ(test_module.outputs[i], MIN_മൂല്യം);

		} അല്ലെങ്കിൽ {
			EXPECT_EQ(test_module.outputs[i], DISARMED_മൂല്യം);
		}
	}

	test_module.reset();

	// failsafe
	test_module.sendActuatorArmed(സത്യം, സത്യം);
	test_module.sendMotors({0.5f, 1.f, 0.1f, 0.2f, 1.f, 1.f, 1.f, 1.f});
	mixing_output.update();

	വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < MAX_അക്കം_OUTPUTS; ++i) {
		EXPECT_EQ(test_module.outputs[i], FAILSAFE_മൂല്യം);
	}

	test_module.reset();

	// restore
	test_module.sendActuatorArmed(സത്യം, അസത്യം);
	test_module.sendMotors({1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f});
	mixing_output.update();

	വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < MAX_അക്കം_OUTPUTS; ++i) {
		എങ്കിൽ (i >= 1 && i <= 3) {
			EXPECT_EQ(test_module.outputs[i], MAX_മൂല്യം);

		} അല്ലെങ്കിൽ {
			EXPECT_EQ(test_module.outputs[i], DISARMED_മൂല്യം);
		}
	}

	test_module.reset();

	// manual lockdown
	test_module.sendActuatorArmed(സത്യം, അസത്യം, സത്യം);
	test_module.sendMotors({0.5f, 1.f, 0.1f, 0.2f, 1.f, 1.f, 1.f, 1.f});
	mixing_output.update();

	വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < MAX_അക്കം_OUTPUTS; ++i) {
		EXPECT_EQ(test_module.outputs[i], DISARMED_മൂല്യം);
	}

	test_module.reset();

	// restore
	test_module.sendActuatorArmed(സത്യം, അസത്യം);
	test_module.sendMotors({0.f, 0.f, 0.f, 0.f, 0.f, 1.f, 1.f, 1.f});
	mixing_output.update();

	വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < MAX_അക്കം_OUTPUTS; ++i) {
		എങ്കിൽ (i >= 1 && i <= 3) {
			EXPECT_EQ(test_module.outputs[i], MIN_മൂല്യം);

		} അല്ലെങ്കിൽ {
			EXPECT_EQ(test_module.outputs[i], DISARMED_മൂല്യം);
		}
	}

	test_module.reset();

	// set motor 5 reversible: expect output to be in center when commanding to 0
	test_module.sendActuatorArmed(സത്യം, അസത്യം);
	test_module.sendMotors({0.f, 0.f, 0.f, 0.f, 0.f, 1.f, 1.f, 1.f}, 1u << 4);
	mixing_output.update();
	EXPECT_EQ(mixing_output.reversibleOutputs(), 1u << 3);

	വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < MAX_അക്കം_OUTPUTS; ++i) {
		എങ്കിൽ (i == 1) {
			EXPECT_EQ(test_module.outputs[i], MIN_മൂല്യം);

		} അല്ലെങ്കിൽ എങ്കിൽ (i == 2) {
			EXPECT_EQ(test_module.outputs[i], MIN_മൂല്യം);

		} അല്ലെങ്കിൽ എങ്കിൽ (i == 3) {
			EXPECT_EQ(test_module.outputs[i], (MAX_മൂല്യം - MIN_മൂല്യം) * 0.5f + MIN_മൂല്യം);

		} അല്ലെങ്കിൽ {
			EXPECT_EQ(test_module.outputs[i], DISARMED_മൂല്യം);
		}
	}

	test_module.reset();

	// disarm
	test_module.sendActuatorArmed(അസത്യം);
	test_module.sendMotors({0.f, 0.f, 0.f, 0.f, 0.f, 1.f, 1.f, 1.f}, 1u << 4);
	mixing_output.update();

	വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < MAX_അക്കം_OUTPUTS; ++i) {
		EXPECT_EQ(test_module.outputs[i], DISARMED_മൂല്യം);
	}

	test_module.reset();

	EXPECT_FALSE(test_module.was_scheduled);
}

TEST_F(MixerModuleTest, prearm)
{
	OutputModuleTest test_module;
	test_module.configureFunctions({
		(പൂർണ്ണസംഖ്യ)OutputFunction::Motor1,
		(പൂർണ്ണസംഖ്യ)OutputFunction::Servo1});
	MixingOutput mixing_output{PARAM_PREFIX, MAX_അക്കം_OUTPUTS, test_module, MixingOutput::SchedulingPolicy::Disabled, അസത്യം, അസത്യം};
	mixing_output.set_All_Disarmed_മൂല്യങ്ങൾ(DISARMED_മൂല്യം);
	mixing_output.set_All_Failsafe_മൂല്യങ്ങൾ(FAILSAFE_മൂല്യം);
	mixing_output.set_All_Min_മൂല്യങ്ങൾ(MIN_മൂല്യം);
	mixing_output.set_All_Max_മൂല്യങ്ങൾ(MAX_മൂല്യം);

	test_module.sendMotors({1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f});
	test_module.sendServos({1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f});
	test_module.sendActuatorArmed(അസത്യം, അസത്യം, അസത്യം, സത്യം);

	// ensure all disarmed, except the servo
	mixing_output.updateSubscriptions(അസത്യം);
	EXPECT_EQ(test_module.അക്കം_updates, update(mixing_output));
	EXPECT_EQ(test_module.അക്കം_outputs, MAX_അക്കം_OUTPUTS);

	വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < MAX_അക്കം_OUTPUTS; ++i) {
		എങ്കിൽ (i == 1) {
			EXPECT_EQ(test_module.outputs[i], MAX_മൂല്യം);

		} അല്ലെങ്കിൽ {
			EXPECT_EQ(test_module.outputs[i], DISARMED_മൂല്യം);
		}
	}

	test_module.reset();

	EXPECT_FALSE(test_module.was_scheduled);
}

വർഗ്ഗം TestMixingOutput : പൊതു MixingOutput
{
പൊതു:
	TestMixingOutput(സ്ഥിരം അക്ഷരം *param_prefix, uint8_t max_അക്കം_outputs, OutputModuleInterface &interface,
			 SchedulingPolicy scheduling_policy,
			 തർക്കം support_esc_calibration, തർക്കം ramp_up = സത്യം)
		: MixingOutput(param_prefix, max_അക്കം_outputs, interface, scheduling_policy, support_esc_calibration, ramp_up)
	{};
	ദശാംശം output_limit_calc_single(പൂർണ്ണസംഖ്യ i, ദശാംശം value) സ്ഥിരം { തിരികെ MixingOutput::output_limit_calc_single(i, value); }
};

TEST_F(MixerModuleTest, OutputLimitCalcSingle)
{
	OutputModuleTest test_module;
	test_module.configureFunctions({(പൂർണ്ണസംഖ്യ)OutputFunction::Motor1});
	TestMixingOutput mixing_output{PARAM_PREFIX, MAX_അക്കം_OUTPUTS, test_module, MixingOutput::SchedulingPolicy::Disabled, അസത്യം, അസത്യം};

	mixing_output.set_All_Min_മൂല്യങ്ങൾ(MIN_മൂല്യം); // default range [1000,2000]
	mixing_output.set_All_Max_മൂല്യങ്ങൾ(MAX_മൂല്യം);
	mixing_output.set_All_Center_മൂല്യങ്ങൾ(CENTER_മൂല്യം); // Set center to middle value
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

	mixing_output.set_All_Min_മൂല്യങ്ങൾ(0); // lower range [0,20]
	mixing_output.set_All_Max_മൂല്യങ്ങൾ(20);
	mixing_output.set_All_Center_മൂല്യങ്ങൾ(10); // Set center to middle value
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

	mixing_output.set_All_Min_മൂല്യങ്ങൾ(20); // inverted range [20,0]
	mixing_output.set_All_Max_മൂല്യങ്ങൾ(0);
	mixing_output.set_All_Center_മൂല്യങ്ങൾ(10); // Set center to middle value
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

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

#अन्तर्भाव <gtest/gtest.h>
#अन्तर्भाव <array>

#अन्तर्भाव <parameters/param.h>
#अन्तर्भाव <uORB/topics/actuator_motors.h>
#अन्तर्भाव <uORB/topics/actuator_servos.h>
#अन्तर्भाव <uORB/topics/actuator_armed.h>
#अन्तर्भाव <uORB/topics/actuator_test.h>
#अन्तर्भाव <uORB/Publication.hpp>
#अन्तर्भाव <uORB/Subscription.hpp>

#अन्तर्भाव "mixer_module.hpp"

#यदि defined(CONFIG_ARCH_BOARD_PX4_SITL)
#define PARAM_PREFIX "PWM_MAIN"
#अन्यथा
#define PARAM_PREFIX "HIL_ACT"
#endif

स्थिर constexpr पूर्णाङ्क MAX_अङ्क_OUTPUTS = 8;
स्थिर constexpr पूर्णाङ्क DISARMED_मान = 900;
स्थिर constexpr पूर्णाङ्क FAILSAFE_मान = 800;
स्थिर constexpr पूर्णाङ्क MIN_मान = 1000;
स्थिर constexpr पूर्णाङ्क CENTER_मान = 1500;
स्थिर constexpr पूर्णाङ्क MAX_मान = 2000;

वर्ग MixerModuleTest : सार्वजनिक ::testing::Test
{
सार्वजनिक:
	शून्य SetUp() override
	{
		param_control_autosave(असत्य);
	}

	पूर्णाङ्क update(MixingOutput &mixing_output)
	{
		mixing_output.update();
		// make sure output_limit switches to ON (if outputs enabled)
		px4_usleep(50000 * 2);
		mixing_output.update();
		mixing_output.update();
		फर्काऊ 3; // expected number of output updates
	}

};

वर्ग OutputModuleTest : सार्वजनिक OutputModuleInterface
{
सार्वजनिक:
	OutputModuleTest() : OutputModuleInterface(MODULE_NAME, px4::wq_configurations::hp_default) {};

	~OutputModuleTest() override
	{
		// Hide this test's commands from the next test: a zero actuator command is not inert
		_actuator_motors_pub.unadvertise();
		_actuator_servos_pub.unadvertise();
		_actuator_armed_pub.unadvertise();
	}

	शून्य Run() override
	{
		was_scheduled = सत्य;
	}

	तार्किक updateOutputs(भिन्न outputs_[MAX_ACTUATORS], अचिन्हित अङ्क_outputs_2, अचिन्हित अङ्क_control_groups_updated) override
	{
		स_नकल(outputs, outputs_, परिमाण(outputs));
		अङ्क_outputs = अङ्क_outputs_2;
		++अङ्क_updates;
		फर्काऊ सत्य;
	}

	शून्य mixerChanged() override
	{
		mixer_changed = सत्य;
	}

	शून्य configureFunctions(अचर std::array<int32_t, MAX_अङ्क_OUTPUTS> &functions)
	{
		लागि (पूर्णाङ्क i = 0; i < MAX_अङ्क_OUTPUTS; ++i) {
			वर्ण buffer[17];

			snprintf(buffer, परिमाण(buffer), "%s_FUNC%u", PARAM_PREFIX, i + 1);
			param_set(param_find(buffer), &functions[i]);
		}

		updateParams();
	}

	शून्य sendMotors(अचर std::array<भिन्न, actuator_motors_s::अङ्क_CONTROLS> &motors, uint16_t reversible = 0)
	{
		actuator_motors_s actuator_motors{};
		actuator_motors.timestamp = hrt_absolute_time();
		actuator_motors.reversible_flags = reversible;

		लागि (अचिन्हित i = 0; i < motors.size(); ++i) {
			actuator_motors.control[i] = motors[i];
		}

		_actuator_motors_pub.publish(actuator_motors);
	}

	शून्य sendServos(अचर std::array<भिन्न, actuator_servos_s::अङ्क_CONTROLS> &servos)
	{
		actuator_servos_s actuator_servos{};
		actuator_servos.timestamp = hrt_absolute_time();

		लागि (अचिन्हित i = 0; i < servos.size(); ++i) {
			actuator_servos.control[i] = servos[i];
		}

		_actuator_servos_pub.publish(actuator_servos);
	}

	शून्य sendActuatorMotorTest(पूर्णाङ्क function, भिन्न value, तार्किक release_control)
	{
		actuator_test_s actuator_test{};
		actuator_test.timestamp = hrt_absolute_time();
		actuator_test.function = function;
		actuator_test.value = value;
		actuator_test.action = release_control ? actuator_test_s::ACTION_RELEASE_CONTROL : actuator_test_s::ACTION_DO_CONTROL;
		actuator_test.timeout_ms = 0;
		_actuator_test_pub.publish(actuator_test);
	}

	शून्य sendActuatorArmed(तार्किक armed, तार्किक termination = असत्य, तार्किक समाप्त = असत्य, तार्किक prearm = असत्य)
	{
		actuator_armed_s actuator_armed{};
		actuator_armed.timestamp = hrt_absolute_time();
		actuator_armed.armed = armed;
		actuator_armed.termination = termination;
		actuator_armed.समाप्त = समाप्त;
		actuator_armed.prearmed = prearm;
		_actuator_armed_pub.publish(actuator_armed);
	}

	शून्य reset()
	{
		स_रखो(outputs, 0, परिमाण(outputs));
		अङ्क_outputs = 0;
		अङ्क_updates = 0;
		mixer_changed = असत्य;
	}

	भिन्न outputs[MAX_ACTUATORS] {};
	पूर्णाङ्क अङ्क_outputs{0};
	पूर्णाङ्क अङ्क_updates{0};
	तार्किक was_scheduled{असत्य};
	तार्किक mixer_changed{असत्य};

गुप्त:
	uORB::Publication<actuator_test_s> _actuator_test_pub{ORB_ID(actuator_test)};
	uORB::Publication<actuator_motors_s> _actuator_motors_pub{ORB_ID(actuator_motors)};
	uORB::Publication<actuator_servos_s> _actuator_servos_pub{ORB_ID(actuator_servos)};
	uORB::Publication<actuator_armed_s> _actuator_armed_pub{ORB_ID(actuator_armed)};
};

TEST_F(MixerModuleTest, basic)
{
	OutputModuleTest test_module;
	test_module.configureFunctions({});
	MixingOutput mixing_output{PARAM_PREFIX, MAX_अङ्क_OUTPUTS, test_module, MixingOutput::SchedulingPolicy::Disabled, असत्य, असत्य};
	mixing_output.set_All_Disarmed_मानहरू(DISARMED_मान);
	mixing_output.set_All_Failsafe_मानहरू(FAILSAFE_मान);
	mixing_output.set_All_Min_मानहरू(MIN_मान);
	mixing_output.set_All_Center_मानहरू(CENTER_मान);
	mixing_output.set_All_Max_मानहरू(MAX_मान);
	EXPECT_EQ(test_module.अङ्क_updates, 0);

	// all functions disabled: expect to get one single update to process disabling the output signal
	mixing_output.update();
	mixing_output.updateSubscriptions(असत्य);
	mixing_output.update();
	EXPECT_EQ(test_module.अङ्क_updates, 1);
	mixing_output.update();
	mixing_output.updateSubscriptions(असत्य);
	mixing_output.update();
	EXPECT_EQ(test_module.अङ्क_updates, 1);
	test_module.reset();

	// configure motor, ensure all still disarmed
	test_module.configureFunctions({(पूर्णाङ्क)OutputFunction::Motor1});
	mixing_output.updateSubscriptions(असत्य);
	EXPECT_TRUE(test_module.mixer_changed);
	EXPECT_EQ(test_module.अङ्क_updates, update(mixing_output));
	EXPECT_EQ(test_module.अङ्क_outputs, MAX_अङ्क_OUTPUTS);

	लागि (पूर्णाङ्क i = 0; i < test_module.अङ्क_outputs; ++i) {
		EXPECT_EQ(test_module.outputs[i], DISARMED_मान);
	}

	test_module.reset();

	// send motors -> still disarmed
	test_module.sendMotors({1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f});
	test_module.configureFunctions({
		0,
		(पूर्णाङ्क)OutputFunction::Motor3,
		(पूर्णाङ्क)OutputFunction::Motor1,
		(पूर्णाङ्क)OutputFunction::Motor5});

	mixing_output.updateSubscriptions(असत्य);
	EXPECT_EQ(test_module.अङ्क_updates, update(mixing_output));
	EXPECT_EQ(test_module.अङ्क_outputs, MAX_अङ्क_OUTPUTS);

	लागि (पूर्णाङ्क i = 0; i < test_module.अङ्क_outputs; ++i) {
		EXPECT_EQ(test_module.outputs[i], DISARMED_मान);
	}

	test_module.reset();


	// actuator test
	test_module.sendActuatorMotorTest((पूर्णाङ्क)OutputFunction::Motor5, 1.f, असत्य);
	test_module.sendMotors({1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f});
	mixing_output.updateSubscriptions(असत्य);
	EXPECT_EQ(test_module.अङ्क_updates, update(mixing_output));
	EXPECT_EQ(test_module.अङ्क_outputs, MAX_अङ्क_OUTPUTS);

	लागि (पूर्णाङ्क i = 0; i < test_module.अङ्क_outputs; ++i) {
		यदि (i == 3) {
			EXPECT_EQ(test_module.outputs[i], MAX_मान);

		} अन्यथा {
			EXPECT_EQ(test_module.outputs[i], DISARMED_मान);
		}
	}

	test_module.reset();

	// stop
	test_module.sendActuatorMotorTest((पूर्णाङ्क)OutputFunction::Motor5, 0.f, सत्य);
	test_module.sendMotors({1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f});
	mixing_output.updateSubscriptions(असत्य);
	EXPECT_EQ(test_module.अङ्क_updates, update(mixing_output));
	EXPECT_EQ(test_module.अङ्क_outputs, MAX_अङ्क_OUTPUTS);

	लागि (पूर्णाङ्क i = 0; i < test_module.अङ्क_outputs; ++i) {
		EXPECT_EQ(test_module.outputs[i], DISARMED_मान);
	}

	test_module.reset();

	EXPECT_FALSE(test_module.was_scheduled);
}

TEST_F(MixerModuleTest, arming)
{
	OutputModuleTest test_module;
	test_module.configureFunctions({
		0,
		(पूर्णाङ्क)OutputFunction::Motor3,
		(पूर्णाङ्क)OutputFunction::Motor1,
		(पूर्णाङ्क)OutputFunction::Motor5,
		(पूर्णाङ्क)OutputFunction::Servo3});
	MixingOutput mixing_output{PARAM_PREFIX, MAX_अङ्क_OUTPUTS, test_module, MixingOutput::SchedulingPolicy::Disabled, असत्य, असत्य};
	mixing_output.set_All_Disarmed_मानहरू(DISARMED_मान);
	mixing_output.set_All_Failsafe_मानहरू(FAILSAFE_मान);
	mixing_output.set_All_Min_मानहरू(MIN_मान);
	mixing_output.set_All_Center_मानहरू(CENTER_मान);
	mixing_output.set_All_Max_मानहरू(MAX_मान);

	test_module.sendMotors({1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f});
	test_module.sendActuatorArmed(असत्य);

	// ensure all disarmed
	mixing_output.updateSubscriptions(असत्य);
	EXPECT_EQ(test_module.अङ्क_updates, update(mixing_output));
	EXPECT_EQ(test_module.अङ्क_outputs, MAX_अङ्क_OUTPUTS);

	लागि (पूर्णाङ्क i = 0; i < MAX_अङ्क_OUTPUTS; ++i) {
		EXPECT_EQ(test_module.outputs[i], DISARMED_मान);
	}

	test_module.reset();

	// arming
	test_module.sendMotors({0.5f, 1.f, 0.1f, 0.2f, 1.f, 1.f, 1.f, 1.f});
	test_module.sendActuatorArmed(सत्य);

	EXPECT_EQ(test_module.अङ्क_updates, update(mixing_output));
	EXPECT_EQ(test_module.अङ्क_outputs, MAX_अङ्क_OUTPUTS);

	लागि (पूर्णाङ्क i = 0; i < MAX_अङ्क_OUTPUTS; ++i) {
		यदि (i == 1) {
			EXPECT_EQ(test_module.outputs[i], (MAX_मान - MIN_मान) * 0.1f + MIN_मान);

		} अन्यथा यदि (i == 2) {
			EXPECT_EQ(test_module.outputs[i], (MAX_मान - MIN_मान) * 0.5f + MIN_मान);

		} अन्यथा यदि (i == 3) {
			EXPECT_EQ(test_module.outputs[i], MAX_मान);

		} अन्यथा {
			EXPECT_EQ(test_module.outputs[i], DISARMED_मान);
		}
	}

	test_module.reset();

	// update motors
	test_module.sendMotors({0.9f, 1.f, 0.24f, 0.2f, 0.f, 1.f, 1.f, 1.f});
	mixing_output.updateSubscriptions(असत्य);
	mixing_output.update();

	लागि (पूर्णाङ्क i = 0; i < MAX_अङ्क_OUTPUTS; ++i) {
		यदि (i == 1) {
			EXPECT_EQ(test_module.outputs[i], (MAX_मान - MIN_मान) * 0.24f + MIN_मान);

		} अन्यथा यदि (i == 2) {
			EXPECT_EQ(test_module.outputs[i], (MAX_मान - MIN_मान) * 0.9f + MIN_मान);

		} अन्यथा यदि (i == 3) {
			EXPECT_EQ(test_module.outputs[i], MIN_मान);

		} अन्यथा {
			EXPECT_EQ(test_module.outputs[i], DISARMED_मान);
		}
	}

	test_module.reset();

	// failsafe
	test_module.sendActuatorArmed(सत्य, सत्य);
	test_module.sendMotors({0.5f, 1.f, 0.1f, 0.2f, 1.f, 1.f, 1.f, 1.f});
	mixing_output.update();

	लागि (पूर्णाङ्क i = 0; i < MAX_अङ्क_OUTPUTS; ++i) {
		EXPECT_EQ(test_module.outputs[i], FAILSAFE_मान);
	}

	test_module.reset();

	// restore
	test_module.sendActuatorArmed(सत्य, असत्य);
	test_module.sendMotors({1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f});
	mixing_output.update();

	लागि (पूर्णाङ्क i = 0; i < MAX_अङ्क_OUTPUTS; ++i) {
		यदि (i >= 1 && i <= 3) {
			EXPECT_EQ(test_module.outputs[i], MAX_मान);

		} अन्यथा {
			EXPECT_EQ(test_module.outputs[i], DISARMED_मान);
		}
	}

	test_module.reset();

	// manual lockdown
	test_module.sendActuatorArmed(सत्य, असत्य, सत्य);
	test_module.sendMotors({0.5f, 1.f, 0.1f, 0.2f, 1.f, 1.f, 1.f, 1.f});
	mixing_output.update();

	लागि (पूर्णाङ्क i = 0; i < MAX_अङ्क_OUTPUTS; ++i) {
		EXPECT_EQ(test_module.outputs[i], DISARMED_मान);
	}

	test_module.reset();

	// restore
	test_module.sendActuatorArmed(सत्य, असत्य);
	test_module.sendMotors({0.f, 0.f, 0.f, 0.f, 0.f, 1.f, 1.f, 1.f});
	mixing_output.update();

	लागि (पूर्णाङ्क i = 0; i < MAX_अङ्क_OUTPUTS; ++i) {
		यदि (i >= 1 && i <= 3) {
			EXPECT_EQ(test_module.outputs[i], MIN_मान);

		} अन्यथा {
			EXPECT_EQ(test_module.outputs[i], DISARMED_मान);
		}
	}

	test_module.reset();

	// set motor 5 reversible: expect output to be in center when commanding to 0
	test_module.sendActuatorArmed(सत्य, असत्य);
	test_module.sendMotors({0.f, 0.f, 0.f, 0.f, 0.f, 1.f, 1.f, 1.f}, 1u << 4);
	mixing_output.update();
	EXPECT_EQ(mixing_output.reversibleOutputs(), 1u << 3);

	लागि (पूर्णाङ्क i = 0; i < MAX_अङ्क_OUTPUTS; ++i) {
		यदि (i == 1) {
			EXPECT_EQ(test_module.outputs[i], MIN_मान);

		} अन्यथा यदि (i == 2) {
			EXPECT_EQ(test_module.outputs[i], MIN_मान);

		} अन्यथा यदि (i == 3) {
			EXPECT_EQ(test_module.outputs[i], (MAX_मान - MIN_मान) * 0.5f + MIN_मान);

		} अन्यथा {
			EXPECT_EQ(test_module.outputs[i], DISARMED_मान);
		}
	}

	test_module.reset();

	// disarm
	test_module.sendActuatorArmed(असत्य);
	test_module.sendMotors({0.f, 0.f, 0.f, 0.f, 0.f, 1.f, 1.f, 1.f}, 1u << 4);
	mixing_output.update();

	लागि (पूर्णाङ्क i = 0; i < MAX_अङ्क_OUTPUTS; ++i) {
		EXPECT_EQ(test_module.outputs[i], DISARMED_मान);
	}

	test_module.reset();

	EXPECT_FALSE(test_module.was_scheduled);
}

TEST_F(MixerModuleTest, prearm)
{
	OutputModuleTest test_module;
	test_module.configureFunctions({
		(पूर्णाङ्क)OutputFunction::Motor1,
		(पूर्णाङ्क)OutputFunction::Servo1});
	MixingOutput mixing_output{PARAM_PREFIX, MAX_अङ्क_OUTPUTS, test_module, MixingOutput::SchedulingPolicy::Disabled, असत्य, असत्य};
	mixing_output.set_All_Disarmed_मानहरू(DISARMED_मान);
	mixing_output.set_All_Failsafe_मानहरू(FAILSAFE_मान);
	mixing_output.set_All_Min_मानहरू(MIN_मान);
	mixing_output.set_All_Max_मानहरू(MAX_मान);

	test_module.sendMotors({1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f});
	test_module.sendServos({1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f});
	test_module.sendActuatorArmed(असत्य, असत्य, असत्य, सत्य);

	// ensure all disarmed, except the servo
	mixing_output.updateSubscriptions(असत्य);
	EXPECT_EQ(test_module.अङ्क_updates, update(mixing_output));
	EXPECT_EQ(test_module.अङ्क_outputs, MAX_अङ्क_OUTPUTS);

	लागि (पूर्णाङ्क i = 0; i < MAX_अङ्क_OUTPUTS; ++i) {
		यदि (i == 1) {
			EXPECT_EQ(test_module.outputs[i], MAX_मान);

		} अन्यथा {
			EXPECT_EQ(test_module.outputs[i], DISARMED_मान);
		}
	}

	test_module.reset();

	EXPECT_FALSE(test_module.was_scheduled);
}

वर्ग TestMixingOutput : सार्वजनिक MixingOutput
{
सार्वजनिक:
	TestMixingOutput(अचर वर्ण *param_prefix, uint8_t max_अङ्क_outputs, OutputModuleInterface &interface,
			 SchedulingPolicy scheduling_policy,
			 तार्किक support_esc_calibration, तार्किक ramp_up = सत्य)
		: MixingOutput(param_prefix, max_अङ्क_outputs, interface, scheduling_policy, support_esc_calibration, ramp_up)
	{};
	भिन्न output_limit_calc_single(पूर्णाङ्क i, भिन्न value) अचर { फर्काऊ MixingOutput::output_limit_calc_single(i, value); }
};

TEST_F(MixerModuleTest, OutputLimitCalcSingle)
{
	OutputModuleTest test_module;
	test_module.configureFunctions({(पूर्णाङ्क)OutputFunction::Motor1});
	TestMixingOutput mixing_output{PARAM_PREFIX, MAX_अङ्क_OUTPUTS, test_module, MixingOutput::SchedulingPolicy::Disabled, असत्य, असत्य};

	mixing_output.set_All_Min_मानहरू(MIN_मान); // default range [1000,2000]
	mixing_output.set_All_Max_मानहरू(MAX_मान);
	mixing_output.set_All_Center_मानहरू(CENTER_मान); // Set center to middle value
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

	mixing_output.set_All_Min_मानहरू(0); // lower range [0,20]
	mixing_output.set_All_Max_मानहरू(20);
	mixing_output.set_All_Center_मानहरू(10); // Set center to middle value
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

	mixing_output.set_All_Min_मानहरू(20); // inverted range [20,0]
	mixing_output.set_All_Max_मानहरू(0);
	mixing_output.set_All_Center_मानहरू(10); // Set center to middle value
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

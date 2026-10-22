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

#অন্তর্ভাব <gtest/gtest.h>
#অন্তর্ভাব <array>

#অন্তর্ভাব <parameters/param.h>
#অন্তর্ভাব <uORB/topics/actuator_motors.h>
#অন্তর্ভাব <uORB/topics/actuator_servos.h>
#অন্তর্ভাব <uORB/topics/actuator_armed.h>
#অন্তর্ভাব <uORB/topics/actuator_test.h>
#অন্তর্ভাব <uORB/Publication.hpp>
#অন্তর্ভাব <uORB/Subscription.hpp>

#অন্তর্ভাব "mixer_module.hpp"

#যদি defined(CONFIG_ARCH_BOARD_PX4_SITL)
#define PARAM_PREFIX "PWM_MAIN"
#অন্যথা
#define PARAM_PREFIX "HIL_ACT"
#endif

স্থির constexpr পূর্ণাঙ্ক MAX_অঙ্ক_OUTPUTS = 8;
স্থির constexpr পূর্ণাঙ্ক DISARMED_মান = 900;
স্থির constexpr পূর্ণাঙ্ক FAILSAFE_মান = 800;
স্থির constexpr পূর্ণাঙ্ক MIN_মান = 1000;
স্থির constexpr পূর্ণাঙ্ক CENTER_মান = 1500;
স্থির constexpr পূর্ণাঙ্ক MAX_মান = 2000;

বর্গ MixerModuleTest : সার্বজনিক ::testing::Test
{
সার্বজনিক:
	শূন্য SetUp() override
	{
		param_control_autosave(অসত্য);
	}

	পূর্ণাঙ্ক update(MixingOutput &mixing_output)
	{
		mixing_output.update();
		// make sure output_limit switches to ON (if outputs enabled)
		px4_usleep(50000 * 2);
		mixing_output.update();
		mixing_output.update();
		প্রত্যাগম 3; // expected number of output updates
	}

};

বর্গ OutputModuleTest : সার্বজনিক OutputModuleInterface
{
সার্বজনিক:
	OutputModuleTest() : OutputModuleInterface(MODULE_NAME, px4::wq_configurations::hp_default) {};

	~OutputModuleTest() override
	{
		// Hide this test's commands from the next test: a zero actuator command is not inert
		_actuator_motors_pub.unadvertise();
		_actuator_servos_pub.unadvertise();
		_actuator_armed_pub.unadvertise();
	}

	শূন্য Run() override
	{
		was_scheduled = সত্য;
	}

	তার্কিক updateOutputs(ভিন্ন outputs_[MAX_ACTUATORS], unsigned অঙ্ক_outputs_2, unsigned অঙ্ক_control_groups_updated) override
	{
		memcpy(outputs, outputs_, পরিমাণ(outputs));
		অঙ্ক_outputs = অঙ্ক_outputs_2;
		++অঙ্ক_updates;
		প্রত্যাগম সত্য;
	}

	শূন্য mixerChanged() override
	{
		mixer_changed = সত্য;
	}

	শূন্য configureFunctions(ধ্রুব std::array<int32_t, MAX_অঙ্ক_OUTPUTS> &functions)
	{
		প্রতি (পূর্ণাঙ্ক i = 0; i < MAX_অঙ্ক_OUTPUTS; ++i) {
			বর্ণ buffer[17];

			snprintf(buffer, পরিমাণ(buffer), "%s_FUNC%u", PARAM_PREFIX, i + 1);
			param_set(param_find(buffer), &functions[i]);
		}

		updateParams();
	}

	শূন্য sendMotors(ধ্রুব std::array<ভিন্ন, actuator_motors_s::অঙ্ক_CONTROLS> &motors, uint16_t reversible = 0)
	{
		actuator_motors_s actuator_motors{};
		actuator_motors.timestamp = hrt_absolute_time();
		actuator_motors.reversible_flags = reversible;

		প্রতি (unsigned i = 0; i < motors.size(); ++i) {
			actuator_motors.control[i] = motors[i];
		}

		_actuator_motors_pub.publish(actuator_motors);
	}

	শূন্য sendServos(ধ্রুব std::array<ভিন্ন, actuator_servos_s::অঙ্ক_CONTROLS> &servos)
	{
		actuator_servos_s actuator_servos{};
		actuator_servos.timestamp = hrt_absolute_time();

		প্রতি (unsigned i = 0; i < servos.size(); ++i) {
			actuator_servos.control[i] = servos[i];
		}

		_actuator_servos_pub.publish(actuator_servos);
	}

	শূন্য sendActuatorMotorTest(পূর্ণাঙ্ক function, ভিন্ন value, তার্কিক release_control)
	{
		actuator_test_s actuator_test{};
		actuator_test.timestamp = hrt_absolute_time();
		actuator_test.function = function;
		actuator_test.value = value;
		actuator_test.action = release_control ? actuator_test_s::ACTION_RELEASE_CONTROL : actuator_test_s::ACTION_DO_CONTROL;
		actuator_test.timeout_ms = 0;
		_actuator_test_pub.publish(actuator_test);
	}

	শূন্য sendActuatorArmed(তার্কিক armed, তার্কিক termination = অসত্য, তার্কিক kill = অসত্য, তার্কিক prearm = অসত্য)
	{
		actuator_armed_s actuator_armed{};
		actuator_armed.timestamp = hrt_absolute_time();
		actuator_armed.armed = armed;
		actuator_armed.termination = termination;
		actuator_armed.kill = kill;
		actuator_armed.prearmed = prearm;
		_actuator_armed_pub.publish(actuator_armed);
	}

	শূন্য reset()
	{
		memset(outputs, 0, পরিমাণ(outputs));
		অঙ্ক_outputs = 0;
		অঙ্ক_updates = 0;
		mixer_changed = অসত্য;
	}

	ভিন্ন outputs[MAX_ACTUATORS] {};
	পূর্ণাঙ্ক অঙ্ক_outputs{0};
	পূর্ণাঙ্ক অঙ্ক_updates{0};
	তার্কিক was_scheduled{অসত্য};
	তার্কিক mixer_changed{অসত্য};

গুপ্ত:
	uORB::Publication<actuator_test_s> _actuator_test_pub{ORB_ID(actuator_test)};
	uORB::Publication<actuator_motors_s> _actuator_motors_pub{ORB_ID(actuator_motors)};
	uORB::Publication<actuator_servos_s> _actuator_servos_pub{ORB_ID(actuator_servos)};
	uORB::Publication<actuator_armed_s> _actuator_armed_pub{ORB_ID(actuator_armed)};
};

TEST_F(MixerModuleTest, basic)
{
	OutputModuleTest test_module;
	test_module.configureFunctions({});
	MixingOutput mixing_output{PARAM_PREFIX, MAX_অঙ্ক_OUTPUTS, test_module, MixingOutput::SchedulingPolicy::Disabled, অসত্য, অসত্য};
	mixing_output.set_All_Disarmed_মানসমূহ(DISARMED_মান);
	mixing_output.set_All_Failsafe_মানসমূহ(FAILSAFE_মান);
	mixing_output.set_All_Min_মানসমূহ(MIN_মান);
	mixing_output.set_All_Center_মানসমূহ(CENTER_মান);
	mixing_output.set_All_Max_মানসমূহ(MAX_মান);
	EXPECT_EQ(test_module.অঙ্ক_updates, 0);

	// all functions disabled: expect to get one single update to process disabling the output signal
	mixing_output.update();
	mixing_output.updateSubscriptions(অসত্য);
	mixing_output.update();
	EXPECT_EQ(test_module.অঙ্ক_updates, 1);
	mixing_output.update();
	mixing_output.updateSubscriptions(অসত্য);
	mixing_output.update();
	EXPECT_EQ(test_module.অঙ্ক_updates, 1);
	test_module.reset();

	// configure motor, ensure all still disarmed
	test_module.configureFunctions({(পূর্ণাঙ্ক)OutputFunction::Motor1});
	mixing_output.updateSubscriptions(অসত্য);
	EXPECT_TRUE(test_module.mixer_changed);
	EXPECT_EQ(test_module.অঙ্ক_updates, update(mixing_output));
	EXPECT_EQ(test_module.অঙ্ক_outputs, MAX_অঙ্ক_OUTPUTS);

	প্রতি (পূর্ণাঙ্ক i = 0; i < test_module.অঙ্ক_outputs; ++i) {
		EXPECT_EQ(test_module.outputs[i], DISARMED_মান);
	}

	test_module.reset();

	// send motors -> still disarmed
	test_module.sendMotors({1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f});
	test_module.configureFunctions({
		0,
		(পূর্ণাঙ্ক)OutputFunction::Motor3,
		(পূর্ণাঙ্ক)OutputFunction::Motor1,
		(পূর্ণাঙ্ক)OutputFunction::Motor5});

	mixing_output.updateSubscriptions(অসত্য);
	EXPECT_EQ(test_module.অঙ্ক_updates, update(mixing_output));
	EXPECT_EQ(test_module.অঙ্ক_outputs, MAX_অঙ্ক_OUTPUTS);

	প্রতি (পূর্ণাঙ্ক i = 0; i < test_module.অঙ্ক_outputs; ++i) {
		EXPECT_EQ(test_module.outputs[i], DISARMED_মান);
	}

	test_module.reset();


	// actuator test
	test_module.sendActuatorMotorTest((পূর্ণাঙ্ক)OutputFunction::Motor5, 1.f, অসত্য);
	test_module.sendMotors({1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f});
	mixing_output.updateSubscriptions(অসত্য);
	EXPECT_EQ(test_module.অঙ্ক_updates, update(mixing_output));
	EXPECT_EQ(test_module.অঙ্ক_outputs, MAX_অঙ্ক_OUTPUTS);

	প্রতি (পূর্ণাঙ্ক i = 0; i < test_module.অঙ্ক_outputs; ++i) {
		যদি (i == 3) {
			EXPECT_EQ(test_module.outputs[i], MAX_মান);

		} অন্যথা {
			EXPECT_EQ(test_module.outputs[i], DISARMED_মান);
		}
	}

	test_module.reset();

	// stop
	test_module.sendActuatorMotorTest((পূর্ণাঙ্ক)OutputFunction::Motor5, 0.f, সত্য);
	test_module.sendMotors({1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f});
	mixing_output.updateSubscriptions(অসত্য);
	EXPECT_EQ(test_module.অঙ্ক_updates, update(mixing_output));
	EXPECT_EQ(test_module.অঙ্ক_outputs, MAX_অঙ্ক_OUTPUTS);

	প্রতি (পূর্ণাঙ্ক i = 0; i < test_module.অঙ্ক_outputs; ++i) {
		EXPECT_EQ(test_module.outputs[i], DISARMED_মান);
	}

	test_module.reset();

	EXPECT_FALSE(test_module.was_scheduled);
}

TEST_F(MixerModuleTest, arming)
{
	OutputModuleTest test_module;
	test_module.configureFunctions({
		0,
		(পূর্ণাঙ্ক)OutputFunction::Motor3,
		(পূর্ণাঙ্ক)OutputFunction::Motor1,
		(পূর্ণাঙ্ক)OutputFunction::Motor5,
		(পূর্ণাঙ্ক)OutputFunction::Servo3});
	MixingOutput mixing_output{PARAM_PREFIX, MAX_অঙ্ক_OUTPUTS, test_module, MixingOutput::SchedulingPolicy::Disabled, অসত্য, অসত্য};
	mixing_output.set_All_Disarmed_মানসমূহ(DISARMED_মান);
	mixing_output.set_All_Failsafe_মানসমূহ(FAILSAFE_মান);
	mixing_output.set_All_Min_মানসমূহ(MIN_মান);
	mixing_output.set_All_Center_মানসমূহ(CENTER_মান);
	mixing_output.set_All_Max_মানসমূহ(MAX_মান);

	test_module.sendMotors({1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f});
	test_module.sendActuatorArmed(অসত্য);

	// ensure all disarmed
	mixing_output.updateSubscriptions(অসত্য);
	EXPECT_EQ(test_module.অঙ্ক_updates, update(mixing_output));
	EXPECT_EQ(test_module.অঙ্ক_outputs, MAX_অঙ্ক_OUTPUTS);

	প্রতি (পূর্ণাঙ্ক i = 0; i < MAX_অঙ্ক_OUTPUTS; ++i) {
		EXPECT_EQ(test_module.outputs[i], DISARMED_মান);
	}

	test_module.reset();

	// arming
	test_module.sendMotors({0.5f, 1.f, 0.1f, 0.2f, 1.f, 1.f, 1.f, 1.f});
	test_module.sendActuatorArmed(সত্য);

	EXPECT_EQ(test_module.অঙ্ক_updates, update(mixing_output));
	EXPECT_EQ(test_module.অঙ্ক_outputs, MAX_অঙ্ক_OUTPUTS);

	প্রতি (পূর্ণাঙ্ক i = 0; i < MAX_অঙ্ক_OUTPUTS; ++i) {
		যদি (i == 1) {
			EXPECT_EQ(test_module.outputs[i], (MAX_মান - MIN_মান) * 0.1f + MIN_মান);

		} অন্যথা যদি (i == 2) {
			EXPECT_EQ(test_module.outputs[i], (MAX_মান - MIN_মান) * 0.5f + MIN_মান);

		} অন্যথা যদি (i == 3) {
			EXPECT_EQ(test_module.outputs[i], MAX_মান);

		} অন্যথা {
			EXPECT_EQ(test_module.outputs[i], DISARMED_মান);
		}
	}

	test_module.reset();

	// update motors
	test_module.sendMotors({0.9f, 1.f, 0.24f, 0.2f, 0.f, 1.f, 1.f, 1.f});
	mixing_output.updateSubscriptions(অসত্য);
	mixing_output.update();

	প্রতি (পূর্ণাঙ্ক i = 0; i < MAX_অঙ্ক_OUTPUTS; ++i) {
		যদি (i == 1) {
			EXPECT_EQ(test_module.outputs[i], (MAX_মান - MIN_মান) * 0.24f + MIN_মান);

		} অন্যথা যদি (i == 2) {
			EXPECT_EQ(test_module.outputs[i], (MAX_মান - MIN_মান) * 0.9f + MIN_মান);

		} অন্যথা যদি (i == 3) {
			EXPECT_EQ(test_module.outputs[i], MIN_মান);

		} অন্যথা {
			EXPECT_EQ(test_module.outputs[i], DISARMED_মান);
		}
	}

	test_module.reset();

	// failsafe
	test_module.sendActuatorArmed(সত্য, সত্য);
	test_module.sendMotors({0.5f, 1.f, 0.1f, 0.2f, 1.f, 1.f, 1.f, 1.f});
	mixing_output.update();

	প্রতি (পূর্ণাঙ্ক i = 0; i < MAX_অঙ্ক_OUTPUTS; ++i) {
		EXPECT_EQ(test_module.outputs[i], FAILSAFE_মান);
	}

	test_module.reset();

	// restore
	test_module.sendActuatorArmed(সত্য, অসত্য);
	test_module.sendMotors({1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f});
	mixing_output.update();

	প্রতি (পূর্ণাঙ্ক i = 0; i < MAX_অঙ্ক_OUTPUTS; ++i) {
		যদি (i >= 1 && i <= 3) {
			EXPECT_EQ(test_module.outputs[i], MAX_মান);

		} অন্যথা {
			EXPECT_EQ(test_module.outputs[i], DISARMED_মান);
		}
	}

	test_module.reset();

	// manual lockdown
	test_module.sendActuatorArmed(সত্য, অসত্য, সত্য);
	test_module.sendMotors({0.5f, 1.f, 0.1f, 0.2f, 1.f, 1.f, 1.f, 1.f});
	mixing_output.update();

	প্রতি (পূর্ণাঙ্ক i = 0; i < MAX_অঙ্ক_OUTPUTS; ++i) {
		EXPECT_EQ(test_module.outputs[i], DISARMED_মান);
	}

	test_module.reset();

	// restore
	test_module.sendActuatorArmed(সত্য, অসত্য);
	test_module.sendMotors({0.f, 0.f, 0.f, 0.f, 0.f, 1.f, 1.f, 1.f});
	mixing_output.update();

	প্রতি (পূর্ণাঙ্ক i = 0; i < MAX_অঙ্ক_OUTPUTS; ++i) {
		যদি (i >= 1 && i <= 3) {
			EXPECT_EQ(test_module.outputs[i], MIN_মান);

		} অন্যথা {
			EXPECT_EQ(test_module.outputs[i], DISARMED_মান);
		}
	}

	test_module.reset();

	// set motor 5 reversible: expect output to be in center when commanding to 0
	test_module.sendActuatorArmed(সত্য, অসত্য);
	test_module.sendMotors({0.f, 0.f, 0.f, 0.f, 0.f, 1.f, 1.f, 1.f}, 1u << 4);
	mixing_output.update();
	EXPECT_EQ(mixing_output.reversibleOutputs(), 1u << 3);

	প্রতি (পূর্ণাঙ্ক i = 0; i < MAX_অঙ্ক_OUTPUTS; ++i) {
		যদি (i == 1) {
			EXPECT_EQ(test_module.outputs[i], MIN_মান);

		} অন্যথা যদি (i == 2) {
			EXPECT_EQ(test_module.outputs[i], MIN_মান);

		} অন্যথা যদি (i == 3) {
			EXPECT_EQ(test_module.outputs[i], (MAX_মান - MIN_মান) * 0.5f + MIN_মান);

		} অন্যথা {
			EXPECT_EQ(test_module.outputs[i], DISARMED_মান);
		}
	}

	test_module.reset();

	// disarm
	test_module.sendActuatorArmed(অসত্য);
	test_module.sendMotors({0.f, 0.f, 0.f, 0.f, 0.f, 1.f, 1.f, 1.f}, 1u << 4);
	mixing_output.update();

	প্রতি (পূর্ণাঙ্ক i = 0; i < MAX_অঙ্ক_OUTPUTS; ++i) {
		EXPECT_EQ(test_module.outputs[i], DISARMED_মান);
	}

	test_module.reset();

	EXPECT_FALSE(test_module.was_scheduled);
}

TEST_F(MixerModuleTest, prearm)
{
	OutputModuleTest test_module;
	test_module.configureFunctions({
		(পূর্ণাঙ্ক)OutputFunction::Motor1,
		(পূর্ণাঙ্ক)OutputFunction::Servo1});
	MixingOutput mixing_output{PARAM_PREFIX, MAX_অঙ্ক_OUTPUTS, test_module, MixingOutput::SchedulingPolicy::Disabled, অসত্য, অসত্য};
	mixing_output.set_All_Disarmed_মানসমূহ(DISARMED_মান);
	mixing_output.set_All_Failsafe_মানসমূহ(FAILSAFE_মান);
	mixing_output.set_All_Min_মানসমূহ(MIN_মান);
	mixing_output.set_All_Max_মানসমূহ(MAX_মান);

	test_module.sendMotors({1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f});
	test_module.sendServos({1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f});
	test_module.sendActuatorArmed(অসত্য, অসত্য, অসত্য, সত্য);

	// ensure all disarmed, except the servo
	mixing_output.updateSubscriptions(অসত্য);
	EXPECT_EQ(test_module.অঙ্ক_updates, update(mixing_output));
	EXPECT_EQ(test_module.অঙ্ক_outputs, MAX_অঙ্ক_OUTPUTS);

	প্রতি (পূর্ণাঙ্ক i = 0; i < MAX_অঙ্ক_OUTPUTS; ++i) {
		যদি (i == 1) {
			EXPECT_EQ(test_module.outputs[i], MAX_মান);

		} অন্যথা {
			EXPECT_EQ(test_module.outputs[i], DISARMED_মান);
		}
	}

	test_module.reset();

	EXPECT_FALSE(test_module.was_scheduled);
}

বর্গ TestMixingOutput : সার্বজনিক MixingOutput
{
সার্বজনিক:
	TestMixingOutput(ধ্রুব বর্ণ *param_prefix, uint8_t max_অঙ্ক_outputs, OutputModuleInterface &interface,
			 SchedulingPolicy scheduling_policy,
			 তার্কিক support_esc_calibration, তার্কিক ramp_up = সত্য)
		: MixingOutput(param_prefix, max_অঙ্ক_outputs, interface, scheduling_policy, support_esc_calibration, ramp_up)
	{};
	ভিন্ন output_limit_calc_single(পূর্ণাঙ্ক i, ভিন্ন value) ধ্রুব { প্রত্যাগম MixingOutput::output_limit_calc_single(i, value); }
};

TEST_F(MixerModuleTest, OutputLimitCalcSingle)
{
	OutputModuleTest test_module;
	test_module.configureFunctions({(পূর্ণাঙ্ক)OutputFunction::Motor1});
	TestMixingOutput mixing_output{PARAM_PREFIX, MAX_অঙ্ক_OUTPUTS, test_module, MixingOutput::SchedulingPolicy::Disabled, অসত্য, অসত্য};

	mixing_output.set_All_Min_মানসমূহ(MIN_মান); // default range [1000,2000]
	mixing_output.set_All_Max_মানসমূহ(MAX_মান);
	mixing_output.set_All_Center_মানসমূহ(CENTER_মান); // Set center to middle value
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

	mixing_output.set_All_Min_মানসমূহ(0); // lower range [0,20]
	mixing_output.set_All_Max_মানসমূহ(20);
	mixing_output.set_All_Center_মানসমূহ(10); // Set center to middle value
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

	mixing_output.set_All_Min_মানসমূহ(20); // inverted range [20,0]
	mixing_output.set_All_Max_মানসমূহ(0);
	mixing_output.set_All_Center_মানসমূহ(10); // Set center to middle value
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

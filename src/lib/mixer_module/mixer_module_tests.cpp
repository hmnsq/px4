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
#تعریف_کن PARAM_پیشوند "PWM_MAIN"
#وإلا
#تعریف_کن PARAM_پیشوند "HIL_ACT"
#پایان_اگر

ساكن constexpr صحيح MAX_رقم_OUTPUTS = 8;
ساكن constexpr صحيح DISARMED_مقدار = 900;
ساكن constexpr صحيح FAILSAFE_مقدار = 800;
ساكن constexpr صحيح MIN_مقدار = 1000;
ساكن constexpr صحيح CENTER_مقدار = 1500;
ساكن constexpr صحيح MAX_مقدار = 2000;

صنف MixerModuleTest : عام ::testing::Test
{
عام:
	فراغ مجموعه_به_بالا() override
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

	فراغ پیاپی() override
	{
		was_scheduled = درست;
	}

	منطقي updateOutputs(عائم outputs_[MAX_ACTUATORS], بیعلامت رقم_outputs_2, بیعلامت رقم_control_groups_updated) override
	{
		کپی_حافظه(outputs, outputs_, حجم(outputs));
		رقم_outputs = رقم_outputs_2;
		++رقم_updates;
		أرجع درست;
	}

	فراغ mixer_تغییریافته() override
	{
		mixer_تغییریافته_2 = درست;
	}

	فراغ configureFunctions(ثابت std::array<int32_t, MAX_رقم_OUTPUTS> &functions)
	{
		لكل (صحيح i = 0; i < MAX_رقم_OUTPUTS; ++i) {
			حرف میانگیر_2[17];

			snprintf(میانگیر_2, حجم(میانگیر_2), "%s_FUNC%u", PARAM_پیشوند, i + 1);
			param_مجموعه(param_پیدا_کن(میانگیر_2), &functions[i]);
		}

		updateParams();
	}

	فراغ sendMotors(ثابت std::array<عائم, actuator_motors_s::رقم_CONTROLS> &motors, uint16_t reversible = 0)
	{
		actuator_motors_s actuator_motors{};
		actuator_motors.timestamp = hrt_absolute_زمان();
		actuator_motors.reversible_flags = reversible;

		لكل (بیعلامت i = 0; i < motors.اندازه(); ++i) {
			actuator_motors.control[i] = motors[i];
		}

		_actuator_motors_pub.publish(actuator_motors);
	}

	فراغ sendServos(ثابت std::array<عائم, actuator_servos_s::رقم_CONTROLS> &servos)
	{
		actuator_servos_s actuator_servos{};
		actuator_servos.timestamp = hrt_absolute_زمان();

		لكل (بیعلامت i = 0; i < servos.اندازه(); ++i) {
			actuator_servos.control[i] = servos[i];
		}

		_actuator_servos_pub.publish(actuator_servos);
	}

	فراغ sendActuatorMotorTest(صحيح function, عائم value, منطقي release_control)
	{
		actuator_test_s actuator_test{};
		actuator_test.timestamp = hrt_absolute_زمان();
		actuator_test.function = function;
		actuator_test.value = value;
		actuator_test.action = release_control ? actuator_test_s::ACTION_RELEASE_CONTROL : actuator_test_s::ACTION_DO_CONTROL;
		actuator_test.timeout_ms = 0;
		_actuator_test_pub.publish(actuator_test);
	}

	فراغ sendActuatorArmed(منطقي armed, منطقي termination = خطأ, منطقي kill = خطأ, منطقي prearm = خطأ)
	{
		actuator_armed_s actuator_armed{};
		actuator_armed.timestamp = hrt_absolute_زمان();
		actuator_armed.armed = armed;
		actuator_armed.termination = termination;
		actuator_armed.kill = kill;
		actuator_armed.prearmed = prearm;
		_actuator_armed_pub.publish(actuator_armed);
	}

	فراغ reset()
	{
		memset(outputs, 0, حجم(outputs));
		رقم_outputs = 0;
		رقم_updates = 0;
		mixer_تغییریافته_2 = خطأ;
	}

	عائم outputs[MAX_ACTUATORS] {};
	صحيح رقم_outputs{0};
	صحيح رقم_updates{0};
	منطقي was_scheduled{خطأ};
	منطقي mixer_تغییریافته_2{خطأ};

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
	MixingOutput mixing_output{PARAM_پیشوند, MAX_رقم_OUTPUTS, test_module, MixingOutput::SchedulingPolicy::Disabled, خطأ, خطأ};
	mixing_output.مجموعه_All_Disarmed_مقادیر(DISARMED_مقدار);
	mixing_output.مجموعه_All_Failsafe_مقادیر(FAILSAFE_مقدار);
	mixing_output.مجموعه_All_Min_مقادیر(MIN_مقدار);
	mixing_output.مجموعه_All_Center_مقادیر(CENTER_مقدار);
	mixing_output.مجموعه_All_Max_مقادیر(MAX_مقدار);
	EXPECT_EQ(test_module.رقم_updates, 0);

	// all functions disabled: expect to get one single update to process disabling the output signal
	mixing_output.update();
	mixing_output.updateSubscriptions(خطأ);
	mixing_output.update();
	EXPECT_EQ(test_module.رقم_updates, 1);
	mixing_output.update();
	mixing_output.updateSubscriptions(خطأ);
	mixing_output.update();
	EXPECT_EQ(test_module.رقم_updates, 1);
	test_module.reset();

	// configure motor, ensure all still disarmed
	test_module.configureFunctions({(صحيح)OutputFunction::Motor1});
	mixing_output.updateSubscriptions(خطأ);
	EXPECT_TRUE(test_module.mixer_تغییریافته_2);
	EXPECT_EQ(test_module.رقم_updates, update(mixing_output));
	EXPECT_EQ(test_module.رقم_outputs, MAX_رقم_OUTPUTS);

	لكل (صحيح i = 0; i < test_module.رقم_outputs; ++i) {
		EXPECT_EQ(test_module.outputs[i], DISARMED_مقدار);
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
	EXPECT_EQ(test_module.رقم_updates, update(mixing_output));
	EXPECT_EQ(test_module.رقم_outputs, MAX_رقم_OUTPUTS);

	لكل (صحيح i = 0; i < test_module.رقم_outputs; ++i) {
		EXPECT_EQ(test_module.outputs[i], DISARMED_مقدار);
	}

	test_module.reset();


	// actuator test
	test_module.sendActuatorMotorTest((صحيح)OutputFunction::Motor5, 1.f, خطأ);
	test_module.sendMotors({1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f});
	mixing_output.updateSubscriptions(خطأ);
	EXPECT_EQ(test_module.رقم_updates, update(mixing_output));
	EXPECT_EQ(test_module.رقم_outputs, MAX_رقم_OUTPUTS);

	لكل (صحيح i = 0; i < test_module.رقم_outputs; ++i) {
		إذا (i == 3) {
			EXPECT_EQ(test_module.outputs[i], MAX_مقدار);

		} وإلا {
			EXPECT_EQ(test_module.outputs[i], DISARMED_مقدار);
		}
	}

	test_module.reset();

	// stop
	test_module.sendActuatorMotorTest((صحيح)OutputFunction::Motor5, 0.f, درست);
	test_module.sendMotors({1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f});
	mixing_output.updateSubscriptions(خطأ);
	EXPECT_EQ(test_module.رقم_updates, update(mixing_output));
	EXPECT_EQ(test_module.رقم_outputs, MAX_رقم_OUTPUTS);

	لكل (صحيح i = 0; i < test_module.رقم_outputs; ++i) {
		EXPECT_EQ(test_module.outputs[i], DISARMED_مقدار);
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
	MixingOutput mixing_output{PARAM_پیشوند, MAX_رقم_OUTPUTS, test_module, MixingOutput::SchedulingPolicy::Disabled, خطأ, خطأ};
	mixing_output.مجموعه_All_Disarmed_مقادیر(DISARMED_مقدار);
	mixing_output.مجموعه_All_Failsafe_مقادیر(FAILSAFE_مقدار);
	mixing_output.مجموعه_All_Min_مقادیر(MIN_مقدار);
	mixing_output.مجموعه_All_Center_مقادیر(CENTER_مقدار);
	mixing_output.مجموعه_All_Max_مقادیر(MAX_مقدار);

	test_module.sendMotors({1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f});
	test_module.sendActuatorArmed(خطأ);

	// ensure all disarmed
	mixing_output.updateSubscriptions(خطأ);
	EXPECT_EQ(test_module.رقم_updates, update(mixing_output));
	EXPECT_EQ(test_module.رقم_outputs, MAX_رقم_OUTPUTS);

	لكل (صحيح i = 0; i < MAX_رقم_OUTPUTS; ++i) {
		EXPECT_EQ(test_module.outputs[i], DISARMED_مقدار);
	}

	test_module.reset();

	// arming
	test_module.sendMotors({0.5f, 1.f, 0.1f, 0.2f, 1.f, 1.f, 1.f, 1.f});
	test_module.sendActuatorArmed(درست);

	EXPECT_EQ(test_module.رقم_updates, update(mixing_output));
	EXPECT_EQ(test_module.رقم_outputs, MAX_رقم_OUTPUTS);

	لكل (صحيح i = 0; i < MAX_رقم_OUTPUTS; ++i) {
		إذا (i == 1) {
			EXPECT_EQ(test_module.outputs[i], (MAX_مقدار - MIN_مقدار) * 0.1f + MIN_مقدار);

		} وإلا إذا (i == 2) {
			EXPECT_EQ(test_module.outputs[i], (MAX_مقدار - MIN_مقدار) * 0.5f + MIN_مقدار);

		} وإلا إذا (i == 3) {
			EXPECT_EQ(test_module.outputs[i], MAX_مقدار);

		} وإلا {
			EXPECT_EQ(test_module.outputs[i], DISARMED_مقدار);
		}
	}

	test_module.reset();

	// update motors
	test_module.sendMotors({0.9f, 1.f, 0.24f, 0.2f, 0.f, 1.f, 1.f, 1.f});
	mixing_output.updateSubscriptions(خطأ);
	mixing_output.update();

	لكل (صحيح i = 0; i < MAX_رقم_OUTPUTS; ++i) {
		إذا (i == 1) {
			EXPECT_EQ(test_module.outputs[i], (MAX_مقدار - MIN_مقدار) * 0.24f + MIN_مقدار);

		} وإلا إذا (i == 2) {
			EXPECT_EQ(test_module.outputs[i], (MAX_مقدار - MIN_مقدار) * 0.9f + MIN_مقدار);

		} وإلا إذا (i == 3) {
			EXPECT_EQ(test_module.outputs[i], MIN_مقدار);

		} وإلا {
			EXPECT_EQ(test_module.outputs[i], DISARMED_مقدار);
		}
	}

	test_module.reset();

	// failsafe
	test_module.sendActuatorArmed(درست, درست);
	test_module.sendMotors({0.5f, 1.f, 0.1f, 0.2f, 1.f, 1.f, 1.f, 1.f});
	mixing_output.update();

	لكل (صحيح i = 0; i < MAX_رقم_OUTPUTS; ++i) {
		EXPECT_EQ(test_module.outputs[i], FAILSAFE_مقدار);
	}

	test_module.reset();

	// restore
	test_module.sendActuatorArmed(درست, خطأ);
	test_module.sendMotors({1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f});
	mixing_output.update();

	لكل (صحيح i = 0; i < MAX_رقم_OUTPUTS; ++i) {
		إذا (i >= 1 && i <= 3) {
			EXPECT_EQ(test_module.outputs[i], MAX_مقدار);

		} وإلا {
			EXPECT_EQ(test_module.outputs[i], DISARMED_مقدار);
		}
	}

	test_module.reset();

	// manual lockdown
	test_module.sendActuatorArmed(درست, خطأ, درست);
	test_module.sendMotors({0.5f, 1.f, 0.1f, 0.2f, 1.f, 1.f, 1.f, 1.f});
	mixing_output.update();

	لكل (صحيح i = 0; i < MAX_رقم_OUTPUTS; ++i) {
		EXPECT_EQ(test_module.outputs[i], DISARMED_مقدار);
	}

	test_module.reset();

	// restore
	test_module.sendActuatorArmed(درست, خطأ);
	test_module.sendMotors({0.f, 0.f, 0.f, 0.f, 0.f, 1.f, 1.f, 1.f});
	mixing_output.update();

	لكل (صحيح i = 0; i < MAX_رقم_OUTPUTS; ++i) {
		إذا (i >= 1 && i <= 3) {
			EXPECT_EQ(test_module.outputs[i], MIN_مقدار);

		} وإلا {
			EXPECT_EQ(test_module.outputs[i], DISARMED_مقدار);
		}
	}

	test_module.reset();

	// set motor 5 reversible: expect output to be in center when commanding to 0
	test_module.sendActuatorArmed(درست, خطأ);
	test_module.sendMotors({0.f, 0.f, 0.f, 0.f, 0.f, 1.f, 1.f, 1.f}, 1u << 4);
	mixing_output.update();
	EXPECT_EQ(mixing_output.reversibleOutputs(), 1u << 3);

	لكل (صحيح i = 0; i < MAX_رقم_OUTPUTS; ++i) {
		إذا (i == 1) {
			EXPECT_EQ(test_module.outputs[i], MIN_مقدار);

		} وإلا إذا (i == 2) {
			EXPECT_EQ(test_module.outputs[i], MIN_مقدار);

		} وإلا إذا (i == 3) {
			EXPECT_EQ(test_module.outputs[i], (MAX_مقدار - MIN_مقدار) * 0.5f + MIN_مقدار);

		} وإلا {
			EXPECT_EQ(test_module.outputs[i], DISARMED_مقدار);
		}
	}

	test_module.reset();

	// disarm
	test_module.sendActuatorArmed(خطأ);
	test_module.sendMotors({0.f, 0.f, 0.f, 0.f, 0.f, 1.f, 1.f, 1.f}, 1u << 4);
	mixing_output.update();

	لكل (صحيح i = 0; i < MAX_رقم_OUTPUTS; ++i) {
		EXPECT_EQ(test_module.outputs[i], DISARMED_مقدار);
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
	MixingOutput mixing_output{PARAM_پیشوند, MAX_رقم_OUTPUTS, test_module, MixingOutput::SchedulingPolicy::Disabled, خطأ, خطأ};
	mixing_output.مجموعه_All_Disarmed_مقادیر(DISARMED_مقدار);
	mixing_output.مجموعه_All_Failsafe_مقادیر(FAILSAFE_مقدار);
	mixing_output.مجموعه_All_Min_مقادیر(MIN_مقدار);
	mixing_output.مجموعه_All_Max_مقادیر(MAX_مقدار);

	test_module.sendMotors({1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f});
	test_module.sendServos({1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f});
	test_module.sendActuatorArmed(خطأ, خطأ, خطأ, درست);

	// ensure all disarmed, except the servo
	mixing_output.updateSubscriptions(خطأ);
	EXPECT_EQ(test_module.رقم_updates, update(mixing_output));
	EXPECT_EQ(test_module.رقم_outputs, MAX_رقم_OUTPUTS);

	لكل (صحيح i = 0; i < MAX_رقم_OUTPUTS; ++i) {
		إذا (i == 1) {
			EXPECT_EQ(test_module.outputs[i], MAX_مقدار);

		} وإلا {
			EXPECT_EQ(test_module.outputs[i], DISARMED_مقدار);
		}
	}

	test_module.reset();

	EXPECT_FALSE(test_module.was_scheduled);
}

صنف TestMixingOutput : عام MixingOutput
{
عام:
	TestMixingOutput(ثابت حرف *param_پیشوند, uint8_t max_رقم_outputs, OutputModuleInterface &interface,
			 SchedulingPolicy scheduling_policy,
			 منطقي support_esc_calibration, منطقي ramp_به_بالا = درست)
		: MixingOutput(param_پیشوند, max_رقم_outputs, interface, scheduling_policy, support_esc_calibration, ramp_به_بالا)
	{};
	عائم output_حد_calc_single(صحيح i, عائم value) ثابت { أرجع MixingOutput::output_حد_calc_single(i, value); }
};

TEST_F(MixerModuleTest, Output_حد_Calc_Single)
{
	OutputModuleTest test_module;
	test_module.configureFunctions({(صحيح)OutputFunction::Motor1});
	TestMixingOutput mixing_output{PARAM_پیشوند, MAX_رقم_OUTPUTS, test_module, MixingOutput::SchedulingPolicy::Disabled, خطأ, خطأ};

	mixing_output.مجموعه_All_Min_مقادیر(MIN_مقدار); // default range [1000,2000]
	mixing_output.مجموعه_All_Max_مقادیر(MAX_مقدار);
	mixing_output.مجموعه_All_Center_مقادیر(CENTER_مقدار); // Set center to middle value
	EXPECT_EQ(mixing_output.output_حد_calc_single(0, -1.f), 1000); // In range
	EXPECT_EQ(mixing_output.output_حد_calc_single(0, -.5f), 1250);
	EXPECT_EQ(mixing_output.output_حد_calc_single(0, 0.f), 1500);
	EXPECT_EQ(mixing_output.output_حد_calc_single(0, .5f), 1750);
	EXPECT_EQ(mixing_output.output_حد_calc_single(0, 1.f), 2000);
	EXPECT_EQ(mixing_output.output_حد_calc_single(0, -1.1f), 1000); // Out of range
	EXPECT_EQ(mixing_output.output_حد_calc_single(0, 1.1f), 2000);
	EXPECT_EQ(mixing_output.output_حد_calc_single(0, -1000.f), 1000); // Way ouf of range
	EXPECT_EQ(mixing_output.output_حد_calc_single(0, 1000.f), 2000);
	EXPECT_EQ(mixing_output.output_حد_calc_single(0, 0.0005), 1500.25f);
	EXPECT_EQ(static_cast<uint16_t>(lroundf(mixing_output.output_حد_calc_single(0, 0.0005))), 1500); // Rounding down
	EXPECT_EQ(mixing_output.output_حد_calc_single(0, 0.0015), 1500.75f);
	EXPECT_EQ(static_cast<uint16_t>(lroundf(mixing_output.output_حد_calc_single(0, 0.0015))), 1501); // Rounding up
	EXPECT_EQ(mixing_output.output_حد_calc_single(0, 0.002), 1501); // Exact value

	mixing_output.مجموعه_All_Min_مقادیر(0); // lower range [0,20]
	mixing_output.مجموعه_All_Max_مقادیر(20);
	mixing_output.مجموعه_All_Center_مقادیر(10); // Set center to middle value
	EXPECT_EQ(mixing_output.output_حد_calc_single(0, -1.f), 0); // In range
	EXPECT_EQ(mixing_output.output_حد_calc_single(0, -.5f), 5);
	EXPECT_EQ(mixing_output.output_حد_calc_single(0, 0.f), 10);
	EXPECT_EQ(mixing_output.output_حد_calc_single(0, .5f), 15);
	EXPECT_EQ(mixing_output.output_حد_calc_single(0, 1.f), 20);
	EXPECT_EQ(mixing_output.output_حد_calc_single(0, -1.1f), 0); // Out of range
	EXPECT_EQ(mixing_output.output_حد_calc_single(0, 1.1f), 20);
	EXPECT_EQ(mixing_output.output_حد_calc_single(0, -1000.f), 0); // Way ouf of range
	EXPECT_EQ(mixing_output.output_حد_calc_single(0, 1000.f), 20);
	EXPECT_EQ(mixing_output.output_حد_calc_single(0, 0.025), 10.25f);
	EXPECT_EQ(static_cast<uint16_t>(lroundf(mixing_output.output_حد_calc_single(0, 0.025))), 10); // Rounding down
	EXPECT_EQ(mixing_output.output_حد_calc_single(0, 0.075), 10.75f);
	EXPECT_EQ(static_cast<uint16_t>(lroundf(mixing_output.output_حد_calc_single(0, 0.075))), 11); // Rounding up
	EXPECT_EQ(mixing_output.output_حد_calc_single(0, 0.1), 11); // Exact value

	mixing_output.مجموعه_All_Min_مقادیر(20); // inverted range [20,0]
	mixing_output.مجموعه_All_Max_مقادیر(0);
	mixing_output.مجموعه_All_Center_مقادیر(10); // Set center to middle value
	EXPECT_EQ(mixing_output.output_حد_calc_single(0, -1.f), 20); // In range
	EXPECT_EQ(mixing_output.output_حد_calc_single(0, -.5f), 15);
	EXPECT_EQ(mixing_output.output_حد_calc_single(0, 0.f), 10);
	EXPECT_EQ(mixing_output.output_حد_calc_single(0, .5f), 5);
	EXPECT_EQ(mixing_output.output_حد_calc_single(0, 1.f), 0);
	EXPECT_EQ(mixing_output.output_حد_calc_single(0, -1.1f), 20); // Out of range
	EXPECT_EQ(mixing_output.output_حد_calc_single(0, 1.1f), 0);
	EXPECT_EQ(mixing_output.output_حد_calc_single(0, -1000.f), 20); // Way ouf of range
	EXPECT_EQ(mixing_output.output_حد_calc_single(0, 1000.f), 0);
	EXPECT_EQ(mixing_output.output_حد_calc_single(0, 0.025), 9.75f);
	EXPECT_EQ(static_cast<uint16_t>(lroundf(mixing_output.output_حد_calc_single(0, 0.025))), 10); // Rounding down
	EXPECT_EQ(mixing_output.output_حد_calc_single(0, 0.075), 9.25f);
	EXPECT_EQ(static_cast<uint16_t>(lroundf(mixing_output.output_حد_calc_single(0, 0.075))), 9); // Rounding up
	EXPECT_EQ(mixing_output.output_حد_calc_single(0, 0.1), 9); // Exact value
}

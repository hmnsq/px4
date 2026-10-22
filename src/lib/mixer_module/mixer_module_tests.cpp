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

#شامل <gtest/gtest.h>
#شامل <array>

#شامل <parameters/param.h>
#شامل <uORB/topics/actuator_motors.h>
#شامل <uORB/topics/actuator_servos.h>
#شامل <uORB/topics/actuator_armed.h>
#شامل <uORB/topics/actuator_test.h>
#شامل <uORB/Publication.hpp>
#شامل <uORB/Subscription.hpp>

#شامل "mixer_module.hpp"

#اگر defined(CONFIG_ARCH_BOARD_PX4_SITL)
#تعریف_کرو PARAM_سابقہ "PWM_MAIN"
#ورنہ
#تعریف_کرو PARAM_سابقہ "HIL_ACT"
#اگر_ختم

ساکن constexpr صحیح MAX_عدد_OUTPUTS = 8;
ساکن constexpr صحیح DISARMED_قدر = 900;
ساکن constexpr صحیح FAILSAFE_قدر = 800;
ساکن constexpr صحیح MIN_قدر = 1000;
ساکن constexpr صحیح CENTER_قدر = 1500;
ساکن constexpr صحیح MAX_قدر = 2000;

جماعت MixerModuleTest : عوامی ::testing::Test
{
عوامی:
	خالی سیٹ_اوپر() override
	{
		param_control_autosave(جھوٹ);
	}

	صحیح update(MixingOutput &mixing_output)
	{
		mixing_output.update();
		// make sure output_limit switches to ON (if outputs enabled)
		px4_usleep(50000 * 2);
		mixing_output.update();
		mixing_output.update();
		واپس 3; // expected number of output updates
	}

};

جماعت OutputModuleTest : عوامی OutputModuleInterface
{
عوامی:
	OutputModuleTest() : OutputModuleInterface(MODULE_NAME, px4::wq_configurations::hp_default) {};

	~OutputModuleTest() override
	{
		// Hide this test's commands from the next test: a zero actuator command is not inert
		_actuator_motors_pub.unadvertise();
		_actuator_servos_pub.unadvertise();
		_actuator_armed_pub.unadvertise();
	}

	خالی تسلسل() override
	{
		was_scheduled = سچ;
	}

	منطقی updateOutputs(اعشاری outputs_[MAX_ACTUATORS], غیرعلامتی عدد_outputs_2, غیرعلامتی عدد_control_groups_updated) override
	{
		حافظہ_نقل(outputs, outputs_, حجم(outputs));
		عدد_outputs = عدد_outputs_2;
		++عدد_updates;
		واپس سچ;
	}

	خالی mixer_بدلا() override
	{
		mixer_بدلا_2 = سچ;
	}

	خالی configureFunctions(ثابت std::array<int32_t, MAX_عدد_OUTPUTS> &functions)
	{
		برائے (صحیح i = 0; i < MAX_عدد_OUTPUTS; ++i) {
			حرف بفر_2[17];

			snprintf(بفر_2, حجم(بفر_2), "%s_FUNC%u", PARAM_سابقہ, i + 1);
			param_سیٹ(param_ڈھونڈو(بفر_2), &functions[i]);
		}

		updateParams();
	}

	خالی sendMotors(ثابت std::array<اعشاری, actuator_motors_s::عدد_CONTROLS> &motors, uint16_t reversible = 0)
	{
		actuator_motors_s actuator_motors{};
		actuator_motors.timestamp = hrt_absolute_وقت();
		actuator_motors.reversible_flags = reversible;

		برائے (غیرعلامتی i = 0; i < motors.سائز(); ++i) {
			actuator_motors.control[i] = motors[i];
		}

		_actuator_motors_pub.publish(actuator_motors);
	}

	خالی sendServos(ثابت std::array<اعشاری, actuator_servos_s::عدد_CONTROLS> &servos)
	{
		actuator_servos_s actuator_servos{};
		actuator_servos.timestamp = hrt_absolute_وقت();

		برائے (غیرعلامتی i = 0; i < servos.سائز(); ++i) {
			actuator_servos.control[i] = servos[i];
		}

		_actuator_servos_pub.publish(actuator_servos);
	}

	خالی sendActuatorMotorTest(صحیح function, اعشاری value, منطقی release_control)
	{
		actuator_test_s actuator_test{};
		actuator_test.timestamp = hrt_absolute_وقت();
		actuator_test.function = function;
		actuator_test.value = value;
		actuator_test.action = release_control ? actuator_test_s::ACTION_RELEASE_CONTROL : actuator_test_s::ACTION_DO_CONTROL;
		actuator_test.timeout_ms = 0;
		_actuator_test_pub.publish(actuator_test);
	}

	خالی sendActuatorArmed(منطقی armed, منطقی termination = جھوٹ, منطقی kill = جھوٹ, منطقی prearm = جھوٹ)
	{
		actuator_armed_s actuator_armed{};
		actuator_armed.timestamp = hrt_absolute_وقت();
		actuator_armed.armed = armed;
		actuator_armed.termination = termination;
		actuator_armed.kill = kill;
		actuator_armed.prearmed = prearm;
		_actuator_armed_pub.publish(actuator_armed);
	}

	خالی reset()
	{
		memset(outputs, 0, حجم(outputs));
		عدد_outputs = 0;
		عدد_updates = 0;
		mixer_بدلا_2 = جھوٹ;
	}

	اعشاری outputs[MAX_ACTUATORS] {};
	صحیح عدد_outputs{0};
	صحیح عدد_updates{0};
	منطقی was_scheduled{جھوٹ};
	منطقی mixer_بدلا_2{جھوٹ};

نجی:
	uORB::Publication<actuator_test_s> _actuator_test_pub{ORB_ID(actuator_test)};
	uORB::Publication<actuator_motors_s> _actuator_motors_pub{ORB_ID(actuator_motors)};
	uORB::Publication<actuator_servos_s> _actuator_servos_pub{ORB_ID(actuator_servos)};
	uORB::Publication<actuator_armed_s> _actuator_armed_pub{ORB_ID(actuator_armed)};
};

TEST_F(MixerModuleTest, basic)
{
	OutputModuleTest test_module;
	test_module.configureFunctions({});
	MixingOutput mixing_output{PARAM_سابقہ, MAX_عدد_OUTPUTS, test_module, MixingOutput::SchedulingPolicy::Disabled, جھوٹ, جھوٹ};
	mixing_output.سیٹ_All_Disarmed_اقدار(DISARMED_قدر);
	mixing_output.سیٹ_All_Failsafe_اقدار(FAILSAFE_قدر);
	mixing_output.سیٹ_All_Min_اقدار(MIN_قدر);
	mixing_output.سیٹ_All_Center_اقدار(CENTER_قدر);
	mixing_output.سیٹ_All_Max_اقدار(MAX_قدر);
	EXPECT_EQ(test_module.عدد_updates, 0);

	// all functions disabled: expect to get one single update to process disabling the output signal
	mixing_output.update();
	mixing_output.updateSubscriptions(جھوٹ);
	mixing_output.update();
	EXPECT_EQ(test_module.عدد_updates, 1);
	mixing_output.update();
	mixing_output.updateSubscriptions(جھوٹ);
	mixing_output.update();
	EXPECT_EQ(test_module.عدد_updates, 1);
	test_module.reset();

	// configure motor, ensure all still disarmed
	test_module.configureFunctions({(صحیح)OutputFunction::Motor1});
	mixing_output.updateSubscriptions(جھوٹ);
	EXPECT_TRUE(test_module.mixer_بدلا_2);
	EXPECT_EQ(test_module.عدد_updates, update(mixing_output));
	EXPECT_EQ(test_module.عدد_outputs, MAX_عدد_OUTPUTS);

	برائے (صحیح i = 0; i < test_module.عدد_outputs; ++i) {
		EXPECT_EQ(test_module.outputs[i], DISARMED_قدر);
	}

	test_module.reset();

	// send motors -> still disarmed
	test_module.sendMotors({1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f});
	test_module.configureFunctions({
		0,
		(صحیح)OutputFunction::Motor3,
		(صحیح)OutputFunction::Motor1,
		(صحیح)OutputFunction::Motor5});

	mixing_output.updateSubscriptions(جھوٹ);
	EXPECT_EQ(test_module.عدد_updates, update(mixing_output));
	EXPECT_EQ(test_module.عدد_outputs, MAX_عدد_OUTPUTS);

	برائے (صحیح i = 0; i < test_module.عدد_outputs; ++i) {
		EXPECT_EQ(test_module.outputs[i], DISARMED_قدر);
	}

	test_module.reset();


	// actuator test
	test_module.sendActuatorMotorTest((صحیح)OutputFunction::Motor5, 1.f, جھوٹ);
	test_module.sendMotors({1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f});
	mixing_output.updateSubscriptions(جھوٹ);
	EXPECT_EQ(test_module.عدد_updates, update(mixing_output));
	EXPECT_EQ(test_module.عدد_outputs, MAX_عدد_OUTPUTS);

	برائے (صحیح i = 0; i < test_module.عدد_outputs; ++i) {
		اگر (i == 3) {
			EXPECT_EQ(test_module.outputs[i], MAX_قدر);

		} ورنہ {
			EXPECT_EQ(test_module.outputs[i], DISARMED_قدر);
		}
	}

	test_module.reset();

	// stop
	test_module.sendActuatorMotorTest((صحیح)OutputFunction::Motor5, 0.f, سچ);
	test_module.sendMotors({1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f});
	mixing_output.updateSubscriptions(جھوٹ);
	EXPECT_EQ(test_module.عدد_updates, update(mixing_output));
	EXPECT_EQ(test_module.عدد_outputs, MAX_عدد_OUTPUTS);

	برائے (صحیح i = 0; i < test_module.عدد_outputs; ++i) {
		EXPECT_EQ(test_module.outputs[i], DISARMED_قدر);
	}

	test_module.reset();

	EXPECT_FALSE(test_module.was_scheduled);
}

TEST_F(MixerModuleTest, arming)
{
	OutputModuleTest test_module;
	test_module.configureFunctions({
		0,
		(صحیح)OutputFunction::Motor3,
		(صحیح)OutputFunction::Motor1,
		(صحیح)OutputFunction::Motor5,
		(صحیح)OutputFunction::Servo3});
	MixingOutput mixing_output{PARAM_سابقہ, MAX_عدد_OUTPUTS, test_module, MixingOutput::SchedulingPolicy::Disabled, جھوٹ, جھوٹ};
	mixing_output.سیٹ_All_Disarmed_اقدار(DISARMED_قدر);
	mixing_output.سیٹ_All_Failsafe_اقدار(FAILSAFE_قدر);
	mixing_output.سیٹ_All_Min_اقدار(MIN_قدر);
	mixing_output.سیٹ_All_Center_اقدار(CENTER_قدر);
	mixing_output.سیٹ_All_Max_اقدار(MAX_قدر);

	test_module.sendMotors({1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f});
	test_module.sendActuatorArmed(جھوٹ);

	// ensure all disarmed
	mixing_output.updateSubscriptions(جھوٹ);
	EXPECT_EQ(test_module.عدد_updates, update(mixing_output));
	EXPECT_EQ(test_module.عدد_outputs, MAX_عدد_OUTPUTS);

	برائے (صحیح i = 0; i < MAX_عدد_OUTPUTS; ++i) {
		EXPECT_EQ(test_module.outputs[i], DISARMED_قدر);
	}

	test_module.reset();

	// arming
	test_module.sendMotors({0.5f, 1.f, 0.1f, 0.2f, 1.f, 1.f, 1.f, 1.f});
	test_module.sendActuatorArmed(سچ);

	EXPECT_EQ(test_module.عدد_updates, update(mixing_output));
	EXPECT_EQ(test_module.عدد_outputs, MAX_عدد_OUTPUTS);

	برائے (صحیح i = 0; i < MAX_عدد_OUTPUTS; ++i) {
		اگر (i == 1) {
			EXPECT_EQ(test_module.outputs[i], (MAX_قدر - MIN_قدر) * 0.1f + MIN_قدر);

		} ورنہ اگر (i == 2) {
			EXPECT_EQ(test_module.outputs[i], (MAX_قدر - MIN_قدر) * 0.5f + MIN_قدر);

		} ورنہ اگر (i == 3) {
			EXPECT_EQ(test_module.outputs[i], MAX_قدر);

		} ورنہ {
			EXPECT_EQ(test_module.outputs[i], DISARMED_قدر);
		}
	}

	test_module.reset();

	// update motors
	test_module.sendMotors({0.9f, 1.f, 0.24f, 0.2f, 0.f, 1.f, 1.f, 1.f});
	mixing_output.updateSubscriptions(جھوٹ);
	mixing_output.update();

	برائے (صحیح i = 0; i < MAX_عدد_OUTPUTS; ++i) {
		اگر (i == 1) {
			EXPECT_EQ(test_module.outputs[i], (MAX_قدر - MIN_قدر) * 0.24f + MIN_قدر);

		} ورنہ اگر (i == 2) {
			EXPECT_EQ(test_module.outputs[i], (MAX_قدر - MIN_قدر) * 0.9f + MIN_قدر);

		} ورنہ اگر (i == 3) {
			EXPECT_EQ(test_module.outputs[i], MIN_قدر);

		} ورنہ {
			EXPECT_EQ(test_module.outputs[i], DISARMED_قدر);
		}
	}

	test_module.reset();

	// failsafe
	test_module.sendActuatorArmed(سچ, سچ);
	test_module.sendMotors({0.5f, 1.f, 0.1f, 0.2f, 1.f, 1.f, 1.f, 1.f});
	mixing_output.update();

	برائے (صحیح i = 0; i < MAX_عدد_OUTPUTS; ++i) {
		EXPECT_EQ(test_module.outputs[i], FAILSAFE_قدر);
	}

	test_module.reset();

	// restore
	test_module.sendActuatorArmed(سچ, جھوٹ);
	test_module.sendMotors({1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f});
	mixing_output.update();

	برائے (صحیح i = 0; i < MAX_عدد_OUTPUTS; ++i) {
		اگر (i >= 1 && i <= 3) {
			EXPECT_EQ(test_module.outputs[i], MAX_قدر);

		} ورنہ {
			EXPECT_EQ(test_module.outputs[i], DISARMED_قدر);
		}
	}

	test_module.reset();

	// manual lockdown
	test_module.sendActuatorArmed(سچ, جھوٹ, سچ);
	test_module.sendMotors({0.5f, 1.f, 0.1f, 0.2f, 1.f, 1.f, 1.f, 1.f});
	mixing_output.update();

	برائے (صحیح i = 0; i < MAX_عدد_OUTPUTS; ++i) {
		EXPECT_EQ(test_module.outputs[i], DISARMED_قدر);
	}

	test_module.reset();

	// restore
	test_module.sendActuatorArmed(سچ, جھوٹ);
	test_module.sendMotors({0.f, 0.f, 0.f, 0.f, 0.f, 1.f, 1.f, 1.f});
	mixing_output.update();

	برائے (صحیح i = 0; i < MAX_عدد_OUTPUTS; ++i) {
		اگر (i >= 1 && i <= 3) {
			EXPECT_EQ(test_module.outputs[i], MIN_قدر);

		} ورنہ {
			EXPECT_EQ(test_module.outputs[i], DISARMED_قدر);
		}
	}

	test_module.reset();

	// set motor 5 reversible: expect output to be in center when commanding to 0
	test_module.sendActuatorArmed(سچ, جھوٹ);
	test_module.sendMotors({0.f, 0.f, 0.f, 0.f, 0.f, 1.f, 1.f, 1.f}, 1u << 4);
	mixing_output.update();
	EXPECT_EQ(mixing_output.reversibleOutputs(), 1u << 3);

	برائے (صحیح i = 0; i < MAX_عدد_OUTPUTS; ++i) {
		اگر (i == 1) {
			EXPECT_EQ(test_module.outputs[i], MIN_قدر);

		} ورنہ اگر (i == 2) {
			EXPECT_EQ(test_module.outputs[i], MIN_قدر);

		} ورنہ اگر (i == 3) {
			EXPECT_EQ(test_module.outputs[i], (MAX_قدر - MIN_قدر) * 0.5f + MIN_قدر);

		} ورنہ {
			EXPECT_EQ(test_module.outputs[i], DISARMED_قدر);
		}
	}

	test_module.reset();

	// disarm
	test_module.sendActuatorArmed(جھوٹ);
	test_module.sendMotors({0.f, 0.f, 0.f, 0.f, 0.f, 1.f, 1.f, 1.f}, 1u << 4);
	mixing_output.update();

	برائے (صحیح i = 0; i < MAX_عدد_OUTPUTS; ++i) {
		EXPECT_EQ(test_module.outputs[i], DISARMED_قدر);
	}

	test_module.reset();

	EXPECT_FALSE(test_module.was_scheduled);
}

TEST_F(MixerModuleTest, prearm)
{
	OutputModuleTest test_module;
	test_module.configureFunctions({
		(صحیح)OutputFunction::Motor1,
		(صحیح)OutputFunction::Servo1});
	MixingOutput mixing_output{PARAM_سابقہ, MAX_عدد_OUTPUTS, test_module, MixingOutput::SchedulingPolicy::Disabled, جھوٹ, جھوٹ};
	mixing_output.سیٹ_All_Disarmed_اقدار(DISARMED_قدر);
	mixing_output.سیٹ_All_Failsafe_اقدار(FAILSAFE_قدر);
	mixing_output.سیٹ_All_Min_اقدار(MIN_قدر);
	mixing_output.سیٹ_All_Max_اقدار(MAX_قدر);

	test_module.sendMotors({1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f});
	test_module.sendServos({1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f});
	test_module.sendActuatorArmed(جھوٹ, جھوٹ, جھوٹ, سچ);

	// ensure all disarmed, except the servo
	mixing_output.updateSubscriptions(جھوٹ);
	EXPECT_EQ(test_module.عدد_updates, update(mixing_output));
	EXPECT_EQ(test_module.عدد_outputs, MAX_عدد_OUTPUTS);

	برائے (صحیح i = 0; i < MAX_عدد_OUTPUTS; ++i) {
		اگر (i == 1) {
			EXPECT_EQ(test_module.outputs[i], MAX_قدر);

		} ورنہ {
			EXPECT_EQ(test_module.outputs[i], DISARMED_قدر);
		}
	}

	test_module.reset();

	EXPECT_FALSE(test_module.was_scheduled);
}

جماعت TestMixingOutput : عوامی MixingOutput
{
عوامی:
	TestMixingOutput(ثابت حرف *param_سابقہ, uint8_t max_عدد_outputs, OutputModuleInterface &interface,
			 SchedulingPolicy scheduling_policy,
			 منطقی support_esc_calibration, منطقی ramp_اوپر = سچ)
		: MixingOutput(param_سابقہ, max_عدد_outputs, interface, scheduling_policy, support_esc_calibration, ramp_اوپر)
	{};
	اعشاری output_حد_بندی_calc_single(صحیح i, اعشاری value) ثابت { واپس MixingOutput::output_حد_بندی_calc_single(i, value); }
};

TEST_F(MixerModuleTest, Output_حد_بندی_Calc_Single)
{
	OutputModuleTest test_module;
	test_module.configureFunctions({(صحیح)OutputFunction::Motor1});
	TestMixingOutput mixing_output{PARAM_سابقہ, MAX_عدد_OUTPUTS, test_module, MixingOutput::SchedulingPolicy::Disabled, جھوٹ, جھوٹ};

	mixing_output.سیٹ_All_Min_اقدار(MIN_قدر); // default range [1000,2000]
	mixing_output.سیٹ_All_Max_اقدار(MAX_قدر);
	mixing_output.سیٹ_All_Center_اقدار(CENTER_قدر); // Set center to middle value
	EXPECT_EQ(mixing_output.output_حد_بندی_calc_single(0, -1.f), 1000); // In range
	EXPECT_EQ(mixing_output.output_حد_بندی_calc_single(0, -.5f), 1250);
	EXPECT_EQ(mixing_output.output_حد_بندی_calc_single(0, 0.f), 1500);
	EXPECT_EQ(mixing_output.output_حد_بندی_calc_single(0, .5f), 1750);
	EXPECT_EQ(mixing_output.output_حد_بندی_calc_single(0, 1.f), 2000);
	EXPECT_EQ(mixing_output.output_حد_بندی_calc_single(0, -1.1f), 1000); // Out of range
	EXPECT_EQ(mixing_output.output_حد_بندی_calc_single(0, 1.1f), 2000);
	EXPECT_EQ(mixing_output.output_حد_بندی_calc_single(0, -1000.f), 1000); // Way ouf of range
	EXPECT_EQ(mixing_output.output_حد_بندی_calc_single(0, 1000.f), 2000);
	EXPECT_EQ(mixing_output.output_حد_بندی_calc_single(0, 0.0005), 1500.25f);
	EXPECT_EQ(static_cast<uint16_t>(lroundf(mixing_output.output_حد_بندی_calc_single(0, 0.0005))), 1500); // Rounding down
	EXPECT_EQ(mixing_output.output_حد_بندی_calc_single(0, 0.0015), 1500.75f);
	EXPECT_EQ(static_cast<uint16_t>(lroundf(mixing_output.output_حد_بندی_calc_single(0, 0.0015))), 1501); // Rounding up
	EXPECT_EQ(mixing_output.output_حد_بندی_calc_single(0, 0.002), 1501); // Exact value

	mixing_output.سیٹ_All_Min_اقدار(0); // lower range [0,20]
	mixing_output.سیٹ_All_Max_اقدار(20);
	mixing_output.سیٹ_All_Center_اقدار(10); // Set center to middle value
	EXPECT_EQ(mixing_output.output_حد_بندی_calc_single(0, -1.f), 0); // In range
	EXPECT_EQ(mixing_output.output_حد_بندی_calc_single(0, -.5f), 5);
	EXPECT_EQ(mixing_output.output_حد_بندی_calc_single(0, 0.f), 10);
	EXPECT_EQ(mixing_output.output_حد_بندی_calc_single(0, .5f), 15);
	EXPECT_EQ(mixing_output.output_حد_بندی_calc_single(0, 1.f), 20);
	EXPECT_EQ(mixing_output.output_حد_بندی_calc_single(0, -1.1f), 0); // Out of range
	EXPECT_EQ(mixing_output.output_حد_بندی_calc_single(0, 1.1f), 20);
	EXPECT_EQ(mixing_output.output_حد_بندی_calc_single(0, -1000.f), 0); // Way ouf of range
	EXPECT_EQ(mixing_output.output_حد_بندی_calc_single(0, 1000.f), 20);
	EXPECT_EQ(mixing_output.output_حد_بندی_calc_single(0, 0.025), 10.25f);
	EXPECT_EQ(static_cast<uint16_t>(lroundf(mixing_output.output_حد_بندی_calc_single(0, 0.025))), 10); // Rounding down
	EXPECT_EQ(mixing_output.output_حد_بندی_calc_single(0, 0.075), 10.75f);
	EXPECT_EQ(static_cast<uint16_t>(lroundf(mixing_output.output_حد_بندی_calc_single(0, 0.075))), 11); // Rounding up
	EXPECT_EQ(mixing_output.output_حد_بندی_calc_single(0, 0.1), 11); // Exact value

	mixing_output.سیٹ_All_Min_اقدار(20); // inverted range [20,0]
	mixing_output.سیٹ_All_Max_اقدار(0);
	mixing_output.سیٹ_All_Center_اقدار(10); // Set center to middle value
	EXPECT_EQ(mixing_output.output_حد_بندی_calc_single(0, -1.f), 20); // In range
	EXPECT_EQ(mixing_output.output_حد_بندی_calc_single(0, -.5f), 15);
	EXPECT_EQ(mixing_output.output_حد_بندی_calc_single(0, 0.f), 10);
	EXPECT_EQ(mixing_output.output_حد_بندی_calc_single(0, .5f), 5);
	EXPECT_EQ(mixing_output.output_حد_بندی_calc_single(0, 1.f), 0);
	EXPECT_EQ(mixing_output.output_حد_بندی_calc_single(0, -1.1f), 20); // Out of range
	EXPECT_EQ(mixing_output.output_حد_بندی_calc_single(0, 1.1f), 0);
	EXPECT_EQ(mixing_output.output_حد_بندی_calc_single(0, -1000.f), 20); // Way ouf of range
	EXPECT_EQ(mixing_output.output_حد_بندی_calc_single(0, 1000.f), 0);
	EXPECT_EQ(mixing_output.output_حد_بندی_calc_single(0, 0.025), 9.75f);
	EXPECT_EQ(static_cast<uint16_t>(lroundf(mixing_output.output_حد_بندی_calc_single(0, 0.025))), 10); // Rounding down
	EXPECT_EQ(mixing_output.output_حد_بندی_calc_single(0, 0.075), 9.25f);
	EXPECT_EQ(static_cast<uint16_t>(lroundf(mixing_output.output_حد_بندی_calc_single(0, 0.075))), 9); // Rounding up
	EXPECT_EQ(mixing_output.output_حد_بندی_calc_single(0, 0.1), 9); // Exact value
}

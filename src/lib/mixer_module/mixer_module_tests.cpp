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

#அந்தர்பாவ <gtest/gtest.h>
#அந்தர்பாவ <array>

#அந்தர்பாவ <parameters/param.h>
#அந்தர்பாவ <uORB/topics/actuator_motors.h>
#அந்தர்பாவ <uORB/topics/actuator_servos.h>
#அந்தர்பாவ <uORB/topics/actuator_armed.h>
#அந்தர்பாவ <uORB/topics/actuator_test.h>
#அந்தர்பாவ <uORB/Publication.hpp>
#அந்தர்பாவ <uORB/Subscription.hpp>

#அந்தர்பாவ "mixer_module.hpp"

#எனில் defined(CONFIG_ARCH_BOARD_PX4_SITL)
#define PARAM_PREFIX "PWM_MAIN"
#இல்லையெனில்
#define PARAM_PREFIX "HIL_ACT"
#endif

நிலையான constexpr முழு MAX_இலக்கம்_OUTPUTS = 8;
நிலையான constexpr முழு DISARMED_மதிப்பு = 900;
நிலையான constexpr முழு FAILSAFE_மதிப்பு = 800;
நிலையான constexpr முழு MIN_மதிப்பு = 1000;
நிலையான constexpr முழு CENTER_மதிப்பு = 1500;
நிலையான constexpr முழு MAX_மதிப்பு = 2000;

வகுப்பு MixerModuleTest : பொது ::testing::Test
{
பொது:
	வெறுமை SetUp() override
	{
		param_control_autosave(பொய்);
	}

	முழு update(MixingOutput &mixing_output)
	{
		mixing_output.update();
		// make sure output_limit switches to ON (if outputs enabled)
		px4_usleep(50000 * 2);
		mixing_output.update();
		mixing_output.update();
		திரும்பு 3; // expected number of output updates
	}

};

வகுப்பு OutputModuleTest : பொது OutputModuleInterface
{
பொது:
	OutputModuleTest() : OutputModuleInterface(MODULE_NAME, px4::wq_configurations::hp_default) {};

	~OutputModuleTest() override
	{
		// Hide this test's commands from the next test: a zero actuator command is not inert
		_actuator_motors_pub.unadvertise();
		_actuator_servos_pub.unadvertise();
		_actuator_armed_pub.unadvertise();
	}

	வெறுமை Run() override
	{
		was_scheduled = உண்மை;
	}

	தருக்க updateOutputs(தசம outputs_[MAX_ACTUATORS], unsigned இலக்கம்_outputs_2, unsigned இலக்கம்_control_groups_updated) override
	{
		memcpy(outputs, outputs_, அளவு(outputs));
		இலக்கம்_outputs = இலக்கம்_outputs_2;
		++இலக்கம்_updates;
		திரும்பு உண்மை;
	}

	வெறுமை mixerChanged() override
	{
		mixer_changed = உண்மை;
	}

	வெறுமை configureFunctions(மாறிலி std::array<int32_t, MAX_இலக்கம்_OUTPUTS> &functions)
	{
		க்கு (முழு i = 0; i < MAX_இலக்கம்_OUTPUTS; ++i) {
			எழுத்து buffer[17];

			snprintf(buffer, அளவு(buffer), "%s_FUNC%u", PARAM_PREFIX, i + 1);
			param_set(param_find(buffer), &functions[i]);
		}

		updateParams();
	}

	வெறுமை sendMotors(மாறிலி std::array<தசம, actuator_motors_s::இலக்கம்_CONTROLS> &motors, uint16_t reversible = 0)
	{
		actuator_motors_s actuator_motors{};
		actuator_motors.timestamp = hrt_absolute_time();
		actuator_motors.reversible_flags = reversible;

		க்கு (unsigned i = 0; i < motors.size(); ++i) {
			actuator_motors.control[i] = motors[i];
		}

		_actuator_motors_pub.publish(actuator_motors);
	}

	வெறுமை sendServos(மாறிலி std::array<தசம, actuator_servos_s::இலக்கம்_CONTROLS> &servos)
	{
		actuator_servos_s actuator_servos{};
		actuator_servos.timestamp = hrt_absolute_time();

		க்கு (unsigned i = 0; i < servos.size(); ++i) {
			actuator_servos.control[i] = servos[i];
		}

		_actuator_servos_pub.publish(actuator_servos);
	}

	வெறுமை sendActuatorMotorTest(முழு function, தசம value, தருக்க release_control)
	{
		actuator_test_s actuator_test{};
		actuator_test.timestamp = hrt_absolute_time();
		actuator_test.function = function;
		actuator_test.value = value;
		actuator_test.action = release_control ? actuator_test_s::ACTION_RELEASE_CONTROL : actuator_test_s::ACTION_DO_CONTROL;
		actuator_test.timeout_ms = 0;
		_actuator_test_pub.publish(actuator_test);
	}

	வெறுமை sendActuatorArmed(தருக்க armed, தருக்க termination = பொய், தருக்க kill = பொய், தருக்க prearm = பொய்)
	{
		actuator_armed_s actuator_armed{};
		actuator_armed.timestamp = hrt_absolute_time();
		actuator_armed.armed = armed;
		actuator_armed.termination = termination;
		actuator_armed.kill = kill;
		actuator_armed.prearmed = prearm;
		_actuator_armed_pub.publish(actuator_armed);
	}

	வெறுமை reset()
	{
		memset(outputs, 0, அளவு(outputs));
		இலக்கம்_outputs = 0;
		இலக்கம்_updates = 0;
		mixer_changed = பொய்;
	}

	தசம outputs[MAX_ACTUATORS] {};
	முழு இலக்கம்_outputs{0};
	முழு இலக்கம்_updates{0};
	தருக்க was_scheduled{பொய்};
	தருக்க mixer_changed{பொய்};

தனிப்பட்ட:
	uORB::Publication<actuator_test_s> _actuator_test_pub{ORB_ID(actuator_test)};
	uORB::Publication<actuator_motors_s> _actuator_motors_pub{ORB_ID(actuator_motors)};
	uORB::Publication<actuator_servos_s> _actuator_servos_pub{ORB_ID(actuator_servos)};
	uORB::Publication<actuator_armed_s> _actuator_armed_pub{ORB_ID(actuator_armed)};
};

TEST_F(MixerModuleTest, basic)
{
	OutputModuleTest test_module;
	test_module.configureFunctions({});
	MixingOutput mixing_output{PARAM_PREFIX, MAX_இலக்கம்_OUTPUTS, test_module, MixingOutput::SchedulingPolicy::Disabled, பொய், பொய்};
	mixing_output.set_All_Disarmed_மதிப்புகள்(DISARMED_மதிப்பு);
	mixing_output.set_All_Failsafe_மதிப்புகள்(FAILSAFE_மதிப்பு);
	mixing_output.set_All_Min_மதிப்புகள்(MIN_மதிப்பு);
	mixing_output.set_All_Center_மதிப்புகள்(CENTER_மதிப்பு);
	mixing_output.set_All_Max_மதிப்புகள்(MAX_மதிப்பு);
	EXPECT_EQ(test_module.இலக்கம்_updates, 0);

	// all functions disabled: expect to get one single update to process disabling the output signal
	mixing_output.update();
	mixing_output.updateSubscriptions(பொய்);
	mixing_output.update();
	EXPECT_EQ(test_module.இலக்கம்_updates, 1);
	mixing_output.update();
	mixing_output.updateSubscriptions(பொய்);
	mixing_output.update();
	EXPECT_EQ(test_module.இலக்கம்_updates, 1);
	test_module.reset();

	// configure motor, ensure all still disarmed
	test_module.configureFunctions({(முழு)OutputFunction::Motor1});
	mixing_output.updateSubscriptions(பொய்);
	EXPECT_TRUE(test_module.mixer_changed);
	EXPECT_EQ(test_module.இலக்கம்_updates, update(mixing_output));
	EXPECT_EQ(test_module.இலக்கம்_outputs, MAX_இலக்கம்_OUTPUTS);

	க்கு (முழு i = 0; i < test_module.இலக்கம்_outputs; ++i) {
		EXPECT_EQ(test_module.outputs[i], DISARMED_மதிப்பு);
	}

	test_module.reset();

	// send motors -> still disarmed
	test_module.sendMotors({1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f});
	test_module.configureFunctions({
		0,
		(முழு)OutputFunction::Motor3,
		(முழு)OutputFunction::Motor1,
		(முழு)OutputFunction::Motor5});

	mixing_output.updateSubscriptions(பொய்);
	EXPECT_EQ(test_module.இலக்கம்_updates, update(mixing_output));
	EXPECT_EQ(test_module.இலக்கம்_outputs, MAX_இலக்கம்_OUTPUTS);

	க்கு (முழு i = 0; i < test_module.இலக்கம்_outputs; ++i) {
		EXPECT_EQ(test_module.outputs[i], DISARMED_மதிப்பு);
	}

	test_module.reset();


	// actuator test
	test_module.sendActuatorMotorTest((முழு)OutputFunction::Motor5, 1.f, பொய்);
	test_module.sendMotors({1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f});
	mixing_output.updateSubscriptions(பொய்);
	EXPECT_EQ(test_module.இலக்கம்_updates, update(mixing_output));
	EXPECT_EQ(test_module.இலக்கம்_outputs, MAX_இலக்கம்_OUTPUTS);

	க்கு (முழு i = 0; i < test_module.இலக்கம்_outputs; ++i) {
		எனில் (i == 3) {
			EXPECT_EQ(test_module.outputs[i], MAX_மதிப்பு);

		} இல்லையெனில் {
			EXPECT_EQ(test_module.outputs[i], DISARMED_மதிப்பு);
		}
	}

	test_module.reset();

	// stop
	test_module.sendActuatorMotorTest((முழு)OutputFunction::Motor5, 0.f, உண்மை);
	test_module.sendMotors({1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f});
	mixing_output.updateSubscriptions(பொய்);
	EXPECT_EQ(test_module.இலக்கம்_updates, update(mixing_output));
	EXPECT_EQ(test_module.இலக்கம்_outputs, MAX_இலக்கம்_OUTPUTS);

	க்கு (முழு i = 0; i < test_module.இலக்கம்_outputs; ++i) {
		EXPECT_EQ(test_module.outputs[i], DISARMED_மதிப்பு);
	}

	test_module.reset();

	EXPECT_FALSE(test_module.was_scheduled);
}

TEST_F(MixerModuleTest, arming)
{
	OutputModuleTest test_module;
	test_module.configureFunctions({
		0,
		(முழு)OutputFunction::Motor3,
		(முழு)OutputFunction::Motor1,
		(முழு)OutputFunction::Motor5,
		(முழு)OutputFunction::Servo3});
	MixingOutput mixing_output{PARAM_PREFIX, MAX_இலக்கம்_OUTPUTS, test_module, MixingOutput::SchedulingPolicy::Disabled, பொய், பொய்};
	mixing_output.set_All_Disarmed_மதிப்புகள்(DISARMED_மதிப்பு);
	mixing_output.set_All_Failsafe_மதிப்புகள்(FAILSAFE_மதிப்பு);
	mixing_output.set_All_Min_மதிப்புகள்(MIN_மதிப்பு);
	mixing_output.set_All_Center_மதிப்புகள்(CENTER_மதிப்பு);
	mixing_output.set_All_Max_மதிப்புகள்(MAX_மதிப்பு);

	test_module.sendMotors({1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f});
	test_module.sendActuatorArmed(பொய்);

	// ensure all disarmed
	mixing_output.updateSubscriptions(பொய்);
	EXPECT_EQ(test_module.இலக்கம்_updates, update(mixing_output));
	EXPECT_EQ(test_module.இலக்கம்_outputs, MAX_இலக்கம்_OUTPUTS);

	க்கு (முழு i = 0; i < MAX_இலக்கம்_OUTPUTS; ++i) {
		EXPECT_EQ(test_module.outputs[i], DISARMED_மதிப்பு);
	}

	test_module.reset();

	// arming
	test_module.sendMotors({0.5f, 1.f, 0.1f, 0.2f, 1.f, 1.f, 1.f, 1.f});
	test_module.sendActuatorArmed(உண்மை);

	EXPECT_EQ(test_module.இலக்கம்_updates, update(mixing_output));
	EXPECT_EQ(test_module.இலக்கம்_outputs, MAX_இலக்கம்_OUTPUTS);

	க்கு (முழு i = 0; i < MAX_இலக்கம்_OUTPUTS; ++i) {
		எனில் (i == 1) {
			EXPECT_EQ(test_module.outputs[i], (MAX_மதிப்பு - MIN_மதிப்பு) * 0.1f + MIN_மதிப்பு);

		} இல்லையெனில் எனில் (i == 2) {
			EXPECT_EQ(test_module.outputs[i], (MAX_மதிப்பு - MIN_மதிப்பு) * 0.5f + MIN_மதிப்பு);

		} இல்லையெனில் எனில் (i == 3) {
			EXPECT_EQ(test_module.outputs[i], MAX_மதிப்பு);

		} இல்லையெனில் {
			EXPECT_EQ(test_module.outputs[i], DISARMED_மதிப்பு);
		}
	}

	test_module.reset();

	// update motors
	test_module.sendMotors({0.9f, 1.f, 0.24f, 0.2f, 0.f, 1.f, 1.f, 1.f});
	mixing_output.updateSubscriptions(பொய்);
	mixing_output.update();

	க்கு (முழு i = 0; i < MAX_இலக்கம்_OUTPUTS; ++i) {
		எனில் (i == 1) {
			EXPECT_EQ(test_module.outputs[i], (MAX_மதிப்பு - MIN_மதிப்பு) * 0.24f + MIN_மதிப்பு);

		} இல்லையெனில் எனில் (i == 2) {
			EXPECT_EQ(test_module.outputs[i], (MAX_மதிப்பு - MIN_மதிப்பு) * 0.9f + MIN_மதிப்பு);

		} இல்லையெனில் எனில் (i == 3) {
			EXPECT_EQ(test_module.outputs[i], MIN_மதிப்பு);

		} இல்லையெனில் {
			EXPECT_EQ(test_module.outputs[i], DISARMED_மதிப்பு);
		}
	}

	test_module.reset();

	// failsafe
	test_module.sendActuatorArmed(உண்மை, உண்மை);
	test_module.sendMotors({0.5f, 1.f, 0.1f, 0.2f, 1.f, 1.f, 1.f, 1.f});
	mixing_output.update();

	க்கு (முழு i = 0; i < MAX_இலக்கம்_OUTPUTS; ++i) {
		EXPECT_EQ(test_module.outputs[i], FAILSAFE_மதிப்பு);
	}

	test_module.reset();

	// restore
	test_module.sendActuatorArmed(உண்மை, பொய்);
	test_module.sendMotors({1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f});
	mixing_output.update();

	க்கு (முழு i = 0; i < MAX_இலக்கம்_OUTPUTS; ++i) {
		எனில் (i >= 1 && i <= 3) {
			EXPECT_EQ(test_module.outputs[i], MAX_மதிப்பு);

		} இல்லையெனில் {
			EXPECT_EQ(test_module.outputs[i], DISARMED_மதிப்பு);
		}
	}

	test_module.reset();

	// manual lockdown
	test_module.sendActuatorArmed(உண்மை, பொய், உண்மை);
	test_module.sendMotors({0.5f, 1.f, 0.1f, 0.2f, 1.f, 1.f, 1.f, 1.f});
	mixing_output.update();

	க்கு (முழு i = 0; i < MAX_இலக்கம்_OUTPUTS; ++i) {
		EXPECT_EQ(test_module.outputs[i], DISARMED_மதிப்பு);
	}

	test_module.reset();

	// restore
	test_module.sendActuatorArmed(உண்மை, பொய்);
	test_module.sendMotors({0.f, 0.f, 0.f, 0.f, 0.f, 1.f, 1.f, 1.f});
	mixing_output.update();

	க்கு (முழு i = 0; i < MAX_இலக்கம்_OUTPUTS; ++i) {
		எனில் (i >= 1 && i <= 3) {
			EXPECT_EQ(test_module.outputs[i], MIN_மதிப்பு);

		} இல்லையெனில் {
			EXPECT_EQ(test_module.outputs[i], DISARMED_மதிப்பு);
		}
	}

	test_module.reset();

	// set motor 5 reversible: expect output to be in center when commanding to 0
	test_module.sendActuatorArmed(உண்மை, பொய்);
	test_module.sendMotors({0.f, 0.f, 0.f, 0.f, 0.f, 1.f, 1.f, 1.f}, 1u << 4);
	mixing_output.update();
	EXPECT_EQ(mixing_output.reversibleOutputs(), 1u << 3);

	க்கு (முழு i = 0; i < MAX_இலக்கம்_OUTPUTS; ++i) {
		எனில் (i == 1) {
			EXPECT_EQ(test_module.outputs[i], MIN_மதிப்பு);

		} இல்லையெனில் எனில் (i == 2) {
			EXPECT_EQ(test_module.outputs[i], MIN_மதிப்பு);

		} இல்லையெனில் எனில் (i == 3) {
			EXPECT_EQ(test_module.outputs[i], (MAX_மதிப்பு - MIN_மதிப்பு) * 0.5f + MIN_மதிப்பு);

		} இல்லையெனில் {
			EXPECT_EQ(test_module.outputs[i], DISARMED_மதிப்பு);
		}
	}

	test_module.reset();

	// disarm
	test_module.sendActuatorArmed(பொய்);
	test_module.sendMotors({0.f, 0.f, 0.f, 0.f, 0.f, 1.f, 1.f, 1.f}, 1u << 4);
	mixing_output.update();

	க்கு (முழு i = 0; i < MAX_இலக்கம்_OUTPUTS; ++i) {
		EXPECT_EQ(test_module.outputs[i], DISARMED_மதிப்பு);
	}

	test_module.reset();

	EXPECT_FALSE(test_module.was_scheduled);
}

TEST_F(MixerModuleTest, prearm)
{
	OutputModuleTest test_module;
	test_module.configureFunctions({
		(முழு)OutputFunction::Motor1,
		(முழு)OutputFunction::Servo1});
	MixingOutput mixing_output{PARAM_PREFIX, MAX_இலக்கம்_OUTPUTS, test_module, MixingOutput::SchedulingPolicy::Disabled, பொய், பொய்};
	mixing_output.set_All_Disarmed_மதிப்புகள்(DISARMED_மதிப்பு);
	mixing_output.set_All_Failsafe_மதிப்புகள்(FAILSAFE_மதிப்பு);
	mixing_output.set_All_Min_மதிப்புகள்(MIN_மதிப்பு);
	mixing_output.set_All_Max_மதிப்புகள்(MAX_மதிப்பு);

	test_module.sendMotors({1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f});
	test_module.sendServos({1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f});
	test_module.sendActuatorArmed(பொய், பொய், பொய், உண்மை);

	// ensure all disarmed, except the servo
	mixing_output.updateSubscriptions(பொய்);
	EXPECT_EQ(test_module.இலக்கம்_updates, update(mixing_output));
	EXPECT_EQ(test_module.இலக்கம்_outputs, MAX_இலக்கம்_OUTPUTS);

	க்கு (முழு i = 0; i < MAX_இலக்கம்_OUTPUTS; ++i) {
		எனில் (i == 1) {
			EXPECT_EQ(test_module.outputs[i], MAX_மதிப்பு);

		} இல்லையெனில் {
			EXPECT_EQ(test_module.outputs[i], DISARMED_மதிப்பு);
		}
	}

	test_module.reset();

	EXPECT_FALSE(test_module.was_scheduled);
}

வகுப்பு TestMixingOutput : பொது MixingOutput
{
பொது:
	TestMixingOutput(மாறிலி எழுத்து *param_prefix, uint8_t max_இலக்கம்_outputs, OutputModuleInterface &interface,
			 SchedulingPolicy scheduling_policy,
			 தருக்க support_esc_calibration, தருக்க ramp_up = உண்மை)
		: MixingOutput(param_prefix, max_இலக்கம்_outputs, interface, scheduling_policy, support_esc_calibration, ramp_up)
	{};
	தசம output_limit_calc_single(முழு i, தசம value) மாறிலி { திரும்பு MixingOutput::output_limit_calc_single(i, value); }
};

TEST_F(MixerModuleTest, OutputLimitCalcSingle)
{
	OutputModuleTest test_module;
	test_module.configureFunctions({(முழு)OutputFunction::Motor1});
	TestMixingOutput mixing_output{PARAM_PREFIX, MAX_இலக்கம்_OUTPUTS, test_module, MixingOutput::SchedulingPolicy::Disabled, பொய், பொய்};

	mixing_output.set_All_Min_மதிப்புகள்(MIN_மதிப்பு); // default range [1000,2000]
	mixing_output.set_All_Max_மதிப்புகள்(MAX_மதிப்பு);
	mixing_output.set_All_Center_மதிப்புகள்(CENTER_மதிப்பு); // Set center to middle value
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

	mixing_output.set_All_Min_மதிப்புகள்(0); // lower range [0,20]
	mixing_output.set_All_Max_மதிப்புகள்(20);
	mixing_output.set_All_Center_மதிப்புகள்(10); // Set center to middle value
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

	mixing_output.set_All_Min_மதிப்புகள்(20); // inverted range [20,0]
	mixing_output.set_All_Max_மதிப்புகள்(0);
	mixing_output.set_All_Center_மதிப்புகள்(10); // Set center to middle value
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

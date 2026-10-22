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

#అన్తర్భావ <gtest/gtest.h>
#అన్తర్భావ <array>

#అన్తర్భావ <parameters/param.h>
#అన్తర్భావ <uORB/topics/actuator_motors.h>
#అన్తర్భావ <uORB/topics/actuator_servos.h>
#అన్తర్భావ <uORB/topics/actuator_armed.h>
#అన్తర్భావ <uORB/topics/actuator_test.h>
#అన్తర్భావ <uORB/Publication.hpp>
#అన్తర్భావ <uORB/Subscription.hpp>

#అన్తర్భావ "mixer_module.hpp"

#ఒకవేళ defined(CONFIG_ARCH_BOARD_PX4_SITL)
#నిర్వచించు PARAM_ఉపసర్గ "PWM_MAIN"
#లేకపోతే
#నిర్వచించు PARAM_ఉపసర్గ "HIL_ACT"
#అయితే_ముగింపు

స్థిర constexpr పూర్ణాంకం MAX_అంకె_OUTPUTS = 8;
స్థిర constexpr పూర్ణాంకం DISARMED_విలువ = 900;
స్థిర constexpr పూర్ణాంకం FAILSAFE_విలువ = 800;
స్థిర constexpr పూర్ణాంకం MIN_విలువ = 1000;
స్థిర constexpr పూర్ణాంకం CENTER_విలువ = 1500;
స్థిర constexpr పూర్ణాంకం MAX_విలువ = 2000;

తరగతి MixerModuleTest : బహిరంగ ::testing::Test
{
బహిరంగ:
	శూన్య సమితి_పైకి() override
	{
		param_control_autosave(అబద్ధం);
	}

	పూర్ణాంకం update(MixingOutput &mixing_output)
	{
		mixing_output.update();
		// make sure output_limit switches to ON (if outputs enabled)
		px4_usleep(50000 * 2);
		mixing_output.update();
		mixing_output.update();
		తిరిగి 3; // expected number of output updates
	}

};

తరగతి OutputModuleTest : బహిరంగ OutputModuleInterface
{
బహిరంగ:
	OutputModuleTest() : OutputModuleInterface(MODULE_NAME, px4::wq_configurations::hp_default) {};

	~OutputModuleTest() override
	{
		// Hide this test's commands from the next test: a zero actuator command is not inert
		_actuator_motors_pub.unadvertise();
		_actuator_servos_pub.unadvertise();
		_actuator_armed_pub.unadvertise();
	}

	శూన్య నిరంతరం() override
	{
		was_scheduled = నిజం;
	}

	తార్కిక updateOutputs(దశాంశం outputs_[MAX_ACTUATORS], గుర్తులేని అంకె_outputs_2, గుర్తులేని అంకె_control_groups_updated) override
	{
		జ్ఞాపకం_నకలు(outputs, outputs_, పరిమాణం(outputs));
		అంకె_outputs = అంకె_outputs_2;
		++అంకె_updates;
		తిరిగి నిజం;
	}

	శూన్య mixer_మారింది() override
	{
		mixer_మారింది_2 = నిజం;
	}

	శూన్య configureFunctions(స్థిరరాశి std::array<int32_t, MAX_అంకె_OUTPUTS> &functions)
	{
		కొరకు (పూర్ణాంకం i = 0; i < MAX_అంకె_OUTPUTS; ++i) {
			అక్షరం బఫరు_2[17];

			snprintf(బఫరు_2, పరిమాణం(బఫరు_2), "%s_FUNC%u", PARAM_ఉపసర్గ, i + 1);
			param_సమితి(param_కనుగొను(బఫరు_2), &functions[i]);
		}

		updateParams();
	}

	శూన్య sendMotors(స్థిరరాశి std::array<దశాంశం, actuator_motors_s::అంకె_CONTROLS> &motors, uint16_t reversible = 0)
	{
		actuator_motors_s actuator_motors{};
		actuator_motors.timestamp = hrt_absolute_సమయం();
		actuator_motors.reversible_flags = reversible;

		కొరకు (గుర్తులేని i = 0; i < motors.కొలత(); ++i) {
			actuator_motors.control[i] = motors[i];
		}

		_actuator_motors_pub.publish(actuator_motors);
	}

	శూన్య sendServos(స్థిరరాశి std::array<దశాంశం, actuator_servos_s::అంకె_CONTROLS> &servos)
	{
		actuator_servos_s actuator_servos{};
		actuator_servos.timestamp = hrt_absolute_సమయం();

		కొరకు (గుర్తులేని i = 0; i < servos.కొలత(); ++i) {
			actuator_servos.control[i] = servos[i];
		}

		_actuator_servos_pub.publish(actuator_servos);
	}

	శూన్య sendActuatorMotorTest(పూర్ణాంకం function, దశాంశం value, తార్కిక release_control)
	{
		actuator_test_s actuator_test{};
		actuator_test.timestamp = hrt_absolute_సమయం();
		actuator_test.function = function;
		actuator_test.value = value;
		actuator_test.action = release_control ? actuator_test_s::ACTION_RELEASE_CONTROL : actuator_test_s::ACTION_DO_CONTROL;
		actuator_test.timeout_ms = 0;
		_actuator_test_pub.publish(actuator_test);
	}

	శూన్య sendActuatorArmed(తార్కిక armed, తార్కిక termination = అబద్ధం, తార్కిక kill = అబద్ధం, తార్కిక prearm = అబద్ధం)
	{
		actuator_armed_s actuator_armed{};
		actuator_armed.timestamp = hrt_absolute_సమయం();
		actuator_armed.armed = armed;
		actuator_armed.termination = termination;
		actuator_armed.kill = kill;
		actuator_armed.prearmed = prearm;
		_actuator_armed_pub.publish(actuator_armed);
	}

	శూన్య reset()
	{
		memset(outputs, 0, పరిమాణం(outputs));
		అంకె_outputs = 0;
		అంకె_updates = 0;
		mixer_మారింది_2 = అబద్ధం;
	}

	దశాంశం outputs[MAX_ACTUATORS] {};
	పూర్ణాంకం అంకె_outputs{0};
	పూర్ణాంకం అంకె_updates{0};
	తార్కిక was_scheduled{అబద్ధం};
	తార్కిక mixer_మారింది_2{అబద్ధం};

అంతరంగ:
	uORB::Publication<actuator_test_s> _actuator_test_pub{ORB_ID(actuator_test)};
	uORB::Publication<actuator_motors_s> _actuator_motors_pub{ORB_ID(actuator_motors)};
	uORB::Publication<actuator_servos_s> _actuator_servos_pub{ORB_ID(actuator_servos)};
	uORB::Publication<actuator_armed_s> _actuator_armed_pub{ORB_ID(actuator_armed)};
};

TEST_F(MixerModuleTest, basic)
{
	OutputModuleTest test_module;
	test_module.configureFunctions({});
	MixingOutput mixing_output{PARAM_ఉపసర్గ, MAX_అంకె_OUTPUTS, test_module, MixingOutput::SchedulingPolicy::Disabled, అబద్ధం, అబద్ధం};
	mixing_output.సమితి_All_Disarmed_విలువలు(DISARMED_విలువ);
	mixing_output.సమితి_All_Failsafe_విలువలు(FAILSAFE_విలువ);
	mixing_output.సమితి_All_Min_విలువలు(MIN_విలువ);
	mixing_output.సమితి_All_Center_విలువలు(CENTER_విలువ);
	mixing_output.సమితి_All_Max_విలువలు(MAX_విలువ);
	EXPECT_EQ(test_module.అంకె_updates, 0);

	// all functions disabled: expect to get one single update to process disabling the output signal
	mixing_output.update();
	mixing_output.updateSubscriptions(అబద్ధం);
	mixing_output.update();
	EXPECT_EQ(test_module.అంకె_updates, 1);
	mixing_output.update();
	mixing_output.updateSubscriptions(అబద్ధం);
	mixing_output.update();
	EXPECT_EQ(test_module.అంకె_updates, 1);
	test_module.reset();

	// configure motor, ensure all still disarmed
	test_module.configureFunctions({(పూర్ణాంకం)OutputFunction::Motor1});
	mixing_output.updateSubscriptions(అబద్ధం);
	EXPECT_TRUE(test_module.mixer_మారింది_2);
	EXPECT_EQ(test_module.అంకె_updates, update(mixing_output));
	EXPECT_EQ(test_module.అంకె_outputs, MAX_అంకె_OUTPUTS);

	కొరకు (పూర్ణాంకం i = 0; i < test_module.అంకె_outputs; ++i) {
		EXPECT_EQ(test_module.outputs[i], DISARMED_విలువ);
	}

	test_module.reset();

	// send motors -> still disarmed
	test_module.sendMotors({1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f});
	test_module.configureFunctions({
		0,
		(పూర్ణాంకం)OutputFunction::Motor3,
		(పూర్ణాంకం)OutputFunction::Motor1,
		(పూర్ణాంకం)OutputFunction::Motor5});

	mixing_output.updateSubscriptions(అబద్ధం);
	EXPECT_EQ(test_module.అంకె_updates, update(mixing_output));
	EXPECT_EQ(test_module.అంకె_outputs, MAX_అంకె_OUTPUTS);

	కొరకు (పూర్ణాంకం i = 0; i < test_module.అంకె_outputs; ++i) {
		EXPECT_EQ(test_module.outputs[i], DISARMED_విలువ);
	}

	test_module.reset();


	// actuator test
	test_module.sendActuatorMotorTest((పూర్ణాంకం)OutputFunction::Motor5, 1.f, అబద్ధం);
	test_module.sendMotors({1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f});
	mixing_output.updateSubscriptions(అబద్ధం);
	EXPECT_EQ(test_module.అంకె_updates, update(mixing_output));
	EXPECT_EQ(test_module.అంకె_outputs, MAX_అంకె_OUTPUTS);

	కొరకు (పూర్ణాంకం i = 0; i < test_module.అంకె_outputs; ++i) {
		ఒకవేళ (i == 3) {
			EXPECT_EQ(test_module.outputs[i], MAX_విలువ);

		} లేకపోతే {
			EXPECT_EQ(test_module.outputs[i], DISARMED_విలువ);
		}
	}

	test_module.reset();

	// stop
	test_module.sendActuatorMotorTest((పూర్ణాంకం)OutputFunction::Motor5, 0.f, నిజం);
	test_module.sendMotors({1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f});
	mixing_output.updateSubscriptions(అబద్ధం);
	EXPECT_EQ(test_module.అంకె_updates, update(mixing_output));
	EXPECT_EQ(test_module.అంకె_outputs, MAX_అంకె_OUTPUTS);

	కొరకు (పూర్ణాంకం i = 0; i < test_module.అంకె_outputs; ++i) {
		EXPECT_EQ(test_module.outputs[i], DISARMED_విలువ);
	}

	test_module.reset();

	EXPECT_FALSE(test_module.was_scheduled);
}

TEST_F(MixerModuleTest, arming)
{
	OutputModuleTest test_module;
	test_module.configureFunctions({
		0,
		(పూర్ణాంకం)OutputFunction::Motor3,
		(పూర్ణాంకం)OutputFunction::Motor1,
		(పూర్ణాంకం)OutputFunction::Motor5,
		(పూర్ణాంకం)OutputFunction::Servo3});
	MixingOutput mixing_output{PARAM_ఉపసర్గ, MAX_అంకె_OUTPUTS, test_module, MixingOutput::SchedulingPolicy::Disabled, అబద్ధం, అబద్ధం};
	mixing_output.సమితి_All_Disarmed_విలువలు(DISARMED_విలువ);
	mixing_output.సమితి_All_Failsafe_విలువలు(FAILSAFE_విలువ);
	mixing_output.సమితి_All_Min_విలువలు(MIN_విలువ);
	mixing_output.సమితి_All_Center_విలువలు(CENTER_విలువ);
	mixing_output.సమితి_All_Max_విలువలు(MAX_విలువ);

	test_module.sendMotors({1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f});
	test_module.sendActuatorArmed(అబద్ధం);

	// ensure all disarmed
	mixing_output.updateSubscriptions(అబద్ధం);
	EXPECT_EQ(test_module.అంకె_updates, update(mixing_output));
	EXPECT_EQ(test_module.అంకె_outputs, MAX_అంకె_OUTPUTS);

	కొరకు (పూర్ణాంకం i = 0; i < MAX_అంకె_OUTPUTS; ++i) {
		EXPECT_EQ(test_module.outputs[i], DISARMED_విలువ);
	}

	test_module.reset();

	// arming
	test_module.sendMotors({0.5f, 1.f, 0.1f, 0.2f, 1.f, 1.f, 1.f, 1.f});
	test_module.sendActuatorArmed(నిజం);

	EXPECT_EQ(test_module.అంకె_updates, update(mixing_output));
	EXPECT_EQ(test_module.అంకె_outputs, MAX_అంకె_OUTPUTS);

	కొరకు (పూర్ణాంకం i = 0; i < MAX_అంకె_OUTPUTS; ++i) {
		ఒకవేళ (i == 1) {
			EXPECT_EQ(test_module.outputs[i], (MAX_విలువ - MIN_విలువ) * 0.1f + MIN_విలువ);

		} లేకపోతే ఒకవేళ (i == 2) {
			EXPECT_EQ(test_module.outputs[i], (MAX_విలువ - MIN_విలువ) * 0.5f + MIN_విలువ);

		} లేకపోతే ఒకవేళ (i == 3) {
			EXPECT_EQ(test_module.outputs[i], MAX_విలువ);

		} లేకపోతే {
			EXPECT_EQ(test_module.outputs[i], DISARMED_విలువ);
		}
	}

	test_module.reset();

	// update motors
	test_module.sendMotors({0.9f, 1.f, 0.24f, 0.2f, 0.f, 1.f, 1.f, 1.f});
	mixing_output.updateSubscriptions(అబద్ధం);
	mixing_output.update();

	కొరకు (పూర్ణాంకం i = 0; i < MAX_అంకె_OUTPUTS; ++i) {
		ఒకవేళ (i == 1) {
			EXPECT_EQ(test_module.outputs[i], (MAX_విలువ - MIN_విలువ) * 0.24f + MIN_విలువ);

		} లేకపోతే ఒకవేళ (i == 2) {
			EXPECT_EQ(test_module.outputs[i], (MAX_విలువ - MIN_విలువ) * 0.9f + MIN_విలువ);

		} లేకపోతే ఒకవేళ (i == 3) {
			EXPECT_EQ(test_module.outputs[i], MIN_విలువ);

		} లేకపోతే {
			EXPECT_EQ(test_module.outputs[i], DISARMED_విలువ);
		}
	}

	test_module.reset();

	// failsafe
	test_module.sendActuatorArmed(నిజం, నిజం);
	test_module.sendMotors({0.5f, 1.f, 0.1f, 0.2f, 1.f, 1.f, 1.f, 1.f});
	mixing_output.update();

	కొరకు (పూర్ణాంకం i = 0; i < MAX_అంకె_OUTPUTS; ++i) {
		EXPECT_EQ(test_module.outputs[i], FAILSAFE_విలువ);
	}

	test_module.reset();

	// restore
	test_module.sendActuatorArmed(నిజం, అబద్ధం);
	test_module.sendMotors({1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f});
	mixing_output.update();

	కొరకు (పూర్ణాంకం i = 0; i < MAX_అంకె_OUTPUTS; ++i) {
		ఒకవేళ (i >= 1 && i <= 3) {
			EXPECT_EQ(test_module.outputs[i], MAX_విలువ);

		} లేకపోతే {
			EXPECT_EQ(test_module.outputs[i], DISARMED_విలువ);
		}
	}

	test_module.reset();

	// manual lockdown
	test_module.sendActuatorArmed(నిజం, అబద్ధం, నిజం);
	test_module.sendMotors({0.5f, 1.f, 0.1f, 0.2f, 1.f, 1.f, 1.f, 1.f});
	mixing_output.update();

	కొరకు (పూర్ణాంకం i = 0; i < MAX_అంకె_OUTPUTS; ++i) {
		EXPECT_EQ(test_module.outputs[i], DISARMED_విలువ);
	}

	test_module.reset();

	// restore
	test_module.sendActuatorArmed(నిజం, అబద్ధం);
	test_module.sendMotors({0.f, 0.f, 0.f, 0.f, 0.f, 1.f, 1.f, 1.f});
	mixing_output.update();

	కొరకు (పూర్ణాంకం i = 0; i < MAX_అంకె_OUTPUTS; ++i) {
		ఒకవేళ (i >= 1 && i <= 3) {
			EXPECT_EQ(test_module.outputs[i], MIN_విలువ);

		} లేకపోతే {
			EXPECT_EQ(test_module.outputs[i], DISARMED_విలువ);
		}
	}

	test_module.reset();

	// set motor 5 reversible: expect output to be in center when commanding to 0
	test_module.sendActuatorArmed(నిజం, అబద్ధం);
	test_module.sendMotors({0.f, 0.f, 0.f, 0.f, 0.f, 1.f, 1.f, 1.f}, 1u << 4);
	mixing_output.update();
	EXPECT_EQ(mixing_output.reversibleOutputs(), 1u << 3);

	కొరకు (పూర్ణాంకం i = 0; i < MAX_అంకె_OUTPUTS; ++i) {
		ఒకవేళ (i == 1) {
			EXPECT_EQ(test_module.outputs[i], MIN_విలువ);

		} లేకపోతే ఒకవేళ (i == 2) {
			EXPECT_EQ(test_module.outputs[i], MIN_విలువ);

		} లేకపోతే ఒకవేళ (i == 3) {
			EXPECT_EQ(test_module.outputs[i], (MAX_విలువ - MIN_విలువ) * 0.5f + MIN_విలువ);

		} లేకపోతే {
			EXPECT_EQ(test_module.outputs[i], DISARMED_విలువ);
		}
	}

	test_module.reset();

	// disarm
	test_module.sendActuatorArmed(అబద్ధం);
	test_module.sendMotors({0.f, 0.f, 0.f, 0.f, 0.f, 1.f, 1.f, 1.f}, 1u << 4);
	mixing_output.update();

	కొరకు (పూర్ణాంకం i = 0; i < MAX_అంకె_OUTPUTS; ++i) {
		EXPECT_EQ(test_module.outputs[i], DISARMED_విలువ);
	}

	test_module.reset();

	EXPECT_FALSE(test_module.was_scheduled);
}

TEST_F(MixerModuleTest, prearm)
{
	OutputModuleTest test_module;
	test_module.configureFunctions({
		(పూర్ణాంకం)OutputFunction::Motor1,
		(పూర్ణాంకం)OutputFunction::Servo1});
	MixingOutput mixing_output{PARAM_ఉపసర్గ, MAX_అంకె_OUTPUTS, test_module, MixingOutput::SchedulingPolicy::Disabled, అబద్ధం, అబద్ధం};
	mixing_output.సమితి_All_Disarmed_విలువలు(DISARMED_విలువ);
	mixing_output.సమితి_All_Failsafe_విలువలు(FAILSAFE_విలువ);
	mixing_output.సమితి_All_Min_విలువలు(MIN_విలువ);
	mixing_output.సమితి_All_Max_విలువలు(MAX_విలువ);

	test_module.sendMotors({1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f});
	test_module.sendServos({1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f});
	test_module.sendActuatorArmed(అబద్ధం, అబద్ధం, అబద్ధం, నిజం);

	// ensure all disarmed, except the servo
	mixing_output.updateSubscriptions(అబద్ధం);
	EXPECT_EQ(test_module.అంకె_updates, update(mixing_output));
	EXPECT_EQ(test_module.అంకె_outputs, MAX_అంకె_OUTPUTS);

	కొరకు (పూర్ణాంకం i = 0; i < MAX_అంకె_OUTPUTS; ++i) {
		ఒకవేళ (i == 1) {
			EXPECT_EQ(test_module.outputs[i], MAX_విలువ);

		} లేకపోతే {
			EXPECT_EQ(test_module.outputs[i], DISARMED_విలువ);
		}
	}

	test_module.reset();

	EXPECT_FALSE(test_module.was_scheduled);
}

తరగతి TestMixingOutput : బహిరంగ MixingOutput
{
బహిరంగ:
	TestMixingOutput(స్థిరరాశి అక్షరం *param_ఉపసర్గ, uint8_t max_అంకె_outputs, OutputModuleInterface &interface,
			 SchedulingPolicy scheduling_policy,
			 తార్కిక support_esc_calibration, తార్కిక ramp_పైకి = నిజం)
		: MixingOutput(param_ఉపసర్గ, max_అంకె_outputs, interface, scheduling_policy, support_esc_calibration, ramp_పైకి)
	{};
	దశాంశం output_హద్దు_calc_single(పూర్ణాంకం i, దశాంశం value) స్థిరరాశి { తిరిగి MixingOutput::output_హద్దు_calc_single(i, value); }
};

TEST_F(MixerModuleTest, Output_హద్దు_Calc_Single)
{
	OutputModuleTest test_module;
	test_module.configureFunctions({(పూర్ణాంకం)OutputFunction::Motor1});
	TestMixingOutput mixing_output{PARAM_ఉపసర్గ, MAX_అంకె_OUTPUTS, test_module, MixingOutput::SchedulingPolicy::Disabled, అబద్ధం, అబద్ధం};

	mixing_output.సమితి_All_Min_విలువలు(MIN_విలువ); // default range [1000,2000]
	mixing_output.సమితి_All_Max_విలువలు(MAX_విలువ);
	mixing_output.సమితి_All_Center_విలువలు(CENTER_విలువ); // Set center to middle value
	EXPECT_EQ(mixing_output.output_హద్దు_calc_single(0, -1.f), 1000); // In range
	EXPECT_EQ(mixing_output.output_హద్దు_calc_single(0, -.5f), 1250);
	EXPECT_EQ(mixing_output.output_హద్దు_calc_single(0, 0.f), 1500);
	EXPECT_EQ(mixing_output.output_హద్దు_calc_single(0, .5f), 1750);
	EXPECT_EQ(mixing_output.output_హద్దు_calc_single(0, 1.f), 2000);
	EXPECT_EQ(mixing_output.output_హద్దు_calc_single(0, -1.1f), 1000); // Out of range
	EXPECT_EQ(mixing_output.output_హద్దు_calc_single(0, 1.1f), 2000);
	EXPECT_EQ(mixing_output.output_హద్దు_calc_single(0, -1000.f), 1000); // Way ouf of range
	EXPECT_EQ(mixing_output.output_హద్దు_calc_single(0, 1000.f), 2000);
	EXPECT_EQ(mixing_output.output_హద్దు_calc_single(0, 0.0005), 1500.25f);
	EXPECT_EQ(static_cast<uint16_t>(lroundf(mixing_output.output_హద్దు_calc_single(0, 0.0005))), 1500); // Rounding down
	EXPECT_EQ(mixing_output.output_హద్దు_calc_single(0, 0.0015), 1500.75f);
	EXPECT_EQ(static_cast<uint16_t>(lroundf(mixing_output.output_హద్దు_calc_single(0, 0.0015))), 1501); // Rounding up
	EXPECT_EQ(mixing_output.output_హద్దు_calc_single(0, 0.002), 1501); // Exact value

	mixing_output.సమితి_All_Min_విలువలు(0); // lower range [0,20]
	mixing_output.సమితి_All_Max_విలువలు(20);
	mixing_output.సమితి_All_Center_విలువలు(10); // Set center to middle value
	EXPECT_EQ(mixing_output.output_హద్దు_calc_single(0, -1.f), 0); // In range
	EXPECT_EQ(mixing_output.output_హద్దు_calc_single(0, -.5f), 5);
	EXPECT_EQ(mixing_output.output_హద్దు_calc_single(0, 0.f), 10);
	EXPECT_EQ(mixing_output.output_హద్దు_calc_single(0, .5f), 15);
	EXPECT_EQ(mixing_output.output_హద్దు_calc_single(0, 1.f), 20);
	EXPECT_EQ(mixing_output.output_హద్దు_calc_single(0, -1.1f), 0); // Out of range
	EXPECT_EQ(mixing_output.output_హద్దు_calc_single(0, 1.1f), 20);
	EXPECT_EQ(mixing_output.output_హద్దు_calc_single(0, -1000.f), 0); // Way ouf of range
	EXPECT_EQ(mixing_output.output_హద్దు_calc_single(0, 1000.f), 20);
	EXPECT_EQ(mixing_output.output_హద్దు_calc_single(0, 0.025), 10.25f);
	EXPECT_EQ(static_cast<uint16_t>(lroundf(mixing_output.output_హద్దు_calc_single(0, 0.025))), 10); // Rounding down
	EXPECT_EQ(mixing_output.output_హద్దు_calc_single(0, 0.075), 10.75f);
	EXPECT_EQ(static_cast<uint16_t>(lroundf(mixing_output.output_హద్దు_calc_single(0, 0.075))), 11); // Rounding up
	EXPECT_EQ(mixing_output.output_హద్దు_calc_single(0, 0.1), 11); // Exact value

	mixing_output.సమితి_All_Min_విలువలు(20); // inverted range [20,0]
	mixing_output.సమితి_All_Max_విలువలు(0);
	mixing_output.సమితి_All_Center_విలువలు(10); // Set center to middle value
	EXPECT_EQ(mixing_output.output_హద్దు_calc_single(0, -1.f), 20); // In range
	EXPECT_EQ(mixing_output.output_హద్దు_calc_single(0, -.5f), 15);
	EXPECT_EQ(mixing_output.output_హద్దు_calc_single(0, 0.f), 10);
	EXPECT_EQ(mixing_output.output_హద్దు_calc_single(0, .5f), 5);
	EXPECT_EQ(mixing_output.output_హద్దు_calc_single(0, 1.f), 0);
	EXPECT_EQ(mixing_output.output_హద్దు_calc_single(0, -1.1f), 20); // Out of range
	EXPECT_EQ(mixing_output.output_హద్దు_calc_single(0, 1.1f), 0);
	EXPECT_EQ(mixing_output.output_హద్దు_calc_single(0, -1000.f), 20); // Way ouf of range
	EXPECT_EQ(mixing_output.output_హద్దు_calc_single(0, 1000.f), 0);
	EXPECT_EQ(mixing_output.output_హద్దు_calc_single(0, 0.025), 9.75f);
	EXPECT_EQ(static_cast<uint16_t>(lroundf(mixing_output.output_హద్దు_calc_single(0, 0.025))), 10); // Rounding down
	EXPECT_EQ(mixing_output.output_హద్దు_calc_single(0, 0.075), 9.25f);
	EXPECT_EQ(static_cast<uint16_t>(lroundf(mixing_output.output_హద్దు_calc_single(0, 0.075))), 9); // Rounding up
	EXPECT_EQ(mixing_output.output_హద్దు_calc_single(0, 0.1), 9); // Exact value
}

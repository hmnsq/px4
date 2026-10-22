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

#ଅନ୍ତର୍ଭାଵ <gtest/gtest.h>
#ଅନ୍ତର୍ଭାଵ <array>

#ଅନ୍ତର୍ଭାଵ <parameters/param.h>
#ଅନ୍ତର୍ଭାଵ <uORB/topics/actuator_motors.h>
#ଅନ୍ତର୍ଭାଵ <uORB/topics/actuator_servos.h>
#ଅନ୍ତର୍ଭାଵ <uORB/topics/actuator_armed.h>
#ଅନ୍ତର୍ଭାଵ <uORB/topics/actuator_test.h>
#ଅନ୍ତର୍ଭାଵ <uORB/Publication.hpp>
#ଅନ୍ତର୍ଭାଵ <uORB/Subscription.hpp>

#ଅନ୍ତର୍ଭାଵ "mixer_module.hpp"

#ଯଦି defined(CONFIG_ARCH_BOARD_PX4_SITL)
#define PARAM_PREFIX "PWM_MAIN"
#ଅନ୍ୟଥା
#define PARAM_PREFIX "HIL_ACT"
#endif

ସ୍ଥିର constexpr ପୂର୍ଣ୍ଣସଂଖ୍ୟା MAX_ଅଙ୍କ_OUTPUTS = 8;
ସ୍ଥିର constexpr ପୂର୍ଣ୍ଣସଂଖ୍ୟା DISARMED_ମୂଲ୍ୟ = 900;
ସ୍ଥିର constexpr ପୂର୍ଣ୍ଣସଂଖ୍ୟା FAILSAFE_ମୂଲ୍ୟ = 800;
ସ୍ଥିର constexpr ପୂର୍ଣ୍ଣସଂଖ୍ୟା MIN_ମୂଲ୍ୟ = 1000;
ସ୍ଥିର constexpr ପୂର୍ଣ୍ଣସଂଖ୍ୟା CENTER_ମୂଲ୍ୟ = 1500;
ସ୍ଥିର constexpr ପୂର୍ଣ୍ଣସଂଖ୍ୟା MAX_ମୂଲ୍ୟ = 2000;

ଶ୍ରେଣୀ MixerModuleTest : ସର୍ବସାଧାରଣ ::testing::Test
{
ସର୍ବସାଧାରଣ:
	ଶୂନ୍ୟ SetUp() override
	{
		param_control_autosave(ମିଥ୍ୟା);
	}

	ପୂର୍ଣ୍ଣସଂଖ୍ୟା update(MixingOutput &mixing_output)
	{
		mixing_output.update();
		// make sure output_limit switches to ON (if outputs enabled)
		px4_usleep(50000 * 2);
		mixing_output.update();
		mixing_output.update();
		ଫେରନ୍ତୁ 3; // expected number of output updates
	}

};

ଶ୍ରେଣୀ OutputModuleTest : ସର୍ବସାଧାରଣ OutputModuleInterface
{
ସର୍ବସାଧାରଣ:
	OutputModuleTest() : OutputModuleInterface(MODULE_NAME, px4::wq_configurations::hp_default) {};

	~OutputModuleTest() override
	{
		// Hide this test's commands from the next test: a zero actuator command is not inert
		_actuator_motors_pub.unadvertise();
		_actuator_servos_pub.unadvertise();
		_actuator_armed_pub.unadvertise();
	}

	ଶୂନ୍ୟ Run() override
	{
		was_scheduled = ସତ୍ୟ;
	}

	ତାର୍କିକ updateOutputs(ଦଶମିକ outputs_[MAX_ACTUATORS], unsigned ଅଙ୍କ_outputs_2, unsigned ଅଙ୍କ_control_groups_updated) override
	{
		memcpy(outputs, outputs_, ଆକାର(outputs));
		ଅଙ୍କ_outputs = ଅଙ୍କ_outputs_2;
		++ଅଙ୍କ_updates;
		ଫେରନ୍ତୁ ସତ୍ୟ;
	}

	ଶୂନ୍ୟ mixerChanged() override
	{
		mixer_changed = ସତ୍ୟ;
	}

	ଶୂନ୍ୟ configureFunctions(ଅଚଳ std::array<int32_t, MAX_ଅଙ୍କ_OUTPUTS> &functions)
	{
		ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < MAX_ଅଙ୍କ_OUTPUTS; ++i) {
			ଅକ୍ଷର buffer[17];

			snprintf(buffer, ଆକାର(buffer), "%s_FUNC%u", PARAM_PREFIX, i + 1);
			param_set(param_find(buffer), &functions[i]);
		}

		updateParams();
	}

	ଶୂନ୍ୟ sendMotors(ଅଚଳ std::array<ଦଶମିକ, actuator_motors_s::ଅଙ୍କ_CONTROLS> &motors, uint16_t reversible = 0)
	{
		actuator_motors_s actuator_motors{};
		actuator_motors.timestamp = hrt_absolute_time();
		actuator_motors.reversible_flags = reversible;

		ପାଇଁ (unsigned i = 0; i < motors.size(); ++i) {
			actuator_motors.control[i] = motors[i];
		}

		_actuator_motors_pub.publish(actuator_motors);
	}

	ଶୂନ୍ୟ sendServos(ଅଚଳ std::array<ଦଶମିକ, actuator_servos_s::ଅଙ୍କ_CONTROLS> &servos)
	{
		actuator_servos_s actuator_servos{};
		actuator_servos.timestamp = hrt_absolute_time();

		ପାଇଁ (unsigned i = 0; i < servos.size(); ++i) {
			actuator_servos.control[i] = servos[i];
		}

		_actuator_servos_pub.publish(actuator_servos);
	}

	ଶୂନ୍ୟ sendActuatorMotorTest(ପୂର୍ଣ୍ଣସଂଖ୍ୟା function, ଦଶମିକ value, ତାର୍କିକ release_control)
	{
		actuator_test_s actuator_test{};
		actuator_test.timestamp = hrt_absolute_time();
		actuator_test.function = function;
		actuator_test.value = value;
		actuator_test.action = release_control ? actuator_test_s::ACTION_RELEASE_CONTROL : actuator_test_s::ACTION_DO_CONTROL;
		actuator_test.timeout_ms = 0;
		_actuator_test_pub.publish(actuator_test);
	}

	ଶୂନ୍ୟ sendActuatorArmed(ତାର୍କିକ armed, ତାର୍କିକ termination = ମିଥ୍ୟା, ତାର୍କିକ kill = ମିଥ୍ୟା, ତାର୍କିକ prearm = ମିଥ୍ୟା)
	{
		actuator_armed_s actuator_armed{};
		actuator_armed.timestamp = hrt_absolute_time();
		actuator_armed.armed = armed;
		actuator_armed.termination = termination;
		actuator_armed.kill = kill;
		actuator_armed.prearmed = prearm;
		_actuator_armed_pub.publish(actuator_armed);
	}

	ଶୂନ୍ୟ reset()
	{
		memset(outputs, 0, ଆକାର(outputs));
		ଅଙ୍କ_outputs = 0;
		ଅଙ୍କ_updates = 0;
		mixer_changed = ମିଥ୍ୟା;
	}

	ଦଶମିକ outputs[MAX_ACTUATORS] {};
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଅଙ୍କ_outputs{0};
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଅଙ୍କ_updates{0};
	ତାର୍କିକ was_scheduled{ମିଥ୍ୟା};
	ତାର୍କିକ mixer_changed{ମିଥ୍ୟା};

ବ୍ୟକ୍ତିଗତ:
	uORB::Publication<actuator_test_s> _actuator_test_pub{ORB_ID(actuator_test)};
	uORB::Publication<actuator_motors_s> _actuator_motors_pub{ORB_ID(actuator_motors)};
	uORB::Publication<actuator_servos_s> _actuator_servos_pub{ORB_ID(actuator_servos)};
	uORB::Publication<actuator_armed_s> _actuator_armed_pub{ORB_ID(actuator_armed)};
};

TEST_F(MixerModuleTest, basic)
{
	OutputModuleTest test_module;
	test_module.configureFunctions({});
	MixingOutput mixing_output{PARAM_PREFIX, MAX_ଅଙ୍କ_OUTPUTS, test_module, MixingOutput::SchedulingPolicy::Disabled, ମିଥ୍ୟା, ମିଥ୍ୟା};
	mixing_output.set_All_Disarmed_ମୂଲ୍ୟଗୁଡ଼ିକ(DISARMED_ମୂଲ୍ୟ);
	mixing_output.set_All_Failsafe_ମୂଲ୍ୟଗୁଡ଼ିକ(FAILSAFE_ମୂଲ୍ୟ);
	mixing_output.set_All_Min_ମୂଲ୍ୟଗୁଡ଼ିକ(MIN_ମୂଲ୍ୟ);
	mixing_output.set_All_Center_ମୂଲ୍ୟଗୁଡ଼ିକ(CENTER_ମୂଲ୍ୟ);
	mixing_output.set_All_Max_ମୂଲ୍ୟଗୁଡ଼ିକ(MAX_ମୂଲ୍ୟ);
	EXPECT_EQ(test_module.ଅଙ୍କ_updates, 0);

	// all functions disabled: expect to get one single update to process disabling the output signal
	mixing_output.update();
	mixing_output.updateSubscriptions(ମିଥ୍ୟା);
	mixing_output.update();
	EXPECT_EQ(test_module.ଅଙ୍କ_updates, 1);
	mixing_output.update();
	mixing_output.updateSubscriptions(ମିଥ୍ୟା);
	mixing_output.update();
	EXPECT_EQ(test_module.ଅଙ୍କ_updates, 1);
	test_module.reset();

	// configure motor, ensure all still disarmed
	test_module.configureFunctions({(ପୂର୍ଣ୍ଣସଂଖ୍ୟା)OutputFunction::Motor1});
	mixing_output.updateSubscriptions(ମିଥ୍ୟା);
	EXPECT_TRUE(test_module.mixer_changed);
	EXPECT_EQ(test_module.ଅଙ୍କ_updates, update(mixing_output));
	EXPECT_EQ(test_module.ଅଙ୍କ_outputs, MAX_ଅଙ୍କ_OUTPUTS);

	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < test_module.ଅଙ୍କ_outputs; ++i) {
		EXPECT_EQ(test_module.outputs[i], DISARMED_ମୂଲ୍ୟ);
	}

	test_module.reset();

	// send motors -> still disarmed
	test_module.sendMotors({1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f});
	test_module.configureFunctions({
		0,
		(ପୂର୍ଣ୍ଣସଂଖ୍ୟା)OutputFunction::Motor3,
		(ପୂର୍ଣ୍ଣସଂଖ୍ୟା)OutputFunction::Motor1,
		(ପୂର୍ଣ୍ଣସଂଖ୍ୟା)OutputFunction::Motor5});

	mixing_output.updateSubscriptions(ମିଥ୍ୟା);
	EXPECT_EQ(test_module.ଅଙ୍କ_updates, update(mixing_output));
	EXPECT_EQ(test_module.ଅଙ୍କ_outputs, MAX_ଅଙ୍କ_OUTPUTS);

	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < test_module.ଅଙ୍କ_outputs; ++i) {
		EXPECT_EQ(test_module.outputs[i], DISARMED_ମୂଲ୍ୟ);
	}

	test_module.reset();


	// actuator test
	test_module.sendActuatorMotorTest((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)OutputFunction::Motor5, 1.f, ମିଥ୍ୟା);
	test_module.sendMotors({1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f});
	mixing_output.updateSubscriptions(ମିଥ୍ୟା);
	EXPECT_EQ(test_module.ଅଙ୍କ_updates, update(mixing_output));
	EXPECT_EQ(test_module.ଅଙ୍କ_outputs, MAX_ଅଙ୍କ_OUTPUTS);

	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < test_module.ଅଙ୍କ_outputs; ++i) {
		ଯଦି (i == 3) {
			EXPECT_EQ(test_module.outputs[i], MAX_ମୂଲ୍ୟ);

		} ଅନ୍ୟଥା {
			EXPECT_EQ(test_module.outputs[i], DISARMED_ମୂଲ୍ୟ);
		}
	}

	test_module.reset();

	// stop
	test_module.sendActuatorMotorTest((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)OutputFunction::Motor5, 0.f, ସତ୍ୟ);
	test_module.sendMotors({1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f});
	mixing_output.updateSubscriptions(ମିଥ୍ୟା);
	EXPECT_EQ(test_module.ଅଙ୍କ_updates, update(mixing_output));
	EXPECT_EQ(test_module.ଅଙ୍କ_outputs, MAX_ଅଙ୍କ_OUTPUTS);

	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < test_module.ଅଙ୍କ_outputs; ++i) {
		EXPECT_EQ(test_module.outputs[i], DISARMED_ମୂଲ୍ୟ);
	}

	test_module.reset();

	EXPECT_FALSE(test_module.was_scheduled);
}

TEST_F(MixerModuleTest, arming)
{
	OutputModuleTest test_module;
	test_module.configureFunctions({
		0,
		(ପୂର୍ଣ୍ଣସଂଖ୍ୟା)OutputFunction::Motor3,
		(ପୂର୍ଣ୍ଣସଂଖ୍ୟା)OutputFunction::Motor1,
		(ପୂର୍ଣ୍ଣସଂଖ୍ୟା)OutputFunction::Motor5,
		(ପୂର୍ଣ୍ଣସଂଖ୍ୟା)OutputFunction::Servo3});
	MixingOutput mixing_output{PARAM_PREFIX, MAX_ଅଙ୍କ_OUTPUTS, test_module, MixingOutput::SchedulingPolicy::Disabled, ମିଥ୍ୟା, ମିଥ୍ୟା};
	mixing_output.set_All_Disarmed_ମୂଲ୍ୟଗୁଡ଼ିକ(DISARMED_ମୂଲ୍ୟ);
	mixing_output.set_All_Failsafe_ମୂଲ୍ୟଗୁଡ଼ିକ(FAILSAFE_ମୂଲ୍ୟ);
	mixing_output.set_All_Min_ମୂଲ୍ୟଗୁଡ଼ିକ(MIN_ମୂଲ୍ୟ);
	mixing_output.set_All_Center_ମୂଲ୍ୟଗୁଡ଼ିକ(CENTER_ମୂଲ୍ୟ);
	mixing_output.set_All_Max_ମୂଲ୍ୟଗୁଡ଼ିକ(MAX_ମୂଲ୍ୟ);

	test_module.sendMotors({1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f});
	test_module.sendActuatorArmed(ମିଥ୍ୟା);

	// ensure all disarmed
	mixing_output.updateSubscriptions(ମିଥ୍ୟା);
	EXPECT_EQ(test_module.ଅଙ୍କ_updates, update(mixing_output));
	EXPECT_EQ(test_module.ଅଙ୍କ_outputs, MAX_ଅଙ୍କ_OUTPUTS);

	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < MAX_ଅଙ୍କ_OUTPUTS; ++i) {
		EXPECT_EQ(test_module.outputs[i], DISARMED_ମୂଲ୍ୟ);
	}

	test_module.reset();

	// arming
	test_module.sendMotors({0.5f, 1.f, 0.1f, 0.2f, 1.f, 1.f, 1.f, 1.f});
	test_module.sendActuatorArmed(ସତ୍ୟ);

	EXPECT_EQ(test_module.ଅଙ୍କ_updates, update(mixing_output));
	EXPECT_EQ(test_module.ଅଙ୍କ_outputs, MAX_ଅଙ୍କ_OUTPUTS);

	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < MAX_ଅଙ୍କ_OUTPUTS; ++i) {
		ଯଦି (i == 1) {
			EXPECT_EQ(test_module.outputs[i], (MAX_ମୂଲ୍ୟ - MIN_ମୂଲ୍ୟ) * 0.1f + MIN_ମୂଲ୍ୟ);

		} ଅନ୍ୟଥା ଯଦି (i == 2) {
			EXPECT_EQ(test_module.outputs[i], (MAX_ମୂଲ୍ୟ - MIN_ମୂଲ୍ୟ) * 0.5f + MIN_ମୂଲ୍ୟ);

		} ଅନ୍ୟଥା ଯଦି (i == 3) {
			EXPECT_EQ(test_module.outputs[i], MAX_ମୂଲ୍ୟ);

		} ଅନ୍ୟଥା {
			EXPECT_EQ(test_module.outputs[i], DISARMED_ମୂଲ୍ୟ);
		}
	}

	test_module.reset();

	// update motors
	test_module.sendMotors({0.9f, 1.f, 0.24f, 0.2f, 0.f, 1.f, 1.f, 1.f});
	mixing_output.updateSubscriptions(ମିଥ୍ୟା);
	mixing_output.update();

	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < MAX_ଅଙ୍କ_OUTPUTS; ++i) {
		ଯଦି (i == 1) {
			EXPECT_EQ(test_module.outputs[i], (MAX_ମୂଲ୍ୟ - MIN_ମୂଲ୍ୟ) * 0.24f + MIN_ମୂଲ୍ୟ);

		} ଅନ୍ୟଥା ଯଦି (i == 2) {
			EXPECT_EQ(test_module.outputs[i], (MAX_ମୂଲ୍ୟ - MIN_ମୂଲ୍ୟ) * 0.9f + MIN_ମୂଲ୍ୟ);

		} ଅନ୍ୟଥା ଯଦି (i == 3) {
			EXPECT_EQ(test_module.outputs[i], MIN_ମୂଲ୍ୟ);

		} ଅନ୍ୟଥା {
			EXPECT_EQ(test_module.outputs[i], DISARMED_ମୂଲ୍ୟ);
		}
	}

	test_module.reset();

	// failsafe
	test_module.sendActuatorArmed(ସତ୍ୟ, ସତ୍ୟ);
	test_module.sendMotors({0.5f, 1.f, 0.1f, 0.2f, 1.f, 1.f, 1.f, 1.f});
	mixing_output.update();

	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < MAX_ଅଙ୍କ_OUTPUTS; ++i) {
		EXPECT_EQ(test_module.outputs[i], FAILSAFE_ମୂଲ୍ୟ);
	}

	test_module.reset();

	// restore
	test_module.sendActuatorArmed(ସତ୍ୟ, ମିଥ୍ୟା);
	test_module.sendMotors({1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f});
	mixing_output.update();

	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < MAX_ଅଙ୍କ_OUTPUTS; ++i) {
		ଯଦି (i >= 1 && i <= 3) {
			EXPECT_EQ(test_module.outputs[i], MAX_ମୂଲ୍ୟ);

		} ଅନ୍ୟଥା {
			EXPECT_EQ(test_module.outputs[i], DISARMED_ମୂଲ୍ୟ);
		}
	}

	test_module.reset();

	// manual lockdown
	test_module.sendActuatorArmed(ସତ୍ୟ, ମିଥ୍ୟା, ସତ୍ୟ);
	test_module.sendMotors({0.5f, 1.f, 0.1f, 0.2f, 1.f, 1.f, 1.f, 1.f});
	mixing_output.update();

	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < MAX_ଅଙ୍କ_OUTPUTS; ++i) {
		EXPECT_EQ(test_module.outputs[i], DISARMED_ମୂଲ୍ୟ);
	}

	test_module.reset();

	// restore
	test_module.sendActuatorArmed(ସତ୍ୟ, ମିଥ୍ୟା);
	test_module.sendMotors({0.f, 0.f, 0.f, 0.f, 0.f, 1.f, 1.f, 1.f});
	mixing_output.update();

	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < MAX_ଅଙ୍କ_OUTPUTS; ++i) {
		ଯଦି (i >= 1 && i <= 3) {
			EXPECT_EQ(test_module.outputs[i], MIN_ମୂଲ୍ୟ);

		} ଅନ୍ୟଥା {
			EXPECT_EQ(test_module.outputs[i], DISARMED_ମୂଲ୍ୟ);
		}
	}

	test_module.reset();

	// set motor 5 reversible: expect output to be in center when commanding to 0
	test_module.sendActuatorArmed(ସତ୍ୟ, ମିଥ୍ୟା);
	test_module.sendMotors({0.f, 0.f, 0.f, 0.f, 0.f, 1.f, 1.f, 1.f}, 1u << 4);
	mixing_output.update();
	EXPECT_EQ(mixing_output.reversibleOutputs(), 1u << 3);

	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < MAX_ଅଙ୍କ_OUTPUTS; ++i) {
		ଯଦି (i == 1) {
			EXPECT_EQ(test_module.outputs[i], MIN_ମୂଲ୍ୟ);

		} ଅନ୍ୟଥା ଯଦି (i == 2) {
			EXPECT_EQ(test_module.outputs[i], MIN_ମୂଲ୍ୟ);

		} ଅନ୍ୟଥା ଯଦି (i == 3) {
			EXPECT_EQ(test_module.outputs[i], (MAX_ମୂଲ୍ୟ - MIN_ମୂଲ୍ୟ) * 0.5f + MIN_ମୂଲ୍ୟ);

		} ଅନ୍ୟଥା {
			EXPECT_EQ(test_module.outputs[i], DISARMED_ମୂଲ୍ୟ);
		}
	}

	test_module.reset();

	// disarm
	test_module.sendActuatorArmed(ମିଥ୍ୟା);
	test_module.sendMotors({0.f, 0.f, 0.f, 0.f, 0.f, 1.f, 1.f, 1.f}, 1u << 4);
	mixing_output.update();

	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < MAX_ଅଙ୍କ_OUTPUTS; ++i) {
		EXPECT_EQ(test_module.outputs[i], DISARMED_ମୂଲ୍ୟ);
	}

	test_module.reset();

	EXPECT_FALSE(test_module.was_scheduled);
}

TEST_F(MixerModuleTest, prearm)
{
	OutputModuleTest test_module;
	test_module.configureFunctions({
		(ପୂର୍ଣ୍ଣସଂଖ୍ୟା)OutputFunction::Motor1,
		(ପୂର୍ଣ୍ଣସଂଖ୍ୟା)OutputFunction::Servo1});
	MixingOutput mixing_output{PARAM_PREFIX, MAX_ଅଙ୍କ_OUTPUTS, test_module, MixingOutput::SchedulingPolicy::Disabled, ମିଥ୍ୟା, ମିଥ୍ୟା};
	mixing_output.set_All_Disarmed_ମୂଲ୍ୟଗୁଡ଼ିକ(DISARMED_ମୂଲ୍ୟ);
	mixing_output.set_All_Failsafe_ମୂଲ୍ୟଗୁଡ଼ିକ(FAILSAFE_ମୂଲ୍ୟ);
	mixing_output.set_All_Min_ମୂଲ୍ୟଗୁଡ଼ିକ(MIN_ମୂଲ୍ୟ);
	mixing_output.set_All_Max_ମୂଲ୍ୟଗୁଡ଼ିକ(MAX_ମୂଲ୍ୟ);

	test_module.sendMotors({1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f});
	test_module.sendServos({1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f});
	test_module.sendActuatorArmed(ମିଥ୍ୟା, ମିଥ୍ୟା, ମିଥ୍ୟା, ସତ୍ୟ);

	// ensure all disarmed, except the servo
	mixing_output.updateSubscriptions(ମିଥ୍ୟା);
	EXPECT_EQ(test_module.ଅଙ୍କ_updates, update(mixing_output));
	EXPECT_EQ(test_module.ଅଙ୍କ_outputs, MAX_ଅଙ୍କ_OUTPUTS);

	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < MAX_ଅଙ୍କ_OUTPUTS; ++i) {
		ଯଦି (i == 1) {
			EXPECT_EQ(test_module.outputs[i], MAX_ମୂଲ୍ୟ);

		} ଅନ୍ୟଥା {
			EXPECT_EQ(test_module.outputs[i], DISARMED_ମୂଲ୍ୟ);
		}
	}

	test_module.reset();

	EXPECT_FALSE(test_module.was_scheduled);
}

ଶ୍ରେଣୀ TestMixingOutput : ସର୍ବସାଧାରଣ MixingOutput
{
ସର୍ବସାଧାରଣ:
	TestMixingOutput(ଅଚଳ ଅକ୍ଷର *param_prefix, uint8_t max_ଅଙ୍କ_outputs, OutputModuleInterface &interface,
			 SchedulingPolicy scheduling_policy,
			 ତାର୍କିକ support_esc_calibration, ତାର୍କିକ ramp_up = ସତ୍ୟ)
		: MixingOutput(param_prefix, max_ଅଙ୍କ_outputs, interface, scheduling_policy, support_esc_calibration, ramp_up)
	{};
	ଦଶମିକ output_limit_calc_single(ପୂର୍ଣ୍ଣସଂଖ୍ୟା i, ଦଶମିକ value) ଅଚଳ { ଫେରନ୍ତୁ MixingOutput::output_limit_calc_single(i, value); }
};

TEST_F(MixerModuleTest, OutputLimitCalcSingle)
{
	OutputModuleTest test_module;
	test_module.configureFunctions({(ପୂର୍ଣ୍ଣସଂଖ୍ୟା)OutputFunction::Motor1});
	TestMixingOutput mixing_output{PARAM_PREFIX, MAX_ଅଙ୍କ_OUTPUTS, test_module, MixingOutput::SchedulingPolicy::Disabled, ମିଥ୍ୟା, ମିଥ୍ୟା};

	mixing_output.set_All_Min_ମୂଲ୍ୟଗୁଡ଼ିକ(MIN_ମୂଲ୍ୟ); // default range [1000,2000]
	mixing_output.set_All_Max_ମୂଲ୍ୟଗୁଡ଼ିକ(MAX_ମୂଲ୍ୟ);
	mixing_output.set_All_Center_ମୂଲ୍ୟଗୁଡ଼ିକ(CENTER_ମୂଲ୍ୟ); // Set center to middle value
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

	mixing_output.set_All_Min_ମୂଲ୍ୟଗୁଡ଼ିକ(0); // lower range [0,20]
	mixing_output.set_All_Max_ମୂଲ୍ୟଗୁଡ଼ିକ(20);
	mixing_output.set_All_Center_ମୂଲ୍ୟଗୁଡ଼ିକ(10); // Set center to middle value
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

	mixing_output.set_All_Min_ମୂଲ୍ୟଗୁଡ଼ିକ(20); // inverted range [20,0]
	mixing_output.set_All_Max_ମୂଲ୍ୟଗୁଡ଼ିକ(0);
	mixing_output.set_All_Center_ମୂଲ୍ୟଗୁଡ଼ିକ(10); // Set center to middle value
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

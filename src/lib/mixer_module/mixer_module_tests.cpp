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

#ਅਨ੍ਤਰ੍ਭਾਵ <gtest/gtest.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <array>

#ਅਨ੍ਤਰ੍ਭਾਵ <parameters/param.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <uORB/topics/actuator_motors.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <uORB/topics/actuator_servos.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <uORB/topics/actuator_armed.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <uORB/topics/actuator_test.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <uORB/Publication.hpp>
#ਅਨ੍ਤਰ੍ਭਾਵ <uORB/Subscription.hpp>

#ਅਨ੍ਤਰ੍ਭਾਵ "mixer_module.hpp"

#ਜੇ defined(CONFIG_ARCH_BOARD_PX4_SITL)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PARAM_ਅਗੇਤਰ "PWM_MAIN"
#ਨਹੀਂਤਾਂ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PARAM_ਅਗੇਤਰ "HIL_ACT"
#ਜੇ_ਅੰਤ

ਸਥਿਰ constexpr ਪੂਰਨ MAX_ਅੰਕ_OUTPUTS = 8;
ਸਥਿਰ constexpr ਪੂਰਨ DISARMED_ਮੁੱਲ = 900;
ਸਥਿਰ constexpr ਪੂਰਨ FAILSAFE_ਮੁੱਲ = 800;
ਸਥਿਰ constexpr ਪੂਰਨ MIN_ਮੁੱਲ = 1000;
ਸਥਿਰ constexpr ਪੂਰਨ CENTER_ਮੁੱਲ = 1500;
ਸਥਿਰ constexpr ਪੂਰਨ MAX_ਮੁੱਲ = 2000;

ਵਰਗ MixerModuleTest : ਜਨਤਕ ::testing::Test
{
ਜਨਤਕ:
	ਖਾਲੀ ਸੈੱਟ_ਉੱਪਰ() override
	{
		param_control_autosave(ਝੂਠ);
	}

	ਪੂਰਨ update(MixingOutput &mixing_output)
	{
		mixing_output.update();
		// make sure output_limit switches to ON (if outputs enabled)
		px4_usleep(50000 * 2);
		mixing_output.update();
		mixing_output.update();
		ਵਾਪਸ 3; // expected number of output updates
	}

};

ਵਰਗ OutputModuleTest : ਜਨਤਕ OutputModuleInterface
{
ਜਨਤਕ:
	OutputModuleTest() : OutputModuleInterface(MODULE_NAME, px4::wq_configurations::hp_default) {};

	~OutputModuleTest() override
	{
		// Hide this test's commands from the next test: a zero actuator command is not inert
		_actuator_motors_pub.unadvertise();
		_actuator_servos_pub.unadvertise();
		_actuator_armed_pub.unadvertise();
	}

	ਖਾਲੀ ਲੜੀਵਾਰ() override
	{
		was_scheduled = ਸੱਚ;
	}

	ਤਾਰਕਿਕ updateOutputs(ਦਸ਼ਮਲਵ outputs_[MAX_ACTUATORS], ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੰਕ_outputs_2, ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੰਕ_control_groups_updated) override
	{
		ਸਮ੍ਰਿਤੀ_ਨਕਲ(outputs, outputs_, ਆਕਾਰ(outputs));
		ਅੰਕ_outputs = ਅੰਕ_outputs_2;
		++ਅੰਕ_updates;
		ਵਾਪਸ ਸੱਚ;
	}

	ਖਾਲੀ mixer_ਬਦਲਿਆ() override
	{
		mixer_ਬਦਲਿਆ_2 = ਸੱਚ;
	}

	ਖਾਲੀ configureFunctions(ਅਚਲ std::array<int32_t, MAX_ਅੰਕ_OUTPUTS> &functions)
	{
		ਲਈ (ਪੂਰਨ i = 0; i < MAX_ਅੰਕ_OUTPUTS; ++i) {
			ਅੱਖਰ ਬਫਰ_2[17];

			snprintf(ਬਫਰ_2, ਆਕਾਰ(ਬਫਰ_2), "%s_FUNC%u", PARAM_ਅਗੇਤਰ, i + 1);
			param_ਸੈੱਟ(param_ਲੱਭੋ(ਬਫਰ_2), &functions[i]);
		}

		updateParams();
	}

	ਖਾਲੀ sendMotors(ਅਚਲ std::array<ਦਸ਼ਮਲਵ, actuator_motors_s::ਅੰਕ_CONTROLS> &motors, uint16_t reversible = 0)
	{
		actuator_motors_s actuator_motors{};
		actuator_motors.timestamp = hrt_absolute_ਸਮਾਂ();
		actuator_motors.reversible_flags = reversible;

		ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ i = 0; i < motors.ਮਾਪ(); ++i) {
			actuator_motors.control[i] = motors[i];
		}

		_actuator_motors_pub.publish(actuator_motors);
	}

	ਖਾਲੀ sendServos(ਅਚਲ std::array<ਦਸ਼ਮਲਵ, actuator_servos_s::ਅੰਕ_CONTROLS> &servos)
	{
		actuator_servos_s actuator_servos{};
		actuator_servos.timestamp = hrt_absolute_ਸਮਾਂ();

		ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ i = 0; i < servos.ਮਾਪ(); ++i) {
			actuator_servos.control[i] = servos[i];
		}

		_actuator_servos_pub.publish(actuator_servos);
	}

	ਖਾਲੀ sendActuatorMotorTest(ਪੂਰਨ function, ਦਸ਼ਮਲਵ value, ਤਾਰਕਿਕ release_control)
	{
		actuator_test_s actuator_test{};
		actuator_test.timestamp = hrt_absolute_ਸਮਾਂ();
		actuator_test.function = function;
		actuator_test.value = value;
		actuator_test.action = release_control ? actuator_test_s::ACTION_RELEASE_CONTROL : actuator_test_s::ACTION_DO_CONTROL;
		actuator_test.timeout_ms = 0;
		_actuator_test_pub.publish(actuator_test);
	}

	ਖਾਲੀ sendActuatorArmed(ਤਾਰਕਿਕ armed, ਤਾਰਕਿਕ termination = ਝੂਠ, ਤਾਰਕਿਕ kill = ਝੂਠ, ਤਾਰਕਿਕ prearm = ਝੂਠ)
	{
		actuator_armed_s actuator_armed{};
		actuator_armed.timestamp = hrt_absolute_ਸਮਾਂ();
		actuator_armed.armed = armed;
		actuator_armed.termination = termination;
		actuator_armed.kill = kill;
		actuator_armed.prearmed = prearm;
		_actuator_armed_pub.publish(actuator_armed);
	}

	ਖਾਲੀ reset()
	{
		memset(outputs, 0, ਆਕਾਰ(outputs));
		ਅੰਕ_outputs = 0;
		ਅੰਕ_updates = 0;
		mixer_ਬਦਲਿਆ_2 = ਝੂਠ;
	}

	ਦਸ਼ਮਲਵ outputs[MAX_ACTUATORS] {};
	ਪੂਰਨ ਅੰਕ_outputs{0};
	ਪੂਰਨ ਅੰਕ_updates{0};
	ਤਾਰਕਿਕ was_scheduled{ਝੂਠ};
	ਤਾਰਕਿਕ mixer_ਬਦਲਿਆ_2{ਝੂਠ};

ਨਿਜੀ:
	uORB::Publication<actuator_test_s> _actuator_test_pub{ORB_ID(actuator_test)};
	uORB::Publication<actuator_motors_s> _actuator_motors_pub{ORB_ID(actuator_motors)};
	uORB::Publication<actuator_servos_s> _actuator_servos_pub{ORB_ID(actuator_servos)};
	uORB::Publication<actuator_armed_s> _actuator_armed_pub{ORB_ID(actuator_armed)};
};

TEST_F(MixerModuleTest, basic)
{
	OutputModuleTest test_module;
	test_module.configureFunctions({});
	MixingOutput mixing_output{PARAM_ਅਗੇਤਰ, MAX_ਅੰਕ_OUTPUTS, test_module, MixingOutput::SchedulingPolicy::Disabled, ਝੂਠ, ਝੂਠ};
	mixing_output.ਸੈੱਟ_All_Disarmed_ਮੁੱਲਾਂ(DISARMED_ਮੁੱਲ);
	mixing_output.ਸੈੱਟ_All_Failsafe_ਮੁੱਲਾਂ(FAILSAFE_ਮੁੱਲ);
	mixing_output.ਸੈੱਟ_All_Min_ਮੁੱਲਾਂ(MIN_ਮੁੱਲ);
	mixing_output.ਸੈੱਟ_All_Center_ਮੁੱਲਾਂ(CENTER_ਮੁੱਲ);
	mixing_output.ਸੈੱਟ_All_Max_ਮੁੱਲਾਂ(MAX_ਮੁੱਲ);
	EXPECT_EQ(test_module.ਅੰਕ_updates, 0);

	// all functions disabled: expect to get one single update to process disabling the output signal
	mixing_output.update();
	mixing_output.updateSubscriptions(ਝੂਠ);
	mixing_output.update();
	EXPECT_EQ(test_module.ਅੰਕ_updates, 1);
	mixing_output.update();
	mixing_output.updateSubscriptions(ਝੂਠ);
	mixing_output.update();
	EXPECT_EQ(test_module.ਅੰਕ_updates, 1);
	test_module.reset();

	// configure motor, ensure all still disarmed
	test_module.configureFunctions({(ਪੂਰਨ)OutputFunction::Motor1});
	mixing_output.updateSubscriptions(ਝੂਠ);
	EXPECT_TRUE(test_module.mixer_ਬਦਲਿਆ_2);
	EXPECT_EQ(test_module.ਅੰਕ_updates, update(mixing_output));
	EXPECT_EQ(test_module.ਅੰਕ_outputs, MAX_ਅੰਕ_OUTPUTS);

	ਲਈ (ਪੂਰਨ i = 0; i < test_module.ਅੰਕ_outputs; ++i) {
		EXPECT_EQ(test_module.outputs[i], DISARMED_ਮੁੱਲ);
	}

	test_module.reset();

	// send motors -> still disarmed
	test_module.sendMotors({1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f});
	test_module.configureFunctions({
		0,
		(ਪੂਰਨ)OutputFunction::Motor3,
		(ਪੂਰਨ)OutputFunction::Motor1,
		(ਪੂਰਨ)OutputFunction::Motor5});

	mixing_output.updateSubscriptions(ਝੂਠ);
	EXPECT_EQ(test_module.ਅੰਕ_updates, update(mixing_output));
	EXPECT_EQ(test_module.ਅੰਕ_outputs, MAX_ਅੰਕ_OUTPUTS);

	ਲਈ (ਪੂਰਨ i = 0; i < test_module.ਅੰਕ_outputs; ++i) {
		EXPECT_EQ(test_module.outputs[i], DISARMED_ਮੁੱਲ);
	}

	test_module.reset();


	// actuator test
	test_module.sendActuatorMotorTest((ਪੂਰਨ)OutputFunction::Motor5, 1.f, ਝੂਠ);
	test_module.sendMotors({1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f});
	mixing_output.updateSubscriptions(ਝੂਠ);
	EXPECT_EQ(test_module.ਅੰਕ_updates, update(mixing_output));
	EXPECT_EQ(test_module.ਅੰਕ_outputs, MAX_ਅੰਕ_OUTPUTS);

	ਲਈ (ਪੂਰਨ i = 0; i < test_module.ਅੰਕ_outputs; ++i) {
		ਜੇ (i == 3) {
			EXPECT_EQ(test_module.outputs[i], MAX_ਮੁੱਲ);

		} ਨਹੀਂਤਾਂ {
			EXPECT_EQ(test_module.outputs[i], DISARMED_ਮੁੱਲ);
		}
	}

	test_module.reset();

	// stop
	test_module.sendActuatorMotorTest((ਪੂਰਨ)OutputFunction::Motor5, 0.f, ਸੱਚ);
	test_module.sendMotors({1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f});
	mixing_output.updateSubscriptions(ਝੂਠ);
	EXPECT_EQ(test_module.ਅੰਕ_updates, update(mixing_output));
	EXPECT_EQ(test_module.ਅੰਕ_outputs, MAX_ਅੰਕ_OUTPUTS);

	ਲਈ (ਪੂਰਨ i = 0; i < test_module.ਅੰਕ_outputs; ++i) {
		EXPECT_EQ(test_module.outputs[i], DISARMED_ਮੁੱਲ);
	}

	test_module.reset();

	EXPECT_FALSE(test_module.was_scheduled);
}

TEST_F(MixerModuleTest, arming)
{
	OutputModuleTest test_module;
	test_module.configureFunctions({
		0,
		(ਪੂਰਨ)OutputFunction::Motor3,
		(ਪੂਰਨ)OutputFunction::Motor1,
		(ਪੂਰਨ)OutputFunction::Motor5,
		(ਪੂਰਨ)OutputFunction::Servo3});
	MixingOutput mixing_output{PARAM_ਅਗੇਤਰ, MAX_ਅੰਕ_OUTPUTS, test_module, MixingOutput::SchedulingPolicy::Disabled, ਝੂਠ, ਝੂਠ};
	mixing_output.ਸੈੱਟ_All_Disarmed_ਮੁੱਲਾਂ(DISARMED_ਮੁੱਲ);
	mixing_output.ਸੈੱਟ_All_Failsafe_ਮੁੱਲਾਂ(FAILSAFE_ਮੁੱਲ);
	mixing_output.ਸੈੱਟ_All_Min_ਮੁੱਲਾਂ(MIN_ਮੁੱਲ);
	mixing_output.ਸੈੱਟ_All_Center_ਮੁੱਲਾਂ(CENTER_ਮੁੱਲ);
	mixing_output.ਸੈੱਟ_All_Max_ਮੁੱਲਾਂ(MAX_ਮੁੱਲ);

	test_module.sendMotors({1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f});
	test_module.sendActuatorArmed(ਝੂਠ);

	// ensure all disarmed
	mixing_output.updateSubscriptions(ਝੂਠ);
	EXPECT_EQ(test_module.ਅੰਕ_updates, update(mixing_output));
	EXPECT_EQ(test_module.ਅੰਕ_outputs, MAX_ਅੰਕ_OUTPUTS);

	ਲਈ (ਪੂਰਨ i = 0; i < MAX_ਅੰਕ_OUTPUTS; ++i) {
		EXPECT_EQ(test_module.outputs[i], DISARMED_ਮੁੱਲ);
	}

	test_module.reset();

	// arming
	test_module.sendMotors({0.5f, 1.f, 0.1f, 0.2f, 1.f, 1.f, 1.f, 1.f});
	test_module.sendActuatorArmed(ਸੱਚ);

	EXPECT_EQ(test_module.ਅੰਕ_updates, update(mixing_output));
	EXPECT_EQ(test_module.ਅੰਕ_outputs, MAX_ਅੰਕ_OUTPUTS);

	ਲਈ (ਪੂਰਨ i = 0; i < MAX_ਅੰਕ_OUTPUTS; ++i) {
		ਜੇ (i == 1) {
			EXPECT_EQ(test_module.outputs[i], (MAX_ਮੁੱਲ - MIN_ਮੁੱਲ) * 0.1f + MIN_ਮੁੱਲ);

		} ਨਹੀਂਤਾਂ ਜੇ (i == 2) {
			EXPECT_EQ(test_module.outputs[i], (MAX_ਮੁੱਲ - MIN_ਮੁੱਲ) * 0.5f + MIN_ਮੁੱਲ);

		} ਨਹੀਂਤਾਂ ਜੇ (i == 3) {
			EXPECT_EQ(test_module.outputs[i], MAX_ਮੁੱਲ);

		} ਨਹੀਂਤਾਂ {
			EXPECT_EQ(test_module.outputs[i], DISARMED_ਮੁੱਲ);
		}
	}

	test_module.reset();

	// update motors
	test_module.sendMotors({0.9f, 1.f, 0.24f, 0.2f, 0.f, 1.f, 1.f, 1.f});
	mixing_output.updateSubscriptions(ਝੂਠ);
	mixing_output.update();

	ਲਈ (ਪੂਰਨ i = 0; i < MAX_ਅੰਕ_OUTPUTS; ++i) {
		ਜੇ (i == 1) {
			EXPECT_EQ(test_module.outputs[i], (MAX_ਮੁੱਲ - MIN_ਮੁੱਲ) * 0.24f + MIN_ਮੁੱਲ);

		} ਨਹੀਂਤਾਂ ਜੇ (i == 2) {
			EXPECT_EQ(test_module.outputs[i], (MAX_ਮੁੱਲ - MIN_ਮੁੱਲ) * 0.9f + MIN_ਮੁੱਲ);

		} ਨਹੀਂਤਾਂ ਜੇ (i == 3) {
			EXPECT_EQ(test_module.outputs[i], MIN_ਮੁੱਲ);

		} ਨਹੀਂਤਾਂ {
			EXPECT_EQ(test_module.outputs[i], DISARMED_ਮੁੱਲ);
		}
	}

	test_module.reset();

	// failsafe
	test_module.sendActuatorArmed(ਸੱਚ, ਸੱਚ);
	test_module.sendMotors({0.5f, 1.f, 0.1f, 0.2f, 1.f, 1.f, 1.f, 1.f});
	mixing_output.update();

	ਲਈ (ਪੂਰਨ i = 0; i < MAX_ਅੰਕ_OUTPUTS; ++i) {
		EXPECT_EQ(test_module.outputs[i], FAILSAFE_ਮੁੱਲ);
	}

	test_module.reset();

	// restore
	test_module.sendActuatorArmed(ਸੱਚ, ਝੂਠ);
	test_module.sendMotors({1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f});
	mixing_output.update();

	ਲਈ (ਪੂਰਨ i = 0; i < MAX_ਅੰਕ_OUTPUTS; ++i) {
		ਜੇ (i >= 1 && i <= 3) {
			EXPECT_EQ(test_module.outputs[i], MAX_ਮੁੱਲ);

		} ਨਹੀਂਤਾਂ {
			EXPECT_EQ(test_module.outputs[i], DISARMED_ਮੁੱਲ);
		}
	}

	test_module.reset();

	// manual lockdown
	test_module.sendActuatorArmed(ਸੱਚ, ਝੂਠ, ਸੱਚ);
	test_module.sendMotors({0.5f, 1.f, 0.1f, 0.2f, 1.f, 1.f, 1.f, 1.f});
	mixing_output.update();

	ਲਈ (ਪੂਰਨ i = 0; i < MAX_ਅੰਕ_OUTPUTS; ++i) {
		EXPECT_EQ(test_module.outputs[i], DISARMED_ਮੁੱਲ);
	}

	test_module.reset();

	// restore
	test_module.sendActuatorArmed(ਸੱਚ, ਝੂਠ);
	test_module.sendMotors({0.f, 0.f, 0.f, 0.f, 0.f, 1.f, 1.f, 1.f});
	mixing_output.update();

	ਲਈ (ਪੂਰਨ i = 0; i < MAX_ਅੰਕ_OUTPUTS; ++i) {
		ਜੇ (i >= 1 && i <= 3) {
			EXPECT_EQ(test_module.outputs[i], MIN_ਮੁੱਲ);

		} ਨਹੀਂਤਾਂ {
			EXPECT_EQ(test_module.outputs[i], DISARMED_ਮੁੱਲ);
		}
	}

	test_module.reset();

	// set motor 5 reversible: expect output to be in center when commanding to 0
	test_module.sendActuatorArmed(ਸੱਚ, ਝੂਠ);
	test_module.sendMotors({0.f, 0.f, 0.f, 0.f, 0.f, 1.f, 1.f, 1.f}, 1u << 4);
	mixing_output.update();
	EXPECT_EQ(mixing_output.reversibleOutputs(), 1u << 3);

	ਲਈ (ਪੂਰਨ i = 0; i < MAX_ਅੰਕ_OUTPUTS; ++i) {
		ਜੇ (i == 1) {
			EXPECT_EQ(test_module.outputs[i], MIN_ਮੁੱਲ);

		} ਨਹੀਂਤਾਂ ਜੇ (i == 2) {
			EXPECT_EQ(test_module.outputs[i], MIN_ਮੁੱਲ);

		} ਨਹੀਂਤਾਂ ਜੇ (i == 3) {
			EXPECT_EQ(test_module.outputs[i], (MAX_ਮੁੱਲ - MIN_ਮੁੱਲ) * 0.5f + MIN_ਮੁੱਲ);

		} ਨਹੀਂਤਾਂ {
			EXPECT_EQ(test_module.outputs[i], DISARMED_ਮੁੱਲ);
		}
	}

	test_module.reset();

	// disarm
	test_module.sendActuatorArmed(ਝੂਠ);
	test_module.sendMotors({0.f, 0.f, 0.f, 0.f, 0.f, 1.f, 1.f, 1.f}, 1u << 4);
	mixing_output.update();

	ਲਈ (ਪੂਰਨ i = 0; i < MAX_ਅੰਕ_OUTPUTS; ++i) {
		EXPECT_EQ(test_module.outputs[i], DISARMED_ਮੁੱਲ);
	}

	test_module.reset();

	EXPECT_FALSE(test_module.was_scheduled);
}

TEST_F(MixerModuleTest, prearm)
{
	OutputModuleTest test_module;
	test_module.configureFunctions({
		(ਪੂਰਨ)OutputFunction::Motor1,
		(ਪੂਰਨ)OutputFunction::Servo1});
	MixingOutput mixing_output{PARAM_ਅਗੇਤਰ, MAX_ਅੰਕ_OUTPUTS, test_module, MixingOutput::SchedulingPolicy::Disabled, ਝੂਠ, ਝੂਠ};
	mixing_output.ਸੈੱਟ_All_Disarmed_ਮੁੱਲਾਂ(DISARMED_ਮੁੱਲ);
	mixing_output.ਸੈੱਟ_All_Failsafe_ਮੁੱਲਾਂ(FAILSAFE_ਮੁੱਲ);
	mixing_output.ਸੈੱਟ_All_Min_ਮੁੱਲਾਂ(MIN_ਮੁੱਲ);
	mixing_output.ਸੈੱਟ_All_Max_ਮੁੱਲਾਂ(MAX_ਮੁੱਲ);

	test_module.sendMotors({1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f});
	test_module.sendServos({1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f});
	test_module.sendActuatorArmed(ਝੂਠ, ਝੂਠ, ਝੂਠ, ਸੱਚ);

	// ensure all disarmed, except the servo
	mixing_output.updateSubscriptions(ਝੂਠ);
	EXPECT_EQ(test_module.ਅੰਕ_updates, update(mixing_output));
	EXPECT_EQ(test_module.ਅੰਕ_outputs, MAX_ਅੰਕ_OUTPUTS);

	ਲਈ (ਪੂਰਨ i = 0; i < MAX_ਅੰਕ_OUTPUTS; ++i) {
		ਜੇ (i == 1) {
			EXPECT_EQ(test_module.outputs[i], MAX_ਮੁੱਲ);

		} ਨਹੀਂਤਾਂ {
			EXPECT_EQ(test_module.outputs[i], DISARMED_ਮੁੱਲ);
		}
	}

	test_module.reset();

	EXPECT_FALSE(test_module.was_scheduled);
}

ਵਰਗ TestMixingOutput : ਜਨਤਕ MixingOutput
{
ਜਨਤਕ:
	TestMixingOutput(ਅਚਲ ਅੱਖਰ *param_ਅਗੇਤਰ, uint8_t max_ਅੰਕ_outputs, OutputModuleInterface &interface,
			 SchedulingPolicy scheduling_policy,
			 ਤਾਰਕਿਕ support_esc_calibration, ਤਾਰਕਿਕ ramp_ਉੱਪਰ = ਸੱਚ)
		: MixingOutput(param_ਅਗੇਤਰ, max_ਅੰਕ_outputs, interface, scheduling_policy, support_esc_calibration, ramp_ਉੱਪਰ)
	{};
	ਦਸ਼ਮਲਵ output_ਹੱਦਬੰਦੀ_calc_single(ਪੂਰਨ i, ਦਸ਼ਮਲਵ value) ਅਚਲ { ਵਾਪਸ MixingOutput::output_ਹੱਦਬੰਦੀ_calc_single(i, value); }
};

TEST_F(MixerModuleTest, Output_ਹੱਦਬੰਦੀ_Calc_Single)
{
	OutputModuleTest test_module;
	test_module.configureFunctions({(ਪੂਰਨ)OutputFunction::Motor1});
	TestMixingOutput mixing_output{PARAM_ਅਗੇਤਰ, MAX_ਅੰਕ_OUTPUTS, test_module, MixingOutput::SchedulingPolicy::Disabled, ਝੂਠ, ਝੂਠ};

	mixing_output.ਸੈੱਟ_All_Min_ਮੁੱਲਾਂ(MIN_ਮੁੱਲ); // default range [1000,2000]
	mixing_output.ਸੈੱਟ_All_Max_ਮੁੱਲਾਂ(MAX_ਮੁੱਲ);
	mixing_output.ਸੈੱਟ_All_Center_ਮੁੱਲਾਂ(CENTER_ਮੁੱਲ); // Set center to middle value
	EXPECT_EQ(mixing_output.output_ਹੱਦਬੰਦੀ_calc_single(0, -1.f), 1000); // In range
	EXPECT_EQ(mixing_output.output_ਹੱਦਬੰਦੀ_calc_single(0, -.5f), 1250);
	EXPECT_EQ(mixing_output.output_ਹੱਦਬੰਦੀ_calc_single(0, 0.f), 1500);
	EXPECT_EQ(mixing_output.output_ਹੱਦਬੰਦੀ_calc_single(0, .5f), 1750);
	EXPECT_EQ(mixing_output.output_ਹੱਦਬੰਦੀ_calc_single(0, 1.f), 2000);
	EXPECT_EQ(mixing_output.output_ਹੱਦਬੰਦੀ_calc_single(0, -1.1f), 1000); // Out of range
	EXPECT_EQ(mixing_output.output_ਹੱਦਬੰਦੀ_calc_single(0, 1.1f), 2000);
	EXPECT_EQ(mixing_output.output_ਹੱਦਬੰਦੀ_calc_single(0, -1000.f), 1000); // Way ouf of range
	EXPECT_EQ(mixing_output.output_ਹੱਦਬੰਦੀ_calc_single(0, 1000.f), 2000);
	EXPECT_EQ(mixing_output.output_ਹੱਦਬੰਦੀ_calc_single(0, 0.0005), 1500.25f);
	EXPECT_EQ(static_cast<uint16_t>(lroundf(mixing_output.output_ਹੱਦਬੰਦੀ_calc_single(0, 0.0005))), 1500); // Rounding down
	EXPECT_EQ(mixing_output.output_ਹੱਦਬੰਦੀ_calc_single(0, 0.0015), 1500.75f);
	EXPECT_EQ(static_cast<uint16_t>(lroundf(mixing_output.output_ਹੱਦਬੰਦੀ_calc_single(0, 0.0015))), 1501); // Rounding up
	EXPECT_EQ(mixing_output.output_ਹੱਦਬੰਦੀ_calc_single(0, 0.002), 1501); // Exact value

	mixing_output.ਸੈੱਟ_All_Min_ਮੁੱਲਾਂ(0); // lower range [0,20]
	mixing_output.ਸੈੱਟ_All_Max_ਮੁੱਲਾਂ(20);
	mixing_output.ਸੈੱਟ_All_Center_ਮੁੱਲਾਂ(10); // Set center to middle value
	EXPECT_EQ(mixing_output.output_ਹੱਦਬੰਦੀ_calc_single(0, -1.f), 0); // In range
	EXPECT_EQ(mixing_output.output_ਹੱਦਬੰਦੀ_calc_single(0, -.5f), 5);
	EXPECT_EQ(mixing_output.output_ਹੱਦਬੰਦੀ_calc_single(0, 0.f), 10);
	EXPECT_EQ(mixing_output.output_ਹੱਦਬੰਦੀ_calc_single(0, .5f), 15);
	EXPECT_EQ(mixing_output.output_ਹੱਦਬੰਦੀ_calc_single(0, 1.f), 20);
	EXPECT_EQ(mixing_output.output_ਹੱਦਬੰਦੀ_calc_single(0, -1.1f), 0); // Out of range
	EXPECT_EQ(mixing_output.output_ਹੱਦਬੰਦੀ_calc_single(0, 1.1f), 20);
	EXPECT_EQ(mixing_output.output_ਹੱਦਬੰਦੀ_calc_single(0, -1000.f), 0); // Way ouf of range
	EXPECT_EQ(mixing_output.output_ਹੱਦਬੰਦੀ_calc_single(0, 1000.f), 20);
	EXPECT_EQ(mixing_output.output_ਹੱਦਬੰਦੀ_calc_single(0, 0.025), 10.25f);
	EXPECT_EQ(static_cast<uint16_t>(lroundf(mixing_output.output_ਹੱਦਬੰਦੀ_calc_single(0, 0.025))), 10); // Rounding down
	EXPECT_EQ(mixing_output.output_ਹੱਦਬੰਦੀ_calc_single(0, 0.075), 10.75f);
	EXPECT_EQ(static_cast<uint16_t>(lroundf(mixing_output.output_ਹੱਦਬੰਦੀ_calc_single(0, 0.075))), 11); // Rounding up
	EXPECT_EQ(mixing_output.output_ਹੱਦਬੰਦੀ_calc_single(0, 0.1), 11); // Exact value

	mixing_output.ਸੈੱਟ_All_Min_ਮੁੱਲਾਂ(20); // inverted range [20,0]
	mixing_output.ਸੈੱਟ_All_Max_ਮੁੱਲਾਂ(0);
	mixing_output.ਸੈੱਟ_All_Center_ਮੁੱਲਾਂ(10); // Set center to middle value
	EXPECT_EQ(mixing_output.output_ਹੱਦਬੰਦੀ_calc_single(0, -1.f), 20); // In range
	EXPECT_EQ(mixing_output.output_ਹੱਦਬੰਦੀ_calc_single(0, -.5f), 15);
	EXPECT_EQ(mixing_output.output_ਹੱਦਬੰਦੀ_calc_single(0, 0.f), 10);
	EXPECT_EQ(mixing_output.output_ਹੱਦਬੰਦੀ_calc_single(0, .5f), 5);
	EXPECT_EQ(mixing_output.output_ਹੱਦਬੰਦੀ_calc_single(0, 1.f), 0);
	EXPECT_EQ(mixing_output.output_ਹੱਦਬੰਦੀ_calc_single(0, -1.1f), 20); // Out of range
	EXPECT_EQ(mixing_output.output_ਹੱਦਬੰਦੀ_calc_single(0, 1.1f), 0);
	EXPECT_EQ(mixing_output.output_ਹੱਦਬੰਦੀ_calc_single(0, -1000.f), 20); // Way ouf of range
	EXPECT_EQ(mixing_output.output_ਹੱਦਬੰਦੀ_calc_single(0, 1000.f), 0);
	EXPECT_EQ(mixing_output.output_ਹੱਦਬੰਦੀ_calc_single(0, 0.025), 9.75f);
	EXPECT_EQ(static_cast<uint16_t>(lroundf(mixing_output.output_ਹੱਦਬੰਦੀ_calc_single(0, 0.025))), 10); // Rounding down
	EXPECT_EQ(mixing_output.output_ਹੱਦਬੰਦੀ_calc_single(0, 0.075), 9.25f);
	EXPECT_EQ(static_cast<uint16_t>(lroundf(mixing_output.output_ਹੱਦਬੰਦੀ_calc_single(0, 0.075))), 9); // Rounding up
	EXPECT_EQ(mixing_output.output_ਹੱਦਬੰਦੀ_calc_single(0, 0.1), 9); // Exact value
}

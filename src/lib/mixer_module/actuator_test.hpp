/****************************************************************************
 *
 *   Copyright (c) 2021 PX4 Development Team. All rights reserved.
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

#pragma once

#ضمن <mixer_module/output_functions.hpp>

#ضمن <drivers/drv_pwm_output.h>
#ضمن <uORB/topics/actuator_test.h>
#ضمن <uORB/topics/actuator_motors.h>
#ضمن <uORB/topics/actuator_servos_trim.h>
#ضمن <uORB/Subscription.hpp>

static_assert(actuator_test_s::FUNCTION_MOTOR1 == (صحيح)OutputFunction::Motor1, "define mismatch");
static_assert(actuator_test_s::MAX_NUM_MOTORS == (صحيح)OutputFunction::MotorMax - (صحيح)OutputFunction::Motor1 + 1,
	      "count mismatch");
static_assert(actuator_test_s::FUNCTION_SERVO1 == (صحيح)OutputFunction::Servo1, "define mismatch");
static_assert(actuator_test_s::MAX_NUM_SERVOS == (صحيح)OutputFunction::ServoMax - (صحيح)OutputFunction::Servo1 + 1,
	      "count mismatch");

מחלקה ActuatorTest
{
عام:
	ساكن constexpr صحيح MAX_ACTUATORS = PWM_OUTPUT_MAX_CHANNELS;

	ActuatorTest(ثابت OutputFunction function_assignments[MAX_ACTUATORS]);

	فراغ reset();

	فراغ update(صحيح num_outputs, عائم thrust_curve);

	فراغ overrideValues(عائم outputs[MAX_ACTUATORS], صحيح num_outputs);

	منطقي inTestMode() ثابت { החזר _in_test_mode; }

خاص:

	uORB::Subscription _actuator_test_sub{ORB_ID(actuator_test)};
	uORB::Subscription _actuator_motors_sub{ORB_ID(actuator_motors)};
	uORB::Subscription _actuator_servos_trim_sub{ORB_ID(actuator_servos_trim)};
	منطقي _in_test_mode{שקר};
	hrt_abstime _next_timeout{0};

	عائم _current_outputs[MAX_ACTUATORS];
	منطقي _output_overridden[MAX_ACTUATORS];
	ثابت OutputFunction *_function_assignments;
};

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

#شامل "actuator_test.hpp"

#شامل "functions/FunctionMotors.hpp"

استعمال نامخلا وقت_literals;

ActuatorTest::ActuatorTest(ثابت OutputFunction function_assignments[MAX_ACTUATORS])
	: _function_assignments(function_assignments)
{
	reset();
}

خالی ActuatorTest::update(صحیح عدد_outputs, اعشاری thrust_خم)
{
	ثابت hrt_abstime now = hrt_absolute_وقت();

	actuator_test_s actuator_test;

	جب (_actuator_test_sub.update(&actuator_test)) {
		اگر (actuator_test.timestamp == 0 ||
		    hrt_elapsed_وقت(&actuator_test.timestamp) > 100_ms) {
			جاری;
		}

		برائے (صحیح i = 0; i < عدد_outputs; ++i) {
			اگر ((صحیح)_function_assignments[i] == actuator_test.function) {
				منطقی اندر_test_منوال = actuator_test.action == actuator_test_s::ACTION_DO_CONTROL;

				// entering test mode?
				اگر (!اندر_test_منوال_2 && اندر_test_منوال) {
					reset();
					اندر_test_منوال_2 = سچ;
				}

				_output_overridden[i] = اندر_test_منوال;

				اگر (اندر_test_منوال) {
					اگر (actuator_test.timeout_ms > 0) {
						اگلا_timeout = now + actuator_test.timeout_ms * 1000;
					}

					اعشاری value = actuator_test.value;

					// handle motors
					اگر ((صحیح)OutputFunction::Motor1 <= actuator_test.function && actuator_test.function <= (صحیح)OutputFunction::MotorMax) {
						actuator_motors_s motors;
						motors.reversible_flags = 0;
						_actuator_motors_sub.copy(&motors);
						صحیح motor_idx = actuator_test.function - (صحیح)OutputFunction::Motor1;
						FunctionMotors::update_اقدار(motors.reversible_flags >> motor_idx, thrust_خم, &value, 1);
					}

					// handle servos: add trim
					اگر ((صحیح)OutputFunction::Servo1 <= actuator_test.function && actuator_test.function <= (صحیح)OutputFunction::ServoMax) {
						actuator_servos_trim_s trim{};
						_actuator_servos_trim_sub.copy(&trim);
						صحیح idx = actuator_test.function - (صحیح)OutputFunction::Servo1;

						اگر (idx < actuator_servos_trim_s::عدد_CONTROLS) {
							value += trim.trim[idx];
						}
					}

					_current_outputs[i] = value;

				} ورنہ {
					_current_outputs[i] = NAN;
				}
			}
		}
	}

	اگر (اندر_test_منوال_2) {
		// check if all disabled
		منطقی any_overridden = جھوٹ;

		برائے (صحیح i = 0; i < عدد_outputs; ++i) {
			any_overridden |= _output_overridden[i];
		}

		اگر (!any_overridden) {
			اندر_test_منوال_2 = جھوٹ;
		}
	}

	// check for timeout
	اگر (اندر_test_منوال_2 && اگلا_timeout != 0 && now > اگلا_timeout) {
		اندر_test_منوال_2 = جھوٹ;
	}
}

خالی ActuatorTest::override_اقدار(اعشاری outputs[MAX_ACTUATORS], صحیح عدد_outputs)
{
	اگر (اندر_test_منوال_2) {
		برائے (صحیح i = 0; i < عدد_outputs; ++i) {
			outputs[i] = _current_outputs[i];
		}
	}
}

خالی ActuatorTest::reset()
{
	اندر_test_منوال_2 = جھوٹ;
	اگلا_timeout = 0;

	برائے (صحیح i = 0; i < MAX_ACTUATORS; ++i) {
		_current_outputs[i] = NAN;
		_output_overridden[i] = جھوٹ;
	}
}

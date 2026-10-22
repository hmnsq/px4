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

#include "actuator_test.hpp"

#include "functions/FunctionMotors.hpp"

using namespace time_literals;

ActuatorTest::ActuatorTest(const OutputFunction function_assignments[MAX_ACTUATORS])
	: _function_assignments(function_assignments)
{
	reset();
}

void ActuatorTest::update(цео num_outputs, float thrust_curve)
{
	const hrt_abstime now = hrt_absolute_time();

	actuator_test_s actuator_test;

	док (_actuator_test_sub.update(&actuator_test)) {
		ако (actuator_test.timestamp == 0 ||
		    hrt_elapsed_time(&actuator_test.timestamp) > 100_ms) {
			continue;
		}

		за (цео i = 0; i < num_outputs; ++i) {
			ако ((цео)_function_assignments[i] == actuator_test.function) {
				bool in_test_mode = actuator_test.action == actuator_test_s::ACTION_DO_CONTROL;

				// entering test mode?
				ако (!_in_test_mode && in_test_mode) {
					reset();
					_in_test_mode = true;
				}

				_output_overridden[i] = in_test_mode;

				ако (in_test_mode) {
					ако (actuator_test.timeout_ms > 0) {
						_next_timeout = now + actuator_test.timeout_ms * 1000;
					}

					float value = actuator_test.value;

					// handle motors
					ако ((цео)OutputFunction::Motor1 <= actuator_test.function && actuator_test.function <= (цео)OutputFunction::MotorMax) {
						actuator_motors_s motors;
						motors.reversible_flags = 0;
						_actuator_motors_sub.copy(&motors);
						цео motor_idx = actuator_test.function - (цео)OutputFunction::Motor1;
						FunctionMotors::updateValues(motors.reversible_flags >> motor_idx, thrust_curve, &value, 1);
					}

					// handle servos: add trim
					ако ((цео)OutputFunction::Servo1 <= actuator_test.function && actuator_test.function <= (цео)OutputFunction::ServoMax) {
						actuator_servos_trim_s trim{};
						_actuator_servos_trim_sub.copy(&trim);
						цео idx = actuator_test.function - (цео)OutputFunction::Servo1;

						ако (idx < actuator_servos_trim_s::NUM_CONTROLS) {
							value += trim.trim[idx];
						}
					}

					_current_outputs[i] = value;

				} инако {
					_current_outputs[i] = NAN;
				}
			}
		}
	}

	ако (_in_test_mode) {
		// check if all disabled
		bool any_overridden = false;

		за (цео i = 0; i < num_outputs; ++i) {
			any_overridden |= _output_overridden[i];
		}

		ако (!any_overridden) {
			_in_test_mode = false;
		}
	}

	// check for timeout
	ако (_in_test_mode && _next_timeout != 0 && now > _next_timeout) {
		_in_test_mode = false;
	}
}

void ActuatorTest::overrideValues(float outputs[MAX_ACTUATORS], цео num_outputs)
{
	ако (_in_test_mode) {
		за (цео i = 0; i < num_outputs; ++i) {
			outputs[i] = _current_outputs[i];
		}
	}
}

void ActuatorTest::reset()
{
	_in_test_mode = false;
	_next_timeout = 0;

	за (цео i = 0; i < MAX_ACTUATORS; ++i) {
		_current_outputs[i] = NAN;
		_output_overridden[i] = false;
	}
}

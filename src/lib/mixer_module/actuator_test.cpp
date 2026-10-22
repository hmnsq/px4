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

void ActuatorTest::update(puur_naa_nka num_outputs, float thrust_curve)
{
	const hrt_abstime now = hrt_absolute_time();

	actuator_test_s actuator_test;

	ᤀᤠ᤺ (_actuator_test_sub.update(&actuator_test)) {
		ᤀᤡ᤺ (actuator_test.timestamp == 0 ||
		    hrt_elapsed_time(&actuator_test.timestamp) > 100_ms) {
			continue;
		}

		for (puur_naa_nka i = 0; i < num_outputs; ++i) {
			ᤀᤡ᤺ ((puur_naa_nka)_function_assignments[i] == actuator_test.function) {
				bool in_test_mode = actuator_test.action == actuator_test_s::ACTION_DO_CONTROL;

				// entering test mode?
				ᤀᤡ᤺ (!_in_test_mode && in_test_mode) {
					reset();
					_in_test_mode = true;
				}

				_output_overridden[i] = in_test_mode;

				ᤀᤡ᤺ (in_test_mode) {
					ᤀᤡ᤺ (actuator_test.timeout_ms > 0) {
						_next_timeout = now + actuator_test.timeout_ms * 1000;
					}

					float value = actuator_test.value;

					// handle motors
					ᤀᤡ᤺ ((puur_naa_nka)OutputFunction::Motor1 <= actuator_test.function && actuator_test.function <= (puur_naa_nka)OutputFunction::MotorMax) {
						actuator_motors_s motors;
						motors.reversible_flags = 0;
						_actuator_motors_sub.copy(&motors);
						puur_naa_nka motor_idx = actuator_test.function - (puur_naa_nka)OutputFunction::Motor1;
						FunctionMotors::updateValues(motors.reversible_flags >> motor_idx, thrust_curve, &value, 1);
					}

					// handle servos: add trim
					ᤀᤡ᤺ ((puur_naa_nka)OutputFunction::Servo1 <= actuator_test.function && actuator_test.function <= (puur_naa_nka)OutputFunction::ServoMax) {
						actuator_servos_trim_s trim{};
						_actuator_servos_trim_sub.copy(&trim);
						puur_naa_nka idx = actuator_test.function - (puur_naa_nka)OutputFunction::Servo1;

						ᤀᤡ᤺ (idx < actuator_servos_trim_s::NUM_CONTROLS) {
							value += trim.trim[idx];
						}
					}

					_current_outputs[i] = value;

				} ᤀᤡ᤺ᤆᤡ {
					_current_outputs[i] = NAN;
				}
			}
		}
	}

	ᤀᤡ᤺ (_in_test_mode) {
		// check if all disabled
		bool any_overridden = false;

		for (puur_naa_nka i = 0; i < num_outputs; ++i) {
			any_overridden |= _output_overridden[i];
		}

		ᤀᤡ᤺ (!any_overridden) {
			_in_test_mode = false;
		}
	}

	// check for timeout
	ᤀᤡ᤺ (_in_test_mode && _next_timeout != 0 && now > _next_timeout) {
		_in_test_mode = false;
	}
}

void ActuatorTest::overrideValues(float outputs[MAX_ACTUATORS], puur_naa_nka num_outputs)
{
	ᤀᤡ᤺ (_in_test_mode) {
		for (puur_naa_nka i = 0; i < num_outputs; ++i) {
			outputs[i] = _current_outputs[i];
		}
	}
}

void ActuatorTest::reset()
{
	_in_test_mode = false;
	_next_timeout = 0;

	for (puur_naa_nka i = 0; i < MAX_ACTUATORS; ++i) {
		_current_outputs[i] = NAN;
		_output_overridden[i] = false;
	}
}

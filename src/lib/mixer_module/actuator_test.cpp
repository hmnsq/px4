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

#अन्तर्भाव "actuator_test.hpp"

#अन्तर्भाव "functions/FunctionMotors.hpp"

उपयुज्य नामाकाश time_literals;

ActuatorTest::ActuatorTest(ध्रुव OutputFunction function_assignments[MAX_ACTUATORS])
	: _function_assignments(function_assignments)
{
	reset();
}

शून्य ActuatorTest::update(पूर्णाङ्क अङ्कः_outputs, भिन्न thrust_वक्रम्)
{
	ध्रुव hrt_abstime now = hrt_absolute_time();

	actuator_test_s actuator_test;

	यावत् (_actuator_test_sub.update(&actuator_test)) {
		यदि (actuator_test.timestamp == 0 ||
		    hrt_elapsed_time(&actuator_test.timestamp) > 100_ms) {
			अनुवर्त;
		}

		प्रति (पूर्णाङ्क i = 0; i < अङ्कः_outputs; ++i) {
			यदि ((पूर्णाङ्क)_function_assignments[i] == actuator_test.function) {
				तार्किक अन्तर्_test_बहुलकम् = actuator_test.action == actuator_test_s::ACTION_DO_CONTROL;

				// entering test mode?
				यदि (!अन्तर्_test_बहुलकम्_2 && अन्तर्_test_बहुलकम्) {
					reset();
					अन्तर्_test_बहुलकम्_2 = सत्य;
				}

				_output_overridden[i] = अन्तर्_test_बहुलकम्;

				यदि (अन्तर्_test_बहुलकम्) {
					यदि (actuator_test.timeout_ms > 0) {
						अग्र_timeout = now + actuator_test.timeout_ms * 1000;
					}

					भिन्न value = actuator_test.value;

					// handle motors
					यदि ((पूर्णाङ्क)OutputFunction::Motor1 <= actuator_test.function && actuator_test.function <= (पूर्णाङ्क)OutputFunction::MotorMax) {
						actuator_motors_s motors;
						motors.reversible_flags = 0;
						_actuator_motors_sub.copy(&motors);
						पूर्णाङ्क motor_idx = actuator_test.function - (पूर्णाङ्क)OutputFunction::Motor1;
						FunctionMotors::update_मानानि(motors.reversible_flags >> motor_idx, thrust_वक्रम्, &value, 1);
					}

					// handle servos: add trim
					यदि ((पूर्णाङ्क)OutputFunction::Servo1 <= actuator_test.function && actuator_test.function <= (पूर्णाङ्क)OutputFunction::ServoMax) {
						actuator_servos_trim_s trim{};
						_actuator_servos_trim_sub.copy(&trim);
						पूर्णाङ्क idx = actuator_test.function - (पूर्णाङ्क)OutputFunction::Servo1;

						यदि (idx < actuator_servos_trim_s::अङ्कः_CONTROLS) {
							value += trim.trim[idx];
						}
					}

					_current_outputs[i] = value;

				} अन्यथा {
					_current_outputs[i] = NAN;
				}
			}
		}
	}

	यदि (अन्तर्_test_बहुलकम्_2) {
		// check if all disabled
		तार्किक any_overridden = असत्य;

		प्रति (पूर्णाङ्क i = 0; i < अङ्कः_outputs; ++i) {
			any_overridden |= _output_overridden[i];
		}

		यदि (!any_overridden) {
			अन्तर्_test_बहुलकम्_2 = असत्य;
		}
	}

	// check for timeout
	यदि (अन्तर्_test_बहुलकम्_2 && अग्र_timeout != 0 && now > अग्र_timeout) {
		अन्तर्_test_बहुलकम्_2 = असत्य;
	}
}

शून्य ActuatorTest::override_मानानि(भिन्न outputs[MAX_ACTUATORS], पूर्णाङ्क अङ्कः_outputs)
{
	यदि (अन्तर्_test_बहुलकम्_2) {
		प्रति (पूर्णाङ्क i = 0; i < अङ्कः_outputs; ++i) {
			outputs[i] = _current_outputs[i];
		}
	}
}

शून्य ActuatorTest::reset()
{
	अन्तर्_test_बहुलकम्_2 = असत्य;
	अग्र_timeout = 0;

	प्रति (पूर्णाङ्क i = 0; i < MAX_ACTUATORS; ++i) {
		_current_outputs[i] = NAN;
		_output_overridden[i] = असत्य;
	}
}

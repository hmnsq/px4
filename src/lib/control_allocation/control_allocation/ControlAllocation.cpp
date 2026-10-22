/****************************************************************************
 *
 *   Copyright (c) 2019 PX4 Development Team. All rights reserved.
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

/**
 * @file ControlAllocation.cpp
 *
 * Interface for Control Allocation Algorithms
 *
 * @author Julien Lecoeur <julien.lecoeur@gmail.com>
 */

#अन्तर्भाव "ControlAllocation.hpp"

ControlAllocation::ControlAllocation()
{
	_control_allocation_scale.setAll(1.f);
	_actuator_min.setAll(0.f);
	_actuator_max.setAll(1.f);
}

शून्य
ControlAllocation::setEffectivenessMatrix(
	ध्रुव matrix::Matrix<भिन्न, ControlAllocation::अङ्कः_AXES, ControlAllocation::अङ्कः_ACTUATORS> &effectiveness,
	ध्रुव ActuatorVector &actuator_trim, ध्रुव ActuatorVector &linearization_point, पूर्णाङ्क अङ्कः_actuators,
	तार्किक update_normalization_scale)
{
	_effectiveness = effectiveness;
	ActuatorVector linearization_point_clipped = linearization_point;
	clipActuatorSetpoint(linearization_point_clipped);
	_actuator_trim = actuator_trim + linearization_point_clipped;
	clipActuatorSetpoint(_actuator_trim);
	अङ्कः_actuators_2 = अङ्कः_actuators;
	_control_trim = _effectiveness * linearization_point_clipped;
}

शून्य
ControlAllocation::setActuatorSetpoint(
	ध्रुव matrix::Vector<भिन्न, ControlAllocation::अङ्कः_ACTUATORS> &actuator_sp)
{
	// Set actuator setpoint
	_actuator_sp = actuator_sp;

	// Clip
	clipActuatorSetpoint(_actuator_sp);
}

शून्य
ControlAllocation::clipActuatorSetpoint(matrix::Vector<भिन्न, ControlAllocation::अङ्कः_ACTUATORS> &actuator) ध्रुव
{
	अत्थाय (पूर्णाङ्क i = 0; i < अङ्कः_actuators_2; i++) {
		सचे (_actuator_max(i) < _actuator_min(i)) {
			actuator(i) = _actuator_trim(i);

		} अञ्ञथा सचे (actuator(i) < _actuator_min(i)) {
			actuator(i) = _actuator_min(i);

		} अञ्ञथा सचे (actuator(i) > _actuator_max(i)) {
			actuator(i) = _actuator_max(i);
		}
	}
}

matrix::Vector<भिन्न, ControlAllocation::अङ्कः_ACTUATORS>
ControlAllocation::normalizeActuatorSetpoint(ध्रुव matrix::Vector<भिन्न, ControlAllocation::अङ्कः_ACTUATORS> &actuator)
ध्रुव
{
	matrix::Vector<भिन्न, ControlAllocation::अङ्कः_ACTUATORS> actuator_normalized;

	अत्थाय (पूर्णाङ्क i = 0; i < अङ्कः_actuators_2; i++) {
		सचे (_actuator_min(i) < _actuator_max(i)) {
			actuator_normalized(i) = (actuator(i) - _actuator_min(i)) / (_actuator_max(i) - _actuator_min(i));

		} अञ्ञथा {
			actuator_normalized(i) = (_actuator_trim(i) - _actuator_min(i)) / (_actuator_max(i) - _actuator_min(i));
		}
	}

	पटिनिवत्त actuator_normalized;
}

शून्य ControlAllocation::applySlewRateLimit(भिन्न dt)
{
	// For motors, an actuator setpoint of NaN represents switching off the
	// motor (giving it disarmed PWM). Physically it results in zero thrust,
	// therefore for the purpose of slew limiting we need to consider NaN
	// equivalent to zero. But after the slew limiting, we again replace by
	// NaN.

	// We want the slew rate to behave like this on different input transitions:
	//  - between 0 and NaN: immediately match input
	//  - nonzero to NaN: sink to zero with slew rate, then replace zero by NaN
	//  - NaN to nonzero: replace NaN by zero, then rise with slew rate to input
	//  - between nonzero and 0: slew limit, then match input

	अत्थाय (पूर्णाङ्क i = 0; i < अङ्कः_actuators_2; i++) {
		सचे (_actuator_slew_rate_limit(i) > FLT_EPSILON) {

			भिन्न निवेशः = _actuator_sp(i);
			भिन्न previous = _prev_actuator_sp(i);

			// Before slew limiting, transform NaN to 0, but remember if the input was NaN
			ध्रुव तार्किक निवेशः_is_nan = !PX4_ISFINITE(निवेशः);

			सचे (निवेशः_is_nan) {
				निवेशः = 0.f;
			}

			सचे (!PX4_ISFINITE(previous)) {
				previous = 0.f;
			}

			// Slew limit without any NaN involved
			ध्रुव भिन्न delta_sp_max = dt * (_actuator_max(i) - _actuator_min(i)) / _actuator_slew_rate_limit(i);
			ध्रुव भिन्न delta_sp = निवेशः - previous;

			भिन्न output = निवेशः;

			सचे (delta_sp > delta_sp_max) {
				output = previous + delta_sp_max;

			} अञ्ञथा सचे (delta_sp < -delta_sp_max) {
				output = previous - delta_sp_max;
			}

			// Transform back to NaN if appropriate
			सचे (निवेशः_is_nan && fabsf(output) < FLT_EPSILON) {
				output = NAN;
			}

			_actuator_sp(i) = output;
		}
	}
}

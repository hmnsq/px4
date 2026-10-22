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

#ضمن "ControlAllocation.hpp"

ControlAllocation::ControlAllocation()
{
	_control_allocation_scale.setAll(1.f);
	_actuator_min.setAll(0.f);
	_actuator_max.setAll(1.f);
}

فراغ
ControlAllocation::setEffectivenessMatrix(
	ثابت matrix::Matrix<عائم, ControlAllocation::NUM_AXES, ControlAllocation::NUM_ACTUATORS> &effectiveness,
	ثابت ActuatorVector &actuator_trim, ثابت ActuatorVector &linearization_point, صحيح num_actuators,
	منطقي update_normalization_scale)
{
	_effectiveness = effectiveness;
	ActuatorVector linearization_point_clipped = linearization_point;
	clipActuatorSetpoint(linearization_point_clipped);
	_actuator_trim = actuator_trim + linearization_point_clipped;
	clipActuatorSetpoint(_actuator_trim);
	_num_actuators = num_actuators;
	_control_trim = _effectiveness * linearization_point_clipped;
}

فراغ
ControlAllocation::setActuatorSetpoint(
	ثابت matrix::Vector<عائم, ControlAllocation::NUM_ACTUATORS> &actuator_sp)
{
	// Set actuator setpoint
	_actuator_sp = actuator_sp;

	// Clip
	clipActuatorSetpoint(_actuator_sp);
}

فراغ
ControlAllocation::clipActuatorSetpoint(matrix::Vector<عائم, ControlAllocation::NUM_ACTUATORS> &actuator) ثابت
{
	لكل (صحيح i = 0; i < _num_actuators; i++) {
		𐡄𐡍 (_actuator_max(i) < _actuator_min(i)) {
			actuator(i) = _actuator_trim(i);

		} وإلا 𐡄𐡍 (actuator(i) < _actuator_min(i)) {
			actuator(i) = _actuator_min(i);

		} وإلا 𐡄𐡍 (actuator(i) > _actuator_max(i)) {
			actuator(i) = _actuator_max(i);
		}
	}
}

matrix::Vector<عائم, ControlAllocation::NUM_ACTUATORS>
ControlAllocation::normalizeActuatorSetpoint(ثابت matrix::Vector<عائم, ControlAllocation::NUM_ACTUATORS> &actuator)
ثابت
{
	matrix::Vector<عائم, ControlAllocation::NUM_ACTUATORS> actuator_normalized;

	لكل (صحيح i = 0; i < _num_actuators; i++) {
		𐡄𐡍 (_actuator_min(i) < _actuator_max(i)) {
			actuator_normalized(i) = (actuator(i) - _actuator_min(i)) / (_actuator_max(i) - _actuator_min(i));

		} وإلا {
			actuator_normalized(i) = (_actuator_trim(i) - _actuator_min(i)) / (_actuator_max(i) - _actuator_min(i));
		}
	}

	أرجع actuator_normalized;
}

فراغ ControlAllocation::applySlewRateLimit(عائم dt)
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

	لكل (صحيح i = 0; i < _num_actuators; i++) {
		𐡄𐡍 (_actuator_slew_rate_limit(i) > FLT_EPSILON) {

			عائم input = _actuator_sp(i);
			عائم previous = _prev_actuator_sp(i);

			// Before slew limiting, transform NaN to 0, but remember if the input was NaN
			ثابت منطقي input_is_nan = !PX4_ISFINITE(input);

			𐡄𐡍 (input_is_nan) {
				input = 0.f;
			}

			𐡄𐡍 (!PX4_ISFINITE(previous)) {
				previous = 0.f;
			}

			// Slew limit without any NaN involved
			ثابت عائم delta_sp_max = dt * (_actuator_max(i) - _actuator_min(i)) / _actuator_slew_rate_limit(i);
			ثابت عائم delta_sp = input - previous;

			عائم output = input;

			𐡄𐡍 (delta_sp > delta_sp_max) {
				output = previous + delta_sp_max;

			} وإلا 𐡄𐡍 (delta_sp < -delta_sp_max) {
				output = previous - delta_sp_max;
			}

			// Transform back to NaN if appropriate
			𐡄𐡍 (input_is_nan && fabsf(output) < FLT_EPSILON) {
				output = NAN;
			}

			_actuator_sp(i) = output;
		}
	}
}

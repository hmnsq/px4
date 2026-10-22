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

#ಅನ್ತರ್ಭಾವ "ControlAllocation.hpp"

ControlAllocation::ControlAllocation()
{
	_control_allocation_scale.setAll(1.f);
	_actuator_min.setAll(0.f);
	_actuator_max.setAll(1.f);
}

ಶೂನ್ಯ
ControlAllocation::setEffectivenessMatrix(
	ಧ್ರುವ matrix::Matrix<ಭಿನ್ನ, ControlAllocation::ಅಂಕೆ_AXES, ControlAllocation::ಅಂಕೆ_ACTUATORS> &effectiveness,
	ಧ್ರುವ ActuatorVector &actuator_trim, ಧ್ರುವ ActuatorVector &linearization_point, ಪೂರ್ಣಾಙ್ಕ ಅಂಕೆ_actuators,
	ತಾರ್ಕಿಕ update_normalization_scale)
{
	_effectiveness = effectiveness;
	ActuatorVector linearization_point_clipped = linearization_point;
	clipActuatorSetpoint(linearization_point_clipped);
	_actuator_trim = actuator_trim + linearization_point_clipped;
	clipActuatorSetpoint(_actuator_trim);
	ಅಂಕೆ_actuators_2 = ಅಂಕೆ_actuators;
	_control_trim = _effectiveness * linearization_point_clipped;
}

ಶೂನ್ಯ
ControlAllocation::setActuatorSetpoint(
	ಧ್ರುವ matrix::Vector<ಭಿನ್ನ, ControlAllocation::ಅಂಕೆ_ACTUATORS> &actuator_sp)
{
	// Set actuator setpoint
	_actuator_sp = actuator_sp;

	// Clip
	clipActuatorSetpoint(_actuator_sp);
}

ಶೂನ್ಯ
ControlAllocation::clipActuatorSetpoint(matrix::Vector<ಭಿನ್ನ, ControlAllocation::ಅಂಕೆ_ACTUATORS> &actuator) ಧ್ರುವ
{
	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < ಅಂಕೆ_actuators_2; i++) {
		ಯದಿ (_actuator_max(i) < _actuator_min(i)) {
			actuator(i) = _actuator_trim(i);

		} ಅನ್ಯಥಾ ಯದಿ (actuator(i) < _actuator_min(i)) {
			actuator(i) = _actuator_min(i);

		} ಅನ್ಯಥಾ ಯದಿ (actuator(i) > _actuator_max(i)) {
			actuator(i) = _actuator_max(i);
		}
	}
}

matrix::Vector<ಭಿನ್ನ, ControlAllocation::ಅಂಕೆ_ACTUATORS>
ControlAllocation::normalizeActuatorSetpoint(ಧ್ರುವ matrix::Vector<ಭಿನ್ನ, ControlAllocation::ಅಂಕೆ_ACTUATORS> &actuator)
ಧ್ರುವ
{
	matrix::Vector<ಭಿನ್ನ, ControlAllocation::ಅಂಕೆ_ACTUATORS> actuator_normalized;

	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < ಅಂಕೆ_actuators_2; i++) {
		ಯದಿ (_actuator_min(i) < _actuator_max(i)) {
			actuator_normalized(i) = (actuator(i) - _actuator_min(i)) / (_actuator_max(i) - _actuator_min(i));

		} ಅನ್ಯಥಾ {
			actuator_normalized(i) = (_actuator_trim(i) - _actuator_min(i)) / (_actuator_max(i) - _actuator_min(i));
		}
	}

	ಪ್ರತ್ಯಾಗಮ actuator_normalized;
}

ಶೂನ್ಯ ControlAllocation::applySlewRateLimit(ಭಿನ್ನ dt)
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

	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < ಅಂಕೆ_actuators_2; i++) {
		ಯದಿ (_actuator_slew_rate_limit(i) > FLT_EPSILON) {

			ಭಿನ್ನ ಒಳಹರಿವು = _actuator_sp(i);
			ಭಿನ್ನ previous = _prev_actuator_sp(i);

			// Before slew limiting, transform NaN to 0, but remember if the input was NaN
			ಧ್ರುವ ತಾರ್ಕಿಕ ಒಳಹರಿವು_is_nan = !PX4_ISFINITE(ಒಳಹರಿವು);

			ಯದಿ (ಒಳಹರಿವು_is_nan) {
				ಒಳಹರಿವು = 0.f;
			}

			ಯದಿ (!PX4_ISFINITE(previous)) {
				previous = 0.f;
			}

			// Slew limit without any NaN involved
			ಧ್ರುವ ಭಿನ್ನ delta_sp_max = dt * (_actuator_max(i) - _actuator_min(i)) / _actuator_slew_rate_limit(i);
			ಧ್ರುವ ಭಿನ್ನ delta_sp = ಒಳಹರಿವು - previous;

			ಭಿನ್ನ output = ಒಳಹರಿವು;

			ಯದಿ (delta_sp > delta_sp_max) {
				output = previous + delta_sp_max;

			} ಅನ್ಯಥಾ ಯದಿ (delta_sp < -delta_sp_max) {
				output = previous - delta_sp_max;
			}

			// Transform back to NaN if appropriate
			ಯದಿ (ಒಳಹರಿವು_is_nan && fabsf(output) < FLT_EPSILON) {
				output = NAN;
			}

			_actuator_sp(i) = output;
		}
	}
}

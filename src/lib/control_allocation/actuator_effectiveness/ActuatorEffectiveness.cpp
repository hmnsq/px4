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

#include "ActuatorEffectiveness.hpp"

#include <px4_platform_common/log.h>

ཧྲིལ ActuatorEffectiveness::Configuration::addActuator(ActuatorType type, const matrix::Vector3f &torque,
		const matrix::Vector3f &thrust)
{
	ཧྲིལ actuator_idx = num_actuators_matrix[selected_matrix];

	གལ་ཏེ (actuator_idx >= NUM_ACTUATORS) {
		PX4_ERR("Too many actuators");
		ཕྱིར -1;
	}

	གལ་ཏེ ((ཧྲིལ)type < (ཧྲིལ)ActuatorType::COUNT - 1 && num_actuators[(ཧྲིལ)type + 1] > 0) {
		PX4_ERR("Trying to add actuators in the wrong order (add motors first, then servos)");
		ཕྱིར -1;
	}

	གལ་ཏེ (type == ActuatorType::MOTORS && selected_matrix != 0) {
		PX4_ERR("Trying to add motors to matrix %d (add to matrix 0)", selected_matrix);
		ཕྱིར -1;
	}

	effectiveness_matrices[selected_matrix](ActuatorEffectiveness::ControlAxis::ROLL, actuator_idx) = torque(0);
	effectiveness_matrices[selected_matrix](ActuatorEffectiveness::ControlAxis::PITCH, actuator_idx) = torque(1);
	effectiveness_matrices[selected_matrix](ActuatorEffectiveness::ControlAxis::YAW, actuator_idx) = torque(2);
	effectiveness_matrices[selected_matrix](ActuatorEffectiveness::ControlAxis::THRUST_X, actuator_idx) = thrust(0);
	effectiveness_matrices[selected_matrix](ActuatorEffectiveness::ControlAxis::THRUST_Y, actuator_idx) = thrust(1);
	effectiveness_matrices[selected_matrix](ActuatorEffectiveness::ControlAxis::THRUST_Z, actuator_idx) = thrust(2);
	matrix_selection_indexes[totalNumActuators()] = selected_matrix;
	++num_actuators[(ཧྲིལ)type];
	ཕྱིར num_actuators_matrix[selected_matrix]++;
}

void ActuatorEffectiveness::Configuration::actuatorsAdded(ActuatorType type, ཧྲིལ count)
{
	ཧྲིལ total_count = totalNumActuators();

	ཆེད་དུ (ཧྲིལ i = 0; i < count; ++i) {
		matrix_selection_indexes[i + total_count] = selected_matrix;
	}

	num_actuators[(ཧྲིལ)type] += count;
	num_actuators_matrix[selected_matrix] += count;
}

ཧྲིལ ActuatorEffectiveness::Configuration::totalNumActuators() const
{
	ཧྲིལ total_count = 0;

	ཆེད་དུ (ཧྲིལ i = 0; i < MAX_NUM_MATRICES; ++i) {
		total_count += num_actuators_matrix[i];
	}

	ཕྱིར total_count;
}

ActuatorEffectiveness::ActuatorBitmask ActuatorEffectiveness::getStoppedMotors() const
{
	// Motors can be stopped (here) for two reasons. They can
	// additionally be stopped in the ControlAllocator, due to motor
	// failure.

	//  a) because they are generally not used in the given flight phase.
	ActuatorEffectiveness::ActuatorBitmask stopped_motors_mask = _stopped_motors_mask_due_to_flight_phase;

	//  b) because the thrust setpoint in a given direction is NaN
	གལ་ཏེ (_longitudinal_motors_stopped_by_thrust) { stopped_motors_mask |= _motor_direction_bitmasks.longitudinal; }

	གལ་ཏེ (_lateral_motors_stopped_by_thrust) { stopped_motors_mask |= _motor_direction_bitmasks.lateral; }

	གལ་ཏེ (_vertical_motors_stopped_by_thrust) { stopped_motors_mask |= _motor_direction_bitmasks.vertical; }

	ཕྱིར stopped_motors_mask;
}

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

#અન્તર્ભાવ "ActuatorEffectiveness.hpp"

#અન્તર્ભાવ <px4_platform_common/log.h>

પૂર્ણાંક ActuatorEffectiveness::Configuration::addActuator(ActuatorType type, અચલ matrix::Vector3f &torque,
		અચલ matrix::Vector3f &thrust)
{
	પૂર્ણાંક actuator_idx = અંક_actuators_matrix[selected_matrix];

	જો (actuator_idx >= અંક_ACTUATORS) {
		PX4_ERR("Too many actuators");
		પાછા -1;
	}

	જો ((પૂર્ણાંક)type < (પૂર્ણાંક)ActuatorType::ગણતરી_2 - 1 && અંક_actuators[(પૂર્ણાંક)type + 1] > 0) {
		PX4_ERR("Trying to add actuators in the wrong order (add motors first, then servos)");
		પાછા -1;
	}

	જો (type == ActuatorType::MOTORS && selected_matrix != 0) {
		PX4_ERR("Trying to add motors to matrix %d (add to matrix 0)", selected_matrix);
		પાછા -1;
	}

	effectiveness_matrices[selected_matrix](ActuatorEffectiveness::ControlAxis::ROLL, actuator_idx) = torque(0);
	effectiveness_matrices[selected_matrix](ActuatorEffectiveness::ControlAxis::PITCH, actuator_idx) = torque(1);
	effectiveness_matrices[selected_matrix](ActuatorEffectiveness::ControlAxis::YAW, actuator_idx) = torque(2);
	effectiveness_matrices[selected_matrix](ActuatorEffectiveness::ControlAxis::THRUST_X, actuator_idx) = thrust(0);
	effectiveness_matrices[selected_matrix](ActuatorEffectiveness::ControlAxis::THRUST_Y, actuator_idx) = thrust(1);
	effectiveness_matrices[selected_matrix](ActuatorEffectiveness::ControlAxis::THRUST_Z, actuator_idx) = thrust(2);
	matrix_selection_indexes[total_અંક_Actuators()] = selected_matrix;
	++અંક_actuators[(પૂર્ણાંક)type];
	પાછા અંક_actuators_matrix[selected_matrix]++;
}

શૂન્ય ActuatorEffectiveness::Configuration::actuatorsAdded(ActuatorType type, પૂર્ણાંક ગણતરી)
{
	પૂર્ણાંક total_ગણતરી = total_અંક_Actuators();

	માટે (પૂર્ણાંક i = 0; i < ગણતરી; ++i) {
		matrix_selection_indexes[i + total_ગણતરી] = selected_matrix;
	}

	અંક_actuators[(પૂર્ણાંક)type] += ગણતરી;
	અંક_actuators_matrix[selected_matrix] += ગણતરી;
}

પૂર્ણાંક ActuatorEffectiveness::Configuration::total_અંક_Actuators() અચલ
{
	પૂર્ણાંક total_ગણતરી = 0;

	માટે (પૂર્ણાંક i = 0; i < MAX_અંક_MATRICES; ++i) {
		total_ગણતરી += અંક_actuators_matrix[i];
	}

	પાછા total_ગણતરી;
}

ActuatorEffectiveness::ActuatorBitmask ActuatorEffectiveness::getStoppedMotors() અચલ
{
	// Motors can be stopped (here) for two reasons. They can
	// additionally be stopped in the ControlAllocator, due to motor
	// failure.

	//  a) because they are generally not used in the given flight phase.
	ActuatorEffectiveness::ActuatorBitmask stopped_motors_mask = _stopped_motors_mask_due_to_flight_phase;

	//  b) because the thrust setpoint in a given direction is NaN
	જો (_longitudinal_motors_stopped_by_thrust) { stopped_motors_mask |= _motor_direction_bitmasks.longitudinal; }

	જો (_lateral_motors_stopped_by_thrust) { stopped_motors_mask |= _motor_direction_bitmasks.lateral; }

	જો (_vertical_motors_stopped_by_thrust) { stopped_motors_mask |= _motor_direction_bitmasks.vertical; }

	પાછા stopped_motors_mask;
}

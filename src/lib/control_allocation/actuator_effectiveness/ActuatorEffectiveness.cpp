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

#अन्तर्भाव "ActuatorEffectiveness.hpp"

#अन्तर्भाव <px4_platform_common/log.h>

पूर्णाङ्क ActuatorEffectiveness::Configuration::addActuator(ActuatorType type, ध्रुव matrix::Vector3f &torque,
		ध्रुव matrix::Vector3f &thrust)
{
	पूर्णाङ्क actuator_idx = अंक_actuators_matrix[selected_matrix];

	यदि (actuator_idx >= अंक_ACTUATORS) {
		PX4_ERR("Too many actuators");
		प्रत्यागम -1;
	}

	यदि ((पूर्णाङ्क)type < (पूर्णाङ्क)ActuatorType::गिनती_2 - 1 && अंक_actuators[(पूर्णाङ्क)type + 1] > 0) {
		PX4_ERR("Trying to add actuators in the wrong order (add motors first, then servos)");
		प्रत्यागम -1;
	}

	यदि (type == ActuatorType::MOTORS && selected_matrix != 0) {
		PX4_ERR("Trying to add motors to matrix %d (add to matrix 0)", selected_matrix);
		प्रत्यागम -1;
	}

	effectiveness_matrices[selected_matrix](ActuatorEffectiveness::ControlAxis::ROLL, actuator_idx) = torque(0);
	effectiveness_matrices[selected_matrix](ActuatorEffectiveness::ControlAxis::PITCH, actuator_idx) = torque(1);
	effectiveness_matrices[selected_matrix](ActuatorEffectiveness::ControlAxis::YAW, actuator_idx) = torque(2);
	effectiveness_matrices[selected_matrix](ActuatorEffectiveness::ControlAxis::THRUST_X, actuator_idx) = thrust(0);
	effectiveness_matrices[selected_matrix](ActuatorEffectiveness::ControlAxis::THRUST_Y, actuator_idx) = thrust(1);
	effectiveness_matrices[selected_matrix](ActuatorEffectiveness::ControlAxis::THRUST_Z, actuator_idx) = thrust(2);
	matrix_selection_indexes[total_अंक_Actuators()] = selected_matrix;
	++अंक_actuators[(पूर्णाङ्क)type];
	प्रत्यागम अंक_actuators_matrix[selected_matrix]++;
}

शून्य ActuatorEffectiveness::Configuration::actuatorsAdded(ActuatorType type, पूर्णाङ्क गिनती)
{
	पूर्णाङ्क total_गिनती = total_अंक_Actuators();

	प्रति (पूर्णाङ्क i = 0; i < गिनती; ++i) {
		matrix_selection_indexes[i + total_गिनती] = selected_matrix;
	}

	अंक_actuators[(पूर्णाङ्क)type] += गिनती;
	अंक_actuators_matrix[selected_matrix] += गिनती;
}

पूर्णाङ्क ActuatorEffectiveness::Configuration::total_अंक_Actuators() ध्रुव
{
	पूर्णाङ्क total_गिनती = 0;

	प्रति (पूर्णाङ्क i = 0; i < MAX_अंक_MATRICES; ++i) {
		total_गिनती += अंक_actuators_matrix[i];
	}

	प्रत्यागम total_गिनती;
}

ActuatorEffectiveness::ActuatorBitmask ActuatorEffectiveness::getStoppedMotors() ध्रुव
{
	// Motors can be stopped (here) for two reasons. They can
	// additionally be stopped in the ControlAllocator, due to motor
	// failure.

	//  a) because they are generally not used in the given flight phase.
	ActuatorEffectiveness::ActuatorBitmask stopped_motors_mask = _stopped_motors_mask_due_to_flight_phase;

	//  b) because the thrust setpoint in a given direction is NaN
	यदि (_longitudinal_motors_stopped_by_thrust) { stopped_motors_mask |= _motor_direction_bitmasks.longitudinal; }

	यदि (_lateral_motors_stopped_by_thrust) { stopped_motors_mask |= _motor_direction_bitmasks.lateral; }

	यदि (_vertical_motors_stopped_by_thrust) { stopped_motors_mask |= _motor_direction_bitmasks.vertical; }

	प्रत्यागम stopped_motors_mask;
}

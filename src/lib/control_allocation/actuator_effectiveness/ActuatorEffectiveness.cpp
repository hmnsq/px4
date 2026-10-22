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

पूर्णांक ActuatorEffectiveness::Configuration::addActuator(ActuatorType type, स्थिरांक matrix::Vector3f &torque,
		स्थिरांक matrix::Vector3f &thrust)
{
	पूर्णांक actuator_idx = अंक_actuators_matrix[selected_matrix];

	जर (actuator_idx >= अंक_ACTUATORS) {
		PX4_ERR("Too many actuators");
		परत -1;
	}

	जर ((पूर्णांक)type < (पूर्णांक)ActuatorType::मोजणी_2 - 1 && अंक_actuators[(पूर्णांक)type + 1] > 0) {
		PX4_ERR("Trying to add actuators in the wrong order (add motors first, then servos)");
		परत -1;
	}

	जर (type == ActuatorType::MOTORS && selected_matrix != 0) {
		PX4_ERR("Trying to add motors to matrix %d (add to matrix 0)", selected_matrix);
		परत -1;
	}

	effectiveness_matrices[selected_matrix](ActuatorEffectiveness::ControlAxis::ROLL, actuator_idx) = torque(0);
	effectiveness_matrices[selected_matrix](ActuatorEffectiveness::ControlAxis::PITCH, actuator_idx) = torque(1);
	effectiveness_matrices[selected_matrix](ActuatorEffectiveness::ControlAxis::YAW, actuator_idx) = torque(2);
	effectiveness_matrices[selected_matrix](ActuatorEffectiveness::ControlAxis::THRUST_X, actuator_idx) = thrust(0);
	effectiveness_matrices[selected_matrix](ActuatorEffectiveness::ControlAxis::THRUST_Y, actuator_idx) = thrust(1);
	effectiveness_matrices[selected_matrix](ActuatorEffectiveness::ControlAxis::THRUST_Z, actuator_idx) = thrust(2);
	matrix_selection_indexes[total_अंक_Actuators()] = selected_matrix;
	++अंक_actuators[(पूर्णांक)type];
	परत अंक_actuators_matrix[selected_matrix]++;
}

शून्य ActuatorEffectiveness::Configuration::actuatorsAdded(ActuatorType type, पूर्णांक मोजणी)
{
	पूर्णांक total_मोजणी = total_अंक_Actuators();

	साठी (पूर्णांक i = 0; i < मोजणी; ++i) {
		matrix_selection_indexes[i + total_मोजणी] = selected_matrix;
	}

	अंक_actuators[(पूर्णांक)type] += मोजणी;
	अंक_actuators_matrix[selected_matrix] += मोजणी;
}

पूर्णांक ActuatorEffectiveness::Configuration::total_अंक_Actuators() स्थिरांक
{
	पूर्णांक total_मोजणी = 0;

	साठी (पूर्णांक i = 0; i < MAX_अंक_MATRICES; ++i) {
		total_मोजणी += अंक_actuators_matrix[i];
	}

	परत total_मोजणी;
}

ActuatorEffectiveness::ActuatorBitmask ActuatorEffectiveness::getStoppedMotors() स्थिरांक
{
	// Motors can be stopped (here) for two reasons. They can
	// additionally be stopped in the ControlAllocator, due to motor
	// failure.

	//  a) because they are generally not used in the given flight phase.
	ActuatorEffectiveness::ActuatorBitmask stopped_motors_mask = _stopped_motors_mask_due_to_flight_phase;

	//  b) because the thrust setpoint in a given direction is NaN
	जर (_longitudinal_motors_stopped_by_thrust) { stopped_motors_mask |= _motor_direction_bitmasks.longitudinal; }

	जर (_lateral_motors_stopped_by_thrust) { stopped_motors_mask |= _motor_direction_bitmasks.lateral; }

	जर (_vertical_motors_stopped_by_thrust) { stopped_motors_mask |= _motor_direction_bitmasks.vertical; }

	परत stopped_motors_mask;
}

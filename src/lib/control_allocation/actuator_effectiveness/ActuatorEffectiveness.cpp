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

#شامل "ActuatorEffectiveness.hpp"

#شامل <px4_platform_common/log.h>

صحیح ActuatorEffectiveness::Configuration::جوڑو_Actuator(ActuatorType type, ثابت میٹرکس::Vector3f &torque,
		ثابت میٹرکس::Vector3f &thrust)
{
	صحیح actuator_idx = عدد_actuators_میٹرکس[selected_میٹرکس];

	اگر (actuator_idx >= عدد_ACTUATORS) {
		PX4_ERR("Too many actuators");
		واپس -1;
	}

	اگر ((صحیح)type < (صحیح)ActuatorType::گنتی_2 - 1 && عدد_actuators[(صحیح)type + 1] > 0) {
		PX4_ERR("Trying to add actuators in the wrong order (add motors first, then servos)");
		واپس -1;
	}

	اگر (type == ActuatorType::MOTORS && selected_میٹرکس != 0) {
		PX4_ERR("Trying to add motors to matrix %d (add to matrix 0)", selected_میٹرکس);
		واپس -1;
	}

	effectiveness_matrices[selected_میٹرکس](ActuatorEffectiveness::ControlAxis::ROLL, actuator_idx) = torque(0);
	effectiveness_matrices[selected_میٹرکس](ActuatorEffectiveness::ControlAxis::PITCH, actuator_idx) = torque(1);
	effectiveness_matrices[selected_میٹرکس](ActuatorEffectiveness::ControlAxis::YAW, actuator_idx) = torque(2);
	effectiveness_matrices[selected_میٹرکس](ActuatorEffectiveness::ControlAxis::THRUST_X, actuator_idx) = thrust(0);
	effectiveness_matrices[selected_میٹرکس](ActuatorEffectiveness::ControlAxis::THRUST_Y, actuator_idx) = thrust(1);
	effectiveness_matrices[selected_میٹرکس](ActuatorEffectiveness::ControlAxis::THRUST_Z, actuator_idx) = thrust(2);
	میٹرکس_selection_indexes[کل_عدد_Actuators()] = selected_میٹرکس;
	++عدد_actuators[(صحیح)type];
	واپس عدد_actuators_میٹرکس[selected_میٹرکس]++;
}

خالی ActuatorEffectiveness::Configuration::actuatorsAdded(ActuatorType type, صحیح گنتی)
{
	صحیح کل_گنتی = کل_عدد_Actuators();

	برائے (صحیح i = 0; i < گنتی; ++i) {
		میٹرکس_selection_indexes[i + کل_گنتی] = selected_میٹرکس;
	}

	عدد_actuators[(صحیح)type] += گنتی;
	عدد_actuators_میٹرکس[selected_میٹرکس] += گنتی;
}

صحیح ActuatorEffectiveness::Configuration::کل_عدد_Actuators() ثابت
{
	صحیح کل_گنتی = 0;

	برائے (صحیح i = 0; i < MAX_عدد_MATRICES; ++i) {
		کل_گنتی += عدد_actuators_میٹرکس[i];
	}

	واپس کل_گنتی;
}

ActuatorEffectiveness::ActuatorBitmask ActuatorEffectiveness::getStoppedMotors() ثابت
{
	// Motors can be stopped (here) for two reasons. They can
	// additionally be stopped in the ControlAllocator, due to motor
	// failure.

	//  a) because they are generally not used in the given flight phase.
	ActuatorEffectiveness::ActuatorBitmask stopped_motors_نقاب = stopped_motors_نقاب_due_مقصد_flight_phase;

	//  b) because the thrust setpoint in a given direction is NaN
	اگر (_longitudinal_motors_stopped_by_thrust) { stopped_motors_نقاب |= _motor_direction_bitmasks.longitudinal; }

	اگر (_lateral_motors_stopped_by_thrust) { stopped_motors_نقاب |= _motor_direction_bitmasks.lateral; }

	اگر (_vertical_motors_stopped_by_thrust) { stopped_motors_نقاب |= _motor_direction_bitmasks.vertical; }

	واپس stopped_motors_نقاب;
}

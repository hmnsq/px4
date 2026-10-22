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

#ضمن "ActuatorEffectiveness.hpp"

#ضمن <px4_platform_common/log.h>

صحيح ActuatorEffectiveness::Configuration::جوڙيو_Actuator(ActuatorType type, ثابت ميٽرڪس::Vector3f &torque,
		ثابت ميٽرڪس::Vector3f &thrust)
{
	صحيح actuator_idx = انگ_actuators_ميٽرڪس[selected_ميٽرڪس];

	إذا (actuator_idx >= انگ_ACTUATORS) {
		PX4_ERR("Too many actuators");
		أرجع -1;
	}

	إذا ((صحيح)type < (صحيح)ActuatorType::ڳڻپ_2 - 1 && انگ_actuators[(صحيح)type + 1] > 0) {
		PX4_ERR("Trying to add actuators in the wrong order (add motors first, then servos)");
		أرجع -1;
	}

	إذا (type == ActuatorType::MOTORS && selected_ميٽرڪس != 0) {
		PX4_ERR("Trying to add motors to matrix %d (add to matrix 0)", selected_ميٽرڪس);
		أرجع -1;
	}

	effectiveness_matrices[selected_ميٽرڪس](ActuatorEffectiveness::ControlAxis::ROLL, actuator_idx) = torque(0);
	effectiveness_matrices[selected_ميٽرڪس](ActuatorEffectiveness::ControlAxis::PITCH, actuator_idx) = torque(1);
	effectiveness_matrices[selected_ميٽرڪس](ActuatorEffectiveness::ControlAxis::YAW, actuator_idx) = torque(2);
	effectiveness_matrices[selected_ميٽرڪس](ActuatorEffectiveness::ControlAxis::THRUST_X, actuator_idx) = thrust(0);
	effectiveness_matrices[selected_ميٽرڪس](ActuatorEffectiveness::ControlAxis::THRUST_Y, actuator_idx) = thrust(1);
	effectiveness_matrices[selected_ميٽرڪس](ActuatorEffectiveness::ControlAxis::THRUST_Z, actuator_idx) = thrust(2);
	ميٽرڪس_selection_indexes[ڪل_انگ_Actuators()] = selected_ميٽرڪس;
	++انگ_actuators[(صحيح)type];
	أرجع انگ_actuators_ميٽرڪس[selected_ميٽرڪس]++;
}

فراغ ActuatorEffectiveness::Configuration::actuatorsAdded(ActuatorType type, صحيح ڳڻپ)
{
	صحيح ڪل_ڳڻپ = ڪل_انگ_Actuators();

	لكل (صحيح i = 0; i < ڳڻپ; ++i) {
		ميٽرڪس_selection_indexes[i + ڪل_ڳڻپ] = selected_ميٽرڪس;
	}

	انگ_actuators[(صحيح)type] += ڳڻپ;
	انگ_actuators_ميٽرڪس[selected_ميٽرڪس] += ڳڻپ;
}

صحيح ActuatorEffectiveness::Configuration::ڪل_انگ_Actuators() ثابت
{
	صحيح ڪل_ڳڻپ = 0;

	لكل (صحيح i = 0; i < MAX_انگ_MATRICES; ++i) {
		ڪل_ڳڻپ += انگ_actuators_ميٽرڪس[i];
	}

	أرجع ڪل_ڳڻپ;
}

ActuatorEffectiveness::ActuatorBitmask ActuatorEffectiveness::getStoppedMotors() ثابت
{
	// Motors can be stopped (here) for two reasons. They can
	// additionally be stopped in the ControlAllocator, due to motor
	// failure.

	//  a) because they are generally not used in the given flight phase.
	ActuatorEffectiveness::ActuatorBitmask stopped_motors_نقاب = stopped_motors_نقاب_due_منزل_flight_phase;

	//  b) because the thrust setpoint in a given direction is NaN
	إذا (_longitudinal_motors_stopped_by_thrust) { stopped_motors_نقاب |= _motor_direction_bitmasks.longitudinal; }

	إذا (_lateral_motors_stopped_by_thrust) { stopped_motors_نقاب |= _motor_direction_bitmasks.lateral; }

	إذا (_vertical_motors_stopped_by_thrust) { stopped_motors_نقاب |= _motor_direction_bitmasks.vertical; }

	أرجع stopped_motors_نقاب;
}

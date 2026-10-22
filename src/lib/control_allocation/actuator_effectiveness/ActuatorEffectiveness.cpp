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

#ਅਨ੍ਤਰ੍ਭਾਵ "ActuatorEffectiveness.hpp"

#ਅਨ੍ਤਰ੍ਭਾਵ <px4_platform_common/log.h>

ਪੂਰਨ ActuatorEffectiveness::Configuration::ਜੋੜੋ_Actuator(ActuatorType type, ਅਚਲ ਮੈਟ੍ਰਿਕਸ::Vector3f &torque,
		ਅਚਲ ਮੈਟ੍ਰਿਕਸ::Vector3f &thrust)
{
	ਪੂਰਨ actuator_idx = ਅੰਕ_actuators_ਮੈਟ੍ਰਿਕਸ[selected_ਮੈਟ੍ਰਿਕਸ];

	ਜੇ (actuator_idx >= ਅੰਕ_ACTUATORS) {
		PX4_ERR("Too many actuators");
		ਵਾਪਸ -1;
	}

	ਜੇ ((ਪੂਰਨ)type < (ਪੂਰਨ)ActuatorType::ਗਿਣਤੀ_3 - 1 && ਅੰਕ_actuators[(ਪੂਰਨ)type + 1] > 0) {
		PX4_ERR("Trying to add actuators in the wrong order (add motors first, then servos)");
		ਵਾਪਸ -1;
	}

	ਜੇ (type == ActuatorType::MOTORS && selected_ਮੈਟ੍ਰਿਕਸ != 0) {
		PX4_ERR("Trying to add motors to matrix %d (add to matrix 0)", selected_ਮੈਟ੍ਰਿਕਸ);
		ਵਾਪਸ -1;
	}

	effectiveness_matrices[selected_ਮੈਟ੍ਰਿਕਸ](ActuatorEffectiveness::ControlAxis::ROLL, actuator_idx) = torque(0);
	effectiveness_matrices[selected_ਮੈਟ੍ਰਿਕਸ](ActuatorEffectiveness::ControlAxis::PITCH, actuator_idx) = torque(1);
	effectiveness_matrices[selected_ਮੈਟ੍ਰਿਕਸ](ActuatorEffectiveness::ControlAxis::YAW, actuator_idx) = torque(2);
	effectiveness_matrices[selected_ਮੈਟ੍ਰਿਕਸ](ActuatorEffectiveness::ControlAxis::THRUST_X, actuator_idx) = thrust(0);
	effectiveness_matrices[selected_ਮੈਟ੍ਰਿਕਸ](ActuatorEffectiveness::ControlAxis::THRUST_Y, actuator_idx) = thrust(1);
	effectiveness_matrices[selected_ਮੈਟ੍ਰਿਕਸ](ActuatorEffectiveness::ControlAxis::THRUST_Z, actuator_idx) = thrust(2);
	ਮੈਟ੍ਰਿਕਸ_selection_indexes[ਕੁੱਲ_ਅੰਕ_Actuators()] = selected_ਮੈਟ੍ਰਿਕਸ;
	++ਅੰਕ_actuators[(ਪੂਰਨ)type];
	ਵਾਪਸ ਅੰਕ_actuators_ਮੈਟ੍ਰਿਕਸ[selected_ਮੈਟ੍ਰਿਕਸ]++;
}

ਖਾਲੀ ActuatorEffectiveness::Configuration::actuatorsAdded(ActuatorType type, ਪੂਰਨ ਗਿਣਤੀ_2)
{
	ਪੂਰਨ ਕੁੱਲ_ਗਿਣਤੀ = ਕੁੱਲ_ਅੰਕ_Actuators();

	ਲਈ (ਪੂਰਨ i = 0; i < ਗਿਣਤੀ_2; ++i) {
		ਮੈਟ੍ਰਿਕਸ_selection_indexes[i + ਕੁੱਲ_ਗਿਣਤੀ] = selected_ਮੈਟ੍ਰਿਕਸ;
	}

	ਅੰਕ_actuators[(ਪੂਰਨ)type] += ਗਿਣਤੀ_2;
	ਅੰਕ_actuators_ਮੈਟ੍ਰਿਕਸ[selected_ਮੈਟ੍ਰਿਕਸ] += ਗਿਣਤੀ_2;
}

ਪੂਰਨ ActuatorEffectiveness::Configuration::ਕੁੱਲ_ਅੰਕ_Actuators() ਅਚਲ
{
	ਪੂਰਨ ਕੁੱਲ_ਗਿਣਤੀ = 0;

	ਲਈ (ਪੂਰਨ i = 0; i < MAX_ਅੰਕ_MATRICES; ++i) {
		ਕੁੱਲ_ਗਿਣਤੀ += ਅੰਕ_actuators_ਮੈਟ੍ਰਿਕਸ[i];
	}

	ਵਾਪਸ ਕੁੱਲ_ਗਿਣਤੀ;
}

ActuatorEffectiveness::ActuatorBitmask ActuatorEffectiveness::getStoppedMotors() ਅਚਲ
{
	// Motors can be stopped (here) for two reasons. They can
	// additionally be stopped in the ControlAllocator, due to motor
	// failure.

	//  a) because they are generally not used in the given flight phase.
	ActuatorEffectiveness::ActuatorBitmask stopped_motors_ਨਕਾਬ = stopped_motors_ਨਕਾਬ_due_ਮੰਜ਼ਿਲ_flight_phase;

	//  b) because the thrust setpoint in a given direction is NaN
	ਜੇ (_longitudinal_motors_stopped_by_thrust) { stopped_motors_ਨਕਾਬ |= _motor_direction_bitmasks.longitudinal; }

	ਜੇ (_lateral_motors_stopped_by_thrust) { stopped_motors_ਨਕਾਬ |= _motor_direction_bitmasks.lateral; }

	ਜੇ (_vertical_motors_stopped_by_thrust) { stopped_motors_ਨਕਾਬ |= _motor_direction_bitmasks.vertical; }

	ਵਾਪਸ stopped_motors_ਨਕਾਬ;
}

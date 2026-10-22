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

#అన్తర్భావ "ActuatorEffectiveness.hpp"

#అన్తర్భావ <px4_platform_common/log.h>

పూర్ణాంకం ActuatorEffectiveness::Configuration::కూడు_Actuator(ActuatorType type, స్థిరరాశి మాతృక::Vector3f &torque,
		స్థిరరాశి మాతృక::Vector3f &thrust)
{
	పూర్ణాంకం actuator_idx = అంకె_actuators_మాతృక[selected_మాతృక];

	ఒకవేళ (actuator_idx >= అంకె_ACTUATORS) {
		PX4_ERR("Too many actuators");
		తిరిగి -1;
	}

	ఒకవేళ ((పూర్ణాంకం)type < (పూర్ణాంకం)ActuatorType::లెక్క_2 - 1 && అంకె_actuators[(పూర్ణాంకం)type + 1] > 0) {
		PX4_ERR("Trying to add actuators in the wrong order (add motors first, then servos)");
		తిరిగి -1;
	}

	ఒకవేళ (type == ActuatorType::MOTORS && selected_మాతృక != 0) {
		PX4_ERR("Trying to add motors to matrix %d (add to matrix 0)", selected_మాతృక);
		తిరిగి -1;
	}

	effectiveness_matrices[selected_మాతృక](ActuatorEffectiveness::ControlAxis::ROLL, actuator_idx) = torque(0);
	effectiveness_matrices[selected_మాతృక](ActuatorEffectiveness::ControlAxis::PITCH, actuator_idx) = torque(1);
	effectiveness_matrices[selected_మాతృక](ActuatorEffectiveness::ControlAxis::YAW, actuator_idx) = torque(2);
	effectiveness_matrices[selected_మాతృక](ActuatorEffectiveness::ControlAxis::THRUST_X, actuator_idx) = thrust(0);
	effectiveness_matrices[selected_మాతృక](ActuatorEffectiveness::ControlAxis::THRUST_Y, actuator_idx) = thrust(1);
	effectiveness_matrices[selected_మాతృక](ActuatorEffectiveness::ControlAxis::THRUST_Z, actuator_idx) = thrust(2);
	మాతృక_selection_indexes[మొత్తం_అంకె_Actuators()] = selected_మాతృక;
	++అంకె_actuators[(పూర్ణాంకం)type];
	తిరిగి అంకె_actuators_మాతృక[selected_మాతృక]++;
}

శూన్య ActuatorEffectiveness::Configuration::actuatorsAdded(ActuatorType type, పూర్ణాంకం లెక్క)
{
	పూర్ణాంకం మొత్తం_లెక్క = మొత్తం_అంకె_Actuators();

	కొరకు (పూర్ణాంకం i = 0; i < లెక్క; ++i) {
		మాతృక_selection_indexes[i + మొత్తం_లెక్క] = selected_మాతృక;
	}

	అంకె_actuators[(పూర్ణాంకం)type] += లెక్క;
	అంకె_actuators_మాతృక[selected_మాతృక] += లెక్క;
}

పూర్ణాంకం ActuatorEffectiveness::Configuration::మొత్తం_అంకె_Actuators() స్థిరరాశి
{
	పూర్ణాంకం మొత్తం_లెక్క = 0;

	కొరకు (పూర్ణాంకం i = 0; i < MAX_అంకె_MATRICES; ++i) {
		మొత్తం_లెక్క += అంకె_actuators_మాతృక[i];
	}

	తిరిగి మొత్తం_లెక్క;
}

ActuatorEffectiveness::ActuatorBitmask ActuatorEffectiveness::getStoppedMotors() స్థిరరాశి
{
	// Motors can be stopped (here) for two reasons. They can
	// additionally be stopped in the ControlAllocator, due to motor
	// failure.

	//  a) because they are generally not used in the given flight phase.
	ActuatorEffectiveness::ActuatorBitmask stopped_motors_ముసుగు = stopped_motors_ముసుగు_due_గమ్యం_flight_phase;

	//  b) because the thrust setpoint in a given direction is NaN
	ఒకవేళ (_longitudinal_motors_stopped_by_thrust) { stopped_motors_ముసుగు |= _motor_direction_bitmasks.longitudinal; }

	ఒకవేళ (_lateral_motors_stopped_by_thrust) { stopped_motors_ముసుగు |= _motor_direction_bitmasks.lateral; }

	ఒకవేళ (_vertical_motors_stopped_by_thrust) { stopped_motors_ముసుగు |= _motor_direction_bitmasks.vertical; }

	తిరిగి stopped_motors_ముసుగు;
}

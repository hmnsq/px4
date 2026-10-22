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

#అన్తర్భావ "ControlAllocation.hpp"

Control_కేటాయింపు::Control_కేటాయింపు()
{
	control_కేటాయింపు_scale.సమితి_All(1.f);
	_actuator_min.సమితి_All(0.f);
	_actuator_max.సమితి_All(1.f);
}

శూన్య
Control_కేటాయింపు::సమితి_Effectiveness_మాతృక(
	స్థిరరాశి మాతృక::మాతృక_2<దశాంశం, Control_కేటాయింపు::అంకె_AXES, Control_కేటాయింపు::అంకె_ACTUATORS> &effectiveness,
	స్థిరరాశి Actuator_సదిశ &actuator_trim, స్థిరరాశి Actuator_సదిశ &linearization_బిందువు, పూర్ణాంకం అంకె_actuators,
	తార్కిక update_normalization_scale)
{
	_effectiveness = effectiveness;
	Actuator_సదిశ linearization_బిందువు_clipped = linearization_బిందువు;
	clipActuatorSetpoint(linearization_బిందువు_clipped);
	_actuator_trim = actuator_trim + linearization_బిందువు_clipped;
	clipActuatorSetpoint(_actuator_trim);
	అంకె_actuators_2 = అంకె_actuators;
	_control_trim = _effectiveness * linearization_బిందువు_clipped;
}

శూన్య
Control_కేటాయింపు::సమితి_Actuator_Setpoint(
	స్థిరరాశి మాతృక::సదిశ<దశాంశం, Control_కేటాయింపు::అంకె_ACTUATORS> &actuator_sp)
{
	// Set actuator setpoint
	_actuator_sp = actuator_sp;

	// Clip
	clipActuatorSetpoint(_actuator_sp);
}

శూన్య
Control_కేటాయింపు::clipActuatorSetpoint(మాతృక::సదిశ<దశాంశం, Control_కేటాయింపు::అంకె_ACTUATORS> &actuator) స్థిరరాశి
{
	కొరకు (పూర్ణాంకం i = 0; i < అంకె_actuators_2; i++) {
		ఒకవేళ (_actuator_max(i) < _actuator_min(i)) {
			actuator(i) = _actuator_trim(i);

		} లేకపోతే ఒకవేళ (actuator(i) < _actuator_min(i)) {
			actuator(i) = _actuator_min(i);

		} లేకపోతే ఒకవేళ (actuator(i) > _actuator_max(i)) {
			actuator(i) = _actuator_max(i);
		}
	}
}

మాతృక::సదిశ<దశాంశం, Control_కేటాయింపు::అంకె_ACTUATORS>
Control_కేటాయింపు::normalizeActuatorSetpoint(స్థిరరాశి మాతృక::సదిశ<దశాంశం, Control_కేటాయింపు::అంకె_ACTUATORS> &actuator)
స్థిరరాశి
{
	మాతృక::సదిశ<దశాంశం, Control_కేటాయింపు::అంకె_ACTUATORS> actuator_normalized;

	కొరకు (పూర్ణాంకం i = 0; i < అంకె_actuators_2; i++) {
		ఒకవేళ (_actuator_min(i) < _actuator_max(i)) {
			actuator_normalized(i) = (actuator(i) - _actuator_min(i)) / (_actuator_max(i) - _actuator_min(i));

		} లేకపోతే {
			actuator_normalized(i) = (_actuator_trim(i) - _actuator_min(i)) / (_actuator_max(i) - _actuator_min(i));
		}
	}

	తిరిగి actuator_normalized;
}

శూన్య Control_కేటాయింపు::apply_Slew_రేటు_హద్దు(దశాంశం dt)
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

	కొరకు (పూర్ణాంకం i = 0; i < అంకె_actuators_2; i++) {
		ఒకవేళ (actuator_slew_రేటు_హద్దు(i) > FLT_EPSILON) {

			దశాంశం ఆదానం = _actuator_sp(i);
			దశాంశం మునుపటి = _prev_actuator_sp(i);

			// Before slew limiting, transform NaN to 0, but remember if the input was NaN
			స్థిరరాశి తార్కిక ఆదానం_is_nan = !PX4_ISFINITE(ఆదానం);

			ఒకవేళ (ఆదానం_is_nan) {
				ఆదానం = 0.f;
			}

			ఒకవేళ (!PX4_ISFINITE(మునుపటి)) {
				మునుపటి = 0.f;
			}

			// Slew limit without any NaN involved
			స్థిరరాశి దశాంశం delta_sp_max = dt * (_actuator_max(i) - _actuator_min(i)) / actuator_slew_రేటు_హద్దు(i);
			స్థిరరాశి దశాంశం delta_sp = ఆదానం - మునుపటి;

			దశాంశం output = ఆదానం;

			ఒకవేళ (delta_sp > delta_sp_max) {
				output = మునుపటి + delta_sp_max;

			} లేకపోతే ఒకవేళ (delta_sp < -delta_sp_max) {
				output = మునుపటి - delta_sp_max;
			}

			// Transform back to NaN if appropriate
			ఒకవేళ (ఆదానం_is_nan && fabsf(output) < FLT_EPSILON) {
				output = NAN;
			}

			_actuator_sp(i) = output;
		}
	}
}

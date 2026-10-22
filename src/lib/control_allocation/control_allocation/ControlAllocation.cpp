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

#ਅਨ੍ਤਰ੍ਭਾਵ "ControlAllocation.hpp"

Control_ਵੰਡ::Control_ਵੰਡ()
{
	control_ਵੰਡ_scale.ਸੈੱਟ_All(1.f);
	_actuator_min.ਸੈੱਟ_All(0.f);
	_actuator_max.ਸੈੱਟ_All(1.f);
}

ਖਾਲੀ
Control_ਵੰਡ::ਸੈੱਟ_Effectiveness_ਮੈਟ੍ਰਿਕਸ(
	ਅਚਲ ਮੈਟ੍ਰਿਕਸ::ਮੈਟ੍ਰਿਕਸ_2<ਦਸ਼ਮਲਵ, Control_ਵੰਡ::ਅੰਕ_AXES, Control_ਵੰਡ::ਅੰਕ_ACTUATORS> &effectiveness,
	ਅਚਲ Actuator_ਸਦਿਸ਼ &actuator_trim, ਅਚਲ Actuator_ਸਦਿਸ਼ &linearization_ਬਿੰਦੂ, ਪੂਰਨ ਅੰਕ_actuators,
	ਤਾਰਕਿਕ update_normalization_scale)
{
	_effectiveness = effectiveness;
	Actuator_ਸਦਿਸ਼ linearization_ਬਿੰਦੂ_clipped = linearization_ਬਿੰਦੂ;
	clipActuatorSetpoint(linearization_ਬਿੰਦੂ_clipped);
	_actuator_trim = actuator_trim + linearization_ਬਿੰਦੂ_clipped;
	clipActuatorSetpoint(_actuator_trim);
	ਅੰਕ_actuators_2 = ਅੰਕ_actuators;
	_control_trim = _effectiveness * linearization_ਬਿੰਦੂ_clipped;
}

ਖਾਲੀ
Control_ਵੰਡ::ਸੈੱਟ_Actuator_Setpoint(
	ਅਚਲ ਮੈਟ੍ਰਿਕਸ::ਸਦਿਸ਼<ਦਸ਼ਮਲਵ, Control_ਵੰਡ::ਅੰਕ_ACTUATORS> &actuator_sp)
{
	// Set actuator setpoint
	_actuator_sp = actuator_sp;

	// Clip
	clipActuatorSetpoint(_actuator_sp);
}

ਖਾਲੀ
Control_ਵੰਡ::clipActuatorSetpoint(ਮੈਟ੍ਰਿਕਸ::ਸਦਿਸ਼<ਦਸ਼ਮਲਵ, Control_ਵੰਡ::ਅੰਕ_ACTUATORS> &actuator) ਅਚਲ
{
	ਲਈ (ਪੂਰਨ i = 0; i < ਅੰਕ_actuators_2; i++) {
		ਜੇ (_actuator_max(i) < _actuator_min(i)) {
			actuator(i) = _actuator_trim(i);

		} ਨਹੀਂਤਾਂ ਜੇ (actuator(i) < _actuator_min(i)) {
			actuator(i) = _actuator_min(i);

		} ਨਹੀਂਤਾਂ ਜੇ (actuator(i) > _actuator_max(i)) {
			actuator(i) = _actuator_max(i);
		}
	}
}

ਮੈਟ੍ਰਿਕਸ::ਸਦਿਸ਼<ਦਸ਼ਮਲਵ, Control_ਵੰਡ::ਅੰਕ_ACTUATORS>
Control_ਵੰਡ::normalizeActuatorSetpoint(ਅਚਲ ਮੈਟ੍ਰਿਕਸ::ਸਦਿਸ਼<ਦਸ਼ਮਲਵ, Control_ਵੰਡ::ਅੰਕ_ACTUATORS> &actuator)
ਅਚਲ
{
	ਮੈਟ੍ਰਿਕਸ::ਸਦਿਸ਼<ਦਸ਼ਮਲਵ, Control_ਵੰਡ::ਅੰਕ_ACTUATORS> actuator_normalized;

	ਲਈ (ਪੂਰਨ i = 0; i < ਅੰਕ_actuators_2; i++) {
		ਜੇ (_actuator_min(i) < _actuator_max(i)) {
			actuator_normalized(i) = (actuator(i) - _actuator_min(i)) / (_actuator_max(i) - _actuator_min(i));

		} ਨਹੀਂਤਾਂ {
			actuator_normalized(i) = (_actuator_trim(i) - _actuator_min(i)) / (_actuator_max(i) - _actuator_min(i));
		}
	}

	ਵਾਪਸ actuator_normalized;
}

ਖਾਲੀ Control_ਵੰਡ::apply_Slew_ਦਰ_ਹੱਦਬੰਦੀ(ਦਸ਼ਮਲਵ dt)
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

	ਲਈ (ਪੂਰਨ i = 0; i < ਅੰਕ_actuators_2; i++) {
		ਜੇ (actuator_slew_ਦਰ_ਹੱਦਬੰਦੀ(i) > FLT_EPSILON) {

			ਦਸ਼ਮਲਵ ਨਿਵੇਸ਼ = _actuator_sp(i);
			ਦਸ਼ਮਲਵ ਪਿਛਲਾ = _prev_actuator_sp(i);

			// Before slew limiting, transform NaN to 0, but remember if the input was NaN
			ਅਚਲ ਤਾਰਕਿਕ ਨਿਵੇਸ਼_is_nan = !PX4_ISFINITE(ਨਿਵੇਸ਼);

			ਜੇ (ਨਿਵੇਸ਼_is_nan) {
				ਨਿਵੇਸ਼ = 0.f;
			}

			ਜੇ (!PX4_ISFINITE(ਪਿਛਲਾ)) {
				ਪਿਛਲਾ = 0.f;
			}

			// Slew limit without any NaN involved
			ਅਚਲ ਦਸ਼ਮਲਵ delta_sp_max = dt * (_actuator_max(i) - _actuator_min(i)) / actuator_slew_ਦਰ_ਹੱਦਬੰਦੀ(i);
			ਅਚਲ ਦਸ਼ਮਲਵ delta_sp = ਨਿਵੇਸ਼ - ਪਿਛਲਾ;

			ਦਸ਼ਮਲਵ output = ਨਿਵੇਸ਼;

			ਜੇ (delta_sp > delta_sp_max) {
				output = ਪਿਛਲਾ + delta_sp_max;

			} ਨਹੀਂਤਾਂ ਜੇ (delta_sp < -delta_sp_max) {
				output = ਪਿਛਲਾ - delta_sp_max;
			}

			// Transform back to NaN if appropriate
			ਜੇ (ਨਿਵੇਸ਼_is_nan && fabsf(output) < FLT_EPSILON) {
				output = NAN;
			}

			_actuator_sp(i) = output;
		}
	}
}

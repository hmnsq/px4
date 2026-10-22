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

#ضمن "ControlAllocation.hpp"

Control_تخصیص::Control_تخصیص()
{
	control_تخصیص_scale.مجموعه_All(1.f);
	_actuator_min.مجموعه_All(0.f);
	_actuator_max.مجموعه_All(1.f);
}

فراغ
Control_تخصیص::مجموعه_Effectiveness_ماتریس(
	ثابت ماتریس::ماتریس_2<عائم, Control_تخصیص::رقم_AXES, Control_تخصیص::رقم_ACTUATORS> &effectiveness,
	ثابت Actuator_وکتور &actuator_trim, ثابت Actuator_وکتور &linearization_نقطه, صحيح رقم_actuators,
	منطقي update_normalization_scale)
{
	_effectiveness = effectiveness;
	Actuator_وکتور linearization_نقطه_clipped = linearization_نقطه;
	clipActuatorSetpoint(linearization_نقطه_clipped);
	_actuator_trim = actuator_trim + linearization_نقطه_clipped;
	clipActuatorSetpoint(_actuator_trim);
	رقم_actuators_2 = رقم_actuators;
	_control_trim = _effectiveness * linearization_نقطه_clipped;
}

فراغ
Control_تخصیص::مجموعه_Actuator_Setpoint(
	ثابت ماتریس::وکتور<عائم, Control_تخصیص::رقم_ACTUATORS> &actuator_sp)
{
	// Set actuator setpoint
	_actuator_sp = actuator_sp;

	// Clip
	clipActuatorSetpoint(_actuator_sp);
}

فراغ
Control_تخصیص::clipActuatorSetpoint(ماتریس::وکتور<عائم, Control_تخصیص::رقم_ACTUATORS> &actuator) ثابت
{
	لكل (صحيح i = 0; i < رقم_actuators_2; i++) {
		إذا (_actuator_max(i) < _actuator_min(i)) {
			actuator(i) = _actuator_trim(i);

		} وإلا إذا (actuator(i) < _actuator_min(i)) {
			actuator(i) = _actuator_min(i);

		} وإلا إذا (actuator(i) > _actuator_max(i)) {
			actuator(i) = _actuator_max(i);
		}
	}
}

ماتریس::وکتور<عائم, Control_تخصیص::رقم_ACTUATORS>
Control_تخصیص::normalizeActuatorSetpoint(ثابت ماتریس::وکتور<عائم, Control_تخصیص::رقم_ACTUATORS> &actuator)
ثابت
{
	ماتریس::وکتور<عائم, Control_تخصیص::رقم_ACTUATORS> actuator_normalized;

	لكل (صحيح i = 0; i < رقم_actuators_2; i++) {
		إذا (_actuator_min(i) < _actuator_max(i)) {
			actuator_normalized(i) = (actuator(i) - _actuator_min(i)) / (_actuator_max(i) - _actuator_min(i));

		} وإلا {
			actuator_normalized(i) = (_actuator_trim(i) - _actuator_min(i)) / (_actuator_max(i) - _actuator_min(i));
		}
	}

	أرجع actuator_normalized;
}

فراغ Control_تخصیص::apply_Slew_نرخ_حد(عائم dt)
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

	لكل (صحيح i = 0; i < رقم_actuators_2; i++) {
		إذا (actuator_slew_نرخ_حد(i) > FLT_EPSILON) {

			عائم درون_داد = _actuator_sp(i);
			عائم قبلی = _prev_actuator_sp(i);

			// Before slew limiting, transform NaN to 0, but remember if the input was NaN
			ثابت منطقي درون_داد_is_nan = !PX4_ISFINITE(درون_داد);

			إذا (درون_داد_is_nan) {
				درون_داد = 0.f;
			}

			إذا (!PX4_ISFINITE(قبلی)) {
				قبلی = 0.f;
			}

			// Slew limit without any NaN involved
			ثابت عائم delta_sp_max = dt * (_actuator_max(i) - _actuator_min(i)) / actuator_slew_نرخ_حد(i);
			ثابت عائم delta_sp = درون_داد - قبلی;

			عائم output = درون_داد;

			إذا (delta_sp > delta_sp_max) {
				output = قبلی + delta_sp_max;

			} وإلا إذا (delta_sp < -delta_sp_max) {
				output = قبلی - delta_sp_max;
			}

			// Transform back to NaN if appropriate
			إذا (درون_داد_is_nan && fabsf(output) < FLT_EPSILON) {
				output = NAN;
			}

			_actuator_sp(i) = output;
		}
	}
}

/****************************************************************************
 *
 *   Copyright (c) 2019-2023 PX4 Development Team. All rights reserved.
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
 * @file rate_control.cpp
 */

#شامل "rate_control.hpp"
#شامل <px4_platform_common/defines.h>

استعمال نامخلا میٹرکس;

خالی شرح_Control::سیٹ_Pid_Gains(ثابت Vector3f &P, ثابت Vector3f &I, ثابت Vector3f &D)
{
	_gain_p = P;
	_gain_i = I;
	_gain_d = D;
}

خالی شرح_Control::سیٹ_Saturation_حالت(ثابت Vector3<منطقی> &saturation_مثبت,
				      ثابت Vector3<منطقی> &saturation_negative)
{
	control_allocator_saturation_مثبت = saturation_مثبت;
	_control_allocator_saturation_negative = saturation_negative;
}

خالی شرح_Control::سیٹ_مثبت_Saturation_Flag(size_t axis, منطقی is_saturated)
{
	اگر (axis < 3) {
		control_allocator_saturation_مثبت(axis) = is_saturated;
	}
}

خالی شرح_Control::سیٹ_Negative_Saturation_Flag(size_t axis, منطقی is_saturated)
{
	اگر (axis < 3) {
		_control_allocator_saturation_negative(axis) = is_saturated;
	}
}

Vector3f شرح_Control::update(ثابت Vector3f &شرح, ثابت Vector3f &شرح_sp, ثابت Vector3f &angular_accel,
			     ثابت اعشاری dt, ثابت منطقی landed)
{
	// angular rates error
	Vector3f شرح_error = شرح_sp - شرح;

	// PID control with feed forward
	Vector3f torque = _gain_p.emult(شرح_error) + شرح_int - _gain_d.emult(angular_accel) + _gain_ff.emult(شرح_sp);

	// update integral only if we are not landed
	اگر (!landed) {
		updateIntegral(شرح_error, dt);
	}

	واپس torque;
}

خالی شرح_Control::updateIntegral(Vector3f &شرح_error, ثابت اعشاری dt)
{
	برائے (صحیح i = 0; i < 3; i++) {
		// prevent further positive control saturation
		اگر (control_allocator_saturation_مثبت(i)) {
			شرح_error(i) = math::min(شرح_error(i), 0.f);
		}

		// prevent further negative control saturation
		اگر (_control_allocator_saturation_negative(i)) {
			شرح_error(i) = math::max(شرح_error(i), 0.f);
		}

		// I term factor: reduce the I gain with increasing rate error.
		// This counteracts a non-linear effect where the integral builds up quickly upon a large setpoint
		// change (noticeable in a bounce-back effect after a flip).
		// The formula leads to a gradual decrease w/o steps, while only affecting the cases where it should:
		// with the parameter set to 400 degrees, up to 100 deg rate error, i_factor is almost 1 (having no effect),
		// and up to 200 deg error leads to <25% reduction of I.
		اعشاری i_ضربی_عدد = شرح_error(i) / math::radians(400.f);
		i_ضربی_عدد = math::max(0.0f, 1.f - i_ضربی_عدد * i_ضربی_عدد);

		// Perform the integration using a first order method
		اعشاری شرح_i = شرح_int(i) + i_ضربی_عدد * _gain_i(i) * شرح_error(i) * dt;

		// do not propagate the result if out of range or invalid
		اگر (PX4_ISFINITE(شرح_i)) {
			شرح_int(i) = math::constrain(شرح_i, -_lim_int(i), _lim_int(i));
		}
	}
}

خالی شرح_Control::get_شرح_Control_حالت(شرح_ctrl_حالت_s &شرح_ctrl_حالت)
{
	شرح_ctrl_حالت.rollspeed_integ = شرح_int(0);
	شرح_ctrl_حالت.pitchspeed_integ = شرح_int(1);
	شرح_ctrl_حالت.yawspeed_integ = شرح_int(2);
}

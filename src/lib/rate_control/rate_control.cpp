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

#ضمن "rate_control.hpp"
#ضمن <px4_platform_common/defines.h>

ورتوں نطاق میٹرکس;

فراغ شرح_Control::سیٹ_Pid_Gains(ثابت Vector3f &P, ثابت Vector3f &I, ثابت Vector3f &D)
{
	_gain_p = P;
	_gain_i = I;
	_gain_d = D;
}

فراغ شرح_Control::سیٹ_Saturation_حالت(ثابت Vector3<منطقي> &saturation_مثبت,
				      ثابت Vector3<منطقي> &saturation_negative)
{
	control_allocator_saturation_مثبت = saturation_مثبت;
	_control_allocator_saturation_negative = saturation_negative;
}

فراغ شرح_Control::سیٹ_مثبت_Saturation_Flag(size_t axis, منطقي is_saturated)
{
	إذا (axis < 3) {
		control_allocator_saturation_مثبت(axis) = is_saturated;
	}
}

فراغ شرح_Control::سیٹ_Negative_Saturation_Flag(size_t axis, منطقي is_saturated)
{
	إذا (axis < 3) {
		_control_allocator_saturation_negative(axis) = is_saturated;
	}
}

Vector3f شرح_Control::update(ثابت Vector3f &شرح, ثابت Vector3f &شرح_sp, ثابت Vector3f &angular_accel,
			     ثابت عائم dt, ثابت منطقي landed)
{
	// angular rates error
	Vector3f شرح_error = شرح_sp - شرح;

	// PID control with feed forward
	Vector3f torque = _gain_p.emult(شرح_error) + شرح_int - _gain_d.emult(angular_accel) + _gain_ff.emult(شرح_sp);

	// update integral only if we are not landed
	إذا (!landed) {
		updateIntegral(شرح_error, dt);
	}

	أرجع torque;
}

فراغ شرح_Control::updateIntegral(Vector3f &شرح_error, ثابت عائم dt)
{
	لكل (صحيح i = 0; i < 3; i++) {
		// prevent further positive control saturation
		إذا (control_allocator_saturation_مثبت(i)) {
			شرح_error(i) = math::min(شرح_error(i), 0.f);
		}

		// prevent further negative control saturation
		إذا (_control_allocator_saturation_negative(i)) {
			شرح_error(i) = math::max(شرح_error(i), 0.f);
		}

		// I term factor: reduce the I gain with increasing rate error.
		// This counteracts a non-linear effect where the integral builds up quickly upon a large setpoint
		// change (noticeable in a bounce-back effect after a flip).
		// The formula leads to a gradual decrease w/o steps, while only affecting the cases where it should:
		// with the parameter set to 400 degrees, up to 100 deg rate error, i_factor is almost 1 (having no effect),
		// and up to 200 deg error leads to <25% reduction of I.
		عائم i_ضارب = شرح_error(i) / math::radians(400.f);
		i_ضارب = math::max(0.0f, 1.f - i_ضارب * i_ضارب);

		// Perform the integration using a first order method
		عائم شرح_i = شرح_int(i) + i_ضارب * _gain_i(i) * شرح_error(i) * dt;

		// do not propagate the result if out of range or invalid
		إذا (PX4_ISFINITE(شرح_i)) {
			شرح_int(i) = math::constrain(شرح_i, -_lim_int(i), _lim_int(i));
		}
	}
}

فراغ شرح_Control::get_شرح_Control_حالت(شرح_ctrl_حالت_s &شرح_ctrl_حالت)
{
	شرح_ctrl_حالت.rollspeed_integ = شرح_int(0);
	شرح_ctrl_حالت.pitchspeed_integ = شرح_int(1);
	شرح_ctrl_حالت.yawspeed_integ = شرح_int(2);
}

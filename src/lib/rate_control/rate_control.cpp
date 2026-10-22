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

#ਅਨ੍ਤਰ੍ਭਾਵ "rate_control.hpp"
#ਅਨ੍ਤਰ੍ਭਾਵ <px4_platform_common/defines.h>

ਵਰਤੋਂ ਨਾਮਾਕਾਸ਼ ਮੈਟ੍ਰਿਕਸ;

ਖਾਲੀ ਦਰ_Control::ਸੈੱਟ_Pid_Gains(ਅਚਲ Vector3f &P, ਅਚਲ Vector3f &I, ਅਚਲ Vector3f &D)
{
	_gain_p = P;
	_gain_i = I;
	_gain_d = D;
}

ਖਾਲੀ ਦਰ_Control::ਸੈੱਟ_Saturation_ਸਥਿਤੀ(ਅਚਲ Vector3<ਤਾਰਕਿਕ> &saturation_ਸਕਾਰਾਤਮਕ,
				      ਅਚਲ Vector3<ਤਾਰਕਿਕ> &saturation_negative)
{
	control_allocator_saturation_ਸਕਾਰਾਤਮਕ = saturation_ਸਕਾਰਾਤਮਕ;
	_control_allocator_saturation_negative = saturation_negative;
}

ਖਾਲੀ ਦਰ_Control::ਸੈੱਟ_ਸਕਾਰਾਤਮਕ_Saturation_Flag(size_t axis, ਤਾਰਕਿਕ is_saturated)
{
	ਜੇ (axis < 3) {
		control_allocator_saturation_ਸਕਾਰਾਤਮਕ(axis) = is_saturated;
	}
}

ਖਾਲੀ ਦਰ_Control::ਸੈੱਟ_Negative_Saturation_Flag(size_t axis, ਤਾਰਕਿਕ is_saturated)
{
	ਜੇ (axis < 3) {
		_control_allocator_saturation_negative(axis) = is_saturated;
	}
}

Vector3f ਦਰ_Control::update(ਅਚਲ Vector3f &ਦਰ, ਅਚਲ Vector3f &ਦਰ_sp, ਅਚਲ Vector3f &angular_accel,
			     ਅਚਲ ਦਸ਼ਮਲਵ dt, ਅਚਲ ਤਾਰਕਿਕ landed)
{
	// angular rates error
	Vector3f ਦਰ_error = ਦਰ_sp - ਦਰ;

	// PID control with feed forward
	Vector3f torque = _gain_p.emult(ਦਰ_error) + ਦਰ_int - _gain_d.emult(angular_accel) + _gain_ff.emult(ਦਰ_sp);

	// update integral only if we are not landed
	ਜੇ (!landed) {
		updateIntegral(ਦਰ_error, dt);
	}

	ਵਾਪਸ torque;
}

ਖਾਲੀ ਦਰ_Control::updateIntegral(Vector3f &ਦਰ_error, ਅਚਲ ਦਸ਼ਮਲਵ dt)
{
	ਲਈ (ਪੂਰਨ i = 0; i < 3; i++) {
		// prevent further positive control saturation
		ਜੇ (control_allocator_saturation_ਸਕਾਰਾਤਮਕ(i)) {
			ਦਰ_error(i) = math::min(ਦਰ_error(i), 0.f);
		}

		// prevent further negative control saturation
		ਜੇ (_control_allocator_saturation_negative(i)) {
			ਦਰ_error(i) = math::max(ਦਰ_error(i), 0.f);
		}

		// I term factor: reduce the I gain with increasing rate error.
		// This counteracts a non-linear effect where the integral builds up quickly upon a large setpoint
		// change (noticeable in a bounce-back effect after a flip).
		// The formula leads to a gradual decrease w/o steps, while only affecting the cases where it should:
		// with the parameter set to 400 degrees, up to 100 deg rate error, i_factor is almost 1 (having no effect),
		// and up to 200 deg error leads to <25% reduction of I.
		ਦਸ਼ਮਲਵ i_ਗੁਣਕ = ਦਰ_error(i) / math::radians(400.f);
		i_ਗੁਣਕ = math::max(0.0f, 1.f - i_ਗੁਣਕ * i_ਗੁਣਕ);

		// Perform the integration using a first order method
		ਦਸ਼ਮਲਵ ਦਰ_i = ਦਰ_int(i) + i_ਗੁਣਕ * _gain_i(i) * ਦਰ_error(i) * dt;

		// do not propagate the result if out of range or invalid
		ਜੇ (PX4_ISFINITE(ਦਰ_i)) {
			ਦਰ_int(i) = math::constrain(ਦਰ_i, -_lim_int(i), _lim_int(i));
		}
	}
}

ਖਾਲੀ ਦਰ_Control::get_ਦਰ_Control_ਸਥਿਤੀ(ਦਰ_ctrl_ਸਥਿਤੀ_s &ਦਰ_ctrl_ਸਥਿਤੀ)
{
	ਦਰ_ctrl_ਸਥਿਤੀ.rollspeed_integ = ਦਰ_int(0);
	ਦਰ_ctrl_ਸਥਿਤੀ.pitchspeed_integ = ਦਰ_int(1);
	ਦਰ_ctrl_ਸਥਿਤੀ.yawspeed_integ = ਦਰ_int(2);
}

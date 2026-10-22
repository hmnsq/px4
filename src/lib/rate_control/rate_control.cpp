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

#अन्तर्भाव "rate_control.hpp"
#अन्तर्भाव <px4_platform_common/defines.h>

उपयुज्य नामाकाश matrix;

शून्य दरः_Control::समुच्चयः_Pid_Gains(ध्रुव Vector3f &P, ध्रुव Vector3f &I, ध्रुव Vector3f &D)
{
	_gain_p = P;
	_gain_i = I;
	_gain_d = D;
}

शून्य दरः_Control::समुच्चयः_Saturation_स्थितिः(ध्रुव Vector3<तार्किक> &saturation_धनात्मकम्,
				      ध्रुव Vector3<तार्किक> &saturation_negative)
{
	control_allocator_saturation_धनात्मकम् = saturation_धनात्मकम्;
	_control_allocator_saturation_negative = saturation_negative;
}

शून्य दरः_Control::समुच्चयः_धनात्मकम्_Saturation_Flag(size_t axis, तार्किक is_saturated)
{
	यदि (axis < 3) {
		control_allocator_saturation_धनात्मकम्(axis) = is_saturated;
	}
}

शून्य दरः_Control::समुच्चयः_Negative_Saturation_Flag(size_t axis, तार्किक is_saturated)
{
	यदि (axis < 3) {
		_control_allocator_saturation_negative(axis) = is_saturated;
	}
}

Vector3f दरः_Control::update(ध्रुव Vector3f &दरः, ध्रुव Vector3f &दरः_sp, ध्रुव Vector3f &angular_accel,
			     ध्रुव भिन्न dt, ध्रुव तार्किक landed)
{
	// angular rates error
	Vector3f दरः_error = दरः_sp - दरः;

	// PID control with feed forward
	Vector3f torque = _gain_p.emult(दरः_error) + दरः_int - _gain_d.emult(angular_accel) + _gain_ff.emult(दरः_sp);

	// update integral only if we are not landed
	यदि (!landed) {
		updateIntegral(दरः_error, dt);
	}

	प्रत्यागम torque;
}

शून्य दरः_Control::updateIntegral(Vector3f &दरः_error, ध्रुव भिन्न dt)
{
	प्रति (पूर्णाङ्क i = 0; i < 3; i++) {
		// prevent further positive control saturation
		यदि (control_allocator_saturation_धनात्मकम्(i)) {
			दरः_error(i) = math::min(दरः_error(i), 0.f);
		}

		// prevent further negative control saturation
		यदि (_control_allocator_saturation_negative(i)) {
			दरः_error(i) = math::max(दरः_error(i), 0.f);
		}

		// I term factor: reduce the I gain with increasing rate error.
		// This counteracts a non-linear effect where the integral builds up quickly upon a large setpoint
		// change (noticeable in a bounce-back effect after a flip).
		// The formula leads to a gradual decrease w/o steps, while only affecting the cases where it should:
		// with the parameter set to 400 degrees, up to 100 deg rate error, i_factor is almost 1 (having no effect),
		// and up to 200 deg error leads to <25% reduction of I.
		भिन्न i_गुणकः = दरः_error(i) / math::radians(400.f);
		i_गुणकः = math::max(0.0f, 1.f - i_गुणकः * i_गुणकः);

		// Perform the integration using a first order method
		भिन्न दरः_i = दरः_int(i) + i_गुणकः * _gain_i(i) * दरः_error(i) * dt;

		// do not propagate the result if out of range or invalid
		यदि (PX4_ISFINITE(दरः_i)) {
			दरः_int(i) = math::constrain(दरः_i, -_lim_int(i), _lim_int(i));
		}
	}
}

शून्य दरः_Control::get_दरः_Control_स्थितिः(दरः_ctrl_स्थितिः_s &दरः_ctrl_स्थितिः)
{
	दरः_ctrl_स्थितिः.rollspeed_integ = दरः_int(0);
	दरः_ctrl_स्थितिः.pitchspeed_integ = दरः_int(1);
	दरः_ctrl_स्थितिः.yawspeed_integ = दरः_int(2);
}

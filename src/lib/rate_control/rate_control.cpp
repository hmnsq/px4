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

#అన్తర్భావ "rate_control.hpp"
#అన్తర్భావ <px4_platform_common/defines.h>

ఉపయోగించి నామాకాశ మాతృక;

శూన్య రేటు_Control::సమితి_Pid_Gains(స్థిరరాశి Vector3f &P, స్థిరరాశి Vector3f &I, స్థిరరాశి Vector3f &D)
{
	_gain_p = P;
	_gain_i = I;
	_gain_d = D;
}

శూన్య రేటు_Control::సమితి_Saturation_స్థితి(స్థిరరాశి Vector3<తార్కిక> &saturation_ధనాత్మక,
				      స్థిరరాశి Vector3<తార్కిక> &saturation_negative)
{
	control_allocator_saturation_ధనాత్మక = saturation_ధనాత్మక;
	_control_allocator_saturation_negative = saturation_negative;
}

శూన్య రేటు_Control::సమితి_ధనాత్మక_Saturation_Flag(size_t axis, తార్కిక is_saturated)
{
	ఒకవేళ (axis < 3) {
		control_allocator_saturation_ధనాత్మక(axis) = is_saturated;
	}
}

శూన్య రేటు_Control::సమితి_Negative_Saturation_Flag(size_t axis, తార్కిక is_saturated)
{
	ఒకవేళ (axis < 3) {
		_control_allocator_saturation_negative(axis) = is_saturated;
	}
}

Vector3f రేటు_Control::update(స్థిరరాశి Vector3f &రేటు, స్థిరరాశి Vector3f &రేటు_sp, స్థిరరాశి Vector3f &angular_accel,
			     స్థిరరాశి దశాంశం dt, స్థిరరాశి తార్కిక landed)
{
	// angular rates error
	Vector3f రేటు_error = రేటు_sp - రేటు;

	// PID control with feed forward
	Vector3f torque = _gain_p.emult(రేటు_error) + రేటు_int - _gain_d.emult(angular_accel) + _gain_ff.emult(రేటు_sp);

	// update integral only if we are not landed
	ఒకవేళ (!landed) {
		updateIntegral(రేటు_error, dt);
	}

	తిరిగి torque;
}

శూన్య రేటు_Control::updateIntegral(Vector3f &రేటు_error, స్థిరరాశి దశాంశం dt)
{
	కొరకు (పూర్ణాంకం i = 0; i < 3; i++) {
		// prevent further positive control saturation
		ఒకవేళ (control_allocator_saturation_ధనాత్మక(i)) {
			రేటు_error(i) = math::min(రేటు_error(i), 0.f);
		}

		// prevent further negative control saturation
		ఒకవేళ (_control_allocator_saturation_negative(i)) {
			రేటు_error(i) = math::max(రేటు_error(i), 0.f);
		}

		// I term factor: reduce the I gain with increasing rate error.
		// This counteracts a non-linear effect where the integral builds up quickly upon a large setpoint
		// change (noticeable in a bounce-back effect after a flip).
		// The formula leads to a gradual decrease w/o steps, while only affecting the cases where it should:
		// with the parameter set to 400 degrees, up to 100 deg rate error, i_factor is almost 1 (having no effect),
		// and up to 200 deg error leads to <25% reduction of I.
		దశాంశం i_గుణకం = రేటు_error(i) / math::radians(400.f);
		i_గుణకం = math::max(0.0f, 1.f - i_గుణకం * i_గుణకం);

		// Perform the integration using a first order method
		దశాంశం రేటు_i = రేటు_int(i) + i_గుణకం * _gain_i(i) * రేటు_error(i) * dt;

		// do not propagate the result if out of range or invalid
		ఒకవేళ (PX4_ISFINITE(రేటు_i)) {
			రేటు_int(i) = math::constrain(రేటు_i, -_lim_int(i), _lim_int(i));
		}
	}
}

శూన్య రేటు_Control::get_రేటు_Control_స్థితి(రేటు_ctrl_స్థితి_s &రేటు_ctrl_స్థితి)
{
	రేటు_ctrl_స్థితి.rollspeed_integ = రేటు_int(0);
	రేటు_ctrl_స్థితి.pitchspeed_integ = రేటు_int(1);
	రేటు_ctrl_స్థితి.yawspeed_integ = రేటు_int(2);
}

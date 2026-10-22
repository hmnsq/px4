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

#ಅನ್ತರ್ಭಾವ "rate_control.hpp"
#ಅನ್ತರ್ಭಾವ <px4_platform_common/defines.h>

ಬಳಸಿ ನಾಮಾಕಾಶ ಮಾತೃಕೆ;

ಶೂನ್ಯ ದರ_Control::ಗಣ_Pid_Gains(ಸ್ಥಿರಾಂಕ Vector3f &P, ಸ್ಥಿರಾಂಕ Vector3f &I, ಸ್ಥಿರಾಂಕ Vector3f &D)
{
	_gain_p = P;
	_gain_i = I;
	_gain_d = D;
}

ಶೂನ್ಯ ದರ_Control::ಗಣ_Saturation_ಸ್ಥಿತಿ(ಸ್ಥಿರಾಂಕ Vector3<ತಾರ್ಕಿಕ> &saturation_ಧನಾತ್ಮಕ,
				      ಸ್ಥಿರಾಂಕ Vector3<ತಾರ್ಕಿಕ> &saturation_negative)
{
	control_allocator_saturation_ಧನಾತ್ಮಕ = saturation_ಧನಾತ್ಮಕ;
	_control_allocator_saturation_negative = saturation_negative;
}

ಶೂನ್ಯ ದರ_Control::ಗಣ_ಧನಾತ್ಮಕ_Saturation_Flag(size_t axis, ತಾರ್ಕಿಕ is_saturated)
{
	ಆದರೆ (axis < 3) {
		control_allocator_saturation_ಧನಾತ್ಮಕ(axis) = is_saturated;
	}
}

ಶೂನ್ಯ ದರ_Control::ಗಣ_Negative_Saturation_Flag(size_t axis, ತಾರ್ಕಿಕ is_saturated)
{
	ಆದರೆ (axis < 3) {
		_control_allocator_saturation_negative(axis) = is_saturated;
	}
}

Vector3f ದರ_Control::update(ಸ್ಥಿರಾಂಕ Vector3f &ದರ, ಸ್ಥಿರಾಂಕ Vector3f &ದರ_sp, ಸ್ಥಿರಾಂಕ Vector3f &angular_accel,
			     ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ dt, ಸ್ಥಿರಾಂಕ ತಾರ್ಕಿಕ landed)
{
	// angular rates error
	Vector3f ದರ_error = ದರ_sp - ದರ;

	// PID control with feed forward
	Vector3f torque = _gain_p.emult(ದರ_error) + ದರ_int - _gain_d.emult(angular_accel) + _gain_ff.emult(ದರ_sp);

	// update integral only if we are not landed
	ಆದರೆ (!landed) {
		updateIntegral(ದರ_error, dt);
	}

	ಹಿಂತಿರುಗಿ torque;
}

ಶೂನ್ಯ ದರ_Control::updateIntegral(Vector3f &ದರ_error, ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ dt)
{
	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < 3; i++) {
		// prevent further positive control saturation
		ಆದರೆ (control_allocator_saturation_ಧನಾತ್ಮಕ(i)) {
			ದರ_error(i) = math::min(ದರ_error(i), 0.f);
		}

		// prevent further negative control saturation
		ಆದರೆ (_control_allocator_saturation_negative(i)) {
			ದರ_error(i) = math::max(ದರ_error(i), 0.f);
		}

		// I term factor: reduce the I gain with increasing rate error.
		// This counteracts a non-linear effect where the integral builds up quickly upon a large setpoint
		// change (noticeable in a bounce-back effect after a flip).
		// The formula leads to a gradual decrease w/o steps, while only affecting the cases where it should:
		// with the parameter set to 400 degrees, up to 100 deg rate error, i_factor is almost 1 (having no effect),
		// and up to 200 deg error leads to <25% reduction of I.
		ದಶಮಾಂಶ i_ಗುಣಕ = ದರ_error(i) / math::radians(400.f);
		i_ಗುಣಕ = math::max(0.0f, 1.f - i_ಗುಣಕ * i_ಗುಣಕ);

		// Perform the integration using a first order method
		ದಶಮಾಂಶ ದರ_i = ದರ_int(i) + i_ಗುಣಕ * _gain_i(i) * ದರ_error(i) * dt;

		// do not propagate the result if out of range or invalid
		ಆದರೆ (PX4_ISFINITE(ದರ_i)) {
			ದರ_int(i) = math::constrain(ದರ_i, -_lim_int(i), _lim_int(i));
		}
	}
}

ಶೂನ್ಯ ದರ_Control::get_ದರ_Control_ಸ್ಥಿತಿ(ದರ_ctrl_ಸ್ಥಿತಿ_s &ದರ_ctrl_ಸ್ಥಿತಿ)
{
	ದರ_ctrl_ಸ್ಥಿತಿ.rollspeed_integ = ದರ_int(0);
	ದರ_ctrl_ಸ್ಥಿತಿ.pitchspeed_integ = ದರ_int(1);
	ದರ_ctrl_ಸ್ಥಿತಿ.yawspeed_integ = ದರ_int(2);
}

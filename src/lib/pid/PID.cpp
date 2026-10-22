/****************************************************************************
 *
 *   Copyright (c) 2022-2024 PX4 Development Team. All rights reserved.
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

#ਅਨ੍ਤਰ੍ਭਾਵ "PID.hpp"
#ਅਨ੍ਤਰ੍ਭਾਵ "lib/mathlib/math/Functions.hpp"

ਖਾਲੀ PID::ਸੈੱਟ_Output_ਹੱਦਬੰਦੀ(ਅਚਲ ਦਸ਼ਮਲਵ ਹੇਠਲਾ_ਹੱਦਬੰਦੀ, ਅਚਲ ਦਸ਼ਮਲਵ ਉੱਤਲਾ_ਹੱਦਬੰਦੀ)
{
	ਹੇਠਲਾ_ਹੱਦਬੰਦੀ_output = ਹੇਠਲਾ_ਹੱਦਬੰਦੀ;
	ਉੱਤਲਾ_ਹੱਦਬੰਦੀ_output = ਉੱਤਲਾ_ਹੱਦਬੰਦੀ;
}

ਖਾਲੀ PID::ਸੈੱਟ_Gains(ਅਚਲ ਦਸ਼ਮਲਵ P, ਅਚਲ ਦਸ਼ਮਲਵ I, ਅਚਲ ਦਸ਼ਮਲਵ D)
{
	_gain_proportional = P;
	_gain_integral = I;
	_gain_derivative = D;
}

ਦਸ਼ਮਲਵ PID::update(ਅਚਲ ਦਸ਼ਮਲਵ feedback, ਅਚਲ ਦਸ਼ਮਲਵ dt, ਅਚਲ ਤਾਰਕਿਕ update_integral)
{
	ਅਚਲ ਦਸ਼ਮਲਵ error = _setpoint - feedback;
	ਅਚਲ ਦਸ਼ਮਲਵ derivative = updateDerivative(feedback, dt);
	ਅਚਲ ਦਸ਼ਮਲਵ output = (_gain_feedforward * _setpoint) + (_gain_proportional * error) + _integral -
			     (_gain_derivative * derivative);
	ਅਚਲ ਦਸ਼ਮਲਵ output_constrained = math::constrain(output, ਹੇਠਲਾ_ਹੱਦਬੰਦੀ_output, ਉੱਤਲਾ_ਹੱਦਬੰਦੀ_output);

	ਜੇ (update_integral) {
		updateIntegral(error, output - output_constrained, dt);
	}

	ਆਖਰੀ_feedback = feedback;
	ਵਾਪਸ output_constrained;
}

ਖਾਲੀ PID::updateIntegral(ਦਸ਼ਮਲਵ error, ਅਚਲ ਦਸ਼ਮਲਵ saturation, ਅਚਲ ਦਸ਼ਮਲਵ dt)
{
	// Anti-windup: stop integrating in the direction that drives the output
	// further into saturation (conditional integration)
	ਜੇ ((saturation > FLT_EPSILON && error > 0.f) || (saturation < -FLT_EPSILON && error < 0.f)) {
		error = 0.f;
	}

	ਅਚਲ ਦਸ਼ਮਲਵ integral_new = _integral + _gain_integral * error * dt;

	ਜੇ (std::isfinite(integral_new)) {
		_integral = math::constrain(integral_new, -ਹੱਦਬੰਦੀ_integral, ਹੱਦਬੰਦੀ_integral);
	}
}

ਦਸ਼ਮਲਵ PID::updateDerivative(ਦਸ਼ਮਲਵ feedback, ਅਚਲ ਦਸ਼ਮਲਵ dt)
{
	ਦਸ਼ਮਲਵ feedback_change = 0.f;

	ਜੇ ((dt > FLT_EPSILON) && std::isfinite(ਆਖਰੀ_feedback)) {
		feedback_change = (feedback - ਆਖਰੀ_feedback) / dt;
	}

	ਵਾਪਸ feedback_change;
}

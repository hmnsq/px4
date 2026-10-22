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

#अन्तर्भाव "PID.hpp"
#अन्तर्भाव "lib/mathlib/math/Functions.hpp"

शून्य PID::setOutputLimit(ध्रुव भिन्न lower_limit, ध्रुव भिन्न upper_limit)
{
	_lower_limit_output = lower_limit;
	_upper_limit_output = upper_limit;
}

शून्य PID::setGains(ध्रुव भिन्न P, ध्रुव भिन्न I, ध्रुव भिन्न D)
{
	_gain_proportional = P;
	_gain_integral = I;
	_gain_derivative = D;
}

भिन्न PID::update(ध्रुव भिन्न feedback, ध्रुव भिन्न dt, ध्रुव तार्किक update_integral)
{
	ध्रुव भिन्न error = _setpoint - feedback;
	ध्रुव भिन्न derivative = updateDerivative(feedback, dt);
	ध्रुव भिन्न output = (_gain_feedforward * _setpoint) + (_gain_proportional * error) + _integral -
			     (_gain_derivative * derivative);
	ध्रुव भिन्न output_constrained = math::constrain(output, _lower_limit_output, _upper_limit_output);

	जइ (update_integral) {
		updateIntegral(error, output - output_constrained, dt);
	}

	_last_feedback = feedback;
	प्रत्यागम output_constrained;
}

शून्य PID::updateIntegral(भिन्न error, ध्रुव भिन्न saturation, ध्रुव भिन्न dt)
{
	// Anti-windup: stop integrating in the direction that drives the output
	// further into saturation (conditional integration)
	जइ ((saturation > FLT_EPSILON && error > 0.f) || (saturation < -FLT_EPSILON && error < 0.f)) {
		error = 0.f;
	}

	ध्रुव भिन्न integral_new = _integral + _gain_integral * error * dt;

	जइ (std::isfinite(integral_new)) {
		_integral = math::constrain(integral_new, -_limit_integral, _limit_integral);
	}
}

भिन्न PID::updateDerivative(भिन्न feedback, ध्रुव भिन्न dt)
{
	भिन्न feedback_change = 0.f;

	जइ ((dt > FLT_EPSILON) && std::isfinite(_last_feedback)) {
		feedback_change = (feedback - _last_feedback) / dt;
	}

	प्रत्यागम feedback_change;
}

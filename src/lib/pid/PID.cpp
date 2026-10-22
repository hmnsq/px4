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

#அந்தர்பாவ "PID.hpp"
#அந்தர்பாவ "lib/mathlib/math/Functions.hpp"

வெறுமை PID::setOutputLimit(மாறிலி தசம lower_limit, மாறிலி தசம upper_limit)
{
	_lower_limit_output = lower_limit;
	_upper_limit_output = upper_limit;
}

வெறுமை PID::setGains(மாறிலி தசம P, மாறிலி தசம I, மாறிலி தசம D)
{
	_gain_proportional = P;
	_gain_integral = I;
	_gain_derivative = D;
}

தசம PID::update(மாறிலி தசம feedback, மாறிலி தசம dt, மாறிலி தருக்க update_integral)
{
	மாறிலி தசம error = _setpoint - feedback;
	மாறிலி தசம derivative = updateDerivative(feedback, dt);
	மாறிலி தசம output = (_gain_feedforward * _setpoint) + (_gain_proportional * error) + _integral -
			     (_gain_derivative * derivative);
	மாறிலி தசம output_constrained = math::constrain(output, _lower_limit_output, _upper_limit_output);

	எனில் (update_integral) {
		updateIntegral(error, output - output_constrained, dt);
	}

	_last_feedback = feedback;
	திரும்பு output_constrained;
}

வெறுமை PID::updateIntegral(தசம error, மாறிலி தசம saturation, மாறிலி தசம dt)
{
	// Anti-windup: stop integrating in the direction that drives the output
	// further into saturation (conditional integration)
	எனில் ((saturation > FLT_EPSILON && error > 0.f) || (saturation < -FLT_EPSILON && error < 0.f)) {
		error = 0.f;
	}

	மாறிலி தசம integral_new = _integral + _gain_integral * error * dt;

	எனில் (std::isfinite(integral_new)) {
		_integral = math::constrain(integral_new, -_limit_integral, _limit_integral);
	}
}

தசம PID::updateDerivative(தசம feedback, மாறிலி தசம dt)
{
	தசம feedback_change = 0.f;

	எனில் ((dt > FLT_EPSILON) && std::isfinite(_last_feedback)) {
		feedback_change = (feedback - _last_feedback) / dt;
	}

	திரும்பு feedback_change;
}

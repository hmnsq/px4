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

शून्य PID::setOutputLimit(स्थिरांक दशांश lower_limit, स्थिरांक दशांश upper_limit)
{
	_lower_limit_output = lower_limit;
	_upper_limit_output = upper_limit;
}

शून्य PID::setGains(स्थिरांक दशांश P, स्थिरांक दशांश I, स्थिरांक दशांश D)
{
	_gain_proportional = P;
	_gain_integral = I;
	_gain_derivative = D;
}

दशांश PID::update(स्थिरांक दशांश feedback, स्थिरांक दशांश dt, स्थिरांक तार्किक update_integral)
{
	स्थिरांक दशांश error = _setpoint - feedback;
	स्थिरांक दशांश derivative = updateDerivative(feedback, dt);
	स्थिरांक दशांश output = (_gain_feedforward * _setpoint) + (_gain_proportional * error) + _integral -
			     (_gain_derivative * derivative);
	स्थिरांक दशांश output_constrained = math::constrain(output, _lower_limit_output, _upper_limit_output);

	जर (update_integral) {
		updateIntegral(error, output - output_constrained, dt);
	}

	_last_feedback = feedback;
	परत output_constrained;
}

शून्य PID::updateIntegral(दशांश error, स्थिरांक दशांश saturation, स्थिरांक दशांश dt)
{
	// Anti-windup: stop integrating in the direction that drives the output
	// further into saturation (conditional integration)
	जर ((saturation > भग्न_अंतर && error > 0.f) || (saturation < -भग्न_अंतर && error < 0.f)) {
		error = 0.f;
	}

	स्थिरांक दशांश integral_new = _integral + _gain_integral * error * dt;

	जर (std::है_सीमित(integral_new)) {
		_integral = math::constrain(integral_new, -_limit_integral, _limit_integral);
	}
}

दशांश PID::updateDerivative(दशांश feedback, स्थिरांक दशांश dt)
{
	दशांश feedback_change = 0.f;

	जर ((dt > भग्न_अंतर) && std::है_सीमित(_last_feedback)) {
		feedback_change = (feedback - _last_feedback) / dt;
	}

	परत feedback_change;
}

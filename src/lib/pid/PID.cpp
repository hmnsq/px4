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

#অন্তর্ভাব "PID.hpp"
#অন্তর্ভাব "lib/mathlib/math/Functions.hpp"

শূন্য PID::setOutputLimit(ধ্রুবক দশমিক lower_limit, ধ্রুবক দশমিক upper_limit)
{
	_lower_limit_output = lower_limit;
	_upper_limit_output = upper_limit;
}

শূন্য PID::setGains(ধ্রুবক দশমিক P, ধ্রুবক দশমিক I, ধ্রুবক দশমিক D)
{
	_gain_proportional = P;
	_gain_integral = I;
	_gain_derivative = D;
}

দশমিক PID::update(ধ্রুবক দশমিক feedback, ধ্রুবক দশমিক dt, ধ্রুবক যৌক্তিক update_integral)
{
	ধ্রুবক দশমিক error = _setpoint - feedback;
	ধ্রুবক দশমিক derivative = updateDerivative(feedback, dt);
	ধ্রুবক দশমিক output = (_gain_feedforward * _setpoint) + (_gain_proportional * error) + _integral -
			     (_gain_derivative * derivative);
	ধ্রুবক দশমিক output_constrained = math::constrain(output, _lower_limit_output, _upper_limit_output);

	যদি (update_integral) {
		updateIntegral(error, output - output_constrained, dt);
	}

	_last_feedback = feedback;
	ফেরত output_constrained;
}

শূন্য PID::updateIntegral(দশমিক error, ধ্রুবক দশমিক saturation, ধ্রুবক দশমিক dt)
{
	// Anti-windup: stop integrating in the direction that drives the output
	// further into saturation (conditional integration)
	যদি ((saturation > FLT_EPSILON && error > 0.f) || (saturation < -FLT_EPSILON && error < 0.f)) {
		error = 0.f;
	}

	ধ্রুবক দশমিক integral_new = _integral + _gain_integral * error * dt;

	যদি (std::isfinite(integral_new)) {
		_integral = math::constrain(integral_new, -_limit_integral, _limit_integral);
	}
}

দশমিক PID::updateDerivative(দশমিক feedback, ধ্রুবক দশমিক dt)
{
	দশমিক feedback_change = 0.f;

	যদি ((dt > FLT_EPSILON) && std::isfinite(_last_feedback)) {
		feedback_change = (feedback - _last_feedback) / dt;
	}

	ফেরত feedback_change;
}

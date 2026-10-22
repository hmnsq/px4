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

#అన్తర్భావ "PID.hpp"
#అన్తర్భావ "lib/mathlib/math/Functions.hpp"

శూన్య PID::సమితి_Output_హద్దు(స్థిరరాశి దశాంశం క్రింది_హద్దు, స్థిరరాశి దశాంశం పై_హద్దు)
{
	క్రింది_హద్దు_output = క్రింది_హద్దు;
	పై_హద్దు_output = పై_హద్దు;
}

శూన్య PID::సమితి_Gains(స్థిరరాశి దశాంశం P, స్థిరరాశి దశాంశం I, స్థిరరాశి దశాంశం D)
{
	_gain_proportional = P;
	_gain_integral = I;
	_gain_derivative = D;
}

దశాంశం PID::update(స్థిరరాశి దశాంశం feedback, స్థిరరాశి దశాంశం dt, స్థిరరాశి తార్కిక update_integral)
{
	స్థిరరాశి దశాంశం error = _setpoint - feedback;
	స్థిరరాశి దశాంశం derivative = updateDerivative(feedback, dt);
	స్థిరరాశి దశాంశం output = (_gain_feedforward * _setpoint) + (_gain_proportional * error) + _integral -
			     (_gain_derivative * derivative);
	స్థిరరాశి దశాంశం output_constrained = math::constrain(output, క్రింది_హద్దు_output, పై_హద్దు_output);

	ఒకవేళ (update_integral) {
		updateIntegral(error, output - output_constrained, dt);
	}

	చివరి_feedback = feedback;
	తిరిగి output_constrained;
}

శూన్య PID::updateIntegral(దశాంశం error, స్థిరరాశి దశాంశం saturation, స్థిరరాశి దశాంశం dt)
{
	// Anti-windup: stop integrating in the direction that drives the output
	// further into saturation (conditional integration)
	ఒకవేళ ((saturation > FLT_EPSILON && error > 0.f) || (saturation < -FLT_EPSILON && error < 0.f)) {
		error = 0.f;
	}

	స్థిరరాశి దశాంశం integral_new = _integral + _gain_integral * error * dt;

	ఒకవేళ (std::isfinite(integral_new)) {
		_integral = math::constrain(integral_new, -హద్దు_integral, హద్దు_integral);
	}
}

దశాంశం PID::updateDerivative(దశాంశం feedback, స్థిరరాశి దశాంశం dt)
{
	దశాంశం feedback_change = 0.f;

	ఒకవేళ ((dt > FLT_EPSILON) && std::isfinite(చివరి_feedback)) {
		feedback_change = (feedback - చివరి_feedback) / dt;
	}

	తిరిగి feedback_change;
}

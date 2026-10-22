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

#ಅನ್ತರ್ಭಾವ "PID.hpp"
#ಅನ್ತರ್ಭಾವ "lib/mathlib/math/Functions.hpp"

ಶೂನ್ಯ PID::ಗಣ_Output_ಎಲ್ಲೆ(ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ ಅಧೋ_ಎಲ್ಲೆ, ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ ಊರ್ಧ್ವ_ಎಲ್ಲೆ)
{
	ಅಧೋ_ಎಲ್ಲೆ_output = ಅಧೋ_ಎಲ್ಲೆ;
	ಊರ್ಧ್ವ_ಎಲ್ಲೆ_output = ಊರ್ಧ್ವ_ಎಲ್ಲೆ;
}

ಶೂನ್ಯ PID::ಗಣ_Gains(ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ P, ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ I, ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ D)
{
	_gain_proportional = P;
	_gain_integral = I;
	_gain_derivative = D;
}

ದಶಮಾಂಶ PID::update(ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ feedback, ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ dt, ಸ್ಥಿರಾಂಕ ತಾರ್ಕಿಕ update_integral)
{
	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ error = _setpoint - feedback;
	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ derivative = updateDerivative(feedback, dt);
	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ output = (_gain_feedforward * _setpoint) + (_gain_proportional * error) + _integral -
			     (_gain_derivative * derivative);
	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ output_constrained = math::constrain(output, ಅಧೋ_ಎಲ್ಲೆ_output, ಊರ್ಧ್ವ_ಎಲ್ಲೆ_output);

	ಆದರೆ (update_integral) {
		updateIntegral(error, output - output_constrained, dt);
	}

	ಕೊನೆಯ_feedback = feedback;
	ಹಿಂತಿರುಗಿ output_constrained;
}

ಶೂನ್ಯ PID::updateIntegral(ದಶಮಾಂಶ error, ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ saturation, ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ dt)
{
	// Anti-windup: stop integrating in the direction that drives the output
	// further into saturation (conditional integration)
	ಆದರೆ ((saturation > FLT_EPSILON && error > 0.f) || (saturation < -FLT_EPSILON && error < 0.f)) {
		error = 0.f;
	}

	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ integral_new = _integral + _gain_integral * error * dt;

	ಆದರೆ (std::isfinite(integral_new)) {
		_integral = math::constrain(integral_new, -ಎಲ್ಲೆ_integral, ಎಲ್ಲೆ_integral);
	}
}

ದಶಮಾಂಶ PID::updateDerivative(ದಶಮಾಂಶ feedback, ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ dt)
{
	ದಶಮಾಂಶ feedback_change = 0.f;

	ಆದರೆ ((dt > FLT_EPSILON) && std::isfinite(ಕೊನೆಯ_feedback)) {
		feedback_change = (feedback - ಕೊನೆಯ_feedback) / dt;
	}

	ಹಿಂತಿರುಗಿ feedback_change;
}

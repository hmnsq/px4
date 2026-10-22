/****************************************************************************
 *
 *   Copyright (c) 2020-2023 PX4 Development Team. All rights reserved.
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

#അന്തര്ഭാവ "StickYaw.hpp"

#അന്തര്ഭാവ <px4_platform_common/defines.h>

using matrix::wrap_pi;

StickYaw::StickYaw(ModuleParams *parent) :
	ModuleParams(parent)
{}

ശൂന്യം StickYaw::reset(സ്ഥിരം ദശാംശം yaw, സ്ഥിരം ദശാംശം unaided_yaw)
{
	// The filter state outlives deactivation; a reset means there is no stick history to carry over.
	resetYawspeed();

	എങ്കിൽ (PX4_ISFINITE(unaided_yaw)) {
		_yaw_error_lpf.reset(wrap_pi(yaw - unaided_yaw));
	}
}

ശൂന്യം StickYaw::ekfResetHandler(സ്ഥിരം ദശാംശം delta_yaw)
{
	_yaw_error_lpf.reset(wrap_pi(_yaw_error_lpf.getState() + delta_yaw));
	_yaw_error_ref = wrap_pi(_yaw_error_ref + delta_yaw);
}

ശൂന്യം StickYaw::generateYawSetpoint(ദശാംശം &yawspeed_setpoint, ദശാംശം &yaw_setpoint, സ്ഥിരം ദശാംശം stick_yaw,
				   സ്ഥിരം ദശാംശം yaw, സ്ഥിരം ദശാംശം deltatime, സ്ഥിരം ദശാംശം unaided_yaw)
{
	സ്ഥിരം ദശാംശം yaw_correction_prev = _yaw_correction;
	സ്ഥിരം തർക്കം reset_setpoint = updateYawCorrection(yaw, unaided_yaw, deltatime);

	എങ്കിൽ (reset_setpoint) {
		yaw_setpoint = NAN;
	}

	_yawspeed_filter.setParameters(deltatime, _param_mpc_man_y_tau.get());
	സ്ഥിരം ദശാംശം yawspeed_scale = math::min(math::radians(_param_mpc_man_y_max.get()), _yawspeed_constraint);
	yawspeed_setpoint = _yawspeed_filter.update(stick_yaw * yawspeed_scale);
	yaw_setpoint = updateYawLock(yaw, yawspeed_setpoint, yaw_setpoint, yaw_correction_prev);
}

തർക്കം StickYaw::updateYawCorrection(സ്ഥിരം ദശാംശം yaw, സ്ഥിരം ദശാംശം unaided_yaw, സ്ഥിരം ദശാംശം deltatime)
{
	എങ്കിൽ (!PX4_ISFINITE(unaided_yaw)) {
		_unaided_yaw_was_invalid = സത്യം;
		// If unaided yaw is not available we leave yaw_correction_ unchanged
		// Meaning yaw_setpoint - yaw_correction_prev + _yaw_correction = yaw_setpoint
		തിരികെ അസത്യം;
	}

	// Detect the convergence phase of the yaw estimate by monitoring its relative
	// distance from an unaided yaw source.
	സ്ഥിരം ദശാംശം yaw_error = wrap_pi(yaw - unaided_yaw);

	എങ്കിൽ (_unaided_yaw_was_invalid) {
		_unaided_yaw_was_invalid = അസത്യം;
		_yaw_error_lpf.reset(yaw_error);
		_yaw_error_ref = yaw_error;
		_yaw_correction = 0.f;
		_yaw_estimate_converging = അസത്യം;
		തിരികെ സത്യം;
	}

	// Run it through a high-pass filter to detect transients
	സ്ഥിരം ദശാംശം yaw_error_hpf = wrap_pi(yaw_error - _yaw_error_lpf.getState());
	_yaw_error_lpf.update(yaw_error, deltatime);

	സ്ഥിരം തർക്കം was_converging = _yaw_estimate_converging;
	_yaw_estimate_converging = fabsf(yaw_error_hpf) > _kYawErrorChangeThreshold;

	തർക്കം reset_setpoint = അസത്യം;

	എങ്കിൽ (!_yaw_estimate_converging) {
		_yaw_error_ref = yaw_error;

		എങ്കിൽ (was_converging) {
			// Force a reset of the locking mechanism
			reset_setpoint = സത്യം;
		}
	}

	_yaw_correction = wrap_pi(yaw_error - _yaw_error_ref);

	തിരികെ reset_setpoint;
}

ദശാംശം StickYaw::updateYawLock(സ്ഥിരം ദശാംശം yaw, സ്ഥിരം ദശാംശം yawspeed_setpoint, സ്ഥിരം ദശാംശം yaw_setpoint,
			      സ്ഥിരം ദശാംശം yaw_correction_prev) സ്ഥിരം
{
	// Yaw-lock depends on desired yawspeed input. If not locked, yaw_sp is set to NAN.
	എങ്കിൽ (fabsf(yawspeed_setpoint) > FLT_EPSILON) {
		// no fixed heading when rotating around yaw by stick
		തിരികെ NAN;

	} അല്ലെങ്കിൽ {
		// slow down and hold the current heading when no more rotation commanded
		എങ്കിൽ (!PX4_ISFINITE(yaw_setpoint)) {
			തിരികെ yaw;

		} അല്ലെങ്കിൽ {
			തിരികെ wrap_pi(yaw_setpoint - yaw_correction_prev + _yaw_correction);
		}
	}
}

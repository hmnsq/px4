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

#ಅನ್ತರ್ಭಾವ "StickYaw.hpp"

#ಅನ್ತರ್ಭಾವ <px4_platform_common/defines.h>

using matrix::wrap_pi;

StickYaw::StickYaw(ModuleParams *parent) :
	ModuleParams(parent)
{}

ಶೂನ್ಯ StickYaw::reset(ಧ್ರುವ ಭಿನ್ನ yaw, ಧ್ರುವ ಭಿನ್ನ unaided_yaw)
{
	// The filter state outlives deactivation; a reset means there is no stick history to carry over.
	resetYawspeed();

	ಯದಿ (PX4_ISFINITE(unaided_yaw)) {
		_yaw_error_lpf.reset(wrap_pi(yaw - unaided_yaw));
	}
}

ಶೂನ್ಯ StickYaw::ekfResetHandler(ಧ್ರುವ ಭಿನ್ನ delta_yaw)
{
	_yaw_error_lpf.reset(wrap_pi(_yaw_error_lpf.getState() + delta_yaw));
	_yaw_error_ref = wrap_pi(_yaw_error_ref + delta_yaw);
}

ಶೂನ್ಯ StickYaw::generateYawSetpoint(ಭಿನ್ನ &yawspeed_setpoint, ಭಿನ್ನ &yaw_setpoint, ಧ್ರುವ ಭಿನ್ನ stick_yaw,
				   ಧ್ರುವ ಭಿನ್ನ yaw, ಧ್ರುವ ಭಿನ್ನ deltatime, ಧ್ರುವ ಭಿನ್ನ unaided_yaw)
{
	ಧ್ರುವ ಭಿನ್ನ yaw_correction_prev = _yaw_correction;
	ಧ್ರುವ ತಾರ್ಕಿಕ reset_setpoint = updateYawCorrection(yaw, unaided_yaw, deltatime);

	ಯದಿ (reset_setpoint) {
		yaw_setpoint = NAN;
	}

	_yawspeed_filter.setParameters(deltatime, _param_mpc_man_y_tau.get());
	ಧ್ರುವ ಭಿನ್ನ yawspeed_scale = math::min(math::radians(_param_mpc_man_y_max.get()), _yawspeed_constraint);
	yawspeed_setpoint = _yawspeed_filter.update(stick_yaw * yawspeed_scale);
	yaw_setpoint = updateYawLock(yaw, yawspeed_setpoint, yaw_setpoint, yaw_correction_prev);
}

ತಾರ್ಕಿಕ StickYaw::updateYawCorrection(ಧ್ರುವ ಭಿನ್ನ yaw, ಧ್ರುವ ಭಿನ್ನ unaided_yaw, ಧ್ರುವ ಭಿನ್ನ deltatime)
{
	ಯದಿ (!PX4_ISFINITE(unaided_yaw)) {
		_unaided_yaw_was_invalid = ಸತ್ಯ;
		// If unaided yaw is not available we leave yaw_correction_ unchanged
		// Meaning yaw_setpoint - yaw_correction_prev + _yaw_correction = yaw_setpoint
		ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
	}

	// Detect the convergence phase of the yaw estimate by monitoring its relative
	// distance from an unaided yaw source.
	ಧ್ರುವ ಭಿನ್ನ yaw_error = wrap_pi(yaw - unaided_yaw);

	ಯದಿ (_unaided_yaw_was_invalid) {
		_unaided_yaw_was_invalid = ಅಸತ್ಯ;
		_yaw_error_lpf.reset(yaw_error);
		_yaw_error_ref = yaw_error;
		_yaw_correction = 0.f;
		_yaw_estimate_converging = ಅಸತ್ಯ;
		ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
	}

	// Run it through a high-pass filter to detect transients
	ಧ್ರುವ ಭಿನ್ನ yaw_error_hpf = wrap_pi(yaw_error - _yaw_error_lpf.getState());
	_yaw_error_lpf.update(yaw_error, deltatime);

	ಧ್ರುವ ತಾರ್ಕಿಕ was_converging = _yaw_estimate_converging;
	_yaw_estimate_converging = fabsf(yaw_error_hpf) > _kYawErrorChangeThreshold;

	ತಾರ್ಕಿಕ reset_setpoint = ಅಸತ್ಯ;

	ಯದಿ (!_yaw_estimate_converging) {
		_yaw_error_ref = yaw_error;

		ಯದಿ (was_converging) {
			// Force a reset of the locking mechanism
			reset_setpoint = ಸತ್ಯ;
		}
	}

	_yaw_correction = wrap_pi(yaw_error - _yaw_error_ref);

	ಪ್ರತ್ಯಾಗಮ reset_setpoint;
}

ಭಿನ್ನ StickYaw::updateYawLock(ಧ್ರುವ ಭಿನ್ನ yaw, ಧ್ರುವ ಭಿನ್ನ yawspeed_setpoint, ಧ್ರುವ ಭಿನ್ನ yaw_setpoint,
			      ಧ್ರುವ ಭಿನ್ನ yaw_correction_prev) ಧ್ರುವ
{
	// Yaw-lock depends on desired yawspeed input. If not locked, yaw_sp is set to NAN.
	ಯದಿ (fabsf(yawspeed_setpoint) > FLT_EPSILON) {
		// no fixed heading when rotating around yaw by stick
		ಪ್ರತ್ಯಾಗಮ NAN;

	} ಅನ್ಯಥಾ {
		// slow down and hold the current heading when no more rotation commanded
		ಯದಿ (!PX4_ISFINITE(yaw_setpoint)) {
			ಪ್ರತ್ಯಾಗಮ yaw;

		} ಅನ್ಯಥಾ {
			ಪ್ರತ್ಯಾಗಮ wrap_pi(yaw_setpoint - yaw_correction_prev + _yaw_correction);
		}
	}
}

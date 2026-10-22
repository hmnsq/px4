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

#अन्तर्भाव "StickYaw.hpp"

#अन्तर्भाव <px4_platform_common/defines.h>

using matrix::wrap_pi;

StickYaw::StickYaw(ModuleParams *parent) :
	ModuleParams(parent)
{}

शून्य StickYaw::reset(ध्रुव भिन्न yaw, ध्रुव भिन्न unaided_yaw)
{
	// The filter state outlives deactivation; a reset means there is no stick history to carry over.
	resetYawspeed();

	यदि (PX4_ISFINITE(unaided_yaw)) {
		_yaw_error_lpf.reset(wrap_pi(yaw - unaided_yaw));
	}
}

शून्य StickYaw::ekfResetHandler(ध्रुव भिन्न delta_yaw)
{
	_yaw_error_lpf.reset(wrap_pi(_yaw_error_lpf.getState() + delta_yaw));
	_yaw_error_ref = wrap_pi(_yaw_error_ref + delta_yaw);
}

शून्य StickYaw::generateYawSetpoint(भिन्न &yawspeed_setpoint, भिन्न &yaw_setpoint, ध्रुव भिन्न stick_yaw,
				   ध्रुव भिन्न yaw, ध्रुव भिन्न deltatime, ध्रुव भिन्न unaided_yaw)
{
	ध्रुव भिन्न yaw_correction_prev = _yaw_correction;
	ध्रुव तार्किक reset_setpoint = updateYawCorrection(yaw, unaided_yaw, deltatime);

	यदि (reset_setpoint) {
		yaw_setpoint = NAN;
	}

	_yawspeed_filter.setParameters(deltatime, _param_mpc_man_y_tau.get());
	ध्रुव भिन्न yawspeed_scale = math::min(math::radians(_param_mpc_man_y_max.get()), _yawspeed_constraint);
	yawspeed_setpoint = _yawspeed_filter.update(stick_yaw * yawspeed_scale);
	yaw_setpoint = updateYawLock(yaw, yawspeed_setpoint, yaw_setpoint, yaw_correction_prev);
}

तार्किक StickYaw::updateYawCorrection(ध्रुव भिन्न yaw, ध्रुव भिन्न unaided_yaw, ध्रुव भिन्न deltatime)
{
	यदि (!PX4_ISFINITE(unaided_yaw)) {
		_unaided_yaw_was_invalid = सत्य;
		// If unaided yaw is not available we leave yaw_correction_ unchanged
		// Meaning yaw_setpoint - yaw_correction_prev + _yaw_correction = yaw_setpoint
		प्रत्यागम असत्य;
	}

	// Detect the convergence phase of the yaw estimate by monitoring its relative
	// distance from an unaided yaw source.
	ध्रुव भिन्न yaw_error = wrap_pi(yaw - unaided_yaw);

	यदि (_unaided_yaw_was_invalid) {
		_unaided_yaw_was_invalid = असत्य;
		_yaw_error_lpf.reset(yaw_error);
		_yaw_error_ref = yaw_error;
		_yaw_correction = 0.f;
		_yaw_estimate_converging = असत्य;
		प्रत्यागम सत्य;
	}

	// Run it through a high-pass filter to detect transients
	ध्रुव भिन्न yaw_error_hpf = wrap_pi(yaw_error - _yaw_error_lpf.getState());
	_yaw_error_lpf.update(yaw_error, deltatime);

	ध्रुव तार्किक was_converging = _yaw_estimate_converging;
	_yaw_estimate_converging = fabsf(yaw_error_hpf) > _kYawErrorChangeThreshold;

	तार्किक reset_setpoint = असत्य;

	यदि (!_yaw_estimate_converging) {
		_yaw_error_ref = yaw_error;

		यदि (was_converging) {
			// Force a reset of the locking mechanism
			reset_setpoint = सत्य;
		}
	}

	_yaw_correction = wrap_pi(yaw_error - _yaw_error_ref);

	प्रत्यागम reset_setpoint;
}

भिन्न StickYaw::updateYawLock(ध्रुव भिन्न yaw, ध्रुव भिन्न yawspeed_setpoint, ध्रुव भिन्न yaw_setpoint,
			      ध्रुव भिन्न yaw_correction_prev) ध्रुव
{
	// Yaw-lock depends on desired yawspeed input. If not locked, yaw_sp is set to NAN.
	यदि (fabsf(yawspeed_setpoint) > FLT_EPSILON) {
		// no fixed heading when rotating around yaw by stick
		प्रत्यागम NAN;

	} अन्यथा {
		// slow down and hold the current heading when no more rotation commanded
		यदि (!PX4_ISFINITE(yaw_setpoint)) {
			प्रत्यागम yaw;

		} अन्यथा {
			प्रत्यागम wrap_pi(yaw_setpoint - yaw_correction_prev + _yaw_correction);
		}
	}
}

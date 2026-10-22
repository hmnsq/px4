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

#شامل "StickYaw.hpp"

#شامل <px4_platform_common/defines.h>

استعمال میٹرکس::wrap_pi;

StickYaw::StickYaw(ModuleParams *والد) :
	ModuleParams(والد)
{}

خالی StickYaw::reset(ثابت اعشاری yaw, ثابت اعشاری unaided_yaw)
{
	// The filter state outlives deactivation; a reset means there is no stick history to carry over.
	resetYawspeed();

	اگر (PX4_ISFINITE(unaided_yaw)) {
		_yaw_error_lpf.reset(wrap_pi(yaw - unaided_yaw));
	}
}

خالی StickYaw::ekfResetHandler(ثابت اعشاری delta_yaw)
{
	_yaw_error_lpf.reset(wrap_pi(_yaw_error_lpf.getState() + delta_yaw));
	_yaw_error_ref = wrap_pi(_yaw_error_ref + delta_yaw);
}

خالی StickYaw::generateYawSetpoint(اعشاری &yawspeed_setpoint, اعشاری &yaw_setpoint, ثابت اعشاری stick_yaw,
				   ثابت اعشاری yaw, ثابت اعشاری deltatime, ثابت اعشاری unaided_yaw)
{
	ثابت اعشاری yaw_correction_پچھلا = _yaw_correction;
	ثابت منطقی reset_setpoint = updateYawCorrection(yaw, unaided_yaw, deltatime);

	اگر (reset_setpoint) {
		yaw_setpoint = NAN;
	}

	_yawspeed_filter.سیٹ_Parameters(deltatime, _param_mpc_man_y_tau.get());
	ثابت اعشاری yawspeed_scale = math::min(math::radians(_param_mpc_man_y_max.get()), _yawspeed_constraint);
	yawspeed_setpoint = _yawspeed_filter.update(stick_yaw * yawspeed_scale);
	yaw_setpoint = update_Yaw_تالا(yaw, yawspeed_setpoint, yaw_setpoint, yaw_correction_پچھلا);
}

منطقی StickYaw::updateYawCorrection(ثابت اعشاری yaw, ثابت اعشاری unaided_yaw, ثابت اعشاری deltatime)
{
	اگر (!PX4_ISFINITE(unaided_yaw)) {
		_unaided_yaw_was_invalid = سچ;
		// If unaided yaw is not available we leave yaw_correction_ unchanged
		// Meaning yaw_setpoint - yaw_correction_prev + _yaw_correction = yaw_setpoint
		واپس جھوٹ;
	}

	// Detect the convergence phase of the yaw estimate by monitoring its relative
	// distance from an unaided yaw source.
	ثابت اعشاری yaw_error = wrap_pi(yaw - unaided_yaw);

	اگر (_unaided_yaw_was_invalid) {
		_unaided_yaw_was_invalid = جھوٹ;
		_yaw_error_lpf.reset(yaw_error);
		_yaw_error_ref = yaw_error;
		_yaw_correction = 0.f;
		_yaw_estimate_converging = جھوٹ;
		واپس سچ;
	}

	// Run it through a high-pass filter to detect transients
	ثابت اعشاری yaw_error_hpf = wrap_pi(yaw_error - _yaw_error_lpf.getState());
	_yaw_error_lpf.update(yaw_error, deltatime);

	ثابت منطقی was_converging = _yaw_estimate_converging;
	_yaw_estimate_converging = fabsf(yaw_error_hpf) > k_Yaw_Error_Change_حد;

	منطقی reset_setpoint = جھوٹ;

	اگر (!_yaw_estimate_converging) {
		_yaw_error_ref = yaw_error;

		اگر (was_converging) {
			// Force a reset of the locking mechanism
			reset_setpoint = سچ;
		}
	}

	_yaw_correction = wrap_pi(yaw_error - _yaw_error_ref);

	واپس reset_setpoint;
}

اعشاری StickYaw::update_Yaw_تالا(ثابت اعشاری yaw, ثابت اعشاری yawspeed_setpoint, ثابت اعشاری yaw_setpoint,
			      ثابت اعشاری yaw_correction_پچھلا) ثابت
{
	// Yaw-lock depends on desired yawspeed input. If not locked, yaw_sp is set to NAN.
	اگر (fabsf(yawspeed_setpoint) > FLT_EPSILON) {
		// no fixed heading when rotating around yaw by stick
		واپس NAN;

	} ورنہ {
		// slow down and hold the current heading when no more rotation commanded
		اگر (!PX4_ISFINITE(yaw_setpoint)) {
			واپس yaw;

		} ورنہ {
			واپس wrap_pi(yaw_setpoint - yaw_correction_پچھلا + _yaw_correction);
		}
	}
}

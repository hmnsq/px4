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

#অন্তর্ভাব "StickYaw.hpp"

#অন্তর্ভাব <px4_platform_common/defines.h>

using matrix::wrap_pi;

StickYaw::StickYaw(ModuleParams *parent) :
	ModuleParams(parent)
{}

শূন্য StickYaw::reset(ধ্রুবক দশমিক yaw, ধ্রুবক দশমিক unaided_yaw)
{
	// The filter state outlives deactivation; a reset means there is no stick history to carry over.
	resetYawspeed();

	যদি (PX4_ISFINITE(unaided_yaw)) {
		_yaw_error_lpf.reset(wrap_pi(yaw - unaided_yaw));
	}
}

শূন্য StickYaw::ekfResetHandler(ধ্রুবক দশমিক delta_yaw)
{
	_yaw_error_lpf.reset(wrap_pi(_yaw_error_lpf.getState() + delta_yaw));
	_yaw_error_ref = wrap_pi(_yaw_error_ref + delta_yaw);
}

শূন্য StickYaw::generateYawSetpoint(দশমিক &yawspeed_setpoint, দশমিক &yaw_setpoint, ধ্রুবক দশমিক stick_yaw,
				   ধ্রুবক দশমিক yaw, ধ্রুবক দশমিক deltatime, ধ্রুবক দশমিক unaided_yaw)
{
	ধ্রুবক দশমিক yaw_correction_পূর্ব = _yaw_correction;
	ধ্রুবক যৌক্তিক reset_setpoint = updateYawCorrection(yaw, unaided_yaw, deltatime);

	যদি (reset_setpoint) {
		yaw_setpoint = NAN;
	}

	_yawspeed_filter.setParameters(deltatime, _param_mpc_man_y_tau.get());
	ধ্রুবক দশমিক yawspeed_scale = math::min(math::radians(_param_mpc_man_y_max.get()), _yawspeed_constraint);
	yawspeed_setpoint = _yawspeed_filter.update(stick_yaw * yawspeed_scale);
	yaw_setpoint = updateYawLock(yaw, yawspeed_setpoint, yaw_setpoint, yaw_correction_পূর্ব);
}

যৌক্তিক StickYaw::updateYawCorrection(ধ্রুবক দশমিক yaw, ধ্রুবক দশমিক unaided_yaw, ধ্রুবক দশমিক deltatime)
{
	যদি (!PX4_ISFINITE(unaided_yaw)) {
		_unaided_yaw_was_invalid = সত্য;
		// If unaided yaw is not available we leave yaw_correction_ unchanged
		// Meaning yaw_setpoint - yaw_correction_prev + _yaw_correction = yaw_setpoint
		ফেরত মিথ্যা;
	}

	// Detect the convergence phase of the yaw estimate by monitoring its relative
	// distance from an unaided yaw source.
	ধ্রুবক দশমিক yaw_error = wrap_pi(yaw - unaided_yaw);

	যদি (_unaided_yaw_was_invalid) {
		_unaided_yaw_was_invalid = মিথ্যা;
		_yaw_error_lpf.reset(yaw_error);
		_yaw_error_ref = yaw_error;
		_yaw_correction = 0.f;
		_yaw_estimate_converging = মিথ্যা;
		ফেরত সত্য;
	}

	// Run it through a high-pass filter to detect transients
	ধ্রুবক দশমিক yaw_error_hpf = wrap_pi(yaw_error - _yaw_error_lpf.getState());
	_yaw_error_lpf.update(yaw_error, deltatime);

	ধ্রুবক যৌক্তিক was_converging = _yaw_estimate_converging;
	_yaw_estimate_converging = fabsf(yaw_error_hpf) > _kYawErrorChangeThreshold;

	যৌক্তিক reset_setpoint = মিথ্যা;

	যদি (!_yaw_estimate_converging) {
		_yaw_error_ref = yaw_error;

		যদি (was_converging) {
			// Force a reset of the locking mechanism
			reset_setpoint = সত্য;
		}
	}

	_yaw_correction = wrap_pi(yaw_error - _yaw_error_ref);

	ফেরত reset_setpoint;
}

দশমিক StickYaw::updateYawLock(ধ্রুবক দশমিক yaw, ধ্রুবক দশমিক yawspeed_setpoint, ধ্রুবক দশমিক yaw_setpoint,
			      ধ্রুবক দশমিক yaw_correction_পূর্ব) ধ্রুবক
{
	// Yaw-lock depends on desired yawspeed input. If not locked, yaw_sp is set to NAN.
	যদি (fabsf(yawspeed_setpoint) > FLT_EPSILON) {
		// no fixed heading when rotating around yaw by stick
		ফেরত NAN;

	} অন্যথা {
		// slow down and hold the current heading when no more rotation commanded
		যদি (!PX4_ISFINITE(yaw_setpoint)) {
			ফেরত yaw;

		} অন্যথা {
			ফেরত wrap_pi(yaw_setpoint - yaw_correction_পূর্ব + _yaw_correction);
		}
	}
}

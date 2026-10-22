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

/**
 * @file StickYaw.hpp
 * @brief Generate yaw and angular yawspeed setpoints from stick input
 * @author Matthias Grob <maetugr@gmail.com>
 */

#pragma once

#अन्तर्भाव <lib/mathlib/math/filter/AlphaFilter.hpp>
#अन्तर्भाव <px4_platform_common/module_params.h>

वग्ग StickYaw : सार्वजनिक ModuleParams
{
सार्वजनिक:
	StickYaw(ModuleParams *parent);
	~StickYaw() = स्वभाव;

	शून्य reset(भिन्न yaw, भिन्न unaided_yaw = NAN);

	शून्य resetYawspeed() { _yawspeed_filter.reset(0.f); }
	शून्य ekfResetHandler(भिन्न delta_yaw);
	शून्य generateYawSetpoint(भिन्न &yawspeed_setpoint, भिन्न &yaw_setpoint, भिन्न stick_yaw, भिन्न yaw, भिन्न deltatime,
				 भिन्न unaided_yaw = NAN);
	शून्य setYawspeedConstraint(भिन्न yawspeed) { _yawspeed_constraint = yawspeed; };

गुप्त:
	AlphaFilter<भिन्न> _yawspeed_filter;

	भिन्न _yaw_error_ref{0.f};
	भिन्न _yaw_correction{0.f};
	तार्किक _yaw_estimate_converging{मिच्छा};
	तार्किक _unaided_yaw_was_invalid{मिच्छा};
	AlphaFilter<भिन्न> _yaw_error_lpf{_kYawErrorTimeConstant}; ///< used to create a high-pass filter
	स्थिर constexpr भिन्न _kYawErrorTimeConstant{1.f}; ///< time constant of the high-pass filter used to detect yaw convergence
	स्थिर constexpr भिन्न _kYawErrorChangeThreshold{radians(1.f)}; ///< we consider the yaw estimate as "converging" when above this threshold

	तार्किक updateYawCorrection(भिन्न yaw, भिन्न unaided_yaw, भिन्न deltatime);

	/**
	 * Lock yaw when not currently turning
	 * When applying a yawspeed the vehicle is turning, when the speed is
	 * set to zero the vehicle needs to slow down and then lock at the yaw
	 * it stops at to not drift over time.
	 * @param yawspeed current yaw rotational rate state
	 * @param yaw current yaw rotational rate state
	 * @param yawspeed_setpoint rotation rate at which to turn around yaw axis
	 * @param yaw current yaw setpoint which then will be overwritten by the return value
	 * @return yaw setpoint to execute to have a yaw lock at the correct moment in time
	 */
	भिन्न updateYawLock(भिन्न yaw, भिन्न yawspeed_setpoint, भिन्न yaw_setpoint, भिन्न yaw_correction_prev) ध्रुव;

	भिन्न _yawspeed_constraint{INFINITY};

	DEFINE_PARAMETERS(
		(ParamFloat<px4::params::MPC_MAN_Y_MAX>) _param_mpc_man_y_max, ///< Maximum yaw speed with full stick deflection
		(ParamFloat<px4::params::MPC_MAN_Y_TAU>) _param_mpc_man_y_tau ///< time constant for yaw speed filtering
	)
};

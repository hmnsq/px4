/****************************************************************************
 *
 *   Copyright (c) 2019-2023 PX4 Development Team. All rights reserved.
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
 * @file rate_control.hpp
 *
 * PID 3 axis angular rate / angular velocity control.
 */

#رهنمود once

#ضمن <matrix/matrix/math.hpp>

#ضمن <mathlib/mathlib.h>
#ضمن <uORB/topics/rate_ctrl_status.h>

صنف نرخ_Control
{
عام:
	نرخ_Control() = افتراضي;
	~نرخ_Control() = افتراضي;

	/**
	 * Set the rate control PID gains
	 * @param P 3D vector of proportional gains for body x,y,z axis
	 * @param I 3D vector of integral gains
	 * @param D 3D vector of derivative gains
	 */
	فراغ مجموعه_Pid_Gains(ثابت ماتریس::Vector3f &P, ثابت ماتریس::Vector3f &I, ثابت ماتریس::Vector3f &D);

	/**
	 * Set the mximum absolute value of the integrator for all axes
	 * @param integrator_limit limit value for all axes x, y, z
	 */
	فراغ مجموعه_Integrator_حد(ثابت ماتریس::Vector3f &integrator_حد) { _lim_int = integrator_حد; };

	/**
	 * Set direct rate to torque feed forward gain
	 * @see _gain_ff
	 * @param FF 3D vector of feed forward gains for body x,y,z axis
	 */
	فراغ مجموعه_Feed_Forward_Gain(ثابت ماتریس::Vector3f &FF) { _gain_ff = FF; };

	/**
	 * Set saturation status
	 * @param control saturation vector from control allocator
	 */
	فراغ مجموعه_Saturation_وضعیت(ثابت ماتریس::Vector3<منطقي> &saturation_مثبت,
				 ثابت ماتریس::Vector3<منطقي> &saturation_negative);

	/**
	 * Set individual saturation flags
	 * @param axis 0 roll, 1 pitch, 2 yaw
	 * @param is_saturated value to update the flag with
	 */
	فراغ مجموعه_مثبت_Saturation_Flag(size_t axis, منطقي is_saturated);
	فراغ مجموعه_Negative_Saturation_Flag(size_t axis, منطقي is_saturated);

	/**
	 * Run one control loop cycle calculation
	 * @param rate estimation of the current vehicle angular rate
	 * @param rate_sp desired vehicle angular rate setpoint
	 * @param dt desired vehicle angular rate setpoint
	 * @return [-1,1] normalized torque vector to apply to the vehicle
	 */
	ماتریس::Vector3f update(ثابت ماتریس::Vector3f &نرخ, ثابت ماتریس::Vector3f &نرخ_sp,
				ثابت ماتریس::Vector3f &angular_accel, ثابت عائم dt, ثابت منطقي landed);

	/**
	 * Set the integral term to 0 to prevent windup
	 * @see _rate_int
	 */
	فراغ resetIntegral() { نرخ_int.zero(); }

	/**
	 * Set the integral term to 0 for specific axes
	 * @param  axis roll 0 / pitch 1 / yaw 2
	 * @see _rate_int
	 */
	فراغ resetIntegral(size_t axis)
	{
		إذا (axis < 3) {
			نرخ_int(axis) = 0.f;
		}
	}

	/**
	 * Get status message of controller for logging/debugging
	 * @param rate_ctrl_status status message to fill with internal states
	 */
	فراغ get_نرخ_Control_وضعیت(نرخ_ctrl_وضعیت_s &نرخ_ctrl_وضعیت);

خاص:
	فراغ updateIntegral(ماتریس::Vector3f &نرخ_error, ثابت عائم dt);

	// Gains
	ماتریس::Vector3f _gain_p; ///< rate control proportional gain for all axes x, y, z
	ماتریس::Vector3f _gain_i; ///< rate control integral gain
	ماتریس::Vector3f _gain_d; ///< rate control derivative gain
	ماتریس::Vector3f _lim_int; ///< integrator term maximum absolute value
	ماتریس::Vector3f _gain_ff; ///< direct rate to torque feed forward gain only useful for helicopters

	// States
	ماتریس::Vector3f نرخ_int; ///< integral term of the rate controller

	// Feedback from control allocation
	ماتریس::وکتور<منطقي, 3> _control_allocator_saturation_negative;
	ماتریس::وکتور<منطقي, 3> control_allocator_saturation_مثبت;
};

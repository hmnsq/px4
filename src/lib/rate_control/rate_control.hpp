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

#ਹਦਾਇਤ once

#ਅਨ੍ਤਰ੍ਭਾਵ <matrix/matrix/math.hpp>

#ਅਨ੍ਤਰ੍ਭਾਵ <mathlib/mathlib.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <uORB/topics/rate_ctrl_status.h>

ਵਰਗ ਦਰ_Control
{
ਜਨਤਕ:
	ਦਰ_Control() = ਮੂਲ;
	~ਦਰ_Control() = ਮੂਲ;

	/**
	 * Set the rate control PID gains
	 * @param P 3D vector of proportional gains for body x,y,z axis
	 * @param I 3D vector of integral gains
	 * @param D 3D vector of derivative gains
	 */
	ਖਾਲੀ ਸੈੱਟ_Pid_Gains(ਅਚਲ ਮੈਟ੍ਰਿਕਸ::Vector3f &P, ਅਚਲ ਮੈਟ੍ਰਿਕਸ::Vector3f &I, ਅਚਲ ਮੈਟ੍ਰਿਕਸ::Vector3f &D);

	/**
	 * Set the mximum absolute value of the integrator for all axes
	 * @param integrator_limit limit value for all axes x, y, z
	 */
	ਖਾਲੀ ਸੈੱਟ_Integrator_ਹੱਦਬੰਦੀ(ਅਚਲ ਮੈਟ੍ਰਿਕਸ::Vector3f &integrator_ਹੱਦਬੰਦੀ) { _lim_int = integrator_ਹੱਦਬੰਦੀ; };

	/**
	 * Set direct rate to torque feed forward gain
	 * @see _gain_ff
	 * @param FF 3D vector of feed forward gains for body x,y,z axis
	 */
	ਖਾਲੀ ਸੈੱਟ_Feed_Forward_Gain(ਅਚਲ ਮੈਟ੍ਰਿਕਸ::Vector3f &FF) { _gain_ff = FF; };

	/**
	 * Set saturation status
	 * @param control saturation vector from control allocator
	 */
	ਖਾਲੀ ਸੈੱਟ_Saturation_ਸਥਿਤੀ(ਅਚਲ ਮੈਟ੍ਰਿਕਸ::Vector3<ਤਾਰਕਿਕ> &saturation_ਸਕਾਰਾਤਮਕ,
				 ਅਚਲ ਮੈਟ੍ਰਿਕਸ::Vector3<ਤਾਰਕਿਕ> &saturation_negative);

	/**
	 * Set individual saturation flags
	 * @param axis 0 roll, 1 pitch, 2 yaw
	 * @param is_saturated value to update the flag with
	 */
	ਖਾਲੀ ਸੈੱਟ_ਸਕਾਰਾਤਮਕ_Saturation_Flag(size_t axis, ਤਾਰਕਿਕ is_saturated);
	ਖਾਲੀ ਸੈੱਟ_Negative_Saturation_Flag(size_t axis, ਤਾਰਕਿਕ is_saturated);

	/**
	 * Run one control loop cycle calculation
	 * @param rate estimation of the current vehicle angular rate
	 * @param rate_sp desired vehicle angular rate setpoint
	 * @param dt desired vehicle angular rate setpoint
	 * @return [-1,1] normalized torque vector to apply to the vehicle
	 */
	ਮੈਟ੍ਰਿਕਸ::Vector3f update(ਅਚਲ ਮੈਟ੍ਰਿਕਸ::Vector3f &ਦਰ, ਅਚਲ ਮੈਟ੍ਰਿਕਸ::Vector3f &ਦਰ_sp,
				ਅਚਲ ਮੈਟ੍ਰਿਕਸ::Vector3f &angular_accel, ਅਚਲ ਦਸ਼ਮਲਵ dt, ਅਚਲ ਤਾਰਕਿਕ landed);

	/**
	 * Set the integral term to 0 to prevent windup
	 * @see _rate_int
	 */
	ਖਾਲੀ resetIntegral() { ਦਰ_int.zero(); }

	/**
	 * Set the integral term to 0 for specific axes
	 * @param  axis roll 0 / pitch 1 / yaw 2
	 * @see _rate_int
	 */
	ਖਾਲੀ resetIntegral(size_t axis)
	{
		ਜੇ (axis < 3) {
			ਦਰ_int(axis) = 0.f;
		}
	}

	/**
	 * Get status message of controller for logging/debugging
	 * @param rate_ctrl_status status message to fill with internal states
	 */
	ਖਾਲੀ get_ਦਰ_Control_ਸਥਿਤੀ(ਦਰ_ctrl_ਸਥਿਤੀ_s &ਦਰ_ctrl_ਸਥਿਤੀ);

ਨਿਜੀ:
	ਖਾਲੀ updateIntegral(ਮੈਟ੍ਰਿਕਸ::Vector3f &ਦਰ_error, ਅਚਲ ਦਸ਼ਮਲਵ dt);

	// Gains
	ਮੈਟ੍ਰਿਕਸ::Vector3f _gain_p; ///< rate control proportional gain for all axes x, y, z
	ਮੈਟ੍ਰਿਕਸ::Vector3f _gain_i; ///< rate control integral gain
	ਮੈਟ੍ਰਿਕਸ::Vector3f _gain_d; ///< rate control derivative gain
	ਮੈਟ੍ਰਿਕਸ::Vector3f _lim_int; ///< integrator term maximum absolute value
	ਮੈਟ੍ਰਿਕਸ::Vector3f _gain_ff; ///< direct rate to torque feed forward gain only useful for helicopters

	// States
	ਮੈਟ੍ਰਿਕਸ::Vector3f ਦਰ_int; ///< integral term of the rate controller

	// Feedback from control allocation
	ਮੈਟ੍ਰਿਕਸ::ਸਦਿਸ਼<ਤਾਰਕਿਕ, 3> _control_allocator_saturation_negative;
	ਮੈਟ੍ਰਿਕਸ::ਸਦਿਸ਼<ਤਾਰਕਿਕ, 3> control_allocator_saturation_ਸਕਾਰਾਤਮਕ;
};

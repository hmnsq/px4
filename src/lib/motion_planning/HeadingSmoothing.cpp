/****************************************************************************
 *
 *   Copyright (c) 2023 PX4 Development Team. All rights reserved.
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

#அந்தர்பாவ "HeadingSmoothing.hpp"

HeadingSmoothing::HeadingSmoothing()
{
	_velocity_smoothing.setMaxVel(M_TWOPI_F); // "velocity" is heading. 2Pi limit is needed for correct angle wrapping
}

வெறுமை HeadingSmoothing::reset(மாறிலி தசம heading, மாறிலி தசம heading_rate)
{
	_velocity_smoothing.setCurrentVelocity(matrix::wrap_pi(heading));

	எனில் (PX4_ISFINITE(heading_rate)) {
		_velocity_smoothing.setCurrentAcceleration(heading_rate);
	}
}

வெறுமை HeadingSmoothing::update(மாறிலி தசம heading_setpoint, மாறிலி தசம time_elapsed)
{
	மாறிலி தசம delta_heading_wrapped = matrix::wrap_pi(heading_setpoint - getSmoothedHeading());
	மாறிலி தசம unwrapped_heading_setpoint = delta_heading_wrapped + getSmoothedHeading();

	_velocity_smoothing.updateDurations(unwrapped_heading_setpoint);
	_velocity_smoothing.updateTraj(time_elapsed);

	மாறிலி தசம wrapped_current_heading = matrix::wrap_pi(getSmoothedHeading());
	_velocity_smoothing.setCurrentVelocity(wrapped_current_heading);
}

/****************************************************************************
 *
 *   Copyright (c) 2019 PX4 Development Team. All rights reserved.
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
 * @file ManualVelocitySmoothingXY.hpp
 *
 */

#pragma once

#ଅନ୍ତର୍ଭାଵ <motion_planning/VelocitySmoothing.hpp>

#ଅନ୍ତର୍ଭାଵ <matrix/matrix/math.hpp>

using matrix::Vector2f;

ଶ୍ରେଣୀ ManualVelocitySmoothingXY final
{
ସର୍ବସାଧାରଣ:
	ManualVelocitySmoothingXY() = ପୂର୍ବନିର୍ଦ୍ଧାରିତ;
	~ManualVelocitySmoothingXY() = ପୂର୍ବନିର୍ଦ୍ଧାରିତ;

	ଶୂନ୍ୟ reset(ଅଚଳ Vector2f &accel, ଅଚଳ Vector2f &vel, ଅଚଳ Vector2f &pos);
	ଶୂନ୍ୟ update(ଦଶମିକ dt, ଅଚଳ Vector2f &velocity_target);

	ଶୂନ୍ୟ setVelSpFeedback(ଅଚଳ Vector2f &fb) { _velocity_setpoint_feedback = fb; }

	ଶୂନ୍ୟ setMaxJerk(ଅଚଳ ଦଶମିକ max_jerk)
	{
		_trajectory[0].setMaxJerk(max_jerk);
		_trajectory[1].setMaxJerk(max_jerk);
	}
	ଦଶମିକ getMaxJerk() ଅଚଳ { ଫେରନ୍ତୁ _trajectory[0].getMaxJerk(); }

	ଶୂନ୍ୟ setMaxAccel(ଅଚଳ ଦଶମିକ max_accel)
	{
		_trajectory[0].setMaxAccel(max_accel);
		_trajectory[1].setMaxAccel(max_accel);
	}
	ଦଶମିକ getMaxAccel() ଅଚଳ { ଫେରନ୍ତୁ _trajectory[0].getMaxAccel(); }

	ଶୂନ୍ୟ setMaxVel(ଅଚଳ ଦଶମିକ max_vel)
	{
		_trajectory[0].setMaxVel(max_vel);
		_trajectory[1].setMaxVel(max_vel);
	}
	ଦଶମିକ getMaxVel() ଅଚଳ { ଫେରନ୍ତୁ _trajectory[0].getMaxVel(); }

	Vector2f getCurrentJerk() ଅଚଳ { ଫେରନ୍ତୁ _state.j; }
	Vector2f getCurrentAcceleration() ଅଚଳ { ଫେରନ୍ତୁ _state.a; }

	ଶୂନ୍ୟ setCurrentVelocity(ଅଚଳ Vector2f &vel)
	{
		_state.v = vel;
		_trajectory[0].setCurrentVelocity(vel(0));
		_trajectory[1].setCurrentVelocity(vel(1));
	}
	Vector2f getCurrentVelocity() ଅଚଳ { ଫେରନ୍ତୁ _state.v; }

	ଶୂନ୍ୟ setCurrentPosition(ଅଚଳ Vector2f &pos)
	{
		_state.x = pos;
		_trajectory[0].setCurrentPosition(pos(0));
		_trajectory[1].setCurrentPosition(pos(1));
		_position_estimate = pos;

		ଯଦି (_position_lock_active) {
			_position_setpoint_locked = pos;
		}
	}
	Vector2f getCurrentPosition() ଅଚଳ { ଫେରନ୍ତୁ _position_setpoint_locked; }

	ଶୂନ୍ୟ setCurrentPositionEstimate(ଅଚଳ Vector2f &pos) { _position_estimate = pos; }

ବ୍ୟକ୍ତିଗତ:
	ଶୂନ୍ୟ resetPositionLock();
	ଶୂନ୍ୟ updateTrajectories(ଦଶମିକ dt);
	ଶୂନ୍ୟ checkPositionLock(ଅଚଳ Vector2f &velocity_target);
	ଶୂନ୍ୟ updateTrajDurations(ଅଚଳ Vector2f &velocity_target);

	VelocitySmoothing _trajectory[2]; ///< Trajectory in x and y directions

	ତାର୍କିକ _position_lock_active{ମିଥ୍ୟା};

	Vector2f _position_setpoint_locked{};

	Vector2f _velocity_setpoint_feedback{};
	Vector2f _position_estimate{};

	ସଂରଚନା {
		Vector2f j;
		Vector2f a;
		Vector2f v;
		Vector2f x;
	} _state{};
};

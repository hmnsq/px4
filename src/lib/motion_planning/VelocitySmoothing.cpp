/****************************************************************************
 *
 *   Copyright (c) 2018-2019 PX4 Development Team. All rights reserved.
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

#ಅನ್ತರ್ಭಾವ "VelocitySmoothing.hpp"

#ಅನ್ತರ್ಭಾವ <cstdio>
#ಅನ್ತರ್ಭಾವ <float.h>
#ಅನ್ತರ್ಭಾವ <mathlib/mathlib.h>
#ಅನ್ತರ್ಭಾವ <matrix/matrix/math.hpp>

using matrix::sign;

VelocitySmoothing::VelocitySmoothing(ಭಿನ್ನ initial_accel, ಭಿನ್ನ initial_vel, ಭಿನ್ನ initial_pos)
{
	reset(initial_accel, initial_vel, initial_pos);
}

ಶೂನ್ಯ VelocitySmoothing::reset(ಭಿನ್ನ accel, ಭಿನ್ನ vel, ಭಿನ್ನ pos)
{
	_state.j = 0.f;
	_state.a = accel;
	_state.v = vel;
	_state.x = pos;

	_state_init = _state;
}

ಭಿನ್ನ VelocitySmoothing::saturateT1ForAccel(ಭಿನ್ನ a0, ಭಿನ್ನ j_max, ಭಿನ್ನ T1, ಭಿನ್ನ a_max) ಧ್ರುವ
{
	/* Check maximum acceleration, saturate and recompute T1 if needed */
	ಭಿನ್ನ accel_T1 = a0 + j_max * T1;
	ಭಿನ್ನ T1_new = T1;

	ಯದಿ (accel_T1 > a_max) {
		T1_new = (a_max - a0) / j_max;

	} ಅನ್ಯಥಾ ಯದಿ (accel_T1 < -a_max) {
		T1_new = (-a_max - a0) / j_max;
	}

	ಪ್ರತ್ಯಾಗಮ T1_new;
}

ಭಿನ್ನ VelocitySmoothing::computeT1(ಭಿನ್ನ a0, ಭಿನ್ನ v3, ಭಿನ್ನ j_max, ಭಿನ್ನ a_max) ಧ್ರುವ
{
	ಭಿನ್ನ delta = 2.f * a0 * a0 + 4.f * j_max * v3;

	ಯದಿ (delta < 0.f) {
		// Solution is not real
		ಪ್ರತ್ಯಾಗಮ 0.f;
	}

	ಭಿನ್ನ sqrt_delta = sqrtf(delta);
	ಭಿನ್ನ T1_plus = (-a0 + 0.5f * sqrt_delta) / j_max;
	ಭಿನ್ನ T1_minus = (-a0 - 0.5f * sqrt_delta) / j_max;

	ಭಿನ್ನ T3_plus = a0 / j_max + T1_plus;
	ಭಿನ್ನ T3_minus = a0 / j_max + T1_minus;

	ಭಿನ್ನ T1 = 0.f;

	ಯದಿ (T1_plus >= 0.f && T3_plus >= 0.f) {
		T1 = T1_plus;

	} ಅನ್ಯಥಾ ಯದಿ (T1_minus >= 0.f && T3_minus >= 0.f) {
		T1 = T1_minus;
	}

	T1 = saturateT1ForAccel(a0, j_max, T1, a_max);

	ಪ್ರತ್ಯಾಗಮ math::max(T1, 0.f);
}

ಭಿನ್ನ VelocitySmoothing::computeT1(ಭಿನ್ನ T123, ಭಿನ್ನ a0, ಭಿನ್ನ v3, ಭಿನ್ನ j_max, ಭಿನ್ನ a_max) ಧ್ರುವ
{
	ಭಿನ್ನ a = -j_max;
	ಭಿನ್ನ b = j_max * T123 - a0;
	ಭಿನ್ನ delta = T123 * T123 * j_max * j_max + 2.f * T123 * a0 * j_max - a0 * a0 - 4.f * j_max * v3;

	ಯದಿ (delta < 0.f) {
		// Solution is not real
		ಪ್ರತ್ಯಾಗಮ 0.f;
	}

	ಭಿನ್ನ sqrt_delta = sqrtf(delta);
	ಭಿನ್ನ denominator_inv = 1.f / (2.f * a);
	ಭಿನ್ನ T1_plus = math::max((-b + sqrt_delta) * denominator_inv, 0.f);
	ಭಿನ್ನ T1_minus = math::max((-b - sqrt_delta) * denominator_inv, 0.f);

	ಭಿನ್ನ T3_plus = a0 / j_max + T1_plus;
	ಭಿನ್ನ T3_minus = a0 / j_max + T1_minus;

	ಭಿನ್ನ T1 = 0.f;

	ಯದಿ ((T1_plus >= 0.f && T3_plus >= 0.f) && ((T1_plus + T3_plus) <= T123)) {
		T1 = T1_plus;

	} ಅನ್ಯಥಾ ಯದಿ ((T1_minus >= 0.f && T3_minus >= 0.f) && ((T1_minus + T3_minus) <= T123)) {
		T1 = T1_minus;
	}

	T1 = saturateT1ForAccel(a0, j_max, T1, a_max);

	ಪ್ರತ್ಯಾಗಮ T1;
}


ಭಿನ್ನ VelocitySmoothing::computeT2(ಭಿನ್ನ T1, ಭಿನ್ನ T3, ಭಿನ್ನ a0, ಭಿನ್ನ v3, ಭಿನ್ನ j_max) ಧ್ರುವ
{
	ಭಿನ್ನ T2 = 0.f;

	ಭಿನ್ನ den = a0 + j_max * T1;

	ಯದಿ (math::abs_t(den) > FLT_EPSILON) {
		T2 = (-0.5f * T1 * T1 * j_max - T1 * T3 * j_max - T1 * a0 + 0.5f * T3 * T3 * j_max - T3 * a0 + v3) / den;
	}

	ಪ್ರತ್ಯಾಗಮ math::max(T2, 0.f);
}

ಭಿನ್ನ VelocitySmoothing::computeT2(ಭಿನ್ನ T123, ಭಿನ್ನ T1, ಭಿನ್ನ T3) ಧ್ರುವ
{
	ಭಿನ್ನ T2 = T123 - T1 - T3;
	ಪ್ರತ್ಯಾಗಮ math::max(T2, 0.f);
}

ಭಿನ್ನ VelocitySmoothing::computeT3(ಭಿನ್ನ T1, ಭಿನ್ನ a0, ಭಿನ್ನ j_max) ಧ್ರುವ
{
	ಭಿನ್ನ T3 = a0 / j_max + T1;
	ಪ್ರತ್ಯಾಗಮ math::max(T3, 0.f);
}

ಶೂನ್ಯ VelocitySmoothing::updateDurations(ಭಿನ್ನ vel_setpoint)
{
	_vel_sp = math::constrain(vel_setpoint, -_max_vel, _max_vel);
	_local_time = 0.f;
	_state_init = _state;

	_direction = computeDirection();

	updateDurationsMinimizeTotalTime();
}

ಪೂರ್ಣಾಙ್ಕ VelocitySmoothing::computeDirection() ಧ್ರುವ
{
	// Compute the velocity at which the trajectory will be
	// when the acceleration will be zero
	ಭಿನ್ನ vel_zero_acc = computeVelAtZeroAcc();

	/* Depending of the direction, start accelerating positively or negatively */
	ಪೂರ್ಣಾಙ್ಕ direction = sign(_vel_sp - vel_zero_acc);

	ಯದಿ (direction == 0) {
		// If by braking immediately the velocity is exactly
		// the required one with zero acceleration, then brake
		direction = sign(_state.a);
	}

	ಪ್ರತ್ಯಾಗಮ direction;
}

ಭಿನ್ನ VelocitySmoothing::computeVelAtZeroAcc() ಧ್ರುವ
{
	ಭಿನ್ನ vel_zero_acc = _state.v;

	ಯದಿ (fabsf(_state.a) > FLT_EPSILON) {
		ಭಿನ್ನ j_zero_acc = -sign(_state.a) * _max_jerk; // Required jerk to reduce the acceleration
		ಭಿನ್ನ t_zero_acc = -_state.a / j_zero_acc; // Required time to cancel the current acceleration
		vel_zero_acc = _state.v + _state.a * t_zero_acc + 0.5f * j_zero_acc * t_zero_acc * t_zero_acc;
	}

	ಪ್ರತ್ಯಾಗಮ vel_zero_acc;
}

ಶೂನ್ಯ VelocitySmoothing::updateDurationsMinimizeTotalTime()
{
	ಭಿನ್ನ jerk_max_T1 = _direction * _max_jerk;
	ಭಿನ್ನ delta_v = _vel_sp - _state.v;

	ಯದಿ (fabsf(jerk_max_T1) > FLT_EPSILON) { // zero direction or jerk would lead to division by zero
		// compute increasing acceleration time
		_T1 = computeT1(_state.a, delta_v, jerk_max_T1, _max_accel);

		// compute decreasing acceleration time
		_T3 = computeT3(_T1, _state.a, jerk_max_T1);

		// compute constant acceleration time
		_T2 = computeT2(_T1, _T3, _state.a, delta_v, jerk_max_T1);

	} ಅನ್ಯಥಾ {
		_T1 = _T2 = _T3 = 0.f;
	}
}

Trajectory VelocitySmoothing::evaluatePoly(ಭಿನ್ನ j, ಭಿನ್ನ a0, ಭಿನ್ನ v0, ಭಿನ್ನ x0, ಭಿನ್ನ t, ಪೂರ್ಣಾಙ್ಕ d) ಧ್ರುವ
{
	Trajectory traj;
	ಭಿನ್ನ jt = d * j;
	ಭಿನ್ನ t2 = t * t;
	ಭಿನ್ನ t3 = t2 * t;

	traj.j = jt;
	traj.a = a0 + jt * t;
	traj.v = v0 + a0 * t + 0.5f * jt * t2;
	traj.x = x0 + v0 * t + 0.5f * a0 * t2 + 1.f / 6.f * jt * t3;

	ಪ್ರತ್ಯಾಗಮ traj;
}

ಶೂನ್ಯ VelocitySmoothing::updateTraj(ಭಿನ್ನ dt, ಭಿನ್ನ time_stretch)
{
	_local_time += dt * time_stretch;
	ಭಿನ್ನ t_remain = _local_time;

	ಭಿನ್ನ t1 = math::min(t_remain, _T1);
	_state = evaluatePoly(_max_jerk, _state_init.a, _state_init.v, _state_init.x, t1, _direction);
	t_remain -= t1;

	ಯದಿ (t_remain > 0.f) {
		ಭಿನ್ನ t2 = math::min(t_remain, _T2);
		_state = evaluatePoly(0.f, _state.a, _state.v, _state.x, t2, 0.f);
		t_remain -= t2;
	}

	ಯದಿ (t_remain > 0.f) {
		ಭಿನ್ನ t3 = math::min(t_remain, _T3);
		_state = evaluatePoly(_max_jerk, _state.a, _state.v, _state.x, t3, -_direction);
		t_remain -= t3;
	}

	ಯದಿ (t_remain > 0.f) {
		_state = evaluatePoly(0.f, 0.f, _state.v, _state.x, t_remain, 0.f);
	}
}

ಶೂನ್ಯ VelocitySmoothing::timeSynchronization(VelocitySmoothing *traj, ಪೂರ್ಣಾಙ್ಕ n_traj)
{
	ಭಿನ್ನ desired_time = 0.f;
	ಪೂರ್ಣಾಙ್ಕ longest_traj_index = 0;

	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < n_traj; i++) {
		ಧ್ರುವ ಭಿನ್ನ T123 = traj[i].getTotalTime();

		ಯದಿ (T123 > desired_time) {
			desired_time = T123;
			longest_traj_index = i;
		}
	}

	ಯದಿ (desired_time > FLT_EPSILON) {
		ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < n_traj; i++) {
			ಯದಿ ((i != longest_traj_index)
			    && (traj[i].getTotalTime() < desired_time)) {
				traj[i].updateDurationsGivenTotalTime(desired_time);
			}
		}
	}
}

ಶೂನ್ಯ VelocitySmoothing::updateDurationsGivenTotalTime(ಭಿನ್ನ T123)
{
	ಭಿನ್ನ jerk_max_T1 = _direction * _max_jerk;
	ಭಿನ್ನ delta_v = _vel_sp - _state.v;

	ಯದಿ (fabsf(jerk_max_T1) > FLT_EPSILON) { // zero direction or jerk would lead to division by zero
		// compute increasing acceleration time
		_T1 = computeT1(T123, _state.a, delta_v, jerk_max_T1, _max_accel);

		// compute decreasing acceleration time
		_T3 = computeT3(_T1, _state.a, jerk_max_T1);

		// compute constant acceleration time
		_T2 = computeT2(T123, _T1, _T3);

	} ಅನ್ಯಥಾ {
		_T1 = _T2 = _T3 = 0.f;
	}
}

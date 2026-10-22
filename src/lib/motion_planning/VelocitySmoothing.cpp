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

#شامل "VelocitySmoothing.hpp"

#شامل <cstdio>
#شامل <float.h>
#شامل <mathlib/mathlib.h>
#شامل <matrix/matrix/math.hpp>

using matrix::sign;

VelocitySmoothing::VelocitySmoothing(اعشاری initial_accel, اعشاری initial_vel, اعشاری initial_pos)
{
	reset(initial_accel, initial_vel, initial_pos);
}

تهی VelocitySmoothing::reset(اعشاری accel, اعشاری vel, اعشاری pos)
{
	_state.j = 0.f;
	_state.a = accel;
	_state.v = vel;
	_state.x = pos;

	_state_init = _state;
}

اعشاری VelocitySmoothing::saturateT1ForAccel(اعشاری a0, اعشاری j_max, اعشاری T1, اعشاری a_max) ثابت
{
	/* Check maximum acceleration, saturate and recompute T1 if needed */
	اعشاری accel_T1 = a0 + j_max * T1;
	اعشاری T1_new = T1;

	اگر (accel_T1 > a_max) {
		T1_new = (a_max - a0) / j_max;

	} وگرنه اگر (accel_T1 < -a_max) {
		T1_new = (-a_max - a0) / j_max;
	}

	بازگشت T1_new;
}

اعشاری VelocitySmoothing::computeT1(اعشاری a0, اعشاری v3, اعشاری j_max, اعشاری a_max) ثابت
{
	اعشاری delta = 2.f * a0 * a0 + 4.f * j_max * v3;

	اگر (delta < 0.f) {
		// Solution is not real
		بازگشت 0.f;
	}

	اعشاری sqrt_delta = sqrtf(delta);
	اعشاری T1_plus = (-a0 + 0.5f * sqrt_delta) / j_max;
	اعشاری T1_minus = (-a0 - 0.5f * sqrt_delta) / j_max;

	اعشاری T3_plus = a0 / j_max + T1_plus;
	اعشاری T3_minus = a0 / j_max + T1_minus;

	اعشاری T1 = 0.f;

	اگر (T1_plus >= 0.f && T3_plus >= 0.f) {
		T1 = T1_plus;

	} وگرنه اگر (T1_minus >= 0.f && T3_minus >= 0.f) {
		T1 = T1_minus;
	}

	T1 = saturateT1ForAccel(a0, j_max, T1, a_max);

	بازگشت math::max(T1, 0.f);
}

اعشاری VelocitySmoothing::computeT1(اعشاری T123, اعشاری a0, اعشاری v3, اعشاری j_max, اعشاری a_max) ثابت
{
	اعشاری a = -j_max;
	اعشاری b = j_max * T123 - a0;
	اعشاری delta = T123 * T123 * j_max * j_max + 2.f * T123 * a0 * j_max - a0 * a0 - 4.f * j_max * v3;

	اگر (delta < 0.f) {
		// Solution is not real
		بازگشت 0.f;
	}

	اعشاری sqrt_delta = sqrtf(delta);
	اعشاری denominator_inv = 1.f / (2.f * a);
	اعشاری T1_plus = math::max((-b + sqrt_delta) * denominator_inv, 0.f);
	اعشاری T1_minus = math::max((-b - sqrt_delta) * denominator_inv, 0.f);

	اعشاری T3_plus = a0 / j_max + T1_plus;
	اعشاری T3_minus = a0 / j_max + T1_minus;

	اعشاری T1 = 0.f;

	اگر ((T1_plus >= 0.f && T3_plus >= 0.f) && ((T1_plus + T3_plus) <= T123)) {
		T1 = T1_plus;

	} وگرنه اگر ((T1_minus >= 0.f && T3_minus >= 0.f) && ((T1_minus + T3_minus) <= T123)) {
		T1 = T1_minus;
	}

	T1 = saturateT1ForAccel(a0, j_max, T1, a_max);

	بازگشت T1;
}


اعشاری VelocitySmoothing::computeT2(اعشاری T1, اعشاری T3, اعشاری a0, اعشاری v3, اعشاری j_max) ثابت
{
	اعشاری T2 = 0.f;

	اعشاری den = a0 + j_max * T1;

	اگر (math::abs_t(den) > FLT_EPSILON) {
		T2 = (-0.5f * T1 * T1 * j_max - T1 * T3 * j_max - T1 * a0 + 0.5f * T3 * T3 * j_max - T3 * a0 + v3) / den;
	}

	بازگشت math::max(T2, 0.f);
}

اعشاری VelocitySmoothing::computeT2(اعشاری T123, اعشاری T1, اعشاری T3) ثابت
{
	اعشاری T2 = T123 - T1 - T3;
	بازگشت math::max(T2, 0.f);
}

اعشاری VelocitySmoothing::computeT3(اعشاری T1, اعشاری a0, اعشاری j_max) ثابت
{
	اعشاری T3 = a0 / j_max + T1;
	بازگشت math::max(T3, 0.f);
}

تهی VelocitySmoothing::updateDurations(اعشاری vel_setpoint)
{
	_vel_sp = math::constrain(vel_setpoint, -_max_vel, _max_vel);
	_local_time = 0.f;
	_state_init = _state;

	_direction = computeDirection();

	updateDurationsMinimizeTotalTime();
}

صحیح VelocitySmoothing::computeDirection() ثابت
{
	// Compute the velocity at which the trajectory will be
	// when the acceleration will be zero
	اعشاری vel_zero_acc = computeVelAtZeroAcc();

	/* Depending of the direction, start accelerating positively or negatively */
	صحیح direction = sign(_vel_sp - vel_zero_acc);

	اگر (direction == 0) {
		// If by braking immediately the velocity is exactly
		// the required one with zero acceleration, then brake
		direction = sign(_state.a);
	}

	بازگشت direction;
}

اعشاری VelocitySmoothing::computeVelAtZeroAcc() ثابت
{
	اعشاری vel_zero_acc = _state.v;

	اگر (fabsf(_state.a) > FLT_EPSILON) {
		اعشاری j_zero_acc = -sign(_state.a) * _max_jerk; // Required jerk to reduce the acceleration
		اعشاری t_zero_acc = -_state.a / j_zero_acc; // Required time to cancel the current acceleration
		vel_zero_acc = _state.v + _state.a * t_zero_acc + 0.5f * j_zero_acc * t_zero_acc * t_zero_acc;
	}

	بازگشت vel_zero_acc;
}

تهی VelocitySmoothing::updateDurationsMinimizeTotalTime()
{
	اعشاری jerk_max_T1 = _direction * _max_jerk;
	اعشاری delta_v = _vel_sp - _state.v;

	اگر (fabsf(jerk_max_T1) > FLT_EPSILON) { // zero direction or jerk would lead to division by zero
		// compute increasing acceleration time
		_T1 = computeT1(_state.a, delta_v, jerk_max_T1, _max_accel);

		// compute decreasing acceleration time
		_T3 = computeT3(_T1, _state.a, jerk_max_T1);

		// compute constant acceleration time
		_T2 = computeT2(_T1, _T3, _state.a, delta_v, jerk_max_T1);

	} وگرنه {
		_T1 = _T2 = _T3 = 0.f;
	}
}

Trajectory VelocitySmoothing::evaluatePoly(اعشاری j, اعشاری a0, اعشاری v0, اعشاری x0, اعشاری t, صحیح d) ثابت
{
	Trajectory traj;
	اعشاری jt = d * j;
	اعشاری t2 = t * t;
	اعشاری t3 = t2 * t;

	traj.j = jt;
	traj.a = a0 + jt * t;
	traj.v = v0 + a0 * t + 0.5f * jt * t2;
	traj.x = x0 + v0 * t + 0.5f * a0 * t2 + 1.f / 6.f * jt * t3;

	بازگشت traj;
}

تهی VelocitySmoothing::updateTraj(اعشاری dt, اعشاری time_stretch)
{
	_local_time += dt * time_stretch;
	اعشاری t_remain = _local_time;

	اعشاری t1 = math::min(t_remain, _T1);
	_state = evaluatePoly(_max_jerk, _state_init.a, _state_init.v, _state_init.x, t1, _direction);
	t_remain -= t1;

	اگر (t_remain > 0.f) {
		اعشاری t2 = math::min(t_remain, _T2);
		_state = evaluatePoly(0.f, _state.a, _state.v, _state.x, t2, 0.f);
		t_remain -= t2;
	}

	اگر (t_remain > 0.f) {
		اعشاری t3 = math::min(t_remain, _T3);
		_state = evaluatePoly(_max_jerk, _state.a, _state.v, _state.x, t3, -_direction);
		t_remain -= t3;
	}

	اگر (t_remain > 0.f) {
		_state = evaluatePoly(0.f, 0.f, _state.v, _state.x, t_remain, 0.f);
	}
}

تهی VelocitySmoothing::timeSynchronization(VelocitySmoothing *traj, صحیح n_traj)
{
	اعشاری desired_time = 0.f;
	صحیح longest_traj_index = 0;

	برای (صحیح i = 0; i < n_traj; i++) {
		ثابت اعشاری T123 = traj[i].getTotalTime();

		اگر (T123 > desired_time) {
			desired_time = T123;
			longest_traj_index = i;
		}
	}

	اگر (desired_time > FLT_EPSILON) {
		برای (صحیح i = 0; i < n_traj; i++) {
			اگر ((i != longest_traj_index)
			    && (traj[i].getTotalTime() < desired_time)) {
				traj[i].updateDurationsGivenTotalTime(desired_time);
			}
		}
	}
}

تهی VelocitySmoothing::updateDurationsGivenTotalTime(اعشاری T123)
{
	اعشاری jerk_max_T1 = _direction * _max_jerk;
	اعشاری delta_v = _vel_sp - _state.v;

	اگر (fabsf(jerk_max_T1) > FLT_EPSILON) { // zero direction or jerk would lead to division by zero
		// compute increasing acceleration time
		_T1 = computeT1(T123, _state.a, delta_v, jerk_max_T1, _max_accel);

		// compute decreasing acceleration time
		_T3 = computeT3(_T1, _state.a, jerk_max_T1);

		// compute constant acceleration time
		_T2 = computeT2(T123, _T1, _T3);

	} وگرنه {
		_T1 = _T2 = _T3 = 0.f;
	}
}

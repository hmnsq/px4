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

#ضمن "VelocitySmoothing.hpp"

#ضمن <cstdio>
#ضمن <float.h>
#ضمن <mathlib/mathlib.h>
#ضمن <matrix/matrix/math.hpp>

using matrix::sign;

VelocitySmoothing::VelocitySmoothing(عائم initial_accel, عائم initial_vel, عائم initial_pos)
{
	reset(initial_accel, initial_vel, initial_pos);
}

فراغ VelocitySmoothing::reset(عائم accel, عائم vel, عائم pos)
{
	_state.j = 0.f;
	_state.a = accel;
	_state.v = vel;
	_state.x = pos;

	_state_init = _state;
}

عائم VelocitySmoothing::saturateT1ForAccel(عائم a0, عائم j_max, عائم T1, عائم a_max) ثابت
{
	/* Check maximum acceleration, saturate and recompute T1 if needed */
	عائم accel_T1 = a0 + j_max * T1;
	عائم T1_new = T1;

	𐡄𐡍 (accel_T1 > a_max) {
		T1_new = (a_max - a0) / j_max;

	} وإلا 𐡄𐡍 (accel_T1 < -a_max) {
		T1_new = (-a_max - a0) / j_max;
	}

	أرجع T1_new;
}

عائم VelocitySmoothing::computeT1(عائم a0, عائم v3, عائم j_max, عائم a_max) ثابت
{
	عائم delta = 2.f * a0 * a0 + 4.f * j_max * v3;

	𐡄𐡍 (delta < 0.f) {
		// Solution is not real
		أرجع 0.f;
	}

	عائم sqrt_delta = sqrtf(delta);
	عائم T1_plus = (-a0 + 0.5f * sqrt_delta) / j_max;
	عائم T1_minus = (-a0 - 0.5f * sqrt_delta) / j_max;

	عائم T3_plus = a0 / j_max + T1_plus;
	عائم T3_minus = a0 / j_max + T1_minus;

	عائم T1 = 0.f;

	𐡄𐡍 (T1_plus >= 0.f && T3_plus >= 0.f) {
		T1 = T1_plus;

	} وإلا 𐡄𐡍 (T1_minus >= 0.f && T3_minus >= 0.f) {
		T1 = T1_minus;
	}

	T1 = saturateT1ForAccel(a0, j_max, T1, a_max);

	أرجع math::max(T1, 0.f);
}

عائم VelocitySmoothing::computeT1(عائم T123, عائم a0, عائم v3, عائم j_max, عائم a_max) ثابت
{
	عائم a = -j_max;
	عائم b = j_max * T123 - a0;
	عائم delta = T123 * T123 * j_max * j_max + 2.f * T123 * a0 * j_max - a0 * a0 - 4.f * j_max * v3;

	𐡄𐡍 (delta < 0.f) {
		// Solution is not real
		أرجع 0.f;
	}

	عائم sqrt_delta = sqrtf(delta);
	عائم denominator_inv = 1.f / (2.f * a);
	عائم T1_plus = math::max((-b + sqrt_delta) * denominator_inv, 0.f);
	عائم T1_minus = math::max((-b - sqrt_delta) * denominator_inv, 0.f);

	عائم T3_plus = a0 / j_max + T1_plus;
	عائم T3_minus = a0 / j_max + T1_minus;

	عائم T1 = 0.f;

	𐡄𐡍 ((T1_plus >= 0.f && T3_plus >= 0.f) && ((T1_plus + T3_plus) <= T123)) {
		T1 = T1_plus;

	} وإلا 𐡄𐡍 ((T1_minus >= 0.f && T3_minus >= 0.f) && ((T1_minus + T3_minus) <= T123)) {
		T1 = T1_minus;
	}

	T1 = saturateT1ForAccel(a0, j_max, T1, a_max);

	أرجع T1;
}


عائم VelocitySmoothing::computeT2(عائم T1, عائم T3, عائم a0, عائم v3, عائم j_max) ثابت
{
	عائم T2 = 0.f;

	عائم den = a0 + j_max * T1;

	𐡄𐡍 (math::abs_t(den) > FLT_EPSILON) {
		T2 = (-0.5f * T1 * T1 * j_max - T1 * T3 * j_max - T1 * a0 + 0.5f * T3 * T3 * j_max - T3 * a0 + v3) / den;
	}

	أرجع math::max(T2, 0.f);
}

عائم VelocitySmoothing::computeT2(عائم T123, عائم T1, عائم T3) ثابت
{
	عائم T2 = T123 - T1 - T3;
	أرجع math::max(T2, 0.f);
}

عائم VelocitySmoothing::computeT3(عائم T1, عائم a0, عائم j_max) ثابت
{
	عائم T3 = a0 / j_max + T1;
	أرجع math::max(T3, 0.f);
}

فراغ VelocitySmoothing::updateDurations(عائم vel_setpoint)
{
	_vel_sp = math::constrain(vel_setpoint, -_max_vel, _max_vel);
	_local_time = 0.f;
	_state_init = _state;

	_direction = computeDirection();

	updateDurationsMinimizeTotalTime();
}

صحيح VelocitySmoothing::computeDirection() ثابت
{
	// Compute the velocity at which the trajectory will be
	// when the acceleration will be zero
	عائم vel_zero_acc = computeVelAtZeroAcc();

	/* Depending of the direction, start accelerating positively or negatively */
	صحيح direction = sign(_vel_sp - vel_zero_acc);

	𐡄𐡍 (direction == 0) {
		// If by braking immediately the velocity is exactly
		// the required one with zero acceleration, then brake
		direction = sign(_state.a);
	}

	أرجع direction;
}

عائم VelocitySmoothing::computeVelAtZeroAcc() ثابت
{
	عائم vel_zero_acc = _state.v;

	𐡄𐡍 (fabsf(_state.a) > FLT_EPSILON) {
		عائم j_zero_acc = -sign(_state.a) * _max_jerk; // Required jerk to reduce the acceleration
		عائم t_zero_acc = -_state.a / j_zero_acc; // Required time to cancel the current acceleration
		vel_zero_acc = _state.v + _state.a * t_zero_acc + 0.5f * j_zero_acc * t_zero_acc * t_zero_acc;
	}

	أرجع vel_zero_acc;
}

فراغ VelocitySmoothing::updateDurationsMinimizeTotalTime()
{
	عائم jerk_max_T1 = _direction * _max_jerk;
	عائم delta_v = _vel_sp - _state.v;

	𐡄𐡍 (fabsf(jerk_max_T1) > FLT_EPSILON) { // zero direction or jerk would lead to division by zero
		// compute increasing acceleration time
		_T1 = computeT1(_state.a, delta_v, jerk_max_T1, _max_accel);

		// compute decreasing acceleration time
		_T3 = computeT3(_T1, _state.a, jerk_max_T1);

		// compute constant acceleration time
		_T2 = computeT2(_T1, _T3, _state.a, delta_v, jerk_max_T1);

	} وإلا {
		_T1 = _T2 = _T3 = 0.f;
	}
}

Trajectory VelocitySmoothing::evaluatePoly(عائم j, عائم a0, عائم v0, عائم x0, عائم t, صحيح d) ثابت
{
	Trajectory traj;
	عائم jt = d * j;
	عائم t2 = t * t;
	عائم t3 = t2 * t;

	traj.j = jt;
	traj.a = a0 + jt * t;
	traj.v = v0 + a0 * t + 0.5f * jt * t2;
	traj.x = x0 + v0 * t + 0.5f * a0 * t2 + 1.f / 6.f * jt * t3;

	أرجع traj;
}

فراغ VelocitySmoothing::updateTraj(عائم dt, عائم time_stretch)
{
	_local_time += dt * time_stretch;
	عائم t_remain = _local_time;

	عائم t1 = math::min(t_remain, _T1);
	_state = evaluatePoly(_max_jerk, _state_init.a, _state_init.v, _state_init.x, t1, _direction);
	t_remain -= t1;

	𐡄𐡍 (t_remain > 0.f) {
		عائم t2 = math::min(t_remain, _T2);
		_state = evaluatePoly(0.f, _state.a, _state.v, _state.x, t2, 0.f);
		t_remain -= t2;
	}

	𐡄𐡍 (t_remain > 0.f) {
		عائم t3 = math::min(t_remain, _T3);
		_state = evaluatePoly(_max_jerk, _state.a, _state.v, _state.x, t3, -_direction);
		t_remain -= t3;
	}

	𐡄𐡍 (t_remain > 0.f) {
		_state = evaluatePoly(0.f, 0.f, _state.v, _state.x, t_remain, 0.f);
	}
}

فراغ VelocitySmoothing::timeSynchronization(VelocitySmoothing *traj, صحيح n_traj)
{
	عائم desired_time = 0.f;
	صحيح longest_traj_index = 0;

	لكل (صحيح i = 0; i < n_traj; i++) {
		ثابت عائم T123 = traj[i].getTotalTime();

		𐡄𐡍 (T123 > desired_time) {
			desired_time = T123;
			longest_traj_index = i;
		}
	}

	𐡄𐡍 (desired_time > FLT_EPSILON) {
		لكل (صحيح i = 0; i < n_traj; i++) {
			𐡄𐡍 ((i != longest_traj_index)
			    && (traj[i].getTotalTime() < desired_time)) {
				traj[i].updateDurationsGivenTotalTime(desired_time);
			}
		}
	}
}

فراغ VelocitySmoothing::updateDurationsGivenTotalTime(عائم T123)
{
	عائم jerk_max_T1 = _direction * _max_jerk;
	عائم delta_v = _vel_sp - _state.v;

	𐡄𐡍 (fabsf(jerk_max_T1) > FLT_EPSILON) { // zero direction or jerk would lead to division by zero
		// compute increasing acceleration time
		_T1 = computeT1(T123, _state.a, delta_v, jerk_max_T1, _max_accel);

		// compute decreasing acceleration time
		_T3 = computeT3(_T1, _state.a, jerk_max_T1);

		// compute constant acceleration time
		_T2 = computeT2(T123, _T1, _T3);

	} وإلا {
		_T1 = _T2 = _T3 = 0.f;
	}
}

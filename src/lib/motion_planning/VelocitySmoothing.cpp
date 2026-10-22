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

#અન્તર્ભાવ "VelocitySmoothing.hpp"

#અન્તર્ભાવ <cstdio>
#અન્તર્ભાવ <float.h>
#અન્તર્ભાવ <mathlib/mathlib.h>
#અન્તર્ભાવ <matrix/matrix/math.hpp>

using matrix::sign;

VelocitySmoothing::VelocitySmoothing(દશાંશ initial_accel, દશાંશ initial_vel, દશાંશ initial_pos)
{
	reset(initial_accel, initial_vel, initial_pos);
}

શૂન્ય VelocitySmoothing::reset(દશાંશ accel, દશાંશ vel, દશાંશ pos)
{
	_state.j = 0.f;
	_state.a = accel;
	_state.v = vel;
	_state.x = pos;

	_state_init = _state;
}

દશાંશ VelocitySmoothing::saturateT1ForAccel(દશાંશ a0, દશાંશ j_max, દશાંશ T1, દશાંશ a_max) અચલ
{
	/* Check maximum acceleration, saturate and recompute T1 if needed */
	દશાંશ accel_T1 = a0 + j_max * T1;
	દશાંશ T1_new = T1;

	જો (accel_T1 > a_max) {
		T1_new = (a_max - a0) / j_max;

	} નહીંતર જો (accel_T1 < -a_max) {
		T1_new = (-a_max - a0) / j_max;
	}

	પાછા T1_new;
}

દશાંશ VelocitySmoothing::computeT1(દશાંશ a0, દશાંશ v3, દશાંશ j_max, દશાંશ a_max) અચલ
{
	દશાંશ delta = 2.f * a0 * a0 + 4.f * j_max * v3;

	જો (delta < 0.f) {
		// Solution is not real
		પાછા 0.f;
	}

	દશાંશ sqrt_delta = sqrtf(delta);
	દશાંશ T1_plus = (-a0 + 0.5f * sqrt_delta) / j_max;
	દશાંશ T1_minus = (-a0 - 0.5f * sqrt_delta) / j_max;

	દશાંશ T3_plus = a0 / j_max + T1_plus;
	દશાંશ T3_minus = a0 / j_max + T1_minus;

	દશાંશ T1 = 0.f;

	જો (T1_plus >= 0.f && T3_plus >= 0.f) {
		T1 = T1_plus;

	} નહીંતર જો (T1_minus >= 0.f && T3_minus >= 0.f) {
		T1 = T1_minus;
	}

	T1 = saturateT1ForAccel(a0, j_max, T1, a_max);

	પાછા math::max(T1, 0.f);
}

દશાંશ VelocitySmoothing::computeT1(દશાંશ T123, દશાંશ a0, દશાંશ v3, દશાંશ j_max, દશાંશ a_max) અચલ
{
	દશાંશ a = -j_max;
	દશાંશ b = j_max * T123 - a0;
	દશાંશ delta = T123 * T123 * j_max * j_max + 2.f * T123 * a0 * j_max - a0 * a0 - 4.f * j_max * v3;

	જો (delta < 0.f) {
		// Solution is not real
		પાછા 0.f;
	}

	દશાંશ sqrt_delta = sqrtf(delta);
	દશાંશ denominator_inv = 1.f / (2.f * a);
	દશાંશ T1_plus = math::max((-b + sqrt_delta) * denominator_inv, 0.f);
	દશાંશ T1_minus = math::max((-b - sqrt_delta) * denominator_inv, 0.f);

	દશાંશ T3_plus = a0 / j_max + T1_plus;
	દશાંશ T3_minus = a0 / j_max + T1_minus;

	દશાંશ T1 = 0.f;

	જો ((T1_plus >= 0.f && T3_plus >= 0.f) && ((T1_plus + T3_plus) <= T123)) {
		T1 = T1_plus;

	} નહીંતર જો ((T1_minus >= 0.f && T3_minus >= 0.f) && ((T1_minus + T3_minus) <= T123)) {
		T1 = T1_minus;
	}

	T1 = saturateT1ForAccel(a0, j_max, T1, a_max);

	પાછા T1;
}


દશાંશ VelocitySmoothing::computeT2(દશાંશ T1, દશાંશ T3, દશાંશ a0, દશાંશ v3, દશાંશ j_max) અચલ
{
	દશાંશ T2 = 0.f;

	દશાંશ den = a0 + j_max * T1;

	જો (math::abs_t(den) > FLT_EPSILON) {
		T2 = (-0.5f * T1 * T1 * j_max - T1 * T3 * j_max - T1 * a0 + 0.5f * T3 * T3 * j_max - T3 * a0 + v3) / den;
	}

	પાછા math::max(T2, 0.f);
}

દશાંશ VelocitySmoothing::computeT2(દશાંશ T123, દશાંશ T1, દશાંશ T3) અચલ
{
	દશાંશ T2 = T123 - T1 - T3;
	પાછા math::max(T2, 0.f);
}

દશાંશ VelocitySmoothing::computeT3(દશાંશ T1, દશાંશ a0, દશાંશ j_max) અચલ
{
	દશાંશ T3 = a0 / j_max + T1;
	પાછા math::max(T3, 0.f);
}

શૂન્ય VelocitySmoothing::updateDurations(દશાંશ vel_setpoint)
{
	_vel_sp = math::constrain(vel_setpoint, -_max_vel, _max_vel);
	_local_time = 0.f;
	_state_init = _state;

	_direction = computeDirection();

	updateDurationsMinimizeTotalTime();
}

પૂર્ણાંક VelocitySmoothing::computeDirection() અચલ
{
	// Compute the velocity at which the trajectory will be
	// when the acceleration will be zero
	દશાંશ vel_zero_acc = computeVelAtZeroAcc();

	/* Depending of the direction, start accelerating positively or negatively */
	પૂર્ણાંક direction = sign(_vel_sp - vel_zero_acc);

	જો (direction == 0) {
		// If by braking immediately the velocity is exactly
		// the required one with zero acceleration, then brake
		direction = sign(_state.a);
	}

	પાછા direction;
}

દશાંશ VelocitySmoothing::computeVelAtZeroAcc() અચલ
{
	દશાંશ vel_zero_acc = _state.v;

	જો (fabsf(_state.a) > FLT_EPSILON) {
		દશાંશ j_zero_acc = -sign(_state.a) * _max_jerk; // Required jerk to reduce the acceleration
		દશાંશ t_zero_acc = -_state.a / j_zero_acc; // Required time to cancel the current acceleration
		vel_zero_acc = _state.v + _state.a * t_zero_acc + 0.5f * j_zero_acc * t_zero_acc * t_zero_acc;
	}

	પાછા vel_zero_acc;
}

શૂન્ય VelocitySmoothing::updateDurationsMinimizeTotalTime()
{
	દશાંશ jerk_max_T1 = _direction * _max_jerk;
	દશાંશ delta_v = _vel_sp - _state.v;

	જો (fabsf(jerk_max_T1) > FLT_EPSILON) { // zero direction or jerk would lead to division by zero
		// compute increasing acceleration time
		_T1 = computeT1(_state.a, delta_v, jerk_max_T1, _max_accel);

		// compute decreasing acceleration time
		_T3 = computeT3(_T1, _state.a, jerk_max_T1);

		// compute constant acceleration time
		_T2 = computeT2(_T1, _T3, _state.a, delta_v, jerk_max_T1);

	} નહીંતર {
		_T1 = _T2 = _T3 = 0.f;
	}
}

Trajectory VelocitySmoothing::evaluatePoly(દશાંશ j, દશાંશ a0, દશાંશ v0, દશાંશ x0, દશાંશ t, પૂર્ણાંક d) અચલ
{
	Trajectory traj;
	દશાંશ jt = d * j;
	દશાંશ t2 = t * t;
	દશાંશ t3 = t2 * t;

	traj.j = jt;
	traj.a = a0 + jt * t;
	traj.v = v0 + a0 * t + 0.5f * jt * t2;
	traj.x = x0 + v0 * t + 0.5f * a0 * t2 + 1.f / 6.f * jt * t3;

	પાછા traj;
}

શૂન્ય VelocitySmoothing::updateTraj(દશાંશ dt, દશાંશ time_stretch)
{
	_local_time += dt * time_stretch;
	દશાંશ t_remain = _local_time;

	દશાંશ t1 = math::min(t_remain, _T1);
	_state = evaluatePoly(_max_jerk, _state_init.a, _state_init.v, _state_init.x, t1, _direction);
	t_remain -= t1;

	જો (t_remain > 0.f) {
		દશાંશ t2 = math::min(t_remain, _T2);
		_state = evaluatePoly(0.f, _state.a, _state.v, _state.x, t2, 0.f);
		t_remain -= t2;
	}

	જો (t_remain > 0.f) {
		દશાંશ t3 = math::min(t_remain, _T3);
		_state = evaluatePoly(_max_jerk, _state.a, _state.v, _state.x, t3, -_direction);
		t_remain -= t3;
	}

	જો (t_remain > 0.f) {
		_state = evaluatePoly(0.f, 0.f, _state.v, _state.x, t_remain, 0.f);
	}
}

શૂન્ય VelocitySmoothing::timeSynchronization(VelocitySmoothing *traj, પૂર્ણાંક n_traj)
{
	દશાંશ desired_time = 0.f;
	પૂર્ણાંક longest_traj_index = 0;

	માટે (પૂર્ણાંક i = 0; i < n_traj; i++) {
		અચલ દશાંશ T123 = traj[i].getTotalTime();

		જો (T123 > desired_time) {
			desired_time = T123;
			longest_traj_index = i;
		}
	}

	જો (desired_time > FLT_EPSILON) {
		માટે (પૂર્ણાંક i = 0; i < n_traj; i++) {
			જો ((i != longest_traj_index)
			    && (traj[i].getTotalTime() < desired_time)) {
				traj[i].updateDurationsGivenTotalTime(desired_time);
			}
		}
	}
}

શૂન્ય VelocitySmoothing::updateDurationsGivenTotalTime(દશાંશ T123)
{
	દશાંશ jerk_max_T1 = _direction * _max_jerk;
	દશાંશ delta_v = _vel_sp - _state.v;

	જો (fabsf(jerk_max_T1) > FLT_EPSILON) { // zero direction or jerk would lead to division by zero
		// compute increasing acceleration time
		_T1 = computeT1(T123, _state.a, delta_v, jerk_max_T1, _max_accel);

		// compute decreasing acceleration time
		_T3 = computeT3(_T1, _state.a, jerk_max_T1);

		// compute constant acceleration time
		_T2 = computeT2(T123, _T1, _T3);

	} નહીંતર {
		_T1 = _T2 = _T3 = 0.f;
	}
}

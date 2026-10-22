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

#অন্তর্ভাৱ "VelocitySmoothing.hpp"

#অন্তর্ভাৱ <cstdio>
#অন্তর্ভাৱ <float.h>
#অন্তর্ভাৱ <mathlib/mathlib.h>
#অন্তর্ভাৱ <matrix/matrix/math.hpp>

using matrix::sign;

VelocitySmoothing::VelocitySmoothing(ভিন্ন initial_accel, ভিন্ন initial_vel, ভিন্ন initial_pos)
{
	reset(initial_accel, initial_vel, initial_pos);
}

শূন্য VelocitySmoothing::reset(ভিন্ন accel, ভিন্ন vel, ভিন্ন pos)
{
	_state.j = 0.f;
	_state.a = accel;
	_state.v = vel;
	_state.x = pos;

	_state_init = _state;
}

ভিন্ন VelocitySmoothing::saturateT1ForAccel(ভিন্ন a0, ভিন্ন j_max, ভিন্ন T1, ভিন্ন a_max) ধ্রুৱ
{
	/* Check maximum acceleration, saturate and recompute T1 if needed */
	ভিন্ন accel_T1 = a0 + j_max * T1;
	ভিন্ন T1_new = T1;

	যদি (accel_T1 > a_max) {
		T1_new = (a_max - a0) / j_max;

	} অন্যথা যদি (accel_T1 < -a_max) {
		T1_new = (-a_max - a0) / j_max;
	}

	ঘূৰাই T1_new;
}

ভিন্ন VelocitySmoothing::computeT1(ভিন্ন a0, ভিন্ন v3, ভিন্ন j_max, ভিন্ন a_max) ধ্রুৱ
{
	ভিন্ন delta = 2.f * a0 * a0 + 4.f * j_max * v3;

	যদি (delta < 0.f) {
		// Solution is not real
		ঘূৰাই 0.f;
	}

	ভিন্ন sqrt_delta = sqrtf(delta);
	ভিন্ন T1_plus = (-a0 + 0.5f * sqrt_delta) / j_max;
	ভিন্ন T1_minus = (-a0 - 0.5f * sqrt_delta) / j_max;

	ভিন্ন T3_plus = a0 / j_max + T1_plus;
	ভিন্ন T3_minus = a0 / j_max + T1_minus;

	ভিন্ন T1 = 0.f;

	যদি (T1_plus >= 0.f && T3_plus >= 0.f) {
		T1 = T1_plus;

	} অন্যথা যদি (T1_minus >= 0.f && T3_minus >= 0.f) {
		T1 = T1_minus;
	}

	T1 = saturateT1ForAccel(a0, j_max, T1, a_max);

	ঘূৰাই math::max(T1, 0.f);
}

ভিন্ন VelocitySmoothing::computeT1(ভিন্ন T123, ভিন্ন a0, ভিন্ন v3, ভিন্ন j_max, ভিন্ন a_max) ধ্রুৱ
{
	ভিন্ন a = -j_max;
	ভিন্ন b = j_max * T123 - a0;
	ভিন্ন delta = T123 * T123 * j_max * j_max + 2.f * T123 * a0 * j_max - a0 * a0 - 4.f * j_max * v3;

	যদি (delta < 0.f) {
		// Solution is not real
		ঘূৰাই 0.f;
	}

	ভিন্ন sqrt_delta = sqrtf(delta);
	ভিন্ন denominator_inv = 1.f / (2.f * a);
	ভিন্ন T1_plus = math::max((-b + sqrt_delta) * denominator_inv, 0.f);
	ভিন্ন T1_minus = math::max((-b - sqrt_delta) * denominator_inv, 0.f);

	ভিন্ন T3_plus = a0 / j_max + T1_plus;
	ভিন্ন T3_minus = a0 / j_max + T1_minus;

	ভিন্ন T1 = 0.f;

	যদি ((T1_plus >= 0.f && T3_plus >= 0.f) && ((T1_plus + T3_plus) <= T123)) {
		T1 = T1_plus;

	} অন্যথা যদি ((T1_minus >= 0.f && T3_minus >= 0.f) && ((T1_minus + T3_minus) <= T123)) {
		T1 = T1_minus;
	}

	T1 = saturateT1ForAccel(a0, j_max, T1, a_max);

	ঘূৰাই T1;
}


ভিন্ন VelocitySmoothing::computeT2(ভিন্ন T1, ভিন্ন T3, ভিন্ন a0, ভিন্ন v3, ভিন্ন j_max) ধ্রুৱ
{
	ভিন্ন T2 = 0.f;

	ভিন্ন den = a0 + j_max * T1;

	যদি (math::abs_t(den) > FLT_EPSILON) {
		T2 = (-0.5f * T1 * T1 * j_max - T1 * T3 * j_max - T1 * a0 + 0.5f * T3 * T3 * j_max - T3 * a0 + v3) / den;
	}

	ঘূৰাই math::max(T2, 0.f);
}

ভিন্ন VelocitySmoothing::computeT2(ভিন্ন T123, ভিন্ন T1, ভিন্ন T3) ধ্রুৱ
{
	ভিন্ন T2 = T123 - T1 - T3;
	ঘূৰাই math::max(T2, 0.f);
}

ভিন্ন VelocitySmoothing::computeT3(ভিন্ন T1, ভিন্ন a0, ভিন্ন j_max) ধ্রুৱ
{
	ভিন্ন T3 = a0 / j_max + T1;
	ঘূৰাই math::max(T3, 0.f);
}

শূন্য VelocitySmoothing::updateDurations(ভিন্ন vel_setpoint)
{
	_vel_sp = math::constrain(vel_setpoint, -_max_vel, _max_vel);
	_local_time = 0.f;
	_state_init = _state;

	_direction = computeDirection();

	updateDurationsMinimizeTotalTime();
}

পূর্ণাঙ্ক VelocitySmoothing::computeDirection() ধ্রুৱ
{
	// Compute the velocity at which the trajectory will be
	// when the acceleration will be zero
	ভিন্ন vel_zero_acc = computeVelAtZeroAcc();

	/* Depending of the direction, start accelerating positively or negatively */
	পূর্ণাঙ্ক direction = sign(_vel_sp - vel_zero_acc);

	যদি (direction == 0) {
		// If by braking immediately the velocity is exactly
		// the required one with zero acceleration, then brake
		direction = sign(_state.a);
	}

	ঘূৰাই direction;
}

ভিন্ন VelocitySmoothing::computeVelAtZeroAcc() ধ্রুৱ
{
	ভিন্ন vel_zero_acc = _state.v;

	যদি (fabsf(_state.a) > FLT_EPSILON) {
		ভিন্ন j_zero_acc = -sign(_state.a) * _max_jerk; // Required jerk to reduce the acceleration
		ভিন্ন t_zero_acc = -_state.a / j_zero_acc; // Required time to cancel the current acceleration
		vel_zero_acc = _state.v + _state.a * t_zero_acc + 0.5f * j_zero_acc * t_zero_acc * t_zero_acc;
	}

	ঘূৰাই vel_zero_acc;
}

শূন্য VelocitySmoothing::updateDurationsMinimizeTotalTime()
{
	ভিন্ন jerk_max_T1 = _direction * _max_jerk;
	ভিন্ন delta_v = _vel_sp - _state.v;

	যদি (fabsf(jerk_max_T1) > FLT_EPSILON) { // zero direction or jerk would lead to division by zero
		// compute increasing acceleration time
		_T1 = computeT1(_state.a, delta_v, jerk_max_T1, _max_accel);

		// compute decreasing acceleration time
		_T3 = computeT3(_T1, _state.a, jerk_max_T1);

		// compute constant acceleration time
		_T2 = computeT2(_T1, _T3, _state.a, delta_v, jerk_max_T1);

	} অন্যথা {
		_T1 = _T2 = _T3 = 0.f;
	}
}

Trajectory VelocitySmoothing::evaluatePoly(ভিন্ন j, ভিন্ন a0, ভিন্ন v0, ভিন্ন x0, ভিন্ন t, পূর্ণাঙ্ক d) ধ্রুৱ
{
	Trajectory traj;
	ভিন্ন jt = d * j;
	ভিন্ন t2 = t * t;
	ভিন্ন t3 = t2 * t;

	traj.j = jt;
	traj.a = a0 + jt * t;
	traj.v = v0 + a0 * t + 0.5f * jt * t2;
	traj.x = x0 + v0 * t + 0.5f * a0 * t2 + 1.f / 6.f * jt * t3;

	ঘূৰাই traj;
}

শূন্য VelocitySmoothing::updateTraj(ভিন্ন dt, ভিন্ন time_stretch)
{
	_local_time += dt * time_stretch;
	ভিন্ন t_remain = _local_time;

	ভিন্ন t1 = math::min(t_remain, _T1);
	_state = evaluatePoly(_max_jerk, _state_init.a, _state_init.v, _state_init.x, t1, _direction);
	t_remain -= t1;

	যদি (t_remain > 0.f) {
		ভিন্ন t2 = math::min(t_remain, _T2);
		_state = evaluatePoly(0.f, _state.a, _state.v, _state.x, t2, 0.f);
		t_remain -= t2;
	}

	যদি (t_remain > 0.f) {
		ভিন্ন t3 = math::min(t_remain, _T3);
		_state = evaluatePoly(_max_jerk, _state.a, _state.v, _state.x, t3, -_direction);
		t_remain -= t3;
	}

	যদি (t_remain > 0.f) {
		_state = evaluatePoly(0.f, 0.f, _state.v, _state.x, t_remain, 0.f);
	}
}

শূন্য VelocitySmoothing::timeSynchronization(VelocitySmoothing *traj, পূর্ণাঙ্ক n_traj)
{
	ভিন্ন desired_time = 0.f;
	পূর্ণাঙ্ক longest_traj_index = 0;

	বাবে (পূর্ণাঙ্ক i = 0; i < n_traj; i++) {
		ধ্রুৱ ভিন্ন T123 = traj[i].getTotalTime();

		যদি (T123 > desired_time) {
			desired_time = T123;
			longest_traj_index = i;
		}
	}

	যদি (desired_time > FLT_EPSILON) {
		বাবে (পূর্ণাঙ্ক i = 0; i < n_traj; i++) {
			যদি ((i != longest_traj_index)
			    && (traj[i].getTotalTime() < desired_time)) {
				traj[i].updateDurationsGivenTotalTime(desired_time);
			}
		}
	}
}

শূন্য VelocitySmoothing::updateDurationsGivenTotalTime(ভিন্ন T123)
{
	ভিন্ন jerk_max_T1 = _direction * _max_jerk;
	ভিন্ন delta_v = _vel_sp - _state.v;

	যদি (fabsf(jerk_max_T1) > FLT_EPSILON) { // zero direction or jerk would lead to division by zero
		// compute increasing acceleration time
		_T1 = computeT1(T123, _state.a, delta_v, jerk_max_T1, _max_accel);

		// compute decreasing acceleration time
		_T3 = computeT3(_T1, _state.a, jerk_max_T1);

		// compute constant acceleration time
		_T2 = computeT2(T123, _T1, _T3);

	} অন্যথা {
		_T1 = _T2 = _T3 = 0.f;
	}
}

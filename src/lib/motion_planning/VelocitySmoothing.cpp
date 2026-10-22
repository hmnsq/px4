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

#ଅନ୍ତର୍ଭାଵ "VelocitySmoothing.hpp"

#ଅନ୍ତର୍ଭାଵ <cstdio>
#ଅନ୍ତର୍ଭାଵ <float.h>
#ଅନ୍ତର୍ଭାଵ <mathlib/mathlib.h>
#ଅନ୍ତର୍ଭାଵ <matrix/matrix/math.hpp>

using matrix::sign;

VelocitySmoothing::VelocitySmoothing(ଦଶମିକ initial_accel, ଦଶମିକ initial_vel, ଦଶମିକ initial_pos)
{
	reset(initial_accel, initial_vel, initial_pos);
}

ଶୂନ୍ୟ VelocitySmoothing::reset(ଦଶମିକ accel, ଦଶମିକ vel, ଦଶମିକ pos)
{
	_state.j = 0.f;
	_state.a = accel;
	_state.v = vel;
	_state.x = pos;

	_state_init = _state;
}

ଦଶମିକ VelocitySmoothing::saturateT1ForAccel(ଦଶମିକ a0, ଦଶମିକ j_max, ଦଶମିକ T1, ଦଶମିକ a_max) ଅଚଳ
{
	/* Check maximum acceleration, saturate and recompute T1 if needed */
	ଦଶମିକ accel_T1 = a0 + j_max * T1;
	ଦଶମିକ T1_new = T1;

	ଯଦି (accel_T1 > a_max) {
		T1_new = (a_max - a0) / j_max;

	} ଅନ୍ୟଥା ଯଦି (accel_T1 < -a_max) {
		T1_new = (-a_max - a0) / j_max;
	}

	ଫେରନ୍ତୁ T1_new;
}

ଦଶମିକ VelocitySmoothing::computeT1(ଦଶମିକ a0, ଦଶମିକ v3, ଦଶମିକ j_max, ଦଶମିକ a_max) ଅଚଳ
{
	ଦଶମିକ delta = 2.f * a0 * a0 + 4.f * j_max * v3;

	ଯଦି (delta < 0.f) {
		// Solution is not real
		ଫେରନ୍ତୁ 0.f;
	}

	ଦଶମିକ sqrt_delta = sqrtf(delta);
	ଦଶମିକ T1_plus = (-a0 + 0.5f * sqrt_delta) / j_max;
	ଦଶମିକ T1_minus = (-a0 - 0.5f * sqrt_delta) / j_max;

	ଦଶମିକ T3_plus = a0 / j_max + T1_plus;
	ଦଶମିକ T3_minus = a0 / j_max + T1_minus;

	ଦଶମିକ T1 = 0.f;

	ଯଦି (T1_plus >= 0.f && T3_plus >= 0.f) {
		T1 = T1_plus;

	} ଅନ୍ୟଥା ଯଦି (T1_minus >= 0.f && T3_minus >= 0.f) {
		T1 = T1_minus;
	}

	T1 = saturateT1ForAccel(a0, j_max, T1, a_max);

	ଫେରନ୍ତୁ math::max(T1, 0.f);
}

ଦଶମିକ VelocitySmoothing::computeT1(ଦଶମିକ T123, ଦଶମିକ a0, ଦଶମିକ v3, ଦଶମିକ j_max, ଦଶମିକ a_max) ଅଚଳ
{
	ଦଶମିକ a = -j_max;
	ଦଶମିକ b = j_max * T123 - a0;
	ଦଶମିକ delta = T123 * T123 * j_max * j_max + 2.f * T123 * a0 * j_max - a0 * a0 - 4.f * j_max * v3;

	ଯଦି (delta < 0.f) {
		// Solution is not real
		ଫେରନ୍ତୁ 0.f;
	}

	ଦଶମିକ sqrt_delta = sqrtf(delta);
	ଦଶମିକ denominator_inv = 1.f / (2.f * a);
	ଦଶମିକ T1_plus = math::max((-b + sqrt_delta) * denominator_inv, 0.f);
	ଦଶମିକ T1_minus = math::max((-b - sqrt_delta) * denominator_inv, 0.f);

	ଦଶମିକ T3_plus = a0 / j_max + T1_plus;
	ଦଶମିକ T3_minus = a0 / j_max + T1_minus;

	ଦଶମିକ T1 = 0.f;

	ଯଦି ((T1_plus >= 0.f && T3_plus >= 0.f) && ((T1_plus + T3_plus) <= T123)) {
		T1 = T1_plus;

	} ଅନ୍ୟଥା ଯଦି ((T1_minus >= 0.f && T3_minus >= 0.f) && ((T1_minus + T3_minus) <= T123)) {
		T1 = T1_minus;
	}

	T1 = saturateT1ForAccel(a0, j_max, T1, a_max);

	ଫେରନ୍ତୁ T1;
}


ଦଶମିକ VelocitySmoothing::computeT2(ଦଶମିକ T1, ଦଶମିକ T3, ଦଶମିକ a0, ଦଶମିକ v3, ଦଶମିକ j_max) ଅଚଳ
{
	ଦଶମିକ T2 = 0.f;

	ଦଶମିକ den = a0 + j_max * T1;

	ଯଦି (math::abs_t(den) > FLT_EPSILON) {
		T2 = (-0.5f * T1 * T1 * j_max - T1 * T3 * j_max - T1 * a0 + 0.5f * T3 * T3 * j_max - T3 * a0 + v3) / den;
	}

	ଫେରନ୍ତୁ math::max(T2, 0.f);
}

ଦଶମିକ VelocitySmoothing::computeT2(ଦଶମିକ T123, ଦଶମିକ T1, ଦଶମିକ T3) ଅଚଳ
{
	ଦଶମିକ T2 = T123 - T1 - T3;
	ଫେରନ୍ତୁ math::max(T2, 0.f);
}

ଦଶମିକ VelocitySmoothing::computeT3(ଦଶମିକ T1, ଦଶମିକ a0, ଦଶମିକ j_max) ଅଚଳ
{
	ଦଶମିକ T3 = a0 / j_max + T1;
	ଫେରନ୍ତୁ math::max(T3, 0.f);
}

ଶୂନ୍ୟ VelocitySmoothing::updateDurations(ଦଶମିକ vel_setpoint)
{
	_vel_sp = math::constrain(vel_setpoint, -_max_vel, _max_vel);
	_local_time = 0.f;
	_state_init = _state;

	_direction = computeDirection();

	updateDurationsMinimizeTotalTime();
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା VelocitySmoothing::computeDirection() ଅଚଳ
{
	// Compute the velocity at which the trajectory will be
	// when the acceleration will be zero
	ଦଶମିକ vel_zero_acc = computeVelAtZeroAcc();

	/* Depending of the direction, start accelerating positively or negatively */
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା direction = sign(_vel_sp - vel_zero_acc);

	ଯଦି (direction == 0) {
		// If by braking immediately the velocity is exactly
		// the required one with zero acceleration, then brake
		direction = sign(_state.a);
	}

	ଫେରନ୍ତୁ direction;
}

ଦଶମିକ VelocitySmoothing::computeVelAtZeroAcc() ଅଚଳ
{
	ଦଶମିକ vel_zero_acc = _state.v;

	ଯଦି (fabsf(_state.a) > FLT_EPSILON) {
		ଦଶମିକ j_zero_acc = -sign(_state.a) * _max_jerk; // Required jerk to reduce the acceleration
		ଦଶମିକ t_zero_acc = -_state.a / j_zero_acc; // Required time to cancel the current acceleration
		vel_zero_acc = _state.v + _state.a * t_zero_acc + 0.5f * j_zero_acc * t_zero_acc * t_zero_acc;
	}

	ଫେରନ୍ତୁ vel_zero_acc;
}

ଶୂନ୍ୟ VelocitySmoothing::updateDurationsMinimizeTotalTime()
{
	ଦଶମିକ jerk_max_T1 = _direction * _max_jerk;
	ଦଶମିକ delta_v = _vel_sp - _state.v;

	ଯଦି (fabsf(jerk_max_T1) > FLT_EPSILON) { // zero direction or jerk would lead to division by zero
		// compute increasing acceleration time
		_T1 = computeT1(_state.a, delta_v, jerk_max_T1, _max_accel);

		// compute decreasing acceleration time
		_T3 = computeT3(_T1, _state.a, jerk_max_T1);

		// compute constant acceleration time
		_T2 = computeT2(_T1, _T3, _state.a, delta_v, jerk_max_T1);

	} ଅନ୍ୟଥା {
		_T1 = _T2 = _T3 = 0.f;
	}
}

Trajectory VelocitySmoothing::evaluatePoly(ଦଶମିକ j, ଦଶମିକ a0, ଦଶମିକ v0, ଦଶମିକ x0, ଦଶମିକ t, ପୂର୍ଣ୍ଣସଂଖ୍ୟା d) ଅଚଳ
{
	Trajectory traj;
	ଦଶମିକ jt = d * j;
	ଦଶମିକ t2 = t * t;
	ଦଶମିକ t3 = t2 * t;

	traj.j = jt;
	traj.a = a0 + jt * t;
	traj.v = v0 + a0 * t + 0.5f * jt * t2;
	traj.x = x0 + v0 * t + 0.5f * a0 * t2 + 1.f / 6.f * jt * t3;

	ଫେରନ୍ତୁ traj;
}

ଶୂନ୍ୟ VelocitySmoothing::updateTraj(ଦଶମିକ dt, ଦଶମିକ time_stretch)
{
	_local_time += dt * time_stretch;
	ଦଶମିକ t_remain = _local_time;

	ଦଶମିକ t1 = math::min(t_remain, _T1);
	_state = evaluatePoly(_max_jerk, _state_init.a, _state_init.v, _state_init.x, t1, _direction);
	t_remain -= t1;

	ଯଦି (t_remain > 0.f) {
		ଦଶମିକ t2 = math::min(t_remain, _T2);
		_state = evaluatePoly(0.f, _state.a, _state.v, _state.x, t2, 0.f);
		t_remain -= t2;
	}

	ଯଦି (t_remain > 0.f) {
		ଦଶମିକ t3 = math::min(t_remain, _T3);
		_state = evaluatePoly(_max_jerk, _state.a, _state.v, _state.x, t3, -_direction);
		t_remain -= t3;
	}

	ଯଦି (t_remain > 0.f) {
		_state = evaluatePoly(0.f, 0.f, _state.v, _state.x, t_remain, 0.f);
	}
}

ଶୂନ୍ୟ VelocitySmoothing::timeSynchronization(VelocitySmoothing *traj, ପୂର୍ଣ୍ଣସଂଖ୍ୟା n_traj)
{
	ଦଶମିକ desired_time = 0.f;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା longest_traj_index = 0;

	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < n_traj; i++) {
		ଅଚଳ ଦଶମିକ T123 = traj[i].getTotalTime();

		ଯଦି (T123 > desired_time) {
			desired_time = T123;
			longest_traj_index = i;
		}
	}

	ଯଦି (desired_time > FLT_EPSILON) {
		ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < n_traj; i++) {
			ଯଦି ((i != longest_traj_index)
			    && (traj[i].getTotalTime() < desired_time)) {
				traj[i].updateDurationsGivenTotalTime(desired_time);
			}
		}
	}
}

ଶୂନ୍ୟ VelocitySmoothing::updateDurationsGivenTotalTime(ଦଶମିକ T123)
{
	ଦଶମିକ jerk_max_T1 = _direction * _max_jerk;
	ଦଶମିକ delta_v = _vel_sp - _state.v;

	ଯଦି (fabsf(jerk_max_T1) > FLT_EPSILON) { // zero direction or jerk would lead to division by zero
		// compute increasing acceleration time
		_T1 = computeT1(T123, _state.a, delta_v, jerk_max_T1, _max_accel);

		// compute decreasing acceleration time
		_T3 = computeT3(_T1, _state.a, jerk_max_T1);

		// compute constant acceleration time
		_T2 = computeT2(T123, _T1, _T3);

	} ଅନ୍ୟଥା {
		_T1 = _T2 = _T3 = 0.f;
	}
}

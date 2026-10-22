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

#അന്തര്ഭാവ "VelocitySmoothing.hpp"

#അന്തര്ഭാവ <cstdio>
#അന്തര്ഭാവ <float.h>
#അന്തര്ഭാവ <mathlib/mathlib.h>
#അന്തര്ഭാവ <matrix/matrix/math.hpp>

using matrix::sign;

VelocitySmoothing::VelocitySmoothing(ദശാംശം initial_accel, ദശാംശം initial_vel, ദശാംശം initial_pos)
{
	reset(initial_accel, initial_vel, initial_pos);
}

ശൂന്യം VelocitySmoothing::reset(ദശാംശം accel, ദശാംശം vel, ദശാംശം pos)
{
	_state.j = 0.f;
	_state.a = accel;
	_state.v = vel;
	_state.x = pos;

	_state_init = _state;
}

ദശാംശം VelocitySmoothing::saturateT1ForAccel(ദശാംശം a0, ദശാംശം j_max, ദശാംശം T1, ദശാംശം a_max) സ്ഥിരം
{
	/* Check maximum acceleration, saturate and recompute T1 if needed */
	ദശാംശം accel_T1 = a0 + j_max * T1;
	ദശാംശം T1_new = T1;

	എങ്കിൽ (accel_T1 > a_max) {
		T1_new = (a_max - a0) / j_max;

	} അല്ലെങ്കിൽ എങ്കിൽ (accel_T1 < -a_max) {
		T1_new = (-a_max - a0) / j_max;
	}

	തിരികെ T1_new;
}

ദശാംശം VelocitySmoothing::computeT1(ദശാംശം a0, ദശാംശം v3, ദശാംശം j_max, ദശാംശം a_max) സ്ഥിരം
{
	ദശാംശം delta = 2.f * a0 * a0 + 4.f * j_max * v3;

	എങ്കിൽ (delta < 0.f) {
		// Solution is not real
		തിരികെ 0.f;
	}

	ദശാംശം sqrt_delta = sqrtf(delta);
	ദശാംശം T1_plus = (-a0 + 0.5f * sqrt_delta) / j_max;
	ദശാംശം T1_minus = (-a0 - 0.5f * sqrt_delta) / j_max;

	ദശാംശം T3_plus = a0 / j_max + T1_plus;
	ദശാംശം T3_minus = a0 / j_max + T1_minus;

	ദശാംശം T1 = 0.f;

	എങ്കിൽ (T1_plus >= 0.f && T3_plus >= 0.f) {
		T1 = T1_plus;

	} അല്ലെങ്കിൽ എങ്കിൽ (T1_minus >= 0.f && T3_minus >= 0.f) {
		T1 = T1_minus;
	}

	T1 = saturateT1ForAccel(a0, j_max, T1, a_max);

	തിരികെ math::max(T1, 0.f);
}

ദശാംശം VelocitySmoothing::computeT1(ദശാംശം T123, ദശാംശം a0, ദശാംശം v3, ദശാംശം j_max, ദശാംശം a_max) സ്ഥിരം
{
	ദശാംശം a = -j_max;
	ദശാംശം b = j_max * T123 - a0;
	ദശാംശം delta = T123 * T123 * j_max * j_max + 2.f * T123 * a0 * j_max - a0 * a0 - 4.f * j_max * v3;

	എങ്കിൽ (delta < 0.f) {
		// Solution is not real
		തിരികെ 0.f;
	}

	ദശാംശം sqrt_delta = sqrtf(delta);
	ദശാംശം denominator_inv = 1.f / (2.f * a);
	ദശാംശം T1_plus = math::max((-b + sqrt_delta) * denominator_inv, 0.f);
	ദശാംശം T1_minus = math::max((-b - sqrt_delta) * denominator_inv, 0.f);

	ദശാംശം T3_plus = a0 / j_max + T1_plus;
	ദശാംശം T3_minus = a0 / j_max + T1_minus;

	ദശാംശം T1 = 0.f;

	എങ്കിൽ ((T1_plus >= 0.f && T3_plus >= 0.f) && ((T1_plus + T3_plus) <= T123)) {
		T1 = T1_plus;

	} അല്ലെങ്കിൽ എങ്കിൽ ((T1_minus >= 0.f && T3_minus >= 0.f) && ((T1_minus + T3_minus) <= T123)) {
		T1 = T1_minus;
	}

	T1 = saturateT1ForAccel(a0, j_max, T1, a_max);

	തിരികെ T1;
}


ദശാംശം VelocitySmoothing::computeT2(ദശാംശം T1, ദശാംശം T3, ദശാംശം a0, ദശാംശം v3, ദശാംശം j_max) സ്ഥിരം
{
	ദശാംശം T2 = 0.f;

	ദശാംശം den = a0 + j_max * T1;

	എങ്കിൽ (math::abs_t(den) > FLT_EPSILON) {
		T2 = (-0.5f * T1 * T1 * j_max - T1 * T3 * j_max - T1 * a0 + 0.5f * T3 * T3 * j_max - T3 * a0 + v3) / den;
	}

	തിരികെ math::max(T2, 0.f);
}

ദശാംശം VelocitySmoothing::computeT2(ദശാംശം T123, ദശാംശം T1, ദശാംശം T3) സ്ഥിരം
{
	ദശാംശം T2 = T123 - T1 - T3;
	തിരികെ math::max(T2, 0.f);
}

ദശാംശം VelocitySmoothing::computeT3(ദശാംശം T1, ദശാംശം a0, ദശാംശം j_max) സ്ഥിരം
{
	ദശാംശം T3 = a0 / j_max + T1;
	തിരികെ math::max(T3, 0.f);
}

ശൂന്യം VelocitySmoothing::updateDurations(ദശാംശം vel_setpoint)
{
	_vel_sp = math::constrain(vel_setpoint, -_max_vel, _max_vel);
	_local_time = 0.f;
	_state_init = _state;

	_direction = computeDirection();

	updateDurationsMinimizeTotalTime();
}

പൂർണ്ണസംഖ്യ VelocitySmoothing::computeDirection() സ്ഥിരം
{
	// Compute the velocity at which the trajectory will be
	// when the acceleration will be zero
	ദശാംശം vel_zero_acc = computeVelAtZeroAcc();

	/* Depending of the direction, start accelerating positively or negatively */
	പൂർണ്ണസംഖ്യ direction = sign(_vel_sp - vel_zero_acc);

	എങ്കിൽ (direction == 0) {
		// If by braking immediately the velocity is exactly
		// the required one with zero acceleration, then brake
		direction = sign(_state.a);
	}

	തിരികെ direction;
}

ദശാംശം VelocitySmoothing::computeVelAtZeroAcc() സ്ഥിരം
{
	ദശാംശം vel_zero_acc = _state.v;

	എങ്കിൽ (fabsf(_state.a) > FLT_EPSILON) {
		ദശാംശം j_zero_acc = -sign(_state.a) * _max_jerk; // Required jerk to reduce the acceleration
		ദശാംശം t_zero_acc = -_state.a / j_zero_acc; // Required time to cancel the current acceleration
		vel_zero_acc = _state.v + _state.a * t_zero_acc + 0.5f * j_zero_acc * t_zero_acc * t_zero_acc;
	}

	തിരികെ vel_zero_acc;
}

ശൂന്യം VelocitySmoothing::updateDurationsMinimizeTotalTime()
{
	ദശാംശം jerk_max_T1 = _direction * _max_jerk;
	ദശാംശം delta_v = _vel_sp - _state.v;

	എങ്കിൽ (fabsf(jerk_max_T1) > FLT_EPSILON) { // zero direction or jerk would lead to division by zero
		// compute increasing acceleration time
		_T1 = computeT1(_state.a, delta_v, jerk_max_T1, _max_accel);

		// compute decreasing acceleration time
		_T3 = computeT3(_T1, _state.a, jerk_max_T1);

		// compute constant acceleration time
		_T2 = computeT2(_T1, _T3, _state.a, delta_v, jerk_max_T1);

	} അല്ലെങ്കിൽ {
		_T1 = _T2 = _T3 = 0.f;
	}
}

Trajectory VelocitySmoothing::evaluatePoly(ദശാംശം j, ദശാംശം a0, ദശാംശം v0, ദശാംശം x0, ദശാംശം t, പൂർണ്ണസംഖ്യ d) സ്ഥിരം
{
	Trajectory traj;
	ദശാംശം jt = d * j;
	ദശാംശം t2 = t * t;
	ദശാംശം t3 = t2 * t;

	traj.j = jt;
	traj.a = a0 + jt * t;
	traj.v = v0 + a0 * t + 0.5f * jt * t2;
	traj.x = x0 + v0 * t + 0.5f * a0 * t2 + 1.f / 6.f * jt * t3;

	തിരികെ traj;
}

ശൂന്യം VelocitySmoothing::updateTraj(ദശാംശം dt, ദശാംശം time_stretch)
{
	_local_time += dt * time_stretch;
	ദശാംശം t_remain = _local_time;

	ദശാംശം t1 = math::min(t_remain, _T1);
	_state = evaluatePoly(_max_jerk, _state_init.a, _state_init.v, _state_init.x, t1, _direction);
	t_remain -= t1;

	എങ്കിൽ (t_remain > 0.f) {
		ദശാംശം t2 = math::min(t_remain, _T2);
		_state = evaluatePoly(0.f, _state.a, _state.v, _state.x, t2, 0.f);
		t_remain -= t2;
	}

	എങ്കിൽ (t_remain > 0.f) {
		ദശാംശം t3 = math::min(t_remain, _T3);
		_state = evaluatePoly(_max_jerk, _state.a, _state.v, _state.x, t3, -_direction);
		t_remain -= t3;
	}

	എങ്കിൽ (t_remain > 0.f) {
		_state = evaluatePoly(0.f, 0.f, _state.v, _state.x, t_remain, 0.f);
	}
}

ശൂന്യം VelocitySmoothing::timeSynchronization(VelocitySmoothing *traj, പൂർണ്ണസംഖ്യ n_traj)
{
	ദശാംശം desired_time = 0.f;
	പൂർണ്ണസംഖ്യ longest_traj_index = 0;

	വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < n_traj; i++) {
		സ്ഥിരം ദശാംശം T123 = traj[i].getTotalTime();

		എങ്കിൽ (T123 > desired_time) {
			desired_time = T123;
			longest_traj_index = i;
		}
	}

	എങ്കിൽ (desired_time > FLT_EPSILON) {
		വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < n_traj; i++) {
			എങ്കിൽ ((i != longest_traj_index)
			    && (traj[i].getTotalTime() < desired_time)) {
				traj[i].updateDurationsGivenTotalTime(desired_time);
			}
		}
	}
}

ശൂന്യം VelocitySmoothing::updateDurationsGivenTotalTime(ദശാംശം T123)
{
	ദശാംശം jerk_max_T1 = _direction * _max_jerk;
	ദശാംശം delta_v = _vel_sp - _state.v;

	എങ്കിൽ (fabsf(jerk_max_T1) > FLT_EPSILON) { // zero direction or jerk would lead to division by zero
		// compute increasing acceleration time
		_T1 = computeT1(T123, _state.a, delta_v, jerk_max_T1, _max_accel);

		// compute decreasing acceleration time
		_T3 = computeT3(_T1, _state.a, jerk_max_T1);

		// compute constant acceleration time
		_T2 = computeT2(T123, _T1, _T3);

	} അല്ലെങ്കിൽ {
		_T1 = _T2 = _T3 = 0.f;
	}
}

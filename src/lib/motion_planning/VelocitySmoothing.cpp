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

#అన్తర్భావ "VelocitySmoothing.hpp"

#అన్తర్భావ <cstdio>
#అన్తర్భావ <float.h>
#అన్తర్భావ <mathlib/mathlib.h>
#అన్తర్భావ <matrix/matrix/math.hpp>

ఉపయోగించి మాతృక::sign;

VelocitySmoothing::VelocitySmoothing(దశాంశం initial_accel, దశాంశం initial_vel, దశాంశం initial_pos)
{
	reset(initial_accel, initial_vel, initial_pos);
}

శూన్య VelocitySmoothing::reset(దశాంశం accel, దశాంశం vel, దశాంశం pos)
{
	_state.j = 0.f;
	_state.a = accel;
	_state.v = vel;
	_state.x = pos;

	_state_init = _state;
}

దశాంశం VelocitySmoothing::saturateT1ForAccel(దశాంశం a0, దశాంశం j_max, దశాంశం T1, దశాంశం a_max) స్థిరరాశి
{
	/* Check maximum acceleration, saturate and recompute T1 if needed */
	దశాంశం accel_T1 = a0 + j_max * T1;
	దశాంశం T1_new = T1;

	ఒకవేళ (accel_T1 > a_max) {
		T1_new = (a_max - a0) / j_max;

	} లేకపోతే ఒకవేళ (accel_T1 < -a_max) {
		T1_new = (-a_max - a0) / j_max;
	}

	తిరిగి T1_new;
}

దశాంశం VelocitySmoothing::computeT1(దశాంశం a0, దశాంశం v3, దశాంశం j_max, దశాంశం a_max) స్థిరరాశి
{
	దశాంశం delta = 2.f * a0 * a0 + 4.f * j_max * v3;

	ఒకవేళ (delta < 0.f) {
		// Solution is not real
		తిరిగి 0.f;
	}

	దశాంశం sqrt_delta = sqrtf(delta);
	దశాంశం T1_plus = (-a0 + 0.5f * sqrt_delta) / j_max;
	దశాంశం T1_minus = (-a0 - 0.5f * sqrt_delta) / j_max;

	దశాంశం T3_plus = a0 / j_max + T1_plus;
	దశాంశం T3_minus = a0 / j_max + T1_minus;

	దశాంశం T1 = 0.f;

	ఒకవేళ (T1_plus >= 0.f && T3_plus >= 0.f) {
		T1 = T1_plus;

	} లేకపోతే ఒకవేళ (T1_minus >= 0.f && T3_minus >= 0.f) {
		T1 = T1_minus;
	}

	T1 = saturateT1ForAccel(a0, j_max, T1, a_max);

	తిరిగి math::max(T1, 0.f);
}

దశాంశం VelocitySmoothing::computeT1(దశాంశం T123, దశాంశం a0, దశాంశం v3, దశాంశం j_max, దశాంశం a_max) స్థిరరాశి
{
	దశాంశం a = -j_max;
	దశాంశం b = j_max * T123 - a0;
	దశాంశం delta = T123 * T123 * j_max * j_max + 2.f * T123 * a0 * j_max - a0 * a0 - 4.f * j_max * v3;

	ఒకవేళ (delta < 0.f) {
		// Solution is not real
		తిరిగి 0.f;
	}

	దశాంశం sqrt_delta = sqrtf(delta);
	దశాంశం denominator_inv = 1.f / (2.f * a);
	దశాంశం T1_plus = math::max((-b + sqrt_delta) * denominator_inv, 0.f);
	దశాంశం T1_minus = math::max((-b - sqrt_delta) * denominator_inv, 0.f);

	దశాంశం T3_plus = a0 / j_max + T1_plus;
	దశాంశం T3_minus = a0 / j_max + T1_minus;

	దశాంశం T1 = 0.f;

	ఒకవేళ ((T1_plus >= 0.f && T3_plus >= 0.f) && ((T1_plus + T3_plus) <= T123)) {
		T1 = T1_plus;

	} లేకపోతే ఒకవేళ ((T1_minus >= 0.f && T3_minus >= 0.f) && ((T1_minus + T3_minus) <= T123)) {
		T1 = T1_minus;
	}

	T1 = saturateT1ForAccel(a0, j_max, T1, a_max);

	తిరిగి T1;
}


దశాంశం VelocitySmoothing::computeT2(దశాంశం T1, దశాంశం T3, దశాంశం a0, దశాంశం v3, దశాంశం j_max) స్థిరరాశి
{
	దశాంశం T2 = 0.f;

	దశాంశం den = a0 + j_max * T1;

	ఒకవేళ (math::abs_t(den) > FLT_EPSILON) {
		T2 = (-0.5f * T1 * T1 * j_max - T1 * T3 * j_max - T1 * a0 + 0.5f * T3 * T3 * j_max - T3 * a0 + v3) / den;
	}

	తిరిగి math::max(T2, 0.f);
}

దశాంశం VelocitySmoothing::computeT2(దశాంశం T123, దశాంశం T1, దశాంశం T3) స్థిరరాశి
{
	దశాంశం T2 = T123 - T1 - T3;
	తిరిగి math::max(T2, 0.f);
}

దశాంశం VelocitySmoothing::computeT3(దశాంశం T1, దశాంశం a0, దశాంశం j_max) స్థిరరాశి
{
	దశాంశం T3 = a0 / j_max + T1;
	తిరిగి math::max(T3, 0.f);
}

శూన్య VelocitySmoothing::updateDurations(దశాంశం vel_setpoint)
{
	_vel_sp = math::constrain(vel_setpoint, -_max_vel, _max_vel);
	local_సమయం = 0.f;
	_state_init = _state;

	_direction = computeDirection();

	update_Durations_Minimize_మొత్తం_సమయం();
}

పూర్ణాంకం VelocitySmoothing::computeDirection() స్థిరరాశి
{
	// Compute the velocity at which the trajectory will be
	// when the acceleration will be zero
	దశాంశం vel_zero_acc = computeVelAtZeroAcc();

	/* Depending of the direction, start accelerating positively or negatively */
	పూర్ణాంకం direction = sign(_vel_sp - vel_zero_acc);

	ఒకవేళ (direction == 0) {
		// If by braking immediately the velocity is exactly
		// the required one with zero acceleration, then brake
		direction = sign(_state.a);
	}

	తిరిగి direction;
}

దశాంశం VelocitySmoothing::computeVelAtZeroAcc() స్థిరరాశి
{
	దశాంశం vel_zero_acc = _state.v;

	ఒకవేళ (fabsf(_state.a) > FLT_EPSILON) {
		దశాంశం j_zero_acc = -sign(_state.a) * _max_jerk; // Required jerk to reduce the acceleration
		దశాంశం t_zero_acc = -_state.a / j_zero_acc; // Required time to cancel the current acceleration
		vel_zero_acc = _state.v + _state.a * t_zero_acc + 0.5f * j_zero_acc * t_zero_acc * t_zero_acc;
	}

	తిరిగి vel_zero_acc;
}

శూన్య VelocitySmoothing::update_Durations_Minimize_మొత్తం_సమయం()
{
	దశాంశం jerk_max_T1 = _direction * _max_jerk;
	దశాంశం delta_v = _vel_sp - _state.v;

	ఒకవేళ (fabsf(jerk_max_T1) > FLT_EPSILON) { // zero direction or jerk would lead to division by zero
		// compute increasing acceleration time
		_T1 = computeT1(_state.a, delta_v, jerk_max_T1, _max_accel);

		// compute decreasing acceleration time
		_T3 = computeT3(_T1, _state.a, jerk_max_T1);

		// compute constant acceleration time
		_T2 = computeT2(_T1, _T3, _state.a, delta_v, jerk_max_T1);

	} లేకపోతే {
		_T1 = _T2 = _T3 = 0.f;
	}
}

Trajectory VelocitySmoothing::evaluatePoly(దశాంశం j, దశాంశం a0, దశాంశం v0, దశాంశం x0, దశాంశం t, పూర్ణాంకం d) స్థిరరాశి
{
	Trajectory traj;
	దశాంశం jt = d * j;
	దశాంశం t2 = t * t;
	దశాంశం t3 = t2 * t;

	traj.j = jt;
	traj.a = a0 + jt * t;
	traj.v = v0 + a0 * t + 0.5f * jt * t2;
	traj.x = x0 + v0 * t + 0.5f * a0 * t2 + 1.f / 6.f * jt * t3;

	తిరిగి traj;
}

శూన్య VelocitySmoothing::updateTraj(దశాంశం dt, దశాంశం సమయం_stretch)
{
	local_సమయం += dt * సమయం_stretch;
	దశాంశం t_remain = local_సమయం;

	దశాంశం t1 = math::min(t_remain, _T1);
	_state = evaluatePoly(_max_jerk, _state_init.a, _state_init.v, _state_init.x, t1, _direction);
	t_remain -= t1;

	ఒకవేళ (t_remain > 0.f) {
		దశాంశం t2 = math::min(t_remain, _T2);
		_state = evaluatePoly(0.f, _state.a, _state.v, _state.x, t2, 0.f);
		t_remain -= t2;
	}

	ఒకవేళ (t_remain > 0.f) {
		దశాంశం t3 = math::min(t_remain, _T3);
		_state = evaluatePoly(_max_jerk, _state.a, _state.v, _state.x, t3, -_direction);
		t_remain -= t3;
	}

	ఒకవేళ (t_remain > 0.f) {
		_state = evaluatePoly(0.f, 0.f, _state.v, _state.x, t_remain, 0.f);
	}
}

శూన్య VelocitySmoothing::సమయం_Synchronization(VelocitySmoothing *traj, పూర్ణాంకం n_traj)
{
	దశాంశం desired_సమయం = 0.f;
	పూర్ణాంకం longest_traj_సూచీ = 0;

	కొరకు (పూర్ణాంకం i = 0; i < n_traj; i++) {
		స్థిరరాశి దశాంశం T123 = traj[i].get_మొత్తం_సమయం();

		ఒకవేళ (T123 > desired_సమయం) {
			desired_సమయం = T123;
			longest_traj_సూచీ = i;
		}
	}

	ఒకవేళ (desired_సమయం > FLT_EPSILON) {
		కొరకు (పూర్ణాంకం i = 0; i < n_traj; i++) {
			ఒకవేళ ((i != longest_traj_సూచీ)
			    && (traj[i].get_మొత్తం_సమయం() < desired_సమయం)) {
				traj[i].update_Durations_Given_మొత్తం_సమయం(desired_సమయం);
			}
		}
	}
}

శూన్య VelocitySmoothing::update_Durations_Given_మొత్తం_సమయం(దశాంశం T123)
{
	దశాంశం jerk_max_T1 = _direction * _max_jerk;
	దశాంశం delta_v = _vel_sp - _state.v;

	ఒకవేళ (fabsf(jerk_max_T1) > FLT_EPSILON) { // zero direction or jerk would lead to division by zero
		// compute increasing acceleration time
		_T1 = computeT1(T123, _state.a, delta_v, jerk_max_T1, _max_accel);

		// compute decreasing acceleration time
		_T3 = computeT3(_T1, _state.a, jerk_max_T1);

		// compute constant acceleration time
		_T2 = computeT2(T123, _T1, _T3);

	} లేకపోతే {
		_T1 = _T2 = _T3 = 0.f;
	}
}

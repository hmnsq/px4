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

#अन्तर्भाव "VelocitySmoothing.hpp"

#अन्तर्भाव <cstdio>
#अन्तर्भाव <float.h>
#अन्तर्भाव <mathlib/mathlib.h>
#अन्तर्भाव <matrix/matrix/math.hpp>

उपयुज्य matrix::sign;

VelocitySmoothing::VelocitySmoothing(भिन्न initial_accel, भिन्न initial_vel, भिन्न initial_pos)
{
	reset(initial_accel, initial_vel, initial_pos);
}

शून्य VelocitySmoothing::reset(भिन्न accel, भिन्न vel, भिन्न pos)
{
	_state.j = 0.f;
	_state.a = accel;
	_state.v = vel;
	_state.x = pos;

	_state_init = _state;
}

भिन्न VelocitySmoothing::saturateT1ForAccel(भिन्न a0, भिन्न j_max, भिन्न T1, भिन्न a_max) ध्रुव
{
	/* Check maximum acceleration, saturate and recompute T1 if needed */
	भिन्न accel_T1 = a0 + j_max * T1;
	भिन्न T1_new = T1;

	यदि (accel_T1 > a_max) {
		T1_new = (a_max - a0) / j_max;

	} अन्यथा यदि (accel_T1 < -a_max) {
		T1_new = (-a_max - a0) / j_max;
	}

	प्रत्यागम T1_new;
}

भिन्न VelocitySmoothing::computeT1(भिन्न a0, भिन्न v3, भिन्न j_max, भिन्न a_max) ध्रुव
{
	भिन्न delta = 2.f * a0 * a0 + 4.f * j_max * v3;

	यदि (delta < 0.f) {
		// Solution is not real
		प्रत्यागम 0.f;
	}

	भिन्न sqrt_delta = sqrtf(delta);
	भिन्न T1_plus = (-a0 + 0.5f * sqrt_delta) / j_max;
	भिन्न T1_minus = (-a0 - 0.5f * sqrt_delta) / j_max;

	भिन्न T3_plus = a0 / j_max + T1_plus;
	भिन्न T3_minus = a0 / j_max + T1_minus;

	भिन्न T1 = 0.f;

	यदि (T1_plus >= 0.f && T3_plus >= 0.f) {
		T1 = T1_plus;

	} अन्यथा यदि (T1_minus >= 0.f && T3_minus >= 0.f) {
		T1 = T1_minus;
	}

	T1 = saturateT1ForAccel(a0, j_max, T1, a_max);

	प्रत्यागम math::max(T1, 0.f);
}

भिन्न VelocitySmoothing::computeT1(भिन्न T123, भिन्न a0, भिन्न v3, भिन्न j_max, भिन्न a_max) ध्रुव
{
	भिन्न a = -j_max;
	भिन्न b = j_max * T123 - a0;
	भिन्न delta = T123 * T123 * j_max * j_max + 2.f * T123 * a0 * j_max - a0 * a0 - 4.f * j_max * v3;

	यदि (delta < 0.f) {
		// Solution is not real
		प्रत्यागम 0.f;
	}

	भिन्न sqrt_delta = sqrtf(delta);
	भिन्न denominator_inv = 1.f / (2.f * a);
	भिन्न T1_plus = math::max((-b + sqrt_delta) * denominator_inv, 0.f);
	भिन्न T1_minus = math::max((-b - sqrt_delta) * denominator_inv, 0.f);

	भिन्न T3_plus = a0 / j_max + T1_plus;
	भिन्न T3_minus = a0 / j_max + T1_minus;

	भिन्न T1 = 0.f;

	यदि ((T1_plus >= 0.f && T3_plus >= 0.f) && ((T1_plus + T3_plus) <= T123)) {
		T1 = T1_plus;

	} अन्यथा यदि ((T1_minus >= 0.f && T3_minus >= 0.f) && ((T1_minus + T3_minus) <= T123)) {
		T1 = T1_minus;
	}

	T1 = saturateT1ForAccel(a0, j_max, T1, a_max);

	प्रत्यागम T1;
}


भिन्न VelocitySmoothing::computeT2(भिन्न T1, भिन्न T3, भिन्न a0, भिन्न v3, भिन्न j_max) ध्रुव
{
	भिन्न T2 = 0.f;

	भिन्न den = a0 + j_max * T1;

	यदि (math::abs_t(den) > FLT_EPSILON) {
		T2 = (-0.5f * T1 * T1 * j_max - T1 * T3 * j_max - T1 * a0 + 0.5f * T3 * T3 * j_max - T3 * a0 + v3) / den;
	}

	प्रत्यागम math::max(T2, 0.f);
}

भिन्न VelocitySmoothing::computeT2(भिन्न T123, भिन्न T1, भिन्न T3) ध्रुव
{
	भिन्न T2 = T123 - T1 - T3;
	प्रत्यागम math::max(T2, 0.f);
}

भिन्न VelocitySmoothing::computeT3(भिन्न T1, भिन्न a0, भिन्न j_max) ध्रुव
{
	भिन्न T3 = a0 / j_max + T1;
	प्रत्यागम math::max(T3, 0.f);
}

शून्य VelocitySmoothing::updateDurations(भिन्न vel_setpoint)
{
	_vel_sp = math::constrain(vel_setpoint, -_max_vel, _max_vel);
	_local_time = 0.f;
	_state_init = _state;

	_direction = computeDirection();

	update_Durations_Minimize_योग_Time();
}

पूर्णाङ्क VelocitySmoothing::computeDirection() ध्रुव
{
	// Compute the velocity at which the trajectory will be
	// when the acceleration will be zero
	भिन्न vel_zero_acc = computeVelAtZeroAcc();

	/* Depending of the direction, start accelerating positively or negatively */
	पूर्णाङ्क direction = sign(_vel_sp - vel_zero_acc);

	यदि (direction == 0) {
		// If by braking immediately the velocity is exactly
		// the required one with zero acceleration, then brake
		direction = sign(_state.a);
	}

	प्रत्यागम direction;
}

भिन्न VelocitySmoothing::computeVelAtZeroAcc() ध्रुव
{
	भिन्न vel_zero_acc = _state.v;

	यदि (fabsf(_state.a) > FLT_EPSILON) {
		भिन्न j_zero_acc = -sign(_state.a) * _max_jerk; // Required jerk to reduce the acceleration
		भिन्न t_zero_acc = -_state.a / j_zero_acc; // Required time to cancel the current acceleration
		vel_zero_acc = _state.v + _state.a * t_zero_acc + 0.5f * j_zero_acc * t_zero_acc * t_zero_acc;
	}

	प्रत्यागम vel_zero_acc;
}

शून्य VelocitySmoothing::update_Durations_Minimize_योग_Time()
{
	भिन्न jerk_max_T1 = _direction * _max_jerk;
	भिन्न delta_v = _vel_sp - _state.v;

	यदि (fabsf(jerk_max_T1) > FLT_EPSILON) { // zero direction or jerk would lead to division by zero
		// compute increasing acceleration time
		_T1 = computeT1(_state.a, delta_v, jerk_max_T1, _max_accel);

		// compute decreasing acceleration time
		_T3 = computeT3(_T1, _state.a, jerk_max_T1);

		// compute constant acceleration time
		_T2 = computeT2(_T1, _T3, _state.a, delta_v, jerk_max_T1);

	} अन्यथा {
		_T1 = _T2 = _T3 = 0.f;
	}
}

Trajectory VelocitySmoothing::evaluatePoly(भिन्न j, भिन्न a0, भिन्न v0, भिन्न x0, भिन्न t, पूर्णाङ्क d) ध्रुव
{
	Trajectory traj;
	भिन्न jt = d * j;
	भिन्न t2 = t * t;
	भिन्न t3 = t2 * t;

	traj.j = jt;
	traj.a = a0 + jt * t;
	traj.v = v0 + a0 * t + 0.5f * jt * t2;
	traj.x = x0 + v0 * t + 0.5f * a0 * t2 + 1.f / 6.f * jt * t3;

	प्रत्यागम traj;
}

शून्य VelocitySmoothing::updateTraj(भिन्न dt, भिन्न time_stretch)
{
	_local_time += dt * time_stretch;
	भिन्न t_remain = _local_time;

	भिन्न t1 = math::min(t_remain, _T1);
	_state = evaluatePoly(_max_jerk, _state_init.a, _state_init.v, _state_init.x, t1, _direction);
	t_remain -= t1;

	यदि (t_remain > 0.f) {
		भिन्न t2 = math::min(t_remain, _T2);
		_state = evaluatePoly(0.f, _state.a, _state.v, _state.x, t2, 0.f);
		t_remain -= t2;
	}

	यदि (t_remain > 0.f) {
		भिन्न t3 = math::min(t_remain, _T3);
		_state = evaluatePoly(_max_jerk, _state.a, _state.v, _state.x, t3, -_direction);
		t_remain -= t3;
	}

	यदि (t_remain > 0.f) {
		_state = evaluatePoly(0.f, 0.f, _state.v, _state.x, t_remain, 0.f);
	}
}

शून्य VelocitySmoothing::timeSynchronization(VelocitySmoothing *traj, पूर्णाङ्क n_traj)
{
	भिन्न desired_time = 0.f;
	पूर्णाङ्क longest_traj_सूचकांक = 0;

	प्रति (पूर्णाङ्क i = 0; i < n_traj; i++) {
		ध्रुव भिन्न T123 = traj[i].get_योग_Time();

		यदि (T123 > desired_time) {
			desired_time = T123;
			longest_traj_सूचकांक = i;
		}
	}

	यदि (desired_time > FLT_EPSILON) {
		प्रति (पूर्णाङ्क i = 0; i < n_traj; i++) {
			यदि ((i != longest_traj_सूचकांक)
			    && (traj[i].get_योग_Time() < desired_time)) {
				traj[i].update_Durations_Given_योग_Time(desired_time);
			}
		}
	}
}

शून्य VelocitySmoothing::update_Durations_Given_योग_Time(भिन्न T123)
{
	भिन्न jerk_max_T1 = _direction * _max_jerk;
	भिन्न delta_v = _vel_sp - _state.v;

	यदि (fabsf(jerk_max_T1) > FLT_EPSILON) { // zero direction or jerk would lead to division by zero
		// compute increasing acceleration time
		_T1 = computeT1(T123, _state.a, delta_v, jerk_max_T1, _max_accel);

		// compute decreasing acceleration time
		_T3 = computeT3(_T1, _state.a, jerk_max_T1);

		// compute constant acceleration time
		_T2 = computeT2(T123, _T1, _T3);

	} अन्यथा {
		_T1 = _T2 = _T3 = 0.f;
	}
}

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

using matrix::sign;

VelocitySmoothing::VelocitySmoothing(दशांश initial_accel, दशांश initial_vel, दशांश initial_pos)
{
	reset(initial_accel, initial_vel, initial_pos);
}

शून्य VelocitySmoothing::reset(दशांश accel, दशांश vel, दशांश pos)
{
	_state.j = 0.f;
	_state.a = accel;
	_state.v = vel;
	_state.x = pos;

	_state_init = _state;
}

दशांश VelocitySmoothing::saturateT1ForAccel(दशांश a0, दशांश j_max, दशांश T1, दशांश a_max) स्थिरांक
{
	/* Check maximum acceleration, saturate and recompute T1 if needed */
	दशांश accel_T1 = a0 + j_max * T1;
	दशांश T1_new = T1;

	जर (accel_T1 > a_max) {
		T1_new = (a_max - a0) / j_max;

	} अन्यथा जर (accel_T1 < -a_max) {
		T1_new = (-a_max - a0) / j_max;
	}

	परत T1_new;
}

दशांश VelocitySmoothing::computeT1(दशांश a0, दशांश v3, दशांश j_max, दशांश a_max) स्थिरांक
{
	दशांश delta = 2.f * a0 * a0 + 4.f * j_max * v3;

	जर (delta < 0.f) {
		// Solution is not real
		परत 0.f;
	}

	दशांश sqrt_delta = sqrtf(delta);
	दशांश T1_plus = (-a0 + 0.5f * sqrt_delta) / j_max;
	दशांश T1_minus = (-a0 - 0.5f * sqrt_delta) / j_max;

	दशांश T3_plus = a0 / j_max + T1_plus;
	दशांश T3_minus = a0 / j_max + T1_minus;

	दशांश T1 = 0.f;

	जर (T1_plus >= 0.f && T3_plus >= 0.f) {
		T1 = T1_plus;

	} अन्यथा जर (T1_minus >= 0.f && T3_minus >= 0.f) {
		T1 = T1_minus;
	}

	T1 = saturateT1ForAccel(a0, j_max, T1, a_max);

	परत math::max(T1, 0.f);
}

दशांश VelocitySmoothing::computeT1(दशांश T123, दशांश a0, दशांश v3, दशांश j_max, दशांश a_max) स्थिरांक
{
	दशांश a = -j_max;
	दशांश b = j_max * T123 - a0;
	दशांश delta = T123 * T123 * j_max * j_max + 2.f * T123 * a0 * j_max - a0 * a0 - 4.f * j_max * v3;

	जर (delta < 0.f) {
		// Solution is not real
		परत 0.f;
	}

	दशांश sqrt_delta = sqrtf(delta);
	दशांश denominator_inv = 1.f / (2.f * a);
	दशांश T1_plus = math::max((-b + sqrt_delta) * denominator_inv, 0.f);
	दशांश T1_minus = math::max((-b - sqrt_delta) * denominator_inv, 0.f);

	दशांश T3_plus = a0 / j_max + T1_plus;
	दशांश T3_minus = a0 / j_max + T1_minus;

	दशांश T1 = 0.f;

	जर ((T1_plus >= 0.f && T3_plus >= 0.f) && ((T1_plus + T3_plus) <= T123)) {
		T1 = T1_plus;

	} अन्यथा जर ((T1_minus >= 0.f && T3_minus >= 0.f) && ((T1_minus + T3_minus) <= T123)) {
		T1 = T1_minus;
	}

	T1 = saturateT1ForAccel(a0, j_max, T1, a_max);

	परत T1;
}


दशांश VelocitySmoothing::computeT2(दशांश T1, दशांश T3, दशांश a0, दशांश v3, दशांश j_max) स्थिरांक
{
	दशांश T2 = 0.f;

	दशांश den = a0 + j_max * T1;

	जर (math::abs_t(den) > भग्न_अंतर) {
		T2 = (-0.5f * T1 * T1 * j_max - T1 * T3 * j_max - T1 * a0 + 0.5f * T3 * T3 * j_max - T3 * a0 + v3) / den;
	}

	परत math::max(T2, 0.f);
}

दशांश VelocitySmoothing::computeT2(दशांश T123, दशांश T1, दशांश T3) स्थिरांक
{
	दशांश T2 = T123 - T1 - T3;
	परत math::max(T2, 0.f);
}

दशांश VelocitySmoothing::computeT3(दशांश T1, दशांश a0, दशांश j_max) स्थिरांक
{
	दशांश T3 = a0 / j_max + T1;
	परत math::max(T3, 0.f);
}

शून्य VelocitySmoothing::updateDurations(दशांश vel_setpoint)
{
	_vel_sp = math::constrain(vel_setpoint, -_max_vel, _max_vel);
	_local_time = 0.f;
	_state_init = _state;

	_direction = computeDirection();

	updateDurationsMinimizeTotalTime();
}

पूर्णांक VelocitySmoothing::computeDirection() स्थिरांक
{
	// Compute the velocity at which the trajectory will be
	// when the acceleration will be zero
	दशांश vel_zero_acc = computeVelAtZeroAcc();

	/* Depending of the direction, start accelerating positively or negatively */
	पूर्णांक direction = sign(_vel_sp - vel_zero_acc);

	जर (direction == 0) {
		// If by braking immediately the velocity is exactly
		// the required one with zero acceleration, then brake
		direction = sign(_state.a);
	}

	परत direction;
}

दशांश VelocitySmoothing::computeVelAtZeroAcc() स्थिरांक
{
	दशांश vel_zero_acc = _state.v;

	जर (fabsf(_state.a) > भग्न_अंतर) {
		दशांश j_zero_acc = -sign(_state.a) * _max_jerk; // Required jerk to reduce the acceleration
		दशांश t_zero_acc = -_state.a / j_zero_acc; // Required time to cancel the current acceleration
		vel_zero_acc = _state.v + _state.a * t_zero_acc + 0.5f * j_zero_acc * t_zero_acc * t_zero_acc;
	}

	परत vel_zero_acc;
}

शून्य VelocitySmoothing::updateDurationsMinimizeTotalTime()
{
	दशांश jerk_max_T1 = _direction * _max_jerk;
	दशांश delta_v = _vel_sp - _state.v;

	जर (fabsf(jerk_max_T1) > भग्न_अंतर) { // zero direction or jerk would lead to division by zero
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

Trajectory VelocitySmoothing::evaluatePoly(दशांश j, दशांश a0, दशांश v0, दशांश x0, दशांश t, पूर्णांक d) स्थिरांक
{
	Trajectory traj;
	दशांश jt = d * j;
	दशांश t2 = t * t;
	दशांश t3 = t2 * t;

	traj.j = jt;
	traj.a = a0 + jt * t;
	traj.v = v0 + a0 * t + 0.5f * jt * t2;
	traj.x = x0 + v0 * t + 0.5f * a0 * t2 + 1.f / 6.f * jt * t3;

	परत traj;
}

शून्य VelocitySmoothing::updateTraj(दशांश dt, दशांश time_stretch)
{
	_local_time += dt * time_stretch;
	दशांश t_remain = _local_time;

	दशांश t1 = math::min(t_remain, _T1);
	_state = evaluatePoly(_max_jerk, _state_init.a, _state_init.v, _state_init.x, t1, _direction);
	t_remain -= t1;

	जर (t_remain > 0.f) {
		दशांश t2 = math::min(t_remain, _T2);
		_state = evaluatePoly(0.f, _state.a, _state.v, _state.x, t2, 0.f);
		t_remain -= t2;
	}

	जर (t_remain > 0.f) {
		दशांश t3 = math::min(t_remain, _T3);
		_state = evaluatePoly(_max_jerk, _state.a, _state.v, _state.x, t3, -_direction);
		t_remain -= t3;
	}

	जर (t_remain > 0.f) {
		_state = evaluatePoly(0.f, 0.f, _state.v, _state.x, t_remain, 0.f);
	}
}

शून्य VelocitySmoothing::timeSynchronization(VelocitySmoothing *traj, पूर्णांक n_traj)
{
	दशांश desired_time = 0.f;
	पूर्णांक longest_traj_index = 0;

	साठी (पूर्णांक i = 0; i < n_traj; i++) {
		स्थिरांक दशांश T123 = traj[i].getTotalTime();

		जर (T123 > desired_time) {
			desired_time = T123;
			longest_traj_index = i;
		}
	}

	जर (desired_time > भग्न_अंतर) {
		साठी (पूर्णांक i = 0; i < n_traj; i++) {
			जर ((i != longest_traj_index)
			    && (traj[i].getTotalTime() < desired_time)) {
				traj[i].updateDurationsGivenTotalTime(desired_time);
			}
		}
	}
}

शून्य VelocitySmoothing::updateDurationsGivenTotalTime(दशांश T123)
{
	दशांश jerk_max_T1 = _direction * _max_jerk;
	दशांश delta_v = _vel_sp - _state.v;

	जर (fabsf(jerk_max_T1) > भग्न_अंतर) { // zero direction or jerk would lead to division by zero
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

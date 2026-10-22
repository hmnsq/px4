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

#ਅਨ੍ਤਰ੍ਭਾਵ "VelocitySmoothing.hpp"

#ਅਨ੍ਤਰ੍ਭਾਵ <cstdio>
#ਅਨ੍ਤਰ੍ਭਾਵ <float.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <mathlib/mathlib.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <matrix/matrix/math.hpp>

ਵਰਤੋਂ ਮੈਟ੍ਰਿਕਸ::sign;

VelocitySmoothing::VelocitySmoothing(ਦਸ਼ਮਲਵ initial_accel, ਦਸ਼ਮਲਵ initial_vel, ਦਸ਼ਮਲਵ initial_pos)
{
	reset(initial_accel, initial_vel, initial_pos);
}

ਖਾਲੀ VelocitySmoothing::reset(ਦਸ਼ਮਲਵ accel, ਦਸ਼ਮਲਵ vel, ਦਸ਼ਮਲਵ pos)
{
	_state.j = 0.f;
	_state.a = accel;
	_state.v = vel;
	_state.x = pos;

	_state_init = _state;
}

ਦਸ਼ਮਲਵ VelocitySmoothing::saturateT1ForAccel(ਦਸ਼ਮਲਵ a0, ਦਸ਼ਮਲਵ j_max, ਦਸ਼ਮਲਵ T1, ਦਸ਼ਮਲਵ a_max) ਅਚਲ
{
	/* Check maximum acceleration, saturate and recompute T1 if needed */
	ਦਸ਼ਮਲਵ accel_T1 = a0 + j_max * T1;
	ਦਸ਼ਮਲਵ T1_new = T1;

	ਜੇ (accel_T1 > a_max) {
		T1_new = (a_max - a0) / j_max;

	} ਨਹੀਂਤਾਂ ਜੇ (accel_T1 < -a_max) {
		T1_new = (-a_max - a0) / j_max;
	}

	ਵਾਪਸ T1_new;
}

ਦਸ਼ਮਲਵ VelocitySmoothing::computeT1(ਦਸ਼ਮਲਵ a0, ਦਸ਼ਮਲਵ v3, ਦਸ਼ਮਲਵ j_max, ਦਸ਼ਮਲਵ a_max) ਅਚਲ
{
	ਦਸ਼ਮਲਵ delta = 2.f * a0 * a0 + 4.f * j_max * v3;

	ਜੇ (delta < 0.f) {
		// Solution is not real
		ਵਾਪਸ 0.f;
	}

	ਦਸ਼ਮਲਵ sqrt_delta = sqrtf(delta);
	ਦਸ਼ਮਲਵ T1_plus = (-a0 + 0.5f * sqrt_delta) / j_max;
	ਦਸ਼ਮਲਵ T1_minus = (-a0 - 0.5f * sqrt_delta) / j_max;

	ਦਸ਼ਮਲਵ T3_plus = a0 / j_max + T1_plus;
	ਦਸ਼ਮਲਵ T3_minus = a0 / j_max + T1_minus;

	ਦਸ਼ਮਲਵ T1 = 0.f;

	ਜੇ (T1_plus >= 0.f && T3_plus >= 0.f) {
		T1 = T1_plus;

	} ਨਹੀਂਤਾਂ ਜੇ (T1_minus >= 0.f && T3_minus >= 0.f) {
		T1 = T1_minus;
	}

	T1 = saturateT1ForAccel(a0, j_max, T1, a_max);

	ਵਾਪਸ math::max(T1, 0.f);
}

ਦਸ਼ਮਲਵ VelocitySmoothing::computeT1(ਦਸ਼ਮਲਵ T123, ਦਸ਼ਮਲਵ a0, ਦਸ਼ਮਲਵ v3, ਦਸ਼ਮਲਵ j_max, ਦਸ਼ਮਲਵ a_max) ਅਚਲ
{
	ਦਸ਼ਮਲਵ a = -j_max;
	ਦਸ਼ਮਲਵ b = j_max * T123 - a0;
	ਦਸ਼ਮਲਵ delta = T123 * T123 * j_max * j_max + 2.f * T123 * a0 * j_max - a0 * a0 - 4.f * j_max * v3;

	ਜੇ (delta < 0.f) {
		// Solution is not real
		ਵਾਪਸ 0.f;
	}

	ਦਸ਼ਮਲਵ sqrt_delta = sqrtf(delta);
	ਦਸ਼ਮਲਵ denominator_inv = 1.f / (2.f * a);
	ਦਸ਼ਮਲਵ T1_plus = math::max((-b + sqrt_delta) * denominator_inv, 0.f);
	ਦਸ਼ਮਲਵ T1_minus = math::max((-b - sqrt_delta) * denominator_inv, 0.f);

	ਦਸ਼ਮਲਵ T3_plus = a0 / j_max + T1_plus;
	ਦਸ਼ਮਲਵ T3_minus = a0 / j_max + T1_minus;

	ਦਸ਼ਮਲਵ T1 = 0.f;

	ਜੇ ((T1_plus >= 0.f && T3_plus >= 0.f) && ((T1_plus + T3_plus) <= T123)) {
		T1 = T1_plus;

	} ਨਹੀਂਤਾਂ ਜੇ ((T1_minus >= 0.f && T3_minus >= 0.f) && ((T1_minus + T3_minus) <= T123)) {
		T1 = T1_minus;
	}

	T1 = saturateT1ForAccel(a0, j_max, T1, a_max);

	ਵਾਪਸ T1;
}


ਦਸ਼ਮਲਵ VelocitySmoothing::computeT2(ਦਸ਼ਮਲਵ T1, ਦਸ਼ਮਲਵ T3, ਦਸ਼ਮਲਵ a0, ਦਸ਼ਮਲਵ v3, ਦਸ਼ਮਲਵ j_max) ਅਚਲ
{
	ਦਸ਼ਮਲਵ T2 = 0.f;

	ਦਸ਼ਮਲਵ den = a0 + j_max * T1;

	ਜੇ (math::abs_t(den) > FLT_EPSILON) {
		T2 = (-0.5f * T1 * T1 * j_max - T1 * T3 * j_max - T1 * a0 + 0.5f * T3 * T3 * j_max - T3 * a0 + v3) / den;
	}

	ਵਾਪਸ math::max(T2, 0.f);
}

ਦਸ਼ਮਲਵ VelocitySmoothing::computeT2(ਦਸ਼ਮਲਵ T123, ਦਸ਼ਮਲਵ T1, ਦਸ਼ਮਲਵ T3) ਅਚਲ
{
	ਦਸ਼ਮਲਵ T2 = T123 - T1 - T3;
	ਵਾਪਸ math::max(T2, 0.f);
}

ਦਸ਼ਮਲਵ VelocitySmoothing::computeT3(ਦਸ਼ਮਲਵ T1, ਦਸ਼ਮਲਵ a0, ਦਸ਼ਮਲਵ j_max) ਅਚਲ
{
	ਦਸ਼ਮਲਵ T3 = a0 / j_max + T1;
	ਵਾਪਸ math::max(T3, 0.f);
}

ਖਾਲੀ VelocitySmoothing::updateDurations(ਦਸ਼ਮਲਵ vel_setpoint)
{
	_vel_sp = math::constrain(vel_setpoint, -_max_vel, _max_vel);
	local_ਸਮਾਂ = 0.f;
	_state_init = _state;

	_direction = computeDirection();

	update_Durations_Minimize_ਕੁੱਲ_ਸਮਾਂ();
}

ਪੂਰਨ VelocitySmoothing::computeDirection() ਅਚਲ
{
	// Compute the velocity at which the trajectory will be
	// when the acceleration will be zero
	ਦਸ਼ਮਲਵ vel_zero_acc = computeVelAtZeroAcc();

	/* Depending of the direction, start accelerating positively or negatively */
	ਪੂਰਨ direction = sign(_vel_sp - vel_zero_acc);

	ਜੇ (direction == 0) {
		// If by braking immediately the velocity is exactly
		// the required one with zero acceleration, then brake
		direction = sign(_state.a);
	}

	ਵਾਪਸ direction;
}

ਦਸ਼ਮਲਵ VelocitySmoothing::computeVelAtZeroAcc() ਅਚਲ
{
	ਦਸ਼ਮਲਵ vel_zero_acc = _state.v;

	ਜੇ (fabsf(_state.a) > FLT_EPSILON) {
		ਦਸ਼ਮਲਵ j_zero_acc = -sign(_state.a) * _max_jerk; // Required jerk to reduce the acceleration
		ਦਸ਼ਮਲਵ t_zero_acc = -_state.a / j_zero_acc; // Required time to cancel the current acceleration
		vel_zero_acc = _state.v + _state.a * t_zero_acc + 0.5f * j_zero_acc * t_zero_acc * t_zero_acc;
	}

	ਵਾਪਸ vel_zero_acc;
}

ਖਾਲੀ VelocitySmoothing::update_Durations_Minimize_ਕੁੱਲ_ਸਮਾਂ()
{
	ਦਸ਼ਮਲਵ jerk_max_T1 = _direction * _max_jerk;
	ਦਸ਼ਮਲਵ delta_v = _vel_sp - _state.v;

	ਜੇ (fabsf(jerk_max_T1) > FLT_EPSILON) { // zero direction or jerk would lead to division by zero
		// compute increasing acceleration time
		_T1 = computeT1(_state.a, delta_v, jerk_max_T1, _max_accel);

		// compute decreasing acceleration time
		_T3 = computeT3(_T1, _state.a, jerk_max_T1);

		// compute constant acceleration time
		_T2 = computeT2(_T1, _T3, _state.a, delta_v, jerk_max_T1);

	} ਨਹੀਂਤਾਂ {
		_T1 = _T2 = _T3 = 0.f;
	}
}

Trajectory VelocitySmoothing::evaluatePoly(ਦਸ਼ਮਲਵ j, ਦਸ਼ਮਲਵ a0, ਦਸ਼ਮਲਵ v0, ਦਸ਼ਮਲਵ x0, ਦਸ਼ਮਲਵ t, ਪੂਰਨ d) ਅਚਲ
{
	Trajectory traj;
	ਦਸ਼ਮਲਵ jt = d * j;
	ਦਸ਼ਮਲਵ t2 = t * t;
	ਦਸ਼ਮਲਵ t3 = t2 * t;

	traj.j = jt;
	traj.a = a0 + jt * t;
	traj.v = v0 + a0 * t + 0.5f * jt * t2;
	traj.x = x0 + v0 * t + 0.5f * a0 * t2 + 1.f / 6.f * jt * t3;

	ਵਾਪਸ traj;
}

ਖਾਲੀ VelocitySmoothing::updateTraj(ਦਸ਼ਮਲਵ dt, ਦਸ਼ਮਲਵ ਸਮਾਂ_stretch)
{
	local_ਸਮਾਂ += dt * ਸਮਾਂ_stretch;
	ਦਸ਼ਮਲਵ t_remain = local_ਸਮਾਂ;

	ਦਸ਼ਮਲਵ t1 = math::min(t_remain, _T1);
	_state = evaluatePoly(_max_jerk, _state_init.a, _state_init.v, _state_init.x, t1, _direction);
	t_remain -= t1;

	ਜੇ (t_remain > 0.f) {
		ਦਸ਼ਮਲਵ t2 = math::min(t_remain, _T2);
		_state = evaluatePoly(0.f, _state.a, _state.v, _state.x, t2, 0.f);
		t_remain -= t2;
	}

	ਜੇ (t_remain > 0.f) {
		ਦਸ਼ਮਲਵ t3 = math::min(t_remain, _T3);
		_state = evaluatePoly(_max_jerk, _state.a, _state.v, _state.x, t3, -_direction);
		t_remain -= t3;
	}

	ਜੇ (t_remain > 0.f) {
		_state = evaluatePoly(0.f, 0.f, _state.v, _state.x, t_remain, 0.f);
	}
}

ਖਾਲੀ VelocitySmoothing::ਸਮਾਂ_Synchronization(VelocitySmoothing *traj, ਪੂਰਨ n_traj)
{
	ਦਸ਼ਮਲਵ desired_ਸਮਾਂ = 0.f;
	ਪੂਰਨ longest_traj_ਸੂਚਕਾਂਕ = 0;

	ਲਈ (ਪੂਰਨ i = 0; i < n_traj; i++) {
		ਅਚਲ ਦਸ਼ਮਲਵ T123 = traj[i].get_ਕੁੱਲ_ਸਮਾਂ();

		ਜੇ (T123 > desired_ਸਮਾਂ) {
			desired_ਸਮਾਂ = T123;
			longest_traj_ਸੂਚਕਾਂਕ = i;
		}
	}

	ਜੇ (desired_ਸਮਾਂ > FLT_EPSILON) {
		ਲਈ (ਪੂਰਨ i = 0; i < n_traj; i++) {
			ਜੇ ((i != longest_traj_ਸੂਚਕਾਂਕ)
			    && (traj[i].get_ਕੁੱਲ_ਸਮਾਂ() < desired_ਸਮਾਂ)) {
				traj[i].update_Durations_Given_ਕੁੱਲ_ਸਮਾਂ(desired_ਸਮਾਂ);
			}
		}
	}
}

ਖਾਲੀ VelocitySmoothing::update_Durations_Given_ਕੁੱਲ_ਸਮਾਂ(ਦਸ਼ਮਲਵ T123)
{
	ਦਸ਼ਮਲਵ jerk_max_T1 = _direction * _max_jerk;
	ਦਸ਼ਮਲਵ delta_v = _vel_sp - _state.v;

	ਜੇ (fabsf(jerk_max_T1) > FLT_EPSILON) { // zero direction or jerk would lead to division by zero
		// compute increasing acceleration time
		_T1 = computeT1(T123, _state.a, delta_v, jerk_max_T1, _max_accel);

		// compute decreasing acceleration time
		_T3 = computeT3(_T1, _state.a, jerk_max_T1);

		// compute constant acceleration time
		_T2 = computeT2(T123, _T1, _T3);

	} ਨਹੀਂਤਾਂ {
		_T1 = _T2 = _T3 = 0.f;
	}
}

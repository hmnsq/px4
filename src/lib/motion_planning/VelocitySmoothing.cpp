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

#ಅನ್ತರ್ಭಾವ "VelocitySmoothing.hpp"

#ಅನ್ತರ್ಭಾವ <cstdio>
#ಅನ್ತರ್ಭಾವ <float.h>
#ಅನ್ತರ್ಭಾವ <mathlib/mathlib.h>
#ಅನ್ತರ್ಭಾವ <matrix/matrix/math.hpp>

ಬಳಸಿ ಮಾತೃಕೆ::sign;

VelocitySmoothing::VelocitySmoothing(ದಶಮಾಂಶ initial_accel, ದಶಮಾಂಶ initial_vel, ದಶಮಾಂಶ initial_pos)
{
	reset(initial_accel, initial_vel, initial_pos);
}

ಶೂನ್ಯ VelocitySmoothing::reset(ದಶಮಾಂಶ accel, ದಶಮಾಂಶ vel, ದಶಮಾಂಶ pos)
{
	_state.j = 0.f;
	_state.a = accel;
	_state.v = vel;
	_state.x = pos;

	_state_init = _state;
}

ದಶಮಾಂಶ VelocitySmoothing::saturateT1ForAccel(ದಶಮಾಂಶ a0, ದಶಮಾಂಶ j_max, ದಶಮಾಂಶ T1, ದಶಮಾಂಶ a_max) ಸ್ಥಿರಾಂಕ
{
	/* Check maximum acceleration, saturate and recompute T1 if needed */
	ದಶಮಾಂಶ accel_T1 = a0 + j_max * T1;
	ದಶಮಾಂಶ T1_new = T1;

	ಆದರೆ (accel_T1 > a_max) {
		T1_new = (a_max - a0) / j_max;

	} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (accel_T1 < -a_max) {
		T1_new = (-a_max - a0) / j_max;
	}

	ಹಿಂತಿರುಗಿ T1_new;
}

ದಶಮಾಂಶ VelocitySmoothing::computeT1(ದಶಮಾಂಶ a0, ದಶಮಾಂಶ v3, ದಶಮಾಂಶ j_max, ದಶಮಾಂಶ a_max) ಸ್ಥಿರಾಂಕ
{
	ದಶಮಾಂಶ delta = 2.f * a0 * a0 + 4.f * j_max * v3;

	ಆದರೆ (delta < 0.f) {
		// Solution is not real
		ಹಿಂತಿರುಗಿ 0.f;
	}

	ದಶಮಾಂಶ sqrt_delta = sqrtf(delta);
	ದಶಮಾಂಶ T1_plus = (-a0 + 0.5f * sqrt_delta) / j_max;
	ದಶಮಾಂಶ T1_minus = (-a0 - 0.5f * sqrt_delta) / j_max;

	ದಶಮಾಂಶ T3_plus = a0 / j_max + T1_plus;
	ದಶಮಾಂಶ T3_minus = a0 / j_max + T1_minus;

	ದಶಮಾಂಶ T1 = 0.f;

	ಆದರೆ (T1_plus >= 0.f && T3_plus >= 0.f) {
		T1 = T1_plus;

	} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (T1_minus >= 0.f && T3_minus >= 0.f) {
		T1 = T1_minus;
	}

	T1 = saturateT1ForAccel(a0, j_max, T1, a_max);

	ಹಿಂತಿರುಗಿ math::max(T1, 0.f);
}

ದಶಮಾಂಶ VelocitySmoothing::computeT1(ದಶಮಾಂಶ T123, ದಶಮಾಂಶ a0, ದಶಮಾಂಶ v3, ದಶಮಾಂಶ j_max, ದಶಮಾಂಶ a_max) ಸ್ಥಿರಾಂಕ
{
	ದಶಮಾಂಶ a = -j_max;
	ದಶಮಾಂಶ b = j_max * T123 - a0;
	ದಶಮಾಂಶ delta = T123 * T123 * j_max * j_max + 2.f * T123 * a0 * j_max - a0 * a0 - 4.f * j_max * v3;

	ಆದರೆ (delta < 0.f) {
		// Solution is not real
		ಹಿಂತಿರುಗಿ 0.f;
	}

	ದಶಮಾಂಶ sqrt_delta = sqrtf(delta);
	ದಶಮಾಂಶ denominator_inv = 1.f / (2.f * a);
	ದಶಮಾಂಶ T1_plus = math::max((-b + sqrt_delta) * denominator_inv, 0.f);
	ದಶಮಾಂಶ T1_minus = math::max((-b - sqrt_delta) * denominator_inv, 0.f);

	ದಶಮಾಂಶ T3_plus = a0 / j_max + T1_plus;
	ದಶಮಾಂಶ T3_minus = a0 / j_max + T1_minus;

	ದಶಮಾಂಶ T1 = 0.f;

	ಆದರೆ ((T1_plus >= 0.f && T3_plus >= 0.f) && ((T1_plus + T3_plus) <= T123)) {
		T1 = T1_plus;

	} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((T1_minus >= 0.f && T3_minus >= 0.f) && ((T1_minus + T3_minus) <= T123)) {
		T1 = T1_minus;
	}

	T1 = saturateT1ForAccel(a0, j_max, T1, a_max);

	ಹಿಂತಿರುಗಿ T1;
}


ದಶಮಾಂಶ VelocitySmoothing::computeT2(ದಶಮಾಂಶ T1, ದಶಮಾಂಶ T3, ದಶಮಾಂಶ a0, ದಶಮಾಂಶ v3, ದಶಮಾಂಶ j_max) ಸ್ಥಿರಾಂಕ
{
	ದಶಮಾಂಶ T2 = 0.f;

	ದಶಮಾಂಶ den = a0 + j_max * T1;

	ಆದರೆ (math::abs_t(den) > FLT_EPSILON) {
		T2 = (-0.5f * T1 * T1 * j_max - T1 * T3 * j_max - T1 * a0 + 0.5f * T3 * T3 * j_max - T3 * a0 + v3) / den;
	}

	ಹಿಂತಿರುಗಿ math::max(T2, 0.f);
}

ದಶಮಾಂಶ VelocitySmoothing::computeT2(ದಶಮಾಂಶ T123, ದಶಮಾಂಶ T1, ದಶಮಾಂಶ T3) ಸ್ಥಿರಾಂಕ
{
	ದಶಮಾಂಶ T2 = T123 - T1 - T3;
	ಹಿಂತಿರುಗಿ math::max(T2, 0.f);
}

ದಶಮಾಂಶ VelocitySmoothing::computeT3(ದಶಮಾಂಶ T1, ದಶಮಾಂಶ a0, ದಶಮಾಂಶ j_max) ಸ್ಥಿರಾಂಕ
{
	ದಶಮಾಂಶ T3 = a0 / j_max + T1;
	ಹಿಂತಿರುಗಿ math::max(T3, 0.f);
}

ಶೂನ್ಯ VelocitySmoothing::updateDurations(ದಶಮಾಂಶ vel_setpoint)
{
	_vel_sp = math::constrain(vel_setpoint, -_max_vel, _max_vel);
	local_ಸಮಯ = 0.f;
	_state_init = _state;

	_direction = computeDirection();

	update_Durations_Minimize_ಒಟ್ಟು_ಸಮಯ();
}

ಪೂರ್ಣಾಂಕ VelocitySmoothing::computeDirection() ಸ್ಥಿರಾಂಕ
{
	// Compute the velocity at which the trajectory will be
	// when the acceleration will be zero
	ದಶಮಾಂಶ vel_zero_acc = computeVelAtZeroAcc();

	/* Depending of the direction, start accelerating positively or negatively */
	ಪೂರ್ಣಾಂಕ direction = sign(_vel_sp - vel_zero_acc);

	ಆದರೆ (direction == 0) {
		// If by braking immediately the velocity is exactly
		// the required one with zero acceleration, then brake
		direction = sign(_state.a);
	}

	ಹಿಂತಿರುಗಿ direction;
}

ದಶಮಾಂಶ VelocitySmoothing::computeVelAtZeroAcc() ಸ್ಥಿರಾಂಕ
{
	ದಶಮಾಂಶ vel_zero_acc = _state.v;

	ಆದರೆ (fabsf(_state.a) > FLT_EPSILON) {
		ದಶಮಾಂಶ j_zero_acc = -sign(_state.a) * _max_jerk; // Required jerk to reduce the acceleration
		ದಶಮಾಂಶ t_zero_acc = -_state.a / j_zero_acc; // Required time to cancel the current acceleration
		vel_zero_acc = _state.v + _state.a * t_zero_acc + 0.5f * j_zero_acc * t_zero_acc * t_zero_acc;
	}

	ಹಿಂತಿರುಗಿ vel_zero_acc;
}

ಶೂನ್ಯ VelocitySmoothing::update_Durations_Minimize_ಒಟ್ಟು_ಸಮಯ()
{
	ದಶಮಾಂಶ jerk_max_T1 = _direction * _max_jerk;
	ದಶಮಾಂಶ delta_v = _vel_sp - _state.v;

	ಆದರೆ (fabsf(jerk_max_T1) > FLT_EPSILON) { // zero direction or jerk would lead to division by zero
		// compute increasing acceleration time
		_T1 = computeT1(_state.a, delta_v, jerk_max_T1, _max_accel);

		// compute decreasing acceleration time
		_T3 = computeT3(_T1, _state.a, jerk_max_T1);

		// compute constant acceleration time
		_T2 = computeT2(_T1, _T3, _state.a, delta_v, jerk_max_T1);

	} ಇಲ್ಲದಿದ್ದರೆ {
		_T1 = _T2 = _T3 = 0.f;
	}
}

Trajectory VelocitySmoothing::evaluatePoly(ದಶಮಾಂಶ j, ದಶಮಾಂಶ a0, ದಶಮಾಂಶ v0, ದಶಮಾಂಶ x0, ದಶಮಾಂಶ t, ಪೂರ್ಣಾಂಕ d) ಸ್ಥಿರಾಂಕ
{
	Trajectory traj;
	ದಶಮಾಂಶ jt = d * j;
	ದಶಮಾಂಶ t2 = t * t;
	ದಶಮಾಂಶ t3 = t2 * t;

	traj.j = jt;
	traj.a = a0 + jt * t;
	traj.v = v0 + a0 * t + 0.5f * jt * t2;
	traj.x = x0 + v0 * t + 0.5f * a0 * t2 + 1.f / 6.f * jt * t3;

	ಹಿಂತಿರುಗಿ traj;
}

ಶೂನ್ಯ VelocitySmoothing::updateTraj(ದಶಮಾಂಶ dt, ದಶಮಾಂಶ ಸಮಯ_stretch)
{
	local_ಸಮಯ += dt * ಸಮಯ_stretch;
	ದಶಮಾಂಶ t_remain = local_ಸಮಯ;

	ದಶಮಾಂಶ t1 = math::min(t_remain, _T1);
	_state = evaluatePoly(_max_jerk, _state_init.a, _state_init.v, _state_init.x, t1, _direction);
	t_remain -= t1;

	ಆದರೆ (t_remain > 0.f) {
		ದಶಮಾಂಶ t2 = math::min(t_remain, _T2);
		_state = evaluatePoly(0.f, _state.a, _state.v, _state.x, t2, 0.f);
		t_remain -= t2;
	}

	ಆದರೆ (t_remain > 0.f) {
		ದಶಮಾಂಶ t3 = math::min(t_remain, _T3);
		_state = evaluatePoly(_max_jerk, _state.a, _state.v, _state.x, t3, -_direction);
		t_remain -= t3;
	}

	ಆದರೆ (t_remain > 0.f) {
		_state = evaluatePoly(0.f, 0.f, _state.v, _state.x, t_remain, 0.f);
	}
}

ಶೂನ್ಯ VelocitySmoothing::ಸಮಯ_Synchronization(VelocitySmoothing *traj, ಪೂರ್ಣಾಂಕ n_traj)
{
	ದಶಮಾಂಶ desired_ಸಮಯ = 0.f;
	ಪೂರ್ಣಾಂಕ longest_traj_ಸೂಚ್ಯಂಕ = 0;

	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < n_traj; i++) {
		ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ T123 = traj[i].get_ಒಟ್ಟು_ಸಮಯ();

		ಆದರೆ (T123 > desired_ಸಮಯ) {
			desired_ಸಮಯ = T123;
			longest_traj_ಸೂಚ್ಯಂಕ = i;
		}
	}

	ಆದರೆ (desired_ಸಮಯ > FLT_EPSILON) {
		ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < n_traj; i++) {
			ಆದರೆ ((i != longest_traj_ಸೂಚ್ಯಂಕ)
			    && (traj[i].get_ಒಟ್ಟು_ಸಮಯ() < desired_ಸಮಯ)) {
				traj[i].update_Durations_Given_ಒಟ್ಟು_ಸಮಯ(desired_ಸಮಯ);
			}
		}
	}
}

ಶೂನ್ಯ VelocitySmoothing::update_Durations_Given_ಒಟ್ಟು_ಸಮಯ(ದಶಮಾಂಶ T123)
{
	ದಶಮಾಂಶ jerk_max_T1 = _direction * _max_jerk;
	ದಶಮಾಂಶ delta_v = _vel_sp - _state.v;

	ಆದರೆ (fabsf(jerk_max_T1) > FLT_EPSILON) { // zero direction or jerk would lead to division by zero
		// compute increasing acceleration time
		_T1 = computeT1(T123, _state.a, delta_v, jerk_max_T1, _max_accel);

		// compute decreasing acceleration time
		_T3 = computeT3(_T1, _state.a, jerk_max_T1);

		// compute constant acceleration time
		_T2 = computeT2(T123, _T1, _T3);

	} ಇಲ್ಲದಿದ್ದರೆ {
		_T1 = _T2 = _T3 = 0.f;
	}
}

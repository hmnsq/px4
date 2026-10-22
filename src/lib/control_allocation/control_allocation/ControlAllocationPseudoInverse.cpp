/****************************************************************************
 *
 *   Copyright (c) 2019 PX4 Development Team. All rights reserved.
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

/**
 * @file ControlAllocationPseudoInverse.cpp
 *
 * Simple Control Allocation Algorithm
 *
 * @author Julien Lecoeur <julien.lecoeur@gmail.com>
 */

#ਅਨ੍ਤਰ੍ਭਾਵ "ControlAllocationPseudoInverse.hpp"

ਖਾਲੀ
Control_ਵੰਡ_Pseudo_Inverse::ਸੈੱਟ_Effectiveness_ਮੈਟ੍ਰਿਕਸ(
	ਅਚਲ ਮੈਟ੍ਰਿਕਸ::ਮੈਟ੍ਰਿਕਸ_2<ਦਸ਼ਮਲਵ, Control_ਵੰਡ::ਅੰਕ_AXES, Control_ਵੰਡ::ਅੰਕ_ACTUATORS> &effectiveness,
	ਅਚਲ Actuator_ਸਦਿਸ਼ &actuator_trim, ਅਚਲ Actuator_ਸਦਿਸ਼ &linearization_ਬਿੰਦੂ, ਪੂਰਨ ਅੰਕ_actuators,
	ਤਾਰਕਿਕ update_normalization_scale)
{
	Control_ਵੰਡ::ਸੈੱਟ_Effectiveness_ਮੈਟ੍ਰਿਕਸ(effectiveness, actuator_trim, linearization_ਬਿੰਦੂ, ਅੰਕ_actuators,
			update_normalization_scale);

	// in place on the stored copy: no second matrix on the (small work queue) stack
	_dropped_axes = dropDependentAxes(_effectiveness);

	_mix_update_needed = ਸੱਚ;
	_normalization_needs_update = update_normalization_scale;

	ਜੇ (metric_ਵੰਡ && update_normalization_scale) {
		// adding #include <px4_platform_common/log.h> + PX4_WARN leads to failed linking on test
		_normalization_needs_update = ਝੂਠ;
	}
}

ਖਾਲੀ
Control_ਵੰਡ_Pseudo_Inverse::updatePseudoInverse()
{
	ਜੇ (_mix_update_needed) {
		ਮੈਟ੍ਰਿਕਸ::geninv(_effectiveness, _mix);

		ਜੇ (!metric_ਵੰਡ) {
			ਜੇ (_normalization_needs_update && !_had_actuator_failure) {
				update_Control_ਵੰਡ_ਮੈਟ੍ਰਿਕਸ_Scale();
				_normalization_needs_update = ਝੂਠ;
			}

			normalize_Control_ਵੰਡ_ਮੈਟ੍ਰਿਕਸ();
		}

		_mix_update_needed = ਝੂਠ;
	}
}

ਖਾਲੀ
Control_ਵੰਡ_Pseudo_Inverse::update_Control_ਵੰਡ_ਮੈਟ੍ਰਿਕਸ_Scale()
{
	// Same scale on roll and pitch
	ਜੇ (_normalize_rpy) {

		ਪੂਰਨ ਅੰਕ_non_zero_roll_torque = 0;
		ਪੂਰਨ ਅੰਕ_non_zero_pitch_torque = 0;

		ਲਈ (ਪੂਰਨ i = 0; i < ਅੰਕ_actuators_2; i++) {

			ਜੇ (fabsf(_mix(i, 0)) > 1e-3f) {
				++ਅੰਕ_non_zero_roll_torque;
			}

			ਜੇ (fabsf(_mix(i, 1)) > 1e-3f) {
				++ਅੰਕ_non_zero_pitch_torque;
			}
		}

		ਦਸ਼ਮਲਵ roll_norm_scale = 1.f;

		ਜੇ (ਅੰਕ_non_zero_roll_torque > 0) {
			roll_norm_scale = sqrtf(_mix.ਖਾਨਾ(0).norm_squared() / (ਅੰਕ_non_zero_roll_torque / 2.f));
		}

		ਦਸ਼ਮਲਵ pitch_norm_scale = 1.f;

		ਜੇ (ਅੰਕ_non_zero_pitch_torque > 0) {
			pitch_norm_scale = sqrtf(_mix.ਖਾਨਾ(1).norm_squared() / (ਅੰਕ_non_zero_pitch_torque / 2.f));
		}

		control_ਵੰਡ_scale(0) = fmaxf(roll_norm_scale, pitch_norm_scale);
		control_ਵੰਡ_scale(1) = control_ਵੰਡ_scale(0);

		// Scale yaw separately
		control_ਵੰਡ_scale(2) = _mix.ਖਾਨਾ(2).max();

	} ਨਹੀਂਤਾਂ {
		control_ਵੰਡ_scale(0) = 1.f;
		control_ਵੰਡ_scale(1) = 1.f;
		control_ਵੰਡ_scale(2) = 1.f;
	}

	// Scale thrust by the sum of the individual thrust axes, and use the scaling for the Z axis if there's no actuators
	// (for tilted actuators)
	control_ਵੰਡ_scale(THRUST_Z) = 1.f;

	ਲਈ (ਪੂਰਨ axis_idx = 2; axis_idx >= 0; --axis_idx) {
		ਪੂਰਨ ਅੰਕ_non_zero_thrust = 0;
		ਦਸ਼ਮਲਵ norm_ਜੋੜ = 0.f;

		ਲਈ (ਪੂਰਨ i = 0; i < ਅੰਕ_actuators_2; i++) {
			ਦਸ਼ਮਲਵ norm = fabsf(_mix(i, 3 + axis_idx));
			norm_ਜੋੜ += norm;

			ਜੇ (norm > FLT_EPSILON) {
				++ਅੰਕ_non_zero_thrust;
			}
		}

		ਜੇ (ਅੰਕ_non_zero_thrust > 0) {
			control_ਵੰਡ_scale(3 + axis_idx) = norm_ਜੋੜ / ਅੰਕ_non_zero_thrust;

		} ਨਹੀਂਤਾਂ {
			control_ਵੰਡ_scale(3 + axis_idx) = control_ਵੰਡ_scale(THRUST_Z);
		}
	}
}

ਖਾਲੀ
Control_ਵੰਡ_Pseudo_Inverse::normalize_Control_ਵੰਡ_ਮੈਟ੍ਰਿਕਸ()
{
	ਜੇ (control_ਵੰਡ_scale(0) > FLT_EPSILON) {
		_mix.ਖਾਨਾ(0) /= control_ਵੰਡ_scale(0);
		_mix.ਖਾਨਾ(1) /= control_ਵੰਡ_scale(1);
	}

	ਜੇ (control_ਵੰਡ_scale(2) > FLT_EPSILON) {
		_mix.ਖਾਨਾ(2) /= control_ਵੰਡ_scale(2);
	}

	ਜੇ (control_ਵੰਡ_scale(3) > FLT_EPSILON) {
		_mix.ਖਾਨਾ(3) /= control_ਵੰਡ_scale(3);
		_mix.ਖਾਨਾ(4) /= control_ਵੰਡ_scale(4);
		_mix.ਖਾਨਾ(5) /= control_ਵੰਡ_scale(5);
	}

	// Set all the small elements to 0 to avoid issues
	// in the control allocation algorithms
	ਲਈ (ਪੂਰਨ i = 0; i < ਅੰਕ_actuators_2; i++) {
		ਲਈ (ਪੂਰਨ j = 0; j < ਅੰਕ_AXES; j++) {
			ਜੇ (fabsf(_mix(i, j)) < 1e-3f) {
				_mix(i, j) = 0.f;
			}
		}
	}
}

ਖਾਲੀ
Control_ਵੰਡ_Pseudo_Inverse::allocate()
{
	//Compute new gains if needed
	updatePseudoInverse();

	_prev_actuator_sp = _actuator_sp;

	// Allocate
	_actuator_sp = _actuator_trim + _mix * (_control_sp - _control_trim);
}

uint8_t
Control_ਵੰਡ_Pseudo_Inverse::dropDependentAxes(ਮੈਟ੍ਰਿਕਸ::ਮੈਟ੍ਰਿਕਸ_2<ਦਸ਼ਮਲਵ, ਅੰਕ_AXES, ਅੰਕ_ACTUATORS> &effectiveness)
{
	// highest priority first
	ਸਥਿਰ constexpr ControlAxis k_ਤਰਜੀਹ[ਅੰਕ_AXES] = {THRUST_Z, ROLL, PITCH, THRUST_X, THRUST_Y, YAW};

	// Gram-Schmidt through the Cholesky factor of the Gram matrix of the accepted unit rows:
	// equivalent to orthogonalizing the rows, without keeping NUM_ACTUATORS-long vectors on the
	// (small work queue) stack
	ਦਸ਼ਮਲਵ chol[ਅੰਕ_AXES][ਅੰਕ_AXES] {};
	ਦਸ਼ਮਲਵ inv_norm[ਅੰਕ_AXES];
	ControlAxis accepted[ਅੰਕ_AXES];
	ਪੂਰਨ ਅੰਕ_accepted = 0;
	uint8_t dropped = 0;

	ਲਈ (ਅਚਲ ControlAxis axis : k_ਤਰਜੀਹ) {
		ਅਚਲ ਦਸ਼ਮਲਵ norm_squared = effectiveness.ਪੰਕਤੀ(axis).norm_squared();

		ਜੇ (norm_squared < FLT_EPSILON) {
			ਜਾਰੀ; // unused axis
		}

		ਅਚਲ ਦਸ਼ਮਲਵ inv = 1.f / sqrtf(norm_squared);

		// projection of the unit row onto the orthonormal basis of the accepted rows
		ਦਸ਼ਮਲਵ projection[ਅੰਕ_AXES];
		ਦਸ਼ਮਲਵ projected_norm_squared = 0.f;

		ਲਈ (ਪੂਰਨ i = 0; i < ਅੰਕ_accepted; i++) {
			ਦਸ਼ਮਲਵ ਜੋੜ = effectiveness.ਪੰਕਤੀ(axis).dot(effectiveness.ਪੰਕਤੀ(accepted[i])) * inv * inv_norm[i];

			ਲਈ (ਪੂਰਨ j = 0; j < i; j++) {
				ਜੋੜ -= chol[i][j] * projection[j];
			}

			projection[i] = ਜੋੜ / chol[i][i];
			projected_norm_squared += projection[i] * projection[i];
		}

		ਅਚਲ ਦਸ਼ਮਲਵ independence = 1.f - projected_norm_squared;

		ਜੇ (independence < kMinAxisIndependence) {
			effectiveness.ਪੰਕਤੀ(axis) = 0.f;
			dropped |= static_cast<uint8_t>(1u << axis);

		} ਨਹੀਂਤਾਂ {
			ਲਈ (ਪੂਰਨ j = 0; j < ਅੰਕ_accepted; j++) {
				chol[ਅੰਕ_accepted][j] = projection[j];
			}

			chol[ਅੰਕ_accepted][ਅੰਕ_accepted] = sqrtf(independence);
			inv_norm[ਅੰਕ_accepted] = inv;
			accepted[ਅੰਕ_accepted] = axis;
			ਅੰਕ_accepted++;
		}
	}

	ਵਾਪਸ dropped;
}

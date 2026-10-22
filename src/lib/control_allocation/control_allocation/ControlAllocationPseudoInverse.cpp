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

#ಅನ್ತರ್ಭಾವ "ControlAllocationPseudoInverse.hpp"

ಶೂನ್ಯ
Control_ಹಂಚಿಕೆ_Pseudo_Inverse::ಗಣ_Effectiveness_ಮಾತೃಕೆ(
	ಸ್ಥಿರಾಂಕ ಮಾತೃಕೆ::ಮಾತೃಕೆ_2<ದಶಮಾಂಶ, Control_ಹಂಚಿಕೆ::ಅಂಕೆ_AXES, Control_ಹಂಚಿಕೆ::ಅಂಕೆ_ACTUATORS> &effectiveness,
	ಸ್ಥಿರಾಂಕ Actuator_ಸದಿಶ &actuator_trim, ಸ್ಥಿರಾಂಕ Actuator_ಸದಿಶ &linearization_ಬಿಂದು, ಪೂರ್ಣಾಂಕ ಅಂಕೆ_actuators,
	ತಾರ್ಕಿಕ update_normalization_scale)
{
	Control_ಹಂಚಿಕೆ::ಗಣ_Effectiveness_ಮಾತೃಕೆ(effectiveness, actuator_trim, linearization_ಬಿಂದು, ಅಂಕೆ_actuators,
			update_normalization_scale);

	// in place on the stored copy: no second matrix on the (small work queue) stack
	_dropped_axes = dropDependentAxes(_effectiveness);

	_mix_update_needed = ನಿಜ;
	_normalization_needs_update = update_normalization_scale;

	ಆದರೆ (metric_ಹಂಚಿಕೆ && update_normalization_scale) {
		// adding #include <px4_platform_common/log.h> + PX4_WARN leads to failed linking on test
		_normalization_needs_update = ಸುಳ್ಳು;
	}
}

ಶೂನ್ಯ
Control_ಹಂಚಿಕೆ_Pseudo_Inverse::updatePseudoInverse()
{
	ಆದರೆ (_mix_update_needed) {
		ಮಾತೃಕೆ::geninv(_effectiveness, _mix);

		ಆದರೆ (!metric_ಹಂಚಿಕೆ) {
			ಆದರೆ (_normalization_needs_update && !_had_actuator_failure) {
				update_Control_ಹಂಚಿಕೆ_ಮಾತೃಕೆ_Scale();
				_normalization_needs_update = ಸುಳ್ಳು;
			}

			normalize_Control_ಹಂಚಿಕೆ_ಮಾತೃಕೆ();
		}

		_mix_update_needed = ಸುಳ್ಳು;
	}
}

ಶೂನ್ಯ
Control_ಹಂಚಿಕೆ_Pseudo_Inverse::update_Control_ಹಂಚಿಕೆ_ಮಾತೃಕೆ_Scale()
{
	// Same scale on roll and pitch
	ಆದರೆ (_normalize_rpy) {

		ಪೂರ್ಣಾಂಕ ಅಂಕೆ_non_zero_roll_torque = 0;
		ಪೂರ್ಣಾಂಕ ಅಂಕೆ_non_zero_pitch_torque = 0;

		ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < ಅಂಕೆ_actuators_2; i++) {

			ಆದರೆ (fabsf(_mix(i, 0)) > 1e-3f) {
				++ಅಂಕೆ_non_zero_roll_torque;
			}

			ಆದರೆ (fabsf(_mix(i, 1)) > 1e-3f) {
				++ಅಂಕೆ_non_zero_pitch_torque;
			}
		}

		ದಶಮಾಂಶ roll_norm_scale = 1.f;

		ಆದರೆ (ಅಂಕೆ_non_zero_roll_torque > 0) {
			roll_norm_scale = sqrtf(_mix.ಲಂಬಸಾಲು(0).norm_squared() / (ಅಂಕೆ_non_zero_roll_torque / 2.f));
		}

		ದಶಮಾಂಶ pitch_norm_scale = 1.f;

		ಆದರೆ (ಅಂಕೆ_non_zero_pitch_torque > 0) {
			pitch_norm_scale = sqrtf(_mix.ಲಂಬಸಾಲು(1).norm_squared() / (ಅಂಕೆ_non_zero_pitch_torque / 2.f));
		}

		control_ಹಂಚಿಕೆ_scale(0) = fmaxf(roll_norm_scale, pitch_norm_scale);
		control_ಹಂಚಿಕೆ_scale(1) = control_ಹಂಚಿಕೆ_scale(0);

		// Scale yaw separately
		control_ಹಂಚಿಕೆ_scale(2) = _mix.ಲಂಬಸಾಲು(2).max();

	} ಇಲ್ಲದಿದ್ದರೆ {
		control_ಹಂಚಿಕೆ_scale(0) = 1.f;
		control_ಹಂಚಿಕೆ_scale(1) = 1.f;
		control_ಹಂಚಿಕೆ_scale(2) = 1.f;
	}

	// Scale thrust by the sum of the individual thrust axes, and use the scaling for the Z axis if there's no actuators
	// (for tilted actuators)
	control_ಹಂಚಿಕೆ_scale(THRUST_Z) = 1.f;

	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ axis_idx = 2; axis_idx >= 0; --axis_idx) {
		ಪೂರ್ಣಾಂಕ ಅಂಕೆ_non_zero_thrust = 0;
		ದಶಮಾಂಶ norm_ಮೊತ್ತ = 0.f;

		ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < ಅಂಕೆ_actuators_2; i++) {
			ದಶಮಾಂಶ norm = fabsf(_mix(i, 3 + axis_idx));
			norm_ಮೊತ್ತ += norm;

			ಆದರೆ (norm > FLT_EPSILON) {
				++ಅಂಕೆ_non_zero_thrust;
			}
		}

		ಆದರೆ (ಅಂಕೆ_non_zero_thrust > 0) {
			control_ಹಂಚಿಕೆ_scale(3 + axis_idx) = norm_ಮೊತ್ತ / ಅಂಕೆ_non_zero_thrust;

		} ಇಲ್ಲದಿದ್ದರೆ {
			control_ಹಂಚಿಕೆ_scale(3 + axis_idx) = control_ಹಂಚಿಕೆ_scale(THRUST_Z);
		}
	}
}

ಶೂನ್ಯ
Control_ಹಂಚಿಕೆ_Pseudo_Inverse::normalize_Control_ಹಂಚಿಕೆ_ಮಾತೃಕೆ()
{
	ಆದರೆ (control_ಹಂಚಿಕೆ_scale(0) > FLT_EPSILON) {
		_mix.ಲಂಬಸಾಲು(0) /= control_ಹಂಚಿಕೆ_scale(0);
		_mix.ಲಂಬಸಾಲು(1) /= control_ಹಂಚಿಕೆ_scale(1);
	}

	ಆದರೆ (control_ಹಂಚಿಕೆ_scale(2) > FLT_EPSILON) {
		_mix.ಲಂಬಸಾಲು(2) /= control_ಹಂಚಿಕೆ_scale(2);
	}

	ಆದರೆ (control_ಹಂಚಿಕೆ_scale(3) > FLT_EPSILON) {
		_mix.ಲಂಬಸಾಲು(3) /= control_ಹಂಚಿಕೆ_scale(3);
		_mix.ಲಂಬಸಾಲು(4) /= control_ಹಂಚಿಕೆ_scale(4);
		_mix.ಲಂಬಸಾಲು(5) /= control_ಹಂಚಿಕೆ_scale(5);
	}

	// Set all the small elements to 0 to avoid issues
	// in the control allocation algorithms
	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < ಅಂಕೆ_actuators_2; i++) {
		ಗಾಗಿ (ಪೂರ್ಣಾಂಕ j = 0; j < ಅಂಕೆ_AXES; j++) {
			ಆದರೆ (fabsf(_mix(i, j)) < 1e-3f) {
				_mix(i, j) = 0.f;
			}
		}
	}
}

ಶೂನ್ಯ
Control_ಹಂಚಿಕೆ_Pseudo_Inverse::allocate()
{
	//Compute new gains if needed
	updatePseudoInverse();

	_prev_actuator_sp = _actuator_sp;

	// Allocate
	_actuator_sp = _actuator_trim + _mix * (_control_sp - _control_trim);
}

uint8_t
Control_ಹಂಚಿಕೆ_Pseudo_Inverse::dropDependentAxes(ಮಾತೃಕೆ::ಮಾತೃಕೆ_2<ದಶಮಾಂಶ, ಅಂಕೆ_AXES, ಅಂಕೆ_ACTUATORS> &effectiveness)
{
	// highest priority first
	ಸ್ಥಿರ constexpr ControlAxis k_ಆದ್ಯತೆ[ಅಂಕೆ_AXES] = {THRUST_Z, ROLL, PITCH, THRUST_X, THRUST_Y, YAW};

	// Gram-Schmidt through the Cholesky factor of the Gram matrix of the accepted unit rows:
	// equivalent to orthogonalizing the rows, without keeping NUM_ACTUATORS-long vectors on the
	// (small work queue) stack
	ದಶಮಾಂಶ chol[ಅಂಕೆ_AXES][ಅಂಕೆ_AXES] {};
	ದಶಮಾಂಶ inv_norm[ಅಂಕೆ_AXES];
	ControlAxis accepted[ಅಂಕೆ_AXES];
	ಪೂರ್ಣಾಂಕ ಅಂಕೆ_accepted = 0;
	uint8_t dropped = 0;

	ಗಾಗಿ (ಸ್ಥಿರಾಂಕ ControlAxis axis : k_ಆದ್ಯತೆ) {
		ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ norm_squared = effectiveness.ಅಡ್ಡಸಾಲು(axis).norm_squared();

		ಆದರೆ (norm_squared < FLT_EPSILON) {
			ಮುಂದುವರಿ; // unused axis
		}

		ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ inv = 1.f / sqrtf(norm_squared);

		// projection of the unit row onto the orthonormal basis of the accepted rows
		ದಶಮಾಂಶ projection[ಅಂಕೆ_AXES];
		ದಶಮಾಂಶ projected_norm_squared = 0.f;

		ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < ಅಂಕೆ_accepted; i++) {
			ದಶಮಾಂಶ ಮೊತ್ತ = effectiveness.ಅಡ್ಡಸಾಲು(axis).dot(effectiveness.ಅಡ್ಡಸಾಲು(accepted[i])) * inv * inv_norm[i];

			ಗಾಗಿ (ಪೂರ್ಣಾಂಕ j = 0; j < i; j++) {
				ಮೊತ್ತ -= chol[i][j] * projection[j];
			}

			projection[i] = ಮೊತ್ತ / chol[i][i];
			projected_norm_squared += projection[i] * projection[i];
		}

		ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ independence = 1.f - projected_norm_squared;

		ಆದರೆ (independence < kMinAxisIndependence) {
			effectiveness.ಅಡ್ಡಸಾಲು(axis) = 0.f;
			dropped |= static_cast<uint8_t>(1u << axis);

		} ಇಲ್ಲದಿದ್ದರೆ {
			ಗಾಗಿ (ಪೂರ್ಣಾಂಕ j = 0; j < ಅಂಕೆ_accepted; j++) {
				chol[ಅಂಕೆ_accepted][j] = projection[j];
			}

			chol[ಅಂಕೆ_accepted][ಅಂಕೆ_accepted] = sqrtf(independence);
			inv_norm[ಅಂಕೆ_accepted] = inv;
			accepted[ಅಂಕೆ_accepted] = axis;
			ಅಂಕೆ_accepted++;
		}
	}

	ಹಿಂತಿರುಗಿ dropped;
}

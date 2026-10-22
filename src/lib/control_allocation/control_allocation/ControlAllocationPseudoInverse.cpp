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
ControlAllocationPseudoInverse::setEffectivenessMatrix(
	ಧ್ರುವ matrix::Matrix<ಭಿನ್ನ, ControlAllocation::ಅಂಕೆ_AXES, ControlAllocation::ಅಂಕೆ_ACTUATORS> &effectiveness,
	ಧ್ರುವ ActuatorVector &actuator_trim, ಧ್ರುವ ActuatorVector &linearization_point, ಪೂರ್ಣಾಙ್ಕ ಅಂಕೆ_actuators,
	ತಾರ್ಕಿಕ update_normalization_scale)
{
	ControlAllocation::setEffectivenessMatrix(effectiveness, actuator_trim, linearization_point, ಅಂಕೆ_actuators,
			update_normalization_scale);

	// in place on the stored copy: no second matrix on the (small work queue) stack
	_dropped_axes = dropDependentAxes(_effectiveness);

	_mix_update_needed = ಸತ್ಯ;
	_normalization_needs_update = update_normalization_scale;

	ಯದಿ (_metric_allocation && update_normalization_scale) {
		// adding #include <px4_platform_common/log.h> + PX4_WARN leads to failed linking on test
		_normalization_needs_update = ಅಸತ್ಯ;
	}
}

ಶೂನ್ಯ
ControlAllocationPseudoInverse::updatePseudoInverse()
{
	ಯದಿ (_mix_update_needed) {
		matrix::geninv(_effectiveness, _mix);

		ಯದಿ (!_metric_allocation) {
			ಯದಿ (_normalization_needs_update && !_had_actuator_failure) {
				updateControlAllocationMatrixScale();
				_normalization_needs_update = ಅಸತ್ಯ;
			}

			normalizeControlAllocationMatrix();
		}

		_mix_update_needed = ಅಸತ್ಯ;
	}
}

ಶೂನ್ಯ
ControlAllocationPseudoInverse::updateControlAllocationMatrixScale()
{
	// Same scale on roll and pitch
	ಯದಿ (_normalize_rpy) {

		ಪೂರ್ಣಾಙ್ಕ ಅಂಕೆ_non_zero_roll_torque = 0;
		ಪೂರ್ಣಾಙ್ಕ ಅಂಕೆ_non_zero_pitch_torque = 0;

		ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < ಅಂಕೆ_actuators_2; i++) {

			ಯದಿ (fabsf(_mix(i, 0)) > 1e-3f) {
				++ಅಂಕೆ_non_zero_roll_torque;
			}

			ಯದಿ (fabsf(_mix(i, 1)) > 1e-3f) {
				++ಅಂಕೆ_non_zero_pitch_torque;
			}
		}

		ಭಿನ್ನ roll_norm_scale = 1.f;

		ಯದಿ (ಅಂಕೆ_non_zero_roll_torque > 0) {
			roll_norm_scale = sqrtf(_mix.col(0).norm_squared() / (ಅಂಕೆ_non_zero_roll_torque / 2.f));
		}

		ಭಿನ್ನ pitch_norm_scale = 1.f;

		ಯದಿ (ಅಂಕೆ_non_zero_pitch_torque > 0) {
			pitch_norm_scale = sqrtf(_mix.col(1).norm_squared() / (ಅಂಕೆ_non_zero_pitch_torque / 2.f));
		}

		_control_allocation_scale(0) = fmaxf(roll_norm_scale, pitch_norm_scale);
		_control_allocation_scale(1) = _control_allocation_scale(0);

		// Scale yaw separately
		_control_allocation_scale(2) = _mix.col(2).max();

	} ಅನ್ಯಥಾ {
		_control_allocation_scale(0) = 1.f;
		_control_allocation_scale(1) = 1.f;
		_control_allocation_scale(2) = 1.f;
	}

	// Scale thrust by the sum of the individual thrust axes, and use the scaling for the Z axis if there's no actuators
	// (for tilted actuators)
	_control_allocation_scale(THRUST_Z) = 1.f;

	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ axis_idx = 2; axis_idx >= 0; --axis_idx) {
		ಪೂರ್ಣಾಙ್ಕ ಅಂಕೆ_non_zero_thrust = 0;
		ಭಿನ್ನ norm_sum = 0.f;

		ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < ಅಂಕೆ_actuators_2; i++) {
			ಭಿನ್ನ norm = fabsf(_mix(i, 3 + axis_idx));
			norm_sum += norm;

			ಯದಿ (norm > FLT_EPSILON) {
				++ಅಂಕೆ_non_zero_thrust;
			}
		}

		ಯದಿ (ಅಂಕೆ_non_zero_thrust > 0) {
			_control_allocation_scale(3 + axis_idx) = norm_sum / ಅಂಕೆ_non_zero_thrust;

		} ಅನ್ಯಥಾ {
			_control_allocation_scale(3 + axis_idx) = _control_allocation_scale(THRUST_Z);
		}
	}
}

ಶೂನ್ಯ
ControlAllocationPseudoInverse::normalizeControlAllocationMatrix()
{
	ಯದಿ (_control_allocation_scale(0) > FLT_EPSILON) {
		_mix.col(0) /= _control_allocation_scale(0);
		_mix.col(1) /= _control_allocation_scale(1);
	}

	ಯದಿ (_control_allocation_scale(2) > FLT_EPSILON) {
		_mix.col(2) /= _control_allocation_scale(2);
	}

	ಯದಿ (_control_allocation_scale(3) > FLT_EPSILON) {
		_mix.col(3) /= _control_allocation_scale(3);
		_mix.col(4) /= _control_allocation_scale(4);
		_mix.col(5) /= _control_allocation_scale(5);
	}

	// Set all the small elements to 0 to avoid issues
	// in the control allocation algorithms
	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < ಅಂಕೆ_actuators_2; i++) {
		ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ j = 0; j < ಅಂಕೆ_AXES; j++) {
			ಯದಿ (fabsf(_mix(i, j)) < 1e-3f) {
				_mix(i, j) = 0.f;
			}
		}
	}
}

ಶೂನ್ಯ
ControlAllocationPseudoInverse::allocate()
{
	//Compute new gains if needed
	updatePseudoInverse();

	_prev_actuator_sp = _actuator_sp;

	// Allocate
	_actuator_sp = _actuator_trim + _mix * (_control_sp - _control_trim);
}

uint8_t
ControlAllocationPseudoInverse::dropDependentAxes(matrix::Matrix<ಭಿನ್ನ, ಅಂಕೆ_AXES, ಅಂಕೆ_ACTUATORS> &effectiveness)
{
	// highest priority first
	ಸ್ಥಿರ constexpr ControlAxis kPriority[ಅಂಕೆ_AXES] = {THRUST_Z, ROLL, PITCH, THRUST_X, THRUST_Y, YAW};

	// Gram-Schmidt through the Cholesky factor of the Gram matrix of the accepted unit rows:
	// equivalent to orthogonalizing the rows, without keeping NUM_ACTUATORS-long vectors on the
	// (small work queue) stack
	ಭಿನ್ನ chol[ಅಂಕೆ_AXES][ಅಂಕೆ_AXES] {};
	ಭಿನ್ನ inv_norm[ಅಂಕೆ_AXES];
	ControlAxis accepted[ಅಂಕೆ_AXES];
	ಪೂರ್ಣಾಙ್ಕ ಅಂಕೆ_accepted = 0;
	uint8_t dropped = 0;

	ಪ್ರತಿ (ಧ್ರುವ ControlAxis axis : kPriority) {
		ಧ್ರುವ ಭಿನ್ನ norm_squared = effectiveness.row(axis).norm_squared();

		ಯದಿ (norm_squared < FLT_EPSILON) {
			ಅನುವರ್ತ; // unused axis
		}

		ಧ್ರುವ ಭಿನ್ನ inv = 1.f / sqrtf(norm_squared);

		// projection of the unit row onto the orthonormal basis of the accepted rows
		ಭಿನ್ನ projection[ಅಂಕೆ_AXES];
		ಭಿನ್ನ projected_norm_squared = 0.f;

		ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < ಅಂಕೆ_accepted; i++) {
			ಭಿನ್ನ sum = effectiveness.row(axis).dot(effectiveness.row(accepted[i])) * inv * inv_norm[i];

			ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ j = 0; j < i; j++) {
				sum -= chol[i][j] * projection[j];
			}

			projection[i] = sum / chol[i][i];
			projected_norm_squared += projection[i] * projection[i];
		}

		ಧ್ರುವ ಭಿನ್ನ independence = 1.f - projected_norm_squared;

		ಯದಿ (independence < kMinAxisIndependence) {
			effectiveness.row(axis) = 0.f;
			dropped |= static_cast<uint8_t>(1u << axis);

		} ಅನ್ಯಥಾ {
			ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ j = 0; j < ಅಂಕೆ_accepted; j++) {
				chol[ಅಂಕೆ_accepted][j] = projection[j];
			}

			chol[ಅಂಕೆ_accepted][ಅಂಕೆ_accepted] = sqrtf(independence);
			inv_norm[ಅಂಕೆ_accepted] = inv;
			accepted[ಅಂಕೆ_accepted] = axis;
			ಅಂಕೆ_accepted++;
		}
	}

	ಪ್ರತ್ಯಾಗಮ dropped;
}

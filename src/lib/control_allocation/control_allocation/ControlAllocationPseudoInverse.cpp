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

#அந்தர்பாவ "ControlAllocationPseudoInverse.hpp"

வெறுமை
ControlAllocationPseudoInverse::setEffectivenessMatrix(
	மாறிலி matrix::Matrix<தசம, ControlAllocation::இலக்கம்_AXES, ControlAllocation::இலக்கம்_ACTUATORS> &effectiveness,
	மாறிலி ActuatorVector &actuator_trim, மாறிலி ActuatorVector &linearization_point, முழு இலக்கம்_actuators,
	தருக்க update_normalization_scale)
{
	ControlAllocation::setEffectivenessMatrix(effectiveness, actuator_trim, linearization_point, இலக்கம்_actuators,
			update_normalization_scale);

	// in place on the stored copy: no second matrix on the (small work queue) stack
	_dropped_axes = dropDependentAxes(_effectiveness);

	_mix_update_needed = உண்மை;
	_normalization_needs_update = update_normalization_scale;

	எனில் (_metric_allocation && update_normalization_scale) {
		// adding #include <px4_platform_common/log.h> + PX4_WARN leads to failed linking on test
		_normalization_needs_update = பொய்;
	}
}

வெறுமை
ControlAllocationPseudoInverse::updatePseudoInverse()
{
	எனில் (_mix_update_needed) {
		matrix::geninv(_effectiveness, _mix);

		எனில் (!_metric_allocation) {
			எனில் (_normalization_needs_update && !_had_actuator_failure) {
				updateControlAllocationMatrixScale();
				_normalization_needs_update = பொய்;
			}

			normalizeControlAllocationMatrix();
		}

		_mix_update_needed = பொய்;
	}
}

வெறுமை
ControlAllocationPseudoInverse::updateControlAllocationMatrixScale()
{
	// Same scale on roll and pitch
	எனில் (_normalize_rpy) {

		முழு இலக்கம்_non_zero_roll_torque = 0;
		முழு இலக்கம்_non_zero_pitch_torque = 0;

		க்கு (முழு i = 0; i < இலக்கம்_actuators_2; i++) {

			எனில் (fabsf(_mix(i, 0)) > 1e-3f) {
				++இலக்கம்_non_zero_roll_torque;
			}

			எனில் (fabsf(_mix(i, 1)) > 1e-3f) {
				++இலக்கம்_non_zero_pitch_torque;
			}
		}

		தசம roll_norm_scale = 1.f;

		எனில் (இலக்கம்_non_zero_roll_torque > 0) {
			roll_norm_scale = sqrtf(_mix.col(0).norm_squared() / (இலக்கம்_non_zero_roll_torque / 2.f));
		}

		தசம pitch_norm_scale = 1.f;

		எனில் (இலக்கம்_non_zero_pitch_torque > 0) {
			pitch_norm_scale = sqrtf(_mix.col(1).norm_squared() / (இலக்கம்_non_zero_pitch_torque / 2.f));
		}

		_control_allocation_scale(0) = fmaxf(roll_norm_scale, pitch_norm_scale);
		_control_allocation_scale(1) = _control_allocation_scale(0);

		// Scale yaw separately
		_control_allocation_scale(2) = _mix.col(2).max();

	} இல்லையெனில் {
		_control_allocation_scale(0) = 1.f;
		_control_allocation_scale(1) = 1.f;
		_control_allocation_scale(2) = 1.f;
	}

	// Scale thrust by the sum of the individual thrust axes, and use the scaling for the Z axis if there's no actuators
	// (for tilted actuators)
	_control_allocation_scale(THRUST_Z) = 1.f;

	க்கு (முழு axis_idx = 2; axis_idx >= 0; --axis_idx) {
		முழு இலக்கம்_non_zero_thrust = 0;
		தசம norm_sum = 0.f;

		க்கு (முழு i = 0; i < இலக்கம்_actuators_2; i++) {
			தசம norm = fabsf(_mix(i, 3 + axis_idx));
			norm_sum += norm;

			எனில் (norm > FLT_EPSILON) {
				++இலக்கம்_non_zero_thrust;
			}
		}

		எனில் (இலக்கம்_non_zero_thrust > 0) {
			_control_allocation_scale(3 + axis_idx) = norm_sum / இலக்கம்_non_zero_thrust;

		} இல்லையெனில் {
			_control_allocation_scale(3 + axis_idx) = _control_allocation_scale(THRUST_Z);
		}
	}
}

வெறுமை
ControlAllocationPseudoInverse::normalizeControlAllocationMatrix()
{
	எனில் (_control_allocation_scale(0) > FLT_EPSILON) {
		_mix.col(0) /= _control_allocation_scale(0);
		_mix.col(1) /= _control_allocation_scale(1);
	}

	எனில் (_control_allocation_scale(2) > FLT_EPSILON) {
		_mix.col(2) /= _control_allocation_scale(2);
	}

	எனில் (_control_allocation_scale(3) > FLT_EPSILON) {
		_mix.col(3) /= _control_allocation_scale(3);
		_mix.col(4) /= _control_allocation_scale(4);
		_mix.col(5) /= _control_allocation_scale(5);
	}

	// Set all the small elements to 0 to avoid issues
	// in the control allocation algorithms
	க்கு (முழு i = 0; i < இலக்கம்_actuators_2; i++) {
		க்கு (முழு j = 0; j < இலக்கம்_AXES; j++) {
			எனில் (fabsf(_mix(i, j)) < 1e-3f) {
				_mix(i, j) = 0.f;
			}
		}
	}
}

வெறுமை
ControlAllocationPseudoInverse::allocate()
{
	//Compute new gains if needed
	updatePseudoInverse();

	_prev_actuator_sp = _actuator_sp;

	// Allocate
	_actuator_sp = _actuator_trim + _mix * (_control_sp - _control_trim);
}

uint8_t
ControlAllocationPseudoInverse::dropDependentAxes(matrix::Matrix<தசம, இலக்கம்_AXES, இலக்கம்_ACTUATORS> &effectiveness)
{
	// highest priority first
	நிலையான constexpr ControlAxis kPriority[இலக்கம்_AXES] = {THRUST_Z, ROLL, PITCH, THRUST_X, THRUST_Y, YAW};

	// Gram-Schmidt through the Cholesky factor of the Gram matrix of the accepted unit rows:
	// equivalent to orthogonalizing the rows, without keeping NUM_ACTUATORS-long vectors on the
	// (small work queue) stack
	தசம chol[இலக்கம்_AXES][இலக்கம்_AXES] {};
	தசம inv_norm[இலக்கம்_AXES];
	ControlAxis accepted[இலக்கம்_AXES];
	முழு இலக்கம்_accepted = 0;
	uint8_t dropped = 0;

	க்கு (மாறிலி ControlAxis axis : kPriority) {
		மாறிலி தசம norm_squared = effectiveness.row(axis).norm_squared();

		எனில் (norm_squared < FLT_EPSILON) {
			தொடர்; // unused axis
		}

		மாறிலி தசம inv = 1.f / sqrtf(norm_squared);

		// projection of the unit row onto the orthonormal basis of the accepted rows
		தசம projection[இலக்கம்_AXES];
		தசம projected_norm_squared = 0.f;

		க்கு (முழு i = 0; i < இலக்கம்_accepted; i++) {
			தசம sum = effectiveness.row(axis).dot(effectiveness.row(accepted[i])) * inv * inv_norm[i];

			க்கு (முழு j = 0; j < i; j++) {
				sum -= chol[i][j] * projection[j];
			}

			projection[i] = sum / chol[i][i];
			projected_norm_squared += projection[i] * projection[i];
		}

		மாறிலி தசம independence = 1.f - projected_norm_squared;

		எனில் (independence < kMinAxisIndependence) {
			effectiveness.row(axis) = 0.f;
			dropped |= static_cast<uint8_t>(1u << axis);

		} இல்லையெனில் {
			க்கு (முழு j = 0; j < இலக்கம்_accepted; j++) {
				chol[இலக்கம்_accepted][j] = projection[j];
			}

			chol[இலக்கம்_accepted][இலக்கம்_accepted] = sqrtf(independence);
			inv_norm[இலக்கம்_accepted] = inv;
			accepted[இலக்கம்_accepted] = axis;
			இலக்கம்_accepted++;
		}
	}

	திரும்பு dropped;
}

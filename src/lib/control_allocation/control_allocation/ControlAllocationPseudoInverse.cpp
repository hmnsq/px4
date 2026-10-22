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

#अन्तर्भाव "ControlAllocationPseudoInverse.hpp"

शून्य
ControlAllocationPseudoInverse::setEffectivenessMatrix(
	ध्रुव matrix::Matrix<भिन्न, ControlAllocation::अङ्कः_AXES, ControlAllocation::अङ्कः_ACTUATORS> &effectiveness,
	ध्रुव ActuatorVector &actuator_trim, ध्रुव ActuatorVector &linearization_point, पूर्णाङ्क अङ्कः_actuators,
	तार्किक update_normalization_scale)
{
	ControlAllocation::setEffectivenessMatrix(effectiveness, actuator_trim, linearization_point, अङ्कः_actuators,
			update_normalization_scale);

	// in place on the stored copy: no second matrix on the (small work queue) stack
	_dropped_axes = dropDependentAxes(_effectiveness);

	_mix_update_needed = सच्च;
	_normalization_needs_update = update_normalization_scale;

	सचे (_metric_allocation && update_normalization_scale) {
		// adding #include <px4_platform_common/log.h> + PX4_WARN leads to failed linking on test
		_normalization_needs_update = मिच्छा;
	}
}

शून्य
ControlAllocationPseudoInverse::updatePseudoInverse()
{
	सचे (_mix_update_needed) {
		matrix::geninv(_effectiveness, _mix);

		सचे (!_metric_allocation) {
			सचे (_normalization_needs_update && !_had_actuator_failure) {
				updateControlAllocationMatrixScale();
				_normalization_needs_update = मिच्छा;
			}

			normalizeControlAllocationMatrix();
		}

		_mix_update_needed = मिच्छा;
	}
}

शून्य
ControlAllocationPseudoInverse::updateControlAllocationMatrixScale()
{
	// Same scale on roll and pitch
	सचे (_normalize_rpy) {

		पूर्णाङ्क अङ्कः_non_zero_roll_torque = 0;
		पूर्णाङ्क अङ्कः_non_zero_pitch_torque = 0;

		अत्थाय (पूर्णाङ्क i = 0; i < अङ्कः_actuators_2; i++) {

			सचे (fabsf(_mix(i, 0)) > 1e-3f) {
				++अङ्कः_non_zero_roll_torque;
			}

			सचे (fabsf(_mix(i, 1)) > 1e-3f) {
				++अङ्कः_non_zero_pitch_torque;
			}
		}

		भिन्न roll_norm_scale = 1.f;

		सचे (अङ्कः_non_zero_roll_torque > 0) {
			roll_norm_scale = sqrtf(_mix.col(0).norm_squared() / (अङ्कः_non_zero_roll_torque / 2.f));
		}

		भिन्न pitch_norm_scale = 1.f;

		सचे (अङ्कः_non_zero_pitch_torque > 0) {
			pitch_norm_scale = sqrtf(_mix.col(1).norm_squared() / (अङ्कः_non_zero_pitch_torque / 2.f));
		}

		_control_allocation_scale(0) = fmaxf(roll_norm_scale, pitch_norm_scale);
		_control_allocation_scale(1) = _control_allocation_scale(0);

		// Scale yaw separately
		_control_allocation_scale(2) = _mix.col(2).max();

	} अञ्ञथा {
		_control_allocation_scale(0) = 1.f;
		_control_allocation_scale(1) = 1.f;
		_control_allocation_scale(2) = 1.f;
	}

	// Scale thrust by the sum of the individual thrust axes, and use the scaling for the Z axis if there's no actuators
	// (for tilted actuators)
	_control_allocation_scale(THRUST_Z) = 1.f;

	अत्थाय (पूर्णाङ्क axis_idx = 2; axis_idx >= 0; --axis_idx) {
		पूर्णाङ्क अङ्कः_non_zero_thrust = 0;
		भिन्न norm_sum = 0.f;

		अत्थाय (पूर्णाङ्क i = 0; i < अङ्कः_actuators_2; i++) {
			भिन्न norm = fabsf(_mix(i, 3 + axis_idx));
			norm_sum += norm;

			सचे (norm > FLT_EPSILON) {
				++अङ्कः_non_zero_thrust;
			}
		}

		सचे (अङ्कः_non_zero_thrust > 0) {
			_control_allocation_scale(3 + axis_idx) = norm_sum / अङ्कः_non_zero_thrust;

		} अञ्ञथा {
			_control_allocation_scale(3 + axis_idx) = _control_allocation_scale(THRUST_Z);
		}
	}
}

शून्य
ControlAllocationPseudoInverse::normalizeControlAllocationMatrix()
{
	सचे (_control_allocation_scale(0) > FLT_EPSILON) {
		_mix.col(0) /= _control_allocation_scale(0);
		_mix.col(1) /= _control_allocation_scale(1);
	}

	सचे (_control_allocation_scale(2) > FLT_EPSILON) {
		_mix.col(2) /= _control_allocation_scale(2);
	}

	सचे (_control_allocation_scale(3) > FLT_EPSILON) {
		_mix.col(3) /= _control_allocation_scale(3);
		_mix.col(4) /= _control_allocation_scale(4);
		_mix.col(5) /= _control_allocation_scale(5);
	}

	// Set all the small elements to 0 to avoid issues
	// in the control allocation algorithms
	अत्थाय (पूर्णाङ्क i = 0; i < अङ्कः_actuators_2; i++) {
		अत्थाय (पूर्णाङ्क j = 0; j < अङ्कः_AXES; j++) {
			सचे (fabsf(_mix(i, j)) < 1e-3f) {
				_mix(i, j) = 0.f;
			}
		}
	}
}

शून्य
ControlAllocationPseudoInverse::allocate()
{
	//Compute new gains if needed
	updatePseudoInverse();

	_prev_actuator_sp = _actuator_sp;

	// Allocate
	_actuator_sp = _actuator_trim + _mix * (_control_sp - _control_trim);
}

uint8_t
ControlAllocationPseudoInverse::dropDependentAxes(matrix::Matrix<भिन्न, अङ्कः_AXES, अङ्कः_ACTUATORS> &effectiveness)
{
	// highest priority first
	स्थिर constexpr ControlAxis kPriority[अङ्कः_AXES] = {THRUST_Z, ROLL, PITCH, THRUST_X, THRUST_Y, YAW};

	// Gram-Schmidt through the Cholesky factor of the Gram matrix of the accepted unit rows:
	// equivalent to orthogonalizing the rows, without keeping NUM_ACTUATORS-long vectors on the
	// (small work queue) stack
	भिन्न chol[अङ्कः_AXES][अङ्कः_AXES] {};
	भिन्न inv_norm[अङ्कः_AXES];
	ControlAxis accepted[अङ्कः_AXES];
	पूर्णाङ्क अङ्कः_accepted = 0;
	uint8_t dropped = 0;

	अत्थाय (ध्रुव ControlAxis axis : kPriority) {
		ध्रुव भिन्न norm_squared = effectiveness.row(axis).norm_squared();

		सचे (norm_squared < FLT_EPSILON) {
			अनुवर्त; // unused axis
		}

		ध्रुव भिन्न inv = 1.f / sqrtf(norm_squared);

		// projection of the unit row onto the orthonormal basis of the accepted rows
		भिन्न projection[अङ्कः_AXES];
		भिन्न projected_norm_squared = 0.f;

		अत्थाय (पूर्णाङ्क i = 0; i < अङ्कः_accepted; i++) {
			भिन्न sum = effectiveness.row(axis).dot(effectiveness.row(accepted[i])) * inv * inv_norm[i];

			अत्थाय (पूर्णाङ्क j = 0; j < i; j++) {
				sum -= chol[i][j] * projection[j];
			}

			projection[i] = sum / chol[i][i];
			projected_norm_squared += projection[i] * projection[i];
		}

		ध्रुव भिन्न independence = 1.f - projected_norm_squared;

		सचे (independence < kMinAxisIndependence) {
			effectiveness.row(axis) = 0.f;
			dropped |= static_cast<uint8_t>(1u << axis);

		} अञ्ञथा {
			अत्थाय (पूर्णाङ्क j = 0; j < अङ्कः_accepted; j++) {
				chol[अङ्कः_accepted][j] = projection[j];
			}

			chol[अङ्कः_accepted][अङ्कः_accepted] = sqrtf(independence);
			inv_norm[अङ्कः_accepted] = inv;
			accepted[अङ्कः_accepted] = axis;
			अङ्कः_accepted++;
		}
	}

	पटिनिवत्त dropped;
}

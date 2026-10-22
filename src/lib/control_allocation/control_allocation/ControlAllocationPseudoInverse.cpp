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

#ଅନ୍ତର୍ଭାଵ "ControlAllocationPseudoInverse.hpp"

ଶୂନ୍ୟ
ControlAllocationPseudoInverse::setEffectivenessMatrix(
	ଅଚଳ matrix::Matrix<ଦଶମିକ, ControlAllocation::ଅଙ୍କ_AXES, ControlAllocation::ଅଙ୍କ_ACTUATORS> &effectiveness,
	ଅଚଳ ActuatorVector &actuator_trim, ଅଚଳ ActuatorVector &linearization_point, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଅଙ୍କ_actuators,
	ତାର୍କିକ update_normalization_scale)
{
	ControlAllocation::setEffectivenessMatrix(effectiveness, actuator_trim, linearization_point, ଅଙ୍କ_actuators,
			update_normalization_scale);

	// in place on the stored copy: no second matrix on the (small work queue) stack
	_dropped_axes = dropDependentAxes(_effectiveness);

	_mix_update_needed = ସତ୍ୟ;
	_normalization_needs_update = update_normalization_scale;

	ଯଦି (_metric_allocation && update_normalization_scale) {
		// adding #include <px4_platform_common/log.h> + PX4_WARN leads to failed linking on test
		_normalization_needs_update = ମିଥ୍ୟା;
	}
}

ଶୂନ୍ୟ
ControlAllocationPseudoInverse::updatePseudoInverse()
{
	ଯଦି (_mix_update_needed) {
		matrix::geninv(_effectiveness, _mix);

		ଯଦି (!_metric_allocation) {
			ଯଦି (_normalization_needs_update && !_had_actuator_failure) {
				updateControlAllocationMatrixScale();
				_normalization_needs_update = ମିଥ୍ୟା;
			}

			normalizeControlAllocationMatrix();
		}

		_mix_update_needed = ମିଥ୍ୟା;
	}
}

ଶୂନ୍ୟ
ControlAllocationPseudoInverse::updateControlAllocationMatrixScale()
{
	// Same scale on roll and pitch
	ଯଦି (_normalize_rpy) {

		ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଅଙ୍କ_non_zero_roll_torque = 0;
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଅଙ୍କ_non_zero_pitch_torque = 0;

		ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < ଅଙ୍କ_actuators_2; i++) {

			ଯଦି (fabsf(_mix(i, 0)) > 1e-3f) {
				++ଅଙ୍କ_non_zero_roll_torque;
			}

			ଯଦି (fabsf(_mix(i, 1)) > 1e-3f) {
				++ଅଙ୍କ_non_zero_pitch_torque;
			}
		}

		ଦଶମିକ roll_norm_scale = 1.f;

		ଯଦି (ଅଙ୍କ_non_zero_roll_torque > 0) {
			roll_norm_scale = sqrtf(_mix.col(0).norm_squared() / (ଅଙ୍କ_non_zero_roll_torque / 2.f));
		}

		ଦଶମିକ pitch_norm_scale = 1.f;

		ଯଦି (ଅଙ୍କ_non_zero_pitch_torque > 0) {
			pitch_norm_scale = sqrtf(_mix.col(1).norm_squared() / (ଅଙ୍କ_non_zero_pitch_torque / 2.f));
		}

		_control_allocation_scale(0) = fmaxf(roll_norm_scale, pitch_norm_scale);
		_control_allocation_scale(1) = _control_allocation_scale(0);

		// Scale yaw separately
		_control_allocation_scale(2) = _mix.col(2).max();

	} ଅନ୍ୟଥା {
		_control_allocation_scale(0) = 1.f;
		_control_allocation_scale(1) = 1.f;
		_control_allocation_scale(2) = 1.f;
	}

	// Scale thrust by the sum of the individual thrust axes, and use the scaling for the Z axis if there's no actuators
	// (for tilted actuators)
	_control_allocation_scale(THRUST_Z) = 1.f;

	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା axis_idx = 2; axis_idx >= 0; --axis_idx) {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଅଙ୍କ_non_zero_thrust = 0;
		ଦଶମିକ norm_sum = 0.f;

		ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < ଅଙ୍କ_actuators_2; i++) {
			ଦଶମିକ norm = fabsf(_mix(i, 3 + axis_idx));
			norm_sum += norm;

			ଯଦି (norm > FLT_EPSILON) {
				++ଅଙ୍କ_non_zero_thrust;
			}
		}

		ଯଦି (ଅଙ୍କ_non_zero_thrust > 0) {
			_control_allocation_scale(3 + axis_idx) = norm_sum / ଅଙ୍କ_non_zero_thrust;

		} ଅନ୍ୟଥା {
			_control_allocation_scale(3 + axis_idx) = _control_allocation_scale(THRUST_Z);
		}
	}
}

ଶୂନ୍ୟ
ControlAllocationPseudoInverse::normalizeControlAllocationMatrix()
{
	ଯଦି (_control_allocation_scale(0) > FLT_EPSILON) {
		_mix.col(0) /= _control_allocation_scale(0);
		_mix.col(1) /= _control_allocation_scale(1);
	}

	ଯଦି (_control_allocation_scale(2) > FLT_EPSILON) {
		_mix.col(2) /= _control_allocation_scale(2);
	}

	ଯଦି (_control_allocation_scale(3) > FLT_EPSILON) {
		_mix.col(3) /= _control_allocation_scale(3);
		_mix.col(4) /= _control_allocation_scale(4);
		_mix.col(5) /= _control_allocation_scale(5);
	}

	// Set all the small elements to 0 to avoid issues
	// in the control allocation algorithms
	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < ଅଙ୍କ_actuators_2; i++) {
		ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା j = 0; j < ଅଙ୍କ_AXES; j++) {
			ଯଦି (fabsf(_mix(i, j)) < 1e-3f) {
				_mix(i, j) = 0.f;
			}
		}
	}
}

ଶୂନ୍ୟ
ControlAllocationPseudoInverse::allocate()
{
	//Compute new gains if needed
	updatePseudoInverse();

	_prev_actuator_sp = _actuator_sp;

	// Allocate
	_actuator_sp = _actuator_trim + _mix * (_control_sp - _control_trim);
}

uint8_t
ControlAllocationPseudoInverse::dropDependentAxes(matrix::Matrix<ଦଶମିକ, ଅଙ୍କ_AXES, ଅଙ୍କ_ACTUATORS> &effectiveness)
{
	// highest priority first
	ସ୍ଥିର constexpr ControlAxis kPriority[ଅଙ୍କ_AXES] = {THRUST_Z, ROLL, PITCH, THRUST_X, THRUST_Y, YAW};

	// Gram-Schmidt through the Cholesky factor of the Gram matrix of the accepted unit rows:
	// equivalent to orthogonalizing the rows, without keeping NUM_ACTUATORS-long vectors on the
	// (small work queue) stack
	ଦଶମିକ chol[ଅଙ୍କ_AXES][ଅଙ୍କ_AXES] {};
	ଦଶମିକ inv_norm[ଅଙ୍କ_AXES];
	ControlAxis accepted[ଅଙ୍କ_AXES];
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଅଙ୍କ_accepted = 0;
	uint8_t dropped = 0;

	ପାଇଁ (ଅଚଳ ControlAxis axis : kPriority) {
		ଅଚଳ ଦଶମିକ norm_squared = effectiveness.row(axis).norm_squared();

		ଯଦି (norm_squared < FLT_EPSILON) {
			ଜାରି; // unused axis
		}

		ଅଚଳ ଦଶମିକ inv = 1.f / sqrtf(norm_squared);

		// projection of the unit row onto the orthonormal basis of the accepted rows
		ଦଶମିକ projection[ଅଙ୍କ_AXES];
		ଦଶମିକ projected_norm_squared = 0.f;

		ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < ଅଙ୍କ_accepted; i++) {
			ଦଶମିକ sum = effectiveness.row(axis).dot(effectiveness.row(accepted[i])) * inv * inv_norm[i];

			ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା j = 0; j < i; j++) {
				sum -= chol[i][j] * projection[j];
			}

			projection[i] = sum / chol[i][i];
			projected_norm_squared += projection[i] * projection[i];
		}

		ଅଚଳ ଦଶମିକ independence = 1.f - projected_norm_squared;

		ଯଦି (independence < kMinAxisIndependence) {
			effectiveness.row(axis) = 0.f;
			dropped |= static_cast<uint8_t>(1u << axis);

		} ଅନ୍ୟଥା {
			ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା j = 0; j < ଅଙ୍କ_accepted; j++) {
				chol[ଅଙ୍କ_accepted][j] = projection[j];
			}

			chol[ଅଙ୍କ_accepted][ଅଙ୍କ_accepted] = sqrtf(independence);
			inv_norm[ଅଙ୍କ_accepted] = inv;
			accepted[ଅଙ୍କ_accepted] = axis;
			ଅଙ୍କ_accepted++;
		}
	}

	ଫେରନ୍ତୁ dropped;
}

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

#অন্তর্ভাৱ "ControlAllocationPseudoInverse.hpp"

শূন্য
ControlAllocationPseudoInverse::setEffectivenessMatrix(
	ধ্রুৱ matrix::Matrix<ভিন্ন, ControlAllocation::অঙ্ক_AXES, ControlAllocation::অঙ্ক_ACTUATORS> &effectiveness,
	ধ্রুৱ ActuatorVector &actuator_trim, ধ্রুৱ ActuatorVector &linearization_point, পূর্ণাঙ্ক অঙ্ক_actuators,
	তার্কিক update_normalization_scale)
{
	ControlAllocation::setEffectivenessMatrix(effectiveness, actuator_trim, linearization_point, অঙ্ক_actuators,
			update_normalization_scale);

	// in place on the stored copy: no second matrix on the (small work queue) stack
	_dropped_axes = dropDependentAxes(_effectiveness);

	_mix_update_needed = সঁচা;
	_normalization_needs_update = update_normalization_scale;

	যদি (_metric_allocation && update_normalization_scale) {
		// adding #include <px4_platform_common/log.h> + PX4_WARN leads to failed linking on test
		_normalization_needs_update = মিছা;
	}
}

শূন্য
ControlAllocationPseudoInverse::updatePseudoInverse()
{
	যদি (_mix_update_needed) {
		matrix::geninv(_effectiveness, _mix);

		যদি (!_metric_allocation) {
			যদি (_normalization_needs_update && !_had_actuator_failure) {
				updateControlAllocationMatrixScale();
				_normalization_needs_update = মিছা;
			}

			normalizeControlAllocationMatrix();
		}

		_mix_update_needed = মিছা;
	}
}

শূন্য
ControlAllocationPseudoInverse::updateControlAllocationMatrixScale()
{
	// Same scale on roll and pitch
	যদি (_normalize_rpy) {

		পূর্ণাঙ্ক অঙ্ক_non_zero_roll_torque = 0;
		পূর্ণাঙ্ক অঙ্ক_non_zero_pitch_torque = 0;

		বাবে (পূর্ণাঙ্ক i = 0; i < অঙ্ক_actuators_2; i++) {

			যদি (fabsf(_mix(i, 0)) > 1e-3f) {
				++অঙ্ক_non_zero_roll_torque;
			}

			যদি (fabsf(_mix(i, 1)) > 1e-3f) {
				++অঙ্ক_non_zero_pitch_torque;
			}
		}

		ভিন্ন roll_norm_scale = 1.f;

		যদি (অঙ্ক_non_zero_roll_torque > 0) {
			roll_norm_scale = sqrtf(_mix.col(0).norm_squared() / (অঙ্ক_non_zero_roll_torque / 2.f));
		}

		ভিন্ন pitch_norm_scale = 1.f;

		যদি (অঙ্ক_non_zero_pitch_torque > 0) {
			pitch_norm_scale = sqrtf(_mix.col(1).norm_squared() / (অঙ্ক_non_zero_pitch_torque / 2.f));
		}

		_control_allocation_scale(0) = fmaxf(roll_norm_scale, pitch_norm_scale);
		_control_allocation_scale(1) = _control_allocation_scale(0);

		// Scale yaw separately
		_control_allocation_scale(2) = _mix.col(2).max();

	} অন্যথা {
		_control_allocation_scale(0) = 1.f;
		_control_allocation_scale(1) = 1.f;
		_control_allocation_scale(2) = 1.f;
	}

	// Scale thrust by the sum of the individual thrust axes, and use the scaling for the Z axis if there's no actuators
	// (for tilted actuators)
	_control_allocation_scale(THRUST_Z) = 1.f;

	বাবে (পূর্ণাঙ্ক axis_idx = 2; axis_idx >= 0; --axis_idx) {
		পূর্ণাঙ্ক অঙ্ক_non_zero_thrust = 0;
		ভিন্ন norm_sum = 0.f;

		বাবে (পূর্ণাঙ্ক i = 0; i < অঙ্ক_actuators_2; i++) {
			ভিন্ন norm = fabsf(_mix(i, 3 + axis_idx));
			norm_sum += norm;

			যদি (norm > FLT_EPSILON) {
				++অঙ্ক_non_zero_thrust;
			}
		}

		যদি (অঙ্ক_non_zero_thrust > 0) {
			_control_allocation_scale(3 + axis_idx) = norm_sum / অঙ্ক_non_zero_thrust;

		} অন্যথা {
			_control_allocation_scale(3 + axis_idx) = _control_allocation_scale(THRUST_Z);
		}
	}
}

শূন্য
ControlAllocationPseudoInverse::normalizeControlAllocationMatrix()
{
	যদি (_control_allocation_scale(0) > FLT_EPSILON) {
		_mix.col(0) /= _control_allocation_scale(0);
		_mix.col(1) /= _control_allocation_scale(1);
	}

	যদি (_control_allocation_scale(2) > FLT_EPSILON) {
		_mix.col(2) /= _control_allocation_scale(2);
	}

	যদি (_control_allocation_scale(3) > FLT_EPSILON) {
		_mix.col(3) /= _control_allocation_scale(3);
		_mix.col(4) /= _control_allocation_scale(4);
		_mix.col(5) /= _control_allocation_scale(5);
	}

	// Set all the small elements to 0 to avoid issues
	// in the control allocation algorithms
	বাবে (পূর্ণাঙ্ক i = 0; i < অঙ্ক_actuators_2; i++) {
		বাবে (পূর্ণাঙ্ক j = 0; j < অঙ্ক_AXES; j++) {
			যদি (fabsf(_mix(i, j)) < 1e-3f) {
				_mix(i, j) = 0.f;
			}
		}
	}
}

শূন্য
ControlAllocationPseudoInverse::allocate()
{
	//Compute new gains if needed
	updatePseudoInverse();

	_prev_actuator_sp = _actuator_sp;

	// Allocate
	_actuator_sp = _actuator_trim + _mix * (_control_sp - _control_trim);
}

uint8_t
ControlAllocationPseudoInverse::dropDependentAxes(matrix::Matrix<ভিন্ন, অঙ্ক_AXES, অঙ্ক_ACTUATORS> &effectiveness)
{
	// highest priority first
	স্থির constexpr ControlAxis kPriority[অঙ্ক_AXES] = {THRUST_Z, ROLL, PITCH, THRUST_X, THRUST_Y, YAW};

	// Gram-Schmidt through the Cholesky factor of the Gram matrix of the accepted unit rows:
	// equivalent to orthogonalizing the rows, without keeping NUM_ACTUATORS-long vectors on the
	// (small work queue) stack
	ভিন্ন chol[অঙ্ক_AXES][অঙ্ক_AXES] {};
	ভিন্ন inv_norm[অঙ্ক_AXES];
	ControlAxis accepted[অঙ্ক_AXES];
	পূর্ণাঙ্ক অঙ্ক_accepted = 0;
	uint8_t dropped = 0;

	বাবে (ধ্রুৱ ControlAxis axis : kPriority) {
		ধ্রুৱ ভিন্ন norm_squared = effectiveness.row(axis).norm_squared();

		যদি (norm_squared < FLT_EPSILON) {
			অনুৱর্ত; // unused axis
		}

		ধ্রুৱ ভিন্ন inv = 1.f / sqrtf(norm_squared);

		// projection of the unit row onto the orthonormal basis of the accepted rows
		ভিন্ন projection[অঙ্ক_AXES];
		ভিন্ন projected_norm_squared = 0.f;

		বাবে (পূর্ণাঙ্ক i = 0; i < অঙ্ক_accepted; i++) {
			ভিন্ন sum = effectiveness.row(axis).dot(effectiveness.row(accepted[i])) * inv * inv_norm[i];

			বাবে (পূর্ণাঙ্ক j = 0; j < i; j++) {
				sum -= chol[i][j] * projection[j];
			}

			projection[i] = sum / chol[i][i];
			projected_norm_squared += projection[i] * projection[i];
		}

		ধ্রুৱ ভিন্ন independence = 1.f - projected_norm_squared;

		যদি (independence < kMinAxisIndependence) {
			effectiveness.row(axis) = 0.f;
			dropped |= static_cast<uint8_t>(1u << axis);

		} অন্যথা {
			বাবে (পূর্ণাঙ্ক j = 0; j < অঙ্ক_accepted; j++) {
				chol[অঙ্ক_accepted][j] = projection[j];
			}

			chol[অঙ্ক_accepted][অঙ্ক_accepted] = sqrtf(independence);
			inv_norm[অঙ্ক_accepted] = inv;
			accepted[অঙ্ক_accepted] = axis;
			অঙ্ক_accepted++;
		}
	}

	ঘূৰাই dropped;
}

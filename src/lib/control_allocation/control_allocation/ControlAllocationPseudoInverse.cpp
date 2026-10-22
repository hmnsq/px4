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

#અન્તર્ભાવ "ControlAllocationPseudoInverse.hpp"

શૂન્ય
ControlAllocationPseudoInverse::setEffectivenessMatrix(
	અચલ matrix::Matrix<દશાંશ, ControlAllocation::અંક_AXES, ControlAllocation::અંક_ACTUATORS> &effectiveness,
	અચલ ActuatorVector &actuator_trim, અચલ ActuatorVector &linearization_point, પૂર્ણાંક અંક_actuators,
	તાર્કિક update_normalization_scale)
{
	ControlAllocation::setEffectivenessMatrix(effectiveness, actuator_trim, linearization_point, અંક_actuators,
			update_normalization_scale);

	// in place on the stored copy: no second matrix on the (small work queue) stack
	_dropped_axes = dropDependentAxes(_effectiveness);

	_mix_update_needed = સાચું;
	_normalization_needs_update = update_normalization_scale;

	જો (_metric_allocation && update_normalization_scale) {
		// adding #include <px4_platform_common/log.h> + PX4_WARN leads to failed linking on test
		_normalization_needs_update = ખોટું;
	}
}

શૂન્ય
ControlAllocationPseudoInverse::updatePseudoInverse()
{
	જો (_mix_update_needed) {
		matrix::geninv(_effectiveness, _mix);

		જો (!_metric_allocation) {
			જો (_normalization_needs_update && !_had_actuator_failure) {
				updateControlAllocationMatrixScale();
				_normalization_needs_update = ખોટું;
			}

			normalizeControlAllocationMatrix();
		}

		_mix_update_needed = ખોટું;
	}
}

શૂન્ય
ControlAllocationPseudoInverse::updateControlAllocationMatrixScale()
{
	// Same scale on roll and pitch
	જો (_normalize_rpy) {

		પૂર્ણાંક અંક_non_zero_roll_torque = 0;
		પૂર્ણાંક અંક_non_zero_pitch_torque = 0;

		માટે (પૂર્ણાંક i = 0; i < અંક_actuators_2; i++) {

			જો (fabsf(_mix(i, 0)) > 1e-3f) {
				++અંક_non_zero_roll_torque;
			}

			જો (fabsf(_mix(i, 1)) > 1e-3f) {
				++અંક_non_zero_pitch_torque;
			}
		}

		દશાંશ roll_norm_scale = 1.f;

		જો (અંક_non_zero_roll_torque > 0) {
			roll_norm_scale = sqrtf(_mix.col(0).norm_squared() / (અંક_non_zero_roll_torque / 2.f));
		}

		દશાંશ pitch_norm_scale = 1.f;

		જો (અંક_non_zero_pitch_torque > 0) {
			pitch_norm_scale = sqrtf(_mix.col(1).norm_squared() / (અંક_non_zero_pitch_torque / 2.f));
		}

		_control_allocation_scale(0) = fmaxf(roll_norm_scale, pitch_norm_scale);
		_control_allocation_scale(1) = _control_allocation_scale(0);

		// Scale yaw separately
		_control_allocation_scale(2) = _mix.col(2).max();

	} નહીંતર {
		_control_allocation_scale(0) = 1.f;
		_control_allocation_scale(1) = 1.f;
		_control_allocation_scale(2) = 1.f;
	}

	// Scale thrust by the sum of the individual thrust axes, and use the scaling for the Z axis if there's no actuators
	// (for tilted actuators)
	_control_allocation_scale(THRUST_Z) = 1.f;

	માટે (પૂર્ણાંક axis_idx = 2; axis_idx >= 0; --axis_idx) {
		પૂર્ણાંક અંક_non_zero_thrust = 0;
		દશાંશ norm_sum = 0.f;

		માટે (પૂર્ણાંક i = 0; i < અંક_actuators_2; i++) {
			દશાંશ norm = fabsf(_mix(i, 3 + axis_idx));
			norm_sum += norm;

			જો (norm > FLT_EPSILON) {
				++અંક_non_zero_thrust;
			}
		}

		જો (અંક_non_zero_thrust > 0) {
			_control_allocation_scale(3 + axis_idx) = norm_sum / અંક_non_zero_thrust;

		} નહીંતર {
			_control_allocation_scale(3 + axis_idx) = _control_allocation_scale(THRUST_Z);
		}
	}
}

શૂન્ય
ControlAllocationPseudoInverse::normalizeControlAllocationMatrix()
{
	જો (_control_allocation_scale(0) > FLT_EPSILON) {
		_mix.col(0) /= _control_allocation_scale(0);
		_mix.col(1) /= _control_allocation_scale(1);
	}

	જો (_control_allocation_scale(2) > FLT_EPSILON) {
		_mix.col(2) /= _control_allocation_scale(2);
	}

	જો (_control_allocation_scale(3) > FLT_EPSILON) {
		_mix.col(3) /= _control_allocation_scale(3);
		_mix.col(4) /= _control_allocation_scale(4);
		_mix.col(5) /= _control_allocation_scale(5);
	}

	// Set all the small elements to 0 to avoid issues
	// in the control allocation algorithms
	માટે (પૂર્ણાંક i = 0; i < અંક_actuators_2; i++) {
		માટે (પૂર્ણાંક j = 0; j < અંક_AXES; j++) {
			જો (fabsf(_mix(i, j)) < 1e-3f) {
				_mix(i, j) = 0.f;
			}
		}
	}
}

શૂન્ય
ControlAllocationPseudoInverse::allocate()
{
	//Compute new gains if needed
	updatePseudoInverse();

	_prev_actuator_sp = _actuator_sp;

	// Allocate
	_actuator_sp = _actuator_trim + _mix * (_control_sp - _control_trim);
}

uint8_t
ControlAllocationPseudoInverse::dropDependentAxes(matrix::Matrix<દશાંશ, અંક_AXES, અંક_ACTUATORS> &effectiveness)
{
	// highest priority first
	સ્થિર constexpr ControlAxis kPriority[અંક_AXES] = {THRUST_Z, ROLL, PITCH, THRUST_X, THRUST_Y, YAW};

	// Gram-Schmidt through the Cholesky factor of the Gram matrix of the accepted unit rows:
	// equivalent to orthogonalizing the rows, without keeping NUM_ACTUATORS-long vectors on the
	// (small work queue) stack
	દશાંશ chol[અંક_AXES][અંક_AXES] {};
	દશાંશ inv_norm[અંક_AXES];
	ControlAxis accepted[અંક_AXES];
	પૂર્ણાંક અંક_accepted = 0;
	uint8_t dropped = 0;

	માટે (અચલ ControlAxis axis : kPriority) {
		અચલ દશાંશ norm_squared = effectiveness.row(axis).norm_squared();

		જો (norm_squared < FLT_EPSILON) {
			ચાલુ; // unused axis
		}

		અચલ દશાંશ inv = 1.f / sqrtf(norm_squared);

		// projection of the unit row onto the orthonormal basis of the accepted rows
		દશાંશ projection[અંક_AXES];
		દશાંશ projected_norm_squared = 0.f;

		માટે (પૂર્ણાંક i = 0; i < અંક_accepted; i++) {
			દશાંશ sum = effectiveness.row(axis).dot(effectiveness.row(accepted[i])) * inv * inv_norm[i];

			માટે (પૂર્ણાંક j = 0; j < i; j++) {
				sum -= chol[i][j] * projection[j];
			}

			projection[i] = sum / chol[i][i];
			projected_norm_squared += projection[i] * projection[i];
		}

		અચલ દશાંશ independence = 1.f - projected_norm_squared;

		જો (independence < kMinAxisIndependence) {
			effectiveness.row(axis) = 0.f;
			dropped |= static_cast<uint8_t>(1u << axis);

		} નહીંતર {
			માટે (પૂર્ણાંક j = 0; j < અંક_accepted; j++) {
				chol[અંક_accepted][j] = projection[j];
			}

			chol[અંક_accepted][અંક_accepted] = sqrtf(independence);
			inv_norm[અંક_accepted] = inv;
			accepted[અંક_accepted] = axis;
			અંક_accepted++;
		}
	}

	પાછા dropped;
}

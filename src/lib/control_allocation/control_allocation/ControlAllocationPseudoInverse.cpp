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
	स्थिरांक matrix::Matrix<दशांश, ControlAllocation::अंक_AXES, ControlAllocation::अंक_ACTUATORS> &effectiveness,
	स्थिरांक ActuatorVector &actuator_trim, स्थिरांक ActuatorVector &linearization_point, पूर्णांक अंक_actuators,
	तार्किक update_normalization_scale)
{
	ControlAllocation::setEffectivenessMatrix(effectiveness, actuator_trim, linearization_point, अंक_actuators,
			update_normalization_scale);

	// in place on the stored copy: no second matrix on the (small work queue) stack
	_dropped_axes = dropDependentAxes(_effectiveness);

	_mix_update_needed = सत्य;
	_normalization_needs_update = update_normalization_scale;

	जर (_metric_allocation && update_normalization_scale) {
		// adding #include <px4_platform_common/log.h> + PX4_WARN leads to failed linking on test
		_normalization_needs_update = असत्य;
	}
}

शून्य
ControlAllocationPseudoInverse::updatePseudoInverse()
{
	जर (_mix_update_needed) {
		matrix::geninv(_effectiveness, _mix);

		जर (!_metric_allocation) {
			जर (_normalization_needs_update && !_had_actuator_failure) {
				updateControlAllocationMatrixScale();
				_normalization_needs_update = असत्य;
			}

			normalizeControlAllocationMatrix();
		}

		_mix_update_needed = असत्य;
	}
}

शून्य
ControlAllocationPseudoInverse::updateControlAllocationMatrixScale()
{
	// Same scale on roll and pitch
	जर (_normalize_rpy) {

		पूर्णांक अंक_non_zero_roll_torque = 0;
		पूर्णांक अंक_non_zero_pitch_torque = 0;

		साठी (पूर्णांक i = 0; i < अंक_actuators_2; i++) {

			जर (fabsf(_mix(i, 0)) > 1e-3f) {
				++अंक_non_zero_roll_torque;
			}

			जर (fabsf(_mix(i, 1)) > 1e-3f) {
				++अंक_non_zero_pitch_torque;
			}
		}

		दशांश roll_norm_scale = 1.f;

		जर (अंक_non_zero_roll_torque > 0) {
			roll_norm_scale = sqrtf(_mix.col(0).norm_squared() / (अंक_non_zero_roll_torque / 2.f));
		}

		दशांश pitch_norm_scale = 1.f;

		जर (अंक_non_zero_pitch_torque > 0) {
			pitch_norm_scale = sqrtf(_mix.col(1).norm_squared() / (अंक_non_zero_pitch_torque / 2.f));
		}

		_control_allocation_scale(0) = fmaxf(roll_norm_scale, pitch_norm_scale);
		_control_allocation_scale(1) = _control_allocation_scale(0);

		// Scale yaw separately
		_control_allocation_scale(2) = _mix.col(2).max();

	} अन्यथा {
		_control_allocation_scale(0) = 1.f;
		_control_allocation_scale(1) = 1.f;
		_control_allocation_scale(2) = 1.f;
	}

	// Scale thrust by the sum of the individual thrust axes, and use the scaling for the Z axis if there's no actuators
	// (for tilted actuators)
	_control_allocation_scale(THRUST_Z) = 1.f;

	साठी (पूर्णांक axis_idx = 2; axis_idx >= 0; --axis_idx) {
		पूर्णांक अंक_non_zero_thrust = 0;
		दशांश norm_sum = 0.f;

		साठी (पूर्णांक i = 0; i < अंक_actuators_2; i++) {
			दशांश norm = fabsf(_mix(i, 3 + axis_idx));
			norm_sum += norm;

			जर (norm > भग्न_अंतर) {
				++अंक_non_zero_thrust;
			}
		}

		जर (अंक_non_zero_thrust > 0) {
			_control_allocation_scale(3 + axis_idx) = norm_sum / अंक_non_zero_thrust;

		} अन्यथा {
			_control_allocation_scale(3 + axis_idx) = _control_allocation_scale(THRUST_Z);
		}
	}
}

शून्य
ControlAllocationPseudoInverse::normalizeControlAllocationMatrix()
{
	जर (_control_allocation_scale(0) > भग्न_अंतर) {
		_mix.col(0) /= _control_allocation_scale(0);
		_mix.col(1) /= _control_allocation_scale(1);
	}

	जर (_control_allocation_scale(2) > भग्न_अंतर) {
		_mix.col(2) /= _control_allocation_scale(2);
	}

	जर (_control_allocation_scale(3) > भग्न_अंतर) {
		_mix.col(3) /= _control_allocation_scale(3);
		_mix.col(4) /= _control_allocation_scale(4);
		_mix.col(5) /= _control_allocation_scale(5);
	}

	// Set all the small elements to 0 to avoid issues
	// in the control allocation algorithms
	साठी (पूर्णांक i = 0; i < अंक_actuators_2; i++) {
		साठी (पूर्णांक j = 0; j < अंक_AXES; j++) {
			जर (fabsf(_mix(i, j)) < 1e-3f) {
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
ControlAllocationPseudoInverse::dropDependentAxes(matrix::Matrix<दशांश, अंक_AXES, अंक_ACTUATORS> &effectiveness)
{
	// highest priority first
	स्थिर constexpr ControlAxis kPriority[अंक_AXES] = {THRUST_Z, ROLL, PITCH, THRUST_X, THRUST_Y, YAW};

	// Gram-Schmidt through the Cholesky factor of the Gram matrix of the accepted unit rows:
	// equivalent to orthogonalizing the rows, without keeping NUM_ACTUATORS-long vectors on the
	// (small work queue) stack
	दशांश chol[अंक_AXES][अंक_AXES] {};
	दशांश inv_norm[अंक_AXES];
	ControlAxis accepted[अंक_AXES];
	पूर्णांक अंक_accepted = 0;
	uint8_t dropped = 0;

	साठी (स्थिरांक ControlAxis axis : kPriority) {
		स्थिरांक दशांश norm_squared = effectiveness.row(axis).norm_squared();

		जर (norm_squared < भग्न_अंतर) {
			सुरूठेवा; // unused axis
		}

		स्थिरांक दशांश inv = 1.f / sqrtf(norm_squared);

		// projection of the unit row onto the orthonormal basis of the accepted rows
		दशांश projection[अंक_AXES];
		दशांश projected_norm_squared = 0.f;

		साठी (पूर्णांक i = 0; i < अंक_accepted; i++) {
			दशांश sum = effectiveness.row(axis).dot(effectiveness.row(accepted[i])) * inv * inv_norm[i];

			साठी (पूर्णांक j = 0; j < i; j++) {
				sum -= chol[i][j] * projection[j];
			}

			projection[i] = sum / chol[i][i];
			projected_norm_squared += projection[i] * projection[i];
		}

		स्थिरांक दशांश independence = 1.f - projected_norm_squared;

		जर (independence < kMinAxisIndependence) {
			effectiveness.row(axis) = 0.f;
			dropped |= static_cast<uint8_t>(1u << axis);

		} अन्यथा {
			साठी (पूर्णांक j = 0; j < अंक_accepted; j++) {
				chol[अंक_accepted][j] = projection[j];
			}

			chol[अंक_accepted][अंक_accepted] = sqrtf(independence);
			inv_norm[अंक_accepted] = inv;
			accepted[अंक_accepted] = axis;
			अंक_accepted++;
		}
	}

	परत dropped;
}

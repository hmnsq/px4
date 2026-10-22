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
Control_आवण्टनम्_Pseudo_Inverse::समुच्चयः_Effectiveness_Matrix(
	ध्रुव matrix::Matrix<भिन्न, Control_आवण्टनम्::अङ्कः_AXES, Control_आवण्टनम्::अङ्कः_ACTUATORS> &effectiveness,
	ध्रुव Actuator_सदिशम् &actuator_trim, ध्रुव Actuator_सदिशम् &linearization_point, पूर्णाङ्क अङ्कः_actuators,
	तार्किक update_normalization_scale)
{
	Control_आवण्टनम्::समुच्चयः_Effectiveness_Matrix(effectiveness, actuator_trim, linearization_point, अङ्कः_actuators,
			update_normalization_scale);

	// in place on the stored copy: no second matrix on the (small work queue) stack
	_dropped_axes = dropDependentAxes(_effectiveness);

	_mix_update_needed = सत्य;
	_normalization_needs_update = update_normalization_scale;

	यदि (metric_आवण्टनम् && update_normalization_scale) {
		// adding #include <px4_platform_common/log.h> + PX4_WARN leads to failed linking on test
		_normalization_needs_update = असत्य;
	}
}

शून्य
Control_आवण्टनम्_Pseudo_Inverse::updatePseudoInverse()
{
	यदि (_mix_update_needed) {
		matrix::geninv(_effectiveness, _mix);

		यदि (!metric_आवण्टनम्) {
			यदि (_normalization_needs_update && !_had_actuator_failure) {
				update_Control_आवण्टनम्_Matrix_Scale();
				_normalization_needs_update = असत्य;
			}

			normalize_Control_आवण्टनम्_Matrix();
		}

		_mix_update_needed = असत्य;
	}
}

शून्य
Control_आवण्टनम्_Pseudo_Inverse::update_Control_आवण्टनम्_Matrix_Scale()
{
	// Same scale on roll and pitch
	यदि (_normalize_rpy) {

		पूर्णाङ्क अङ्कः_non_zero_roll_torque = 0;
		पूर्णाङ्क अङ्कः_non_zero_pitch_torque = 0;

		प्रति (पूर्णाङ्क i = 0; i < अङ्कः_actuators_2; i++) {

			यदि (fabsf(_mix(i, 0)) > 1e-3f) {
				++अङ्कः_non_zero_roll_torque;
			}

			यदि (fabsf(_mix(i, 1)) > 1e-3f) {
				++अङ्कः_non_zero_pitch_torque;
			}
		}

		भिन्न roll_norm_scale = 1.f;

		यदि (अङ्कः_non_zero_roll_torque > 0) {
			roll_norm_scale = sqrtf(_mix.col(0).norm_squared() / (अङ्कः_non_zero_roll_torque / 2.f));
		}

		भिन्न pitch_norm_scale = 1.f;

		यदि (अङ्कः_non_zero_pitch_torque > 0) {
			pitch_norm_scale = sqrtf(_mix.col(1).norm_squared() / (अङ्कः_non_zero_pitch_torque / 2.f));
		}

		control_आवण्टनम्_scale(0) = fmaxf(roll_norm_scale, pitch_norm_scale);
		control_आवण्टनम्_scale(1) = control_आवण्टनम्_scale(0);

		// Scale yaw separately
		control_आवण्टनम्_scale(2) = _mix.col(2).max();

	} अन्यथा {
		control_आवण्टनम्_scale(0) = 1.f;
		control_आवण्टनम्_scale(1) = 1.f;
		control_आवण्टनम्_scale(2) = 1.f;
	}

	// Scale thrust by the sum of the individual thrust axes, and use the scaling for the Z axis if there's no actuators
	// (for tilted actuators)
	control_आवण्टनम्_scale(THRUST_Z) = 1.f;

	प्रति (पूर्णाङ्क axis_idx = 2; axis_idx >= 0; --axis_idx) {
		पूर्णाङ्क अङ्कः_non_zero_thrust = 0;
		भिन्न norm_योग = 0.f;

		प्रति (पूर्णाङ्क i = 0; i < अङ्कः_actuators_2; i++) {
			भिन्न norm = fabsf(_mix(i, 3 + axis_idx));
			norm_योग += norm;

			यदि (norm > FLT_EPSILON) {
				++अङ्कः_non_zero_thrust;
			}
		}

		यदि (अङ्कः_non_zero_thrust > 0) {
			control_आवण्टनम्_scale(3 + axis_idx) = norm_योग / अङ्कः_non_zero_thrust;

		} अन्यथा {
			control_आवण्टनम्_scale(3 + axis_idx) = control_आवण्टनम्_scale(THRUST_Z);
		}
	}
}

शून्य
Control_आवण्टनम्_Pseudo_Inverse::normalize_Control_आवण्टनम्_Matrix()
{
	यदि (control_आवण्टनम्_scale(0) > FLT_EPSILON) {
		_mix.col(0) /= control_आवण्टनम्_scale(0);
		_mix.col(1) /= control_आवण्टनम्_scale(1);
	}

	यदि (control_आवण्टनम्_scale(2) > FLT_EPSILON) {
		_mix.col(2) /= control_आवण्टनम्_scale(2);
	}

	यदि (control_आवण्टनम्_scale(3) > FLT_EPSILON) {
		_mix.col(3) /= control_आवण्टनम्_scale(3);
		_mix.col(4) /= control_आवण्टनम्_scale(4);
		_mix.col(5) /= control_आवण्टनम्_scale(5);
	}

	// Set all the small elements to 0 to avoid issues
	// in the control allocation algorithms
	प्रति (पूर्णाङ्क i = 0; i < अङ्कः_actuators_2; i++) {
		प्रति (पूर्णाङ्क j = 0; j < अङ्कः_AXES; j++) {
			यदि (fabsf(_mix(i, j)) < 1e-3f) {
				_mix(i, j) = 0.f;
			}
		}
	}
}

शून्य
Control_आवण्टनम्_Pseudo_Inverse::allocate()
{
	//Compute new gains if needed
	updatePseudoInverse();

	पूर्व_actuator_sp = _actuator_sp;

	// Allocate
	_actuator_sp = _actuator_trim + _mix * (_control_sp - _control_trim);
}

uint8_t
Control_आवण्टनम्_Pseudo_Inverse::dropDependentAxes(matrix::Matrix<भिन्न, अङ्कः_AXES, अङ्कः_ACTUATORS> &effectiveness)
{
	// highest priority first
	स्थिर constexpr ControlAxis k_प्राथमिकता[अङ्कः_AXES] = {THRUST_Z, ROLL, PITCH, THRUST_X, THRUST_Y, YAW};

	// Gram-Schmidt through the Cholesky factor of the Gram matrix of the accepted unit rows:
	// equivalent to orthogonalizing the rows, without keeping NUM_ACTUATORS-long vectors on the
	// (small work queue) stack
	भिन्न chol[अङ्कः_AXES][अङ्कः_AXES] {};
	भिन्न inv_norm[अङ्कः_AXES];
	ControlAxis accepted[अङ्कः_AXES];
	पूर्णाङ्क अङ्कः_accepted = 0;
	uint8_t dropped = 0;

	प्रति (ध्रुव ControlAxis axis : k_प्राथमिकता) {
		ध्रुव भिन्न norm_squared = effectiveness.row(axis).norm_squared();

		यदि (norm_squared < FLT_EPSILON) {
			अनुवर्त; // unused axis
		}

		ध्रुव भिन्न inv = 1.f / sqrtf(norm_squared);

		// projection of the unit row onto the orthonormal basis of the accepted rows
		भिन्न projection[अङ्कः_AXES];
		भिन्न projected_norm_squared = 0.f;

		प्रति (पूर्णाङ्क i = 0; i < अङ्कः_accepted; i++) {
			भिन्न योग = effectiveness.row(axis).dot(effectiveness.row(accepted[i])) * inv * inv_norm[i];

			प्रति (पूर्णाङ्क j = 0; j < i; j++) {
				योग -= chol[i][j] * projection[j];
			}

			projection[i] = योग / chol[i][i];
			projected_norm_squared += projection[i] * projection[i];
		}

		ध्रुव भिन्न independence = 1.f - projected_norm_squared;

		यदि (independence < kMinAxisIndependence) {
			effectiveness.row(axis) = 0.f;
			dropped |= static_cast<uint8_t>(1u << axis);

		} अन्यथा {
			प्रति (पूर्णाङ्क j = 0; j < अङ्कः_accepted; j++) {
				chol[अङ्कः_accepted][j] = projection[j];
			}

			chol[अङ्कः_accepted][अङ्कः_accepted] = sqrtf(independence);
			inv_norm[अङ्कः_accepted] = inv;
			accepted[अङ्कः_accepted] = axis;
			अङ्कः_accepted++;
		}
	}

	प्रत्यागम dropped;
}

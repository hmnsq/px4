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

#അന്തര്ഭാവ "ControlAllocationPseudoInverse.hpp"

ശൂന്യം
ControlAllocationPseudoInverse::setEffectivenessMatrix(
	സ്ഥിരം matrix::Matrix<ദശാംശം, ControlAllocation::അക്കം_AXES, ControlAllocation::അക്കം_ACTUATORS> &effectiveness,
	സ്ഥിരം ActuatorVector &actuator_trim, സ്ഥിരം ActuatorVector &linearization_point, പൂർണ്ണസംഖ്യ അക്കം_actuators,
	തർക്കം update_normalization_scale)
{
	ControlAllocation::setEffectivenessMatrix(effectiveness, actuator_trim, linearization_point, അക്കം_actuators,
			update_normalization_scale);

	// in place on the stored copy: no second matrix on the (small work queue) stack
	_dropped_axes = dropDependentAxes(_effectiveness);

	_mix_update_needed = സത്യം;
	_normalization_needs_update = update_normalization_scale;

	എങ്കിൽ (_metric_allocation && update_normalization_scale) {
		// adding #include <px4_platform_common/log.h> + PX4_WARN leads to failed linking on test
		_normalization_needs_update = അസത്യം;
	}
}

ശൂന്യം
ControlAllocationPseudoInverse::updatePseudoInverse()
{
	എങ്കിൽ (_mix_update_needed) {
		matrix::geninv(_effectiveness, _mix);

		എങ്കിൽ (!_metric_allocation) {
			എങ്കിൽ (_normalization_needs_update && !_had_actuator_failure) {
				updateControlAllocationMatrixScale();
				_normalization_needs_update = അസത്യം;
			}

			normalizeControlAllocationMatrix();
		}

		_mix_update_needed = അസത്യം;
	}
}

ശൂന്യം
ControlAllocationPseudoInverse::updateControlAllocationMatrixScale()
{
	// Same scale on roll and pitch
	എങ്കിൽ (_normalize_rpy) {

		പൂർണ്ണസംഖ്യ അക്കം_non_zero_roll_torque = 0;
		പൂർണ്ണസംഖ്യ അക്കം_non_zero_pitch_torque = 0;

		വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < അക്കം_actuators_2; i++) {

			എങ്കിൽ (fabsf(_mix(i, 0)) > 1e-3f) {
				++അക്കം_non_zero_roll_torque;
			}

			എങ്കിൽ (fabsf(_mix(i, 1)) > 1e-3f) {
				++അക്കം_non_zero_pitch_torque;
			}
		}

		ദശാംശം roll_norm_scale = 1.f;

		എങ്കിൽ (അക്കം_non_zero_roll_torque > 0) {
			roll_norm_scale = sqrtf(_mix.col(0).norm_squared() / (അക്കം_non_zero_roll_torque / 2.f));
		}

		ദശാംശം pitch_norm_scale = 1.f;

		എങ്കിൽ (അക്കം_non_zero_pitch_torque > 0) {
			pitch_norm_scale = sqrtf(_mix.col(1).norm_squared() / (അക്കം_non_zero_pitch_torque / 2.f));
		}

		_control_allocation_scale(0) = fmaxf(roll_norm_scale, pitch_norm_scale);
		_control_allocation_scale(1) = _control_allocation_scale(0);

		// Scale yaw separately
		_control_allocation_scale(2) = _mix.col(2).max();

	} അല്ലെങ്കിൽ {
		_control_allocation_scale(0) = 1.f;
		_control_allocation_scale(1) = 1.f;
		_control_allocation_scale(2) = 1.f;
	}

	// Scale thrust by the sum of the individual thrust axes, and use the scaling for the Z axis if there's no actuators
	// (for tilted actuators)
	_control_allocation_scale(THRUST_Z) = 1.f;

	വേണ്ടി (പൂർണ്ണസംഖ്യ axis_idx = 2; axis_idx >= 0; --axis_idx) {
		പൂർണ്ണസംഖ്യ അക്കം_non_zero_thrust = 0;
		ദശാംശം norm_sum = 0.f;

		വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < അക്കം_actuators_2; i++) {
			ദശാംശം norm = fabsf(_mix(i, 3 + axis_idx));
			norm_sum += norm;

			എങ്കിൽ (norm > FLT_EPSILON) {
				++അക്കം_non_zero_thrust;
			}
		}

		എങ്കിൽ (അക്കം_non_zero_thrust > 0) {
			_control_allocation_scale(3 + axis_idx) = norm_sum / അക്കം_non_zero_thrust;

		} അല്ലെങ്കിൽ {
			_control_allocation_scale(3 + axis_idx) = _control_allocation_scale(THRUST_Z);
		}
	}
}

ശൂന്യം
ControlAllocationPseudoInverse::normalizeControlAllocationMatrix()
{
	എങ്കിൽ (_control_allocation_scale(0) > FLT_EPSILON) {
		_mix.col(0) /= _control_allocation_scale(0);
		_mix.col(1) /= _control_allocation_scale(1);
	}

	എങ്കിൽ (_control_allocation_scale(2) > FLT_EPSILON) {
		_mix.col(2) /= _control_allocation_scale(2);
	}

	എങ്കിൽ (_control_allocation_scale(3) > FLT_EPSILON) {
		_mix.col(3) /= _control_allocation_scale(3);
		_mix.col(4) /= _control_allocation_scale(4);
		_mix.col(5) /= _control_allocation_scale(5);
	}

	// Set all the small elements to 0 to avoid issues
	// in the control allocation algorithms
	വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < അക്കം_actuators_2; i++) {
		വേണ്ടി (പൂർണ്ണസംഖ്യ j = 0; j < അക്കം_AXES; j++) {
			എങ്കിൽ (fabsf(_mix(i, j)) < 1e-3f) {
				_mix(i, j) = 0.f;
			}
		}
	}
}

ശൂന്യം
ControlAllocationPseudoInverse::allocate()
{
	//Compute new gains if needed
	updatePseudoInverse();

	_prev_actuator_sp = _actuator_sp;

	// Allocate
	_actuator_sp = _actuator_trim + _mix * (_control_sp - _control_trim);
}

uint8_t
ControlAllocationPseudoInverse::dropDependentAxes(matrix::Matrix<ദശാംശം, അക്കം_AXES, അക്കം_ACTUATORS> &effectiveness)
{
	// highest priority first
	സ്ഥിതം constexpr ControlAxis kPriority[അക്കം_AXES] = {THRUST_Z, ROLL, PITCH, THRUST_X, THRUST_Y, YAW};

	// Gram-Schmidt through the Cholesky factor of the Gram matrix of the accepted unit rows:
	// equivalent to orthogonalizing the rows, without keeping NUM_ACTUATORS-long vectors on the
	// (small work queue) stack
	ദശാംശം chol[അക്കം_AXES][അക്കം_AXES] {};
	ദശാംശം inv_norm[അക്കം_AXES];
	ControlAxis accepted[അക്കം_AXES];
	പൂർണ്ണസംഖ്യ അക്കം_accepted = 0;
	uint8_t dropped = 0;

	വേണ്ടി (സ്ഥിരം ControlAxis axis : kPriority) {
		സ്ഥിരം ദശാംശം norm_squared = effectiveness.row(axis).norm_squared();

		എങ്കിൽ (norm_squared < FLT_EPSILON) {
			തുടരുക; // unused axis
		}

		സ്ഥിരം ദശാംശം inv = 1.f / sqrtf(norm_squared);

		// projection of the unit row onto the orthonormal basis of the accepted rows
		ദശാംശം projection[അക്കം_AXES];
		ദശാംശം projected_norm_squared = 0.f;

		വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < അക്കം_accepted; i++) {
			ദശാംശം sum = effectiveness.row(axis).dot(effectiveness.row(accepted[i])) * inv * inv_norm[i];

			വേണ്ടി (പൂർണ്ണസംഖ്യ j = 0; j < i; j++) {
				sum -= chol[i][j] * projection[j];
			}

			projection[i] = sum / chol[i][i];
			projected_norm_squared += projection[i] * projection[i];
		}

		സ്ഥിരം ദശാംശം independence = 1.f - projected_norm_squared;

		എങ്കിൽ (independence < kMinAxisIndependence) {
			effectiveness.row(axis) = 0.f;
			dropped |= static_cast<uint8_t>(1u << axis);

		} അല്ലെങ്കിൽ {
			വേണ്ടി (പൂർണ്ണസംഖ്യ j = 0; j < അക്കം_accepted; j++) {
				chol[അക്കം_accepted][j] = projection[j];
			}

			chol[അക്കം_accepted][അക്കം_accepted] = sqrtf(independence);
			inv_norm[അക്കം_accepted] = inv;
			accepted[അക്കം_accepted] = axis;
			അക്കം_accepted++;
		}
	}

	തിരികെ dropped;
}

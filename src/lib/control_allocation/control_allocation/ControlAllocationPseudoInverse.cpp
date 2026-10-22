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

#అన్తర్భావ "ControlAllocationPseudoInverse.hpp"

శూన్య
Control_కేటాయింపు_Pseudo_Inverse::సమితి_Effectiveness_మాతృక(
	స్థిరరాశి మాతృక::మాతృక_2<దశాంశం, Control_కేటాయింపు::అంకె_AXES, Control_కేటాయింపు::అంకె_ACTUATORS> &effectiveness,
	స్థిరరాశి Actuator_సదిశ &actuator_trim, స్థిరరాశి Actuator_సదిశ &linearization_బిందువు, పూర్ణాంకం అంకె_actuators,
	తార్కిక update_normalization_scale)
{
	Control_కేటాయింపు::సమితి_Effectiveness_మాతృక(effectiveness, actuator_trim, linearization_బిందువు, అంకె_actuators,
			update_normalization_scale);

	// in place on the stored copy: no second matrix on the (small work queue) stack
	_dropped_axes = dropDependentAxes(_effectiveness);

	_mix_update_needed = నిజం;
	_normalization_needs_update = update_normalization_scale;

	ఒకవేళ (metric_కేటాయింపు && update_normalization_scale) {
		// adding #include <px4_platform_common/log.h> + PX4_WARN leads to failed linking on test
		_normalization_needs_update = అబద్ధం;
	}
}

శూన్య
Control_కేటాయింపు_Pseudo_Inverse::updatePseudoInverse()
{
	ఒకవేళ (_mix_update_needed) {
		మాతృక::geninv(_effectiveness, _mix);

		ఒకవేళ (!metric_కేటాయింపు) {
			ఒకవేళ (_normalization_needs_update && !_had_actuator_failure) {
				update_Control_కేటాయింపు_మాతృక_Scale();
				_normalization_needs_update = అబద్ధం;
			}

			normalize_Control_కేటాయింపు_మాతృక();
		}

		_mix_update_needed = అబద్ధం;
	}
}

శూన్య
Control_కేటాయింపు_Pseudo_Inverse::update_Control_కేటాయింపు_మాతృక_Scale()
{
	// Same scale on roll and pitch
	ఒకవేళ (_normalize_rpy) {

		పూర్ణాంకం అంకె_non_zero_roll_torque = 0;
		పూర్ణాంకం అంకె_non_zero_pitch_torque = 0;

		కొరకు (పూర్ణాంకం i = 0; i < అంకె_actuators_2; i++) {

			ఒకవేళ (fabsf(_mix(i, 0)) > 1e-3f) {
				++అంకె_non_zero_roll_torque;
			}

			ఒకవేళ (fabsf(_mix(i, 1)) > 1e-3f) {
				++అంకె_non_zero_pitch_torque;
			}
		}

		దశాంశం roll_norm_scale = 1.f;

		ఒకవేళ (అంకె_non_zero_roll_torque > 0) {
			roll_norm_scale = sqrtf(_mix.నిలువు(0).norm_squared() / (అంకె_non_zero_roll_torque / 2.f));
		}

		దశాంశం pitch_norm_scale = 1.f;

		ఒకవేళ (అంకె_non_zero_pitch_torque > 0) {
			pitch_norm_scale = sqrtf(_mix.నిలువు(1).norm_squared() / (అంకె_non_zero_pitch_torque / 2.f));
		}

		control_కేటాయింపు_scale(0) = fmaxf(roll_norm_scale, pitch_norm_scale);
		control_కేటాయింపు_scale(1) = control_కేటాయింపు_scale(0);

		// Scale yaw separately
		control_కేటాయింపు_scale(2) = _mix.నిలువు(2).max();

	} లేకపోతే {
		control_కేటాయింపు_scale(0) = 1.f;
		control_కేటాయింపు_scale(1) = 1.f;
		control_కేటాయింపు_scale(2) = 1.f;
	}

	// Scale thrust by the sum of the individual thrust axes, and use the scaling for the Z axis if there's no actuators
	// (for tilted actuators)
	control_కేటాయింపు_scale(THRUST_Z) = 1.f;

	కొరకు (పూర్ణాంకం axis_idx = 2; axis_idx >= 0; --axis_idx) {
		పూర్ణాంకం అంకె_non_zero_thrust = 0;
		దశాంశం norm_కూడిక = 0.f;

		కొరకు (పూర్ణాంకం i = 0; i < అంకె_actuators_2; i++) {
			దశాంశం norm = fabsf(_mix(i, 3 + axis_idx));
			norm_కూడిక += norm;

			ఒకవేళ (norm > FLT_EPSILON) {
				++అంకె_non_zero_thrust;
			}
		}

		ఒకవేళ (అంకె_non_zero_thrust > 0) {
			control_కేటాయింపు_scale(3 + axis_idx) = norm_కూడిక / అంకె_non_zero_thrust;

		} లేకపోతే {
			control_కేటాయింపు_scale(3 + axis_idx) = control_కేటాయింపు_scale(THRUST_Z);
		}
	}
}

శూన్య
Control_కేటాయింపు_Pseudo_Inverse::normalize_Control_కేటాయింపు_మాతృక()
{
	ఒకవేళ (control_కేటాయింపు_scale(0) > FLT_EPSILON) {
		_mix.నిలువు(0) /= control_కేటాయింపు_scale(0);
		_mix.నిలువు(1) /= control_కేటాయింపు_scale(1);
	}

	ఒకవేళ (control_కేటాయింపు_scale(2) > FLT_EPSILON) {
		_mix.నిలువు(2) /= control_కేటాయింపు_scale(2);
	}

	ఒకవేళ (control_కేటాయింపు_scale(3) > FLT_EPSILON) {
		_mix.నిలువు(3) /= control_కేటాయింపు_scale(3);
		_mix.నిలువు(4) /= control_కేటాయింపు_scale(4);
		_mix.నిలువు(5) /= control_కేటాయింపు_scale(5);
	}

	// Set all the small elements to 0 to avoid issues
	// in the control allocation algorithms
	కొరకు (పూర్ణాంకం i = 0; i < అంకె_actuators_2; i++) {
		కొరకు (పూర్ణాంకం j = 0; j < అంకె_AXES; j++) {
			ఒకవేళ (fabsf(_mix(i, j)) < 1e-3f) {
				_mix(i, j) = 0.f;
			}
		}
	}
}

శూన్య
Control_కేటాయింపు_Pseudo_Inverse::allocate()
{
	//Compute new gains if needed
	updatePseudoInverse();

	_prev_actuator_sp = _actuator_sp;

	// Allocate
	_actuator_sp = _actuator_trim + _mix * (_control_sp - _control_trim);
}

uint8_t
Control_కేటాయింపు_Pseudo_Inverse::dropDependentAxes(మాతృక::మాతృక_2<దశాంశం, అంకె_AXES, అంకె_ACTUATORS> &effectiveness)
{
	// highest priority first
	స్థిర constexpr ControlAxis k_ప్రాధాన్యత[అంకె_AXES] = {THRUST_Z, ROLL, PITCH, THRUST_X, THRUST_Y, YAW};

	// Gram-Schmidt through the Cholesky factor of the Gram matrix of the accepted unit rows:
	// equivalent to orthogonalizing the rows, without keeping NUM_ACTUATORS-long vectors on the
	// (small work queue) stack
	దశాంశం chol[అంకె_AXES][అంకె_AXES] {};
	దశాంశం inv_norm[అంకె_AXES];
	ControlAxis accepted[అంకె_AXES];
	పూర్ణాంకం అంకె_accepted = 0;
	uint8_t dropped = 0;

	కొరకు (స్థిరరాశి ControlAxis axis : k_ప్రాధాన్యత) {
		స్థిరరాశి దశాంశం norm_squared = effectiveness.అడ్డువరుస(axis).norm_squared();

		ఒకవేళ (norm_squared < FLT_EPSILON) {
			కొనసాగు; // unused axis
		}

		స్థిరరాశి దశాంశం inv = 1.f / sqrtf(norm_squared);

		// projection of the unit row onto the orthonormal basis of the accepted rows
		దశాంశం projection[అంకె_AXES];
		దశాంశం projected_norm_squared = 0.f;

		కొరకు (పూర్ణాంకం i = 0; i < అంకె_accepted; i++) {
			దశాంశం కూడిక = effectiveness.అడ్డువరుస(axis).dot(effectiveness.అడ్డువరుస(accepted[i])) * inv * inv_norm[i];

			కొరకు (పూర్ణాంకం j = 0; j < i; j++) {
				కూడిక -= chol[i][j] * projection[j];
			}

			projection[i] = కూడిక / chol[i][i];
			projected_norm_squared += projection[i] * projection[i];
		}

		స్థిరరాశి దశాంశం independence = 1.f - projected_norm_squared;

		ఒకవేళ (independence < kMinAxisIndependence) {
			effectiveness.అడ్డువరుస(axis) = 0.f;
			dropped |= static_cast<uint8_t>(1u << axis);

		} లేకపోతే {
			కొరకు (పూర్ణాంకం j = 0; j < అంకె_accepted; j++) {
				chol[అంకె_accepted][j] = projection[j];
			}

			chol[అంకె_accepted][అంకె_accepted] = sqrtf(independence);
			inv_norm[అంకె_accepted] = inv;
			accepted[అంకె_accepted] = axis;
			అంకె_accepted++;
		}
	}

	తిరిగి dropped;
}

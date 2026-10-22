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

#شامل "ControlAllocationPseudoInverse.hpp"

خالی
Control_تخصیص_Pseudo_Inverse::سیٹ_Effectiveness_میٹرکس(
	ثابت میٹرکس::میٹرکس_2<اعشاری, Control_تخصیص::عدد_AXES, Control_تخصیص::عدد_ACTUATORS> &effectiveness,
	ثابت Actuator_سمتیہ &actuator_trim, ثابت Actuator_سمتیہ &linearization_نقطہ, صحیح عدد_actuators,
	منطقی update_normalization_scale)
{
	Control_تخصیص::سیٹ_Effectiveness_میٹرکس(effectiveness, actuator_trim, linearization_نقطہ, عدد_actuators,
			update_normalization_scale);

	// in place on the stored copy: no second matrix on the (small work queue) stack
	_dropped_axes = dropDependentAxes(_effectiveness);

	_mix_update_needed = سچ;
	_normalization_needs_update = update_normalization_scale;

	اگر (metric_تخصیص && update_normalization_scale) {
		// adding #include <px4_platform_common/log.h> + PX4_WARN leads to failed linking on test
		_normalization_needs_update = جھوٹ;
	}
}

خالی
Control_تخصیص_Pseudo_Inverse::updatePseudoInverse()
{
	اگر (_mix_update_needed) {
		میٹرکس::geninv(_effectiveness, _mix);

		اگر (!metric_تخصیص) {
			اگر (_normalization_needs_update && !_had_actuator_failure) {
				update_Control_تخصیص_میٹرکس_Scale();
				_normalization_needs_update = جھوٹ;
			}

			normalize_Control_تخصیص_میٹرکس();
		}

		_mix_update_needed = جھوٹ;
	}
}

خالی
Control_تخصیص_Pseudo_Inverse::update_Control_تخصیص_میٹرکس_Scale()
{
	// Same scale on roll and pitch
	اگر (_normalize_rpy) {

		صحیح عدد_non_zero_roll_torque = 0;
		صحیح عدد_non_zero_pitch_torque = 0;

		برائے (صحیح i = 0; i < عدد_actuators_2; i++) {

			اگر (fabsf(_mix(i, 0)) > 1e-3f) {
				++عدد_non_zero_roll_torque;
			}

			اگر (fabsf(_mix(i, 1)) > 1e-3f) {
				++عدد_non_zero_pitch_torque;
			}
		}

		اعشاری roll_norm_scale = 1.f;

		اگر (عدد_non_zero_roll_torque > 0) {
			roll_norm_scale = sqrtf(_mix.ستون(0).norm_squared() / (عدد_non_zero_roll_torque / 2.f));
		}

		اعشاری pitch_norm_scale = 1.f;

		اگر (عدد_non_zero_pitch_torque > 0) {
			pitch_norm_scale = sqrtf(_mix.ستون(1).norm_squared() / (عدد_non_zero_pitch_torque / 2.f));
		}

		control_تخصیص_scale(0) = fmaxf(roll_norm_scale, pitch_norm_scale);
		control_تخصیص_scale(1) = control_تخصیص_scale(0);

		// Scale yaw separately
		control_تخصیص_scale(2) = _mix.ستون(2).max();

	} ورنہ {
		control_تخصیص_scale(0) = 1.f;
		control_تخصیص_scale(1) = 1.f;
		control_تخصیص_scale(2) = 1.f;
	}

	// Scale thrust by the sum of the individual thrust axes, and use the scaling for the Z axis if there's no actuators
	// (for tilted actuators)
	control_تخصیص_scale(THRUST_Z) = 1.f;

	برائے (صحیح axis_idx = 2; axis_idx >= 0; --axis_idx) {
		صحیح عدد_non_zero_thrust = 0;
		اعشاری norm_مجموع = 0.f;

		برائے (صحیح i = 0; i < عدد_actuators_2; i++) {
			اعشاری norm = fabsf(_mix(i, 3 + axis_idx));
			norm_مجموع += norm;

			اگر (norm > FLT_EPSILON) {
				++عدد_non_zero_thrust;
			}
		}

		اگر (عدد_non_zero_thrust > 0) {
			control_تخصیص_scale(3 + axis_idx) = norm_مجموع / عدد_non_zero_thrust;

		} ورنہ {
			control_تخصیص_scale(3 + axis_idx) = control_تخصیص_scale(THRUST_Z);
		}
	}
}

خالی
Control_تخصیص_Pseudo_Inverse::normalize_Control_تخصیص_میٹرکس()
{
	اگر (control_تخصیص_scale(0) > FLT_EPSILON) {
		_mix.ستون(0) /= control_تخصیص_scale(0);
		_mix.ستون(1) /= control_تخصیص_scale(1);
	}

	اگر (control_تخصیص_scale(2) > FLT_EPSILON) {
		_mix.ستون(2) /= control_تخصیص_scale(2);
	}

	اگر (control_تخصیص_scale(3) > FLT_EPSILON) {
		_mix.ستون(3) /= control_تخصیص_scale(3);
		_mix.ستون(4) /= control_تخصیص_scale(4);
		_mix.ستون(5) /= control_تخصیص_scale(5);
	}

	// Set all the small elements to 0 to avoid issues
	// in the control allocation algorithms
	برائے (صحیح i = 0; i < عدد_actuators_2; i++) {
		برائے (صحیح j = 0; j < عدد_AXES; j++) {
			اگر (fabsf(_mix(i, j)) < 1e-3f) {
				_mix(i, j) = 0.f;
			}
		}
	}
}

خالی
Control_تخصیص_Pseudo_Inverse::allocate()
{
	//Compute new gains if needed
	updatePseudoInverse();

	پچھلا_actuator_sp = _actuator_sp;

	// Allocate
	_actuator_sp = _actuator_trim + _mix * (_control_sp - _control_trim);
}

uint8_t
Control_تخصیص_Pseudo_Inverse::dropDependentAxes(میٹرکس::میٹرکس_2<اعشاری, عدد_AXES, عدد_ACTUATORS> &effectiveness)
{
	// highest priority first
	ساکن constexpr ControlAxis k_ترجیح[عدد_AXES] = {THRUST_Z, ROLL, PITCH, THRUST_X, THRUST_Y, YAW};

	// Gram-Schmidt through the Cholesky factor of the Gram matrix of the accepted unit rows:
	// equivalent to orthogonalizing the rows, without keeping NUM_ACTUATORS-long vectors on the
	// (small work queue) stack
	اعشاری chol[عدد_AXES][عدد_AXES] {};
	اعشاری inv_norm[عدد_AXES];
	ControlAxis accepted[عدد_AXES];
	صحیح عدد_accepted = 0;
	uint8_t dropped = 0;

	برائے (ثابت ControlAxis axis : k_ترجیح) {
		ثابت اعشاری norm_squared = effectiveness.صف(axis).norm_squared();

		اگر (norm_squared < FLT_EPSILON) {
			جاری; // unused axis
		}

		ثابت اعشاری inv = 1.f / sqrtf(norm_squared);

		// projection of the unit row onto the orthonormal basis of the accepted rows
		اعشاری projection[عدد_AXES];
		اعشاری projected_norm_squared = 0.f;

		برائے (صحیح i = 0; i < عدد_accepted; i++) {
			اعشاری مجموع = effectiveness.صف(axis).dot(effectiveness.صف(accepted[i])) * inv * inv_norm[i];

			برائے (صحیح j = 0; j < i; j++) {
				مجموع -= chol[i][j] * projection[j];
			}

			projection[i] = مجموع / chol[i][i];
			projected_norm_squared += projection[i] * projection[i];
		}

		ثابت اعشاری independence = 1.f - projected_norm_squared;

		اگر (independence < kMinAxisIndependence) {
			effectiveness.صف(axis) = 0.f;
			dropped |= static_cast<uint8_t>(1u << axis);

		} ورنہ {
			برائے (صحیح j = 0; j < عدد_accepted; j++) {
				chol[عدد_accepted][j] = projection[j];
			}

			chol[عدد_accepted][عدد_accepted] = sqrtf(independence);
			inv_norm[عدد_accepted] = inv;
			accepted[عدد_accepted] = axis;
			عدد_accepted++;
		}
	}

	واپس dropped;
}

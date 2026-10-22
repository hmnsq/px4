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

#ضمن "ControlAllocationPseudoInverse.hpp"

فراغ
Control_ورهاست_Pseudo_Inverse::سيٽ_Effectiveness_ميٽرڪس(
	ثابت ميٽرڪس::ميٽرڪس_2<عائم, Control_ورهاست::انگ_AXES, Control_ورهاست::انگ_ACTUATORS> &effectiveness,
	ثابت Actuator_ويڪٽر &actuator_trim, ثابت Actuator_ويڪٽر &linearization_نقطو, صحيح انگ_actuators,
	منطقي update_normalization_scale)
{
	Control_ورهاست::سيٽ_Effectiveness_ميٽرڪس(effectiveness, actuator_trim, linearization_نقطو, انگ_actuators,
			update_normalization_scale);

	// in place on the stored copy: no second matrix on the (small work queue) stack
	_dropped_axes = dropDependentAxes(_effectiveness);

	_mix_update_needed = سچ;
	_normalization_needs_update = update_normalization_scale;

	إذا (metric_ورهاست && update_normalization_scale) {
		// adding #include <px4_platform_common/log.h> + PX4_WARN leads to failed linking on test
		_normalization_needs_update = خطأ;
	}
}

فراغ
Control_ورهاست_Pseudo_Inverse::updatePseudoInverse()
{
	إذا (_mix_update_needed) {
		ميٽرڪس::geninv(_effectiveness, _mix);

		إذا (!metric_ورهاست) {
			إذا (_normalization_needs_update && !_had_actuator_failure) {
				update_Control_ورهاست_ميٽرڪس_Scale();
				_normalization_needs_update = خطأ;
			}

			normalize_Control_ورهاست_ميٽرڪس();
		}

		_mix_update_needed = خطأ;
	}
}

فراغ
Control_ورهاست_Pseudo_Inverse::update_Control_ورهاست_ميٽرڪس_Scale()
{
	// Same scale on roll and pitch
	إذا (_normalize_rpy) {

		صحيح انگ_non_zero_roll_torque = 0;
		صحيح انگ_non_zero_pitch_torque = 0;

		لكل (صحيح i = 0; i < انگ_actuators_2; i++) {

			إذا (fabsf(_mix(i, 0)) > 1e-3f) {
				++انگ_non_zero_roll_torque;
			}

			إذا (fabsf(_mix(i, 1)) > 1e-3f) {
				++انگ_non_zero_pitch_torque;
			}
		}

		عائم roll_norm_scale = 1.f;

		إذا (انگ_non_zero_roll_torque > 0) {
			roll_norm_scale = sqrtf(_mix.کانو(0).norm_squared() / (انگ_non_zero_roll_torque / 2.f));
		}

		عائم pitch_norm_scale = 1.f;

		إذا (انگ_non_zero_pitch_torque > 0) {
			pitch_norm_scale = sqrtf(_mix.کانو(1).norm_squared() / (انگ_non_zero_pitch_torque / 2.f));
		}

		control_ورهاست_scale(0) = fmaxf(roll_norm_scale, pitch_norm_scale);
		control_ورهاست_scale(1) = control_ورهاست_scale(0);

		// Scale yaw separately
		control_ورهاست_scale(2) = _mix.کانو(2).max();

	} وإلا {
		control_ورهاست_scale(0) = 1.f;
		control_ورهاست_scale(1) = 1.f;
		control_ورهاست_scale(2) = 1.f;
	}

	// Scale thrust by the sum of the individual thrust axes, and use the scaling for the Z axis if there's no actuators
	// (for tilted actuators)
	control_ورهاست_scale(THRUST_Z) = 1.f;

	لكل (صحيح axis_idx = 2; axis_idx >= 0; --axis_idx) {
		صحيح انگ_non_zero_thrust = 0;
		عائم norm_جوڙ = 0.f;

		لكل (صحيح i = 0; i < انگ_actuators_2; i++) {
			عائم norm = fabsf(_mix(i, 3 + axis_idx));
			norm_جوڙ += norm;

			إذا (norm > FLT_EPSILON) {
				++انگ_non_zero_thrust;
			}
		}

		إذا (انگ_non_zero_thrust > 0) {
			control_ورهاست_scale(3 + axis_idx) = norm_جوڙ / انگ_non_zero_thrust;

		} وإلا {
			control_ورهاست_scale(3 + axis_idx) = control_ورهاست_scale(THRUST_Z);
		}
	}
}

فراغ
Control_ورهاست_Pseudo_Inverse::normalize_Control_ورهاست_ميٽرڪس()
{
	إذا (control_ورهاست_scale(0) > FLT_EPSILON) {
		_mix.کانو(0) /= control_ورهاست_scale(0);
		_mix.کانو(1) /= control_ورهاست_scale(1);
	}

	إذا (control_ورهاست_scale(2) > FLT_EPSILON) {
		_mix.کانو(2) /= control_ورهاست_scale(2);
	}

	إذا (control_ورهاست_scale(3) > FLT_EPSILON) {
		_mix.کانو(3) /= control_ورهاست_scale(3);
		_mix.کانو(4) /= control_ورهاست_scale(4);
		_mix.کانو(5) /= control_ورهاست_scale(5);
	}

	// Set all the small elements to 0 to avoid issues
	// in the control allocation algorithms
	لكل (صحيح i = 0; i < انگ_actuators_2; i++) {
		لكل (صحيح j = 0; j < انگ_AXES; j++) {
			إذا (fabsf(_mix(i, j)) < 1e-3f) {
				_mix(i, j) = 0.f;
			}
		}
	}
}

فراغ
Control_ورهاست_Pseudo_Inverse::allocate()
{
	//Compute new gains if needed
	updatePseudoInverse();

	_prev_actuator_sp = _actuator_sp;

	// Allocate
	_actuator_sp = _actuator_trim + _mix * (_control_sp - _control_trim);
}

uint8_t
Control_ورهاست_Pseudo_Inverse::dropDependentAxes(ميٽرڪس::ميٽرڪس_2<عائم, انگ_AXES, انگ_ACTUATORS> &effectiveness)
{
	// highest priority first
	ساكن constexpr ControlAxis k_ترجيح[انگ_AXES] = {THRUST_Z, ROLL, PITCH, THRUST_X, THRUST_Y, YAW};

	// Gram-Schmidt through the Cholesky factor of the Gram matrix of the accepted unit rows:
	// equivalent to orthogonalizing the rows, without keeping NUM_ACTUATORS-long vectors on the
	// (small work queue) stack
	عائم chol[انگ_AXES][انگ_AXES] {};
	عائم inv_norm[انگ_AXES];
	ControlAxis accepted[انگ_AXES];
	صحيح انگ_accepted = 0;
	uint8_t dropped = 0;

	لكل (ثابت ControlAxis axis : k_ترجيح) {
		ثابت عائم norm_squared = effectiveness.صف(axis).norm_squared();

		إذا (norm_squared < FLT_EPSILON) {
			استمر; // unused axis
		}

		ثابت عائم inv = 1.f / sqrtf(norm_squared);

		// projection of the unit row onto the orthonormal basis of the accepted rows
		عائم projection[انگ_AXES];
		عائم projected_norm_squared = 0.f;

		لكل (صحيح i = 0; i < انگ_accepted; i++) {
			عائم جوڙ = effectiveness.صف(axis).dot(effectiveness.صف(accepted[i])) * inv * inv_norm[i];

			لكل (صحيح j = 0; j < i; j++) {
				جوڙ -= chol[i][j] * projection[j];
			}

			projection[i] = جوڙ / chol[i][i];
			projected_norm_squared += projection[i] * projection[i];
		}

		ثابت عائم independence = 1.f - projected_norm_squared;

		إذا (independence < kMinAxisIndependence) {
			effectiveness.صف(axis) = 0.f;
			dropped |= static_cast<uint8_t>(1u << axis);

		} وإلا {
			لكل (صحيح j = 0; j < انگ_accepted; j++) {
				chol[انگ_accepted][j] = projection[j];
			}

			chol[انگ_accepted][انگ_accepted] = sqrtf(independence);
			inv_norm[انگ_accepted] = inv;
			accepted[انگ_accepted] = axis;
			انگ_accepted++;
		}
	}

	أرجع dropped;
}

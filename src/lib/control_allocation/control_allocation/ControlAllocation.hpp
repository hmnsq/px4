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
 * @file ControlAllocation.hpp
 *
 * Interface for Control Allocation Algorithms
 *
 * Implementers of this interface are expected to update the members
 * of this base class in the `allocate` method.
 *
 * Example usage:
 * ```
 * [...]
 * // Initialization
 * ControlAllocationMethodImpl alloc();
 * alloc.setEffectivenessMatrix(effectiveness, actuator_trim);
 * alloc.setActuatorMin(actuator_min);
 * alloc.setActuatorMin(actuator_max);
 *
 * while (1) {
 * 	[...]
 *
 * 	// Set control setpoint, allocate actuator setpoint, retrieve actuator setpoint
 * 	alloc.setControlSetpoint(control_sp);
 * 	alloc.allocate();
 * 	actuator_sp = alloc.getActuatorSetpoint();
 *
 * 	// Check if the control setpoint was fully allocated
 *	unallocated_control = control_sp - alloc.getAllocatedControl()
 *
 *	[...]
 * }
 * ```
 *
 *
 * @author Julien Lecoeur <julien.lecoeur@gmail.com>
 */

#ಸೂಚನೆ once

#ಅನ್ತರ್ಭಾವ <matrix/matrix/math.hpp>

#ಅನ್ತರ್ಭಾವ "control_allocation/actuator_effectiveness/ActuatorEffectiveness.hpp"

ವರ್ಗ Control_ಹಂಚಿಕೆ
{
ಸಾರ್ವಜನಿಕ:
	Control_ಹಂಚಿಕೆ();
	ವಾಸ್ತವಿಕ ~Control_ಹಂಚಿಕೆ() = ಪೂರ್ವನಿಯೋಜಿತ;

	ಸ್ಥಿರ constexpr uint8_t ಅಂಕೆ_ACTUATORS = ActuatorEffectiveness::ಅಂಕೆ_ACTUATORS;
	ಸ್ಥಿರ constexpr uint8_t ಅಂಕೆ_AXES = ActuatorEffectiveness::ಅಂಕೆ_AXES;

	ಬಳಸಿ Actuator_ಸದಿಶ = ಮಾತೃಕೆ::ಸದಿಶ<ದಶಮಾಂಶ, ಅಂಕೆ_ACTUATORS>;
	ಬಳಸಿ ActuatorBitmask = ActuatorEffectiveness::ActuatorBitmask;

	ಎಣಿಕೆ ControlAxis {
		ROLL = 0,
		PITCH,
		YAW,
		THRUST_X,
		THRUST_Y,
		THRUST_Z
	};

	/**
	 * Allocate control setpoint to actuators
	 */
	ವಾಸ್ತವಿಕ ಶೂನ್ಯ allocate() = 0;

	/**
	 * Set actuator failure flag
	 * This prevents a change of the scaling in the matrix normalization step
	 * in case of a motor failure.
	 *
	 * @param failure  Motor failure flag
	 */
	ಶೂನ್ಯ ಗಣ_Had_Actuator_Failure(ತಾರ್ಕಿಕ failure) { _had_actuator_failure = failure; }

	/**
	 * Set the control effectiveness matrix
	 *
	 * @param B Effectiveness matrix
	 */
	ವಾಸ್ತವಿಕ ಶೂನ್ಯ ಗಣ_Effectiveness_ಮಾತೃಕೆ(ಸ್ಥಿರಾಂಕ ಮಾತೃಕೆ::ಮಾತೃಕೆ_2<ದಶಮಾಂಶ, ಅಂಕೆ_AXES, ಅಂಕೆ_ACTUATORS> &effectiveness,
					    ಸ್ಥಿರಾಂಕ Actuator_ಸದಿಶ &actuator_trim, ಸ್ಥಿರಾಂಕ Actuator_ಸದಿಶ &linearization_ಬಿಂದು, ಪೂರ್ಣಾಂಕ ಅಂಕೆ_actuators,
					    ತಾರ್ಕಿಕ update_normalization_scale);

	/**
	 * Get the allocated actuator vector
	 *
	 * @return Actuator vector
	 */
	ಸ್ಥಿರಾಂಕ Actuator_ಸದಿಶ &getActuatorSetpoint() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ _actuator_sp; }

	/**
	 * Set the desired control vector
	 *
	 * @param Control vector
	 */
	ಶೂನ್ಯ ಗಣ_Control_Setpoint(ಸ್ಥಿರಾಂಕ ಮಾತೃಕೆ::ಸದಿಶ<ದಶಮಾಂಶ, ಅಂಕೆ_AXES> &control) { _control_sp = control; }

	/**
	 * Get the desired control vector
	 *
	 * @return Control vector
	 */
	ಸ್ಥಿರಾಂಕ ಮಾತೃಕೆ::ಸದಿಶ<ದಶಮಾಂಶ, ಅಂಕೆ_AXES> &getControlSetpoint() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ _control_sp; }

	/**
	 * Get the allocated control vector
	 *
	 * @return Control vector
	 */
	ಮಾತೃಕೆ::ಸದಿಶ<ದಶಮಾಂಶ, ಅಂಕೆ_AXES> getAllocatedControl() ಸ್ಥಿರಾಂಕ
	{
		// _actuator_sp contains NaN to indicate stopped motors.
		// Covert back to zero to represent physical thrust.
		Actuator_ಸದಿಶ actuator_sp{_actuator_sp};
		actuator_sp.nan_ಗಮ್ಯ_Zero();
		ಹಿಂತಿರುಗಿ (_effectiveness * (actuator_sp - _actuator_trim)).emult(control_ಹಂಚಿಕೆ_scale);
	}

	/**
	 * Get the control effectiveness matrix
	 *
	 * @return Effectiveness matrix
	 */
	ಸ್ಥಿರಾಂಕ ಮಾತೃಕೆ::ಮಾತೃಕೆ_2<ದಶಮಾಂಶ, ಅಂಕೆ_AXES, ಅಂಕೆ_ACTUATORS> &get_Effectiveness_ಮಾತೃಕೆ() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ _effectiveness; }

	/**
	 * Set the minimum actuator values
	 *
	 * @param actuator_min Minimum actuator values
	 */
	ಶೂನ್ಯ ಗಣ_Actuator_Min(ಸ್ಥಿರಾಂಕ Actuator_ಸದಿಶ &actuator_min) { _actuator_min = actuator_min; }

	/**
	 * Get the minimum actuator values
	 *
	 * @return Minimum actuator values
	 */
	ಸ್ಥಿರಾಂಕ Actuator_ಸದಿಶ &getActuatorMin() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ _actuator_min; }

	/**
	 * Set the maximum actuator values
	 *
	 * @param actuator_max Maximum actuator values
	 */
	ಶೂನ್ಯ ಗಣ_Actuator_Max(ಸ್ಥಿರಾಂಕ Actuator_ಸದಿಶ &actuator_max) { _actuator_max = actuator_max; }

	/**
	 * Get the maximum actuator values
	 *
	 * @return Maximum actuator values
	 */
	ಸ್ಥಿರಾಂಕ Actuator_ಸದಿಶ &getActuatorMax() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ _actuator_max; }

	/**
	 * Set the current actuator setpoint.
	 *
	 * Use this when a new allocation method is started to initialize it properly.
	 * In most cases, it is not needed to call this method before `allocate()`.
	 * Indeed the previous actuator setpoint is expected to be stored during calls to `allocate()`.
	 *
	 * @param actuator_sp Actuator setpoint
	 */
	ಶೂನ್ಯ ಗಣ_Actuator_Setpoint(ಸ್ಥಿರಾಂಕ Actuator_ಸದಿಶ &actuator_sp);

	ಶೂನ್ಯ ಗಣ_Slew_ದರ_ಎಲ್ಲೆ(ಸ್ಥಿರಾಂಕ Actuator_ಸದಿಶ &slew_ದರ_ಎಲ್ಲೆ)
	{ actuator_slew_ದರ_ಎಲ್ಲೆ = slew_ದರ_ಎಲ್ಲೆ; }

	/**
	 * Apply slew rate to current actuator setpoint
	 */
	ಶೂನ್ಯ apply_Slew_ದರ_ಎಲ್ಲೆ(ದಶಮಾಂಶ dt);

	/**
	 * Clip the actuator setpoint between minimum and maximum values.
	 *
	 * The output is in the range [min; max]
	 *
	 * @param actuator Actuator vector to clip
	 */
	ಶೂನ್ಯ clipActuatorSetpoint(Actuator_ಸದಿಶ &actuator) ಸ್ಥಿರಾಂಕ;

	ಶೂನ್ಯ clipActuatorSetpoint() { clipActuatorSetpoint(_actuator_sp); }

	/**
	 * Normalize the actuator setpoint between minimum and maximum values.
	 *
	 * The output is in the range [-1; +1]
	 *
	 * @param actuator Actuator vector to normalize
	 *
	 * @return Clipped actuator setpoint
	 */
	Actuator_ಸದಿಶ normalizeActuatorSetpoint(ಸ್ಥಿರಾಂಕ Actuator_ಸದಿಶ &actuator)
	ಸ್ಥಿರಾಂಕ;

	/**
	 * Apply a mask of actuators to be set to NaN.
	 *
	 * A NaN value in _actuator_sp represents a disabled or stopped actuator.
	 * This mask is typically used to stop motors in specific flight phases or when certain thrust components are NaN.
	 *
	 * @param nan_actuators_mask Bitmask indicating which actuators to set to NaN.
	 *                           If (nan_actuators_mask & (1 << i)), _actuator_sp(i) becomes NaN.
	 */
	ಶೂನ್ಯ apply_Nan_ಗಮ್ಯ_Actuators(ActuatorBitmask nan_actuators_ಮುಖವಾಡ)
	{
		ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < ಅಂಕೆ_actuators_2; i++) {
			ಆದರೆ (nan_actuators_ಮುಖವಾಡ & (1u << i)) {
				_actuator_sp(i) = NAN;
			}
		}
	}

	ವಾಸ್ತವಿಕ ಶೂನ್ಯ updateParameters() {}

	ಪೂರ್ಣಾಂಕ ಅಂಕೆ_Configured_Actuators() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ ಅಂಕೆ_actuators_2; }

	ಶೂನ್ಯ ಗಣ_Normalize_RPY(ತಾರ್ಕಿಕ normalize_rpy) { _normalize_rpy = normalize_rpy; }

	/// Axes (bitmask over ControlAxis) dropped from the effectiveness matrix as not independently
	/// achievable, e.g. yaw when collinear with roll after a motor failure
	uint8_t getDroppedAxes() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ _dropped_axes; }

ಸಂರಕ್ಷಿತ:
	ಮಿತ್ರ ವರ್ಗ ControlAllocator; // for _actuator_sp

	ಮಾತೃಕೆ::ಮಾತೃಕೆ_2<ದಶಮಾಂಶ, ಅಂಕೆ_AXES, ಅಂಕೆ_ACTUATORS> _effectiveness;  ///< Effectiveness matrix
	ಮಾತೃಕೆ::ಸದಿಶ<ದಶಮಾಂಶ, ಅಂಕೆ_AXES> control_ಹಂಚಿಕೆ_scale;  	///< Scaling applied during allocation
	Actuator_ಸದಿಶ _actuator_trim; 	///< Neutral actuator values
	Actuator_ಸದಿಶ _actuator_min; 	///< Minimum actuator values
	Actuator_ಸದಿಶ _actuator_max; 	///< Maximum actuator values
	Actuator_ಸದಿಶ actuator_slew_ದರ_ಎಲ್ಲೆ; 	///< Slew rate limit
	Actuator_ಸದಿಶ _prev_actuator_sp;  	///< Previous actuator setpoint
	Actuator_ಸದಿಶ _actuator_sp;  	///< Actuator setpoint
	ಮಾತೃಕೆ::ಸದಿಶ<ದಶಮಾಂಶ, ಅಂಕೆ_AXES> _control_sp;   		///< Control setpoint
	ಮಾತೃಕೆ::ಸದಿಶ<ದಶಮಾಂಶ, ಅಂಕೆ_AXES> _control_trim; 		///< Control at trim actuator values
	ಪೂರ್ಣಾಂಕ ಅಂಕೆ_actuators_2{0};
	ತಾರ್ಕಿಕ _normalize_rpy{ಸುಳ್ಳು};				///< if true, normalize roll, pitch and yaw columns
	ತಾರ್ಕಿಕ _had_actuator_failure{ಸುಳ್ಳು};
	uint8_t _dropped_axes{0};				///< bitmask over ControlAxis
};

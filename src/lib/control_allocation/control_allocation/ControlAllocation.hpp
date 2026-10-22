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

#ਹਦਾਇਤ once

#ਅਨ੍ਤਰ੍ਭਾਵ <matrix/matrix/math.hpp>

#ਅਨ੍ਤਰ੍ਭਾਵ "control_allocation/actuator_effectiveness/ActuatorEffectiveness.hpp"

ਵਰਗ Control_ਵੰਡ
{
ਜਨਤਕ:
	Control_ਵੰਡ();
	ਆਭਾਸੀ ~Control_ਵੰਡ() = ਮੂਲ;

	ਸਥਿਰ constexpr uint8_t ਅੰਕ_ACTUATORS = ActuatorEffectiveness::ਅੰਕ_ACTUATORS;
	ਸਥਿਰ constexpr uint8_t ਅੰਕ_AXES = ActuatorEffectiveness::ਅੰਕ_AXES;

	ਵਰਤੋਂ Actuator_ਸਦਿਸ਼ = ਮੈਟ੍ਰਿਕਸ::ਸਦਿਸ਼<ਦਸ਼ਮਲਵ, ਅੰਕ_ACTUATORS>;
	ਵਰਤੋਂ ActuatorBitmask = ActuatorEffectiveness::ActuatorBitmask;

	ਗਿਣਤੀ ControlAxis {
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
	ਆਭਾਸੀ ਖਾਲੀ allocate() = 0;

	/**
	 * Set actuator failure flag
	 * This prevents a change of the scaling in the matrix normalization step
	 * in case of a motor failure.
	 *
	 * @param failure  Motor failure flag
	 */
	ਖਾਲੀ ਸੈੱਟ_Had_Actuator_Failure(ਤਾਰਕਿਕ failure) { _had_actuator_failure = failure; }

	/**
	 * Set the control effectiveness matrix
	 *
	 * @param B Effectiveness matrix
	 */
	ਆਭਾਸੀ ਖਾਲੀ ਸੈੱਟ_Effectiveness_ਮੈਟ੍ਰਿਕਸ(ਅਚਲ ਮੈਟ੍ਰਿਕਸ::ਮੈਟ੍ਰਿਕਸ_2<ਦਸ਼ਮਲਵ, ਅੰਕ_AXES, ਅੰਕ_ACTUATORS> &effectiveness,
					    ਅਚਲ Actuator_ਸਦਿਸ਼ &actuator_trim, ਅਚਲ Actuator_ਸਦਿਸ਼ &linearization_ਬਿੰਦੂ, ਪੂਰਨ ਅੰਕ_actuators,
					    ਤਾਰਕਿਕ update_normalization_scale);

	/**
	 * Get the allocated actuator vector
	 *
	 * @return Actuator vector
	 */
	ਅਚਲ Actuator_ਸਦਿਸ਼ &getActuatorSetpoint() ਅਚਲ { ਵਾਪਸ _actuator_sp; }

	/**
	 * Set the desired control vector
	 *
	 * @param Control vector
	 */
	ਖਾਲੀ ਸੈੱਟ_Control_Setpoint(ਅਚਲ ਮੈਟ੍ਰਿਕਸ::ਸਦਿਸ਼<ਦਸ਼ਮਲਵ, ਅੰਕ_AXES> &control) { _control_sp = control; }

	/**
	 * Get the desired control vector
	 *
	 * @return Control vector
	 */
	ਅਚਲ ਮੈਟ੍ਰਿਕਸ::ਸਦਿਸ਼<ਦਸ਼ਮਲਵ, ਅੰਕ_AXES> &getControlSetpoint() ਅਚਲ { ਵਾਪਸ _control_sp; }

	/**
	 * Get the allocated control vector
	 *
	 * @return Control vector
	 */
	ਮੈਟ੍ਰਿਕਸ::ਸਦਿਸ਼<ਦਸ਼ਮਲਵ, ਅੰਕ_AXES> getAllocatedControl() ਅਚਲ
	{
		// _actuator_sp contains NaN to indicate stopped motors.
		// Covert back to zero to represent physical thrust.
		Actuator_ਸਦਿਸ਼ actuator_sp{_actuator_sp};
		actuator_sp.nan_ਮੰਜ਼ਿਲ_Zero();
		ਵਾਪਸ (_effectiveness * (actuator_sp - _actuator_trim)).emult(control_ਵੰਡ_scale);
	}

	/**
	 * Get the control effectiveness matrix
	 *
	 * @return Effectiveness matrix
	 */
	ਅਚਲ ਮੈਟ੍ਰਿਕਸ::ਮੈਟ੍ਰਿਕਸ_2<ਦਸ਼ਮਲਵ, ਅੰਕ_AXES, ਅੰਕ_ACTUATORS> &get_Effectiveness_ਮੈਟ੍ਰਿਕਸ() ਅਚਲ { ਵਾਪਸ _effectiveness; }

	/**
	 * Set the minimum actuator values
	 *
	 * @param actuator_min Minimum actuator values
	 */
	ਖਾਲੀ ਸੈੱਟ_Actuator_Min(ਅਚਲ Actuator_ਸਦਿਸ਼ &actuator_min) { _actuator_min = actuator_min; }

	/**
	 * Get the minimum actuator values
	 *
	 * @return Minimum actuator values
	 */
	ਅਚਲ Actuator_ਸਦਿਸ਼ &getActuatorMin() ਅਚਲ { ਵਾਪਸ _actuator_min; }

	/**
	 * Set the maximum actuator values
	 *
	 * @param actuator_max Maximum actuator values
	 */
	ਖਾਲੀ ਸੈੱਟ_Actuator_Max(ਅਚਲ Actuator_ਸਦਿਸ਼ &actuator_max) { _actuator_max = actuator_max; }

	/**
	 * Get the maximum actuator values
	 *
	 * @return Maximum actuator values
	 */
	ਅਚਲ Actuator_ਸਦਿਸ਼ &getActuatorMax() ਅਚਲ { ਵਾਪਸ _actuator_max; }

	/**
	 * Set the current actuator setpoint.
	 *
	 * Use this when a new allocation method is started to initialize it properly.
	 * In most cases, it is not needed to call this method before `allocate()`.
	 * Indeed the previous actuator setpoint is expected to be stored during calls to `allocate()`.
	 *
	 * @param actuator_sp Actuator setpoint
	 */
	ਖਾਲੀ ਸੈੱਟ_Actuator_Setpoint(ਅਚਲ Actuator_ਸਦਿਸ਼ &actuator_sp);

	ਖਾਲੀ ਸੈੱਟ_Slew_ਦਰ_ਹੱਦਬੰਦੀ(ਅਚਲ Actuator_ਸਦਿਸ਼ &slew_ਦਰ_ਹੱਦਬੰਦੀ)
	{ actuator_slew_ਦਰ_ਹੱਦਬੰਦੀ = slew_ਦਰ_ਹੱਦਬੰਦੀ; }

	/**
	 * Apply slew rate to current actuator setpoint
	 */
	ਖਾਲੀ apply_Slew_ਦਰ_ਹੱਦਬੰਦੀ(ਦਸ਼ਮਲਵ dt);

	/**
	 * Clip the actuator setpoint between minimum and maximum values.
	 *
	 * The output is in the range [min; max]
	 *
	 * @param actuator Actuator vector to clip
	 */
	ਖਾਲੀ clipActuatorSetpoint(Actuator_ਸਦਿਸ਼ &actuator) ਅਚਲ;

	ਖਾਲੀ clipActuatorSetpoint() { clipActuatorSetpoint(_actuator_sp); }

	/**
	 * Normalize the actuator setpoint between minimum and maximum values.
	 *
	 * The output is in the range [-1; +1]
	 *
	 * @param actuator Actuator vector to normalize
	 *
	 * @return Clipped actuator setpoint
	 */
	Actuator_ਸਦਿਸ਼ normalizeActuatorSetpoint(ਅਚਲ Actuator_ਸਦਿਸ਼ &actuator)
	ਅਚਲ;

	/**
	 * Apply a mask of actuators to be set to NaN.
	 *
	 * A NaN value in _actuator_sp represents a disabled or stopped actuator.
	 * This mask is typically used to stop motors in specific flight phases or when certain thrust components are NaN.
	 *
	 * @param nan_actuators_mask Bitmask indicating which actuators to set to NaN.
	 *                           If (nan_actuators_mask & (1 << i)), _actuator_sp(i) becomes NaN.
	 */
	ਖਾਲੀ apply_Nan_ਮੰਜ਼ਿਲ_Actuators(ActuatorBitmask nan_actuators_ਨਕਾਬ)
	{
		ਲਈ (ਪੂਰਨ i = 0; i < ਅੰਕ_actuators_2; i++) {
			ਜੇ (nan_actuators_ਨਕਾਬ & (1u << i)) {
				_actuator_sp(i) = NAN;
			}
		}
	}

	ਆਭਾਸੀ ਖਾਲੀ updateParameters() {}

	ਪੂਰਨ ਅੰਕ_Configured_Actuators() ਅਚਲ { ਵਾਪਸ ਅੰਕ_actuators_2; }

	ਖਾਲੀ ਸੈੱਟ_Normalize_RPY(ਤਾਰਕਿਕ normalize_rpy) { _normalize_rpy = normalize_rpy; }

	/// Axes (bitmask over ControlAxis) dropped from the effectiveness matrix as not independently
	/// achievable, e.g. yaw when collinear with roll after a motor failure
	uint8_t getDroppedAxes() ਅਚਲ { ਵਾਪਸ _dropped_axes; }

ਸੁਰੱਖਿਅਤ:
	ਮਿੱਤਰ ਵਰਗ ControlAllocator; // for _actuator_sp

	ਮੈਟ੍ਰਿਕਸ::ਮੈਟ੍ਰਿਕਸ_2<ਦਸ਼ਮਲਵ, ਅੰਕ_AXES, ਅੰਕ_ACTUATORS> _effectiveness;  ///< Effectiveness matrix
	ਮੈਟ੍ਰਿਕਸ::ਸਦਿਸ਼<ਦਸ਼ਮਲਵ, ਅੰਕ_AXES> control_ਵੰਡ_scale;  	///< Scaling applied during allocation
	Actuator_ਸਦਿਸ਼ _actuator_trim; 	///< Neutral actuator values
	Actuator_ਸਦਿਸ਼ _actuator_min; 	///< Minimum actuator values
	Actuator_ਸਦਿਸ਼ _actuator_max; 	///< Maximum actuator values
	Actuator_ਸਦਿਸ਼ actuator_slew_ਦਰ_ਹੱਦਬੰਦੀ; 	///< Slew rate limit
	Actuator_ਸਦਿਸ਼ _prev_actuator_sp;  	///< Previous actuator setpoint
	Actuator_ਸਦਿਸ਼ _actuator_sp;  	///< Actuator setpoint
	ਮੈਟ੍ਰਿਕਸ::ਸਦਿਸ਼<ਦਸ਼ਮਲਵ, ਅੰਕ_AXES> _control_sp;   		///< Control setpoint
	ਮੈਟ੍ਰਿਕਸ::ਸਦਿਸ਼<ਦਸ਼ਮਲਵ, ਅੰਕ_AXES> _control_trim; 		///< Control at trim actuator values
	ਪੂਰਨ ਅੰਕ_actuators_2{0};
	ਤਾਰਕਿਕ _normalize_rpy{ਝੂਠ};				///< if true, normalize roll, pitch and yaw columns
	ਤਾਰਕਿਕ _had_actuator_failure{ਝੂਠ};
	uint8_t _dropped_axes{0};				///< bitmask over ControlAxis
};

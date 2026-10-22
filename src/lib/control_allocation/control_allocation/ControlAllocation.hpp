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

#pragma once

#അന്തര്ഭാവ <matrix/matrix/math.hpp>

#അന്തര്ഭാവ "control_allocation/actuator_effectiveness/ActuatorEffectiveness.hpp"

വർഗ്ഗം ControlAllocation
{
പൊതു:
	ControlAllocation();
	മായാ ~ControlAllocation() = സ്ഥിരസ്ഥിതി;

	സ്ഥിതം constexpr uint8_t അക്കം_ACTUATORS = ActuatorEffectiveness::അക്കം_ACTUATORS;
	സ്ഥിതം constexpr uint8_t അക്കം_AXES = ActuatorEffectiveness::അക്കം_AXES;

	using ActuatorVector = matrix::Vector<ദശാംശം, അക്കം_ACTUATORS>;
	using ActuatorBitmask = ActuatorEffectiveness::ActuatorBitmask;

	എണ്ണൽ ControlAxis {
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
	മായാ ശൂന്യം allocate() = 0;

	/**
	 * Set actuator failure flag
	 * This prevents a change of the scaling in the matrix normalization step
	 * in case of a motor failure.
	 *
	 * @param failure  Motor failure flag
	 */
	ശൂന്യം setHadActuatorFailure(തർക്കം failure) { _had_actuator_failure = failure; }

	/**
	 * Set the control effectiveness matrix
	 *
	 * @param B Effectiveness matrix
	 */
	മായാ ശൂന്യം setEffectivenessMatrix(സ്ഥിരം matrix::Matrix<ദശാംശം, അക്കം_AXES, അക്കം_ACTUATORS> &effectiveness,
					    സ്ഥിരം ActuatorVector &actuator_trim, സ്ഥിരം ActuatorVector &linearization_point, പൂർണ്ണസംഖ്യ അക്കം_actuators,
					    തർക്കം update_normalization_scale);

	/**
	 * Get the allocated actuator vector
	 *
	 * @return Actuator vector
	 */
	സ്ഥിരം ActuatorVector &getActuatorSetpoint() സ്ഥിരം { തിരികെ _actuator_sp; }

	/**
	 * Set the desired control vector
	 *
	 * @param Control vector
	 */
	ശൂന്യം setControlSetpoint(സ്ഥിരം matrix::Vector<ദശാംശം, അക്കം_AXES> &control) { _control_sp = control; }

	/**
	 * Get the desired control vector
	 *
	 * @return Control vector
	 */
	സ്ഥിരം matrix::Vector<ദശാംശം, അക്കം_AXES> &getControlSetpoint() സ്ഥിരം { തിരികെ _control_sp; }

	/**
	 * Get the allocated control vector
	 *
	 * @return Control vector
	 */
	matrix::Vector<ദശാംശം, അക്കം_AXES> getAllocatedControl() സ്ഥിരം
	{
		// _actuator_sp contains NaN to indicate stopped motors.
		// Covert back to zero to represent physical thrust.
		ActuatorVector actuator_sp{_actuator_sp};
		actuator_sp.nanToZero();
		തിരികെ (_effectiveness * (actuator_sp - _actuator_trim)).emult(_control_allocation_scale);
	}

	/**
	 * Get the control effectiveness matrix
	 *
	 * @return Effectiveness matrix
	 */
	സ്ഥിരം matrix::Matrix<ദശാംശം, അക്കം_AXES, അക്കം_ACTUATORS> &getEffectivenessMatrix() സ്ഥിരം { തിരികെ _effectiveness; }

	/**
	 * Set the minimum actuator values
	 *
	 * @param actuator_min Minimum actuator values
	 */
	ശൂന്യം setActuatorMin(സ്ഥിരം ActuatorVector &actuator_min) { _actuator_min = actuator_min; }

	/**
	 * Get the minimum actuator values
	 *
	 * @return Minimum actuator values
	 */
	സ്ഥിരം ActuatorVector &getActuatorMin() സ്ഥിരം { തിരികെ _actuator_min; }

	/**
	 * Set the maximum actuator values
	 *
	 * @param actuator_max Maximum actuator values
	 */
	ശൂന്യം setActuatorMax(സ്ഥിരം ActuatorVector &actuator_max) { _actuator_max = actuator_max; }

	/**
	 * Get the maximum actuator values
	 *
	 * @return Maximum actuator values
	 */
	സ്ഥിരം ActuatorVector &getActuatorMax() സ്ഥിരം { തിരികെ _actuator_max; }

	/**
	 * Set the current actuator setpoint.
	 *
	 * Use this when a new allocation method is started to initialize it properly.
	 * In most cases, it is not needed to call this method before `allocate()`.
	 * Indeed the previous actuator setpoint is expected to be stored during calls to `allocate()`.
	 *
	 * @param actuator_sp Actuator setpoint
	 */
	ശൂന്യം setActuatorSetpoint(സ്ഥിരം ActuatorVector &actuator_sp);

	ശൂന്യം setSlewRateLimit(സ്ഥിരം ActuatorVector &slew_rate_limit)
	{ _actuator_slew_rate_limit = slew_rate_limit; }

	/**
	 * Apply slew rate to current actuator setpoint
	 */
	ശൂന്യം applySlewRateLimit(ദശാംശം dt);

	/**
	 * Clip the actuator setpoint between minimum and maximum values.
	 *
	 * The output is in the range [min; max]
	 *
	 * @param actuator Actuator vector to clip
	 */
	ശൂന്യം clipActuatorSetpoint(ActuatorVector &actuator) സ്ഥിരം;

	ശൂന്യം clipActuatorSetpoint() { clipActuatorSetpoint(_actuator_sp); }

	/**
	 * Normalize the actuator setpoint between minimum and maximum values.
	 *
	 * The output is in the range [-1; +1]
	 *
	 * @param actuator Actuator vector to normalize
	 *
	 * @return Clipped actuator setpoint
	 */
	ActuatorVector normalizeActuatorSetpoint(സ്ഥിരം ActuatorVector &actuator)
	സ്ഥിരം;

	/**
	 * Apply a mask of actuators to be set to NaN.
	 *
	 * A NaN value in _actuator_sp represents a disabled or stopped actuator.
	 * This mask is typically used to stop motors in specific flight phases or when certain thrust components are NaN.
	 *
	 * @param nan_actuators_mask Bitmask indicating which actuators to set to NaN.
	 *                           If (nan_actuators_mask & (1 << i)), _actuator_sp(i) becomes NaN.
	 */
	ശൂന്യം applyNanToActuators(ActuatorBitmask nan_actuators_mask)
	{
		വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < അക്കം_actuators_2; i++) {
			എങ്കിൽ (nan_actuators_mask & (1u << i)) {
				_actuator_sp(i) = NAN;
			}
		}
	}

	മായാ ശൂന്യം updateParameters() {}

	പൂർണ്ണസംഖ്യ അക്കം_Configured_Actuators() സ്ഥിരം { തിരികെ അക്കം_actuators_2; }

	ശൂന്യം setNormalizeRPY(തർക്കം normalize_rpy) { _normalize_rpy = normalize_rpy; }

	/// Axes (bitmask over ControlAxis) dropped from the effectiveness matrix as not independently
	/// achievable, e.g. yaw when collinear with roll after a motor failure
	uint8_t getDroppedAxes() സ്ഥിരം { തിരികെ _dropped_axes; }

സംരക്ഷിത:
	friend വർഗ്ഗം ControlAllocator; // for _actuator_sp

	matrix::Matrix<ദശാംശം, അക്കം_AXES, അക്കം_ACTUATORS> _effectiveness;  ///< Effectiveness matrix
	matrix::Vector<ദശാംശം, അക്കം_AXES> _control_allocation_scale;  	///< Scaling applied during allocation
	ActuatorVector _actuator_trim; 	///< Neutral actuator values
	ActuatorVector _actuator_min; 	///< Minimum actuator values
	ActuatorVector _actuator_max; 	///< Maximum actuator values
	ActuatorVector _actuator_slew_rate_limit; 	///< Slew rate limit
	ActuatorVector _prev_actuator_sp;  	///< Previous actuator setpoint
	ActuatorVector _actuator_sp;  	///< Actuator setpoint
	matrix::Vector<ദശാംശം, അക്കം_AXES> _control_sp;   		///< Control setpoint
	matrix::Vector<ദശാംശം, അക്കം_AXES> _control_trim; 		///< Control at trim actuator values
	പൂർണ്ണസംഖ്യ അക്കം_actuators_2{0};
	തർക്കം _normalize_rpy{അസത്യം};				///< if true, normalize roll, pitch and yaw columns
	തർക്കം _had_actuator_failure{അസത്യം};
	uint8_t _dropped_axes{0};				///< bitmask over ControlAxis
};

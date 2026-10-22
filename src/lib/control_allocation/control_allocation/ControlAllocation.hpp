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

#અન્તર્ભાવ <matrix/matrix/math.hpp>

#અન્તર્ભાવ "control_allocation/actuator_effectiveness/ActuatorEffectiveness.hpp"

વર્ગ ControlAllocation
{
જાહેર:
	ControlAllocation();
	આભાસી ~ControlAllocation() = મૂળભૂત;

	સ્થિર constexpr uint8_t અંક_ACTUATORS = ActuatorEffectiveness::અંક_ACTUATORS;
	સ્થિર constexpr uint8_t અંક_AXES = ActuatorEffectiveness::અંક_AXES;

	using ActuatorVector = matrix::Vector<દશાંશ, અંક_ACTUATORS>;
	using ActuatorBitmask = ActuatorEffectiveness::ActuatorBitmask;

	ગણના ControlAxis {
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
	આભાસી શૂન્ય allocate() = 0;

	/**
	 * Set actuator failure flag
	 * This prevents a change of the scaling in the matrix normalization step
	 * in case of a motor failure.
	 *
	 * @param failure  Motor failure flag
	 */
	શૂન્ય setHadActuatorFailure(તાર્કિક failure) { _had_actuator_failure = failure; }

	/**
	 * Set the control effectiveness matrix
	 *
	 * @param B Effectiveness matrix
	 */
	આભાસી શૂન્ય setEffectivenessMatrix(અચલ matrix::Matrix<દશાંશ, અંક_AXES, અંક_ACTUATORS> &effectiveness,
					    અચલ ActuatorVector &actuator_trim, અચલ ActuatorVector &linearization_point, પૂર્ણાંક અંક_actuators,
					    તાર્કિક update_normalization_scale);

	/**
	 * Get the allocated actuator vector
	 *
	 * @return Actuator vector
	 */
	અચલ ActuatorVector &getActuatorSetpoint() અચલ { પાછા _actuator_sp; }

	/**
	 * Set the desired control vector
	 *
	 * @param Control vector
	 */
	શૂન્ય setControlSetpoint(અચલ matrix::Vector<દશાંશ, અંક_AXES> &control) { _control_sp = control; }

	/**
	 * Get the desired control vector
	 *
	 * @return Control vector
	 */
	અચલ matrix::Vector<દશાંશ, અંક_AXES> &getControlSetpoint() અચલ { પાછા _control_sp; }

	/**
	 * Get the allocated control vector
	 *
	 * @return Control vector
	 */
	matrix::Vector<દશાંશ, અંક_AXES> getAllocatedControl() અચલ
	{
		// _actuator_sp contains NaN to indicate stopped motors.
		// Covert back to zero to represent physical thrust.
		ActuatorVector actuator_sp{_actuator_sp};
		actuator_sp.nanToZero();
		પાછા (_effectiveness * (actuator_sp - _actuator_trim)).emult(_control_allocation_scale);
	}

	/**
	 * Get the control effectiveness matrix
	 *
	 * @return Effectiveness matrix
	 */
	અચલ matrix::Matrix<દશાંશ, અંક_AXES, અંક_ACTUATORS> &getEffectivenessMatrix() અચલ { પાછા _effectiveness; }

	/**
	 * Set the minimum actuator values
	 *
	 * @param actuator_min Minimum actuator values
	 */
	શૂન્ય setActuatorMin(અચલ ActuatorVector &actuator_min) { _actuator_min = actuator_min; }

	/**
	 * Get the minimum actuator values
	 *
	 * @return Minimum actuator values
	 */
	અચલ ActuatorVector &getActuatorMin() અચલ { પાછા _actuator_min; }

	/**
	 * Set the maximum actuator values
	 *
	 * @param actuator_max Maximum actuator values
	 */
	શૂન્ય setActuatorMax(અચલ ActuatorVector &actuator_max) { _actuator_max = actuator_max; }

	/**
	 * Get the maximum actuator values
	 *
	 * @return Maximum actuator values
	 */
	અચલ ActuatorVector &getActuatorMax() અચલ { પાછા _actuator_max; }

	/**
	 * Set the current actuator setpoint.
	 *
	 * Use this when a new allocation method is started to initialize it properly.
	 * In most cases, it is not needed to call this method before `allocate()`.
	 * Indeed the previous actuator setpoint is expected to be stored during calls to `allocate()`.
	 *
	 * @param actuator_sp Actuator setpoint
	 */
	શૂન્ય setActuatorSetpoint(અચલ ActuatorVector &actuator_sp);

	શૂન્ય setSlewRateLimit(અચલ ActuatorVector &slew_rate_limit)
	{ _actuator_slew_rate_limit = slew_rate_limit; }

	/**
	 * Apply slew rate to current actuator setpoint
	 */
	શૂન્ય applySlewRateLimit(દશાંશ dt);

	/**
	 * Clip the actuator setpoint between minimum and maximum values.
	 *
	 * The output is in the range [min; max]
	 *
	 * @param actuator Actuator vector to clip
	 */
	શૂન્ય clipActuatorSetpoint(ActuatorVector &actuator) અચલ;

	શૂન્ય clipActuatorSetpoint() { clipActuatorSetpoint(_actuator_sp); }

	/**
	 * Normalize the actuator setpoint between minimum and maximum values.
	 *
	 * The output is in the range [-1; +1]
	 *
	 * @param actuator Actuator vector to normalize
	 *
	 * @return Clipped actuator setpoint
	 */
	ActuatorVector normalizeActuatorSetpoint(અચલ ActuatorVector &actuator)
	અચલ;

	/**
	 * Apply a mask of actuators to be set to NaN.
	 *
	 * A NaN value in _actuator_sp represents a disabled or stopped actuator.
	 * This mask is typically used to stop motors in specific flight phases or when certain thrust components are NaN.
	 *
	 * @param nan_actuators_mask Bitmask indicating which actuators to set to NaN.
	 *                           If (nan_actuators_mask & (1 << i)), _actuator_sp(i) becomes NaN.
	 */
	શૂન્ય applyNanToActuators(ActuatorBitmask nan_actuators_mask)
	{
		માટે (પૂર્ણાંક i = 0; i < અંક_actuators_2; i++) {
			જો (nan_actuators_mask & (1u << i)) {
				_actuator_sp(i) = NAN;
			}
		}
	}

	આભાસી શૂન્ય updateParameters() {}

	પૂર્ણાંક અંક_Configured_Actuators() અચલ { પાછા અંક_actuators_2; }

	શૂન્ય setNormalizeRPY(તાર્કિક normalize_rpy) { _normalize_rpy = normalize_rpy; }

	/// Axes (bitmask over ControlAxis) dropped from the effectiveness matrix as not independently
	/// achievable, e.g. yaw when collinear with roll after a motor failure
	uint8_t getDroppedAxes() અચલ { પાછા _dropped_axes; }

સુરક્ષિત:
	friend વર્ગ ControlAllocator; // for _actuator_sp

	matrix::Matrix<દશાંશ, અંક_AXES, અંક_ACTUATORS> _effectiveness;  ///< Effectiveness matrix
	matrix::Vector<દશાંશ, અંક_AXES> _control_allocation_scale;  	///< Scaling applied during allocation
	ActuatorVector _actuator_trim; 	///< Neutral actuator values
	ActuatorVector _actuator_min; 	///< Minimum actuator values
	ActuatorVector _actuator_max; 	///< Maximum actuator values
	ActuatorVector _actuator_slew_rate_limit; 	///< Slew rate limit
	ActuatorVector _prev_actuator_sp;  	///< Previous actuator setpoint
	ActuatorVector _actuator_sp;  	///< Actuator setpoint
	matrix::Vector<દશાંશ, અંક_AXES> _control_sp;   		///< Control setpoint
	matrix::Vector<દશાંશ, અંક_AXES> _control_trim; 		///< Control at trim actuator values
	પૂર્ણાંક અંક_actuators_2{0};
	તાર્કિક _normalize_rpy{ખોટું};				///< if true, normalize roll, pitch and yaw columns
	તાર્કિક _had_actuator_failure{ખોટું};
	uint8_t _dropped_axes{0};				///< bitmask over ControlAxis
};

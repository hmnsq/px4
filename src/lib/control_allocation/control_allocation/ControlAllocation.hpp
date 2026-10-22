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

#ଅନ୍ତର୍ଭାଵ <matrix/matrix/math.hpp>

#ଅନ୍ତର୍ଭାଵ "control_allocation/actuator_effectiveness/ActuatorEffectiveness.hpp"

ଶ୍ରେଣୀ ControlAllocation
{
ସର୍ବସାଧାରଣ:
	ControlAllocation();
	ଆଭାସୀ ~ControlAllocation() = ପୂର୍ବନିର୍ଦ୍ଧାରିତ;

	ସ୍ଥିର constexpr uint8_t ଅଙ୍କ_ACTUATORS = ActuatorEffectiveness::ଅଙ୍କ_ACTUATORS;
	ସ୍ଥିର constexpr uint8_t ଅଙ୍କ_AXES = ActuatorEffectiveness::ଅଙ୍କ_AXES;

	using ActuatorVector = matrix::Vector<ଦଶମିକ, ଅଙ୍କ_ACTUATORS>;
	using ActuatorBitmask = ActuatorEffectiveness::ActuatorBitmask;

	ଗଣନା ControlAxis {
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
	ଆଭାସୀ ଶୂନ୍ୟ allocate() = 0;

	/**
	 * Set actuator failure flag
	 * This prevents a change of the scaling in the matrix normalization step
	 * in case of a motor failure.
	 *
	 * @param failure  Motor failure flag
	 */
	ଶୂନ୍ୟ setHadActuatorFailure(ତାର୍କିକ failure) { _had_actuator_failure = failure; }

	/**
	 * Set the control effectiveness matrix
	 *
	 * @param B Effectiveness matrix
	 */
	ଆଭାସୀ ଶୂନ୍ୟ setEffectivenessMatrix(ଅଚଳ matrix::Matrix<ଦଶମିକ, ଅଙ୍କ_AXES, ଅଙ୍କ_ACTUATORS> &effectiveness,
					    ଅଚଳ ActuatorVector &actuator_trim, ଅଚଳ ActuatorVector &linearization_point, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଅଙ୍କ_actuators,
					    ତାର୍କିକ update_normalization_scale);

	/**
	 * Get the allocated actuator vector
	 *
	 * @return Actuator vector
	 */
	ଅଚଳ ActuatorVector &getActuatorSetpoint() ଅଚଳ { ଫେରନ୍ତୁ _actuator_sp; }

	/**
	 * Set the desired control vector
	 *
	 * @param Control vector
	 */
	ଶୂନ୍ୟ setControlSetpoint(ଅଚଳ matrix::Vector<ଦଶମିକ, ଅଙ୍କ_AXES> &control) { _control_sp = control; }

	/**
	 * Get the desired control vector
	 *
	 * @return Control vector
	 */
	ଅଚଳ matrix::Vector<ଦଶମିକ, ଅଙ୍କ_AXES> &getControlSetpoint() ଅଚଳ { ଫେରନ୍ତୁ _control_sp; }

	/**
	 * Get the allocated control vector
	 *
	 * @return Control vector
	 */
	matrix::Vector<ଦଶମିକ, ଅଙ୍କ_AXES> getAllocatedControl() ଅଚଳ
	{
		// _actuator_sp contains NaN to indicate stopped motors.
		// Covert back to zero to represent physical thrust.
		ActuatorVector actuator_sp{_actuator_sp};
		actuator_sp.nanToZero();
		ଫେରନ୍ତୁ (_effectiveness * (actuator_sp - _actuator_trim)).emult(_control_allocation_scale);
	}

	/**
	 * Get the control effectiveness matrix
	 *
	 * @return Effectiveness matrix
	 */
	ଅଚଳ matrix::Matrix<ଦଶମିକ, ଅଙ୍କ_AXES, ଅଙ୍କ_ACTUATORS> &getEffectivenessMatrix() ଅଚଳ { ଫେରନ୍ତୁ _effectiveness; }

	/**
	 * Set the minimum actuator values
	 *
	 * @param actuator_min Minimum actuator values
	 */
	ଶୂନ୍ୟ setActuatorMin(ଅଚଳ ActuatorVector &actuator_min) { _actuator_min = actuator_min; }

	/**
	 * Get the minimum actuator values
	 *
	 * @return Minimum actuator values
	 */
	ଅଚଳ ActuatorVector &getActuatorMin() ଅଚଳ { ଫେରନ୍ତୁ _actuator_min; }

	/**
	 * Set the maximum actuator values
	 *
	 * @param actuator_max Maximum actuator values
	 */
	ଶୂନ୍ୟ setActuatorMax(ଅଚଳ ActuatorVector &actuator_max) { _actuator_max = actuator_max; }

	/**
	 * Get the maximum actuator values
	 *
	 * @return Maximum actuator values
	 */
	ଅଚଳ ActuatorVector &getActuatorMax() ଅଚଳ { ଫେରନ୍ତୁ _actuator_max; }

	/**
	 * Set the current actuator setpoint.
	 *
	 * Use this when a new allocation method is started to initialize it properly.
	 * In most cases, it is not needed to call this method before `allocate()`.
	 * Indeed the previous actuator setpoint is expected to be stored during calls to `allocate()`.
	 *
	 * @param actuator_sp Actuator setpoint
	 */
	ଶୂନ୍ୟ setActuatorSetpoint(ଅଚଳ ActuatorVector &actuator_sp);

	ଶୂନ୍ୟ setSlewRateLimit(ଅଚଳ ActuatorVector &slew_rate_limit)
	{ _actuator_slew_rate_limit = slew_rate_limit; }

	/**
	 * Apply slew rate to current actuator setpoint
	 */
	ଶୂନ୍ୟ applySlewRateLimit(ଦଶମିକ dt);

	/**
	 * Clip the actuator setpoint between minimum and maximum values.
	 *
	 * The output is in the range [min; max]
	 *
	 * @param actuator Actuator vector to clip
	 */
	ଶୂନ୍ୟ clipActuatorSetpoint(ActuatorVector &actuator) ଅଚଳ;

	ଶୂନ୍ୟ clipActuatorSetpoint() { clipActuatorSetpoint(_actuator_sp); }

	/**
	 * Normalize the actuator setpoint between minimum and maximum values.
	 *
	 * The output is in the range [-1; +1]
	 *
	 * @param actuator Actuator vector to normalize
	 *
	 * @return Clipped actuator setpoint
	 */
	ActuatorVector normalizeActuatorSetpoint(ଅଚଳ ActuatorVector &actuator)
	ଅଚଳ;

	/**
	 * Apply a mask of actuators to be set to NaN.
	 *
	 * A NaN value in _actuator_sp represents a disabled or stopped actuator.
	 * This mask is typically used to stop motors in specific flight phases or when certain thrust components are NaN.
	 *
	 * @param nan_actuators_mask Bitmask indicating which actuators to set to NaN.
	 *                           If (nan_actuators_mask & (1 << i)), _actuator_sp(i) becomes NaN.
	 */
	ଶୂନ୍ୟ applyNanToActuators(ActuatorBitmask nan_actuators_mask)
	{
		ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < ଅଙ୍କ_actuators_2; i++) {
			ଯଦି (nan_actuators_mask & (1u << i)) {
				_actuator_sp(i) = NAN;
			}
		}
	}

	ଆଭାସୀ ଶୂନ୍ୟ updateParameters() {}

	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଅଙ୍କ_Configured_Actuators() ଅଚଳ { ଫେରନ୍ତୁ ଅଙ୍କ_actuators_2; }

	ଶୂନ୍ୟ setNormalizeRPY(ତାର୍କିକ normalize_rpy) { _normalize_rpy = normalize_rpy; }

	/// Axes (bitmask over ControlAxis) dropped from the effectiveness matrix as not independently
	/// achievable, e.g. yaw when collinear with roll after a motor failure
	uint8_t getDroppedAxes() ଅଚଳ { ଫେରନ୍ତୁ _dropped_axes; }

ସୁରକ୍ଷିତ:
	friend ଶ୍ରେଣୀ ControlAllocator; // for _actuator_sp

	matrix::Matrix<ଦଶମିକ, ଅଙ୍କ_AXES, ଅଙ୍କ_ACTUATORS> _effectiveness;  ///< Effectiveness matrix
	matrix::Vector<ଦଶମିକ, ଅଙ୍କ_AXES> _control_allocation_scale;  	///< Scaling applied during allocation
	ActuatorVector _actuator_trim; 	///< Neutral actuator values
	ActuatorVector _actuator_min; 	///< Minimum actuator values
	ActuatorVector _actuator_max; 	///< Maximum actuator values
	ActuatorVector _actuator_slew_rate_limit; 	///< Slew rate limit
	ActuatorVector _prev_actuator_sp;  	///< Previous actuator setpoint
	ActuatorVector _actuator_sp;  	///< Actuator setpoint
	matrix::Vector<ଦଶମିକ, ଅଙ୍କ_AXES> _control_sp;   		///< Control setpoint
	matrix::Vector<ଦଶମିକ, ଅଙ୍କ_AXES> _control_trim; 		///< Control at trim actuator values
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଅଙ୍କ_actuators_2{0};
	ତାର୍କିକ _normalize_rpy{ମିଥ୍ୟା};				///< if true, normalize roll, pitch and yaw columns
	ତାର୍କିକ _had_actuator_failure{ମିଥ୍ୟା};
	uint8_t _dropped_axes{0};				///< bitmask over ControlAxis
};

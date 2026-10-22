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

#निर्देशः once

#अन्तर्भाव <matrix/matrix/math.hpp>

#अन्तर्भाव "control_allocation/actuator_effectiveness/ActuatorEffectiveness.hpp"

वर्ग Control_आवण्टनम्
{
सार्वजनिक:
	Control_आवण्टनम्();
	आभास ~Control_आवण्टनम्() = स्वभाव;

	स्थिर constexpr uint8_t अङ्कः_ACTUATORS = ActuatorEffectiveness::अङ्कः_ACTUATORS;
	स्थिर constexpr uint8_t अङ्कः_AXES = ActuatorEffectiveness::अङ्कः_AXES;

	उपयुज्य Actuator_सदिशम् = matrix::सदिशम्<भिन्न, अङ्कः_ACTUATORS>;
	उपयुज्य ActuatorBitmask = ActuatorEffectiveness::ActuatorBitmask;

	गणना ControlAxis {
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
	आभास शून्य allocate() = 0;

	/**
	 * Set actuator failure flag
	 * This prevents a change of the scaling in the matrix normalization step
	 * in case of a motor failure.
	 *
	 * @param failure  Motor failure flag
	 */
	शून्य समुच्चयः_Had_Actuator_Failure(तार्किक failure) { _had_actuator_failure = failure; }

	/**
	 * Set the control effectiveness matrix
	 *
	 * @param B Effectiveness matrix
	 */
	आभास शून्य समुच्चयः_Effectiveness_Matrix(ध्रुव matrix::Matrix<भिन्न, अङ्कः_AXES, अङ्कः_ACTUATORS> &effectiveness,
					    ध्रुव Actuator_सदिशम् &actuator_trim, ध्रुव Actuator_सदिशम् &linearization_point, पूर्णाङ्क अङ्कः_actuators,
					    तार्किक update_normalization_scale);

	/**
	 * Get the allocated actuator vector
	 *
	 * @return Actuator vector
	 */
	ध्रुव Actuator_सदिशम् &getActuatorSetpoint() ध्रुव { प्रत्यागम _actuator_sp; }

	/**
	 * Set the desired control vector
	 *
	 * @param Control vector
	 */
	शून्य समुच्चयः_Control_Setpoint(ध्रुव matrix::सदिशम्<भिन्न, अङ्कः_AXES> &control) { _control_sp = control; }

	/**
	 * Get the desired control vector
	 *
	 * @return Control vector
	 */
	ध्रुव matrix::सदिशम्<भिन्न, अङ्कः_AXES> &getControlSetpoint() ध्रुव { प्रत्यागम _control_sp; }

	/**
	 * Get the allocated control vector
	 *
	 * @return Control vector
	 */
	matrix::सदिशम्<भिन्न, अङ्कः_AXES> getAllocatedControl() ध्रुव
	{
		// _actuator_sp contains NaN to indicate stopped motors.
		// Covert back to zero to represent physical thrust.
		Actuator_सदिशम् actuator_sp{_actuator_sp};
		actuator_sp.nan_गन्तव्यम्_Zero();
		प्रत्यागम (_effectiveness * (actuator_sp - _actuator_trim)).emult(control_आवण्टनम्_scale);
	}

	/**
	 * Get the control effectiveness matrix
	 *
	 * @return Effectiveness matrix
	 */
	ध्रुव matrix::Matrix<भिन्न, अङ्कः_AXES, अङ्कः_ACTUATORS> &getEffectivenessMatrix() ध्रुव { प्रत्यागम _effectiveness; }

	/**
	 * Set the minimum actuator values
	 *
	 * @param actuator_min Minimum actuator values
	 */
	शून्य समुच्चयः_Actuator_Min(ध्रुव Actuator_सदिशम् &actuator_min) { _actuator_min = actuator_min; }

	/**
	 * Get the minimum actuator values
	 *
	 * @return Minimum actuator values
	 */
	ध्रुव Actuator_सदिशम् &getActuatorMin() ध्रुव { प्रत्यागम _actuator_min; }

	/**
	 * Set the maximum actuator values
	 *
	 * @param actuator_max Maximum actuator values
	 */
	शून्य समुच्चयः_Actuator_Max(ध्रुव Actuator_सदिशम् &actuator_max) { _actuator_max = actuator_max; }

	/**
	 * Get the maximum actuator values
	 *
	 * @return Maximum actuator values
	 */
	ध्रुव Actuator_सदिशम् &getActuatorMax() ध्रुव { प्रत्यागम _actuator_max; }

	/**
	 * Set the current actuator setpoint.
	 *
	 * Use this when a new allocation method is started to initialize it properly.
	 * In most cases, it is not needed to call this method before `allocate()`.
	 * Indeed the previous actuator setpoint is expected to be stored during calls to `allocate()`.
	 *
	 * @param actuator_sp Actuator setpoint
	 */
	शून्य समुच्चयः_Actuator_Setpoint(ध्रुव Actuator_सदिशम् &actuator_sp);

	शून्य समुच्चयः_Slew_दरः_सीमा(ध्रुव Actuator_सदिशम् &slew_दरः_सीमा)
	{ actuator_slew_दरः_सीमा = slew_दरः_सीमा; }

	/**
	 * Apply slew rate to current actuator setpoint
	 */
	शून्य apply_Slew_दरः_सीमा(भिन्न dt);

	/**
	 * Clip the actuator setpoint between minimum and maximum values.
	 *
	 * The output is in the range [min; max]
	 *
	 * @param actuator Actuator vector to clip
	 */
	शून्य clipActuatorSetpoint(Actuator_सदिशम् &actuator) ध्रुव;

	शून्य clipActuatorSetpoint() { clipActuatorSetpoint(_actuator_sp); }

	/**
	 * Normalize the actuator setpoint between minimum and maximum values.
	 *
	 * The output is in the range [-1; +1]
	 *
	 * @param actuator Actuator vector to normalize
	 *
	 * @return Clipped actuator setpoint
	 */
	Actuator_सदिशम् normalizeActuatorSetpoint(ध्रुव Actuator_सदिशम् &actuator)
	ध्रुव;

	/**
	 * Apply a mask of actuators to be set to NaN.
	 *
	 * A NaN value in _actuator_sp represents a disabled or stopped actuator.
	 * This mask is typically used to stop motors in specific flight phases or when certain thrust components are NaN.
	 *
	 * @param nan_actuators_mask Bitmask indicating which actuators to set to NaN.
	 *                           If (nan_actuators_mask & (1 << i)), _actuator_sp(i) becomes NaN.
	 */
	शून्य apply_Nan_गन्तव्यम्_Actuators(ActuatorBitmask nan_actuators_आवरणम्)
	{
		प्रति (पूर्णाङ्क i = 0; i < अङ्कः_actuators_2; i++) {
			यदि (nan_actuators_आवरणम् & (1u << i)) {
				_actuator_sp(i) = NAN;
			}
		}
	}

	आभास शून्य updateParameters() {}

	पूर्णाङ्क अङ्कः_Configured_Actuators() ध्रुव { प्रत्यागम अङ्कः_actuators_2; }

	शून्य समुच्चयः_Normalize_RPY(तार्किक normalize_rpy) { _normalize_rpy = normalize_rpy; }

	/// Axes (bitmask over ControlAxis) dropped from the effectiveness matrix as not independently
	/// achievable, e.g. yaw when collinear with roll after a motor failure
	uint8_t getDroppedAxes() ध्रुव { प्रत्यागम _dropped_axes; }

रक्षित:
	मित्रम् वर्ग ControlAllocator; // for _actuator_sp

	matrix::Matrix<भिन्न, अङ्कः_AXES, अङ्कः_ACTUATORS> _effectiveness;  ///< Effectiveness matrix
	matrix::सदिशम्<भिन्न, अङ्कः_AXES> control_आवण्टनम्_scale;  	///< Scaling applied during allocation
	Actuator_सदिशम् _actuator_trim; 	///< Neutral actuator values
	Actuator_सदिशम् _actuator_min; 	///< Minimum actuator values
	Actuator_सदिशम् _actuator_max; 	///< Maximum actuator values
	Actuator_सदिशम् actuator_slew_दरः_सीमा; 	///< Slew rate limit
	Actuator_सदिशम् पूर्व_actuator_sp;  	///< Previous actuator setpoint
	Actuator_सदिशम् _actuator_sp;  	///< Actuator setpoint
	matrix::सदिशम्<भिन्न, अङ्कः_AXES> _control_sp;   		///< Control setpoint
	matrix::सदिशम्<भिन्न, अङ्कः_AXES> _control_trim; 		///< Control at trim actuator values
	पूर्णाङ्क अङ्कः_actuators_2{0};
	तार्किक _normalize_rpy{असत्य};				///< if true, normalize roll, pitch and yaw columns
	तार्किक _had_actuator_failure{असत्य};
	uint8_t _dropped_axes{0};				///< bitmask over ControlAxis
};

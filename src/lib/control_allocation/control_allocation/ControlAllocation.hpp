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

#لارښوونه once

#ضمن <matrix/matrix/math.hpp>

#ضمن "control_allocation/actuator_effectiveness/ActuatorEffectiveness.hpp"

صنف Control_ویش
{
عام:
	Control_ویش();
	مجازي ~Control_ویش() = افتراضي;

	ساكن constexpr uint8_t عدد_ACTUATORS = ActuatorEffectiveness::عدد_ACTUATORS;
	ساكن constexpr uint8_t عدد_AXES = ActuatorEffectiveness::عدد_AXES;

	کارول Actuator_ویکټور = ماتریکس::ویکټور<عائم, عدد_ACTUATORS>;
	کارول ActuatorBitmask = ActuatorEffectiveness::ActuatorBitmask;

	تعداد ControlAxis {
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
	مجازي فراغ allocate() = 0;

	/**
	 * Set actuator failure flag
	 * This prevents a change of the scaling in the matrix normalization step
	 * in case of a motor failure.
	 *
	 * @param failure  Motor failure flag
	 */
	فراغ سېټ_Had_Actuator_Failure(منطقي failure) { _had_actuator_failure = failure; }

	/**
	 * Set the control effectiveness matrix
	 *
	 * @param B Effectiveness matrix
	 */
	مجازي فراغ سېټ_Effectiveness_ماتریکس(ثابت ماتریکس::ماتریکس_2<عائم, عدد_AXES, عدد_ACTUATORS> &effectiveness,
					    ثابت Actuator_ویکټور &actuator_trim, ثابت Actuator_ویکټور &linearization_ټکی, صحيح عدد_actuators,
					    منطقي update_normalization_scale);

	/**
	 * Get the allocated actuator vector
	 *
	 * @return Actuator vector
	 */
	ثابت Actuator_ویکټور &getActuatorSetpoint() ثابت { أرجع _actuator_sp; }

	/**
	 * Set the desired control vector
	 *
	 * @param Control vector
	 */
	فراغ سېټ_Control_Setpoint(ثابت ماتریکس::ویکټور<عائم, عدد_AXES> &control) { _control_sp = control; }

	/**
	 * Get the desired control vector
	 *
	 * @return Control vector
	 */
	ثابت ماتریکس::ویکټور<عائم, عدد_AXES> &getControlSetpoint() ثابت { أرجع _control_sp; }

	/**
	 * Get the allocated control vector
	 *
	 * @return Control vector
	 */
	ماتریکس::ویکټور<عائم, عدد_AXES> getAllocatedControl() ثابت
	{
		// _actuator_sp contains NaN to indicate stopped motors.
		// Covert back to zero to represent physical thrust.
		Actuator_ویکټور actuator_sp{_actuator_sp};
		actuator_sp.nan_منزل_Zero();
		أرجع (_effectiveness * (actuator_sp - _actuator_trim)).emult(control_ویش_scale);
	}

	/**
	 * Get the control effectiveness matrix
	 *
	 * @return Effectiveness matrix
	 */
	ثابت ماتریکس::ماتریکس_2<عائم, عدد_AXES, عدد_ACTUATORS> &get_Effectiveness_ماتریکس() ثابت { أرجع _effectiveness; }

	/**
	 * Set the minimum actuator values
	 *
	 * @param actuator_min Minimum actuator values
	 */
	فراغ سېټ_Actuator_Min(ثابت Actuator_ویکټور &actuator_min) { _actuator_min = actuator_min; }

	/**
	 * Get the minimum actuator values
	 *
	 * @return Minimum actuator values
	 */
	ثابت Actuator_ویکټور &getActuatorMin() ثابت { أرجع _actuator_min; }

	/**
	 * Set the maximum actuator values
	 *
	 * @param actuator_max Maximum actuator values
	 */
	فراغ سېټ_Actuator_Max(ثابت Actuator_ویکټور &actuator_max) { _actuator_max = actuator_max; }

	/**
	 * Get the maximum actuator values
	 *
	 * @return Maximum actuator values
	 */
	ثابت Actuator_ویکټور &getActuatorMax() ثابت { أرجع _actuator_max; }

	/**
	 * Set the current actuator setpoint.
	 *
	 * Use this when a new allocation method is started to initialize it properly.
	 * In most cases, it is not needed to call this method before `allocate()`.
	 * Indeed the previous actuator setpoint is expected to be stored during calls to `allocate()`.
	 *
	 * @param actuator_sp Actuator setpoint
	 */
	فراغ سېټ_Actuator_Setpoint(ثابت Actuator_ویکټور &actuator_sp);

	فراغ سېټ_Slew_شرح_حد(ثابت Actuator_ویکټور &slew_شرح_حد)
	{ actuator_slew_شرح_حد = slew_شرح_حد; }

	/**
	 * Apply slew rate to current actuator setpoint
	 */
	فراغ apply_Slew_شرح_حد(عائم dt);

	/**
	 * Clip the actuator setpoint between minimum and maximum values.
	 *
	 * The output is in the range [min; max]
	 *
	 * @param actuator Actuator vector to clip
	 */
	فراغ clipActuatorSetpoint(Actuator_ویکټور &actuator) ثابت;

	فراغ clipActuatorSetpoint() { clipActuatorSetpoint(_actuator_sp); }

	/**
	 * Normalize the actuator setpoint between minimum and maximum values.
	 *
	 * The output is in the range [-1; +1]
	 *
	 * @param actuator Actuator vector to normalize
	 *
	 * @return Clipped actuator setpoint
	 */
	Actuator_ویکټور normalizeActuatorSetpoint(ثابت Actuator_ویکټور &actuator)
	ثابت;

	/**
	 * Apply a mask of actuators to be set to NaN.
	 *
	 * A NaN value in _actuator_sp represents a disabled or stopped actuator.
	 * This mask is typically used to stop motors in specific flight phases or when certain thrust components are NaN.
	 *
	 * @param nan_actuators_mask Bitmask indicating which actuators to set to NaN.
	 *                           If (nan_actuators_mask & (1 << i)), _actuator_sp(i) becomes NaN.
	 */
	فراغ apply_Nan_منزل_Actuators(ActuatorBitmask nan_actuators_ماسک)
	{
		لكل (صحيح i = 0; i < عدد_actuators_2; i++) {
			إذا (nan_actuators_ماسک & (1u << i)) {
				_actuator_sp(i) = NAN;
			}
		}
	}

	مجازي فراغ updateParameters() {}

	صحيح عدد_Configured_Actuators() ثابت { أرجع عدد_actuators_2; }

	فراغ سېټ_Normalize_RPY(منطقي normalize_rpy) { _normalize_rpy = normalize_rpy; }

	/// Axes (bitmask over ControlAxis) dropped from the effectiveness matrix as not independently
	/// achievable, e.g. yaw when collinear with roll after a motor failure
	uint8_t getDroppedAxes() ثابت { أرجع _dropped_axes; }

محمي:
	ملګری صنف ControlAllocator; // for _actuator_sp

	ماتریکس::ماتریکس_2<عائم, عدد_AXES, عدد_ACTUATORS> _effectiveness;  ///< Effectiveness matrix
	ماتریکس::ویکټور<عائم, عدد_AXES> control_ویش_scale;  	///< Scaling applied during allocation
	Actuator_ویکټور _actuator_trim; 	///< Neutral actuator values
	Actuator_ویکټور _actuator_min; 	///< Minimum actuator values
	Actuator_ویکټور _actuator_max; 	///< Maximum actuator values
	Actuator_ویکټور actuator_slew_شرح_حد; 	///< Slew rate limit
	Actuator_ویکټور _prev_actuator_sp;  	///< Previous actuator setpoint
	Actuator_ویکټور _actuator_sp;  	///< Actuator setpoint
	ماتریکس::ویکټور<عائم, عدد_AXES> _control_sp;   		///< Control setpoint
	ماتریکس::ویکټور<عائم, عدد_AXES> _control_trim; 		///< Control at trim actuator values
	صحيح عدد_actuators_2{0};
	منطقي _normalize_rpy{خطأ};				///< if true, normalize roll, pitch and yaw columns
	منطقي _had_actuator_failure{خطأ};
	uint8_t _dropped_axes{0};				///< bitmask over ControlAxis
};

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

#ہدایت once

#ضمن <matrix/matrix/math.hpp>

#ضمن "control_allocation/actuator_effectiveness/ActuatorEffectiveness.hpp"

صنف Control_ونڈ
{
عام:
	Control_ونڈ();
	مجازی ~Control_ونڈ() = افتراضي;

	ساكن constexpr uint8_t عدد_ACTUATORS = ActuatorEffectiveness::عدد_ACTUATORS;
	ساكن constexpr uint8_t عدد_AXES = ActuatorEffectiveness::عدد_AXES;

	ورتوں Actuator_سمتیہ = میٹرکس::سمتیہ<عائم, عدد_ACTUATORS>;
	ورتوں ActuatorBitmask = ActuatorEffectiveness::ActuatorBitmask;

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
	مجازی فراغ allocate() = 0;

	/**
	 * Set actuator failure flag
	 * This prevents a change of the scaling in the matrix normalization step
	 * in case of a motor failure.
	 *
	 * @param failure  Motor failure flag
	 */
	فراغ سیٹ_Had_Actuator_Failure(منطقي failure) { _had_actuator_failure = failure; }

	/**
	 * Set the control effectiveness matrix
	 *
	 * @param B Effectiveness matrix
	 */
	مجازی فراغ سیٹ_Effectiveness_میٹرکس(ثابت میٹرکس::میٹرکس_2<عائم, عدد_AXES, عدد_ACTUATORS> &effectiveness,
					    ثابت Actuator_سمتیہ &actuator_trim, ثابت Actuator_سمتیہ &linearization_نقطہ, صحيح عدد_actuators,
					    منطقي update_normalization_scale);

	/**
	 * Get the allocated actuator vector
	 *
	 * @return Actuator vector
	 */
	ثابت Actuator_سمتیہ &getActuatorSetpoint() ثابت { أرجع _actuator_sp; }

	/**
	 * Set the desired control vector
	 *
	 * @param Control vector
	 */
	فراغ سیٹ_Control_Setpoint(ثابت میٹرکس::سمتیہ<عائم, عدد_AXES> &control) { _control_sp = control; }

	/**
	 * Get the desired control vector
	 *
	 * @return Control vector
	 */
	ثابت میٹرکس::سمتیہ<عائم, عدد_AXES> &getControlSetpoint() ثابت { أرجع _control_sp; }

	/**
	 * Get the allocated control vector
	 *
	 * @return Control vector
	 */
	میٹرکس::سمتیہ<عائم, عدد_AXES> getAllocatedControl() ثابت
	{
		// _actuator_sp contains NaN to indicate stopped motors.
		// Covert back to zero to represent physical thrust.
		Actuator_سمتیہ actuator_sp{_actuator_sp};
		actuator_sp.nan_منزل_Zero();
		أرجع (_effectiveness * (actuator_sp - _actuator_trim)).emult(control_ونڈ_scale);
	}

	/**
	 * Get the control effectiveness matrix
	 *
	 * @return Effectiveness matrix
	 */
	ثابت میٹرکس::میٹرکس_2<عائم, عدد_AXES, عدد_ACTUATORS> &get_Effectiveness_میٹرکس() ثابت { أرجع _effectiveness; }

	/**
	 * Set the minimum actuator values
	 *
	 * @param actuator_min Minimum actuator values
	 */
	فراغ سیٹ_Actuator_Min(ثابت Actuator_سمتیہ &actuator_min) { _actuator_min = actuator_min; }

	/**
	 * Get the minimum actuator values
	 *
	 * @return Minimum actuator values
	 */
	ثابت Actuator_سمتیہ &getActuatorMin() ثابت { أرجع _actuator_min; }

	/**
	 * Set the maximum actuator values
	 *
	 * @param actuator_max Maximum actuator values
	 */
	فراغ سیٹ_Actuator_Max(ثابت Actuator_سمتیہ &actuator_max) { _actuator_max = actuator_max; }

	/**
	 * Get the maximum actuator values
	 *
	 * @return Maximum actuator values
	 */
	ثابت Actuator_سمتیہ &getActuatorMax() ثابت { أرجع _actuator_max; }

	/**
	 * Set the current actuator setpoint.
	 *
	 * Use this when a new allocation method is started to initialize it properly.
	 * In most cases, it is not needed to call this method before `allocate()`.
	 * Indeed the previous actuator setpoint is expected to be stored during calls to `allocate()`.
	 *
	 * @param actuator_sp Actuator setpoint
	 */
	فراغ سیٹ_Actuator_Setpoint(ثابت Actuator_سمتیہ &actuator_sp);

	فراغ سیٹ_Slew_شرح_حد_بندی(ثابت Actuator_سمتیہ &slew_شرح_حد_بندی)
	{ actuator_slew_شرح_حد_بندی = slew_شرح_حد_بندی; }

	/**
	 * Apply slew rate to current actuator setpoint
	 */
	فراغ apply_Slew_شرح_حد_بندی(عائم dt);

	/**
	 * Clip the actuator setpoint between minimum and maximum values.
	 *
	 * The output is in the range [min; max]
	 *
	 * @param actuator Actuator vector to clip
	 */
	فراغ clipActuatorSetpoint(Actuator_سمتیہ &actuator) ثابت;

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
	Actuator_سمتیہ normalizeActuatorSetpoint(ثابت Actuator_سمتیہ &actuator)
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
	فراغ apply_Nan_منزل_Actuators(ActuatorBitmask nan_actuators_نقاب)
	{
		لكل (صحيح i = 0; i < عدد_actuators_2; i++) {
			إذا (nan_actuators_نقاب & (1u << i)) {
				_actuator_sp(i) = NAN;
			}
		}
	}

	مجازی فراغ updateParameters() {}

	صحيح عدد_Configured_Actuators() ثابت { أرجع عدد_actuators_2; }

	فراغ سیٹ_Normalize_RPY(منطقي normalize_rpy) { _normalize_rpy = normalize_rpy; }

	/// Axes (bitmask over ControlAxis) dropped from the effectiveness matrix as not independently
	/// achievable, e.g. yaw when collinear with roll after a motor failure
	uint8_t getDroppedAxes() ثابت { أرجع _dropped_axes; }

محمي:
	یار صنف ControlAllocator; // for _actuator_sp

	میٹرکس::میٹرکس_2<عائم, عدد_AXES, عدد_ACTUATORS> _effectiveness;  ///< Effectiveness matrix
	میٹرکس::سمتیہ<عائم, عدد_AXES> control_ونڈ_scale;  	///< Scaling applied during allocation
	Actuator_سمتیہ _actuator_trim; 	///< Neutral actuator values
	Actuator_سمتیہ _actuator_min; 	///< Minimum actuator values
	Actuator_سمتیہ _actuator_max; 	///< Maximum actuator values
	Actuator_سمتیہ actuator_slew_شرح_حد_بندی; 	///< Slew rate limit
	Actuator_سمتیہ _prev_actuator_sp;  	///< Previous actuator setpoint
	Actuator_سمتیہ _actuator_sp;  	///< Actuator setpoint
	میٹرکس::سمتیہ<عائم, عدد_AXES> _control_sp;   		///< Control setpoint
	میٹرکس::سمتیہ<عائم, عدد_AXES> _control_trim; 		///< Control at trim actuator values
	صحيح عدد_actuators_2{0};
	منطقي _normalize_rpy{خطأ};				///< if true, normalize roll, pitch and yaw columns
	منطقي _had_actuator_failure{خطأ};
	uint8_t _dropped_axes{0};				///< bitmask over ControlAxis
};

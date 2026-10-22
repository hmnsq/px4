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

#సూచన once

#అన్తర్భావ <matrix/matrix/math.hpp>

#అన్తర్భావ "control_allocation/actuator_effectiveness/ActuatorEffectiveness.hpp"

తరగతి Control_కేటాయింపు
{
బహిరంగ:
	Control_కేటాయింపు();
	మాయా ~Control_కేటాయింపు() = అప్రమేయ;

	స్థిర constexpr uint8_t అంకె_ACTUATORS = ActuatorEffectiveness::అంకె_ACTUATORS;
	స్థిర constexpr uint8_t అంకె_AXES = ActuatorEffectiveness::అంకె_AXES;

	ఉపయోగించి Actuator_సదిశ = మాతృక::సదిశ<దశాంశం, అంకె_ACTUATORS>;
	ఉపయోగించి ActuatorBitmask = ActuatorEffectiveness::ActuatorBitmask;

	గణన ControlAxis {
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
	మాయా శూన్య allocate() = 0;

	/**
	 * Set actuator failure flag
	 * This prevents a change of the scaling in the matrix normalization step
	 * in case of a motor failure.
	 *
	 * @param failure  Motor failure flag
	 */
	శూన్య సమితి_Had_Actuator_Failure(తార్కిక failure) { _had_actuator_failure = failure; }

	/**
	 * Set the control effectiveness matrix
	 *
	 * @param B Effectiveness matrix
	 */
	మాయా శూన్య సమితి_Effectiveness_మాతృక(స్థిరరాశి మాతృక::మాతృక_2<దశాంశం, అంకె_AXES, అంకె_ACTUATORS> &effectiveness,
					    స్థిరరాశి Actuator_సదిశ &actuator_trim, స్థిరరాశి Actuator_సదిశ &linearization_బిందువు, పూర్ణాంకం అంకె_actuators,
					    తార్కిక update_normalization_scale);

	/**
	 * Get the allocated actuator vector
	 *
	 * @return Actuator vector
	 */
	స్థిరరాశి Actuator_సదిశ &getActuatorSetpoint() స్థిరరాశి { తిరిగి _actuator_sp; }

	/**
	 * Set the desired control vector
	 *
	 * @param Control vector
	 */
	శూన్య సమితి_Control_Setpoint(స్థిరరాశి మాతృక::సదిశ<దశాంశం, అంకె_AXES> &control) { _control_sp = control; }

	/**
	 * Get the desired control vector
	 *
	 * @return Control vector
	 */
	స్థిరరాశి మాతృక::సదిశ<దశాంశం, అంకె_AXES> &getControlSetpoint() స్థిరరాశి { తిరిగి _control_sp; }

	/**
	 * Get the allocated control vector
	 *
	 * @return Control vector
	 */
	మాతృక::సదిశ<దశాంశం, అంకె_AXES> getAllocatedControl() స్థిరరాశి
	{
		// _actuator_sp contains NaN to indicate stopped motors.
		// Covert back to zero to represent physical thrust.
		Actuator_సదిశ actuator_sp{_actuator_sp};
		actuator_sp.nan_గమ్యం_Zero();
		తిరిగి (_effectiveness * (actuator_sp - _actuator_trim)).emult(control_కేటాయింపు_scale);
	}

	/**
	 * Get the control effectiveness matrix
	 *
	 * @return Effectiveness matrix
	 */
	స్థిరరాశి మాతృక::మాతృక_2<దశాంశం, అంకె_AXES, అంకె_ACTUATORS> &get_Effectiveness_మాతృక() స్థిరరాశి { తిరిగి _effectiveness; }

	/**
	 * Set the minimum actuator values
	 *
	 * @param actuator_min Minimum actuator values
	 */
	శూన్య సమితి_Actuator_Min(స్థిరరాశి Actuator_సదిశ &actuator_min) { _actuator_min = actuator_min; }

	/**
	 * Get the minimum actuator values
	 *
	 * @return Minimum actuator values
	 */
	స్థిరరాశి Actuator_సదిశ &getActuatorMin() స్థిరరాశి { తిరిగి _actuator_min; }

	/**
	 * Set the maximum actuator values
	 *
	 * @param actuator_max Maximum actuator values
	 */
	శూన్య సమితి_Actuator_Max(స్థిరరాశి Actuator_సదిశ &actuator_max) { _actuator_max = actuator_max; }

	/**
	 * Get the maximum actuator values
	 *
	 * @return Maximum actuator values
	 */
	స్థిరరాశి Actuator_సదిశ &getActuatorMax() స్థిరరాశి { తిరిగి _actuator_max; }

	/**
	 * Set the current actuator setpoint.
	 *
	 * Use this when a new allocation method is started to initialize it properly.
	 * In most cases, it is not needed to call this method before `allocate()`.
	 * Indeed the previous actuator setpoint is expected to be stored during calls to `allocate()`.
	 *
	 * @param actuator_sp Actuator setpoint
	 */
	శూన్య సమితి_Actuator_Setpoint(స్థిరరాశి Actuator_సదిశ &actuator_sp);

	శూన్య సమితి_Slew_రేటు_హద్దు(స్థిరరాశి Actuator_సదిశ &slew_రేటు_హద్దు)
	{ actuator_slew_రేటు_హద్దు = slew_రేటు_హద్దు; }

	/**
	 * Apply slew rate to current actuator setpoint
	 */
	శూన్య apply_Slew_రేటు_హద్దు(దశాంశం dt);

	/**
	 * Clip the actuator setpoint between minimum and maximum values.
	 *
	 * The output is in the range [min; max]
	 *
	 * @param actuator Actuator vector to clip
	 */
	శూన్య clipActuatorSetpoint(Actuator_సదిశ &actuator) స్థిరరాశి;

	శూన్య clipActuatorSetpoint() { clipActuatorSetpoint(_actuator_sp); }

	/**
	 * Normalize the actuator setpoint between minimum and maximum values.
	 *
	 * The output is in the range [-1; +1]
	 *
	 * @param actuator Actuator vector to normalize
	 *
	 * @return Clipped actuator setpoint
	 */
	Actuator_సదిశ normalizeActuatorSetpoint(స్థిరరాశి Actuator_సదిశ &actuator)
	స్థిరరాశి;

	/**
	 * Apply a mask of actuators to be set to NaN.
	 *
	 * A NaN value in _actuator_sp represents a disabled or stopped actuator.
	 * This mask is typically used to stop motors in specific flight phases or when certain thrust components are NaN.
	 *
	 * @param nan_actuators_mask Bitmask indicating which actuators to set to NaN.
	 *                           If (nan_actuators_mask & (1 << i)), _actuator_sp(i) becomes NaN.
	 */
	శూన్య apply_Nan_గమ్యం_Actuators(ActuatorBitmask nan_actuators_ముసుగు)
	{
		కొరకు (పూర్ణాంకం i = 0; i < అంకె_actuators_2; i++) {
			ఒకవేళ (nan_actuators_ముసుగు & (1u << i)) {
				_actuator_sp(i) = NAN;
			}
		}
	}

	మాయా శూన్య updateParameters() {}

	పూర్ణాంకం అంకె_Configured_Actuators() స్థిరరాశి { తిరిగి అంకె_actuators_2; }

	శూన్య సమితి_Normalize_RPY(తార్కిక normalize_rpy) { _normalize_rpy = normalize_rpy; }

	/// Axes (bitmask over ControlAxis) dropped from the effectiveness matrix as not independently
	/// achievable, e.g. yaw when collinear with roll after a motor failure
	uint8_t getDroppedAxes() స్థిరరాశి { తిరిగి _dropped_axes; }

రక్షిత:
	మిత్ర తరగతి ControlAllocator; // for _actuator_sp

	మాతృక::మాతృక_2<దశాంశం, అంకె_AXES, అంకె_ACTUATORS> _effectiveness;  ///< Effectiveness matrix
	మాతృక::సదిశ<దశాంశం, అంకె_AXES> control_కేటాయింపు_scale;  	///< Scaling applied during allocation
	Actuator_సదిశ _actuator_trim; 	///< Neutral actuator values
	Actuator_సదిశ _actuator_min; 	///< Minimum actuator values
	Actuator_సదిశ _actuator_max; 	///< Maximum actuator values
	Actuator_సదిశ actuator_slew_రేటు_హద్దు; 	///< Slew rate limit
	Actuator_సదిశ _prev_actuator_sp;  	///< Previous actuator setpoint
	Actuator_సదిశ _actuator_sp;  	///< Actuator setpoint
	మాతృక::సదిశ<దశాంశం, అంకె_AXES> _control_sp;   		///< Control setpoint
	మాతృక::సదిశ<దశాంశం, అంకె_AXES> _control_trim; 		///< Control at trim actuator values
	పూర్ణాంకం అంకె_actuators_2{0};
	తార్కిక _normalize_rpy{అబద్ధం};				///< if true, normalize roll, pitch and yaw columns
	తార్కిక _had_actuator_failure{అబద్ధం};
	uint8_t _dropped_axes{0};				///< bitmask over ControlAxis
};

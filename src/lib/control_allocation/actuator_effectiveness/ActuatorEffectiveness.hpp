/****************************************************************************
 *
 *   Copyright (c) 2020 PX4 Development Team. All rights reserved.
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
 * @file ActuatorEffectiveness.hpp
 *
 * Interface for Actuator Effectiveness
 *
 * @author Julien Lecoeur <julien.lecoeur@gmail.com>
 */

#pragma once

#अन्तर्भाव <cstdint>

#अन्तर्भाव <matrix/matrix/math.hpp>
#अन्तर्भाव <uORB/topics/control_allocator_status.h>

गणना वर्ग AllocationMethod {
	NONE = -1,
	PSEUDO_INVERSE = 0,
	SEQUENTIAL_DESATURATION = 1,
	AUTO = 2,
};

गणना वर्ग ActuatorType {
	MOTORS = 0,
	SERVOS,

	गिनती_2
};

गणना वर्ग EffectivenessUpdateReason {
	NO_EXTERNAL_UPDATE = 0,
	CONFIGURATION_UPDATE = 1, ///< config changes (parameter)
	MOTOR_ACTIVATION_UPDATE = 2, ///< motor failure detected or certain redundant motors are switched off to save energy
};

वर्ग ActuatorEffectiveness
{
सार्वजनिक:
	ActuatorEffectiveness() = स्वभाव;
	आभास ~ActuatorEffectiveness() = स्वभाव;

	स्थिर constexpr पूर्णाङ्क अंक_ACTUATORS = 16;
	स्थिर constexpr पूर्णाङ्क अंक_AXES = 6;

	गणना ControlAxis {
		ROLL = 0,
		PITCH,
		YAW,
		THRUST_X,
		THRUST_Y,
		THRUST_Z
	};

	स्थिर constexpr पूर्णाङ्क MAX_अंक_MATRICES = 2;

	using EffectivenessMatrix = matrix::Matrix<भिन्न, अंक_AXES, अंक_ACTUATORS>;
	using ActuatorVector = matrix::Vector<भिन्न, अंक_ACTUATORS>;
	using ActuatorBitmask = uint32_t;

	static_assert(अंक_ACTUATORS <= 8 * परिमाण(ActuatorBitmask),
		      "NUM_ACTUATORS exceeds the number of bits available in the mask type.");

	गणना वर्ग FlightPhase {
		HOVER_FLIGHT = 0,
		FORWARD_FLIGHT = 1,
		TRANSITION_HF_TO_FF = 2,
		TRANSITION_FF_TO_HF = 3
	};

	संरचना Configuration {
		/**
		 * Add an actuator to the selected matrix, returning the index, or -1 on error
		 */
		पूर्णाङ्क addActuator(ActuatorType type, ध्रुव matrix::Vector3f &torque, ध्रुव matrix::Vector3f &thrust);

		/**
		 * Call this after manually adding N actuators to the selected matrix
		 */
		शून्य actuatorsAdded(ActuatorType type, पूर्णाङ्क गिनती);

		पूर्णाङ्क total_अंक_Actuators() ध्रुव;

		/// Configured effectiveness matrix. Actuators are expected to be filled in order, motors first, then servos
		EffectivenessMatrix effectiveness_matrices[MAX_अंक_MATRICES];

		पूर्णाङ्क अंक_actuators_matrix[MAX_अंक_MATRICES]; ///< current amount, and next actuator index to fill in to effectiveness_matrices
		ActuatorVector trim[MAX_अंक_MATRICES];

		ActuatorVector linearization_point[MAX_अंक_MATRICES];

		पूर्णाङ्क selected_matrix;

		uint8_t matrix_selection_indexes[अंक_ACTUATORS * MAX_अंक_MATRICES];
		पूर्णाङ्क अंक_actuators[(पूर्णाङ्क)ActuatorType::गिनती_2];
	};

	/**
	 * Set the current flight phase
	 *
	 * @param Flight phase
	 */
	आभास शून्य setFlightPhase(ध्रुव FlightPhase &flight_phase)
	{
		_flight_phase = flight_phase;
	}

	/**
	 * Get the number of effectiveness matrices. Must be <= MAX_NUM_MATRICES.
	 * This is expected to stay constant.
	 */
	आभास पूर्णाङ्क अंक_Matrices() ध्रुव { प्रत्यागम 1; }

	/**
	 * Get the desired allocation method(s) for each matrix, if configured as AUTO
	 */
	आभास शून्य getDesiredAllocationMethod(AllocationMethod allocation_method_out[MAX_अंक_MATRICES]) ध्रुव
	{
		प्रति (पूर्णाङ्क i = 0; i < MAX_अंक_MATRICES; ++i) {
			allocation_method_out[i] = AllocationMethod::PSEUDO_INVERSE;
		}
	}

	/**
	 * Query if the roll, pitch and yaw columns of the mixing matrix should be normalized
	 */
	आभास शून्य getNormalizeRPY(तार्किक normalize[MAX_अंक_MATRICES]) ध्रुव
	{
		प्रति (पूर्णाङ्क i = 0; i < MAX_अंक_MATRICES; ++i) {
			normalize[i] = असत्य;
		}
	}

	/**
	 * Get the control effectiveness matrix if updated
	 *
	 * @return true if updated and matrix is set
	 */
	आभास तार्किक getEffectivenessMatrix(Configuration &configuration, EffectivenessUpdateReason external_update) { प्रत्यागम असत्य;}

	/**
	 * Get the current flight phase
	 *
	 * @return Flight phase
	 */
	ध्रुव FlightPhase &getFlightPhase() ध्रुव
	{
		प्रत्यागम _flight_phase;
	}

	/**
	 * Display name
	 */
	आभास ध्रुव वर्ण *name() ध्रुव = 0;

	/**
	 * Callback from the control allocation, allowing to manipulate the setpoint.
	 * Used to allocate auxiliary controls to actuators (e.g. flaps and spoilers).
	 *
	 * @param actuator_sp input & output setpoint
	 */
	आभास शून्य allocateAuxilaryControls(ध्रुव भिन्न dt, पूर्णाङ्क matrix_index, ActuatorVector &actuator_sp) {}

	/**
	 * Callback from the control allocation, allowing to manipulate the setpoint.
	 * This can be used to e.g. add non-linear or external terms.
	 * It is called after the matrix multiplication and before final clipping.
	 * @param actuator_sp input & output setpoint
	 */
	आभास शून्य updateSetpoint(ध्रुव matrix::Vector<भिन्न, अंक_AXES> &control_sp, पूर्णाङ्क matrix_index,
				    ActuatorVector &actuator_sp, ध्रुव ActuatorVector &actuator_min, ध्रुव ActuatorVector &actuator_max) {}

	/**
	 * Get a bitmask of motors to be stopped
	 */
	ActuatorBitmask getStoppedMotors() ध्रुव;

	/**
	 * Fill in the unallocated torque and thrust, customized by effectiveness type.
	 * Can be implemented for every type separately. If not implemented then the effectivenes matrix is used instead.
	 */
	आभास शून्य getUnallocatedControl(पूर्णाङ्क matrix_index, control_allocator_status_s &status) {}

	/**
	 * Override the collective tilt setpoint that would normally come from
	 * tiltrotor_extra_controls. Base implementation is a no-op.
	 *
	 * @param do_override When true, use @p collective_tilt instead of the uORB value.
	 * @param collective_tilt Normalised setpoint in [0, 1]. 0: vertical, 1: horizontal.
	 */
	आभास शून्य overrideCollectiveTilt(तार्किक /*do_override*/, भिन्न /*collective_tilt*/) {}

	/**
	 * Record which components of the thrust setpoint are NaN, so that motors in that direction are stopped.
	 *
	 * @param thrust_sp The thrust setpoint as received by the allocator (instance zero - multicopter only)
	 */
	शून्य stopMotorsBasedOnThrustSetpoint(ध्रुव matrix::Vector3f &thrust_sp)
	{
		_longitudinal_motors_stopped_by_thrust = !PX4_ISFINITE(thrust_sp(0));
		_lateral_motors_stopped_by_thrust = !PX4_ISFINITE(thrust_sp(1));
		_vertical_motors_stopped_by_thrust = !PX4_ISFINITE(thrust_sp(2));
	}

रक्षित:

	संरचना MotorDirectionBitmasks {
		ActuatorBitmask longitudinal{};
		ActuatorBitmask lateral{};
		ActuatorBitmask vertical{};
	} _motor_direction_bitmasks;

	FlightPhase _flight_phase{FlightPhase::HOVER_FLIGHT};
	ActuatorBitmask _stopped_motors_mask_due_to_flight_phase{};

	तार्किक _longitudinal_motors_stopped_by_thrust{असत्य};
	तार्किक _vertical_motors_stopped_by_thrust{असत्य};
	तार्किक _lateral_motors_stopped_by_thrust{असत्य};

};

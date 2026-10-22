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

#அந்தர்பாவ <cstdint>

#அந்தர்பாவ <matrix/matrix/math.hpp>
#அந்தர்பாவ <uORB/topics/control_allocator_status.h>

எண்ணிக்கை வகுப்பு AllocationMethod {
	NONE = -1,
	PSEUDO_INVERSE = 0,
	SEQUENTIAL_DESATURATION = 1,
	AUTO = 2,
};

எண்ணிக்கை வகுப்பு ActuatorType {
	MOTORS = 0,
	SERVOS,

	மொத்தம்_2
};

எண்ணிக்கை வகுப்பு EffectivenessUpdateReason {
	NO_EXTERNAL_UPDATE = 0,
	CONFIGURATION_UPDATE = 1, ///< config changes (parameter)
	MOTOR_ACTIVATION_UPDATE = 2, ///< motor failure detected or certain redundant motors are switched off to save energy
};

வகுப்பு ActuatorEffectiveness
{
பொது:
	ActuatorEffectiveness() = இயல்பு;
	மெய்நிகர் ~ActuatorEffectiveness() = இயல்பு;

	நிலையான constexpr முழு இலக்கம்_ACTUATORS = 16;
	நிலையான constexpr முழு இலக்கம்_AXES = 6;

	எண்ணிக்கை ControlAxis {
		ROLL = 0,
		PITCH,
		YAW,
		THRUST_X,
		THRUST_Y,
		THRUST_Z
	};

	நிலையான constexpr முழு MAX_இலக்கம்_MATRICES = 2;

	using EffectivenessMatrix = matrix::Matrix<தசம, இலக்கம்_AXES, இலக்கம்_ACTUATORS>;
	using ActuatorVector = matrix::Vector<தசம, இலக்கம்_ACTUATORS>;
	using ActuatorBitmask = uint32_t;

	static_assert(இலக்கம்_ACTUATORS <= 8 * அளவு(ActuatorBitmask),
		      "NUM_ACTUATORS exceeds the number of bits available in the mask type.");

	எண்ணிக்கை வகுப்பு FlightPhase {
		HOVER_FLIGHT = 0,
		FORWARD_FLIGHT = 1,
		TRANSITION_HF_TO_FF = 2,
		TRANSITION_FF_TO_HF = 3
	};

	கட்டமைப்பு Configuration {
		/**
		 * Add an actuator to the selected matrix, returning the index, or -1 on error
		 */
		முழு addActuator(ActuatorType type, மாறிலி matrix::Vector3f &torque, மாறிலி matrix::Vector3f &thrust);

		/**
		 * Call this after manually adding N actuators to the selected matrix
		 */
		வெறுமை actuatorsAdded(ActuatorType type, முழு மொத்தம்);

		முழு total_இலக்கம்_Actuators() மாறிலி;

		/// Configured effectiveness matrix. Actuators are expected to be filled in order, motors first, then servos
		EffectivenessMatrix effectiveness_matrices[MAX_இலக்கம்_MATRICES];

		முழு இலக்கம்_actuators_matrix[MAX_இலக்கம்_MATRICES]; ///< current amount, and next actuator index to fill in to effectiveness_matrices
		ActuatorVector trim[MAX_இலக்கம்_MATRICES];

		ActuatorVector linearization_point[MAX_இலக்கம்_MATRICES];

		முழு selected_matrix;

		uint8_t matrix_selection_indexes[இலக்கம்_ACTUATORS * MAX_இலக்கம்_MATRICES];
		முழு இலக்கம்_actuators[(முழு)ActuatorType::மொத்தம்_2];
	};

	/**
	 * Set the current flight phase
	 *
	 * @param Flight phase
	 */
	மெய்நிகர் வெறுமை setFlightPhase(மாறிலி FlightPhase &flight_phase)
	{
		_flight_phase = flight_phase;
	}

	/**
	 * Get the number of effectiveness matrices. Must be <= MAX_NUM_MATRICES.
	 * This is expected to stay constant.
	 */
	மெய்நிகர் முழு இலக்கம்_Matrices() மாறிலி { திரும்பு 1; }

	/**
	 * Get the desired allocation method(s) for each matrix, if configured as AUTO
	 */
	மெய்நிகர் வெறுமை getDesiredAllocationMethod(AllocationMethod allocation_method_out[MAX_இலக்கம்_MATRICES]) மாறிலி
	{
		க்கு (முழு i = 0; i < MAX_இலக்கம்_MATRICES; ++i) {
			allocation_method_out[i] = AllocationMethod::PSEUDO_INVERSE;
		}
	}

	/**
	 * Query if the roll, pitch and yaw columns of the mixing matrix should be normalized
	 */
	மெய்நிகர் வெறுமை getNormalizeRPY(தருக்க normalize[MAX_இலக்கம்_MATRICES]) மாறிலி
	{
		க்கு (முழு i = 0; i < MAX_இலக்கம்_MATRICES; ++i) {
			normalize[i] = பொய்;
		}
	}

	/**
	 * Get the control effectiveness matrix if updated
	 *
	 * @return true if updated and matrix is set
	 */
	மெய்நிகர் தருக்க getEffectivenessMatrix(Configuration &configuration, EffectivenessUpdateReason external_update) { திரும்பு பொய்;}

	/**
	 * Get the current flight phase
	 *
	 * @return Flight phase
	 */
	மாறிலி FlightPhase &getFlightPhase() மாறிலி
	{
		திரும்பு _flight_phase;
	}

	/**
	 * Display name
	 */
	மெய்நிகர் மாறிலி எழுத்து *name() மாறிலி = 0;

	/**
	 * Callback from the control allocation, allowing to manipulate the setpoint.
	 * Used to allocate auxiliary controls to actuators (e.g. flaps and spoilers).
	 *
	 * @param actuator_sp input & output setpoint
	 */
	மெய்நிகர் வெறுமை allocateAuxilaryControls(மாறிலி தசம dt, முழு matrix_index, ActuatorVector &actuator_sp) {}

	/**
	 * Callback from the control allocation, allowing to manipulate the setpoint.
	 * This can be used to e.g. add non-linear or external terms.
	 * It is called after the matrix multiplication and before final clipping.
	 * @param actuator_sp input & output setpoint
	 */
	மெய்நிகர் வெறுமை updateSetpoint(மாறிலி matrix::Vector<தசம, இலக்கம்_AXES> &control_sp, முழு matrix_index,
				    ActuatorVector &actuator_sp, மாறிலி ActuatorVector &actuator_min, மாறிலி ActuatorVector &actuator_max) {}

	/**
	 * Get a bitmask of motors to be stopped
	 */
	ActuatorBitmask getStoppedMotors() மாறிலி;

	/**
	 * Fill in the unallocated torque and thrust, customized by effectiveness type.
	 * Can be implemented for every type separately. If not implemented then the effectivenes matrix is used instead.
	 */
	மெய்நிகர் வெறுமை getUnallocatedControl(முழு matrix_index, control_allocator_status_s &status) {}

	/**
	 * Override the collective tilt setpoint that would normally come from
	 * tiltrotor_extra_controls. Base implementation is a no-op.
	 *
	 * @param do_override When true, use @p collective_tilt instead of the uORB value.
	 * @param collective_tilt Normalised setpoint in [0, 1]. 0: vertical, 1: horizontal.
	 */
	மெய்நிகர் வெறுமை overrideCollectiveTilt(தருக்க /*do_override*/, தசம /*collective_tilt*/) {}

	/**
	 * Record which components of the thrust setpoint are NaN, so that motors in that direction are stopped.
	 *
	 * @param thrust_sp The thrust setpoint as received by the allocator (instance zero - multicopter only)
	 */
	வெறுமை stopMotorsBasedOnThrustSetpoint(மாறிலி matrix::Vector3f &thrust_sp)
	{
		_longitudinal_motors_stopped_by_thrust = !PX4_ISFINITE(thrust_sp(0));
		_lateral_motors_stopped_by_thrust = !PX4_ISFINITE(thrust_sp(1));
		_vertical_motors_stopped_by_thrust = !PX4_ISFINITE(thrust_sp(2));
	}

பாதுகாக்கப்பட்ட:

	கட்டமைப்பு MotorDirectionBitmasks {
		ActuatorBitmask longitudinal{};
		ActuatorBitmask lateral{};
		ActuatorBitmask vertical{};
	} _motor_direction_bitmasks;

	FlightPhase _flight_phase{FlightPhase::HOVER_FLIGHT};
	ActuatorBitmask _stopped_motors_mask_due_to_flight_phase{};

	தருக்க _longitudinal_motors_stopped_by_thrust{பொய்};
	தருக்க _vertical_motors_stopped_by_thrust{பொய்};
	தருக்க _lateral_motors_stopped_by_thrust{பொய்};

};

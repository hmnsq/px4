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

#ଅନ୍ତର୍ଭାଵ <cstdint>

#ଅନ୍ତର୍ଭାଵ <matrix/matrix/math.hpp>
#ଅନ୍ତର୍ଭାଵ <uORB/topics/control_allocator_status.h>

ଗଣନା ଶ୍ରେଣୀ AllocationMethod {
	NONE = -1,
	PSEUDO_INVERSE = 0,
	SEQUENTIAL_DESATURATION = 1,
	AUTO = 2,
};

ଗଣନା ଶ୍ରେଣୀ ActuatorType {
	MOTORS = 0,
	SERVOS,

	ଗଣନା_3
};

ଗଣନା ଶ୍ରେଣୀ EffectivenessUpdateReason {
	NO_EXTERNAL_UPDATE = 0,
	CONFIGURATION_UPDATE = 1, ///< config changes (parameter)
	MOTOR_ACTIVATION_UPDATE = 2, ///< motor failure detected or certain redundant motors are switched off to save energy
};

ଶ୍ରେଣୀ ActuatorEffectiveness
{
ସର୍ବସାଧାରଣ:
	ActuatorEffectiveness() = ପୂର୍ବନିର୍ଦ୍ଧାରିତ;
	ଆଭାସୀ ~ActuatorEffectiveness() = ପୂର୍ବନିର୍ଦ୍ଧାରିତ;

	ସ୍ଥିର constexpr ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଅଙ୍କ_ACTUATORS = 16;
	ସ୍ଥିର constexpr ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଅଙ୍କ_AXES = 6;

	ଗଣନା ControlAxis {
		ROLL = 0,
		PITCH,
		YAW,
		THRUST_X,
		THRUST_Y,
		THRUST_Z
	};

	ସ୍ଥିର constexpr ପୂର୍ଣ୍ଣସଂଖ୍ୟା MAX_ଅଙ୍କ_MATRICES = 2;

	using EffectivenessMatrix = matrix::Matrix<ଦଶମିକ, ଅଙ୍କ_AXES, ଅଙ୍କ_ACTUATORS>;
	using ActuatorVector = matrix::Vector<ଦଶମିକ, ଅଙ୍କ_ACTUATORS>;
	using ActuatorBitmask = uint32_t;

	static_assert(ଅଙ୍କ_ACTUATORS <= 8 * ଆକାର(ActuatorBitmask),
		      "NUM_ACTUATORS exceeds the number of bits available in the mask type.");

	ଗଣନା ଶ୍ରେଣୀ FlightPhase {
		HOVER_FLIGHT = 0,
		FORWARD_FLIGHT = 1,
		TRANSITION_HF_TO_FF = 2,
		TRANSITION_FF_TO_HF = 3
	};

	ସଂରଚନା Configuration {
		/**
		 * Add an actuator to the selected matrix, returning the index, or -1 on error
		 */
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା addActuator(ActuatorType type, ଅଚଳ matrix::Vector3f &torque, ଅଚଳ matrix::Vector3f &thrust);

		/**
		 * Call this after manually adding N actuators to the selected matrix
		 */
		ଶୂନ୍ୟ actuatorsAdded(ActuatorType type, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଗଣନା_2);

		ପୂର୍ଣ୍ଣସଂଖ୍ୟା total_ଅଙ୍କ_Actuators() ଅଚଳ;

		/// Configured effectiveness matrix. Actuators are expected to be filled in order, motors first, then servos
		EffectivenessMatrix effectiveness_matrices[MAX_ଅଙ୍କ_MATRICES];

		ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଅଙ୍କ_actuators_matrix[MAX_ଅଙ୍କ_MATRICES]; ///< current amount, and next actuator index to fill in to effectiveness_matrices
		ActuatorVector trim[MAX_ଅଙ୍କ_MATRICES];

		ActuatorVector linearization_point[MAX_ଅଙ୍କ_MATRICES];

		ପୂର୍ଣ୍ଣସଂଖ୍ୟା selected_matrix;

		uint8_t matrix_selection_indexes[ଅଙ୍କ_ACTUATORS * MAX_ଅଙ୍କ_MATRICES];
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଅଙ୍କ_actuators[(ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ActuatorType::ଗଣନା_3];
	};

	/**
	 * Set the current flight phase
	 *
	 * @param Flight phase
	 */
	ଆଭାସୀ ଶୂନ୍ୟ setFlightPhase(ଅଚଳ FlightPhase &flight_phase)
	{
		_flight_phase = flight_phase;
	}

	/**
	 * Get the number of effectiveness matrices. Must be <= MAX_NUM_MATRICES.
	 * This is expected to stay constant.
	 */
	ଆଭାସୀ ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଅଙ୍କ_Matrices() ଅଚଳ { ଫେରନ୍ତୁ 1; }

	/**
	 * Get the desired allocation method(s) for each matrix, if configured as AUTO
	 */
	ଆଭାସୀ ଶୂନ୍ୟ getDesiredAllocationMethod(AllocationMethod allocation_method_out[MAX_ଅଙ୍କ_MATRICES]) ଅଚଳ
	{
		ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < MAX_ଅଙ୍କ_MATRICES; ++i) {
			allocation_method_out[i] = AllocationMethod::PSEUDO_INVERSE;
		}
	}

	/**
	 * Query if the roll, pitch and yaw columns of the mixing matrix should be normalized
	 */
	ଆଭାସୀ ଶୂନ୍ୟ getNormalizeRPY(ତାର୍କିକ normalize[MAX_ଅଙ୍କ_MATRICES]) ଅଚଳ
	{
		ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < MAX_ଅଙ୍କ_MATRICES; ++i) {
			normalize[i] = ମିଥ୍ୟା;
		}
	}

	/**
	 * Get the control effectiveness matrix if updated
	 *
	 * @return true if updated and matrix is set
	 */
	ଆଭାସୀ ତାର୍କିକ getEffectivenessMatrix(Configuration &configuration, EffectivenessUpdateReason external_update) { ଫେରନ୍ତୁ ମିଥ୍ୟା;}

	/**
	 * Get the current flight phase
	 *
	 * @return Flight phase
	 */
	ଅଚଳ FlightPhase &getFlightPhase() ଅଚଳ
	{
		ଫେରନ୍ତୁ _flight_phase;
	}

	/**
	 * Display name
	 */
	ଆଭାସୀ ଅଚଳ ଅକ୍ଷର *name() ଅଚଳ = 0;

	/**
	 * Callback from the control allocation, allowing to manipulate the setpoint.
	 * Used to allocate auxiliary controls to actuators (e.g. flaps and spoilers).
	 *
	 * @param actuator_sp input & output setpoint
	 */
	ଆଭାସୀ ଶୂନ୍ୟ allocateAuxilaryControls(ଅଚଳ ଦଶମିକ dt, ପୂର୍ଣ୍ଣସଂଖ୍ୟା matrix_index, ActuatorVector &actuator_sp) {}

	/**
	 * Callback from the control allocation, allowing to manipulate the setpoint.
	 * This can be used to e.g. add non-linear or external terms.
	 * It is called after the matrix multiplication and before final clipping.
	 * @param actuator_sp input & output setpoint
	 */
	ଆଭାସୀ ଶୂନ୍ୟ updateSetpoint(ଅଚଳ matrix::Vector<ଦଶମିକ, ଅଙ୍କ_AXES> &control_sp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା matrix_index,
				    ActuatorVector &actuator_sp, ଅଚଳ ActuatorVector &actuator_min, ଅଚଳ ActuatorVector &actuator_max) {}

	/**
	 * Get a bitmask of motors to be stopped
	 */
	ActuatorBitmask getStoppedMotors() ଅଚଳ;

	/**
	 * Fill in the unallocated torque and thrust, customized by effectiveness type.
	 * Can be implemented for every type separately. If not implemented then the effectivenes matrix is used instead.
	 */
	ଆଭାସୀ ଶୂନ୍ୟ getUnallocatedControl(ପୂର୍ଣ୍ଣସଂଖ୍ୟା matrix_index, control_allocator_status_s &status) {}

	/**
	 * Override the collective tilt setpoint that would normally come from
	 * tiltrotor_extra_controls. Base implementation is a no-op.
	 *
	 * @param do_override When true, use @p collective_tilt instead of the uORB value.
	 * @param collective_tilt Normalised setpoint in [0, 1]. 0: vertical, 1: horizontal.
	 */
	ଆଭାସୀ ଶୂନ୍ୟ overrideCollectiveTilt(ତାର୍କିକ /*do_override*/, ଦଶମିକ /*collective_tilt*/) {}

	/**
	 * Record which components of the thrust setpoint are NaN, so that motors in that direction are stopped.
	 *
	 * @param thrust_sp The thrust setpoint as received by the allocator (instance zero - multicopter only)
	 */
	ଶୂନ୍ୟ stopMotorsBasedOnThrustSetpoint(ଅଚଳ matrix::Vector3f &thrust_sp)
	{
		_longitudinal_motors_stopped_by_thrust = !PX4_ISFINITE(thrust_sp(0));
		_lateral_motors_stopped_by_thrust = !PX4_ISFINITE(thrust_sp(1));
		_vertical_motors_stopped_by_thrust = !PX4_ISFINITE(thrust_sp(2));
	}

ସୁରକ୍ଷିତ:

	ସଂରଚନା MotorDirectionBitmasks {
		ActuatorBitmask longitudinal{};
		ActuatorBitmask lateral{};
		ActuatorBitmask vertical{};
	} _motor_direction_bitmasks;

	FlightPhase _flight_phase{FlightPhase::HOVER_FLIGHT};
	ActuatorBitmask _stopped_motors_mask_due_to_flight_phase{};

	ତାର୍କିକ _longitudinal_motors_stopped_by_thrust{ମିଥ୍ୟା};
	ତାର୍କିକ _vertical_motors_stopped_by_thrust{ମିଥ୍ୟା};
	ତାର୍କିକ _lateral_motors_stopped_by_thrust{ମିଥ୍ୟା};

};

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

#ಅನ್ತರ್ಭಾವ <cstdint>

#ಅನ್ತರ್ಭಾವ <matrix/matrix/math.hpp>
#ಅನ್ತರ್ಭಾವ <uORB/topics/control_allocator_status.h>

ಗಣನಾ ವರ್ಗ AllocationMethod {
	NONE = -1,
	PSEUDO_INVERSE = 0,
	SEQUENTIAL_DESATURATION = 1,
	AUTO = 2,
};

ಗಣನಾ ವರ್ಗ ActuatorType {
	MOTORS = 0,
	SERVOS,

	ಎಣಿಕೆ_2
};

ಗಣನಾ ವರ್ಗ EffectivenessUpdateReason {
	NO_EXTERNAL_UPDATE = 0,
	CONFIGURATION_UPDATE = 1, ///< config changes (parameter)
	MOTOR_ACTIVATION_UPDATE = 2, ///< motor failure detected or certain redundant motors are switched off to save energy
};

ವರ್ಗ ActuatorEffectiveness
{
ಸಾರ್ವಜನಿಕ:
	ActuatorEffectiveness() = ಸ್ವಭಾವ;
	ಆಭಾಸ ~ActuatorEffectiveness() = ಸ್ವಭಾವ;

	ಸ್ಥಿರ constexpr ಪೂರ್ಣಾಙ್ಕ ಅಂಕೆ_ACTUATORS = 16;
	ಸ್ಥಿರ constexpr ಪೂರ್ಣಾಙ್ಕ ಅಂಕೆ_AXES = 6;

	ಗಣನಾ ControlAxis {
		ROLL = 0,
		PITCH,
		YAW,
		THRUST_X,
		THRUST_Y,
		THRUST_Z
	};

	ಸ್ಥಿರ constexpr ಪೂರ್ಣಾಙ್ಕ MAX_ಅಂಕೆ_MATRICES = 2;

	using EffectivenessMatrix = matrix::Matrix<ಭಿನ್ನ, ಅಂಕೆ_AXES, ಅಂಕೆ_ACTUATORS>;
	using ActuatorVector = matrix::Vector<ಭಿನ್ನ, ಅಂಕೆ_ACTUATORS>;
	using ActuatorBitmask = uint32_t;

	static_assert(ಅಂಕೆ_ACTUATORS <= 8 * ಪರಿಮಾಣ(ActuatorBitmask),
		      "NUM_ACTUATORS exceeds the number of bits available in the mask type.");

	ಗಣನಾ ವರ್ಗ FlightPhase {
		HOVER_FLIGHT = 0,
		FORWARD_FLIGHT = 1,
		TRANSITION_HF_TO_FF = 2,
		TRANSITION_FF_TO_HF = 3
	};

	ಸಂರಚನಾ Configuration {
		/**
		 * Add an actuator to the selected matrix, returning the index, or -1 on error
		 */
		ಪೂರ್ಣಾಙ್ಕ addActuator(ActuatorType type, ಧ್ರುವ matrix::Vector3f &torque, ಧ್ರುವ matrix::Vector3f &thrust);

		/**
		 * Call this after manually adding N actuators to the selected matrix
		 */
		ಶೂನ್ಯ actuatorsAdded(ActuatorType type, ಪೂರ್ಣಾಙ್ಕ ಎಣಿಕೆ);

		ಪೂರ್ಣಾಙ್ಕ total_ಅಂಕೆ_Actuators() ಧ್ರುವ;

		/// Configured effectiveness matrix. Actuators are expected to be filled in order, motors first, then servos
		EffectivenessMatrix effectiveness_matrices[MAX_ಅಂಕೆ_MATRICES];

		ಪೂರ್ಣಾಙ್ಕ ಅಂಕೆ_actuators_matrix[MAX_ಅಂಕೆ_MATRICES]; ///< current amount, and next actuator index to fill in to effectiveness_matrices
		ActuatorVector trim[MAX_ಅಂಕೆ_MATRICES];

		ActuatorVector linearization_point[MAX_ಅಂಕೆ_MATRICES];

		ಪೂರ್ಣಾಙ್ಕ selected_matrix;

		uint8_t matrix_selection_indexes[ಅಂಕೆ_ACTUATORS * MAX_ಅಂಕೆ_MATRICES];
		ಪೂರ್ಣಾಙ್ಕ ಅಂಕೆ_actuators[(ಪೂರ್ಣಾಙ್ಕ)ActuatorType::ಎಣಿಕೆ_2];
	};

	/**
	 * Set the current flight phase
	 *
	 * @param Flight phase
	 */
	ಆಭಾಸ ಶೂನ್ಯ setFlightPhase(ಧ್ರುವ FlightPhase &flight_phase)
	{
		_flight_phase = flight_phase;
	}

	/**
	 * Get the number of effectiveness matrices. Must be <= MAX_NUM_MATRICES.
	 * This is expected to stay constant.
	 */
	ಆಭಾಸ ಪೂರ್ಣಾಙ್ಕ ಅಂಕೆ_Matrices() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ 1; }

	/**
	 * Get the desired allocation method(s) for each matrix, if configured as AUTO
	 */
	ಆಭಾಸ ಶೂನ್ಯ getDesiredAllocationMethod(AllocationMethod allocation_method_out[MAX_ಅಂಕೆ_MATRICES]) ಧ್ರುವ
	{
		ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < MAX_ಅಂಕೆ_MATRICES; ++i) {
			allocation_method_out[i] = AllocationMethod::PSEUDO_INVERSE;
		}
	}

	/**
	 * Query if the roll, pitch and yaw columns of the mixing matrix should be normalized
	 */
	ಆಭಾಸ ಶೂನ್ಯ getNormalizeRPY(ತಾರ್ಕಿಕ normalize[MAX_ಅಂಕೆ_MATRICES]) ಧ್ರುವ
	{
		ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < MAX_ಅಂಕೆ_MATRICES; ++i) {
			normalize[i] = ಅಸತ್ಯ;
		}
	}

	/**
	 * Get the control effectiveness matrix if updated
	 *
	 * @return true if updated and matrix is set
	 */
	ಆಭಾಸ ತಾರ್ಕಿಕ getEffectivenessMatrix(Configuration &configuration, EffectivenessUpdateReason external_update) { ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;}

	/**
	 * Get the current flight phase
	 *
	 * @return Flight phase
	 */
	ಧ್ರುವ FlightPhase &getFlightPhase() ಧ್ರುವ
	{
		ಪ್ರತ್ಯಾಗಮ _flight_phase;
	}

	/**
	 * Display name
	 */
	ಆಭಾಸ ಧ್ರುವ ವರ್ಣ *name() ಧ್ರುವ = 0;

	/**
	 * Callback from the control allocation, allowing to manipulate the setpoint.
	 * Used to allocate auxiliary controls to actuators (e.g. flaps and spoilers).
	 *
	 * @param actuator_sp input & output setpoint
	 */
	ಆಭಾಸ ಶೂನ್ಯ allocateAuxilaryControls(ಧ್ರುವ ಭಿನ್ನ dt, ಪೂರ್ಣಾಙ್ಕ matrix_index, ActuatorVector &actuator_sp) {}

	/**
	 * Callback from the control allocation, allowing to manipulate the setpoint.
	 * This can be used to e.g. add non-linear or external terms.
	 * It is called after the matrix multiplication and before final clipping.
	 * @param actuator_sp input & output setpoint
	 */
	ಆಭಾಸ ಶೂನ್ಯ updateSetpoint(ಧ್ರುವ matrix::Vector<ಭಿನ್ನ, ಅಂಕೆ_AXES> &control_sp, ಪೂರ್ಣಾಙ್ಕ matrix_index,
				    ActuatorVector &actuator_sp, ಧ್ರುವ ActuatorVector &actuator_min, ಧ್ರುವ ActuatorVector &actuator_max) {}

	/**
	 * Get a bitmask of motors to be stopped
	 */
	ActuatorBitmask getStoppedMotors() ಧ್ರುವ;

	/**
	 * Fill in the unallocated torque and thrust, customized by effectiveness type.
	 * Can be implemented for every type separately. If not implemented then the effectivenes matrix is used instead.
	 */
	ಆಭಾಸ ಶೂನ್ಯ getUnallocatedControl(ಪೂರ್ಣಾಙ್ಕ matrix_index, control_allocator_status_s &status) {}

	/**
	 * Override the collective tilt setpoint that would normally come from
	 * tiltrotor_extra_controls. Base implementation is a no-op.
	 *
	 * @param do_override When true, use @p collective_tilt instead of the uORB value.
	 * @param collective_tilt Normalised setpoint in [0, 1]. 0: vertical, 1: horizontal.
	 */
	ಆಭಾಸ ಶೂನ್ಯ overrideCollectiveTilt(ತಾರ್ಕಿಕ /*do_override*/, ಭಿನ್ನ /*collective_tilt*/) {}

	/**
	 * Record which components of the thrust setpoint are NaN, so that motors in that direction are stopped.
	 *
	 * @param thrust_sp The thrust setpoint as received by the allocator (instance zero - multicopter only)
	 */
	ಶೂನ್ಯ stopMotorsBasedOnThrustSetpoint(ಧ್ರುವ matrix::Vector3f &thrust_sp)
	{
		_longitudinal_motors_stopped_by_thrust = !PX4_ISFINITE(thrust_sp(0));
		_lateral_motors_stopped_by_thrust = !PX4_ISFINITE(thrust_sp(1));
		_vertical_motors_stopped_by_thrust = !PX4_ISFINITE(thrust_sp(2));
	}

ರಕ್ಷಿತ:

	ಸಂರಚನಾ MotorDirectionBitmasks {
		ActuatorBitmask longitudinal{};
		ActuatorBitmask lateral{};
		ActuatorBitmask vertical{};
	} _motor_direction_bitmasks;

	FlightPhase _flight_phase{FlightPhase::HOVER_FLIGHT};
	ActuatorBitmask _stopped_motors_mask_due_to_flight_phase{};

	ತಾರ್ಕಿಕ _longitudinal_motors_stopped_by_thrust{ಅಸತ್ಯ};
	ತಾರ್ಕಿಕ _vertical_motors_stopped_by_thrust{ಅಸತ್ಯ};
	ತಾರ್ಕಿಕ _lateral_motors_stopped_by_thrust{ಅಸತ್ಯ};

};

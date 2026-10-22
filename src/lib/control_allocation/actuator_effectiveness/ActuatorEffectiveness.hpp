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

#شامل <cstdint>

#شامل <matrix/matrix/math.hpp>
#شامل <uORB/topics/control_allocator_status.h>

شمارش رده AllocationMethod {
	NONE = -1,
	PSEUDO_INVERSE = 0,
	SEQUENTIAL_DESATURATION = 1,
	AUTO = 2,
};

شمارش رده ActuatorType {
	MOTORS = 0,
	SERVOS,

	شمار_2
};

شمارش رده EffectivenessUpdateReason {
	NO_EXTERNAL_UPDATE = 0,
	CONFIGURATION_UPDATE = 1, ///< config changes (parameter)
	MOTOR_ACTIVATION_UPDATE = 2, ///< motor failure detected or certain redundant motors are switched off to save energy
};

رده ActuatorEffectiveness
{
عمومی:
	ActuatorEffectiveness() = پیشفرض;
	مجازی ~ActuatorEffectiveness() = پیشفرض;

	ایستا constexpr صحیح رقم_ACTUATORS = 16;
	ایستا constexpr صحیح رقم_AXES = 6;

	شمارش ControlAxis {
		ROLL = 0,
		PITCH,
		YAW,
		THRUST_X,
		THRUST_Y,
		THRUST_Z
	};

	ایستا constexpr صحیح MAX_رقم_MATRICES = 2;

	using EffectivenessMatrix = matrix::Matrix<اعشاری, رقم_AXES, رقم_ACTUATORS>;
	using ActuatorVector = matrix::Vector<اعشاری, رقم_ACTUATORS>;
	using ActuatorBitmask = uint32_t;

	static_assert(رقم_ACTUATORS <= 8 * اندازه(ActuatorBitmask),
		      "NUM_ACTUATORS exceeds the number of bits available in the mask type.");

	شمارش رده FlightPhase {
		HOVER_FLIGHT = 0,
		FORWARD_FLIGHT = 1,
		TRANSITION_HF_TO_FF = 2,
		TRANSITION_FF_TO_HF = 3
	};

	ساختار Configuration {
		/**
		 * Add an actuator to the selected matrix, returning the index, or -1 on error
		 */
		صحیح addActuator(ActuatorType type, ثابت matrix::Vector3f &torque, ثابت matrix::Vector3f &thrust);

		/**
		 * Call this after manually adding N actuators to the selected matrix
		 */
		تهی actuatorsAdded(ActuatorType type, صحیح شمار);

		صحیح total_رقم_Actuators() ثابت;

		/// Configured effectiveness matrix. Actuators are expected to be filled in order, motors first, then servos
		EffectivenessMatrix effectiveness_matrices[MAX_رقم_MATRICES];

		صحیح رقم_actuators_matrix[MAX_رقم_MATRICES]; ///< current amount, and next actuator index to fill in to effectiveness_matrices
		ActuatorVector trim[MAX_رقم_MATRICES];

		ActuatorVector linearization_point[MAX_رقم_MATRICES];

		صحیح selected_matrix;

		uint8_t matrix_selection_indexes[رقم_ACTUATORS * MAX_رقم_MATRICES];
		صحیح رقم_actuators[(صحیح)ActuatorType::شمار_2];
	};

	/**
	 * Set the current flight phase
	 *
	 * @param Flight phase
	 */
	مجازی تهی setFlightPhase(ثابت FlightPhase &flight_phase)
	{
		_flight_phase = flight_phase;
	}

	/**
	 * Get the number of effectiveness matrices. Must be <= MAX_NUM_MATRICES.
	 * This is expected to stay constant.
	 */
	مجازی صحیح رقم_Matrices() ثابت { بازگشت 1; }

	/**
	 * Get the desired allocation method(s) for each matrix, if configured as AUTO
	 */
	مجازی تهی getDesiredAllocationMethod(AllocationMethod allocation_method_out[MAX_رقم_MATRICES]) ثابت
	{
		برای (صحیح i = 0; i < MAX_رقم_MATRICES; ++i) {
			allocation_method_out[i] = AllocationMethod::PSEUDO_INVERSE;
		}
	}

	/**
	 * Query if the roll, pitch and yaw columns of the mixing matrix should be normalized
	 */
	مجازی تهی getNormalizeRPY(منطقی normalize[MAX_رقم_MATRICES]) ثابت
	{
		برای (صحیح i = 0; i < MAX_رقم_MATRICES; ++i) {
			normalize[i] = نادرست;
		}
	}

	/**
	 * Get the control effectiveness matrix if updated
	 *
	 * @return true if updated and matrix is set
	 */
	مجازی منطقی getEffectivenessMatrix(Configuration &configuration, EffectivenessUpdateReason external_update) { بازگشت نادرست;}

	/**
	 * Get the current flight phase
	 *
	 * @return Flight phase
	 */
	ثابت FlightPhase &getFlightPhase() ثابت
	{
		بازگشت _flight_phase;
	}

	/**
	 * Display name
	 */
	مجازی ثابت نویسه *name() ثابت = 0;

	/**
	 * Callback from the control allocation, allowing to manipulate the setpoint.
	 * Used to allocate auxiliary controls to actuators (e.g. flaps and spoilers).
	 *
	 * @param actuator_sp input & output setpoint
	 */
	مجازی تهی allocateAuxilaryControls(ثابت اعشاری dt, صحیح matrix_index, ActuatorVector &actuator_sp) {}

	/**
	 * Callback from the control allocation, allowing to manipulate the setpoint.
	 * This can be used to e.g. add non-linear or external terms.
	 * It is called after the matrix multiplication and before final clipping.
	 * @param actuator_sp input & output setpoint
	 */
	مجازی تهی updateSetpoint(ثابت matrix::Vector<اعشاری, رقم_AXES> &control_sp, صحیح matrix_index,
				    ActuatorVector &actuator_sp, ثابت ActuatorVector &actuator_min, ثابت ActuatorVector &actuator_max) {}

	/**
	 * Get a bitmask of motors to be stopped
	 */
	ActuatorBitmask getStoppedMotors() ثابت;

	/**
	 * Fill in the unallocated torque and thrust, customized by effectiveness type.
	 * Can be implemented for every type separately. If not implemented then the effectivenes matrix is used instead.
	 */
	مجازی تهی getUnallocatedControl(صحیح matrix_index, control_allocator_status_s &status) {}

	/**
	 * Override the collective tilt setpoint that would normally come from
	 * tiltrotor_extra_controls. Base implementation is a no-op.
	 *
	 * @param do_override When true, use @p collective_tilt instead of the uORB value.
	 * @param collective_tilt Normalised setpoint in [0, 1]. 0: vertical, 1: horizontal.
	 */
	مجازی تهی overrideCollectiveTilt(منطقی /*do_override*/, اعشاری /*collective_tilt*/) {}

	/**
	 * Record which components of the thrust setpoint are NaN, so that motors in that direction are stopped.
	 *
	 * @param thrust_sp The thrust setpoint as received by the allocator (instance zero - multicopter only)
	 */
	تهی stopMotorsBasedOnThrustSetpoint(ثابت matrix::Vector3f &thrust_sp)
	{
		_longitudinal_motors_stopped_by_thrust = !PX4_ISFINITE(thrust_sp(0));
		_lateral_motors_stopped_by_thrust = !PX4_ISFINITE(thrust_sp(1));
		_vertical_motors_stopped_by_thrust = !PX4_ISFINITE(thrust_sp(2));
	}

محافظتشده:

	ساختار MotorDirectionBitmasks {
		ActuatorBitmask longitudinal{};
		ActuatorBitmask lateral{};
		ActuatorBitmask vertical{};
	} _motor_direction_bitmasks;

	FlightPhase _flight_phase{FlightPhase::HOVER_FLIGHT};
	ActuatorBitmask _stopped_motors_mask_due_to_flight_phase{};

	منطقی _longitudinal_motors_stopped_by_thrust{نادرست};
	منطقی _vertical_motors_stopped_by_thrust{نادرست};
	منطقی _lateral_motors_stopped_by_thrust{نادرست};

};

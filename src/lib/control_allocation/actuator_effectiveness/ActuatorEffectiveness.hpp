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

#ہدایت once

#شامل <cstdint>

#شامل <matrix/matrix/math.hpp>
#شامل <uORB/topics/control_allocator_status.h>

شمار جماعت تخصیص_طریقہ {
	NONE = -1,
	PSEUDO_INVERSE = 0,
	SEQUENTIAL_DESATURATION = 1,
	AUTO = 2,
};

شمار جماعت ActuatorType {
	MOTORS = 0,
	SERVOS,

	گنتی_2
};

شمار جماعت EffectivenessUpdateReason {
	NO_EXTERNAL_UPDATE = 0,
	CONFIGURATION_UPDATE = 1, ///< config changes (parameter)
	MOTOR_ACTIVATION_UPDATE = 2, ///< motor failure detected or certain redundant motors are switched off to save energy
};

جماعت ActuatorEffectiveness
{
عوامی:
	ActuatorEffectiveness() = طےشدہ;
	مجازی ~ActuatorEffectiveness() = طےشدہ;

	ساکن constexpr صحیح عدد_ACTUATORS = 16;
	ساکن constexpr صحیح عدد_AXES = 6;

	شمار ControlAxis {
		ROLL = 0,
		PITCH,
		YAW,
		THRUST_X,
		THRUST_Y,
		THRUST_Z
	};

	ساکن constexpr صحیح MAX_عدد_MATRICES = 2;

	استعمال Effectiveness_میٹرکس = میٹرکس::میٹرکس_2<اعشاری, عدد_AXES, عدد_ACTUATORS>;
	استعمال Actuator_سمتیہ = میٹرکس::سمتیہ<اعشاری, عدد_ACTUATORS>;
	استعمال ActuatorBitmask = uint32_t;

	static_assert(عدد_ACTUATORS <= 8 * حجم(ActuatorBitmask),
		      "NUM_ACTUATORS exceeds the number of bits available in the mask type.");

	شمار جماعت FlightPhase {
		HOVER_FLIGHT = 0,
		FORWARD_FLIGHT = 1,
		TRANSITION_HF_مقصد_FF = 2,
		TRANSITION_FF_مقصد_HF = 3
	};

	ساخت Configuration {
		/**
		 * Add an actuator to the selected matrix, returning the index, or -1 on error
		 */
		صحیح جوڑو_Actuator(ActuatorType type, ثابت میٹرکس::Vector3f &torque, ثابت میٹرکس::Vector3f &thrust);

		/**
		 * Call this after manually adding N actuators to the selected matrix
		 */
		خالی actuatorsAdded(ActuatorType type, صحیح گنتی);

		صحیح کل_عدد_Actuators() ثابت;

		/// Configured effectiveness matrix. Actuators are expected to be filled in order, motors first, then servos
		Effectiveness_میٹرکس effectiveness_matrices[MAX_عدد_MATRICES];

		صحیح عدد_actuators_میٹرکس[MAX_عدد_MATRICES]; ///< current amount, and next actuator index to fill in to effectiveness_matrices
		Actuator_سمتیہ trim[MAX_عدد_MATRICES];

		Actuator_سمتیہ linearization_نقطہ[MAX_عدد_MATRICES];

		صحیح selected_میٹرکس;

		uint8_t میٹرکس_selection_indexes[عدد_ACTUATORS * MAX_عدد_MATRICES];
		صحیح عدد_actuators[(صحیح)ActuatorType::گنتی_2];
	};

	/**
	 * Set the current flight phase
	 *
	 * @param Flight phase
	 */
	مجازی خالی سیٹ_Flight_Phase(ثابت FlightPhase &flight_phase)
	{
		_flight_phase = flight_phase;
	}

	/**
	 * Get the number of effectiveness matrices. Must be <= MAX_NUM_MATRICES.
	 * This is expected to stay constant.
	 */
	مجازی صحیح عدد_Matrices() ثابت { واپس 1; }

	/**
	 * Get the desired allocation method(s) for each matrix, if configured as AUTO
	 */
	مجازی خالی get_Desired_تخصیص_طریقہ(تخصیص_طریقہ تخصیص_طریقہ_برآمد[MAX_عدد_MATRICES]) ثابت
	{
		برائے (صحیح i = 0; i < MAX_عدد_MATRICES; ++i) {
			تخصیص_طریقہ_برآمد[i] = تخصیص_طریقہ::PSEUDO_INVERSE;
		}
	}

	/**
	 * Query if the roll, pitch and yaw columns of the mixing matrix should be normalized
	 */
	مجازی خالی getNormalizeRPY(منطقی normalize[MAX_عدد_MATRICES]) ثابت
	{
		برائے (صحیح i = 0; i < MAX_عدد_MATRICES; ++i) {
			normalize[i] = جھوٹ;
		}
	}

	/**
	 * Get the control effectiveness matrix if updated
	 *
	 * @return true if updated and matrix is set
	 */
	مجازی منطقی get_Effectiveness_میٹرکس(Configuration &configuration, EffectivenessUpdateReason external_update) { واپس جھوٹ;}

	/**
	 * Get the current flight phase
	 *
	 * @return Flight phase
	 */
	ثابت FlightPhase &getFlightPhase() ثابت
	{
		واپس _flight_phase;
	}

	/**
	 * Display name
	 */
	مجازی ثابت حرف *name() ثابت = 0;

	/**
	 * Callback from the control allocation, allowing to manipulate the setpoint.
	 * Used to allocate auxiliary controls to actuators (e.g. flaps and spoilers).
	 *
	 * @param actuator_sp input & output setpoint
	 */
	مجازی خالی allocateAuxilaryControls(ثابت اعشاری dt, صحیح میٹرکس_اشاریہ, Actuator_سمتیہ &actuator_sp) {}

	/**
	 * Callback from the control allocation, allowing to manipulate the setpoint.
	 * This can be used to e.g. add non-linear or external terms.
	 * It is called after the matrix multiplication and before final clipping.
	 * @param actuator_sp input & output setpoint
	 */
	مجازی خالی updateSetpoint(ثابت میٹرکس::سمتیہ<اعشاری, عدد_AXES> &control_sp, صحیح میٹرکس_اشاریہ,
				    Actuator_سمتیہ &actuator_sp, ثابت Actuator_سمتیہ &actuator_min, ثابت Actuator_سمتیہ &actuator_max) {}

	/**
	 * Get a bitmask of motors to be stopped
	 */
	ActuatorBitmask getStoppedMotors() ثابت;

	/**
	 * Fill in the unallocated torque and thrust, customized by effectiveness type.
	 * Can be implemented for every type separately. If not implemented then the effectivenes matrix is used instead.
	 */
	مجازی خالی getUnallocatedControl(صحیح میٹرکس_اشاریہ, control_allocator_حالت_s &حالت_2) {}

	/**
	 * Override the collective tilt setpoint that would normally come from
	 * tiltrotor_extra_controls. Base implementation is a no-op.
	 *
	 * @param do_override When true, use @p collective_tilt instead of the uORB value.
	 * @param collective_tilt Normalised setpoint in [0, 1]. 0: vertical, 1: horizontal.
	 */
	مجازی خالی overrideCollectiveTilt(منطقی /*do_override*/, اعشاری /*collective_tilt*/) {}

	/**
	 * Record which components of the thrust setpoint are NaN, so that motors in that direction are stopped.
	 *
	 * @param thrust_sp The thrust setpoint as received by the allocator (instance zero - multicopter only)
	 */
	خالی stopMotorsBasedOnThrustSetpoint(ثابت میٹرکس::Vector3f &thrust_sp)
	{
		_longitudinal_motors_stopped_by_thrust = !PX4_ISFINITE(thrust_sp(0));
		_lateral_motors_stopped_by_thrust = !PX4_ISFINITE(thrust_sp(1));
		_vertical_motors_stopped_by_thrust = !PX4_ISFINITE(thrust_sp(2));
	}

محفوظ:

	ساخت MotorDirectionBitmasks {
		ActuatorBitmask longitudinal{};
		ActuatorBitmask lateral{};
		ActuatorBitmask vertical{};
	} _motor_direction_bitmasks;

	FlightPhase _flight_phase{FlightPhase::HOVER_FLIGHT};
	ActuatorBitmask stopped_motors_نقاب_due_مقصد_flight_phase{};

	منطقی _longitudinal_motors_stopped_by_thrust{جھوٹ};
	منطقی _vertical_motors_stopped_by_thrust{جھوٹ};
	منطقی _lateral_motors_stopped_by_thrust{جھوٹ};

};

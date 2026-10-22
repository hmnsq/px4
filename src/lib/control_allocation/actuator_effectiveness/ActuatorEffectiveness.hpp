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

#ਹਦਾਇਤ once

#ਅਨ੍ਤਰ੍ਭਾਵ <cstdint>

#ਅਨ੍ਤਰ੍ਭਾਵ <matrix/matrix/math.hpp>
#ਅਨ੍ਤਰ੍ਭਾਵ <uORB/topics/control_allocator_status.h>

ਗਿਣਤੀ ਵਰਗ ਵੰਡ_ਵਿਧੀ {
	NONE = -1,
	PSEUDO_INVERSE = 0,
	SEQUENTIAL_DESATURATION = 1,
	AUTO = 2,
};

ਗਿਣਤੀ ਵਰਗ ActuatorType {
	MOTORS = 0,
	SERVOS,

	ਗਿਣਤੀ_3
};

ਗਿਣਤੀ ਵਰਗ EffectivenessUpdateReason {
	NO_EXTERNAL_UPDATE = 0,
	CONFIGURATION_UPDATE = 1, ///< config changes (parameter)
	MOTOR_ACTIVATION_UPDATE = 2, ///< motor failure detected or certain redundant motors are switched off to save energy
};

ਵਰਗ ActuatorEffectiveness
{
ਜਨਤਕ:
	ActuatorEffectiveness() = ਮੂਲ;
	ਆਭਾਸੀ ~ActuatorEffectiveness() = ਮੂਲ;

	ਸਥਿਰ constexpr ਪੂਰਨ ਅੰਕ_ACTUATORS = 16;
	ਸਥਿਰ constexpr ਪੂਰਨ ਅੰਕ_AXES = 6;

	ਗਿਣਤੀ ControlAxis {
		ROLL = 0,
		PITCH,
		YAW,
		THRUST_X,
		THRUST_Y,
		THRUST_Z
	};

	ਸਥਿਰ constexpr ਪੂਰਨ MAX_ਅੰਕ_MATRICES = 2;

	ਵਰਤੋਂ Effectiveness_ਮੈਟ੍ਰਿਕਸ = ਮੈਟ੍ਰਿਕਸ::ਮੈਟ੍ਰਿਕਸ_2<ਦਸ਼ਮਲਵ, ਅੰਕ_AXES, ਅੰਕ_ACTUATORS>;
	ਵਰਤੋਂ Actuator_ਸਦਿਸ਼ = ਮੈਟ੍ਰਿਕਸ::ਸਦਿਸ਼<ਦਸ਼ਮਲਵ, ਅੰਕ_ACTUATORS>;
	ਵਰਤੋਂ ActuatorBitmask = uint32_t;

	static_assert(ਅੰਕ_ACTUATORS <= 8 * ਆਕਾਰ(ActuatorBitmask),
		      "NUM_ACTUATORS exceeds the number of bits available in the mask type.");

	ਗਿਣਤੀ ਵਰਗ FlightPhase {
		HOVER_FLIGHT = 0,
		FORWARD_FLIGHT = 1,
		TRANSITION_HF_ਮੰਜ਼ਿਲ_FF = 2,
		TRANSITION_FF_ਮੰਜ਼ਿਲ_HF = 3
	};

	ਬਣਤਰ Configuration {
		/**
		 * Add an actuator to the selected matrix, returning the index, or -1 on error
		 */
		ਪੂਰਨ ਜੋੜੋ_Actuator(ActuatorType type, ਅਚਲ ਮੈਟ੍ਰਿਕਸ::Vector3f &torque, ਅਚਲ ਮੈਟ੍ਰਿਕਸ::Vector3f &thrust);

		/**
		 * Call this after manually adding N actuators to the selected matrix
		 */
		ਖਾਲੀ actuatorsAdded(ActuatorType type, ਪੂਰਨ ਗਿਣਤੀ_2);

		ਪੂਰਨ ਕੁੱਲ_ਅੰਕ_Actuators() ਅਚਲ;

		/// Configured effectiveness matrix. Actuators are expected to be filled in order, motors first, then servos
		Effectiveness_ਮੈਟ੍ਰਿਕਸ effectiveness_matrices[MAX_ਅੰਕ_MATRICES];

		ਪੂਰਨ ਅੰਕ_actuators_ਮੈਟ੍ਰਿਕਸ[MAX_ਅੰਕ_MATRICES]; ///< current amount, and next actuator index to fill in to effectiveness_matrices
		Actuator_ਸਦਿਸ਼ trim[MAX_ਅੰਕ_MATRICES];

		Actuator_ਸਦਿਸ਼ linearization_ਬਿੰਦੂ[MAX_ਅੰਕ_MATRICES];

		ਪੂਰਨ selected_ਮੈਟ੍ਰਿਕਸ;

		uint8_t ਮੈਟ੍ਰਿਕਸ_selection_indexes[ਅੰਕ_ACTUATORS * MAX_ਅੰਕ_MATRICES];
		ਪੂਰਨ ਅੰਕ_actuators[(ਪੂਰਨ)ActuatorType::ਗਿਣਤੀ_3];
	};

	/**
	 * Set the current flight phase
	 *
	 * @param Flight phase
	 */
	ਆਭਾਸੀ ਖਾਲੀ ਸੈੱਟ_Flight_Phase(ਅਚਲ FlightPhase &flight_phase)
	{
		_flight_phase = flight_phase;
	}

	/**
	 * Get the number of effectiveness matrices. Must be <= MAX_NUM_MATRICES.
	 * This is expected to stay constant.
	 */
	ਆਭਾਸੀ ਪੂਰਨ ਅੰਕ_Matrices() ਅਚਲ { ਵਾਪਸ 1; }

	/**
	 * Get the desired allocation method(s) for each matrix, if configured as AUTO
	 */
	ਆਭਾਸੀ ਖਾਲੀ get_Desired_ਵੰਡ_ਵਿਧੀ(ਵੰਡ_ਵਿਧੀ ਵੰਡ_ਵਿਧੀ_ਬਾਹਰਲਾ[MAX_ਅੰਕ_MATRICES]) ਅਚਲ
	{
		ਲਈ (ਪੂਰਨ i = 0; i < MAX_ਅੰਕ_MATRICES; ++i) {
			ਵੰਡ_ਵਿਧੀ_ਬਾਹਰਲਾ[i] = ਵੰਡ_ਵਿਧੀ::PSEUDO_INVERSE;
		}
	}

	/**
	 * Query if the roll, pitch and yaw columns of the mixing matrix should be normalized
	 */
	ਆਭਾਸੀ ਖਾਲੀ getNormalizeRPY(ਤਾਰਕਿਕ normalize[MAX_ਅੰਕ_MATRICES]) ਅਚਲ
	{
		ਲਈ (ਪੂਰਨ i = 0; i < MAX_ਅੰਕ_MATRICES; ++i) {
			normalize[i] = ਝੂਠ;
		}
	}

	/**
	 * Get the control effectiveness matrix if updated
	 *
	 * @return true if updated and matrix is set
	 */
	ਆਭਾਸੀ ਤਾਰਕਿਕ get_Effectiveness_ਮੈਟ੍ਰਿਕਸ(Configuration &configuration, EffectivenessUpdateReason external_update) { ਵਾਪਸ ਝੂਠ;}

	/**
	 * Get the current flight phase
	 *
	 * @return Flight phase
	 */
	ਅਚਲ FlightPhase &getFlightPhase() ਅਚਲ
	{
		ਵਾਪਸ _flight_phase;
	}

	/**
	 * Display name
	 */
	ਆਭਾਸੀ ਅਚਲ ਅੱਖਰ *name() ਅਚਲ = 0;

	/**
	 * Callback from the control allocation, allowing to manipulate the setpoint.
	 * Used to allocate auxiliary controls to actuators (e.g. flaps and spoilers).
	 *
	 * @param actuator_sp input & output setpoint
	 */
	ਆਭਾਸੀ ਖਾਲੀ allocateAuxilaryControls(ਅਚਲ ਦਸ਼ਮਲਵ dt, ਪੂਰਨ ਮੈਟ੍ਰਿਕਸ_ਸੂਚਕਾਂਕ, Actuator_ਸਦਿਸ਼ &actuator_sp) {}

	/**
	 * Callback from the control allocation, allowing to manipulate the setpoint.
	 * This can be used to e.g. add non-linear or external terms.
	 * It is called after the matrix multiplication and before final clipping.
	 * @param actuator_sp input & output setpoint
	 */
	ਆਭਾਸੀ ਖਾਲੀ updateSetpoint(ਅਚਲ ਮੈਟ੍ਰਿਕਸ::ਸਦਿਸ਼<ਦਸ਼ਮਲਵ, ਅੰਕ_AXES> &control_sp, ਪੂਰਨ ਮੈਟ੍ਰਿਕਸ_ਸੂਚਕਾਂਕ,
				    Actuator_ਸਦਿਸ਼ &actuator_sp, ਅਚਲ Actuator_ਸਦਿਸ਼ &actuator_min, ਅਚਲ Actuator_ਸਦਿਸ਼ &actuator_max) {}

	/**
	 * Get a bitmask of motors to be stopped
	 */
	ActuatorBitmask getStoppedMotors() ਅਚਲ;

	/**
	 * Fill in the unallocated torque and thrust, customized by effectiveness type.
	 * Can be implemented for every type separately. If not implemented then the effectivenes matrix is used instead.
	 */
	ਆਭਾਸੀ ਖਾਲੀ getUnallocatedControl(ਪੂਰਨ ਮੈਟ੍ਰਿਕਸ_ਸੂਚਕਾਂਕ, control_allocator_ਸਥਿਤੀ_s &ਸਥਿਤੀ) {}

	/**
	 * Override the collective tilt setpoint that would normally come from
	 * tiltrotor_extra_controls. Base implementation is a no-op.
	 *
	 * @param do_override When true, use @p collective_tilt instead of the uORB value.
	 * @param collective_tilt Normalised setpoint in [0, 1]. 0: vertical, 1: horizontal.
	 */
	ਆਭਾਸੀ ਖਾਲੀ overrideCollectiveTilt(ਤਾਰਕਿਕ /*do_override*/, ਦਸ਼ਮਲਵ /*collective_tilt*/) {}

	/**
	 * Record which components of the thrust setpoint are NaN, so that motors in that direction are stopped.
	 *
	 * @param thrust_sp The thrust setpoint as received by the allocator (instance zero - multicopter only)
	 */
	ਖਾਲੀ stopMotorsBasedOnThrustSetpoint(ਅਚਲ ਮੈਟ੍ਰਿਕਸ::Vector3f &thrust_sp)
	{
		_longitudinal_motors_stopped_by_thrust = !PX4_ISFINITE(thrust_sp(0));
		_lateral_motors_stopped_by_thrust = !PX4_ISFINITE(thrust_sp(1));
		_vertical_motors_stopped_by_thrust = !PX4_ISFINITE(thrust_sp(2));
	}

ਸੁਰੱਖਿਅਤ:

	ਬਣਤਰ MotorDirectionBitmasks {
		ActuatorBitmask longitudinal{};
		ActuatorBitmask lateral{};
		ActuatorBitmask vertical{};
	} _motor_direction_bitmasks;

	FlightPhase _flight_phase{FlightPhase::HOVER_FLIGHT};
	ActuatorBitmask stopped_motors_ਨਕਾਬ_due_ਮੰਜ਼ਿਲ_flight_phase{};

	ਤਾਰਕਿਕ _longitudinal_motors_stopped_by_thrust{ਝੂਠ};
	ਤਾਰਕਿਕ _vertical_motors_stopped_by_thrust{ਝੂਠ};
	ਤਾਰਕਿਕ _lateral_motors_stopped_by_thrust{ਝੂਠ};

};

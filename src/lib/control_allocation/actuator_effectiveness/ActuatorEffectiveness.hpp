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

#ಸೂಚನೆ once

#ಅನ್ತರ್ಭಾವ <cstdint>

#ಅನ್ತರ್ಭಾವ <matrix/matrix/math.hpp>
#ಅನ್ತರ್ಭಾವ <uORB/topics/control_allocator_status.h>

ಎಣಿಕೆ ವರ್ಗ ಹಂಚಿಕೆ_ವಿಧಾನ {
	NONE = -1,
	PSEUDO_INVERSE = 0,
	SEQUENTIAL_DESATURATION = 1,
	AUTO = 2,
};

ಎಣಿಕೆ ವರ್ಗ ActuatorType {
	MOTORS = 0,
	SERVOS,

	ಎಣಿಕೆ_3
};

ಎಣಿಕೆ ವರ್ಗ EffectivenessUpdateReason {
	NO_EXTERNAL_UPDATE = 0,
	CONFIGURATION_UPDATE = 1, ///< config changes (parameter)
	MOTOR_ACTIVATION_UPDATE = 2, ///< motor failure detected or certain redundant motors are switched off to save energy
};

ವರ್ಗ ActuatorEffectiveness
{
ಸಾರ್ವಜನಿಕ:
	ActuatorEffectiveness() = ಪೂರ್ವನಿಯೋಜಿತ;
	ವಾಸ್ತವಿಕ ~ActuatorEffectiveness() = ಪೂರ್ವನಿಯೋಜಿತ;

	ಸ್ಥಿರ constexpr ಪೂರ್ಣಾಂಕ ಅಂಕೆ_ACTUATORS = 16;
	ಸ್ಥಿರ constexpr ಪೂರ್ಣಾಂಕ ಅಂಕೆ_AXES = 6;

	ಎಣಿಕೆ ControlAxis {
		ROLL = 0,
		PITCH,
		YAW,
		THRUST_X,
		THRUST_Y,
		THRUST_Z
	};

	ಸ್ಥಿರ constexpr ಪೂರ್ಣಾಂಕ MAX_ಅಂಕೆ_MATRICES = 2;

	ಬಳಸಿ Effectiveness_ಮಾತೃಕೆ = ಮಾತೃಕೆ::ಮಾತೃಕೆ_2<ದಶಮಾಂಶ, ಅಂಕೆ_AXES, ಅಂಕೆ_ACTUATORS>;
	ಬಳಸಿ Actuator_ಸದಿಶ = ಮಾತೃಕೆ::ಸದಿಶ<ದಶಮಾಂಶ, ಅಂಕೆ_ACTUATORS>;
	ಬಳಸಿ ActuatorBitmask = uint32_t;

	static_assert(ಅಂಕೆ_ACTUATORS <= 8 * ಗಾತ್ರ(ActuatorBitmask),
		      "NUM_ACTUATORS exceeds the number of bits available in the mask type.");

	ಎಣಿಕೆ ವರ್ಗ FlightPhase {
		HOVER_FLIGHT = 0,
		FORWARD_FLIGHT = 1,
		TRANSITION_HF_ಗಮ್ಯ_FF = 2,
		TRANSITION_FF_ಗಮ್ಯ_HF = 3
	};

	ರಚನೆ Configuration {
		/**
		 * Add an actuator to the selected matrix, returning the index, or -1 on error
		 */
		ಪೂರ್ಣಾಂಕ ಕೂಡಿಸು_Actuator(ActuatorType type, ಸ್ಥಿರಾಂಕ ಮಾತೃಕೆ::Vector3f &torque, ಸ್ಥಿರಾಂಕ ಮಾತೃಕೆ::Vector3f &thrust);

		/**
		 * Call this after manually adding N actuators to the selected matrix
		 */
		ಶೂನ್ಯ actuatorsAdded(ActuatorType type, ಪೂರ್ಣಾಂಕ ಎಣಿಕೆ_2);

		ಪೂರ್ಣಾಂಕ ಒಟ್ಟು_ಅಂಕೆ_Actuators() ಸ್ಥಿರಾಂಕ;

		/// Configured effectiveness matrix. Actuators are expected to be filled in order, motors first, then servos
		Effectiveness_ಮಾತೃಕೆ effectiveness_matrices[MAX_ಅಂಕೆ_MATRICES];

		ಪೂರ್ಣಾಂಕ ಅಂಕೆ_actuators_ಮಾತೃಕೆ[MAX_ಅಂಕೆ_MATRICES]; ///< current amount, and next actuator index to fill in to effectiveness_matrices
		Actuator_ಸದಿಶ trim[MAX_ಅಂಕೆ_MATRICES];

		Actuator_ಸದಿಶ linearization_ಬಿಂದು[MAX_ಅಂಕೆ_MATRICES];

		ಪೂರ್ಣಾಂಕ selected_ಮಾತೃಕೆ;

		uint8_t ಮಾತೃಕೆ_selection_indexes[ಅಂಕೆ_ACTUATORS * MAX_ಅಂಕೆ_MATRICES];
		ಪೂರ್ಣಾಂಕ ಅಂಕೆ_actuators[(ಪೂರ್ಣಾಂಕ)ActuatorType::ಎಣಿಕೆ_3];
	};

	/**
	 * Set the current flight phase
	 *
	 * @param Flight phase
	 */
	ವಾಸ್ತವಿಕ ಶೂನ್ಯ ಗಣ_Flight_Phase(ಸ್ಥಿರಾಂಕ FlightPhase &flight_phase)
	{
		_flight_phase = flight_phase;
	}

	/**
	 * Get the number of effectiveness matrices. Must be <= MAX_NUM_MATRICES.
	 * This is expected to stay constant.
	 */
	ವಾಸ್ತವಿಕ ಪೂರ್ಣಾಂಕ ಅಂಕೆ_Matrices() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ 1; }

	/**
	 * Get the desired allocation method(s) for each matrix, if configured as AUTO
	 */
	ವಾಸ್ತವಿಕ ಶೂನ್ಯ get_Desired_ಹಂಚಿಕೆ_ವಿಧಾನ(ಹಂಚಿಕೆ_ವಿಧಾನ ಹಂಚಿಕೆ_ವಿಧಾನ_ಹೊರಹೋಗು[MAX_ಅಂಕೆ_MATRICES]) ಸ್ಥಿರಾಂಕ
	{
		ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < MAX_ಅಂಕೆ_MATRICES; ++i) {
			ಹಂಚಿಕೆ_ವಿಧಾನ_ಹೊರಹೋಗು[i] = ಹಂಚಿಕೆ_ವಿಧಾನ::PSEUDO_INVERSE;
		}
	}

	/**
	 * Query if the roll, pitch and yaw columns of the mixing matrix should be normalized
	 */
	ವಾಸ್ತವಿಕ ಶೂನ್ಯ getNormalizeRPY(ತಾರ್ಕಿಕ normalize[MAX_ಅಂಕೆ_MATRICES]) ಸ್ಥಿರಾಂಕ
	{
		ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < MAX_ಅಂಕೆ_MATRICES; ++i) {
			normalize[i] = ಸುಳ್ಳು;
		}
	}

	/**
	 * Get the control effectiveness matrix if updated
	 *
	 * @return true if updated and matrix is set
	 */
	ವಾಸ್ತವಿಕ ತಾರ್ಕಿಕ get_Effectiveness_ಮಾತೃಕೆ(Configuration &configuration, EffectivenessUpdateReason external_update) { ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;}

	/**
	 * Get the current flight phase
	 *
	 * @return Flight phase
	 */
	ಸ್ಥಿರಾಂಕ FlightPhase &getFlightPhase() ಸ್ಥಿರಾಂಕ
	{
		ಹಿಂತಿರುಗಿ _flight_phase;
	}

	/**
	 * Display name
	 */
	ವಾಸ್ತವಿಕ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name() ಸ್ಥಿರಾಂಕ = 0;

	/**
	 * Callback from the control allocation, allowing to manipulate the setpoint.
	 * Used to allocate auxiliary controls to actuators (e.g. flaps and spoilers).
	 *
	 * @param actuator_sp input & output setpoint
	 */
	ವಾಸ್ತವಿಕ ಶೂನ್ಯ allocateAuxilaryControls(ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ dt, ಪೂರ್ಣಾಂಕ ಮಾತೃಕೆ_ಸೂಚ್ಯಂಕ, Actuator_ಸದಿಶ &actuator_sp) {}

	/**
	 * Callback from the control allocation, allowing to manipulate the setpoint.
	 * This can be used to e.g. add non-linear or external terms.
	 * It is called after the matrix multiplication and before final clipping.
	 * @param actuator_sp input & output setpoint
	 */
	ವಾಸ್ತವಿಕ ಶೂನ್ಯ updateSetpoint(ಸ್ಥಿರಾಂಕ ಮಾತೃಕೆ::ಸದಿಶ<ದಶಮಾಂಶ, ಅಂಕೆ_AXES> &control_sp, ಪೂರ್ಣಾಂಕ ಮಾತೃಕೆ_ಸೂಚ್ಯಂಕ,
				    Actuator_ಸದಿಶ &actuator_sp, ಸ್ಥಿರಾಂಕ Actuator_ಸದಿಶ &actuator_min, ಸ್ಥಿರಾಂಕ Actuator_ಸದಿಶ &actuator_max) {}

	/**
	 * Get a bitmask of motors to be stopped
	 */
	ActuatorBitmask getStoppedMotors() ಸ್ಥಿರಾಂಕ;

	/**
	 * Fill in the unallocated torque and thrust, customized by effectiveness type.
	 * Can be implemented for every type separately. If not implemented then the effectivenes matrix is used instead.
	 */
	ವಾಸ್ತವಿಕ ಶೂನ್ಯ getUnallocatedControl(ಪೂರ್ಣಾಂಕ ಮಾತೃಕೆ_ಸೂಚ್ಯಂಕ, control_allocator_ಸ್ಥಿತಿ_s &ಸ್ಥಿತಿ_2) {}

	/**
	 * Override the collective tilt setpoint that would normally come from
	 * tiltrotor_extra_controls. Base implementation is a no-op.
	 *
	 * @param do_override When true, use @p collective_tilt instead of the uORB value.
	 * @param collective_tilt Normalised setpoint in [0, 1]. 0: vertical, 1: horizontal.
	 */
	ವಾಸ್ತವಿಕ ಶೂನ್ಯ overrideCollectiveTilt(ತಾರ್ಕಿಕ /*do_override*/, ದಶಮಾಂಶ /*collective_tilt*/) {}

	/**
	 * Record which components of the thrust setpoint are NaN, so that motors in that direction are stopped.
	 *
	 * @param thrust_sp The thrust setpoint as received by the allocator (instance zero - multicopter only)
	 */
	ಶೂನ್ಯ stopMotorsBasedOnThrustSetpoint(ಸ್ಥಿರಾಂಕ ಮಾತೃಕೆ::Vector3f &thrust_sp)
	{
		_longitudinal_motors_stopped_by_thrust = !PX4_ISFINITE(thrust_sp(0));
		_lateral_motors_stopped_by_thrust = !PX4_ISFINITE(thrust_sp(1));
		_vertical_motors_stopped_by_thrust = !PX4_ISFINITE(thrust_sp(2));
	}

ಸಂರಕ್ಷಿತ:

	ರಚನೆ MotorDirectionBitmasks {
		ActuatorBitmask longitudinal{};
		ActuatorBitmask lateral{};
		ActuatorBitmask vertical{};
	} _motor_direction_bitmasks;

	FlightPhase _flight_phase{FlightPhase::HOVER_FLIGHT};
	ActuatorBitmask stopped_motors_ಮುಖವಾಡ_due_ಗಮ್ಯ_flight_phase{};

	ತಾರ್ಕಿಕ _longitudinal_motors_stopped_by_thrust{ಸುಳ್ಳು};
	ತಾರ್ಕಿಕ _vertical_motors_stopped_by_thrust{ಸುಳ್ಳು};
	ತಾರ್ಕಿಕ _lateral_motors_stopped_by_thrust{ಸುಳ್ಳು};

};

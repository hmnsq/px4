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
 * @file ControlAllocationSequentialDesaturation.cpp
 *
 * @author Roman Bapst <bapstroman@gmail.com>
 * @author Beat Küng <beat-kueng@gmx.net>
 */

#ಅನ್ತರ್ಭಾವ "ControlAllocationSequentialDesaturation.hpp"


ಶೂನ್ಯ
Control_ಹಂಚಿಕೆ_Sequential_Desaturation::allocate()
{
	//Compute new gains if needed
	updatePseudoInverse();

	_prev_actuator_sp = _actuator_sp;

	ಆಯ್ಕೆ (_param_mc_airmode.get()) {
	ಸ್ಥಿತಿ 1:
		mixAirmodeRP();
		ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ 2:
		mixAirmodeRPY();
		ನಿಲ್ಲಿಸು;

	ಪೂರ್ವನಿಯೋಜಿತ:
		mixAirmodeDisabled();
		ನಿಲ್ಲಿಸು;
	}
}

ಶೂನ್ಯ Control_ಹಂಚಿಕೆ_Sequential_Desaturation::desaturateActuators(
	Actuator_ಸದಿಶ &actuator_sp,
	ಸ್ಥಿರಾಂಕ Actuator_ಸದಿಶ &desaturation_ಸದಿಶ, ತಾರ್ಕಿಕ increase_only)
{
	ದಶಮಾಂಶ gain = computeDesaturationGain(desaturation_ಸದಿಶ, actuator_sp);

	ಆದರೆ (increase_only && gain < 0.f) {
		ಹಿಂತಿರುಗಿ;
	}

	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < ಅಂಕೆ_actuators_2; i++) {
		actuator_sp(i) += gain * desaturation_ಸದಿಶ(i);
	}

	gain = 0.5f * computeDesaturationGain(desaturation_ಸದಿಶ, actuator_sp);

	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < ಅಂಕೆ_actuators_2; i++) {
		actuator_sp(i) += gain * desaturation_ಸದಿಶ(i);
	}
}

ದಶಮಾಂಶ Control_ಹಂಚಿಕೆ_Sequential_Desaturation::computeDesaturationGain(ಸ್ಥಿರಾಂಕ Actuator_ಸದಿಶ &desaturation_ಸದಿಶ,
		ಸ್ಥಿರಾಂಕ Actuator_ಸದಿಶ &actuator_sp)
{
	ದಶಮಾಂಶ k_min = 0.f;
	ದಶಮಾಂಶ k_max = 0.f;

	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < ಅಂಕೆ_actuators_2; i++) {
		// Do not use try to desaturate using an actuator with weak effectiveness to avoid large desaturation gains
		ಆದರೆ (fabsf(desaturation_ಸದಿಶ(i)) < 0.2f) {
			ಮುಂದುವರಿ;
		}

		ಆದರೆ (actuator_sp(i) < _actuator_min(i)) {
			ದಶಮಾಂಶ k = (_actuator_min(i) - actuator_sp(i)) / desaturation_ಸದಿಶ(i);

			ಆದರೆ (k < k_min) { k_min = k; }

			ಆದರೆ (k > k_max) { k_max = k; }
		}

		ಆದರೆ (actuator_sp(i) > _actuator_max(i)) {
			ದಶಮಾಂಶ k = (_actuator_max(i) - actuator_sp(i)) / desaturation_ಸದಿಶ(i);

			ಆದರೆ (k < k_min) { k_min = k; }

			ಆದರೆ (k > k_max) { k_max = k; }
		}
	}

	// Reduce the saturation as much as possible
	ಹಿಂತಿರುಗಿ k_min + k_max;
}

ಶೂನ್ಯ
Control_ಹಂಚಿಕೆ_Sequential_Desaturation::mixAirmodeRP()
{
	// Airmode for roll and pitch, but not yaw

	// Mix without yaw
	Actuator_ಸದಿಶ thrust_z;

	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < ಅಂಕೆ_actuators_2; i++) {
		_actuator_sp(i) = _actuator_trim(i) +
				  _mix(i, ControlAxis::ROLL) * (_control_sp(ControlAxis::ROLL) - _control_trim(ControlAxis::ROLL)) +
				  _mix(i, ControlAxis::PITCH) * (_control_sp(ControlAxis::PITCH) - _control_trim(ControlAxis::PITCH)) +
				  _mix(i, ControlAxis::THRUST_X) * (_control_sp(ControlAxis::THRUST_X) - _control_trim(ControlAxis::THRUST_X)) +
				  _mix(i, ControlAxis::THRUST_Y) * (_control_sp(ControlAxis::THRUST_Y) - _control_trim(ControlAxis::THRUST_Y)) +
				  _mix(i, ControlAxis::THRUST_Z) * (_control_sp(ControlAxis::THRUST_Z) - _control_trim(ControlAxis::THRUST_Z));
		thrust_z(i) = _mix(i, ControlAxis::THRUST_Z);
	}

	desaturateActuators(_actuator_sp, thrust_z);

	// Mix yaw independently
	mixYaw();
}

ಶೂನ್ಯ
Control_ಹಂಚಿಕೆ_Sequential_Desaturation::mixAirmodeRPY()
{
	// Airmode for roll, pitch and yaw

	// Do full mixing
	Actuator_ಸದಿಶ thrust_z;
	Actuator_ಸದಿಶ yaw;

	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < ಅಂಕೆ_actuators_2; i++) {
		_actuator_sp(i) = _actuator_trim(i) +
				  _mix(i, ControlAxis::ROLL) * (_control_sp(ControlAxis::ROLL) - _control_trim(ControlAxis::ROLL)) +
				  _mix(i, ControlAxis::PITCH) * (_control_sp(ControlAxis::PITCH) - _control_trim(ControlAxis::PITCH)) +
				  _mix(i, ControlAxis::YAW) * (_control_sp(ControlAxis::YAW) - _control_trim(ControlAxis::YAW)) +
				  _mix(i, ControlAxis::THRUST_X) * (_control_sp(ControlAxis::THRUST_X) - _control_trim(ControlAxis::THRUST_X)) +
				  _mix(i, ControlAxis::THRUST_Y) * (_control_sp(ControlAxis::THRUST_Y) - _control_trim(ControlAxis::THRUST_Y)) +
				  _mix(i, ControlAxis::THRUST_Z) * (_control_sp(ControlAxis::THRUST_Z) - _control_trim(ControlAxis::THRUST_Z));
		thrust_z(i) = _mix(i, ControlAxis::THRUST_Z);
		yaw(i) = _mix(i, ControlAxis::YAW);
	}

	desaturateActuators(_actuator_sp, thrust_z);

	// Unsaturate yaw (in case upper and lower bounds are exceeded)
	// to prioritize roll/pitch over yaw.
	desaturateActuators(_actuator_sp, yaw);
}

ಶೂನ್ಯ
Control_ಹಂಚಿಕೆ_Sequential_Desaturation::mixAirmodeDisabled()
{
	// Airmode disabled: never allow to increase the thrust to unsaturate a motor

	// Mix without yaw
	Actuator_ಸದಿಶ thrust_z;
	Actuator_ಸದಿಶ roll;
	Actuator_ಸದಿಶ pitch;

	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < ಅಂಕೆ_actuators_2; i++) {
		_actuator_sp(i) = _actuator_trim(i) +
				  _mix(i, ControlAxis::ROLL) * (_control_sp(ControlAxis::ROLL) - _control_trim(ControlAxis::ROLL)) +
				  _mix(i, ControlAxis::PITCH) * (_control_sp(ControlAxis::PITCH) - _control_trim(ControlAxis::PITCH)) +
				  _mix(i, ControlAxis::THRUST_X) * (_control_sp(ControlAxis::THRUST_X) - _control_trim(ControlAxis::THRUST_X)) +
				  _mix(i, ControlAxis::THRUST_Y) * (_control_sp(ControlAxis::THRUST_Y) - _control_trim(ControlAxis::THRUST_Y)) +
				  _mix(i, ControlAxis::THRUST_Z) * (_control_sp(ControlAxis::THRUST_Z) - _control_trim(ControlAxis::THRUST_Z));
		thrust_z(i) = _mix(i, ControlAxis::THRUST_Z);
		roll(i) = _mix(i, ControlAxis::ROLL);
		pitch(i) = _mix(i, ControlAxis::PITCH);
	}

	// only reduce thrust
	desaturateActuators(_actuator_sp, thrust_z, ನಿಜ);

	// Reduce roll/pitch acceleration if needed to unsaturate
	desaturateActuators(_actuator_sp, roll);
	desaturateActuators(_actuator_sp, pitch);

	// Mix yaw independently
	mixYaw();
}

ಶೂನ್ಯ
Control_ಹಂಚಿಕೆ_Sequential_Desaturation::mixYaw()
{
	// Add yaw to outputs
	Actuator_ಸದಿಶ yaw;
	Actuator_ಸದಿಶ thrust_z;

	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < ಅಂಕೆ_actuators_2; i++) {
		_actuator_sp(i) += _mix(i, ControlAxis::YAW) * (_control_sp(ControlAxis::YAW) - _control_trim(ControlAxis::YAW));
		yaw(i) = _mix(i, ControlAxis::YAW);
		thrust_z(i) = _mix(i, ControlAxis::THRUST_Z);
	}

	// Change yaw acceleration to unsaturate the outputs if needed (do not change roll/pitch),
	// and allow some yaw response at maximum thrust
	Actuator_ಸದಿಶ max_prev = _actuator_max;
	_actuator_max += (_actuator_max - _actuator_min) * MINIMUM_YAW_MARGIN;
	desaturateActuators(_actuator_sp, yaw);
	_actuator_max = max_prev;

	// reduce thrust only
	desaturateActuators(_actuator_sp, thrust_z, ನಿಜ);
}

ಶೂನ್ಯ
Control_ಹಂಚಿಕೆ_Sequential_Desaturation::updateParameters()
{
	updateParams();
}

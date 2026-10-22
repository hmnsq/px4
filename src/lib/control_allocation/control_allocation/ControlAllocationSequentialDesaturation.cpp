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

#شامل "ControlAllocationSequentialDesaturation.hpp"


خالی
Control_تخصیص_Sequential_Desaturation::allocate()
{
	//Compute new gains if needed
	updatePseudoInverse();

	پچھلا_actuator_sp = _actuator_sp;

	انتخاب (_param_mc_airmode.get()) {
	حالت 1:
		mixAirmodeRP();
		توڑو;

	حالت 2:
		mixAirmodeRPY();
		توڑو;

	طےشدہ:
		mixAirmodeDisabled();
		توڑو;
	}
}

خالی Control_تخصیص_Sequential_Desaturation::desaturateActuators(
	Actuator_سمتیہ &actuator_sp,
	ثابت Actuator_سمتیہ &desaturation_سمتیہ, منطقی increase_only)
{
	اعشاری gain = computeDesaturationGain(desaturation_سمتیہ, actuator_sp);

	اگر (increase_only && gain < 0.f) {
		واپس;
	}

	برائے (صحیح i = 0; i < عدد_actuators_2; i++) {
		actuator_sp(i) += gain * desaturation_سمتیہ(i);
	}

	gain = 0.5f * computeDesaturationGain(desaturation_سمتیہ, actuator_sp);

	برائے (صحیح i = 0; i < عدد_actuators_2; i++) {
		actuator_sp(i) += gain * desaturation_سمتیہ(i);
	}
}

اعشاری Control_تخصیص_Sequential_Desaturation::computeDesaturationGain(ثابت Actuator_سمتیہ &desaturation_سمتیہ,
		ثابت Actuator_سمتیہ &actuator_sp)
{
	اعشاری k_min = 0.f;
	اعشاری k_max = 0.f;

	برائے (صحیح i = 0; i < عدد_actuators_2; i++) {
		// Do not use try to desaturate using an actuator with weak effectiveness to avoid large desaturation gains
		اگر (fabsf(desaturation_سمتیہ(i)) < 0.2f) {
			جاری;
		}

		اگر (actuator_sp(i) < _actuator_min(i)) {
			اعشاری k = (_actuator_min(i) - actuator_sp(i)) / desaturation_سمتیہ(i);

			اگر (k < k_min) { k_min = k; }

			اگر (k > k_max) { k_max = k; }
		}

		اگر (actuator_sp(i) > _actuator_max(i)) {
			اعشاری k = (_actuator_max(i) - actuator_sp(i)) / desaturation_سمتیہ(i);

			اگر (k < k_min) { k_min = k; }

			اگر (k > k_max) { k_max = k; }
		}
	}

	// Reduce the saturation as much as possible
	واپس k_min + k_max;
}

خالی
Control_تخصیص_Sequential_Desaturation::mixAirmodeRP()
{
	// Airmode for roll and pitch, but not yaw

	// Mix without yaw
	Actuator_سمتیہ thrust_z;

	برائے (صحیح i = 0; i < عدد_actuators_2; i++) {
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

خالی
Control_تخصیص_Sequential_Desaturation::mixAirmodeRPY()
{
	// Airmode for roll, pitch and yaw

	// Do full mixing
	Actuator_سمتیہ thrust_z;
	Actuator_سمتیہ yaw;

	برائے (صحیح i = 0; i < عدد_actuators_2; i++) {
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

خالی
Control_تخصیص_Sequential_Desaturation::mixAirmodeDisabled()
{
	// Airmode disabled: never allow to increase the thrust to unsaturate a motor

	// Mix without yaw
	Actuator_سمتیہ thrust_z;
	Actuator_سمتیہ roll;
	Actuator_سمتیہ pitch;

	برائے (صحیح i = 0; i < عدد_actuators_2; i++) {
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
	desaturateActuators(_actuator_sp, thrust_z, سچ);

	// Reduce roll/pitch acceleration if needed to unsaturate
	desaturateActuators(_actuator_sp, roll);
	desaturateActuators(_actuator_sp, pitch);

	// Mix yaw independently
	mixYaw();
}

خالی
Control_تخصیص_Sequential_Desaturation::mixYaw()
{
	// Add yaw to outputs
	Actuator_سمتیہ yaw;
	Actuator_سمتیہ thrust_z;

	برائے (صحیح i = 0; i < عدد_actuators_2; i++) {
		_actuator_sp(i) += _mix(i, ControlAxis::YAW) * (_control_sp(ControlAxis::YAW) - _control_trim(ControlAxis::YAW));
		yaw(i) = _mix(i, ControlAxis::YAW);
		thrust_z(i) = _mix(i, ControlAxis::THRUST_Z);
	}

	// Change yaw acceleration to unsaturate the outputs if needed (do not change roll/pitch),
	// and allow some yaw response at maximum thrust
	Actuator_سمتیہ max_پچھلا = _actuator_max;
	_actuator_max += (_actuator_max - _actuator_min) * MINIMUM_YAW_MARGIN;
	desaturateActuators(_actuator_sp, yaw);
	_actuator_max = max_پچھلا;

	// reduce thrust only
	desaturateActuators(_actuator_sp, thrust_z, سچ);
}

خالی
Control_تخصیص_Sequential_Desaturation::updateParameters()
{
	updateParams();
}

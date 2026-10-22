/****************************************************************************
 *
 *   Copyright (c) 2021-2022 PX4 Development Team. All rights reserved.
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

#pragma once

#ضمن "FunctionProviderBase.hpp"

#ضمن <math.h>
#ضمن <uORB/topics/vehicle_command.h>

/**
 * Functions: Peripheral_via_Actuator_Set1 ... Peripheral_via_Actuator_Set6
 */
صنف FunctionActuatorSet : عام FunctionProviderBase
{
عام:
	FunctionActuatorSet()
	{
		لكل (صحيح i = 0; i < max_عدد_actuators; ++i) {
			_data[i] = NAN;
		}
	}

	ساكن FunctionProviderBase *allocate(ثابت Context &context) { أرجع جديد FunctionActuatorSet(); }

	فراغ update() override
	{
		vehicle_command_s vehicle_command;

		طالما (_topic.update(&vehicle_command)) {
			إذا (vehicle_command.command == vehicle_command_s::VEHICLE_CMD_DO_SET_ACTUATOR) {
				صحيح index = static_cast<صحيح>(lroundf(vehicle_command.param7));

				إذا (index == 0) {
					إذا (PX4_ISFINITE(vehicle_command.param1)) { _data[0] = vehicle_command.param1; }

					إذا (PX4_ISFINITE(vehicle_command.param2)) {_data[1] = vehicle_command.param2; }

					إذا (PX4_ISFINITE(vehicle_command.param3)) {_data[2] = vehicle_command.param3; }

					إذا (PX4_ISFINITE(vehicle_command.param4)) {_data[3] = vehicle_command.param4; }

					إذا (PX4_ISFINITE(vehicle_command.param5)) {_data[4] = vehicle_command.param5; }

					إذا (PX4_ISFINITE(vehicle_command.param6)) {_data[5] = vehicle_command.param6; }
				}
			}
		}
	}

	عائم value(OutputFunction func) override { أرجع _data[(صحيح)func - (صحيح)OutputFunction::Peripheral_via_Actuator_Set1]; }

خاص:
	ساكن constexpr صحيح max_عدد_actuators = 6;

	uORB::Subscription _topic{ORB_ID(vehicle_command)};
	عائم _data[max_عدد_actuators] {};
};

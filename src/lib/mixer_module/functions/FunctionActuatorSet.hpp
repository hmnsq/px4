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

#અન્તર્ભાવ "FunctionProviderBase.hpp"

#અન્તર્ભાવ <math.h>
#અન્તર્ભાવ <uORB/topics/vehicle_command.h>

/**
 * Functions: Peripheral_via_Actuator_Set1 ... Peripheral_via_Actuator_Set6
 */
વર્ગ FunctionActuatorSet : જાહેર FunctionProviderBase
{
જાહેર:
	FunctionActuatorSet()
	{
		માટે (પૂર્ણાંક i = 0; i < max_અંક_actuators; ++i) {
			_data[i] = NAN;
		}
	}

	સ્થિર FunctionProviderBase *allocate(અચલ Context &context) { પાછા નવું FunctionActuatorSet(); }

	શૂન્ય update() override
	{
		vehicle_command_s vehicle_command;

		જ્યારે (_topic.update(&vehicle_command)) {
			જો (vehicle_command.command == vehicle_command_s::VEHICLE_CMD_DO_SET_ACTUATOR) {
				પૂર્ણાંક index = static_cast<પૂર્ણાંક>(lroundf(vehicle_command.param7));

				જો (index == 0) {
					જો (PX4_ISFINITE(vehicle_command.param1)) { _data[0] = vehicle_command.param1; }

					જો (PX4_ISFINITE(vehicle_command.param2)) {_data[1] = vehicle_command.param2; }

					જો (PX4_ISFINITE(vehicle_command.param3)) {_data[2] = vehicle_command.param3; }

					જો (PX4_ISFINITE(vehicle_command.param4)) {_data[3] = vehicle_command.param4; }

					જો (PX4_ISFINITE(vehicle_command.param5)) {_data[4] = vehicle_command.param5; }

					જો (PX4_ISFINITE(vehicle_command.param6)) {_data[5] = vehicle_command.param6; }
				}
			}
		}
	}

	દશાંશ value(OutputFunction func) override { પાછા _data[(પૂર્ણાંક)func - (પૂર્ણાંક)OutputFunction::Peripheral_via_Actuator_Set1]; }

ખાનગી:
	સ્થિર constexpr પૂર્ણાંક max_અંક_actuators = 6;

	uORB::Subscription _topic{ORB_ID(vehicle_command)};
	દશાંશ _data[max_અંક_actuators] {};
};

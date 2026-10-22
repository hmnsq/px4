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

#अन्तर्भाव "FunctionProviderBase.hpp"

#अन्तर्भाव <math.h>
#अन्तर्भाव <uORB/topics/vehicle_command.h>

/**
 * Functions: Peripheral_via_Actuator_Set1 ... Peripheral_via_Actuator_Set6
 */
वर्ग FunctionActuatorSet : सार्वजनिक FunctionProviderBase
{
सार्वजनिक:
	FunctionActuatorSet()
	{
		साठी (पूर्णांक i = 0; i < max_अंक_actuators; ++i) {
			_data[i] = न_अंक;
		}
	}

	स्थिर FunctionProviderBase *allocate(स्थिरांक Context &context) { परत नवीन FunctionActuatorSet(); }

	शून्य update() override
	{
		vehicle_command_s vehicle_command;

		पर्यंत (_topic.update(&vehicle_command)) {
			जर (vehicle_command.command == vehicle_command_s::VEHICLE_CMD_DO_SET_ACTUATOR) {
				पूर्णांक index = static_cast<पूर्णांक>(lroundf(vehicle_command.param7));

				जर (index == 0) {
					जर (PX4_ISFINITE(vehicle_command.param1)) { _data[0] = vehicle_command.param1; }

					जर (PX4_ISFINITE(vehicle_command.param2)) {_data[1] = vehicle_command.param2; }

					जर (PX4_ISFINITE(vehicle_command.param3)) {_data[2] = vehicle_command.param3; }

					जर (PX4_ISFINITE(vehicle_command.param4)) {_data[3] = vehicle_command.param4; }

					जर (PX4_ISFINITE(vehicle_command.param5)) {_data[4] = vehicle_command.param5; }

					जर (PX4_ISFINITE(vehicle_command.param6)) {_data[5] = vehicle_command.param6; }
				}
			}
		}
	}

	दशांश value(OutputFunction func) override { परत _data[(पूर्णांक)func - (पूर्णांक)OutputFunction::Peripheral_via_Actuator_Set1]; }

खाजगी:
	स्थिर constexpr पूर्णांक max_अंक_actuators = 6;

	uORB::Subscription _topic{ORB_ID(vehicle_command)};
	दशांश _data[max_अंक_actuators] {};
};

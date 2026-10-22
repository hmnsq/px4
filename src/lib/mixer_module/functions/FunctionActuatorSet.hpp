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

#ଅନ୍ତର୍ଭାଵ "FunctionProviderBase.hpp"

#ଅନ୍ତର୍ଭାଵ <math.h>
#ଅନ୍ତର୍ଭାଵ <uORB/topics/vehicle_command.h>

/**
 * Functions: Peripheral_via_Actuator_Set1 ... Peripheral_via_Actuator_Set6
 */
ଶ୍ରେଣୀ FunctionActuatorSet : ସର୍ବସାଧାରଣ FunctionProviderBase
{
ସର୍ବସାଧାରଣ:
	FunctionActuatorSet()
	{
		ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < max_ଅଙ୍କ_actuators; ++i) {
			_data[i] = NAN;
		}
	}

	ସ୍ଥିର FunctionProviderBase *allocate(ଅଚଳ Context &context) { ଫେରନ୍ତୁ ନୂତନ FunctionActuatorSet(); }

	ଶୂନ୍ୟ update() override
	{
		vehicle_command_s vehicle_command;

		ଯେତେବେଳେ (_topic.update(&vehicle_command)) {
			ଯଦି (vehicle_command.command == vehicle_command_s::VEHICLE_CMD_DO_SET_ACTUATOR) {
				ପୂର୍ଣ୍ଣସଂଖ୍ୟା index = static_cast<ପୂର୍ଣ୍ଣସଂଖ୍ୟା>(lroundf(vehicle_command.param7));

				ଯଦି (index == 0) {
					ଯଦି (PX4_ISFINITE(vehicle_command.param1)) { _data[0] = vehicle_command.param1; }

					ଯଦି (PX4_ISFINITE(vehicle_command.param2)) {_data[1] = vehicle_command.param2; }

					ଯଦି (PX4_ISFINITE(vehicle_command.param3)) {_data[2] = vehicle_command.param3; }

					ଯଦି (PX4_ISFINITE(vehicle_command.param4)) {_data[3] = vehicle_command.param4; }

					ଯଦି (PX4_ISFINITE(vehicle_command.param5)) {_data[4] = vehicle_command.param5; }

					ଯଦି (PX4_ISFINITE(vehicle_command.param6)) {_data[5] = vehicle_command.param6; }
				}
			}
		}
	}

	ଦଶମିକ value(OutputFunction func) override { ଫେରନ୍ତୁ _data[(ପୂର୍ଣ୍ଣସଂଖ୍ୟା)func - (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)OutputFunction::Peripheral_via_Actuator_Set1]; }

ବ୍ୟକ୍ତିଗତ:
	ସ୍ଥିର constexpr ପୂର୍ଣ୍ଣସଂଖ୍ୟା max_ଅଙ୍କ_actuators = 6;

	uORB::Subscription _topic{ORB_ID(vehicle_command)};
	ଦଶମିକ _data[max_ଅଙ୍କ_actuators] {};
};

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

#include "FunctionProviderBase.hpp"

#include <math.h>
#include <uORB/topics/vehicle_command.h>

/**
 * Functions: Peripheral_via_Actuator_Set1 ... Peripheral_via_Actuator_Set6
 */
class FunctionActuatorSet : public FunctionProviderBase
{
public:
	FunctionActuatorSet()
	{
		สำหรับ (จำนวนเต็ม i = 0; i < max_num_actuators; ++i) {
			_data[i] = NAN;
		}
	}

	static FunctionProviderBase *allocate(const Context &context) { คืน new FunctionActuatorSet(); }

	void update() override
	{
		vehicle_command_s vehicle_command;

		ขณะ (_topic.update(&vehicle_command)) {
			ถ้า (vehicle_command.command == vehicle_command_s::VEHICLE_CMD_DO_SET_ACTUATOR) {
				จำนวนเต็ม index = static_cast<จำนวนเต็ม>(lroundf(vehicle_command.param7));

				ถ้า (index == 0) {
					ถ้า (PX4_ISFINITE(vehicle_command.param1)) { _data[0] = vehicle_command.param1; }

					ถ้า (PX4_ISFINITE(vehicle_command.param2)) {_data[1] = vehicle_command.param2; }

					ถ้า (PX4_ISFINITE(vehicle_command.param3)) {_data[2] = vehicle_command.param3; }

					ถ้า (PX4_ISFINITE(vehicle_command.param4)) {_data[3] = vehicle_command.param4; }

					ถ้า (PX4_ISFINITE(vehicle_command.param5)) {_data[4] = vehicle_command.param5; }

					ถ้า (PX4_ISFINITE(vehicle_command.param6)) {_data[5] = vehicle_command.param6; }
				}
			}
		}
	}

	float value(OutputFunction func) override { คืน _data[(จำนวนเต็ม)func - (จำนวนเต็ม)OutputFunction::Peripheral_via_Actuator_Set1]; }

private:
	static constexpr จำนวนเต็ม max_num_actuators = 6;

	uORB::Subscription _topic{ORB_ID(vehicle_command)};
	float _data[max_num_actuators] {};
};

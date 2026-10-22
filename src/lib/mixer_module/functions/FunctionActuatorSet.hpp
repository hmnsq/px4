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

#ਹਦਾਇਤ once

#ਅਨ੍ਤਰ੍ਭਾਵ "FunctionProviderBase.hpp"

#ਅਨ੍ਤਰ੍ਭਾਵ <math.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <uORB/topics/vehicle_command.h>

/**
 * Functions: Peripheral_via_Actuator_Set1 ... Peripheral_via_Actuator_Set6
 */
ਵਰਗ Function_Actuator_ਸੈੱਟ : ਜਨਤਕ Function_Provider_ਅਧਾਰ
{
ਜਨਤਕ:
	Function_Actuator_ਸੈੱਟ()
	{
		ਲਈ (ਪੂਰਨ i = 0; i < max_ਅੰਕ_actuators; ++i) {
			ਅੰਕੜੇ_3[i] = NAN;
		}
	}

	ਸਥਿਰ Function_Provider_ਅਧਾਰ *allocate(ਅਚਲ Context &context) { ਵਾਪਸ ਨਵਾਂ Function_Actuator_ਸੈੱਟ(); }

	ਖਾਲੀ update() override
	{
		vehicle_command_s vehicle_command;

		ਜਦੋਂ (_topic.update(&vehicle_command)) {
			ਜੇ (vehicle_command.command == vehicle_command_s::VEHICLE_CMD_DO_ਸੈੱਟ_ACTUATOR) {
				ਪੂਰਨ ਸੂਚਕਾਂਕ = static_cast<ਪੂਰਨ>(lroundf(vehicle_command.param7));

				ਜੇ (ਸੂਚਕਾਂਕ == 0) {
					ਜੇ (PX4_ISFINITE(vehicle_command.param1)) { ਅੰਕੜੇ_3[0] = vehicle_command.param1; }

					ਜੇ (PX4_ISFINITE(vehicle_command.param2)) {ਅੰਕੜੇ_3[1] = vehicle_command.param2; }

					ਜੇ (PX4_ISFINITE(vehicle_command.param3)) {ਅੰਕੜੇ_3[2] = vehicle_command.param3; }

					ਜੇ (PX4_ISFINITE(vehicle_command.param4)) {ਅੰਕੜੇ_3[3] = vehicle_command.param4; }

					ਜੇ (PX4_ISFINITE(vehicle_command.param5)) {ਅੰਕੜੇ_3[4] = vehicle_command.param5; }

					ਜੇ (PX4_ISFINITE(vehicle_command.param6)) {ਅੰਕੜੇ_3[5] = vehicle_command.param6; }
				}
			}
		}
	}

	ਦਸ਼ਮਲਵ value(OutputFunction func) override { ਵਾਪਸ ਅੰਕੜੇ_3[(ਪੂਰਨ)func - (ਪੂਰਨ)OutputFunction::Peripheral_ਰਾਹੀਂ_Actuator_Set1]; }

ਨਿਜੀ:
	ਸਥਿਰ constexpr ਪੂਰਨ max_ਅੰਕ_actuators = 6;

	uORB::Subscription _topic{ORB_ID(vehicle_command)};
	ਦਸ਼ਮਲਵ ਅੰਕੜੇ_3[max_ਅੰਕ_actuators] {};
};

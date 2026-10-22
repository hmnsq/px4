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

#ضمن <uORB/topics/manual_control_setpoint.h>

/**
 * Functions: RC_Roll .. RCAUX_Max
 */
صنف FunctionManualRC : عام FunctionProviderBase
{
عام:
	FunctionManualRC()
	{
		reset_All_To_Disarmed_قىممەت();
	}

	ساكن FunctionProviderBase *allocate(ثابت Context &context) { أرجع جديد FunctionManualRC(); }

	فراغ update() override
	{
		manual_control_setpoint_s manual_control_setpoint;

		إذا (_topic.update(&manual_control_setpoint)) {
			إذا (manual_control_setpoint.valid) {
				_data[0] = manual_control_setpoint.roll;
				_data[1] = manual_control_setpoint.pitch;
				_data[2] = manual_control_setpoint.throttle;
				_data[3] = manual_control_setpoint.yaw;
				_data[4] = manual_control_setpoint.flaps;
				_data[5] = manual_control_setpoint.aux1;
				_data[6] = manual_control_setpoint.aux2;
				_data[7] = manual_control_setpoint.aux3;
				_data[8] = manual_control_setpoint.aux4;
				_data[9] = manual_control_setpoint.aux5;
				_data[10] = manual_control_setpoint.aux6;

			} وإلا {
				reset_All_To_Disarmed_قىممەت();
			}
		}
	}

	عائم value(OutputFunction func) override { أرجع _data[(صحيح)func - (صحيح)OutputFunction::RC_Roll]; }

خاص:
	ساكن constexpr صحيح رەقەم_data_points = 11;

	فراغ reset_All_To_Disarmed_قىممەت()
	{
		لكل (صحيح i = 0; i < رەقەم_data_points; ++i) {
			_data[i] = NAN;
		}
	}

	static_assert(رەقەم_data_points == (صحيح)OutputFunction::RC_AUXMax - (صحيح)OutputFunction::RC_Roll + 1,
		      "number of functions mismatch");

	uORB::Subscription _topic{ORB_ID(manual_control_setpoint)};
	عائم _data[رەقەم_data_points] {};
};

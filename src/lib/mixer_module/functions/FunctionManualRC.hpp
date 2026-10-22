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

#ہدایت once

#ضمن "FunctionProviderBase.hpp"

#ضمن <uORB/topics/manual_control_setpoint.h>

/**
 * Functions: RC_Roll .. RCAUX_Max
 */
صنف FunctionManualRC : عام Function_Provider_ادھار
{
عام:
	FunctionManualRC()
	{
		reset_All_منزل_Disarmed_قدر();
	}

	ساكن Function_Provider_ادھار *allocate(ثابت Context &context) { أرجع جديد FunctionManualRC(); }

	فراغ update() override
	{
		manual_control_setpoint_s manual_control_setpoint;

		إذا (_topic.update(&manual_control_setpoint)) {
			إذا (manual_control_setpoint.valid) {
				انکڑے_3[0] = manual_control_setpoint.roll;
				انکڑے_3[1] = manual_control_setpoint.pitch;
				انکڑے_3[2] = manual_control_setpoint.throttle;
				انکڑے_3[3] = manual_control_setpoint.yaw;
				انکڑے_3[4] = manual_control_setpoint.flaps;
				انکڑے_3[5] = manual_control_setpoint.aux1;
				انکڑے_3[6] = manual_control_setpoint.aux2;
				انکڑے_3[7] = manual_control_setpoint.aux3;
				انکڑے_3[8] = manual_control_setpoint.aux4;
				انکڑے_3[9] = manual_control_setpoint.aux5;
				انکڑے_3[10] = manual_control_setpoint.aux6;

			} وإلا {
				reset_All_منزل_Disarmed_قدر();
			}
		}
	}

	عائم value(OutputFunction func) override { أرجع انکڑے_3[(صحيح)func - (صحيح)OutputFunction::RC_Roll]; }

خاص:
	ساكن constexpr صحيح عدد_انکڑے_points = 11;

	فراغ reset_All_منزل_Disarmed_قدر()
	{
		لكل (صحيح i = 0; i < عدد_انکڑے_points; ++i) {
			انکڑے_3[i] = NAN;
		}
	}

	static_assert(عدد_انکڑے_points == (صحيح)OutputFunction::RC_AUXMax - (صحيح)OutputFunction::RC_Roll + 1,
		      "number of functions mismatch");

	uORB::Subscription _topic{ORB_ID(manual_control_setpoint)};
	عائم انکڑے_3[عدد_انکڑے_points] {};
};

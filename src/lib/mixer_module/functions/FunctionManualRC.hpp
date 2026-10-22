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

#شامل "FunctionProviderBase.hpp"

#شامل <uORB/topics/manual_control_setpoint.h>

/**
 * Functions: RC_Roll .. RCAUX_Max
 */
جماعت FunctionManualRC : عوامی Function_Provider_بنیاد
{
عوامی:
	FunctionManualRC()
	{
		reset_All_مقصد_Disarmed_قدر();
	}

	ساکن Function_Provider_بنیاد *allocate(ثابت Context &context) { واپس نیا FunctionManualRC(); }

	خالی update() override
	{
		manual_control_setpoint_s manual_control_setpoint;

		اگر (_topic.update(&manual_control_setpoint)) {
			اگر (manual_control_setpoint.valid) {
				ڈیٹا_3[0] = manual_control_setpoint.roll;
				ڈیٹا_3[1] = manual_control_setpoint.pitch;
				ڈیٹا_3[2] = manual_control_setpoint.throttle;
				ڈیٹا_3[3] = manual_control_setpoint.yaw;
				ڈیٹا_3[4] = manual_control_setpoint.flaps;
				ڈیٹا_3[5] = manual_control_setpoint.aux1;
				ڈیٹا_3[6] = manual_control_setpoint.aux2;
				ڈیٹا_3[7] = manual_control_setpoint.aux3;
				ڈیٹا_3[8] = manual_control_setpoint.aux4;
				ڈیٹا_3[9] = manual_control_setpoint.aux5;
				ڈیٹا_3[10] = manual_control_setpoint.aux6;

			} ورنہ {
				reset_All_مقصد_Disarmed_قدر();
			}
		}
	}

	اعشاری value(OutputFunction func) override { واپس ڈیٹا_3[(صحیح)func - (صحیح)OutputFunction::RC_Roll]; }

نجی:
	ساکن constexpr صحیح عدد_ڈیٹا_points = 11;

	خالی reset_All_مقصد_Disarmed_قدر()
	{
		برائے (صحیح i = 0; i < عدد_ڈیٹا_points; ++i) {
			ڈیٹا_3[i] = NAN;
		}
	}

	static_assert(عدد_ڈیٹا_points == (صحیح)OutputFunction::RC_AUXMax - (صحیح)OutputFunction::RC_Roll + 1,
		      "number of functions mismatch");

	uORB::Subscription _topic{ORB_ID(manual_control_setpoint)};
	اعشاری ڈیٹا_3[عدد_ڈیٹا_points] {};
};

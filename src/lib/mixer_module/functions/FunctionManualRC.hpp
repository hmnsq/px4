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

#ਅਨ੍ਤਰ੍ਭਾਵ <uORB/topics/manual_control_setpoint.h>

/**
 * Functions: RC_Roll .. RCAUX_Max
 */
ਵਰਗ FunctionManualRC : ਜਨਤਕ Function_Provider_ਅਧਾਰ
{
ਜਨਤਕ:
	FunctionManualRC()
	{
		reset_All_ਮੰਜ਼ਿਲ_Disarmed_ਮੁੱਲ();
	}

	ਸਥਿਰ Function_Provider_ਅਧਾਰ *allocate(ਅਚਲ Context &context) { ਵਾਪਸ ਨਵਾਂ FunctionManualRC(); }

	ਖਾਲੀ update() override
	{
		manual_control_setpoint_s manual_control_setpoint;

		ਜੇ (_topic.update(&manual_control_setpoint)) {
			ਜੇ (manual_control_setpoint.valid) {
				ਅੰਕੜੇ_3[0] = manual_control_setpoint.roll;
				ਅੰਕੜੇ_3[1] = manual_control_setpoint.pitch;
				ਅੰਕੜੇ_3[2] = manual_control_setpoint.throttle;
				ਅੰਕੜੇ_3[3] = manual_control_setpoint.yaw;
				ਅੰਕੜੇ_3[4] = manual_control_setpoint.flaps;
				ਅੰਕੜੇ_3[5] = manual_control_setpoint.aux1;
				ਅੰਕੜੇ_3[6] = manual_control_setpoint.aux2;
				ਅੰਕੜੇ_3[7] = manual_control_setpoint.aux3;
				ਅੰਕੜੇ_3[8] = manual_control_setpoint.aux4;
				ਅੰਕੜੇ_3[9] = manual_control_setpoint.aux5;
				ਅੰਕੜੇ_3[10] = manual_control_setpoint.aux6;

			} ਨਹੀਂਤਾਂ {
				reset_All_ਮੰਜ਼ਿਲ_Disarmed_ਮੁੱਲ();
			}
		}
	}

	ਦਸ਼ਮਲਵ value(OutputFunction func) override { ਵਾਪਸ ਅੰਕੜੇ_3[(ਪੂਰਨ)func - (ਪੂਰਨ)OutputFunction::RC_Roll]; }

ਨਿਜੀ:
	ਸਥਿਰ constexpr ਪੂਰਨ ਅੰਕ_ਅੰਕੜੇ_points = 11;

	ਖਾਲੀ reset_All_ਮੰਜ਼ਿਲ_Disarmed_ਮੁੱਲ()
	{
		ਲਈ (ਪੂਰਨ i = 0; i < ਅੰਕ_ਅੰਕੜੇ_points; ++i) {
			ਅੰਕੜੇ_3[i] = NAN;
		}
	}

	static_assert(ਅੰਕ_ਅੰਕੜੇ_points == (ਪੂਰਨ)OutputFunction::RC_AUXMax - (ਪੂਰਨ)OutputFunction::RC_Roll + 1,
		      "number of functions mismatch");

	uORB::Subscription _topic{ORB_ID(manual_control_setpoint)};
	ਦਸ਼ਮਲਵ ਅੰਕੜੇ_3[ਅੰਕ_ਅੰਕੜੇ_points] {};
};

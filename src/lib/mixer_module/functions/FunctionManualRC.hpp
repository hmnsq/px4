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

#निर्देशः once

#अन्तर्भाव "FunctionProviderBase.hpp"

#अन्तर्भाव <uORB/topics/manual_control_setpoint.h>

/**
 * Functions: RC_Roll .. RCAUX_Max
 */
वर्ग FunctionManualRC : सार्वजनिक Function_Provider_आधारः
{
सार्वजनिक:
	FunctionManualRC()
	{
		reset_All_गन्तव्यम्_Disarmed_मानम्();
	}

	स्थिर Function_Provider_आधारः *allocate(ध्रुव Context &context) { प्रत्यागम नूतन FunctionManualRC(); }

	शून्य update() override
	{
		manual_control_setpoint_s manual_control_setpoint;

		यदि (_topic.update(&manual_control_setpoint)) {
			यदि (manual_control_setpoint.valid) {
				दत्त_3[0] = manual_control_setpoint.roll;
				दत्त_3[1] = manual_control_setpoint.pitch;
				दत्त_3[2] = manual_control_setpoint.throttle;
				दत्त_3[3] = manual_control_setpoint.yaw;
				दत्त_3[4] = manual_control_setpoint.flaps;
				दत्त_3[5] = manual_control_setpoint.aux1;
				दत्त_3[6] = manual_control_setpoint.aux2;
				दत्त_3[7] = manual_control_setpoint.aux3;
				दत्त_3[8] = manual_control_setpoint.aux4;
				दत्त_3[9] = manual_control_setpoint.aux5;
				दत्त_3[10] = manual_control_setpoint.aux6;

			} अन्यथा {
				reset_All_गन्तव्यम्_Disarmed_मानम्();
			}
		}
	}

	भिन्न value(OutputFunction func) override { प्रत्यागम दत्त_3[(पूर्णाङ्क)func - (पूर्णाङ्क)OutputFunction::RC_Roll]; }

गुप्त:
	स्थिर constexpr पूर्णाङ्क अङ्कः_दत्त_points = 11;

	शून्य reset_All_गन्तव्यम्_Disarmed_मानम्()
	{
		प्रति (पूर्णाङ्क i = 0; i < अङ्कः_दत्त_points; ++i) {
			दत्त_3[i] = NAN;
		}
	}

	static_assert(अङ्कः_दत्त_points == (पूर्णाङ्क)OutputFunction::RC_AUXMax - (पूर्णाङ्क)OutputFunction::RC_Roll + 1,
		      "number of functions mismatch");

	uORB::Subscription _topic{ORB_ID(manual_control_setpoint)};
	भिन्न दत्त_3[अङ्कः_दत्त_points] {};
};

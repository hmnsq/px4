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

#సూచన once

#అన్తర్భావ "FunctionProviderBase.hpp"

#అన్తర్భావ <uORB/topics/manual_control_setpoint.h>

/**
 * Functions: RC_Roll .. RCAUX_Max
 */
తరగతి FunctionManualRC : బహిరంగ Function_Provider_పునాది
{
బహిరంగ:
	FunctionManualRC()
	{
		reset_All_గమ్యం_Disarmed_విలువ();
	}

	స్థిర Function_Provider_పునాది *allocate(స్థిరరాశి Context &context) { తిరిగి కొత్త FunctionManualRC(); }

	శూన్య update() override
	{
		manual_control_setpoint_s manual_control_setpoint;

		ఒకవేళ (_topic.update(&manual_control_setpoint)) {
			ఒకవేళ (manual_control_setpoint.valid) {
				దత్తాంశం_3[0] = manual_control_setpoint.roll;
				దత్తాంశం_3[1] = manual_control_setpoint.pitch;
				దత్తాంశం_3[2] = manual_control_setpoint.throttle;
				దత్తాంశం_3[3] = manual_control_setpoint.yaw;
				దత్తాంశం_3[4] = manual_control_setpoint.flaps;
				దత్తాంశం_3[5] = manual_control_setpoint.aux1;
				దత్తాంశం_3[6] = manual_control_setpoint.aux2;
				దత్తాంశం_3[7] = manual_control_setpoint.aux3;
				దత్తాంశం_3[8] = manual_control_setpoint.aux4;
				దత్తాంశం_3[9] = manual_control_setpoint.aux5;
				దత్తాంశం_3[10] = manual_control_setpoint.aux6;

			} లేకపోతే {
				reset_All_గమ్యం_Disarmed_విలువ();
			}
		}
	}

	దశాంశం value(OutputFunction func) override { తిరిగి దత్తాంశం_3[(పూర్ణాంకం)func - (పూర్ణాంకం)OutputFunction::RC_Roll]; }

అంతరంగ:
	స్థిర constexpr పూర్ణాంకం అంకె_దత్తాంశం_points = 11;

	శూన్య reset_All_గమ్యం_Disarmed_విలువ()
	{
		కొరకు (పూర్ణాంకం i = 0; i < అంకె_దత్తాంశం_points; ++i) {
			దత్తాంశం_3[i] = NAN;
		}
	}

	static_assert(అంకె_దత్తాంశం_points == (పూర్ణాంకం)OutputFunction::RC_AUXMax - (పూర్ణాంకం)OutputFunction::RC_Roll + 1,
		      "number of functions mismatch");

	uORB::Subscription _topic{ORB_ID(manual_control_setpoint)};
	దశాంశం దత్తాంశం_3[అంకె_దత్తాంశం_points] {};
};

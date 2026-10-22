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

#అన్తర్భావ <uORB/topics/gimbal_controls.h>

/**
 * Functions: Gimbal_Roll .. Gimbal_Yaw
 */
తరగతి FunctionGimbal : బహిరంగ Function_Provider_పునాది
{
బహిరంగ:
	FunctionGimbal() = అప్రమేయ;
	స్థిర Function_Provider_పునాది *allocate(స్థిరరాశి Context &context) { తిరిగి కొత్త FunctionGimbal(); }

	శూన్య update() override
	{
		gimbal_controls_s gimbal_controls;

		ఒకవేళ (_topic.update(&gimbal_controls)) {
			దత్తాంశం_3[0] = gimbal_controls.control[gimbal_controls_s::సూచీ_ROLL];
			దత్తాంశం_3[1] = gimbal_controls.control[gimbal_controls_s::సూచీ_PITCH];
			దత్తాంశం_3[2] = gimbal_controls.control[gimbal_controls_s::సూచీ_YAW];
		}
	}

	దశాంశం value(OutputFunction func) override { తిరిగి దత్తాంశం_3[(పూర్ణాంకం)func - (పూర్ణాంకం)OutputFunction::Gimbal_Roll]; }

అంతరంగ:
	uORB::Subscription _topic{ORB_ID(gimbal_controls)};
	దశాంశం దత్తాంశం_3[3] { NAN, NAN, NAN };
};

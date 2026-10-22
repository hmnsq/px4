/****************************************************************************
 *
 *   Copyright (c) 2025 PX4 Development Team. All rights reserved.
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

#అన్తర్భావ <uORB/topics/internal_combustion_engine_control.h>

/**
 * Functions: ICE...
 */
తరగతి FunctionICEControl : బహిరంగ Function_Provider_పునాది
{
బహిరంగ:
	FunctionICEControl()
	{
		reset_All_గమ్యం_Disarmed_విలువ();
	}

	స్థిర Function_Provider_పునాది *allocate(స్థిరరాశి Context &context) { తిరిగి కొత్త FunctionICEControl(); }

	శూన్య update() override
	{
		అంతర్గత_combustion_engine_control_s అంతర్గత_combustion_engine_control;

		// map [0, 1] to [-1, 1] which is the interface for non-motor PWM channels
		// NAN is mapped to disarmed
		ఒకవేళ (అంతర్గత_combustion_engine_control_sub.update(&అంతర్గత_combustion_engine_control)) {
			దత్తాంశం_3[0] = అంతర్గత_combustion_engine_control.ignition_on * 2.f - 1.f;
			దత్తాంశం_3[1] = అంతర్గత_combustion_engine_control.throttle_control * 2.f - 1.f;
			దత్తాంశం_3[2] = అంతర్గత_combustion_engine_control.choke_control * 2.f - 1.f;
			దత్తాంశం_3[3] = అంతర్గత_combustion_engine_control.starter_engine_control * 2.f - 1.f;
		}
	}

	దశాంశం value(OutputFunction func) override { తిరిగి దత్తాంశం_3[(పూర్ణాంకం)func - (పూర్ణాంకం)OutputFunction::IC_Engine_Ignition]; }

అంతరంగ:
	స్థిర constexpr పూర్ణాంకం అంకె_దత్తాంశం_points = 4;

	శూన్య reset_All_గమ్యం_Disarmed_విలువ()
	{
		కొరకు (పూర్ణాంకం i = 0; i < అంకె_దత్తాంశం_points; ++i) {
			దత్తాంశం_3[i] = NAN;
		}
	}

	static_assert(అంకె_దత్తాంశం_points == (పూర్ణాంకం)OutputFunction::IC_Engine_Starter - (పూర్ణాంకం)OutputFunction::IC_Engine_Ignition + 1,
		      "number of functions mismatch");

	uORB::Subscription అంతర్గత_combustion_engine_control_sub{ORB_ID(అంతర్గత_combustion_engine_control)};
	దశాంశం దత్తాంశం_3[అంకె_దత్తాంశం_points] {};
};

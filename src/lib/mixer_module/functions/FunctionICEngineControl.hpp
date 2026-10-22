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

#निर्देशः once

#अन्तर्भाव "FunctionProviderBase.hpp"

#अन्तर्भाव <uORB/topics/internal_combustion_engine_control.h>

/**
 * Functions: ICE...
 */
वर्ग FunctionICEControl : सार्वजनिक Function_Provider_आधारः
{
सार्वजनिक:
	FunctionICEControl()
	{
		reset_All_गन्तव्यम्_Disarmed_मानम्();
	}

	स्थिर Function_Provider_आधारः *allocate(ध्रुव Context &context) { प्रत्यागम नूतन FunctionICEControl(); }

	शून्य update() override
	{
		आन्तरिकम्_combustion_engine_control_s आन्तरिकम्_combustion_engine_control;

		// map [0, 1] to [-1, 1] which is the interface for non-motor PWM channels
		// NAN is mapped to disarmed
		यदि (आन्तरिकम्_combustion_engine_control_sub.update(&आन्तरिकम्_combustion_engine_control)) {
			दत्त_3[0] = आन्तरिकम्_combustion_engine_control.ignition_on * 2.f - 1.f;
			दत्त_3[1] = आन्तरिकम्_combustion_engine_control.throttle_control * 2.f - 1.f;
			दत्त_3[2] = आन्तरिकम्_combustion_engine_control.choke_control * 2.f - 1.f;
			दत्त_3[3] = आन्तरिकम्_combustion_engine_control.starter_engine_control * 2.f - 1.f;
		}
	}

	भिन्न value(OutputFunction func) override { प्रत्यागम दत्त_3[(पूर्णाङ्क)func - (पूर्णाङ्क)OutputFunction::IC_Engine_Ignition]; }

गुप्त:
	स्थिर constexpr पूर्णाङ्क अङ्कः_दत्त_points = 4;

	शून्य reset_All_गन्तव्यम्_Disarmed_मानम्()
	{
		प्रति (पूर्णाङ्क i = 0; i < अङ्कः_दत्त_points; ++i) {
			दत्त_3[i] = NAN;
		}
	}

	static_assert(अङ्कः_दत्त_points == (पूर्णाङ्क)OutputFunction::IC_Engine_Starter - (पूर्णाङ्क)OutputFunction::IC_Engine_Ignition + 1,
		      "number of functions mismatch");

	uORB::Subscription आन्तरिकम्_combustion_engine_control_sub{ORB_ID(आन्तरिकम्_combustion_engine_control)};
	भिन्न दत्त_3[अङ्कः_दत्त_points] {};
};

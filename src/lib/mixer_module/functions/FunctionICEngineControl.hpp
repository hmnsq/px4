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

#ہدایت once

#ضمن "FunctionProviderBase.hpp"

#ضمن <uORB/topics/internal_combustion_engine_control.h>

/**
 * Functions: ICE...
 */
صنف FunctionICEControl : عام Function_Provider_ادھار
{
عام:
	FunctionICEControl()
	{
		reset_All_منزل_Disarmed_قدر();
	}

	ساكن Function_Provider_ادھار *allocate(ثابت Context &context) { أرجع جديد FunctionICEControl(); }

	فراغ update() override
	{
		اندرونی_combustion_engine_control_s اندرونی_combustion_engine_control;

		// map [0, 1] to [-1, 1] which is the interface for non-motor PWM channels
		// NAN is mapped to disarmed
		إذا (اندرونی_combustion_engine_control_sub.update(&اندرونی_combustion_engine_control)) {
			انکڑے_3[0] = اندرونی_combustion_engine_control.ignition_on * 2.f - 1.f;
			انکڑے_3[1] = اندرونی_combustion_engine_control.throttle_control * 2.f - 1.f;
			انکڑے_3[2] = اندرونی_combustion_engine_control.choke_control * 2.f - 1.f;
			انکڑے_3[3] = اندرونی_combustion_engine_control.starter_engine_control * 2.f - 1.f;
		}
	}

	عائم value(OutputFunction func) override { أرجع انکڑے_3[(صحيح)func - (صحيح)OutputFunction::IC_Engine_Ignition]; }

خاص:
	ساكن constexpr صحيح عدد_انکڑے_points = 4;

	فراغ reset_All_منزل_Disarmed_قدر()
	{
		لكل (صحيح i = 0; i < عدد_انکڑے_points; ++i) {
			انکڑے_3[i] = NAN;
		}
	}

	static_assert(عدد_انکڑے_points == (صحيح)OutputFunction::IC_Engine_Starter - (صحيح)OutputFunction::IC_Engine_Ignition + 1,
		      "number of functions mismatch");

	uORB::Subscription اندرونی_combustion_engine_control_sub{ORB_ID(اندرونی_combustion_engine_control)};
	عائم انکڑے_3[عدد_انکڑے_points] {};
};

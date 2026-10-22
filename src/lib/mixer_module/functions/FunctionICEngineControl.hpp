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

#شامل "FunctionProviderBase.hpp"

#شامل <uORB/topics/internal_combustion_engine_control.h>

/**
 * Functions: ICE...
 */
جماعت FunctionICEControl : عوامی Function_Provider_بنیاد
{
عوامی:
	FunctionICEControl()
	{
		reset_All_مقصد_Disarmed_قدر();
	}

	ساکن Function_Provider_بنیاد *allocate(ثابت Context &context) { واپس نیا FunctionICEControl(); }

	خالی update() override
	{
		اندرونی_combustion_engine_control_s اندرونی_combustion_engine_control;

		// map [0, 1] to [-1, 1] which is the interface for non-motor PWM channels
		// NAN is mapped to disarmed
		اگر (اندرونی_combustion_engine_control_sub.update(&اندرونی_combustion_engine_control)) {
			ڈیٹا_3[0] = اندرونی_combustion_engine_control.ignition_on * 2.f - 1.f;
			ڈیٹا_3[1] = اندرونی_combustion_engine_control.throttle_control * 2.f - 1.f;
			ڈیٹا_3[2] = اندرونی_combustion_engine_control.choke_control * 2.f - 1.f;
			ڈیٹا_3[3] = اندرونی_combustion_engine_control.starter_engine_control * 2.f - 1.f;
		}
	}

	اعشاری value(OutputFunction func) override { واپس ڈیٹا_3[(صحیح)func - (صحیح)OutputFunction::IC_Engine_Ignition]; }

نجی:
	ساکن constexpr صحیح عدد_ڈیٹا_points = 4;

	خالی reset_All_مقصد_Disarmed_قدر()
	{
		برائے (صحیح i = 0; i < عدد_ڈیٹا_points; ++i) {
			ڈیٹا_3[i] = NAN;
		}
	}

	static_assert(عدد_ڈیٹا_points == (صحیح)OutputFunction::IC_Engine_Starter - (صحیح)OutputFunction::IC_Engine_Ignition + 1,
		      "number of functions mismatch");

	uORB::Subscription اندرونی_combustion_engine_control_sub{ORB_ID(اندرونی_combustion_engine_control)};
	اعشاری ڈیٹا_3[عدد_ڈیٹا_points] {};
};

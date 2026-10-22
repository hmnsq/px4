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

#pragma once

#অন্তর্ভাব "FunctionProviderBase.hpp"

#অন্তর্ভাব <uORB/topics/internal_combustion_engine_control.h>

/**
 * Functions: ICE...
 */
শ্রেণী FunctionICEControl : সর্বজনীন FunctionProviderBase
{
সর্বজনীন:
	FunctionICEControl()
	{
		reset_All_To_Disarmed_মান();
	}

	স্থির FunctionProviderBase *allocate(ধ্রুবক Context &context) { ফেরত নতুন FunctionICEControl(); }

	শূন্য update() override
	{
		internal_combustion_engine_control_s internal_combustion_engine_control;

		// map [0, 1] to [-1, 1] which is the interface for non-motor PWM channels
		// NAN is mapped to disarmed
		যদি (_internal_combustion_engine_control_sub.update(&internal_combustion_engine_control)) {
			তথ্য_3[0] = internal_combustion_engine_control.ignition_on * 2.f - 1.f;
			তথ্য_3[1] = internal_combustion_engine_control.throttle_control * 2.f - 1.f;
			তথ্য_3[2] = internal_combustion_engine_control.choke_control * 2.f - 1.f;
			তথ্য_3[3] = internal_combustion_engine_control.starter_engine_control * 2.f - 1.f;
		}
	}

	দশমিক value(OutputFunction func) override { ফেরত তথ্য_3[(পূর্ণসংখ্যা)func - (পূর্ণসংখ্যা)OutputFunction::IC_Engine_Ignition]; }

ব্যক্তিগত:
	স্থির constexpr পূর্ণসংখ্যা অঙ্ক_তথ্য_points = 4;

	শূন্য reset_All_To_Disarmed_মান()
	{
		জন্য (পূর্ণসংখ্যা i = 0; i < অঙ্ক_তথ্য_points; ++i) {
			তথ্য_3[i] = NAN;
		}
	}

	static_assert(অঙ্ক_তথ্য_points == (পূর্ণসংখ্যা)OutputFunction::IC_Engine_Starter - (পূর্ণসংখ্যা)OutputFunction::IC_Engine_Ignition + 1,
		      "number of functions mismatch");

	uORB::Subscription _internal_combustion_engine_control_sub{ORB_ID(internal_combustion_engine_control)};
	দশমিক তথ্য_3[অঙ্ক_তথ্য_points] {};
};

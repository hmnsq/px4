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

#অন্তর্ভাৱ "FunctionProviderBase.hpp"

#অন্তর্ভাৱ <uORB/topics/internal_combustion_engine_control.h>

/**
 * Functions: ICE...
 */
শ্ৰেণী FunctionICEControl : সার্ৱজনিক FunctionProviderBase
{
সার্ৱজনিক:
	FunctionICEControl()
	{
		reset_All_To_Disarmed_মান();
	}

	স্থির FunctionProviderBase *allocate(ধ্রুৱ Context &context) { ঘূৰাই নূতন FunctionICEControl(); }

	শূন্য update() override
	{
		internal_combustion_engine_control_s internal_combustion_engine_control;

		// map [0, 1] to [-1, 1] which is the interface for non-motor PWM channels
		// NAN is mapped to disarmed
		যদি (_internal_combustion_engine_control_sub.update(&internal_combustion_engine_control)) {
			_data[0] = internal_combustion_engine_control.ignition_on * 2.f - 1.f;
			_data[1] = internal_combustion_engine_control.throttle_control * 2.f - 1.f;
			_data[2] = internal_combustion_engine_control.choke_control * 2.f - 1.f;
			_data[3] = internal_combustion_engine_control.starter_engine_control * 2.f - 1.f;
		}
	}

	ভিন্ন value(OutputFunction func) override { ঘূৰাই _data[(পূর্ণাঙ্ক)func - (পূর্ণাঙ্ক)OutputFunction::IC_Engine_Ignition]; }

গুপ্ত:
	স্থির constexpr পূর্ণাঙ্ক অঙ্ক_data_points = 4;

	শূন্য reset_All_To_Disarmed_মান()
	{
		বাবে (পূর্ণাঙ্ক i = 0; i < অঙ্ক_data_points; ++i) {
			_data[i] = NAN;
		}
	}

	static_assert(অঙ্ক_data_points == (পূর্ণাঙ্ক)OutputFunction::IC_Engine_Starter - (পূর্ণাঙ্ক)OutputFunction::IC_Engine_Ignition + 1,
		      "number of functions mismatch");

	uORB::Subscription _internal_combustion_engine_control_sub{ORB_ID(internal_combustion_engine_control)};
	ভিন্ন _data[অঙ্ক_data_points] {};
};

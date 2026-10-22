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

#அந்தர்பாவ "FunctionProviderBase.hpp"

#அந்தர்பாவ <uORB/topics/internal_combustion_engine_control.h>

/**
 * Functions: ICE...
 */
வகுப்பு FunctionICEControl : பொது FunctionProviderBase
{
பொது:
	FunctionICEControl()
	{
		reset_All_To_Disarmed_மதிப்பு();
	}

	நிலையான FunctionProviderBase *allocate(மாறிலி Context &context) { திரும்பு புதிய FunctionICEControl(); }

	வெறுமை update() override
	{
		internal_combustion_engine_control_s internal_combustion_engine_control;

		// map [0, 1] to [-1, 1] which is the interface for non-motor PWM channels
		// NAN is mapped to disarmed
		எனில் (_internal_combustion_engine_control_sub.update(&internal_combustion_engine_control)) {
			_data[0] = internal_combustion_engine_control.ignition_on * 2.f - 1.f;
			_data[1] = internal_combustion_engine_control.throttle_control * 2.f - 1.f;
			_data[2] = internal_combustion_engine_control.choke_control * 2.f - 1.f;
			_data[3] = internal_combustion_engine_control.starter_engine_control * 2.f - 1.f;
		}
	}

	தசம value(OutputFunction func) override { திரும்பு _data[(முழு)func - (முழு)OutputFunction::IC_Engine_Ignition]; }

தனிப்பட்ட:
	நிலையான constexpr முழு இலக்கம்_data_points = 4;

	வெறுமை reset_All_To_Disarmed_மதிப்பு()
	{
		க்கு (முழு i = 0; i < இலக்கம்_data_points; ++i) {
			_data[i] = NAN;
		}
	}

	static_assert(இலக்கம்_data_points == (முழு)OutputFunction::IC_Engine_Starter - (முழு)OutputFunction::IC_Engine_Ignition + 1,
		      "number of functions mismatch");

	uORB::Subscription _internal_combustion_engine_control_sub{ORB_ID(internal_combustion_engine_control)};
	தசம _data[இலக்கம்_data_points] {};
};

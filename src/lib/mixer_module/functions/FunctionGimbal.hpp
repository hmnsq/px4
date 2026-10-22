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

#pragma once

#অন্তর্ভাব "FunctionProviderBase.hpp"

#অন্তর্ভাব <uORB/topics/gimbal_controls.h>

/**
 * Functions: Gimbal_Roll .. Gimbal_Yaw
 */
বর্গ FunctionGimbal : সার্বজনিক FunctionProviderBase
{
সার্বজনিক:
	FunctionGimbal() = স্বভাব;
	স্থির FunctionProviderBase *allocate(ধ্রুব Context &context) { প্রত্যাগম নূতন FunctionGimbal(); }

	শূন্য update() override
	{
		gimbal_controls_s gimbal_controls;

		যদি (_topic.update(&gimbal_controls)) {
			_data[0] = gimbal_controls.control[gimbal_controls_s::INDEX_ROLL];
			_data[1] = gimbal_controls.control[gimbal_controls_s::INDEX_PITCH];
			_data[2] = gimbal_controls.control[gimbal_controls_s::INDEX_YAW];
		}
	}

	ভিন্ন value(OutputFunction func) override { প্রত্যাগম _data[(পূর্ণাঙ্ক)func - (পূর্ণাঙ্ক)OutputFunction::Gimbal_Roll]; }

গুপ্ত:
	uORB::Subscription _topic{ORB_ID(gimbal_controls)};
	ভিন্ন _data[3] { NAN, NAN, NAN };
};

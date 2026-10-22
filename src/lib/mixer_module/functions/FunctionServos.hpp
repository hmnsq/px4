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

#অন্তর্ভাব <uORB/topics/actuator_servos.h>

/**
 * Functions: Servo1 ... ServoMax
 */
বর্গ FunctionServos : সার্বজনিক FunctionProviderBase
{
সার্বজনিক:
	static_assert(actuator_servos_s::অঙ্ক_CONTROLS == (পূর্ণাঙ্ক)OutputFunction::ServoMax - (পূর্ণাঙ্ক)OutputFunction::Servo1 + 1,
		      "Unexpected num servos");

	FunctionServos(ধ্রুব Context &context) :
		_topic(&context.work_item, ORB_ID(actuator_servos))
	{
		প্রতি (পূর্ণাঙ্ক i = 0; i < actuator_servos_s::অঙ্ক_CONTROLS; ++i) {
			_data.control[i] = NAN;
		}
	}

	স্থির FunctionProviderBase *allocate(ধ্রুব Context &context) { প্রত্যাগম নূতন FunctionServos(context); }

	শূন্য update() override { _topic.update(&_data); }
	ভিন্ন value(OutputFunction func) override { প্রত্যাগম _data.control[(পূর্ণাঙ্ক)func - (পূর্ণাঙ্ক)OutputFunction::Servo1]; }

	uORB::SubscriptionCallbackWorkItem *subscriptionCallback() override { প্রত্যাগম &_topic; }

	ভিন্ন default_Failsafe_মান(OutputFunction func) ধ্রুব override { প্রত্যাগম 0.f; }

গুপ্ত:
	uORB::SubscriptionCallbackWorkItem _topic;
	actuator_servos_s _data{};
};

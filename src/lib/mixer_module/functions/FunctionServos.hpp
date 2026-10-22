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

#ضمن "FunctionProviderBase.hpp"

#ضمن <uORB/topics/actuator_servos.h>

/**
 * Functions: Servo1 ... ServoMax
 */
صنف FunctionServos : عام FunctionProviderBase
{
عام:
	static_assert(actuator_servos_s::رقم_CONTROLS == (صحيح)OutputFunction::ServoMax - (صحيح)OutputFunction::Servo1 + 1,
		      "Unexpected num servos");

	FunctionServos(ثابت Context &context) :
		_topic(&context.work_item, ORB_ID(actuator_servos))
	{
		لكل (صحيح i = 0; i < actuator_servos_s::رقم_CONTROLS; ++i) {
			_data.control[i] = NAN;
		}
	}

	ساكن FunctionProviderBase *allocate(ثابت Context &context) { إرجاع جديد FunctionServos(context); }

	فراغ update() override { _topic.update(&_data); }
	عائم value(OutputFunction func) override { إرجاع _data.control[(صحيح)func - (صحيح)OutputFunction::Servo1]; }

	uORB::SubscriptionCallbackWorkItem *subscriptionCallback() override { إرجاع &_topic; }

	عائم default_Failsafe_قيمة(OutputFunction func) ثابت override { إرجاع 0.f; }

خاص:
	uORB::SubscriptionCallbackWorkItem _topic;
	actuator_servos_s _data{};
};

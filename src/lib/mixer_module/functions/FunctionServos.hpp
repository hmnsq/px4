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

#ଅନ୍ତର୍ଭାଵ "FunctionProviderBase.hpp"

#ଅନ୍ତର୍ଭାଵ <uORB/topics/actuator_servos.h>

/**
 * Functions: Servo1 ... ServoMax
 */
ଶ୍ରେଣୀ FunctionServos : ସର୍ବସାଧାରଣ FunctionProviderBase
{
ସର୍ବସାଧାରଣ:
	static_assert(actuator_servos_s::ଅଙ୍କ_CONTROLS == (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)OutputFunction::ServoMax - (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)OutputFunction::Servo1 + 1,
		      "Unexpected num servos");

	FunctionServos(ଅଚଳ Context &context) :
		_topic(&context.work_item, ORB_ID(actuator_servos))
	{
		ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < actuator_servos_s::ଅଙ୍କ_CONTROLS; ++i) {
			_data.control[i] = NAN;
		}
	}

	ସ୍ଥିର FunctionProviderBase *allocate(ଅଚଳ Context &context) { ଫେରନ୍ତୁ ନୂତନ FunctionServos(context); }

	ଶୂନ୍ୟ update() override { _topic.update(&_data); }
	ଦଶମିକ value(OutputFunction func) override { ଫେରନ୍ତୁ _data.control[(ପୂର୍ଣ୍ଣସଂଖ୍ୟା)func - (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)OutputFunction::Servo1]; }

	uORB::SubscriptionCallbackWorkItem *subscriptionCallback() override { ଫେରନ୍ତୁ &_topic; }

	ଦଶମିକ default_Failsafe_ମୂଲ୍ୟ(OutputFunction func) ଅଚଳ override { ଫେରନ୍ତୁ 0.f; }

ବ୍ୟକ୍ତିଗତ:
	uORB::SubscriptionCallbackWorkItem _topic;
	actuator_servos_s _data{};
};

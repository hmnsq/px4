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

#અન્તર્ભાવ "FunctionProviderBase.hpp"

#અન્તર્ભાવ <uORB/topics/actuator_servos.h>

/**
 * Functions: Servo1 ... ServoMax
 */
વર્ગ FunctionServos : જાહેર FunctionProviderBase
{
જાહેર:
	static_assert(actuator_servos_s::અંક_CONTROLS == (પૂર્ણાંક)OutputFunction::ServoMax - (પૂર્ણાંક)OutputFunction::Servo1 + 1,
		      "Unexpected num servos");

	FunctionServos(અચલ Context &context) :
		_topic(&context.work_item, ORB_ID(actuator_servos))
	{
		માટે (પૂર્ણાંક i = 0; i < actuator_servos_s::અંક_CONTROLS; ++i) {
			_data.control[i] = NAN;
		}
	}

	સ્થિર FunctionProviderBase *allocate(અચલ Context &context) { પાછા નવું FunctionServos(context); }

	શૂન્ય update() override { _topic.update(&_data); }
	દશાંશ value(OutputFunction func) override { પાછા _data.control[(પૂર્ણાંક)func - (પૂર્ણાંક)OutputFunction::Servo1]; }

	uORB::SubscriptionCallbackWorkItem *subscriptionCallback() override { પાછા &_topic; }

	દશાંશ default_Failsafe_કિંમત(OutputFunction func) અચલ override { પાછા 0.f; }

ખાનગી:
	uORB::SubscriptionCallbackWorkItem _topic;
	actuator_servos_s _data{};
};
